/**
  *************************************************************************************************************************
  * @file    control.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   ���ƺ���.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "control.h"
/* ���� -----------------------------------------------------------------------------------------------------------------*/
Param_InitTypedef Param;
Flag_InitTypedef  Flag;

float pitch,roll,yaw;//ŷ����
#define T 0.158f
#define L 0.1545f


/**
 * ������:EXTI9_5_IRQHandler
 * ����:�������ж�����10ms��ʱ
 * ����:��
 * ���:��
 */
void EXTI9_5_IRQHandler(void)
{
	static uint16_t Time_Cnt,Timer_Cnt;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//һ���ж�
	{
		if(PBin(5)==0)//�����ж�
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//����жϱ�־λ		

			//1.��Ҫ����������ʱ��ȡ��ǰ�Ƕ�
			if((Flag.Is_Go_straight==1)||(Flag.Is_Turn_Car==1))
			{
			 if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0){}				 
			}	
			//2.��ȡ��λʱ����������
			Param.UnitTime_Motor1Pluse=(short)Read_Speed(2);
			Param.UnitTime_Motor2Pluse=(short)Read_Speed(4);	

			Position_PID_Servo_Realize();//���PID����		
			Param.Motor1_PWM_Out=VelocityRing_MOTOR1_Control();//���1PID����
			Param.Motor2_PWM_Out=VelocityRing_MOTOR2_Control();//���2PID����		
			Limit(&Param.Motor2_PWM_Out,&Param.Motor1_PWM_Out,&Param.Servo_Target_Position);//�޷�
			Load(Param.Motor2_PWM_Out,Param.Motor1_PWM_Out,Param.Servo_Target_Position);//װ��			

			if(++Time_Cnt==10)//���ƣ���������Ƿ�λ�ɹ�
			{
				Time_Cnt=0;
				LED_TOGGLE;
			}		

			if(Flag.Start_Count==1) //��������
			{
				if(++Timer_Cnt==Param.Timer_threshold_value)//��ʱʱ�䵽
				{
					Timer_Cnt=0;//�������ֵ
					LED_TOGGLE;//led��ת
					Flag.Is_Timer_Up=1;//��ʱʱ�䵽��־λ��1
				}
			}					
		}
	}
}



/**
 * ������:EXTI0_IRQHandler
 * ����:�ⲿ�ж�-KEY1�������
 * ����:��
 * ���:��
 */
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(kEY1==1)	 //����KEY1
	{				
		Param.ModeChoose=BACK_PACKING;//ֻ���е��������� 
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //���LINE10�ϵ��жϱ�־λ  
}


/**
 * ������:EXTI4_IRQHandler
 * ����:�ⲿ�ж�-KEY1�������
 * ����:��
 * ���:��
 */
void EXTI4_IRQHandler(void)
{
	delay_ms(10);//����
	if(kEY2==1)	 //����KEY2
	{				
		Param.ModeChoose=SIDE_PACKING;//ֻ���в෽������  
	}
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE10�ϵ��жϱ�־λ  
}


/**
 * ������:EXTI15_10_IRQHandler
 * ����:�ⲿ�ж�-KEY3�������
 * ����:��
 * ���:��
 */
void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);//����
	if(kEY3==1)	 //����KEY3
	{				
		Param.ModeChoose=BACK_SIDE_PACKING;//���������������Ͳ෽ͣ������  
	}
	EXTI_ClearITPendingBit(EXTI_Line12);  //���LINE10�ϵ��жϱ�־λ  
}

/**
 * ������:Control_Proc
 * ����:�߼����ƺ���-���Ĳ���
 * ����:��
 * ���:��
 */
