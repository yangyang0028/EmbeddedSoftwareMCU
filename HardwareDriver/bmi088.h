#ifndef _BMI088_
#define _BMI088_

#include "common.h"

typedef struct BMI088 {
  float accel[3];    // mg
  float gyro[3];     // °/s
  float temperature; // °C
  void (*AcceDataTransfer)(uint8_t *tx_buf, uint32_t tx_len,
                           uint8_t *rx_buf, uint32_t rx_len);
  void (*GryoDataTransfer)(uint8_t *tx_buf, uint32_t tx_len,
                           uint8_t *rx_buf, uint32_t rx_len);
  void (*DelayMs)(uint32_t Ms);
} BMI088;

RETURN_CODE Bmi088Init(BMI088 *bmi088);
RETURN_CODE Bmi088ReadAccel(BMI088 *bmi088);
RETURN_CODE Bmi088ReadGryo(BMI088 *bmi088);
RETURN_CODE Bmi088ReadTemp(BMI088 *bmi088);

#endif
