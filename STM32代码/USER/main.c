/**
  *************************************************************************************************************************
  * @file    main.c
  * @author  AMKL
  * @version V1.0
  * @date    2022-10-13
  * @brief   2022��ʮ�·�ȫ����ѧ��������ƾ���-�Զ�����ϵͳ
  *************************************************************************************************************************
  * @attention
  * 	
  *������1-�ҵ��      ������2-����    ����          MPU6050 	     KEY 	         SERVO   	 			 BEEP   
  *PA0/PA1---TIM2      PB6/PB7---TIM4    TXD-PB11      SCL-PB8       KEY1-PB0	     SigLine-PA6     SigLine-PA4   
  *���1               ���2             RXD-PB10      SDA-PB9	     KEY2-PB4						 		
  *PB14/PB15           PB12/PB13                       INT-PB5		   KEY3-PA12				 	
  *PWM1                PWM2																					
  *PA8                 PA11																									 
  *	
  *************************************************************************************************************************
  */ 

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h" 


int main(void)	
{
	delay_init();//�δ�ʱ����ʼ��
	NVIC_Config();//�ⲿ�ж�����
	Usart3_Init(115200);//����ͷ��Ӧ����3��ʼ��-115200-8-N-1	

	MPU_Init();//��ʼ��MPU6050
	while(mpu_dmp_init()){;}//DMP��ʼ��
	MPU6050_EXTI_Init();//MPU6050�ⲿ�жϳ�ʼ��	
			
	Motor_Init();//�����ʼ��
		
	PWM_Init_TIM1(0,7199);//10khzƵ���������
  TIM3_PWM_Init(TIM3_ARR,TIM3_PSC);//50hz ����20ms-�����ʼ��
	LED_BEEP_GPIO_Config();//LED��������ʼ��
	EXTIX_Init();//������ʼ��-�ⲿ�жϴ���
	BEEP_OFF;//��ʼ�������ر�
	
	PID_Param_Init();//pid������ʼ��		
	while(Flag.Run_Step == 0)//�ȴ���ʼ����ָ��
	{
		Kinematic_Analysis(0,yaw);//С��ֹͣ,������� 						
		while(Param.ModeChoose==0){;}//�ȴ��������£�ȷ������ģʽ
		Flag.Run_Step=1;//��ʼ����
		Flag.Is_Go_straight=1;//��ֱ��			
	}
	switch(Param.ModeChoose)
	{
		case BACK_PACKING:Usart3_SendString("startcnt1");break;	//����ģʽѡ�����������ʶ����	
		case SIDE_PACKING:Usart3_SendString("startcnt2");break; //����ģʽѡ�����෽ͣ��ʶ����	
		case BACK_SIDE_PACKING:Usart3_SendString("startcnt1");break;//����ģʽ��ѡ�����������ʶ����,�����ı�
		default:break;		
	}
  while(1)	
	{	
		 Control_Proc();		
	} 	
}
