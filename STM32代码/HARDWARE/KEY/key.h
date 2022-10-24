/**
  *************************************************************************************************************************
  * @file    key.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   ����ģ��.h�ļ�����
  *************************************************************************************************************************
  * @attention
  * 
  * 
  * 
  *************************************************************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------------------------------------------------*/
#ifndef __key_H
#define	__key_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h" 


/* ���� -----------------------------------------------------------------------------------------------------------------*/
#define kEY1 			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define kEY2 			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define kEY3 			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)

#define BEEP_ON  		GPIO_ResetBits(GPIOA,GPIO_Pin_4)//��ȡ����1
#define BEEP_OFF  	GPIO_SetBits(GPIOA,GPIO_Pin_4)//��ȡ����1


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) {p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */
#define LED_TOGGLE		   digitalToggle(GPIOC,GPIO_Pin_13)
#define LED_OFF		   		 digitalHi(GPIOC,GPIO_Pin_13)
#define LED_ON			     digitalLo(GPIOC,GPIO_Pin_13)

/* �ṩ������C�ļ����õĺ��� --------------------------------------------------------------------------------------------*/
void LED_BEEP_GPIO_Config(void);
void EXTIX_Init(void);

#endif /* __XXX_H */

/*****************************************************END OF FILE*********************************************************/	


