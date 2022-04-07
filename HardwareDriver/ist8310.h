#ifndef _IST8310_
#define _IST8310_

#include "common.h"

#define IST8310_ADDRESS 0x0E

typedef struct IST8310 {
  float magne[3];    // uT
  uint16_t temperature; // Raw data
  void (*MagneDataReadData)(uint8_t *rx_buf, uint16_t rx_len);
  void (*MagneDataWriteData)(uint8_t *tx_buf, uint16_t tx_len);                   
  void (*DelayMs)(uint16_t Ms);
} IST8310;

RETURN_CODE IST8310Init(IST8310 *ist8310);
RETURN_CODE IST8310ReadMagne(IST8310 *ist8310);
RETURN_CODE IST8310ReadTemp(IST8310 *ist8310);

#endif