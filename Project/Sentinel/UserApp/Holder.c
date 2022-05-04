
#include "common.h"

#include "Holder.h"
#include "BspCan.h"
#include "BspUsart.h"
#include "Shoot.h"
#include "Pid.h"

static const uint16_t g_motor_angle_range = 8192;
static const uint16_t g_half_motor_angle_range = 4096;
static const int16_t g_motor_min_voltage = -30000;
static const int16_t g_motor_max_voltage = 30000;
static const float  g_pi = 3.1415926;
static const float  g_2pi = 6.28318530718;
static const float  g_pi_3 = 1.0471975512;
static const float  g_pi_6 = 0.52359877559;
static const float  g_rpm_to_rps = 1.0 / 3600.0 * 6.28318530718 ; // rad/s

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
    .Kp = 60.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_yaw_speed = {
    .Kp = 18000.0, // 10000
    .Ki = 0.000,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_angle = {
    .Kp = 60.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_speed = {
    .Kp = 8000.0, //10000
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_track_pid_x = {
    .Kp = 0.0000015,
    .Ki = 0.0,
    .Kd = 0,
};

struct PID g_track_pid_y = {
    .Kp = -0.0000015,
    .Ki = 0.0,
    .Kd = 0,
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
	float rotor_angle = g_motor_info[motor_id].rotor_angle;
	if (offset_angle < g_half_motor_angle_range) {
		uint16_t mirror_offset_angle = g_half_motor_angle_range + offset_angle;
		if (rotor_angle > mirror_offset_angle) rotor_angle = -(g_motor_angle_range - rotor_angle) - offset_angle;
		else rotor_angle = rotor_angle - offset_angle;
	}else {
		uint16_t mirror_offset_angle = offset_angle - g_half_motor_angle_range;
		if (rotor_angle < mirror_offset_angle) rotor_angle = rotor_angle + g_motor_angle_range - offset_angle;
		else rotor_angle = rotor_angle - offset_angle;
	}
	return  (rotor_angle * g_pi) / g_motor_angle_range ;
}

// rad/s
static float GetActualSpeed(uint8_t motor_id) {
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

extern struct Shoot g_shoot;
extern struct SerialPortTx g_jetson_nano_tx;
extern struct SerialPortRx g_jetson_nano_rx;

void HolderTask(void const * argument) {
	OUTPUT("Holder task start!!!\n");
	CanUserInit();
	vTaskDelay(1000);
	g_holder.target_yaw_angle = 0;
	g_holder.target_pitch_angle = 0;
	g_holder.offset_yaw_angle = 8177 ;
	g_holder.offset_pitch_angle = 5790;
	int num = 0;
	while(1) {

		if(g_jetson_nano_rx.shooting){
			g_track_pid_x.target = 0;
			g_track_pid_x.feedback = g_jetson_nano_rx.x_offset;
			g_holder.target_yaw_angle += PIDCalc(&g_track_pid_x);
			g_track_pid_y.target = 0;
			g_track_pid_y.feedback = g_jetson_nano_rx.y_offset;
			g_holder.target_pitch_angle += PIDCalc(&g_track_pid_y);
			// OUTPUT("%f %f\n", g_holder.target_yaw_angle, g_holder.target_pitch_angle);
		}

		g_holder.target_yaw_angle = InPutLimitAngle(g_holder.target_yaw_angle, -0.3, 0.3);
		g_holder.mg6020_pid_yaw_angle->target = g_holder.target_yaw_angle;
		g_holder.mg6020_pid_yaw_angle->feedback = GetActualAngle(YAW_MOTOR, g_holder.offset_yaw_angle);
		g_holder.mg6020_pid_yaw_speed->target = PIDCalc(g_holder.mg6020_pid_yaw_angle);
		g_holder.mg6020_pid_yaw_speed->feedback = GetActualSpeed(YAW_MOTOR);
		g_holder.output_yaw_voltage = GetOutPutVoltage(PIDCalc(g_holder.mg6020_pid_yaw_speed));
		
		g_holder.target_pitch_angle = InPutLimitAngle(g_holder.target_pitch_angle, -0.1, 0.1);
		g_holder.mg6020_pid_pitch_angle->target = g_holder.target_pitch_angle;
		g_holder.mg6020_pid_pitch_angle->feedback = GetActualAngle(PITCH_MOTOR, g_holder.offset_pitch_angle);
		g_holder.mg6020_pid_pitch_speed->target = PIDCalc(g_holder.mg6020_pid_pitch_angle);
		g_holder.mg6020_pid_pitch_speed->feedback = GetActualSpeed(PITCH_MOTOR);
		g_holder.output_pitch_voltage = GetOutPutVoltage(PIDCalc(g_holder.mg6020_pid_pitch_speed));

		SetMotorVoltageMg6020(g_holder.output_yaw_voltage, g_holder.output_pitch_voltage, g_shoot.output_voltage, 0);
		// SetMotorVoltageMg6020(g_holder.output_yaw_voltage, g_holder.output_pitch_voltage, 0, 0);
		num++;
		if(num >= 10) {
			HAL_UART_Transmit(&huart6, (uint8_t*)&g_jetson_nano_tx, sizeof(SerialPortTx), 0xFFFF);
			g_jetson_nano_tx.pitch = g_holder.mg6020_pid_pitch_angle->feedback;
			g_jetson_nano_tx.yaw = g_holder.mg6020_pid_yaw_angle->feedback;
			g_jetson_nano_tx.roll = 0;
			num = 0;
		}
		// OUTPUT("YAW_MOTOR %d PITCH_MOTOR %d\n",  g_motor_info[YAW_MOTOR].rotor_angle, g_motor_info[PITCH_MOTOR].rotor_angle);
		vTaskDelay(1);
	}
}
