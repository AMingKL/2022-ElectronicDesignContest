/**
  *************************************************************************************************************************
  * @file    control.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   控制函数.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "control.h"
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
Param_InitTypedef Param;
Flag_InitTypedef  Flag;

float pitch,roll,yaw;//欧拉角
#define T 0.158f
#define L 0.1545f


/**
 * 函数名:EXTI9_5_IRQHandler
 * 描述:陀螺仪中断引脚10ms定时
 * 输入:无
 * 输出:无
 */
void EXTI9_5_IRQHandler(void)
{
	static uint16_t Time_Cnt,Timer_Cnt;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//一级判定
	{
		if(PBin(5)==0)//二级判定
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位		

			//1.需要开启陀螺仪时获取当前角度
			if((Flag.Is_Go_straight==1)||(Flag.Is_Turn_Car==1))
			{
			 if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0){}				 
			}	
			//2.获取单位时间内脉冲数
			Param.UnitTime_Motor1Pluse=(short)Read_Speed(2);
			Param.UnitTime_Motor2Pluse=(short)Read_Speed(4);	

			Position_PID_Servo_Realize();//舵机PID计算		
			Param.Motor1_PWM_Out=VelocityRing_MOTOR1_Control();//电机1PID计算
			Param.Motor2_PWM_Out=VelocityRing_MOTOR2_Control();//电机2PID计算		
			Limit(&Param.Motor2_PWM_Out,&Param.Motor1_PWM_Out,&Param.Servo_Target_Position);//限幅
			Load(Param.Motor2_PWM_Out,Param.Motor1_PWM_Out,Param.Servo_Target_Position);//装载			

			if(++Time_Cnt==10)//闪灯，检验程序是否复位成功
			{
				Time_Cnt=0;
				LED_TOGGLE;
			}		

			if(Flag.Start_Count==1) //开启计数
			{
				if(++Timer_Cnt==Param.Timer_threshold_value)//定时时间到
				{
					Timer_Cnt=0;//清零计数值
					LED_TOGGLE;//led翻转
					Flag.Is_Timer_Up=1;//定时时间到标志位置1
				}
			}					
		}
	}
}



/**
 * 函数名:EXTI0_IRQHandler
 * 描述:外部中断-KEY1服务程序
 * 输入:无
 * 输出:无
 */
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(kEY1==1)	 //按键KEY1
	{				
		Param.ModeChoose=BACK_PACKING;//只进行倒车入库操作 
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除LINE10上的中断标志位  
}


/**
 * 函数名:EXTI4_IRQHandler
 * 描述:外部中断-KEY1服务程序
 * 输入:无
 * 输出:无
 */
void EXTI4_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(kEY2==1)	 //按键KEY2
	{				
		Param.ModeChoose=SIDE_PACKING;//只进行侧方入库操作  
	}
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE10上的中断标志位  
}


/**
 * 函数名:EXTI15_10_IRQHandler
 * 描述:外部中断-KEY3服务程序
 * 输入:无
 * 输出:无
 */
void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(kEY3==1)	 //按键KEY3
	{				
		Param.ModeChoose=BACK_SIDE_PACKING;//进行连续倒车入库和侧方停车操作  
	}
	EXTI_ClearITPendingBit(EXTI_Line12);  //清除LINE10上的中断标志位  
}

/**
 * 函数名:Control_Proc
 * 描述:逻辑控制函数-核心部分
 * 输入:无
 * 输出:无
 */
