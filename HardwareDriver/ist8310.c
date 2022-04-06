#include "ist8310.h"

#define IST8310_CHIP_ID 0x00 // the register is  " Who am I "
#define IST8310_CHIP_ID_VALUE 0x10

#define IST8310_R_CONFA 0x0A
#define IST8310_R_CONFB 0x0B
#define IST8310_R_MODE 0x02

#define IST8310_R_XL 0x03
#define IST8310_R_XM 0x04
#define IST8310_R_YL 0x05
#define IST8310_R_YM 0x06
#define IST8310_R_ZL 0x07
#define IST8310_R_ZM 0x08

#define IST8310_AVGCNTL 0x41
#define IST8310_PDCNTL 0x42

#define IST8310_WRITE_REG_NUM 2

static uint8_t ist8310_write_reg_data_error[IST8310_WRITE_REG_NUM][3] ={
        {0x0B, 0x08, 0x01},     //enalbe interrupt  and low pin polarity.
        {0x0A, 0x0B, 0x04}};    //200Hz output rate.

RETURN_CODE IST8310Init(IST8310 *ist8310) {
    if (ist8310 == NULL || ist8310->DelayMs == NULL ||
        ist8310->MagneDataReadData == NULL || 
        ist8310->MagneDataWriteData == NULL) {
            return EEMPTY;
        }
    uint8_t tx_buf[2] = {0};
    uint8_t rx_buf[2] = {0};
    tx_buf[0] = IST8310_CHIP_ID;
    ist8310->DelayMs(10);
    ist8310->MagneDataWriteData(tx_buf, 1);
    ist8310->DelayMs(1);
    ist8310->MagneDataReadData(rx_buf, 1);
    if (rx_buf[0] != IST8310_CHIP_ID_VALUE) {
        OUTPUT("magne chip ID error!!!\n");
        return EERROR;
    }

    for (int writeNum = 0; writeNum < IST8310_WRITE_REG_NUM; writeNum++) {
        ist8310->MagneDataWriteData(&ist8310_write_reg_data_error[writeNum][0], 2);
        ist8310->MagneDataWriteData(&ist8310_write_reg_data_error[writeNum][0], 1);
        ist8310->MagneDataReadData(rx_buf, 1);
        if(rx_buf[0] != ist8310_write_reg_data_error[writeNum][1]) {
            OUTPUT("set magne config is failed!!!\n");
            return EERROR;
        }
    }
    return EOK;
}

RETURN_CODE IST8310ReadMagne(IST8310 *ist8310) {
    if (ist8310 == NULL || ist8310->DelayMs == NULL ||
        ist8310->MagneDataReadData == NULL || 
        ist8310->MagneDataWriteData == NULL) {
            return EEMPTY;
        }
    uint8_t tx_buf[1] = {0};
    uint8_t rx_buf[6] = {0};
    tx_buf[0] = IST8310_R_XL;
    ist8310->MagneDataWriteData(tx_buf, 1);
    ist8310->DelayMs(1);
    ist8310->MagneDataReadData(rx_buf, 6);
    int16_t magne[3] ={0};
    memcpy(&magne, rx_buf, 6);
    ist8310->magne[0] = magne[0] * 1600.0 / 32768.0;
    ist8310->magne[1] = magne[1] * 1600.0 / 32768.0;
    ist8310->magne[2] = magne[2] * 1600.0 / 32768.0;

    return EOK;
}

RETURN_CODE IST8310ReadTemp(IST8310 *ist8310) {
    if (ist8310 == NULL || ist8310->DelayMs == NULL ||
        ist8310->MagneDataReadData == NULL || 
        ist8310->MagneDataWriteData == NULL) {
            return EEMPTY;
        }
    uint8_t tx_buf[1] = {0};
    uint8_t rx_buf[2] = {0};
    tx_buf[0] = IST8310_AVGCNTL;
    ist8310->MagneDataWriteData(tx_buf, 1);
    ist8310->MagneDataReadData(rx_buf, 2);
    memcpy(rx_buf, &ist8310->temperature, 2);
    return EOK;
}

