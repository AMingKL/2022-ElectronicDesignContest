/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "timer4.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/

/**
 * @brief  TIM4��ʼ������
 * @param  ��
 * @retval ��
 */
void Timer4_Init(void) 
{
	/* ����һ��TIM_InitTypeDef���͵Ľṹ�� */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
	/* ʹ��TIM4ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
	/* ��ʱ����ʼ�� */
	TIM_TimeBaseStructure.TIM_Period = 10000-1; 								  /* �Զ�װ��ֵΪ1000-1 */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;									  /* Ԥ��Ƶ��          */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* ����ģʽ ����     */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);						 
	
	/* TIM4Ԥ��Ƶ����:1MHZ,APB1��Ƶϵ��2��TIM4ʱ��Ϊ36MHzx2 = 72MHz */
	TIM_PrescalerConfig(TIM4,71, TIM_PSCReloadMode_Immediate);	
	
	/* ͨ�ö�ʱ��TIM4�ж�ʹ�� */
	TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);	
	
	/* ��ʱ��4ʹ�� */
	TIM_Cmd(TIM4, ENABLE);
}




/*****************************************************END OF FILE*********************************************************/	
