#include "BspUsart.h"


uint8_t g_remote_control_buff[100] = {0};
uint8_t g_jetson_nano_rx_buff[100] = {0};
uint8_t g_dji_judge_buff[100] = {0};


HAL_StatusTypeDef UART_Receive_DMA_NoIT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    uint32_t *tmp;
  
    /* Check that a Rx process is not already ongoing */
    if(huart->RxState == HAL_UART_STATE_READY) {
    if((pData == NULL) || (Size == 0U)) {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    /* Set the DMA abort callback */
    huart->hdmarx->XferAbortCallback = NULL;

    /* Enable the DMA channel */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t*)tmp, Size);

    /* Clear the Overrun flag just before enabling the DMA Rx request: can be mandatory for the second transfer */
    __HAL_UART_CLEAR_OREFLAG(huart);

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error Interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
    in the UART CR3 register */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef HAL_UART_Receive_IT_IDLE(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    return UART_Receive_DMA_NoIT(huart,pData,Size);
}

void UsartUserInit() {
    while(HAL_UART_Receive_IT_IDLE(&huart3, g_remote_control_buff, 100));
    while(HAL_UART_Receive_IT_IDLE(&huart6, g_jetson_nano_rx_buff, 100));
}

void RCHandle(RCType* rc, uint8_t* buff);
void JNHandle(JNReadType* Jn, uint8_t* buff);

void UartIdleTrqHandler(UART_HandleTypeDef *huart){
    if(huart==&huart6) JNHandle(&g_jetson_nano,g_jetson_nano_rx_buff);
    if(huart==&huart3) RCHandle(&g_remote_control,g_remote_control_buff);
}

void HAL_UART_IDLE_IRQHandler(UART_HandleTypeDef *huart) {
	uint32_t DMA_FLAGS,tmp;
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
		tmp = huart->Instance->SR;
		tmp = huart->Instance->DR;
		tmp++;  
		DMA_FLAGS = __HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmarx);
	    __HAL_DMA_DISABLE(huart->hdmarx);
	    __HAL_DMA_CLEAR_FLAG(huart->hdmarx,DMA_FLAGS);
		huart->hdmarx->Instance->NDTR = huart->RxXferSize;
		__HAL_DMA_ENABLE(huart->hdmarx);
	}
    UartIdleTrqHandler(huart);
}

void RCHandle(RCType* rc, uint8_t* buff) {
    rc->ch1 = (buff[0] | buff[1]<<8) & 0x07FF;
	rc->ch1 -= 1024;
	rc->ch2 = (buff[1]>>3 | buff[2]<<5 ) & 0x07FF;
	rc->ch2 -= 1024;
	rc->ch3 = (buff[2]>>6 | buff[3]<<2 | buff[4]<<10) & 0x07FF;
	rc->ch3 -= 1024;
	rc->ch4 = (buff[4]>>1 | buff[5]<<7) & 0x07FF;		
	rc->ch4 -= 1024;
	
	rc->switch_left = ( (buff[5] >> 4)& 0x000C ) >> 2;
	rc->switch_right =  (buff[5] >> 4)& 0x0003 ;
	
	rc->x = ((int16_t)buff[6]) | ((int16_t)buff[7] << 8);
	rc->y = ((int16_t)buff[8]) | ((int16_t)buff[9] << 8);
	rc->z = ((int16_t)buff[10]) | ((int16_t)buff[11] << 8);
	rc->press_l = buff[12];
	rc->press_r = buff[13];
	
	rc->v = ((int16_t)buff[14]);
    OUTPUT("%d, %d\n", g_remote_control.ch1, g_remote_control.ch2);
}

void JNHandle(JNReadType* Jn, uint8_t* buff) {
    uint16_t temp = 0;
    if(buff[0] == 0x55) {
        memcpy(buff+1, &temp, 2);
        Jn->x_deviation = temp - 32768;
        memcpy(buff+3, &temp, 2);
        Jn->y_deviation = temp - 32768;
    }
}