void Control_Proc(void)
{
	/**************************倒车入库****************************/
	if (Param.ModeChoose==BACK_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://小车直行，同时检测串口数据等待停车
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(60,60,yaw);//小车运动											
								openMv_Proc();//检测串口信息--等待停车
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//不执行直行函数
									Flag.Is_Stop_Car=1;//开始停车					
									Flag.Run_Step=2;//跳转下一步							  
								}
							}
			break;		
			case 2: //停车1s,蜂鸣器响1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计数
								Param.Timer_threshold_value=100;//计数周期定为1s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//取消停车
								Flag.Is_Start_Astern=1;//开始倒车第一步
								Flag.Run_Step=3;//跳转下一步
							}	
			break;	
			case 3://匀速后退，定时0.72s
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-60,-60,SERVO_INIT);								
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=72;//计数周期定为0.72s
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=4;//跳转下一步
						}								
				break;
							
			case 4://右转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-20,-100,SERVO_INIT+(60.0*5));								
							Flag.Is_Turn_Car=1;//开始转弯
						}
						if(abs((int)yaw)==80)//角度为-80°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=2;//开始倒车第二步
								Flag.Run_Step=5;//跳转下一步
						}	
			break;
			case 5://车头归正，倒车，匀速，定时1.5s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,0.0);				
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=150;//定时1.5s					
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=0;//清零倒车步骤
								Flag.Is_Stop_Car=1;//停车
								Flag.Run_Step=6;//跳转下一步
						}	
			break;
			
			case 6://停车，蜂鸣器响，定时5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=500;//定时5s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=7;//跳转下一步
							}	
			break;
							/*出库*/
			case 7://直行，匀速,定时0.85s
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(100,100,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=85;//定时0.85s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=8;//跳转下一步
							}	
			break;
			case 8://右转60°，差速，定时1.5s
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,200,SERVO_INIT+(60.0*5));				
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=150;//计数周期定为1.5s					  
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=1;//停车						
								Flag.Run_Step=9;//跳转下一步
							}	
			break;
			case 9: //停车
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
							}	      
			break;			
			default:break;
		}	 
	}
	/**************************侧方停车****************************/
	else	if (Param.ModeChoose==SIDE_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://小车直行，同时检测串口数据等待停车
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//小车运动											
								openMv_Proc();//检测串口信息--等待停车
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//不执行直行函数
									Flag.Is_Stop_Car=1;//开始停车					
									Flag.Run_Step=2;//跳转下一步							  
								}
							}
			break;		
			case 2: //停车1s,蜂鸣器响1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计数
								Param.Timer_threshold_value=100;//计数周期定为1s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//取消停车
								Flag.Is_Start_Astern=1;//开始倒车第一步
								Flag.Run_Step=3;//跳转下一步
							}	
			break;		
			case 3://右转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(0,-200,SERVO_INIT+(60.0*5));								
							Flag.Is_Turn_Car=1;//开始转弯
						}
						if(abs((int)yaw)==40)//角度为-40°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=2;//开始倒车第二步
								Flag.Run_Step=4;//跳转下一步
						}	
			break;
			case 4://回正，倒车，不差速转弯，定时1s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);								
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=100;//定时1s					
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=3;//清零倒车步骤
								Flag.Run_Step=5;//跳转下一步
						}							
			break;				
			case 5://左转转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==3)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-230,-100,SERVO_INIT+(-60.0*5));							
							Flag.Is_Turn_Car=1;//开始转弯			
						}
							if(abs((int)yaw)==0)//定时时间到和角度为0°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=0;//清零倒车步骤
								Flag.Run_Step=6;//跳转下一步
						}	
			break;

			case 6://直行到中心位置
							if((Flag.Is_Start_Astern==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(100,100,SERVO_INIT);	
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=100;//定时1s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=1;//不停车
								Flag.Run_Step=7;//跳转下一步
							}	
			break;
						
			case 7://停车，回正，蜂鸣器响，定时5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=500;//定时5s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=8;//跳转下一步
							}	
			break;
							/*出库*/
			case 8://后退
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(-100,-100,SERVO_INIT);	
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=60;//定时0.6s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=9;//跳转下一步
							}	
			break;							
							
			case 9://左转转60°，前进，差速转弯
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(200,0,SERVO_INIT+(-60.0*5));	
								Flag.Is_Turn_Car=1;//开始转弯
							}
							if(abs((int)yaw)==40)//角度为-40°结束/*********/
							{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=10;//跳转下一步
							}	
			break;
			case 10://回正，直行，不差速转弯，定时0.8s
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
								{
									Kinematic_Analysis(100,100,0.0);					
									Flag.Start_Count=1;//开始计时
									Param.Timer_threshold_value=80;//定时0.8s					
								}
								if(Flag.Is_Timer_Up==1)//定时时间到
								{
									Flag.Start_Count=0;//不开启计数
									Flag.Is_Timer_Up=0;//定时时间到标志位清零
									Flag.Run_Step=11;//跳转下一步
								}				
			
			break;
			case 11: //右转60°，前进，差速转弯
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,200,SERVO_INIT+60.0*5);										
								Flag.Is_Turn_Car=1;//开始转弯					  
							}
							if(abs((int)yaw)==0)//角度为0°结束
							{	
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=1;//停车						
								Flag.Run_Step=12;//跳转下一步
							}	      
			break;	
			case 12: //停车
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
							}	      
			break;									
			default:break;
		}	 	 
	}
	/**************************先倒车入库，后侧方停车****************************/
	else	if (Param.ModeChoose==BACK_SIDE_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://小车直行，同时检测串口数据等待停车
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//小车运动													
								openMv_Proc();//检测串口信息--停车
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//不执行直行函数
									Flag.Is_Stop_Car=1;//开始停车					
									Flag.Run_Step=2;//跳转下一步							  
								}
							}
			break;		
			case 2: //停车1s,蜂鸣器响1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计数
								Param.Timer_threshold_value=100;//计数周期定为1s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//取消停车
								Flag.Is_Start_Astern=1;//开始倒车第一步
								Flag.Run_Step=3;//跳转下一步
							}	
			break;	
		  case 3 ://匀速后退，定时0.72s
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);		
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=72;//计数周期定为0.72s
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=4;//跳转下一步
						}						
				break;
							
			case 4://右转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-50,-150,SERVO_INIT+60.0*5);									
							Flag.Is_Turn_Car=1;//开始转弯
						}
						if(abs((int)yaw)==80)//角度为-80°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=2;//开始倒车第二步
								Flag.Run_Step=5;//跳转下一步
						}	
			break;
			case 5://车头归正，倒车，匀速，定时1.5s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,0.0);				
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=150;//定时1.5s					
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=0;//清零倒车步骤
								Flag.Is_Stop_Car=1;//停车
								Flag.Run_Step=6;//跳转下一步
						}	
			break;
			
			case 6://停车，蜂鸣器响，定时5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=500;//定时5s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=7;//跳转下一步
							}	
			break;
							/*出库*/
			case 7://直行，匀速,定时0.85s
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(100,100,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=75;//定时0.85s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=8;//跳转下一步
							}	
			break;
			case 8://右转60°，差速
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(0,200,SERVO_INIT+60.0*5);				
								Flag.Is_Turn_Car=1;//开始转弯			  
							}
							if(abs((int)yaw)==5)//角度为0°结束
							{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车		
								Flag.Is_Go_straight=1;//执行直行函数
								Param.openMV_Data=0;//重新检测停车
								Usart3_SendString("startcnt1");	//切换到识别函数								
								Flag.Run_Step=9;//跳转下一步
							}		
			break;
			case 9://小车直行，同时检测串口数据等待停车
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//小车运动					
								openMv_Proc();//检测串口信息--停车
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//不执行直行函数
									Flag.Is_Stop_Car=1;//开始停车					
									Flag.Run_Step=10;//跳转下一步								
								}
							}			
			break;
			case 10: //停车1s,蜂鸣器响1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计数
								Param.Timer_threshold_value=100;//计数周期定为1s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//取消停车
								Flag.Is_Start_Astern=1;//开始倒车第一步
								Flag.Run_Step=11;//跳转下一步
							}									
			break;
			case 11://右转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(0,-150,SERVO_INIT+60.0*5);							
							Flag.Is_Turn_Car=1;//开始转弯
						}
						if(abs((int)yaw)==40)//角度为-40°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=2;//开始倒车第二步
								Flag.Run_Step=12;//跳转下一步
						}								
			break;
			case 12://回正，倒车，不差速转弯，定时1s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);								
							Flag.Start_Count=1;//开始计时
							Param.Timer_threshold_value=100;//定时1s					
						}
						if(Flag.Is_Timer_Up==1)//定时时间到
						{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=3;//清零倒车步骤
								Flag.Run_Step=13;//跳转下一步
						}					
			break;
			case 13://左转转60°，倒车，差速转弯
						if((Flag.Is_Start_Astern==3)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-200,-100,SERVO_INIT+(-60.0*5));							
							Flag.Is_Turn_Car=1;//开始转弯			
						}
							if(abs((int)yaw)==0)//定时时间到和角度为0°结束
						{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Start_Astern=0;//清零倒车步骤
								Flag.Run_Step=14;//跳转下一步
						}				
		  break;
			case 14://直行到中心位置
							if((Flag.Is_Start_Astern==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(100,100,SERVO_INIT);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=100;//定时1s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=1;//不停车
								Flag.Run_Step=15;//跳转下一步
							}				
		  break;
			case 15://停车，回正，蜂鸣器响，定时5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=500;//定时5s
								BEEP_ON;//蜂鸣器响
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								BEEP_OFF;//关蜂鸣器
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=16;//跳转下一步
							}				
		  break;
							/*出库*/
			case 16://后退
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(-100,-100,SERVO_INIT);
								Flag.Start_Count=1;//开始计时
								Param.Timer_threshold_value=60;//定时0.6s
							}
							if(Flag.Is_Timer_Up==1)//定时时间到
							{
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=0;//不停车
								Flag.Run_Step=17;//跳转下一步
							}				
		  break;		
			case 17://左转转60°，前进，差速转弯
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(150,0,SERVO_INIT+(-60.0*5));									
								Flag.Is_Turn_Car=1;//开始转弯
							}
							if(abs((int)yaw)==40)//角度为-40°结束/*********/
							{
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Run_Step=18;//跳转下一步
							}									
		  break;	
			case 18://回正，直行，不差速转弯，定时0.8s
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
								{
									Kinematic_Analysis(100,100,0.0);					
									Flag.Start_Count=1;//开始计时
									Param.Timer_threshold_value=80;//定时0.8s					
								}
								if(Flag.Is_Timer_Up==1)//定时时间到
								{
									Flag.Start_Count=0;//不开启计数
									Flag.Is_Timer_Up=0;//定时时间到标志位清零
									Flag.Run_Step=19;//跳转下一步
								}											
		  break;	
			case 19://右转60°，前进，差速转弯
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,150,SERVO_INIT+60.0*5);								
								Flag.Is_Turn_Car=1;//开始转弯					  
							}
							if(abs((int)yaw)==0)//角度为0°结束
							{	
								Flag.Is_Turn_Car=0;//不开启转弯
								Flag.Start_Count=0;//不开启计数
								Flag.Is_Timer_Up=0;//定时时间到标志位清零
								Flag.Is_Stop_Car=1;//停车						
								Flag.Run_Step=20;//跳转下一步
							}	   								
		  break;	
			case 20://停车
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
							}	   								
		  break;				
			default:break;
		}	 
	}
}


