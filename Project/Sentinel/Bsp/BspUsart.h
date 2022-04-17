#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "usart.h"
#include "common.h"

#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<5)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<7)

typedef struct RCType{
    // each ch value from -660 -- +660.
	int ch1;
	int ch2;
	int ch3;
	int ch4;
	int switch_left;
	int switch_right;
    // x y z value from -32767 -- +32767.
	int16_t x;
	int16_t y;
	int16_t z;
	int press_l;
	int press_r;
	uint16_t v;
}RCType;

typedef struct JNReadType{
    int x_deviation;
    int y_deviation;
}JNReadType;

static RCType g_remote_control;
static JNReadType g_jetson_nano;

void UsartUserInit();

void HAL_UART_IDLE_IRQHandler(UART_HandleTypeDef *huart);

#endif