#ifndef _SHOOT_H
#define _SHOOT_H

#include "Pid.h"

typedef struct Shoot {
	PID *pid_angle;
	PID *pid_speed;
    float current_anage;
    float tagle_anage;
    int rotational_speed;
    int output_voltage;
}Shoot;

void ShootTask(void const *argument);

#endif