/**
  *************************************************************************************************************************
  * @file    exti.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   MPU6050ģ��.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "exti.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/

/**
 * ������:
 * ����:
 * ����:
 * ���:
 */
void MPU6050_EXTI_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);//����ʱ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;/**��1��**///GPIO_Mode_AF_PP
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;//PB5����Ϊ��������
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line5;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}




/*****************************************************END OF FILE*********************************************************/	









