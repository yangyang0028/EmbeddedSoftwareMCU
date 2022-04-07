#include "bmi088.h"
#include <math.h>
#include <string.h>

#define BMI088_ACC_CHIP_ID 0x00 // the register is  " Who am I "
#define BMI088_ACC_CHIP_ID_VALUE 0x1E

#define BMI088_GYRO_CHIP_ID 0x00
#define BMI088_GYRO_CHIP_ID_VALUE 0x0F

#define BMI088_WRITE_ACCEL_REG_NUM 6
#define BMI088_WRITE_GYRO_REG_NUM 6

enum BMI088_ERROR {
  BMI088_NO_ERROR = 0x00,
  BMI088_ACC_PWR_CTRL_ERROR = 0x01,
  BMI088_ACC_PWR_CONF_ERROR = 0x02,
  BMI088_ACC_CONF_ERROR = 0x03,
  BMI088_ACC_SELF_TEST_ERROR = 0x04,
  BMI088_ACC_RANGE_ERROR = 0x05,
  BMI088_INT1_IO_CTRL_ERROR = 0x06,
  BMI088_INT_MAP_DATA_ERROR = 0x07,
  BMI088_GYRO_RANGE_ERROR = 0x08,
  BMI088_GYRO_BANDWIDTH_ERROR = 0x09,
  BMI088_GYRO_LPM1_ERROR = 0x0A,
  BMI088_GYRO_CTRL_ERROR = 0x0B,
  BMI088_GYRO_INT3_INT4_IO_CONF_ERROR = 0x0C,
  BMI088_GYRO_INT3_INT4_IO_MAP_ERROR = 0x0D,
  BMI088_SELF_TEST_ACCEL_ERROR = 0x80,
  BMI088_SELF_TEST_GYRO_ERROR = 0x40,
  BMI088_NO_SENSOR = 0xFF,
};

#define BMI088_ACC_CONF 0x40
#define BMI088_ACC_CONF_MUST_Set 0x80
#define BMI088_ACC_BWP_SHFITS 0x4
#define BMI088_ACC_OSR4 (0x8 << BMI088_ACC_BWP_SHFITS)
#define BMI088_ACC_OSR2 (0x9 << BMI088_ACC_BWP_SHFITS)
#define BMI088_ACC_NORMAL (0xA << BMI088_ACC_BWP_SHFITS)

#define BMI088_ACC_ODR_SHFITS 0x0
#define BMI088_ACC_12_5_HZ (0x5 << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_25_HZ (0x6 << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_50_HZ (0x7 << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_100_HZ (0x8 << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_200_HZ (0x9 << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_400_HZ (0xA << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_800_HZ (0xB << BMI088_ACC_ODR_SHFITS)
#define BMI088_ACC_1600_HZ (0xC << BMI088_ACC_ODR_SHFITS)

#define BMI088_ACC_RANGE 0x41

#define BMI088_ACC_RANGE_SHFITS 0x0
#define BMI088_ACC_RANGE_3G (0x0 << BMI088_ACC_RANGE_SHFITS)
#define BMI088_ACC_RANGE_6G (0x1 << BMI088_ACC_RANGE_SHFITS)
#define BMI088_ACC_RANGE_12G (0x2 << BMI088_ACC_RANGE_SHFITS)
#define BMI088_ACC_RANGE_24G (0x3 << BMI088_ACC_RANGE_SHFITS)

#define BMI088_INT1_IO_CTRL 0x53
#define BMI088_ACC_INT1_IO_ENABLE_SHFITS 0x3
#define BMI088_ACC_INT1_IO_ENABLE (0x1 << BMI088_ACC_INT1_IO_ENABLE_SHFITS)
#define BMI088_ACC_INT1_GPIO_MODE_SHFITS 0x2
#define BMI088_ACC_INT1_GPIO_PP (0x0 << BMI088_ACC_INT1_GPIO_MODE_SHFITS)
#define BMI088_ACC_INT1_GPIO_OD (0x1 << BMI088_ACC_INT1_GPIO_MODE_SHFITS)
#define BMI088_ACC_INT1_GPIO_LVL_SHFITS 0x1
#define BMI088_ACC_INT1_GPIO_LOW (0x0 << BMI088_ACC_INT1_GPIO_LVL_SHFITS)
#define BMI088_ACC_INT1_GPIO_HIGH (0x1 << BMI088_ACC_INT1_GPIO_LVL_SHFITS)

