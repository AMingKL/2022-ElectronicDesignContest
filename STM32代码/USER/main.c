/**
  *************************************************************************************************************************
  * @file    main.c
  * @author  AMKL
  * @version V1.0
  * @date    2022-10-13
  * @brief   2022年十月份全国大学生电子设计竞赛-自动泊车系统
  *************************************************************************************************************************
  * @attention
  * 	
  *编码器1-右电机      编码器2-左电机    串口          MPU6050 	     KEY 	         SERVO   	 			 BEEP   
  *PA0/PA1---TIM2      PB6/PB7---TIM4    TXD-PB11      SCL-PB8       KEY1-PB0	     SigLine-PA6     SigLine-PA4   
  *电机1               电机2             RXD-PB10      SDA-PB9	     KEY2-PB4						 		
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
	delay_init();//滴答定时器初始化
	NVIC_Config();//外部中断配置
	Usart3_Init(115200);//摄像头对应串口3初始化-115200-8-N-1	

	MPU_Init();//初始化MPU6050
	while(mpu_dmp_init()){;}//DMP初始化
	MPU6050_EXTI_Init();//MPU6050外部中断初始化	
			
	Motor_Init();//电机初始化
		
	PWM_Init_TIM1(0,7199);//10khz频率驱动电机
  TIM3_PWM_Init(TIM3_ARR,TIM3_PSC);//50hz 周期20ms-舵机初始化
	LED_BEEP_GPIO_Config();//LED蜂鸣器初始化
	EXTIX_Init();//按键初始化-外部中断触发
	BEEP_OFF;//初始蜂鸣器关闭
	
	PID_Param_Init();//pid参数初始化		
	while(Flag.Run_Step == 0)//等待开始运行指令
	{
		Kinematic_Analysis(0,yaw);//小车停止,舵机归正 						
		while(Param.ModeChoose==0){;}//等待按键按下，确定运行模式
		Flag.Run_Step=1;//开始运行
		Flag.Is_Go_straight=1;//走直线			
	}
	switch(Param.ModeChoose)
	{
		case BACK_PACKING:Usart3_SendString("startcnt1");break;	//根据模式选择开启倒车入库识别函数	
		case SIDE_PACKING:Usart3_SendString("startcnt2");break; //根据模式选择开启侧方停车识别函数	
		case BACK_SIDE_PACKING:Usart3_SendString("startcnt1");break;//根据模式先选择开启倒车入库识别函数,后续改变
		default:break;		
	}
  while(1)	
	{	
		 Control_Proc();		
	} 	
}
