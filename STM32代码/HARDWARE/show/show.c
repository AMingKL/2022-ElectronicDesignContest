/**
  *************************************************************************************************************************
  * @file    show.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   oled显示.c文件配置
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "show.h"
#include "sys.h" 
/* 定义 -----------------------------------------------------------------------------------------------------------------*/
int Encoder_Left1,Encoder_Right1;
/**
 * 函数名:
 * 描述:
 * 输入:
 * 输出:
 */
void Oled_Proc(void)
{
	OLED_ShowString(0,0,"    Car   ",16 );
	//第五行显示俯仰角角度//
	OLED_ShowString(0,5,"Angle:",12);
	if((int)yaw>=0)
	{
		OLED_ShowString(44+6,5,"+",12);
		if((int)yaw%1000>99)
		{
	   OLED_ShowNumber(48+10,5,(int)yaw%1000,3,12);
		 OLED_ShowString(68+10,5,"'",12);
		}
		else
		{
	   OLED_ShowNumber(48+10,5,(int)yaw%1000,2,12);
		 OLED_ShowString(60+10,5,"'  ",12);		
		}
	}
		else 		
	{
	  OLED_ShowString(44+6,5,"-",12);				
		if((7000-(int)(yaw)%1000)<7099)
		{
		 OLED_ShowNumber(48+10,5,(7000-(int)(yaw)%1000),2,12);
		 OLED_ShowString(60+10,5,"'  ",12);
		}
		else
		{
		 OLED_ShowNumber(48+10,5,(7000-(int)(yaw)%1000),3,12);
		 OLED_ShowString(68+10,5,"'",12);
		}
	}
}

/*****************************************************END OF FILE*********************************************************/	
