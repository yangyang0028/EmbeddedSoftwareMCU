#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "main.h"

#include "UserMain.h"
#include "InsTask.h"
#include "Holder.h"
#include "BspUsart.h"



#define START_TASK_PRIO 5
#define START_TASK_SIZE 256
static TaskHandle_t StartTaskHandler = NULL;

#define INS_TASK_PRIO 4
#define INS_TASK_SIZE 512
static TaskHandle_t InsTaskHandler  = NULL;

#define LED_TASK_PRIO 1
#define LED_TASK_SIZE 64
static TaskHandle_t LedTaskHandler  = NULL;

#define HOLDER_TASK_PRIO 3
#define HOLDER_TASK_SIZE 256
static TaskHandle_t HolderTaskHandler  = NULL;

void LedTask(void const * argument) {
    while(1){
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET);
        vTaskDelay(500);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET);
        vTaskDelay(500);
    }
}


void StartTask(void const * argument) {
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)LedTask,
        (const char *)"LedTask",
        (uint16_t)LED_TASK_SIZE,
        (void *)NULL,
        (UBaseType_t)LED_TASK_PRIO,
        (TaskHandle_t *)&LedTaskHandler);

    xTaskCreate((TaskFunction_t)INSTask,
            (const char *)"INSTask",
            (uint16_t)INS_TASK_SIZE,
            (void *)NULL,
            (UBaseType_t)INS_TASK_PRIO,
            (TaskHandle_t *)&InsTaskHandler);

    xTaskCreate((TaskFunction_t)HolderTask,
            (const char *)"HolderTask",
            (uint16_t)HOLDER_TASK_SIZE,
            (void *)NULL,
            (UBaseType_t)HOLDER_TASK_PRIO,
            (TaskHandle_t *)&HolderTaskHandler);

    vTaskDelete(StartTaskHandler);
    taskEXIT_CRITICAL();
}

void UserMain(){
    DBG_OUTPUT(INFORMATION, "System core clock is %0.3fMhz.\n", (float)SystemCoreClock*1e-6);
    UsartUserInit();
    xTaskCreate((TaskFunction_t)StartTask,
            (const char *)"StartTask",
            (uint16_t)START_TASK_SIZE,
            (void *)NULL,
            (UBaseType_t)START_TASK_PRIO,
            (TaskHandle_t *)&StartTaskHandler);
    vTaskStartScheduler();
    while(1);
    return ;
}