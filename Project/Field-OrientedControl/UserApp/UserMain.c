#include "common.h"
#include "AS5600.h"
#include "svpwm.h"
#include "IQmathLib.h"

#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "Foc.h"

#include <stdint.h>

void AS5600Receive(uint8_t address, uint8_t *buffer, uint32_t len) {
  HAL_I2C_Master_Receive(&hi2c1, address, buffer, len, 0xff);
}

void AS5600Transmit(uint8_t address, uint8_t *buffer, uint32_t len) {
  HAL_I2C_Master_Transmit(&hi2c1, (address | 1), buffer, len, 0xff);
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

float GetAnage() {
  AS5600GetAngle(&g_as5600);
  // OUTPUT("%d\n", g_as5600.Angle);
  return g_as5600.Angle / 4096.0 * _2PI;
}

struct PID  velocity_pid = {
    .Kp = 1.0,
    .Ki = 0.0,
    .Kd = 0.0,
};

struct FOCConfig g_foc_config = {
    .controller = Type_velocity,
    .voltage_power_supply = 12.0,
    .voltage_limit = 3.0,
    .voltage_sensor_align = 1,
    .velocity_limit = 20,
    .velocity_pid = &velocity_pid,
    .pole_pairs = 7,
    .OutPutPWM = OutPutPWM,
    .DelayMs = DelayMs,
    .GetAnage = GetAnage,
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
    // HAL_TIM_Base_Start_IT(&htim1);
    HAL_ADC_Start_IT(&hadc1);
    DBG_OUTPUT(INFORMATION, "HELLO Word!");
    FOCInit(&g_foc);
    RUN = 1;
    while(1) {
      FOCMove(&g_foc, _2PI);
      // OUTPUT("%lld\n", _micros());
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  ADC_Values[0]=HAL_ADC_GetValue(hadc);
  ADC_Values[1]=HAL_ADC_GetValue(hadc);
  // DBG_OUTPUT(INFORMATION, "%ld %ld", ADC_Values[0], ADC_Values[1]);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (RUN)
  FOCMove(&g_foc, _2PI);
}
