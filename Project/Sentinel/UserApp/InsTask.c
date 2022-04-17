#include "InsTask.h"
#include "bmi088.h"
#include "ist8310.h"
#include "FreeRTOS.h"
#include "spi.h"
#include "i2c.h"
#include "tim.h"
#include "AHRS.h"
#include "AHRS_MiddleWare.h"


extern void vTaskDelay( const TickType_t xTicksToDelay);

static void AcceDataTransfer(uint8_t *tx_buf, uint16_t tx_len,
                           uint8_t *rx_buf, uint16_t rx_len) {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_buf, tx_len, 0xff);
    HAL_SPI_Receive(&hspi1, rx_buf, rx_len, 0xff);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

static void GryoDataTransfer(uint8_t *tx_buf, uint16_t tx_len,
                           uint8_t *rx_buf, uint16_t rx_len) {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_buf, tx_len, 0xff);
    HAL_SPI_Receive(&hspi1, rx_buf, rx_len, 0xff);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
}

static void DelayMs(uint16_t Ms) {
    vTaskDelay(Ms);
}

struct BMI088 g_bmi088 = {
    .accel = {0,0,0},
    .gyro = {0,0,0},
    .temperature = 0,
    .AcceDataTransfer = AcceDataTransfer,
    .GryoDataTransfer = GryoDataTransfer,
    .DelayMs = DelayMs,
};

void MagneDataReadData(uint8_t *rx_buf, uint16_t rx_len) {
    HAL_I2C_Master_Receive(&hi2c3, IST8310_ADDRESS << 1, rx_buf, rx_len, 0xff);
}

void MagneDataWriteData(uint8_t *tx_buf, uint16_t tx_len) {
    HAL_I2C_Master_Transmit(&hi2c3, IST8310_ADDRESS << 1, tx_buf, tx_len, 0xff);
}

struct IST8310 g_ist8310 = {
    .magne={0,0,0},
    .temperature = 0,
    .MagneDataReadData = MagneDataReadData,
    .MagneDataWriteData = MagneDataWriteData,
    .DelayMs = DelayMs,
};

float quat[4]={0};
float accel[3]={0,0,9.8};
float gyro[3]={0,0,0};

float gyro_drift[3] = {0,0,0};
float mag[3]={0,0,0};
float yaw = 0;
float pitch =0;
float roll =0;

char buff[4]={"abcd"};

void INSTask(void const * argument) {
    while(Bmi088Init(&g_bmi088) != EOK) vTaskDelay(100);
    Bmi088ReadAccel(&g_bmi088);
    for (int i=0 ;i< 1000; i++) {
        Bmi088ReadGryo(&g_bmi088);
        for (int i=0;i<3;i++){
        gyro_drift[i] += g_bmi088.gyro[i];
        }
        vTaskDelay(1);
    }
    for (int i=0;i<3;i++){
        gyro_drift[i] = gyro_drift[i] / 1000;
    }
    for (int i=0;i<3;i++){
        accel[i] = g_bmi088.accel[i]/100;
    }
    AHRS_init(quat, accel, mag);
    HAL_TIM_Base_Start_IT(&htim3);
    while(1) {
        vTaskDelay(1);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &htim3){
        Bmi088ReadAccel(&g_bmi088);
        for (int i=0;i<3;i++) {
            accel[i] = g_bmi088.accel[i]/100;
        }
        Bmi088ReadGryo(&g_bmi088);
        for (int i=0;i<3;i++) {
            gyro[i] = (g_bmi088.gyro[i] - gyro_drift[i]) / 180.0 * 3.14159;
        }
        AHRS_update(quat, 0.002f, gyro, accel, mag);
        get_angle(quat, &yaw, &pitch, &roll);
        yaw = yaw * 360.0 / (2.0 * 3.14159);
        pitch = pitch * 360.0 / (2.0 * 3.14159);
        roll = roll * 360.0 / (2.0 * 3.14159);
    }
}

