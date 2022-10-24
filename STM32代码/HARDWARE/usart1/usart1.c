/**
  *************************************************************************************************************************
  * @file    usart1.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   ����1.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart1.h"
#include "string.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/
u8 USART1_RX_BUF[USART1_REC_LEN]={0};//����1���ݻ�����
u8 USART1_REC_CNT_LEN=0;//�������ݸ�����������

/**
 * ������:Usart1_Init
 * ����:����1��ʼ��
 * ����:Bound-������
 * ���:��
 */
void Usart1_Init(u32 Bound)
{
  GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TXD-PA9
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RXD-PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=Bound;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART1, &USART_InitStructure);
	
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;		
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE|USART_IT_IDLE,ENABLE);//������1�����жϺͿ����ж�
	USART_Cmd(USART1,ENABLE);//ʹ�ܴ���1
}

/**
 * ������:Usart1_SendByte
 * ����:����1���͵����ֽ�
 * ����:Data
 * ���:��
 */
void Usart1_SendByte(uint8_t Data)
{
	USART_SendData(USART1,Data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
}


/**
 * ������:Usart1_SendString
 * ����:����1�����ַ���
 * ����:*str
 * ���:��
 */
void Usart1_SendString(u8* data, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
		USART_SendData(USART1,data[i]);   
	}
}



/**
 * ������:USART1_IRQHandler
 * ����:����1�жϷ�����-�������ݵ�������
 * ����:��
 * ���:��
 */
void USART1_IRQHandler(void)
{
	uint8_t data;//���������ݴ����
	uint8_t BufCopy[128];//���ֻȡǰ64������

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		data = USART_ReceiveData(USART1);
		USART1_RX_BUF[USART1_REC_CNT_LEN++] = data; //���յ����ݴ����������   			
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//�����ж�
	{
		data=USART1->SR;//���ڿ����жϵ��жϱ�־ֻ��ͨ���ȶ�SR�Ĵ������ٶ�DR�Ĵ��������
		data=USART1->DR;
		
		//��ձ��ؽ�������
		memset(BufCopy,0,128);
		memcpy(BufCopy,USART1_RX_BUF,USART1_REC_CNT_LEN);//�����յ������ݸ��Ƶ�BufCopy������
		protocol_data_recv(BufCopy, USART1_REC_CNT_LEN);
		memset(USART1_RX_BUF,0,128);
		USART1_REC_CNT_LEN=0;
	}
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}


/*****************************************************END OF FILE*********************************************************/	