/**
 * 函数名:Position_PID_Servo_Realize
 * 描述:舵机位置式PID实现
 * 输入:无
 * 输出:无
 */
void Position_PID_Servo_Realize(void)
{		
		Param.Servo_Speed	= Position_PID_Servo(Param.Servo_Target_Val);
		Param.Servo_Target_Position	+= Param.Servo_Speed;
}


/**
 * 函数名:Kinematic_Analysis
 * 描述:小车运动分析函数
 * 输入:velocity:输入期望速度(velocity1=>电机1 velocity2=>电机2) angle:前轮当前转角
 * 输出:无
 */
void Kinematic_Analysis(float velocity1,float velocity2,float angle)
{
	//1.角度限幅,硬件上60°为最大角度
	angle=angle>=60?angle=60:angle;
	angle=angle<=-60?angle=-60:angle;
	
	//2.直行保持俩轮匀速
	if((Flag.Is_Go_straight==1)||(Flag.Is_Start_Astern!=1))
	{
		PID.Motor1_Velocity_Target_Val=velocity1;
		PID.Motor2_Velocity_Target_Val=velocity2;
	}
	//3.倒车保持俩轮差速
	else if (Flag.Is_Start_Astern==1)
	{
		PID.Motor1_Velocity_Target_Val=velocity1*(1-T*tan(angle)/2/L);
		PID.Motor2_Velocity_Target_Val=velocity1*(1+T*tan(angle)/2/L);	 
	}
	Param.Servo_Target_Val=SERVO_INIT+angle*5;
}


/*****************************************************END OF FILE*********************************************************/	











