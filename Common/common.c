#include "common.h"
#include <stdarg.h>
#include <stdio.h>
#ifdef USING_USB
#include "usbd_cdc_if.h"
#endif

void log_put(const char * fmt,...) {
    va_list args;
    size_t length;
    static char log_buf[CONSOLEBUF_SIZE];
    va_start(args, fmt);
    length = vsnprintf(log_buf, sizeof(log_buf) - 1, fmt, args);
    va_end(args);
#ifdef USING_USB
    CDC_Transmit_FS(log_buf, length);
#endif
#ifdef AT32
    size_t usart2_tx_counter = 0;
    for (int i = 0; i< length ; i++){
      while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
      usart_data_transmit(USART1, log_buf[usart2_tx_counter++]);
    }
#else
    HAL_UART_Transmit(&huart1, (uint8_t*)log_buf, length, 0xFFFF);
#endif
}

__STATIC_INLINE uint32_t LL_SYSTICK_IsActiveCounterFlag(void) {
    return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}


uint64_t _micros() {
  LL_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (LL_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}
