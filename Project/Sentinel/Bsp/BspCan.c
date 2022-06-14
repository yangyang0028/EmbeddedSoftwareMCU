#include "BspCan.h"
#include "stm32f4xx_hal.h"
#include "common.h"

MotorInfo g_motor_info[MOTOR_MAX_NUM];

void CanUserInit(void) {
    CAN_FilterTypeDef can_filter_st; 
    can_filter_st.FilterActivation = ENABLE; 
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK; 
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT; 
    can_filter_st.FilterIdHigh = 0x0000; 
    can_filter_st.FilterIdLow = 0x0000; 
    can_filter_st.FilterMaskIdHigh = 0x0000; 
    can_filter_st.FilterMaskIdLow = 0x0000; 
    can_filter_st.FilterBank = 0; 
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0; 
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st); 
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); 
    HAL_CAN_Start(&hcan1);
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); 
  if ((rx_header.StdId >= FEEDBACK_ID_BASE)
   && (rx_header.StdId <  FEEDBACK_ID_BASE + MOTOR_MAX_NUM)) {
    uint8_t index = rx_header.StdId - FEEDBACK_ID_BASE;
    g_motor_info[index].rotor_angle    = ((rx_data[0] << 8) | rx_data[1]);
    g_motor_info[index].rotor_speed    = ((rx_data[2] << 8) | rx_data[3]);
    g_motor_info[index].torque_current = ((rx_data[4] << 8) | rx_data[5]);
    g_motor_info[index].temp           =   rx_data[6];
  }
}

void SetMotorVoltageMg6020(int16_t v1, int16_t v2, int16_t v3, int16_t v4) {
  CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
  tx_header.StdId = 0x1ff;
  tx_header.IDE   = CAN_ID_STD;
  tx_header.RTR   = CAN_RTR_DATA;
  tx_header.DLC   = 8;
  tx_data[0] = (v1>>8)&0xff;
  tx_data[1] =    (v1)&0xff;
  tx_data[2] = (v2>>8)&0xff;
  tx_data[3] =    (v2)&0xff;
  tx_data[4] = (v3>>8)&0xff;
  tx_data[5] =    (v3)&0xff;
  tx_data[6] = (v4>>8)&0xff;
  tx_data[7] =    (v4)&0xff;
  HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0); 
}
