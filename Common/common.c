#include "common.h"
#include <stdarg.h>
#include <stdio.h>

void log_put(const char * fmt,...) {
    va_list args;
    size_t length;
    static char log_buf[CONSOLEBUF_SIZE];
    va_start(args, fmt);
    length = vsnprintf(log_buf, sizeof(log_buf) - 1, fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t*)log_buf, length, 0xFFFF);
}
