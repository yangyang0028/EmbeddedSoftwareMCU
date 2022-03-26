#include "common.h"
#include "AS5600.h"
#include "svpwm.h"
#include "IQmathLib.h"

#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

uint32_t ADC_Values[2]={0};
_iq X, Y, Z;
float x, y, z;
float time_cnt;

void UserMain() {
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
    // HAL_TIM_IRQHandler(&htim2);
    HAL_ADC_Start_IT(&hadc1);
    DBG_OUTPUT(INFORMATION, "HELLO Word!");
    while(1){
        time_cnt+=0.05;
        X = _IQsinPU(_IQ(time_cnt));
        x = _IQtoF(X) + 1;
            Y = _IQsinPU(_IQ(time_cnt + 1.0/3.0));
        y = _IQtoF(Y) + 1;
        Z = _IQsinPU(_IQ(time_cnt - 1.0/3.0));
        z = _IQtoF(Z) + 1;

        DBG_OUTPUT(INFORMATION, ",X=%ld,Y=%ld,Z=%ld,",(int)(x*2500.0), (int)(y*2500.0), (int)(z*2500.0));

        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, (int)(x * 2500));
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, (int)(y * 2500));
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, (int)(z * 2500));
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  ADC_Values[0]=HAL_ADC_GetValue(hadc);
  ADC_Values[1]=HAL_ADC_GetValue(hadc);
  // DBG_OUTPUT(INFORMATION, "%ld %ld", ADC_Values[0], ADC_Values[1]);
}
