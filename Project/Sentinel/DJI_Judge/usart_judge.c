/**
  ****************************(C) COPYRIGHT 2021 HRBUST_AIR****************************
  * @file    usart_judge.c
	* @brief   DJI����ϵͳ��������           
  * @author  JackyJuu , HRBUST_AIR_TEAM , website:www.airclub.tech
	* @Note 	 ����DMA˫������ɣ�����Ҫ����DMAͨ��
  * @version V1.0.0
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     12-2-2020      JackyJuu            Done
  ****************************(C) COPYRIGHT 2021 HRBUST_AIR****************************
	* @describe DJI����ϵͳ��������
*/
#include "usart_judge.h"
#include "Judge_Data.h"


//����ϵͳ���ݻ�����;
uint8_t DJI_Judge_Usart_Buff[2][DJI_Judge_Buf_Len_Max];
volatile int DJI_Judge_Data_Len;

//��ʼ��
void DJI_Judge_Usart_Init(void)
{

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_DMA2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	USART_DeInit(USART6);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_Init(USART6, &USART_InitStructure);

  USART_ClearFlag(USART6, USART_FLAG_IDLE);
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);  

	USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE); 
	USART_Cmd(USART6,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	
	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel= DMA_Channel_5;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART6->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DJI_Judge_Usart_Buff[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = DJI_Judge_Buf_Len_Max;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1,&DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)DJI_Judge_Usart_Buff[1], DMA_Memory_0);
  	DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);
	
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA2_Stream1,DISABLE);
	DMA_Cmd(DMA2_Stream1,ENABLE);
	

}

void Judge_SendChar(uint8_t U8Data)
{
	while (USART_GetFlagStatus( USART6, USART_FLAG_TC ) == RESET);
	USART_SendData(USART6, U8Data );   
}

void USART6_IRQHandler(void)
{

	volatile	uint16_t this_time_rx_len = 0;
	volatile	int Usart6_Clean_IDLE_Flag = 0;
	
    if (USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
    {
			Usart6_Clean_IDLE_Flag = USART6->SR;
			Usart6_Clean_IDLE_Flag = USART6->DR;

			if(DMA_GetCurrentMemoryTarget(DMA2_Stream1) == 1)
        {

					DMA_MemoryTargetConfig(DMA2_Stream1,(uint32_t)DJI_Judge_Usart_Buff[0],DMA_Memory_0);
					Judge_Data_check(DJI_Judge_Usart_Buff[0]);
					memset(DJI_Judge_Usart_Buff[0],0,DJI_Judge_Buf_Len_Max);
        }
        else
				{
					DMA_MemoryTargetConfig(DMA2_Stream1,(uint32_t)DJI_Judge_Usart_Buff[1],DMA_Memory_1);
					Judge_Data_check(DJI_Judge_Usart_Buff[1]);
					memset(DJI_Judge_Usart_Buff[1],0,DJI_Judge_Buf_Len_Max);
        }
				
    }
		else if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_DMEIF2) != RESET)
    {
        USART_ReceiveData(USART6);
    }
}

