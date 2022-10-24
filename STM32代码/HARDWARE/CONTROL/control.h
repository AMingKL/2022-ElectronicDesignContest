/**
  *************************************************************************************************************************
  * @file    control.h
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   控制函数.h文件配置
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
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
#define BACK_PACKING 			1
#define SIDE_PACKING 			2
#define BACK_SIDE_PACKING 3

/* 编码器物理分辨率 */
#define ENCODER_RESOLUTION                     13
/* 经过倍频之后的总分辨率 */
#define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)  /* 4倍频后的总分辨率 */
/* 减速电机减速比 */
#define REDUCTION_RATIO  30
/* 定时器周期10ms */
#define VELOCITY_PID_PERIOD  10   

typedef struct{	
	float 		Servo_Speed;//舵机速度
	float 		Servo_Target_Position;//舵机目标位置
	float 		Servo_Target_Val;//舵机目标值
	int   		Motor1_PWM_Out;//电机1PWM输出
	int  			Motor2_PWM_Out;//电机2PWM输出	
	uint8_t 	openMV_Data;//openMV数据	
	uint16_t 	Timer_threshold_value;//定时阈值
	uint8_t 	ModeChoose;//模式选择
	
	short UnitTime_Motor1Pluse;//单位时间内电机1脉冲数
	short UnitTime_Motor2Pluse;//单位时间内电机2脉冲数	
}Param_InitTypedef;

typedef struct{	
 uint8_t Is_Go_straight;//是否直行
 uint8_t Is_Stop_Car;//是否停车
 uint8_t Is_Turn_Car;//是否转弯
 uint8_t Is_Timer_Up;//是否定时到
 uint8_t Is_Start_Astern;//是否开始倒车
 uint8_t Run_Step;//运行步骤
 uint8_t Start_Count;//开始计时	
}Flag_InitTypedef;

#define SERVO_INIT  85.0

/* 提供给其他C文件调用的函数 --------------------------------------------------------------------------------------------*/
extern Param_InitTypedef Param;
extern Flag_InitTypedef  Flag;
extern float 	pitch,roll,yaw; 		//欧拉角 			

void Control_Proc(void);
void Kinematic_Analysis(float velocity1,float velocity2,float angle);
void Position_PID_Servo_Realize(void);
void Key_Proc(void);

#endif /* __XXX_H */
/*****************************************************END OF FILE*********************************************************/	

