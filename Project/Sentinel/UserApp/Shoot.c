#include "Shoot.h"
#include "tim.h"
#include "BspUsart.h"
#include "BspCan.h"

static const float  g_pi_6 = 0.52359877559;

static const int16_t g_motor_min_current = 0;

static const int16_t g_motor_max_current = 10000;

static const float  g_rpm_to_rps = 1.0 / 3600.0 * 6.28318530718 ; // rad/s

static struct PID pid_angle = {
    .Kp = 20.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

static  struct PID pid_speed = {
    .Kp = 200.0,
    .Ki = 0.000,
    .Kd = 0.0,
};

Shoot g_shoot = {
    .pid_angle = &pid_angle,
    .pid_speed = &pid_speed,
};

static float GetActualSpeed(uint8_t motor_id) {
	return g_motor_info[motor_id].rotor_speed * g_rpm_to_rps;
}

static int16_t GetOutPutVoltage(float voltage) {
	if(voltage < g_motor_min_current ) return g_motor_min_current;
	if(voltage > g_motor_max_current ) return g_motor_max_current;
	return (int16_t)voltage;
}

extern struct SerialPortRx g_jetson_nano_rx;

void ShootTask(void const *argument) {
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
    g_shoot.rotational_speed = 0;
    while(g_motor_info[DIAL_MOTOR].rotor_angle == 0) ;
    while(1) {
        g_shoot.rotational_speed ++;
        if(g_shoot.rotational_speed > 1440 ) g_shoot.rotational_speed = 1440;
		// __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, g_shoot.rotational_speed); 
		// __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, g_shoot.rotational_speed);
        g_shoot.current_anage += GetActualSpeed(DIAL_MOTOR) * 0.001;

        if(g_jetson_nano_rx.shooting) {
            g_shoot.tagle_anage = g_shoot.current_anage + g_pi_6;
            g_shoot.pid_angle->target = g_shoot.tagle_anage;
            g_shoot.pid_angle->feedback = g_shoot.current_anage;
            g_shoot.pid_speed->target = PIDCalc(g_shoot.pid_angle);
            g_shoot.pid_speed->feedback = GetActualSpeed(DIAL_MOTOR);
            g_shoot.output_voltage = GetOutPutVoltage(PIDCalc(g_shoot.pid_speed));
        } else {
            g_shoot.output_voltage = 0;
        }
        vTaskDelay(1);
    }
    return ;
}