#define BMI088_INT2_IO_CTRL 0x54
#define BMI088_ACC_INT2_IO_ENABLE_SHFITS 0x3
#define BMI088_ACC_INT2_IO_ENABLE (0x1 << BMI088_ACC_INT2_IO_ENABLE_SHFITS)
#define BMI088_ACC_INT2_GPIO_MODE_SHFITS 0x2
#define BMI088_ACC_INT2_GPIO_PP (0x0 << BMI088_ACC_INT2_GPIO_MODE_SHFITS)
#define BMI088_ACC_INT2_GPIO_OD (0x1 << BMI088_ACC_INT2_GPIO_MODE_SHFITS)
#define BMI088_ACC_INT2_GPIO_LVL_SHFITS 0x1
#define BMI088_ACC_INT2_GPIO_LOW (0x0 << BMI088_ACC_INT2_GPIO_LVL_SHFITS)
#define BMI088_ACC_INT2_GPIO_HIGH (0x1 << BMI088_ACC_INT2_GPIO_LVL_SHFITS)

#define BMI088_INT_MAP_DATA 0x58
#define BMI088_ACC_INT2_DRDY_INTERRUPT_SHFITS 0x6
#define BMI088_ACC_INT2_DRDY_INTERRUPT                                         \
  (0x1 << BMI088_ACC_INT2_DRDY_INTERRUPT_SHFITS)
#define BMI088_ACC_INT1_DRDY_INTERRUPT_SHFITS 0x2
#define BMI088_ACC_INT1_DRDY_INTERRUPT                                         \
  (0x1 << BMI088_ACC_INT1_DRDY_INTERRUPT_SHFITS)

#define BMI088_ACC_SELF_TEST 0x6D
#define BMI088_ACC_SELF_TEST_OFF 0x00
#define BMI088_ACC_SELF_TEST_POSITIVE_SIGNAL 0x0D
#define BMI088_ACC_SELF_TEST_NEGATIVE_SIGNAL 0x09

#define BMI088_ACC_PWR_CONF 0x7C
#define BMI088_ACC_PWR_SUSPEND_MODE 0x03
#define BMI088_ACC_PWR_ACTIVE_MODE 0x00

#define BMI088_ACC_PWR_CTRL 0x7D
#define BMI088_ACC_ENABLE_ACC_OFF 0x00
#define BMI088_ACC_ENABLE_ACC_ON 0x04

#define BMI088_ACCEL_XOUT_L 0x12
#define BMI088_ACCEL_XOUT_M 0x13
#define BMI088_ACCEL_YOUT_L 0x14
#define BMI088_ACCEL_YOUT_M 0x15
#define BMI088_ACCEL_ZOUT_L 0x16
#define BMI088_ACCEL_ZOUT_M 0x17

#define BMI088_GYRO_SOFTRESET 0x14
#define BMI088_GYRO_SOFTRESET_VALUE 0xB6

#define BMI088_ACC_SOFTRESET 0x7E
#define BMI088_ACC_SOFTRESET_VALUE 0xB6

static uint8_t
    g_write_bmi088_accel_reg_data_error[BMI088_WRITE_ACCEL_REG_NUM][3] = {
        // Switches accelerometer ON or OFF. Required to do after every reset in
        // order to obtain acceleration values.
        {BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON,
         BMI088_ACC_PWR_CTRL_ERROR},
        // Switches accelerometer into suspend mode for saving power. In this
        // mode the data acquisition is stopped.
        {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE,
         BMI088_ACC_PWR_CONF_ERROR},
        // Accelerometer configuration register.
        {BMI088_ACC_CONF,
         BMI088_ACC_OSR4 | BMI088_ACC_1600_HZ | BMI088_ACC_CONF_MUST_Set,
         BMI088_ACC_CONF_ERROR},
        // Accelerometer range setting register.
        {BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G, BMI088_ACC_RANGE_ERROR},
        // Configures the input/output pin INT1.
        {BMI088_INT1_IO_CTRL,
         BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP |
             BMI088_ACC_INT1_GPIO_LOW,
         BMI088_INT1_IO_CTRL_ERROR},
        // Map data ready interrupt to output pin INT1 and/or INT2.
        {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT,
         BMI088_INT_MAP_DATA_ERROR}

};

#define BMI088_GYRO_RANGE 0x0F
#define BMI088_GYRO_RANGE_SHFITS 0x0
#define BMI088_GYRO_2000 (0x0 << BMI088_GYRO_RANGE_SHFITS)
#define BMI088_GYRO_1000 (0x1 << BMI088_GYRO_RANGE_SHFITS)
#define BMI088_GYRO_500 (0x2 << BMI088_GYRO_RANGE_SHFITS)
#define BMI088_GYRO_250 (0x3 << BMI088_GYRO_RANGE_SHFITS)
#define BMI088_GYRO_125 (0x4 << BMI088_GYRO_RANGE_SHFITS)

