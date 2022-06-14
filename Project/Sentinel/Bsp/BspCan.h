#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#include "can.h"

#define FEEDBACK_ID_BASE      0x200
#define MOTOR_MAX_NUM         8
#define YAW_MOTOR             5
#define PITCH_MOTOR           6
#define DIAL_MOTOR            7

typedef struct {
    uint16_t can_id;
    int16_t  set_voltage;
    uint16_t rotor_angle;
    int16_t  rotor_speed;
    int16_t  torque_current;
    uint8_t  temp;
}MotorInfo;

extern MotorInfo g_motor_info[MOTOR_MAX_NUM];

void CanUserInit(void);
void SetMotorVoltageMg6020(int16_t v1, int16_t v2, int16_t v3, int16_t v4);
#endif
