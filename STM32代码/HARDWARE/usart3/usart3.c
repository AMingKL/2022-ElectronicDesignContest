/**
  *************************************************************************************************************************
  * @file    usart3.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   串口3.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart3.h"
#include "string.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
u8 USART3_RX_BUF[USART3_REC_LEN]={0};//串口3数据缓冲区
u8 USART3_REC_CNT_LEN=0;//接收数据个数计数变量

/**
 * 函数名:Usart3_Init
 * 描述:串口3初始化
 * 输入:Bound-波特率
 * 输出:无
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
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//允许串口3接收中断
	USART_Cmd(USART3,ENABLE);//使能串口3
	

}

/**
 * 函数名:Usart3_SendByte
 * 描述:串口3发送单个字节
 * 输入:Data
 * 输出:无
 */
void Usart3_SendByte(uint8_t Data)
{
	USART_SendData(USART3,Data);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
}


/**
 * 函数名:Usart3_SendString
 * 描述:串口3发送字符串
 * 输入:*str
 * 输出:无
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
 * 函数名:USART3_IRQHandler
 * 描述:串口3中断服务函数-接收数据到缓冲区
 * 输入:无
 * 输出:无
 */
void USART3_IRQHandler(void)
{
		if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
		{	    
			USART3_RX_BUF[USART3_REC_CNT_LEN] = USART_ReceiveData(USART3);	
			USART3_REC_CNT_LEN++;                			      //缓存指针向后移动
			if(USART3_REC_CNT_LEN > USART3_REC_LEN)       		//如果缓存满,将缓存指针指向缓存的首地址
			{
				USART3_REC_CNT_LEN=0;//缓冲区满，重新指向首地址
			}
		}	 
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}

//openMV处理函数 
void openMv_Proc(void)
{
  if(strstr((const char*)USART3_RX_BUF,"end")!=NULL)	//接收一组数据完毕
	{
	  if(strcmp((const char*)USART3_RX_BUF,"stopend")==0)//停车
		{
			Param.openMV_Data=1;
		}		
			USART3_REC_CNT_LEN=0; 
			memset((char*)USART3_RX_BUF,0,128);
	}
}


/*****************************************************END OF FILE*********************************************************/	