void Control_Proc(void)
{
	/**************************�������****************************/
	if (Param.ModeChoose==BACK_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://С��ֱ�У�ͬʱ��⴮�����ݵȴ�ͣ��
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(60,60,yaw);//С���˶�											
								openMv_Proc();//��⴮����Ϣ--�ȴ�ͣ��
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//��ִ��ֱ�к���
									Flag.Is_Stop_Car=1;//��ʼͣ��					
									Flag.Run_Step=2;//��ת��һ��							  
								}
							}
			break;		
			case 2: //ͣ��1s,��������1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ����
								Param.Timer_threshold_value=100;//�������ڶ�Ϊ1s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//ȡ��ͣ��
								Flag.Is_Start_Astern=1;//��ʼ������һ��
								Flag.Run_Step=3;//��ת��һ��
							}	
			break;	
			case 3://���ٺ��ˣ���ʱ0.72s
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-60,-60,SERVO_INIT);								
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=72;//�������ڶ�Ϊ0.72s
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=4;//��ת��һ��
						}								
				break;
							
			case 4://��ת60�㣬����������ת��
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-20,-100,SERVO_INIT+(60.0*5));								
							Flag.Is_Turn_Car=1;//��ʼת��
						}
						if(abs((int)yaw)==80)//�Ƕ�Ϊ-80�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=2;//��ʼ�����ڶ���
								Flag.Run_Step=5;//��ת��һ��
						}	
			break;
			case 5://��ͷ���������������٣���ʱ1.5s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,0.0);				
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=150;//��ʱ1.5s					
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=0;//���㵹������
								Flag.Is_Stop_Car=1;//ͣ��
								Flag.Run_Step=6;//��ת��һ��
						}	
			break;
			
			case 6://ͣ�����������죬��ʱ5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=500;//��ʱ5s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=7;//��ת��һ��
							}	
			break;
							/*����*/
			case 7://ֱ�У�����,��ʱ0.85s
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(100,100,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=85;//��ʱ0.85s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=8;//��ת��һ��
							}	
			break;
			case 8://��ת60�㣬���٣���ʱ1.5s
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,200,SERVO_INIT+(60.0*5));				
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=150;//�������ڶ�Ϊ1.5s					  
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=1;//ͣ��						
								Flag.Run_Step=9;//��ת��һ��
							}	
			break;
			case 9: //ͣ��
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
							}	      
			break;			
			default:break;
		}	 
	}
	/**************************�෽ͣ��****************************/
	else	if (Param.ModeChoose==SIDE_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://С��ֱ�У�ͬʱ��⴮�����ݵȴ�ͣ��
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//С���˶�											
								openMv_Proc();//��⴮����Ϣ--�ȴ�ͣ��
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//��ִ��ֱ�к���
									Flag.Is_Stop_Car=1;//��ʼͣ��					
									Flag.Run_Step=2;//��ת��һ��							  
								}
							}
			break;		
			case 2: //ͣ��1s,��������1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ����
								Param.Timer_threshold_value=100;//�������ڶ�Ϊ1s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//ȡ��ͣ��
								Flag.Is_Start_Astern=1;//��ʼ������һ��
								Flag.Run_Step=3;//��ת��һ��
							}	
			break;		
			case 3://��ת60�㣬����������ת��
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(0,-200,SERVO_INIT+(60.0*5));								
							Flag.Is_Turn_Car=1;//��ʼת��
						}
						if(abs((int)yaw)==40)//�Ƕ�Ϊ-40�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=2;//��ʼ�����ڶ���
								Flag.Run_Step=4;//��ת��һ��
						}	
			break;
			case 4://������������������ת�䣬��ʱ1s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);								
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=100;//��ʱ1s					
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=3;//���㵹������
								Flag.Run_Step=5;//��ת��һ��
						}							
			break;				
			case 5://��תת60�㣬����������ת��
						if((Flag.Is_Start_Astern==3)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-230,-100,SERVO_INIT+(-60.0*5));							
							Flag.Is_Turn_Car=1;//��ʼת��			
						}
							if(abs((int)yaw)==0)//��ʱʱ�䵽�ͽǶ�Ϊ0�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=0;//���㵹������
								Flag.Run_Step=6;//��ת��һ��
						}	
			break;

			case 6://ֱ�е�����λ��
							if((Flag.Is_Start_Astern==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(100,100,SERVO_INIT);	
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=100;//��ʱ1s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=1;//��ͣ��
								Flag.Run_Step=7;//��ת��һ��
							}	
			break;
						
			case 7://ͣ�����������������죬��ʱ5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=500;//��ʱ5s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=8;//��ת��һ��
							}	
			break;
							/*����*/
			case 8://����
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(-100,-100,SERVO_INIT);	
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=60;//��ʱ0.6s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=9;//��ת��һ��
							}	
			break;							
							
			case 9://��תת60�㣬ǰ��������ת��
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(200,0,SERVO_INIT+(-60.0*5));	
								Flag.Is_Turn_Car=1;//��ʼת��
							}
							if(abs((int)yaw)==40)//�Ƕ�Ϊ-40�����/*********/
							{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=10;//��ת��һ��
							}	
			break;
			case 10://������ֱ�У�������ת�䣬��ʱ0.8s
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
								{
									Kinematic_Analysis(100,100,0.0);					
									Flag.Start_Count=1;//��ʼ��ʱ
									Param.Timer_threshold_value=80;//��ʱ0.8s					
								}
								if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
								{
									Flag.Start_Count=0;//����������
									Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
									Flag.Run_Step=11;//��ת��һ��
								}				
			
			break;
			case 11: //��ת60�㣬ǰ��������ת��
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,200,SERVO_INIT+60.0*5);										
								Flag.Is_Turn_Car=1;//��ʼת��					  
							}
							if(abs((int)yaw)==0)//�Ƕ�Ϊ0�����
							{	
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=1;//ͣ��						
								Flag.Run_Step=12;//��ת��һ��
							}	      
			break;	
			case 12: //ͣ��
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
							}	      
			break;									
			default:break;
		}	 	 
	}
	/**************************�ȵ�����⣬��෽ͣ��****************************/
	else	if (Param.ModeChoose==BACK_SIDE_PACKING)
	{
		switch(Flag.Run_Step)
		{
			case 1://С��ֱ�У�ͬʱ��⴮�����ݵȴ�ͣ��
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//С���˶�													
								openMv_Proc();//��⴮����Ϣ--ͣ��
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//��ִ��ֱ�к���
									Flag.Is_Stop_Car=1;//��ʼͣ��					
									Flag.Run_Step=2;//��ת��һ��							  
								}
							}
			break;		
			case 2: //ͣ��1s,��������1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ����
								Param.Timer_threshold_value=100;//�������ڶ�Ϊ1s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//ȡ��ͣ��
								Flag.Is_Start_Astern=1;//��ʼ������һ��
								Flag.Run_Step=3;//��ת��һ��
							}	
			break;	
		  case 3 ://���ٺ��ˣ���ʱ0.72s
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);		
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=72;//�������ڶ�Ϊ0.72s
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=4;//��ת��һ��
						}						
				break;
							
			case 4://��ת60�㣬����������ת��
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-50,-150,SERVO_INIT+60.0*5);									
							Flag.Is_Turn_Car=1;//��ʼת��
						}
						if(abs((int)yaw)==80)//�Ƕ�Ϊ-80�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=2;//��ʼ�����ڶ���
								Flag.Run_Step=5;//��ת��һ��
						}	
			break;
			case 5://��ͷ���������������٣���ʱ1.5s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,0.0);				
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=150;//��ʱ1.5s					
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=0;//���㵹������
								Flag.Is_Stop_Car=1;//ͣ��
								Flag.Run_Step=6;//��ת��һ��
						}	
			break;
			
			case 6://ͣ�����������죬��ʱ5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=500;//��ʱ5s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=7;//��ת��һ��
							}	
			break;
							/*����*/
			case 7://ֱ�У�����,��ʱ0.85s
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(100,100,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=75;//��ʱ0.85s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=8;//��ת��һ��
							}	
			break;
			case 8://��ת60�㣬����
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(0,200,SERVO_INIT+60.0*5);				
								Flag.Is_Turn_Car=1;//��ʼת��			  
							}
							if(abs((int)yaw)==5)//�Ƕ�Ϊ0�����
							{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��		
								Flag.Is_Go_straight=1;//ִ��ֱ�к���
								Param.openMV_Data=0;//���¼��ͣ��
								Usart3_SendString("startcnt1");	//�л���ʶ����								
								Flag.Run_Step=9;//��ת��һ��
							}		
			break;
			case 9://С��ֱ�У�ͬʱ��⴮�����ݵȴ�ͣ��
							if(Flag.Is_Go_straight==1)
							{
								Kinematic_Analysis(100,100,yaw);//С���˶�					
								openMv_Proc();//��⴮����Ϣ--ͣ��
								if(Param.openMV_Data==1)
								{
									Flag.Is_Go_straight=0;//��ִ��ֱ�к���
									Flag.Is_Stop_Car=1;//��ʼͣ��					
									Flag.Run_Step=10;//��ת��һ��								
								}
							}			
			break;
			case 10: //ͣ��1s,��������1s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ����
								Param.Timer_threshold_value=100;//�������ڶ�Ϊ1s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//ȡ��ͣ��
								Flag.Is_Start_Astern=1;//��ʼ������һ��
								Flag.Run_Step=11;//��ת��һ��
							}									
			break;
			case 11://��ת60�㣬����������ת��
						if((Flag.Is_Start_Astern==1)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(0,-150,SERVO_INIT+60.0*5);							
							Flag.Is_Turn_Car=1;//��ʼת��
						}
						if(abs((int)yaw)==40)//�Ƕ�Ϊ-40�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=2;//��ʼ�����ڶ���
								Flag.Run_Step=12;//��ת��һ��
						}								
			break;
			case 12://������������������ת�䣬��ʱ1s
						if((Flag.Is_Start_Astern==2)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-100,-100,SERVO_INIT);								
							Flag.Start_Count=1;//��ʼ��ʱ
							Param.Timer_threshold_value=100;//��ʱ1s					
						}
						if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
						{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=3;//���㵹������
								Flag.Run_Step=13;//��ת��һ��
						}					
			break;
			case 13://��תת60�㣬����������ת��
						if((Flag.Is_Start_Astern==3)&&(Flag.Start_Count==0))
						{
							Kinematic_Analysis(-200,-100,SERVO_INIT+(-60.0*5));							
							Flag.Is_Turn_Car=1;//��ʼת��			
						}
							if(abs((int)yaw)==0)//��ʱʱ�䵽�ͽǶ�Ϊ0�����
						{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Start_Astern=0;//���㵹������
								Flag.Run_Step=14;//��ת��һ��
						}				
		  break;
			case 14://ֱ�е�����λ��
							if((Flag.Is_Start_Astern==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(100,100,SERVO_INIT);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=100;//��ʱ1s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=1;//��ͣ��
								Flag.Run_Step=15;//��ת��һ��
							}				
		  break;
			case 15://ͣ�����������������죬��ʱ5s
							if((Flag.Is_Stop_Car==1)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(0,0,0.0);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=500;//��ʱ5s
								BEEP_ON;//��������
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								BEEP_OFF;//�ط�����
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=16;//��ת��һ��
							}				
		  break;
							/*����*/
			case 16://����
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
							{
								Kinematic_Analysis(-100,-100,SERVO_INIT);
								Flag.Start_Count=1;//��ʼ��ʱ
								Param.Timer_threshold_value=60;//��ʱ0.6s
							}
							if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
							{
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=0;//��ͣ��
								Flag.Run_Step=17;//��ת��һ��
							}				
		  break;		
			case 17://��תת60�㣬ǰ��������ת��
							if(Flag.Is_Stop_Car==0)
							{
								Kinematic_Analysis(150,0,SERVO_INIT+(-60.0*5));									
								Flag.Is_Turn_Car=1;//��ʼת��
							}
							if(abs((int)yaw)==40)//�Ƕ�Ϊ-40�����/*********/
							{
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Run_Step=18;//��ת��һ��
							}									
		  break;	
			case 18://������ֱ�У�������ת�䣬��ʱ0.8s
							if((Flag.Is_Timer_Up==0)&&(Flag.Start_Count==0))
								{
									Kinematic_Analysis(100,100,0.0);					
									Flag.Start_Count=1;//��ʼ��ʱ
									Param.Timer_threshold_value=80;//��ʱ0.8s					
								}
								if(Flag.Is_Timer_Up==1)//��ʱʱ�䵽
								{
									Flag.Start_Count=0;//����������
									Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
									Flag.Run_Step=19;//��ת��һ��
								}											
		  break;	
			case 19://��ת60�㣬ǰ��������ת��
							if(Flag.Start_Count==0)
							{
								Kinematic_Analysis(100,150,SERVO_INIT+60.0*5);								
								Flag.Is_Turn_Car=1;//��ʼת��					  
							}
							if(abs((int)yaw)==0)//�Ƕ�Ϊ0�����
							{	
								Flag.Is_Turn_Car=0;//������ת��
								Flag.Start_Count=0;//����������
								Flag.Is_Timer_Up=0;//��ʱʱ�䵽��־λ����
								Flag.Is_Stop_Car=1;//ͣ��						
								Flag.Run_Step=20;//��ת��һ��
							}	   								
		  break;	
			case 20://ͣ��
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
 * ������:Position_PID_Servo_Realize
 * ����:���λ��ʽPIDʵ��
 * ����:��
 * ���:��
 */
void Position_PID_Servo_Realize(void)
{		
		Param.Servo_Speed	= Position_PID_Servo(Param.Servo_Target_Val);
		Param.Servo_Target_Position	+= Param.Servo_Speed;
}


/**
 * ������:Kinematic_Analysis
 * ����:С���˶���������
 * ����:velocity:���������ٶ�(velocity1=>���1 velocity2=>���2) angle:ǰ�ֵ�ǰת��
 * ���:��
 */
void Kinematic_Analysis(float velocity1,float velocity2,float angle)
{
	//1.�Ƕ��޷�,Ӳ����60��Ϊ���Ƕ�
	angle=angle>=60?angle=60:angle;
	angle=angle<=-60?angle=-60:angle;
	
	//2.ֱ�б�����������
	if((Flag.Is_Go_straight==1)||(Flag.Is_Start_Astern!=1))
	{
		PID.Motor1_Velocity_Target_Val=velocity1;
		PID.Motor2_Velocity_Target_Val=velocity2;
	}
	//3.�����������ֲ���
	else if (Flag.Is_Start_Astern==1)
	{
		PID.Motor1_Velocity_Target_Val=velocity1*(1-T*tan(angle)/2/L);
		PID.Motor2_Velocity_Target_Val=velocity1*(1+T*tan(angle)/2/L);	 
	}
	Param.Servo_Target_Val=SERVO_INIT+angle*5;
}


/*****************************************************END OF FILE*********************************************************/	











