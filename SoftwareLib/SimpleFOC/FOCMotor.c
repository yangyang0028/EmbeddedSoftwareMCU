
#include "FOCMotor.h"
#include "foc_utils.h"

/******************************************************************************/
float shaft_angle;//!< current motor angle
float electrical_angle;
float shaft_velocity;
float current_sp;
float shaft_velocity_sp;
float shaft_angle_sp;
DQVoltage_s voltage;
DQCurrent_s current;

TorqueControlType torque_controller;
MotionControlType controller;

float sensor_offset=0;
float zero_electric_angle;
/******************************************************************************/
float electricalAngle(__uint8_t pole_pairs)
{
  return _normalizeAngle((shaft_angle + sensor_offset) * pole_pairs - zero_electric_angle);
}
/******************************************************************************/


