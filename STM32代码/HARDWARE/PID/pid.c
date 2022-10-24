/**
  *************************************************************************************************************************
  * @file    pid.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   pid算法.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "pid.h"
#include "control.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
PID_InitTypedef PID;




/**
 * 函数名:PID_Para_Init
 * 描述:PID参数初始化
 * 输入:无
 * 输出:无
 */
void PID_Param_Init(void)
{
	/*位置式PID-舵机*/
  PID.Servo_Position_Kp=-9.0; 
	PID.Servo_Position_Ki=0.0;
	PID.Servo_Position_Kd=-4.0;
	
	/*位置式PID-电机1*/
  PID.Motor1_Velocity_Kp=70.0; 
	PID.Motor1_Velocity_Ki=7.0;
	PID.Motor1_Velocity_Kd=0.0;
	/*位置式PID-电机2*/
  PID.Motor2_Velocity_Kp=70.0; 
	PID.Motor2_Velocity_Ki=7.0;
	PID.Motor2_Velocity_Kd=0.0;
}





/**
 * 函数名:Position_PID_Servo
 * 描述:舵机位置式PID算法实现
 * 输入:Target_Val:目标角度
 * 输出:PID.Location_Out:位置环PID输出值
 */
float Position_PID_Servo(float Target_Val)
{
	static float  Error,Position_PID_OUT,Integral_Error,Error_Last,Actual_Val,Capture_Val;

	//1.获取当前输入捕获的值	
	Capture_Val=TIM_GetCapture1(TIM3);
	//2.计算当前角度
	Actual_Val=((1950.0-Capture_Val)*9)/10;		
  //3.计算偏差
	Error=Target_Val-Actual_Val;
	//4.累计偏差
	Integral_Error+=Error;
	//5.PID算法实现
	Position_PID_OUT=PID.Servo_Position_Kp*Error/100+PID.Servo_Position_Ki*Integral_Error/100+PID.Servo_Position_Kd*(Error-Error_Last)/100;
  //6.更新上一次误差
	Error_Last=Error;
	//7.返回控制量输出值
	return Position_PID_OUT;
}

/**
 * 函数名:VelocityRing_PID_MOTOR1_Realize
 * 描述:电机1速度环PID实现
 * 输入:Velocity_Actual_Val:实际值
 * 输出:PWM_OUT:速度环PID输出值
 */
float VelocityRing_PID_MOTOR1_Realize(float Velocity_Actual_Val)
{
	static float  Error,Error_Last,Integral_Error,PWM_OUT;
  //1.计算偏差
	Error=PID.Motor1_Velocity_Target_Val-Velocity_Actual_Val;
	
	//2.消除机械误差
	if((Error<0.5f)&&(Error>-0.5f))//实际运行1分钟，观察目标与实际位置差为1个轮子的周长 
	{
	 Error=0.0f;
	}
	
	//3.累计偏差
	Integral_Error+=Error;
	
	//4.积分限幅 (限幅值的确定为：PID.Velocity_Ki*限幅值等于PWM输出最大值)
	Integral_Error=(Integral_Error>690)?Integral_Error=690:((Integral_Error<-690)?Integral_Error=-690:Integral_Error);
	
	//5.PID算法实现
	PWM_OUT=PID.Motor1_Velocity_Kp*Error+PID.Motor1_Velocity_Ki*Integral_Error+PID.Motor1_Velocity_Kd*(Error-Error_Last);

  //6.更新上一次误差
	Error_Last=Error;

	//7.返回位置环当前输出值
	return PWM_OUT;
}

/**
 * 函数名:VelocityRing_PID_MOTOR2_Realize
 * 描述:电机2速度环PID实现
 * 输入:Velocity_Actual_Val:实际值
 * 输出:PWM_OUT:速度环PID输出值
 */
float VelocityRing_PID_MOTOR2_Realize(float Velocity_Actual_Val)
{
	static float  Error,Error_Last,Integral_Error,PWM_OUT;
  //1.计算偏差
	Error=PID.Motor2_Velocity_Target_Val-Velocity_Actual_Val;
	
	//2.消除机械误差
	if((Error<0.5f)&&(Error>-0.5f))//实际运行1分钟，观察目标与实际位置差为1个轮子的周长 
	{
	 Error=0.0f;
	}
	
	//3.累计偏差
	Integral_Error+=Error;
	
	//4.积分限幅 (限幅值的确定为：PID.Velocity_Ki*限幅值等于PWM输出最大值)
	Integral_Error=(Integral_Error>690)?Integral_Error=690:((Integral_Error<-690)?Integral_Error=-690:Integral_Error);
	
	//5.PID算法实现
	PWM_OUT=PID.Motor2_Velocity_Kp*Error+PID.Motor2_Velocity_Ki*Integral_Error+PID.Motor2_Velocity_Kd*(Error-Error_Last);

  //6.更新上一次误差
	Error_Last=Error;

	//7.返回位置环当前输出值
	return PWM_OUT;
}

/**
 * 函数名:VelocityRing_MOTOR1_Control
 * 描述:速度环电机1输出
 * 输入:无
 * 输出:输出期望PWM
 */
float VelocityRing_MOTOR1_Control(void)
{
    float ExpectPWM = 0.0;//当前控制值
	
//	  PID.Motor1_Velocity_Actual_Val=((float)Param.UnitTime_Motor1Pluse*1000.0*60.0)/(ENCODER_TOTAL_RESOLUTION*REDUCTION_RATIO*VELOCITY_PID_PERIOD);	
		PID.Motor1_Velocity_Actual_Val=((float)Param.UnitTime_Motor1Pluse*60000.0)/15600;	
    ExpectPWM=VelocityRing_PID_MOTOR1_Realize(PID.Motor1_Velocity_Actual_Val);

	  return ExpectPWM;
}

/**
 * 函数名:VelocityRing_MOTOR2_Control
 * 描述:速度环电机2输出
 * 输入:无
 * 输出:输出期望PWM
 */
float VelocityRing_MOTOR2_Control(void)
{
    float ExpectPWM = 0.0;//当前控制值
	
	  PID.Motor2_Velocity_Actual_Val=((float)Param.UnitTime_Motor2Pluse*60000.0)/15600;
    ExpectPWM=VelocityRing_PID_MOTOR2_Realize(PID.Motor2_Velocity_Actual_Val);
	
	  return ExpectPWM;
}

/**
 * 函数名:set_p_i_d
 * 描述:设置pid各项参数
 * 输入:p-设置比例系数 i-设置积分系数 d-设置微分系数
 * 输出:无
 */
void set_p_i_d(PID_InitTypedef *PID, float Val_Kp, float Val_Ki, float Val_Kd)
{
		PID->Motor1_Velocity_Kp=Val_Kp;//设置比例系数
		PID->Motor1_Velocity_Ki=Val_Ki;//设置积分系数
		PID->Motor1_Velocity_Kd=Val_Kd;//设置微分系数
}

/**
 * 函数名:get_pid_target
 * 描述:获取目标值
 * 输入:choose-选择参数(0=>电机1 1=>电机2)
 * 输出:对应参数返回值
 */
float get_pid_target(void)
{
	 return PID.Motor1_Velocity_Target_Val;//返回电机1速度环的目标值
}



/**
 * 函数名:set_pid_target
 * 描述:设置目标值
 * 输入:vaL-目标值
 * 输出:无
 */
void set_pid_target(PID_InitTypedef *PID,float val_temp)
{
		PID->Motor1_Velocity_Target_Val=val_temp;//设置电机1目标速度
}



/*****************************************************END OF FILE*********************************************************/	