#define BMI088_GYRO_BANDWIDTH 0x10
// the first num means Output data  rate, the second num means bandwidth
#define BMI088_GYRO_BANDWIDTH_MUST_Set 0x80
#define BMI088_GYRO_2000_532_HZ 0x00
#define BMI088_GYRO_2000_230_HZ 0x01
#define BMI088_GYRO_1000_116_HZ 0x02
#define BMI088_GYRO_400_47_HZ 0x03
#define BMI088_GYRO_200_23_HZ 0x04
#define BMI088_GYRO_100_12_HZ 0x05
#define BMI088_GYRO_200_64_HZ 0x06
#define BMI088_GYRO_100_32_HZ 0x07

#define BMI088_GYRO_LPM1 0x11
#define BMI088_GYRO_NORMAL_MODE 0x00
#define BMI088_GYRO_SUSPEND_MODE 0x80
#define BMI088_GYRO_DEEP_SUSPEND_MODE 0x20

#define BMI088_GYRO_SOFTRESET 0x14
#define BMI088_GYRO_SOFTRESET_VALUE 0xB6

#define BMI088_GYRO_CTRL 0x15
#define BMI088_DRDY_OFF 0x00
#define BMI088_DRDY_ON 0x80

#define BMI088_GYRO_INT3_INT4_IO_CONF 0x16
#define BMI088_GYRO_INT4_GPIO_MODE_SHFITS 0x3
#define BMI088_GYRO_INT4_GPIO_PP (0x0 << BMI088_GYRO_INT4_GPIO_MODE_SHFITS)
#define BMI088_GYRO_INT4_GPIO_OD (0x1 << BMI088_GYRO_INT4_GPIO_MODE_SHFITS)
#define BMI088_GYRO_INT4_GPIO_LVL_SHFITS 0x2
#define BMI088_GYRO_INT4_GPIO_LOW (0x0 << BMI088_GYRO_INT4_GPIO_LVL_SHFITS)
#define BMI088_GYRO_INT4_GPIO_HIGH (0x1 << BMI088_GYRO_INT4_GPIO_LVL_SHFITS)
#define BMI088_GYRO_INT3_GPIO_MODE_SHFITS 0x1
#define BMI088_GYRO_INT3_GPIO_PP (0x0 << BMI088_GYRO_INT3_GPIO_MODE_SHFITS)
#define BMI088_GYRO_INT3_GPIO_OD (0x1 << BMI088_GYRO_INT3_GPIO_MODE_SHFITS)
#define BMI088_GYRO_INT3_GPIO_LVL_SHFITS 0x0
#define BMI088_GYRO_INT3_GPIO_LOW (0x0 << BMI088_GYRO_INT3_GPIO_LVL_SHFITS)
#define BMI088_GYRO_INT3_GPIO_HIGH (0x1 << BMI088_GYRO_INT3_GPIO_LVL_SHFITS)

#define BMI088_GYRO_INT3_INT4_IO_MAP 0x18

#define BMI088_GYRO_DRDY_IO_OFF 0x00
#define BMI088_GYRO_DRDY_IO_INT3 0x01
#define BMI088_GYRO_DRDY_IO_INT4 0x80
#define BMI088_GYRO_DRDY_IO_BOTH                                               \
  (BMI088_GYRO_DRDY_IO_INT3 | BMI088_GYRO_DRDY_IO_INT4)

#define BMI088_GYRO_X_L 0x02
#define BMI088_GYRO_X_H 0x03
#define BMI088_GYRO_Y_L 0x04
#define BMI088_GYRO_Y_H 0x05
#define BMI088_GYRO_Z_L 0x06
#define BMI088_GYRO_Z_H 0x07

