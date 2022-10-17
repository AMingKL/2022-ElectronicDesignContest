/**
  *************************************************************************************************************************
  * @file    usart3.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   ����3.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart3.h"
#include "string.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/
u8 USART3_RX_BUF[USART3_REC_LEN]={0};//����3���ݻ�����
u8 USART3_REC_CNT_LEN=0;//�������ݸ�����������

/**
 * ������:Usart3_Init
 * ����:����3��ʼ��
 * ����:Bound-������
 * ���:��
 */
void Usart3_Init(u32 Bound)
{
  GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TXD-PB10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//RXD-PB11
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=Bound;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//������3�����ж�
	USART_Cmd(USART3,ENABLE);//ʹ�ܴ���3
	

}

/**
 * ������:Usart3_SendByte
 * ����:����3���͵����ֽ�
 * ����:Data
 * ���:��
 */
void Usart3_SendByte(uint8_t Data)
{
	USART_SendData(USART3,Data);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
}


/**
 * ������:Usart3_SendString
 * ����:����3�����ַ���
 * ����:*str
 * ���:��
 */
	void Usart3_SendString(char* str)
	{
		 while(* str)
		 {
			 while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET);
			 USART_SendData(USART3,* str++);
		 }
	}

/**
 * ������:USART3_IRQHandler
 * ����:����3�жϷ�����-�������ݵ�������
 * ����:��
 * ���:��
 */
void USART3_IRQHandler(void)
{
		if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
		{	    
			USART3_RX_BUF[USART3_REC_CNT_LEN] = USART_ReceiveData(USART3);	
			USART3_REC_CNT_LEN++;                			      //����ָ������ƶ�
			if(USART3_REC_CNT_LEN > USART3_REC_LEN)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				USART3_REC_CNT_LEN=0;//��������������ָ���׵�ַ
			}
		}	 
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}

//openMV������ 
void openMv_Proc(void)
{
  if(strstr((const char*)USART3_RX_BUF,"end")!=NULL)	//����һ���������
	{
	  if(strcmp((const char*)USART3_RX_BUF,"stopend")==0)//ͣ��
		{
			Param.openMV_Data=1;
		}		
			USART3_REC_CNT_LEN=0; 
			memset((char*)USART3_RX_BUF,0,128);
	}
}


/*****************************************************END OF FILE*********************************************************/	
