
#include <stdint.h>
#include "usart.h"

#define DEBUG "D"
#define INFORMATION "I"
#define WARNING "W"
#define ERROR "E"

#define CONSOLEBUF_SIZE 256

void log_put(const char * fmt,...);

#define OUTPUT(fmt, args...) \
  log_put(fmt, ##args)

#define DBG_OUTPUT(level, fmt, args...) \
  log_put("[" level "]%s%d::" fmt "\n", __FILE__, __LINE__, ##args)

#define RETURN_CODE uint8_t
#define EOK  0
#define EERROR  1
#define ETIMEOUT 2
#define EFULL  3
#define EEMPTY  4
#define ENOMEM  6
#define ENOSYS  7
#define EBUSY  8
#define EIO  9

uint64_t _micros(void);
