#ifndef __BSP_AS5600_H
#define __BSP_AS5600_H

#include "common.h"
#include <stdint.h>

#define AS5600_RAW_ADDR    0x36
#define AS5600_ADDR        (AS5600_RAW_ADDR << 1)
#define AS5600_WRITE_ADDR  (AS5600_RAW_ADDR << 1)
#define AS5600_READ_ADDR   ((AS5600_RAW_ADDR << 1) | 1)

typedef struct AS5600{
    uint16_t Angle;
    void (*AS5600Transmit)(uint8_t dev_addr, uint8_t *pData, uint32_t size);
    void (*AS5600Receive)(uint8_t dev_addr, uint8_t *pData, uint32_t size);
}AS5600;

void AS5600GetAngle(AS5600 *as5600);

#endif 


