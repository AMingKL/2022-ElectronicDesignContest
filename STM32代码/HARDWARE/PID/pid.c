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
	
	/*λ��ʽPID-���1*/
  PID.Motor1_Velocity_Kp=70.0; 
	PID.Motor1_Velocity_Ki=7.0;
	PID.Motor1_Velocity_Kd=0.0;
	/*λ��ʽPID-���2*/
  PID.Motor2_Velocity_Kp=70.0; 
	PID.Motor2_Velocity_Ki=7.0;
	PID.Motor2_Velocity_Kd=0.0;
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
 * ������:VelocityRing_PID_MOTOR1_Realize
 * ����:���1�ٶȻ�PIDʵ��
 * ����:Velocity_Actual_Val:ʵ��ֵ
 * ���:PWM_OUT:�ٶȻ�PID���ֵ
 */
float VelocityRing_PID_MOTOR1_Realize(float Velocity_Actual_Val)
{
	static float  Error,Error_Last,Integral_Error,PWM_OUT;
  //1.����ƫ��
	Error=PID.Motor1_Velocity_Target_Val-Velocity_Actual_Val;
	
	//2.������е���
	if((Error<0.5f)&&(Error>-0.5f))//ʵ������1���ӣ��۲�Ŀ����ʵ��λ�ò�Ϊ1�����ӵ��ܳ� 
	{
	 Error=0.0f;
	}
	
	//3.�ۼ�ƫ��
	Integral_Error+=Error;
	
	//4.�����޷� (�޷�ֵ��ȷ��Ϊ��PID.Velocity_Ki*�޷�ֵ����PWM������ֵ)
	Integral_Error=(Integral_Error>690)?Integral_Error=690:((Integral_Error<-690)?Integral_Error=-690:Integral_Error);
	
	//5.PID�㷨ʵ��
	PWM_OUT=PID.Motor1_Velocity_Kp*Error+PID.Motor1_Velocity_Ki*Integral_Error+PID.Motor1_Velocity_Kd*(Error-Error_Last);

  //6.������һ�����
	Error_Last=Error;

	//7.����λ�û���ǰ���ֵ
	return PWM_OUT;
}

/**
 * ������:VelocityRing_PID_MOTOR2_Realize
 * ����:���2�ٶȻ�PIDʵ��
 * ����:Velocity_Actual_Val:ʵ��ֵ
 * ���:PWM_OUT:�ٶȻ�PID���ֵ
 */
float VelocityRing_PID_MOTOR2_Realize(float Velocity_Actual_Val)
{
	static float  Error,Error_Last,Integral_Error,PWM_OUT;
  //1.����ƫ��
	Error=PID.Motor2_Velocity_Target_Val-Velocity_Actual_Val;
	
	//2.������е���
	if((Error<0.5f)&&(Error>-0.5f))//ʵ������1���ӣ��۲�Ŀ����ʵ��λ�ò�Ϊ1�����ӵ��ܳ� 
	{
	 Error=0.0f;
	}
	
	//3.�ۼ�ƫ��
	Integral_Error+=Error;
	
	//4.�����޷� (�޷�ֵ��ȷ��Ϊ��PID.Velocity_Ki*�޷�ֵ����PWM������ֵ)
	Integral_Error=(Integral_Error>690)?Integral_Error=690:((Integral_Error<-690)?Integral_Error=-690:Integral_Error);
	
	//5.PID�㷨ʵ��
	PWM_OUT=PID.Motor2_Velocity_Kp*Error+PID.Motor2_Velocity_Ki*Integral_Error+PID.Motor2_Velocity_Kd*(Error-Error_Last);

  //6.������һ�����
	Error_Last=Error;

	//7.����λ�û���ǰ���ֵ
	return PWM_OUT;
}

/**
 * ������:VelocityRing_MOTOR1_Control
 * ����:�ٶȻ����1���
 * ����:��
 * ���:�������PWM
 */
float VelocityRing_MOTOR1_Control(void)
{
    float ExpectPWM = 0.0;//��ǰ����ֵ
	
//	  PID.Motor1_Velocity_Actual_Val=((float)Param.UnitTime_Motor1Pluse*1000.0*60.0)/(ENCODER_TOTAL_RESOLUTION*REDUCTION_RATIO*VELOCITY_PID_PERIOD);	
		PID.Motor1_Velocity_Actual_Val=((float)Param.UnitTime_Motor1Pluse*60000.0)/15600;	
    ExpectPWM=VelocityRing_PID_MOTOR1_Realize(PID.Motor1_Velocity_Actual_Val);

	  return ExpectPWM;
}

/**
 * ������:VelocityRing_MOTOR2_Control
 * ����:�ٶȻ����2���
 * ����:��
 * ���:�������PWM
 */
float VelocityRing_MOTOR2_Control(void)
{
    float ExpectPWM = 0.0;//��ǰ����ֵ
	
	  PID.Motor2_Velocity_Actual_Val=((float)Param.UnitTime_Motor2Pluse*60000.0)/15600;
    ExpectPWM=VelocityRing_PID_MOTOR2_Realize(PID.Motor2_Velocity_Actual_Val);
	
	  return ExpectPWM;
}

/**
 * ������:set_p_i_d
 * ����:����pid�������
 * ����:p-���ñ���ϵ�� i-���û���ϵ�� d-����΢��ϵ��
 * ���:��
 */
void set_p_i_d(PID_InitTypedef *PID, float Val_Kp, float Val_Ki, float Val_Kd)
{
		PID->Motor1_Velocity_Kp=Val_Kp;//���ñ���ϵ��
		PID->Motor1_Velocity_Ki=Val_Ki;//���û���ϵ��
		PID->Motor1_Velocity_Kd=Val_Kd;//����΢��ϵ��
}

/**
 * ������:get_pid_target
 * ����:��ȡĿ��ֵ
 * ����:choose-ѡ�����(0=>���1 1=>���2)
 * ���:��Ӧ��������ֵ
 */
float get_pid_target(void)
{
	 return PID.Motor1_Velocity_Target_Val;//���ص��1�ٶȻ���Ŀ��ֵ
}



/**
 * ������:set_pid_target
 * ����:����Ŀ��ֵ
 * ����:vaL-Ŀ��ֵ
 * ���:��
 */
void set_pid_target(PID_InitTypedef *PID,float val_temp)
{
		PID->Motor1_Velocity_Target_Val=val_temp;//���õ��1Ŀ���ٶ�
}



/*****************************************************END OF FILE*********************************************************/	
