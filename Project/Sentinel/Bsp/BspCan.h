#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#include "can.h"

#define FEEDBACK_ID_BASE      0x200
#define MOTOR_MAX_NUM         8
#define YAW_MOTOR             5

typedef struct {
    uint16_t can_id;
    int16_t  set_voltage;
    uint16_t rotor_angle;
    int16_t  rotor_speed;
    int16_t  torque_current;
    uint8_t  temp;
}moto_info_t;

moto_info_t motor_info[MOTOR_MAX_NUM];

void can_user_init(void);
void set_motor_voltage_mg6020(int16_t v1, int16_t v2, int16_t v3, int16_t v4);
#endif
