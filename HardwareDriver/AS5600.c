#include "AS5600.h"

#define AS5600_RAW_ANGLE_REGISTER  0x0E

void AS5600GetAngle(AS5600 *as5600) {
  if(as5600->AS5600Receive == NULL || as5600->AS5600Transmit == NULL){
    return ;
  }
  uint8_t raw_angle_register = AS5600_RAW_ANGLE_REGISTER;
  uint8_t buf[2]={0};
  as5600->AS5600Transmit(AS5600_ADDR, &raw_angle_register, 1);
  as5600->AS5600Receive(AS5600_ADDR, buf, 2);
  as5600->Angle = buf[0]<<8 | buf[1];
  return ;
}