/**
  *************************************************************************************************************************
  * @file    pid.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   pid�㷨.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "pid.h"
#include "control.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/
PID_InitTypedef PID;




/**
 * ������:PID_Para_Init
 * ����:PID������ʼ��
 * ����:��
 * ���:��
 */
void PID_Param_Init(void)
{
	/*λ��ʽPID-���*/
  PID.Servo_Position_Kp=-9.0; 
	PID.Servo_Position_Ki=0.0;
	PID.Servo_Position_Kd=-4.0;
}





/**
 * ������:Position_PID_Servo
 * ����:���λ��ʽPID�㷨ʵ��
 * ����:Target_Val:Ŀ��Ƕ�
 * ���:PID.Location_Out:λ�û�PID���ֵ
 */
float Position_PID_Servo(float Target_Val)
{
	static float  Error,Position_PID_OUT,Integral_Error,Error_Last,Actual_Val,Capture_Val;

	//1.��ȡ��ǰ���벶���ֵ	
	Capture_Val=TIM_GetCapture1(TIM3);
	//2.���㵱ǰ�Ƕ�
	Actual_Val=((1950.0-Capture_Val)*9)/10;	
	
	/**/
	#if defined(PID_ASSISTANT_EN)
  set_computer_value(SEND_FACT_CMD,CURVES_CH1, &Actual_Val, 1);    // ͬ����λ����������ť״̬
	set_computer_value(SEND_TARGET_CMD, CURVES_CH1,&Target_Val, 1); 
	#endif
	
  //3.����ƫ��
	Error=Target_Val-Actual_Val;
	//4.�ۼ�ƫ��
	Integral_Error+=Error;
	//5.PID�㷨ʵ��
	Position_PID_OUT=PID.Servo_Position_Kp*Error/100+PID.Servo_Position_Ki*Integral_Error/100+PID.Servo_Position_Kd*(Error-Error_Last)/100;
  //6.������һ�����
	Error_Last=Error;
	//7.���ؿ��������ֵ
	return Position_PID_OUT;
}

/**
 * ������:set_p_i_d
 * ����:����pid�������
 * ����:choose-ѡ�������p-���ñ���ϵ�� i-���û���ϵ�� d-����΢��ϵ��
 * ���:��
 */
void set_p_i_d(uint8_t choose, float Val_Kp, float Val_Ki, float Val_Kd)
{
	if(0==choose)
	{
		PID.Servo_Position_Kp=-Val_Kp;//���ñ���ϵ��
		PID.Servo_Position_Ki=Val_Ki;//���û���ϵ��
		PID.Servo_Position_Kd=Val_Kd;//����΢��ϵ��
	}
	else if(1==choose)
	{
//		PID.Velocity_Kp=Val_Kp;//���ñ���ϵ��
//		PID.Velocity_Ki=Val_Ki;//���û���ϵ��
//		PID.Velocity_Kd=Val_Kd;//����΢��ϵ��
	}
}

/**
 * ������:get_pid_target
 * ����:��ȡĿ��ֵ
 * ����:choose-ѡ�����
 * ���:��Ӧ��������ֵ
 */
float get_pid_target(uint8_t choose)
{

	return 0;
}



/**
 * ������:set_pid_target
 * ����:����Ŀ��ֵ
 * ����:choose-ѡ�����,vaL-Ŀ��ֵ
 * ���:��
 */
void set_pid_target(uint8_t choose,float val_temp)
{

}



/*****************************************************END OF FILE*********************************************************/	
