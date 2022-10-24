/**
  *************************************************************************************************************************
  * @file    motor.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   xxģ��.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "motor.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/
/**
 * ������:
 * ����:
 * ����:
 * ���:
 */
/*�����ʼ������*/
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//����ʱ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//��ʼ��GPIO--PB12��PB13��PB14��PB15Ϊ�������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12 |GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
}

/*�޷�����*/
void Limit(int *motoA,int *motoB,float *servo)
{
	if(*motoA>=6500)*motoA=6500;
	if(*motoA<=-6500)*motoA=-6500;
	
	if(*motoB>=6500)*motoB=6500;
	if(*motoB<=-6500)*motoB=-6500;

	if(*servo>=1900)*servo=1900;
	if(*servo<=1800)*servo=1800;	
}

/*����ֵ����*/
int abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*��ֵ����*/
/*��ڲ�����PID������ɺ������PWMֵ*/
void Load(int moto2,int moto1,uint16_t Target_Position)//moto1=-200����ת200������
{
	//1.�о������ţ���Ӧ����ת
	if(moto1>0)	Ain1=1,Ain2=0;//��ת
	else 				Ain1=0,Ain2=1;//��ת
	
	if(moto2>0)	Bin1=1,Bin2=0;
	else 				Bin1=0,Bin2=1;	
	
	//2.װ��PWMֵ
	TIM_SetCompare1(TIM1,abs(moto1));
	TIM_SetCompare4(TIM1,abs(moto2));
	
	//3.װ�ض��PWMֵ
	TIM_SetCompare1(TIM3,Target_Position);
}


/**
 * ������:set_motor_enable
 * ����:ʹ�ܵ��
 * ����:��
 * ���:��
 */
void set_motor_enable(void)  
{
	TIM_Cmd(TIM1,ENABLE); //ʹ�ܶ�ʱ��1
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Enable);
}

/**
 * ������:set_motor_disable
 * ����:ʧ�ܵ��
 * ����:��
 * ���:��
 */
void set_motor_disable(void)
{
	TIM_Cmd(TIM1,DISABLE); //ʧ�ܶ�ʱ��1
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
	TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Disable);
}


/*****************************************************END OF FILE*********************************************************/	
