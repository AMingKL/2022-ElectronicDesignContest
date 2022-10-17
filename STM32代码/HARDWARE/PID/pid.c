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
	
	/**/
	#if defined(PID_ASSISTANT_EN)
  set_computer_value(SEND_FACT_CMD,CURVES_CH1, &Actual_Val, 1);    // 同步上位机的启动按钮状态
	set_computer_value(SEND_TARGET_CMD, CURVES_CH1,&Target_Val, 1); 
	#endif
	
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
 * 函数名:set_p_i_d
 * 描述:设置pid各项参数
 * 输入:choose-选择参数，p-设置比例系数 i-设置积分系数 d-设置微分系数
 * 输出:无
 */
void set_p_i_d(uint8_t choose, float Val_Kp, float Val_Ki, float Val_Kd)
{
	if(0==choose)
	{
		PID.Servo_Position_Kp=-Val_Kp;//设置比例系数
		PID.Servo_Position_Ki=Val_Ki;//设置积分系数
		PID.Servo_Position_Kd=Val_Kd;//设置微分系数
	}
	else if(1==choose)
	{
//		PID.Velocity_Kp=Val_Kp;//设置比例系数
//		PID.Velocity_Ki=Val_Ki;//设置积分系数
//		PID.Velocity_Kd=Val_Kd;//设置微分系数
	}
}

/**
 * 函数名:get_pid_target
 * 描述:获取目标值
 * 输入:choose-选择参数
 * 输出:对应参数返回值
 */
float get_pid_target(uint8_t choose)
{

	return 0;
}



/**
 * 函数名:set_pid_target
 * 描述:设置目标值
 * 输入:choose-选择参数,vaL-目标值
 * 输出:无
 */
void set_pid_target(uint8_t choose,float val_temp)
{

}



/*****************************************************END OF FILE*********************************************************/	
