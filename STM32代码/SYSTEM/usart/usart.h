#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


void uart3_init(u32 bound);					//串口1初始化函数
void USART3_IRQHandler(void);     	//串口1中断服务程序

void USART3_Send_String(char *String);
#endif


