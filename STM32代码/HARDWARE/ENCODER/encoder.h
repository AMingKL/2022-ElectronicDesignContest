/**
  *************************************************************************************************************************
  * @file    encoder.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   编码器模块.h文件配置
  *************************************************************************************************************************
  * @attention
  * 
  * 
  * 
  *************************************************************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------------------------------------------------*/
#ifndef _ENCODER_H
#define	_ENCODER_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h" 

/* 定义 -----------------------------------------------------------------------------------------------------------------*/

/* 提供给其他C文件调用的函数 --------------------------------------------------------------------------------------------*/
void TIM2_IRQHandler(void);
void TIM4_IRQHandler(void);
int Read_Speed(int TIMx);
void Encoder_TIM2_Init(void);
void Encoder_TIM4_Init(void);


#endif /* _ENCODER_H */

/*****************************************************END OF FILE*********************************************************/	

