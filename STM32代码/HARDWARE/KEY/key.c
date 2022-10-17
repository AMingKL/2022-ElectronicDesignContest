/**
  *************************************************************************************************************************
  * @file    key.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   按键模块.c文件配置
  *************************************************************************************************************************
  * @attention  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "key.h" 
#include "delay.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/

/**
 * 函数名:LED_BEEP_GPIO_Config
 * 描述:led，beep-GPIO配置函数
 * 输入:无
 * 输出:无
 */
void LED_BEEP_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	  BEEP_OFF;
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

/**
 * 函数名:EXTIX_Init
 * 描述:按键外部中断配置函数
 * 输入:无
 * 输出:无
 */
void EXTIX_Init(void)
{ 
   	EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


   //GPIOA.9/10/12	  中断线以及中断初始化配置 上降沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);
	
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);
}





/*****************************************************END OF FILE*********************************************************/	

