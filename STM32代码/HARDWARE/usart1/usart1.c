/**
  *************************************************************************************************************************
  * @file    usart1.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   串口1.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "usart1.h"
#include "string.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
u8 USART1_RX_BUF[USART1_REC_LEN]={0};//串口1数据缓冲区
u8 USART1_REC_CNT_LEN=0;//接收数据个数计数变量

/**
 * 函数名:Usart1_Init
 * 描述:串口1初始化
 * 输入:Bound-波特率
 * 输出:无
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
	
	USART_ITConfig(USART1,USART_IT_RXNE|USART_IT_IDLE,ENABLE);//允许串口1接收中断和空闲中断
	USART_Cmd(USART1,ENABLE);//使能串口1
}

/**
 * 函数名:Usart1_SendByte
 * 描述:串口1发送单个字节
 * 输入:Data
 * 输出:无
 */
void Usart1_SendByte(uint8_t Data)
{
	USART_SendData(USART1,Data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
}


/**
 * 函数名:Usart1_SendString
 * 描述:串口1发送字符串
 * 输入:*str
 * 输出:无
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
 * 函数名:USART1_IRQHandler
 * 描述:串口1中断服务函数-接收数据到缓冲区
 * 输入:无
 * 输出:无
 */
void USART1_IRQHandler(void)
{
	uint8_t data;//接收数据暂存变量
	uint8_t BufCopy[128];//最多只取前64个数据

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		data = USART_ReceiveData(USART1);
		USART1_RX_BUF[USART1_REC_CNT_LEN++] = data; //接收的数据存入接收数组   			
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//空闲中断
	{
		data=USART1->SR;//串口空闲中断的中断标志只能通过先读SR寄存器，再读DR寄存器清除！
		data=USART1->DR;
		
		//清空本地接收数组
		memset(BufCopy,0,128);
		memcpy(BufCopy,USART1_RX_BUF,USART1_REC_CNT_LEN);//将接收到的数据复制到BufCopy数组中
		protocol_data_recv(BufCopy, USART1_REC_CNT_LEN);
		memset(USART1_RX_BUF,0,128);
		USART1_REC_CNT_LEN=0;
	}
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}


/*****************************************************END OF FILE*********************************************************/	
