/**
  *************************************************************************************************************************
  * @file    servo.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   舵机模块.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "servo.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/


/**
 * 函数名:TIM3_PWM_Init
 * 描述:定时器3初始化
 * 输入:Arr:周期  Psc:预分频值
 * 输出:无
 */
void TIM3_PWM_Init(uint16_t Arr,uint16_t Psc)
{
	 GPIO_InitTypeDef        GPIO_InitStructure;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	 TIM_OCInitTypeDef       TIM_OCInitStructure;
		
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//CH1-PA6
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(GPIOA,&GPIO_InitStructure);  

	 TIM_TimeBaseStructure.TIM_ClockDivision=0;
	 TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	 TIM_TimeBaseStructure.TIM_Period=Arr;
	 TIM_TimeBaseStructure.TIM_Prescaler=Psc;
	 TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
 
   TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;
	 TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	 TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	 
	 TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	 TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);

	 TIM_ARRPreloadConfig(TIM3,ENABLE);
	 TIM_Cmd(TIM3,ENABLE);
	 
	 TIM_SetCompare1(TIM3,1853);
}



/*****************************************************END OF FILE*********************************************************/	


