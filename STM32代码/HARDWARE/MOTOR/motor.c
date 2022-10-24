/**
  *************************************************************************************************************************
  * @file    motor.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   xx模块.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "motor.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
/**
 * 函数名:
 * 描述:
 * 输入:
 * 输出:
 */
/*电机初始化函数*/
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//开启时钟
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//初始化GPIO--PB12、PB13、PB14、PB15为推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12 |GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
}

/*限幅函数*/
void Limit(int *motoA,int *motoB,float *servo)
{
	if(*motoA>=6500)*motoA=6500;
	if(*motoA<=-6500)*motoA=-6500;
	
	if(*motoB>=6500)*motoB=6500;
	if(*motoB<=-6500)*motoB=-6500;

	if(*servo>=1900)*servo=1900;
	if(*servo<=1800)*servo=1800;	
}

/*绝对值函数*/
int abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*赋值函数*/
/*入口参数：PID运算完成后的最终PWM值*/
void Load(int moto2,int moto1,uint16_t Target_Position)//moto1=-200：反转200个脉冲
{
	//1.研究正负号，对应正反转
	if(moto1>0)	Ain1=1,Ain2=0;//正转
	else 				Ain1=0,Ain2=1;//反转
	
	if(moto2>0)	Bin1=1,Bin2=0;
	else 				Bin1=0,Bin2=1;	
	
	//2.装载PWM值
	TIM_SetCompare1(TIM1,abs(moto1));
	TIM_SetCompare4(TIM1,abs(moto2));
	
	//3.装载舵机PWM值
	TIM_SetCompare1(TIM3,Target_Position);
}


/**
 * 函数名:set_motor_enable
 * 描述:使能电机
 * 输入:无
 * 输出:无
 */
void set_motor_enable(void)  
{
	TIM_Cmd(TIM1,ENABLE); //使能定时器1
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Enable);
}

/**
 * 函数名:set_motor_disable
 * 描述:失能电机
 * 输入:无
 * 输出:无
 */
void set_motor_disable(void)
{
	TIM_Cmd(TIM1,DISABLE); //失能定时器1
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
	TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Disable);
}


/*****************************************************END OF FILE*********************************************************/	
