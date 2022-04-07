
#include "common.h"

#include "Holder.h"
#include "BspCan.h"
#include "Pid.h"

const uint16_t g_motor_angle_range = 8191;
const int16_t g_motor_min_voltage = -30000;
const int16_t g_motor_max_voltage = 30000;
const float  g_2pi = 6.28318530718 ;
const float  g_rpm_to_rps = 1.0 / 3600.0 * 6.28318530718 ; // rad/s

typedef struct Holder {
	PID *mg6020_pid_yaw_angle;
	PID *mg6020_pid_yaw_speed;
	PID *mg6020_pid_pitch_angle;
	PID *mg6020_pid_pitch_speed;
	const uint16_t offset_yaw_angle;
	const uint16_t offset_pitch_angle;

	float target_yaw_angle;
	float actual_yaw_angle;
	float last_actual_yaw_angle;

	float target_pitch_angle;
	float actual_pitch_angle;
	float last_actual_pitch_angle;

	float target_yaw_speed;
	float actual_yaw_speed;
	float last_actual_yaw_speed;

	float target_pitch_speed;
	float actual_pitch_speed;
	float last_actual_pitch_speed;

	int16_t output_yaw_voltage;
	int16_t output_pitch_voltage;
}Holder;

struct PID g_mg6020_pid_yaw_angle = {
    .Kp = 10.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_yaw_speed = {
    .Kp = 1000.0,
    .Ki = 0.000,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_angle = {
    .Kp = 0.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct PID g_mg6020_pid_pitch_speed = {
    .Kp = 0.0,
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
float GetActualAngle(uint8_t motor_id, uint16_t offset_angle, float last_actual_angle) {
	// Get the angle from encoder after offset.
	uint16_t rotor_angle = ((motor_info[motor_id].rotor_angle - offset_angle) + g_motor_angle_range) % g_motor_angle_range;
	float unit_angle = (rotor_angle * 1.0) / g_motor_angle_range ;
	int16_t laps = (int16_t) (last_actual_angle / g_2pi);
	if (last_actual_angle < 0) laps = laps - 1;
	float unit_last_angle = last_actual_angle / g_2pi - laps ;
	// OUTPUT("laps=%d, unit_last_angle=%f, last_actual_angle=%f \n", laps, unit_last_angle, last_actual_angle);
	if(abs_f(unit_last_angle -  unit_angle) > 0.5) {
		if (motor_info[motor_id].rotor_speed > 0) {
			return ((float)laps + 1 + unit_angle) * g_2pi;
		} else {
			return ((float)laps - 1 + unit_angle) * g_2pi;
		}
	} else {
		return ((float)laps + unit_angle) * g_2pi;
	}
	// not running
	return 0;
}

// rad/s
float GetActualSpeed(uint8_t motor_id) {
	return motor_info[motor_id].rotor_speed * g_rpm_to_rps;
}

int16_t GetOutPutVoltage(float voltage) {
	if(voltage < g_motor_min_voltage ) return g_motor_min_voltage;
	if(voltage > g_motor_max_voltage ) return g_motor_max_voltage;
	return (int16_t)voltage;
}

void HolderTask(void const * argument) {
	OUTPUT("Holder task start!!!\n");
	can_user_init();
	vTaskDelay(1000);
	g_holder.target_yaw_angle = 3.14;
	g_holder.last_actual_yaw_angle = GetActualAngle(YAW_MOTOR, g_holder.offset_yaw_angle, 0);
	if (g_holder.last_actual_yaw_angle < 0 ) {
		g_holder.last_actual_yaw_angle = g_holder.last_actual_yaw_angle + g_2pi;
	}
	OUTPUT("last_actual_yaw_angle=%f,\n", g_holder.last_actual_yaw_angle);
	while(1) {
		g_holder.mg6020_pid_yaw_angle->target = g_holder.target_yaw_angle;
		g_holder.mg6020_pid_yaw_angle->feedback = GetActualAngle(YAW_MOTOR, g_holder.offset_yaw_angle, g_holder.last_actual_yaw_angle) ;
		g_holder.last_actual_yaw_angle = g_holder.mg6020_pid_yaw_angle->feedback;
		g_holder.mg6020_pid_yaw_speed->target = PIDCalc(g_holder.mg6020_pid_yaw_angle);
		g_holder.mg6020_pid_yaw_speed->feedback = GetActualSpeed(YAW_MOTOR);
		g_holder.output_yaw_voltage = GetOutPutVoltage(PIDCalc(g_holder.mg6020_pid_yaw_speed));
		
		// OUTPUT("target=%f, feedback=%f, rotor_angle=%d\n", g_holder.mg6020_pid_yaw_angle->target, g_holder.mg6020_pid_yaw_angle->feedback, motor_info[YAW_MOTOR].rotor_angle);
		// OUTPUT("OutPutYawVoltage=%d,\n", g_holder.output_yaw_voltage);
		set_motor_voltage_mg6020(g_holder.output_yaw_voltage, 0, 0, 0);
		vTaskDelay(1);
	}
}
