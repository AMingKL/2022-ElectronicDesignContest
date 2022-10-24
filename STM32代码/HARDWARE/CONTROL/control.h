/**
  *************************************************************************************************************************
  * @file    control.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   ���ƺ���.h�ļ�����
  *************************************************************************************************************************
  * @attention
  * 
  * 
  * 
  *************************************************************************************************************************
  */


/* Define to prevent recursive inclusion --------------------------------------------------------------------------------*/
#ifndef  _CONTROL_H
#define  _CONTROL_H

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h" 
/* ���� -----------------------------------------------------------------------------------------------------------------*/
#define BACK_PACKING 			1
#define SIDE_PACKING 			2
#define BACK_SIDE_PACKING 3

/* ����������ֱ��� */
#define ENCODER_RESOLUTION                     13
/* ������Ƶ֮����ֱܷ��� */
#define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)  /* 4��Ƶ����ֱܷ��� */
/* ���ٵ�����ٱ� */
#define REDUCTION_RATIO  30
/* ��ʱ������10ms */
#define VELOCITY_PID_PERIOD  10   

typedef struct{	
	float 		Servo_Speed;//����ٶ�
	float 		Servo_Target_Position;//���Ŀ��λ��
	float 		Servo_Target_Val;//���Ŀ��ֵ
	int   		Motor1_PWM_Out;//���1PWM���
	int  			Motor2_PWM_Out;//���2PWM���	
	uint8_t 	openMV_Data;//openMV����	
	uint16_t 	Timer_threshold_value;//��ʱ��ֵ
	uint8_t 	ModeChoose;//ģʽѡ��
	
	short UnitTime_Motor1Pluse;//��λʱ���ڵ��1������
	short UnitTime_Motor2Pluse;//��λʱ���ڵ��2������	
}Param_InitTypedef;

typedef struct{	
 uint8_t Is_Go_straight;//�Ƿ�ֱ��
 uint8_t Is_Stop_Car;//�Ƿ�ͣ��
 uint8_t Is_Turn_Car;//�Ƿ�ת��
 uint8_t Is_Timer_Up;//�Ƿ�ʱ��
 uint8_t Is_Start_Astern;//�Ƿ�ʼ����
 uint8_t Run_Step;//���в���
 uint8_t Start_Count;//��ʼ��ʱ	
}Flag_InitTypedef;

#define SERVO_INIT  85.0

/* �ṩ������C�ļ����õĺ��� --------------------------------------------------------------------------------------------*/
extern Param_InitTypedef Param;
extern Flag_InitTypedef  Flag;
extern float 	pitch,roll,yaw; 		//ŷ���� 			

void Control_Proc(void);
void Kinematic_Analysis(float velocity1,float velocity2,float angle);
void Position_PID_Servo_Realize(void);
void Key_Proc(void);

#endif /* __XXX_H */
/*****************************************************END OF FILE*********************************************************/	

