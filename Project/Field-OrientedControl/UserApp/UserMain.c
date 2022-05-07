#include "common.h"
#include "AS5600.h"

#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "Foc.h"

#include <stdint.h>

void AS5600Receive(uint8_t address, uint8_t *buffer, uint32_t len) {
  while(HAL_I2C_Master_Receive(&hi2c1, address, buffer, len, 0xff) != HAL_OK);
}

void AS5600Transmit(uint8_t address, uint8_t *buffer, uint32_t len) {
   while(HAL_I2C_Master_Transmit(&hi2c1, (address | 1), buffer, len, 0xff) != HAL_OK);
}

struct AS5600 g_as5600 = {
  .Angle = 0,
  .AS5600Receive = AS5600Receive,
  .AS5600Transmit = AS5600Transmit,
};

void OutPutPWM(float Ta, float Tb, float Tc) {
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, (int)(Ta * 1000));
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, (int)(Tb * 1000));
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, (int)(Tc * 1000));
}

void DelayMs(uint32_t ms) {
  HAL_Delay(ms);
}

float GetShaftAnage() {
  AS5600GetAngle(&g_as5600);
  return g_as5600.Angle / 4096.0 * _2PI;
}

struct PID  velocity_pid = {
    .Kp = 0.7,
    .Ki = 0.001,
    .Kd = 0.0,
};

struct FOCConfig g_foc_config = {
    .controller = Type_velocity_openloop_angle,
    .voltage_power_supply = 12.0,
    .voltage_limit = 3.0,
    .voltage_sensor_align = 1,
    .velocity_limit = 20,
    .velocity_pid = &velocity_pid,
    .pole_pairs = 11,
    .zero_electric_angle = 1.828502,
    .OutPutPWM = OutPutPWM,
    .DelayMs = DelayMs,
    .GetShaftAnage = GetShaftAnage,
};

struct FOC g_foc = {
  .foc_config = &g_foc_config,
};

uint32_t ADC_Values[2]={0};

uint8_t RUN = 0;
void UserMain() {
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
    HAL_ADC_Start_IT(&hadc1);
    DBG_OUTPUT(INFORMATION, "Field-OrientedControl");
    // __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 500);
    // __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 500);
    // __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 500);
    // DBG_OUTPUT(INFORMATION, "FOCInit %d", FOCInit(&g_foc));
    // OUTPUT("%f", FOCTestZeroElectricAngle(&g_foc));
    while(1) {
      // FOCMove(&g_foc, 2);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  ADC_Values[0]=HAL_ADC_GetValue(hadc);
  ADC_Values[1]=HAL_ADC_GetValue(hadc);
  // OUTPUT("a=%d,b=%d,", ADC_Values[0], ADC_Values[1]);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (RUN)
  FOCMove(&g_foc, _2PI);
}
