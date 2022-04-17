
#include "common.h"

#include "Holder.h"
#include "BspCan.h"
#include "BspUsart.h"
#include "Pid.h"

const uint16_t g_motor_angle_range = 8191;
const uint16_t g_half_motor_angle_range = 4096;
const int16_t g_motor_min_voltage = -30000;
const int16_t g_motor_max_voltage = 30000;
const float  g_pi = 3.1415926;
const float  g_2pi = 6.28318530718;
const float  g_pi_3 = 1.0471975512;
const float  g_pi_6 = 0.52359877559;
const float  g_rpm_to_rps = 1.0 / 3600.0 * 6.28318530718 ; // rad/s

typedef struct Holder {
	PID *mg6020_pid_yaw_angle;
	PID *mg6020_pid_yaw_speed;
	PID *mg6020_pid_pitch_angle;
	PID *mg6020_pid_pitch_speed;

	uint16_t offset_yaw_angle;
	uint16_t offset_pitch_angle;

	float target_yaw_angle;
	float actual_yaw_angle;

	float target_pitch_angle;
	float actual_pitch_angle;

	float target_yaw_speed;
	float actual_yaw_speed;

	float target_pitch_speed;
	float actual_pitch_speed;

	int16_t output_yaw_voltage;
	int16_t output_pitch_voltage;
}Holder;

struct PID g_mg6020_pid_yaw_angle = {
    .Kp = 40.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_yaw_speed = {
    .Kp = 10000.0,
    .Ki = 0.000,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_angle = {
    .Kp = 60.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_speed = {
    .Kp = 20000.0,
    .Ki = 0.0,
    .Kd = 0.0,
};


Holder g_holder = {
	.mg6020_pid_yaw_angle = &g_mg6020_pid_yaw_angle,
	.mg6020_pid_yaw_speed = &g_mg6020_pid_yaw_speed,
	.mg6020_pid_pitch_angle = &g_mg6020_pid_pitch_angle,
	.mg6020_pid_pitch_speed = &g_mg6020_pid_pitch_speed,
};

float abs_f(float x){
	return x<0?-x:x;
}

// rad
float GetActualAngle(uint8_t motor_id, uint16_t offset_angle) {
	uint16_t rotor_angle = (g_motor_info[motor_id].rotor_angle - offset_angle) + g_motor_angle_range;
	return  (rotor_angle * g_pi) / g_motor_angle_range - g_pi;
}

// rad/s
float GetActualSpeed(uint8_t motor_id) {
	return g_motor_info[motor_id].rotor_speed * g_rpm_to_rps;
}

int16_t GetOutPutVoltage(float voltage) {
	if(voltage < g_motor_min_voltage ) return g_motor_min_voltage;
	if(voltage > g_motor_max_voltage ) return g_motor_max_voltage;
	return (int16_t)voltage;
}

float InPutLimitAngle(float input_anage, float min_anage, float max_anage) {
	if (input_anage < min_anage) return min_anage;
	if (input_anage > max_anage) return max_anage;
	return input_anage;
} 

uint8_t g_jetson_nano_tx_buff [100];

void HolderTask(void const * argument) {
	OUTPUT("Holder task start!!!\n");
	CanUserInit();
	vTaskDelay(1000);
	g_holder.target_yaw_angle = 0;
	g_holder.target_pitch_angle = 0;
	g_holder.offset_yaw_angle = 6159;
	g_holder.offset_pitch_angle = 5893;
	while(1) {
		g_holder.mg6020_pid_yaw_angle->target = InPutLimitAngle(g_holder.target_yaw_angle, -g_pi_3, g_pi_3);
		g_holder.mg6020_pid_yaw_angle->feedback = GetActualAngle(YAW_MOTOR, g_holder.offset_yaw_angle);
		g_holder.mg6020_pid_yaw_speed->target = PIDCalc(g_holder.mg6020_pid_yaw_angle);
		g_holder.mg6020_pid_yaw_speed->feedback = GetActualSpeed(YAW_MOTOR);
		g_holder.output_yaw_voltage = GetOutPutVoltage(PIDCalc(g_holder.mg6020_pid_yaw_speed));
		
		g_holder.mg6020_pid_pitch_angle->target = InPutLimitAngle(g_holder.target_pitch_angle, -g_pi_6, g_pi_6);
		g_holder.mg6020_pid_pitch_angle->feedback = GetActualAngle(PITCH_MOTOR, g_holder.offset_pitch_angle);
		g_holder.mg6020_pid_pitch_speed->target = PIDCalc(g_holder.mg6020_pid_pitch_angle);
		g_holder.mg6020_pid_pitch_speed->feedback = GetActualSpeed(PITCH_MOTOR);
		g_holder.output_pitch_voltage = GetOutPutVoltage(PIDCalc(g_holder.mg6020_pid_pitch_speed));

		// OUTPUT("target=%f, feedback=%f, rotor_angle=%d\n", g_holder.mg6020_pid_yaw_angle->target, g_holder.mg6020_pid_yaw_angle->feedback, g_motor_info[YAW_MOTOR].rotor_angle);
		// OUTPUT("OutPutYawVoltage=%d,\n", g_holder.output_yaw_voltage);
		SetMotorVoltageMg6020(g_holder.output_yaw_voltage, g_holder.output_pitch_voltage, 0, 0);
		// OUTPUT("g_motor_info %d %d\n", g_motor_info[5].rotor_angle, g_motor_info[6].rotor_angle);
		// OUTPUT("%d, %d\n", g_remote_control.ch1, g_remote_control.ch2);
		g_jetson_nano_tx_buff[0] = 0x55;
		memcpy((uint8_t*)&g_holder.mg6020_pid_yaw_angle->feedback, g_jetson_nano_tx_buff+1, 4);
		memcpy((uint8_t*)&g_holder.mg6020_pid_pitch_angle->feedback, g_jetson_nano_tx_buff+5, 4);
		HAL_UART_Transmit(&huart6, g_jetson_nano_tx_buff, 9, 0xFFFF);
		
		vTaskDelay(1);
	}
}
