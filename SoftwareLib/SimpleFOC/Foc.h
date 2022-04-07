#ifndef _FOC_H
#define _FOC_H

#include <stdint.h>
#include "common.h"
#include "foc_utils.h"
#include "Pid.h"

typedef enum {
	Type_torque,
	Type_velocity,
	Type_angle,
	Type_velocity_openloop,
	Type_velocity_openloop_angle,
}MotionControlType;

typedef enum {
    CW      = 1,  //clockwise
    CCW     = -1, // counter clockwise
    UNKNOWN = 0   //not yet known or invalid state
} Direction;

typedef struct FOCConfig {
    MotionControlType controller;
    float voltage_power_supply;
    float voltage_limit;
    float voltage_sensor_align;
    uint8_t velocity_limit; //rad/s
    uint8_t pole_pairs;
    float zero_electric_angle; //电磁角度与机械角度差
    PID *velocity_pid;
    void (*OutPutPWM)(float Ta, float Tb, float Tc);
    void (*DelayMs)(uint16_t ms);
    float (*GetShaftAnage)(); // 0~2pi; 
}FOCConfig;

typedef struct FOC {
    FOCConfig *foc_config;
    DQVoltage_s voltage;
    DQCurrent_s current; 
    uint64_t time_prev; // 上一次时间
    float angle_prev; // 上一次角度
    Direction sensor_direction; // 方向
    float shaft_angle; // 轴角度  0~2pi;
    float shaft_velocity; // 轴速度 rad/s
    float shaft_velocity_prev;
}FOC;

RETURN_CODE FOCInit(FOC *foc);
RETURN_CODE FOCMove(FOC *foc, float target);
void FOCTestPolePairs(FOC *foc, int pole_pairs);
float FOCTestZeroElectricAngle(FOC *foc);

#endif