static uint8_t
    g_write_BMI088_gyro_reg_data_error[BMI088_WRITE_GYRO_REG_NUM][3] = {
        // Angular rate range and resolution.
        {BMI088_GYRO_RANGE, BMI088_GYRO_2000, BMI088_GYRO_RANGE_ERROR},
        // The register allows the selection of the rate data filter bandwidth
        // and output data rate (ODR).
        {BMI088_GYRO_BANDWIDTH,
         BMI088_GYRO_100_32_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set,
         BMI088_GYRO_BANDWIDTH_ERROR},
        // Switch to the main power modes
        {BMI088_GYRO_LPM1, BMI088_GYRO_NORMAL_MODE, BMI088_GYRO_LPM1_ERROR},
        // Enables the new data interrupt to be triggered on new data.
        {BMI088_GYRO_CTRL, BMI088_DRDY_ON, BMI088_GYRO_CTRL_ERROR},
        // Sets electrical and logical properties of the interrupt pins.
        {BMI088_GYRO_INT3_INT4_IO_CONF,
         BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW,
         BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
        // Map the data ready interrupt pin to one of the interrupt pins INT3
        // and/or INT4.
        {BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3,
         BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}

};

#define BMI088_TEMP_M 0x22
#define BMI088_TEMP_L 0x23

static void Bmi088AccelReadSingleReg(BMI088 *bmi088, uint8_t reg,
                                     uint8_t *data) {
  uint8_t temp_reg[3] = {0};
  uint8_t temp_data[3] = {0};
  temp_reg[0] = reg | 0x80;
  bmi088->AcceDataTransfer(temp_reg, 1, temp_data, 2);
  *data = temp_data[1];
}

static void Bmi088AccelWriteSingleReg(BMI088 *bmi088, uint8_t reg,
                                      uint8_t data) {
  uint8_t temp_reg[3] = {0};
  uint8_t temp_data[3] = {0};
  temp_reg[0] = reg & 0x7f;
  temp_reg[1] = data;
  bmi088->AcceDataTransfer(temp_reg, 2, temp_data, 0);
}

static RETURN_CODE Bmi088AccelInit(BMI088 *bmi088) {
  uint8_t res = 0;
  Bmi088AccelReadSingleReg(bmi088, BMI088_ACC_CHIP_ID, &res);
  bmi088->DelayMs(1);
  Bmi088AccelWriteSingleReg(bmi088, BMI088_ACC_SOFTRESET,
                            BMI088_ACC_SOFTRESET_VALUE);
  bmi088->DelayMs(1);

  Bmi088AccelReadSingleReg(bmi088, BMI088_ACC_CHIP_ID, &res);
  bmi088->DelayMs(1);

  Bmi088AccelReadSingleReg(bmi088, BMI088_ACC_CHIP_ID, &res);
  bmi088->DelayMs(1);

  if (res != BMI088_ACC_CHIP_ID_VALUE) {
    OUTPUT("accel chip ID error!!!\n");
    return EERROR;
  }
  return EOK;
}

static void Bmi088GryoReadSingleReg(BMI088 *bmi088, uint8_t reg,
                                    uint8_t *data) {
  uint8_t temp_reg[3] = {0};
  uint8_t temp_data[3] = {0};
  temp_reg[0] = reg | 0x80;
  bmi088->GryoDataTransfer(temp_reg, 1, temp_data, 1);
  *data = temp_data[0];
}

static void Bmi088GryoWriteSingleReg(BMI088 *bmi088, uint8_t reg,
                                     uint8_t data) {
  uint8_t temp_reg[3] = {0};
  uint8_t temp_data[3] = {0};
  temp_reg[0] = reg & 0x7f;
  temp_reg[1] = data;
  bmi088->GryoDataTransfer(temp_reg, 2, temp_data, 0);
}

static RETURN_CODE Bmi088GryoInit(BMI088 *bmi088) {
  uint8_t res = 0;

  Bmi088GryoWriteSingleReg(bmi088, BMI088_GYRO_SOFTRESET,
                           BMI088_GYRO_SOFTRESET_VALUE);
  bmi088->DelayMs(1);

  Bmi088GryoReadSingleReg(bmi088, BMI088_GYRO_CHIP_ID, &res);
  bmi088->DelayMs(1);

  Bmi088GryoReadSingleReg(bmi088, BMI088_GYRO_CHIP_ID, &res);
  bmi088->DelayMs(1);

  if (res != BMI088_GYRO_CHIP_ID_VALUE) {
    OUTPUT("gyro chip ID error!!!\n");
    return EERROR;
  }
  return EOK;
}

RETURN_CODE Bmi088AccelConfig(BMI088 *bmi088) {
  uint8_t res = 0;
  for (int write_reg_num = 0; write_reg_num < BMI088_WRITE_ACCEL_REG_NUM;
       write_reg_num++) {
    Bmi088AccelWriteSingleReg(
        bmi088, g_write_bmi088_accel_reg_data_error[write_reg_num][0],
        g_write_bmi088_accel_reg_data_error[write_reg_num][1]);
    bmi088->DelayMs(1);
    Bmi088AccelReadSingleReg(
        bmi088, g_write_bmi088_accel_reg_data_error[write_reg_num][0], &res);
    bmi088->DelayMs(1);
    if (res != g_write_bmi088_accel_reg_data_error[write_reg_num][1]) {
      OUTPUT("set accel config is failed!!!\n");
      return g_write_bmi088_accel_reg_data_error[write_reg_num][2];
    }
  }
  return EOK;
}

RETURN_CODE Bmi088GryoConfig(BMI088 *bmi088) {
  uint8_t res = 0;
  for (int write_reg_num = 0; write_reg_num < BMI088_WRITE_GYRO_REG_NUM;
       write_reg_num++) {
    Bmi088GryoWriteSingleReg(
        bmi088, g_write_BMI088_gyro_reg_data_error[write_reg_num][0],
        g_write_BMI088_gyro_reg_data_error[write_reg_num][1]);
    bmi088->DelayMs(1);
    Bmi088GryoReadSingleReg(
        bmi088, g_write_BMI088_gyro_reg_data_error[write_reg_num][0], &res);
    bmi088->DelayMs(1);
    if (res != g_write_BMI088_gyro_reg_data_error[write_reg_num][1]) {
      OUTPUT("set gryo config is failed!!!\n");
      return g_write_BMI088_gyro_reg_data_error[write_reg_num][2];
    }
  }
  return EOK;
}

RETURN_CODE Bmi088Init(BMI088 *bmi088) {
  if (bmi088 == NULL || bmi088->AcceDataTransfer == NULL ||
      bmi088->GryoDataTransfer == NULL || bmi088->DelayMs == NULL) {
    return EEMPTY;
  }
  if (Bmi088AccelInit(bmi088)) {
    return EERROR;
  }
  if (Bmi088AccelConfig(bmi088)) {
    return EERROR;
  }
  if (Bmi088GryoInit(bmi088)) {
    return EERROR;
  }
  if (Bmi088GryoConfig(bmi088)) {
    return EERROR;
  }
  return EOK;
}

RETURN_CODE Bmi088ReadAccel(BMI088 *bmi088) {
  if (bmi088 == NULL || bmi088->AcceDataTransfer == NULL ||
      bmi088->GryoDataTransfer == NULL || bmi088->DelayMs == NULL) {
    return EEMPTY;
  }
  uint8_t reg_buf[1] = {BMI088_ACCEL_XOUT_L | 0x80};
  uint8_t data_buf[7] = {0};
  bmi088->AcceDataTransfer(reg_buf, 1, data_buf, 7);
  int16_t accel[3];
  memcpy((uint8_t *)accel, &data_buf[1], 6);
  bmi088->accel[0] =
      accel[0] / 32768.0 * 1000 * pow(2, (BMI088_ACC_RANGE_3G + 1)) * 1.5;
  bmi088->accel[1] =
      accel[1] / 32768.0 * 1000 * pow(2, (BMI088_ACC_RANGE_3G + 1)) * 1.5;
  bmi088->accel[2] =
      accel[2] / 32768.0 * 1000 * pow(2, (BMI088_ACC_RANGE_3G + 1)) * 1.5;
  return EOK;
}

RETURN_CODE Bmi088ReadTemp(BMI088 *bmi088) {
  if (bmi088 == NULL || bmi088->AcceDataTransfer == NULL ||
      bmi088->GryoDataTransfer == NULL || bmi088->DelayMs == NULL) {
    return EEMPTY;
  }
  uint8_t reg_buf[1] = {BMI088_TEMP_M | 0x80};
  uint8_t data_buf[3] = {0};
  bmi088->AcceDataTransfer(reg_buf, 1, data_buf, 3);
  uint16_t temp_uint = data_buf[1] * 8 + data_buf[2] / 32.0;
  int16_t temp_int = 0;
  if (temp_uint > 1023) {
    temp_int = temp_uint - 2048;
  } else {
    temp_int = temp_uint;
  }
  bmi088->temperature = temp_int * 0.125 + 23;
  return EOK;
}

RETURN_CODE Bmi088ReadGryo(BMI088 *bmi088) {
  if (bmi088 == NULL || bmi088->AcceDataTransfer == NULL ||
      bmi088->GryoDataTransfer == NULL || bmi088->DelayMs == NULL) {
    return EEMPTY;
  }
  uint8_t reg_buf[1] = {BMI088_GYRO_X_L | 0x80};
  uint8_t data_buf[6] = {0};
  bmi088->GryoDataTransfer(reg_buf, 1, data_buf, 6);
  int16_t gyro[3];
  memcpy((uint8_t *)gyro, data_buf, 6);
  bmi088->gyro[0] = gyro[0] / 32767.0 * 2000;
  bmi088->gyro[1] = gyro[1] / 32767.0 * 2000;
  bmi088->gyro[2] = gyro[2] / 32767.0 * 2000;
  return EOK;
}
