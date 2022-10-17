/**
  *************************************************************************************************************************
  * @file    pid.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   pid算法.h文件配置
  *************************************************************************************************************************
  * @attention
  * 
  * 
  * 
  *************************************************************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------------------------------------------------*/
#ifndef __PID_H 
#define	__PID_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"

/* 定义 -----------------------------------------------------------------------------------------------------------------*/

typedef struct
{
	float Servo_Position_Kp;//舵机位置环KP
	float Servo_Position_Ki;//舵机位置环Ki
	float Servo_Position_Kd;//舵机位置环Kd

	float Motor1_Velocity_Kp;	     		//速度环KP
	float Motor1_Velocity_Ki;       		//速度环KI
	float Motor1_Velocity_Kd;       		//速度环KD
	float Motor1_Velocity_Target_Val;  //期望值
	float Motor1_Velocity_Actual_Val;  //实际值
	int 	Motor1_Velocity_Out;      		//速度环输出变量	
	
	float Motor2_Velocity_Kp;	     		//速度环KP
	float Motor2_Velocity_Ki;       		//速度环KI
	float Motor2_Velocity_Kd;       		//速度环KD
	float Motor2_Velocity_Target_Val;  //期望值
	float Motor2_Velocity_Actual_Val;  //实际值
	int 	Motor2_Velocity_Out;      		//速度环输出变量	
	
}PID_InitTypedef;


/* 提供给其他C文件调用的函数 --------------------------------------------------------------------------------------------*/
extern PID_InitTypedef PID;
void   PID_Param_Init(void);
float  Position_PID_Servo(float Target_Val);
float VelocityRing_PID_Motor1_Realize(float Velocity_Actual_Val);
float VelocityRing_PID_Motor2_Realize(float Velocity_Actual_Val);



void set_p_i_d(uint8_t choose, float Val_Kp, float Val_Ki, float Val_Kd);
float get_pid_target(uint8_t choose);
void set_pid_target(uint8_t choose,float val_temp);

#endif /* __PID_H */

/*****************************************************END OF FILE*********************************************************/	
