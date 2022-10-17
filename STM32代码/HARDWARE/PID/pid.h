/**
  *************************************************************************************************************************
  * @file    pid.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   pid�㷨.h�ļ�����
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

/* ���� -----------------------------------------------------------------------------------------------------------------*/

typedef struct
{
	float Servo_Position_Kp;//���λ�û�KP
	float Servo_Position_Ki;//���λ�û�Ki
	float Servo_Position_Kd;//���λ�û�Kd

	float Motor1_Velocity_Kp;	     		//�ٶȻ�KP
	float Motor1_Velocity_Ki;       		//�ٶȻ�KI
	float Motor1_Velocity_Kd;       		//�ٶȻ�KD
	float Motor1_Velocity_Target_Val;  //����ֵ
	float Motor1_Velocity_Actual_Val;  //ʵ��ֵ
	int 	Motor1_Velocity_Out;      		//�ٶȻ��������	
	
	float Motor2_Velocity_Kp;	     		//�ٶȻ�KP
	float Motor2_Velocity_Ki;       		//�ٶȻ�KI
	float Motor2_Velocity_Kd;       		//�ٶȻ�KD
	float Motor2_Velocity_Target_Val;  //����ֵ
	float Motor2_Velocity_Actual_Val;  //ʵ��ֵ
	int 	Motor2_Velocity_Out;      		//�ٶȻ��������	
	
}PID_InitTypedef;


/* �ṩ������C�ļ����õĺ��� --------------------------------------------------------------------------------------------*/
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
