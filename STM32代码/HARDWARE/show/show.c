/**
  *************************************************************************************************************************
  * @file    show.c
  * @author  amkl
  * @version V1.0
  * @date    2022-09-22
  * @brief   oled��ʾ.c�ļ�����
  *************************************************************************************************************************
  * @attention
  *
  *
  *************************************************************************************************************************
  */

/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "show.h"
#include "sys.h" 
/* ���� -----------------------------------------------------------------------------------------------------------------*/
int Encoder_Left1,Encoder_Right1;
/**
 * ������:
 * ����:
 * ����:
 * ���:
 */
void Oled_Proc(void)
{
	OLED_ShowString(0,0,"    Car   ",16 );

		//��������ʾ���ֱ�����//
	OLED_ShowString(0,6,"Enco--M1:",12);
	if( Param.UnitTime_Motor1Pluse<0)
	{
		Encoder_Left1=-Param.UnitTime_Motor1Pluse;
	  OLED_ShowString(62+10,6,"-",12);
		if(Encoder_Left1<=9)
		{
		 OLED_ShowNumber(70+10,6,Encoder_Left1,1,12);
		 OLED_ShowString(76+10,6,"    ",12);
		}	
		else if((Encoder_Left1>9)&&(Encoder_Left1<=99))
		{
		 OLED_ShowNumber(70+10,6,Encoder_Left1,2,12);
		 OLED_ShowString(82+10,6,"    ",12);
		}
		else if(Encoder_Left1>99)
		{
		 OLED_ShowNumber(70+10,6,Encoder_Left1,3,12);
		}
	}
	else
	{
	  OLED_ShowString(62+10,6,"+",12);
		if(Param.UnitTime_Motor1Pluse<=9)
		{
		 OLED_ShowNumber(70+10,6,Param.UnitTime_Motor1Pluse,1,12);
			OLED_ShowString(76+10,6,"    ",12);
		}	
		else if((Param.UnitTime_Motor1Pluse>9)&&(Param.UnitTime_Motor1Pluse<=99))
		{
		 OLED_ShowNumber(70+10,6,Param.UnitTime_Motor1Pluse,2,12);
		 OLED_ShowString(82+10,6,"    ",12);
		}
		else if(Param.UnitTime_Motor1Pluse>99)
		{
		 OLED_ShowNumber(70+10,6,Param.UnitTime_Motor1Pluse,3,12);
		}
	}
	//��������ʾ���ֱ�����//
		OLED_ShowString(0,7,"Enco---M2:",12);
	if( Param.UnitTime_Motor2Pluse<0)
	{
		Encoder_Right1=-Param.UnitTime_Motor2Pluse;
	  OLED_ShowString(62+15,7,"-",12);
		if(Encoder_Right1<=9)
		{
		 OLED_ShowNumber(70+15,7,Encoder_Right1,1,12);
		 OLED_ShowString(76+15,7,"    ",12);
		}	
		else if((Encoder_Right1>9)&&(Encoder_Right1<=99))
		{
		 OLED_ShowNumber(70+15,7,Encoder_Right1,2,12);		
		 OLED_ShowString(82+15,7,"    ",12);
		}
		else if(Encoder_Right1>99)
		{
		 OLED_ShowNumber(70+15,7,Encoder_Right1,3,12);
		}
	}
	else
	{
	  OLED_ShowString(62+15,7,"+",12);
		if(Param.UnitTime_Motor2Pluse<=9)
		{
		 OLED_ShowNumber(70+15,7,Param.UnitTime_Motor2Pluse,1,12);
		 OLED_ShowString(76+15,7,"    ",12);
		}	
		else if((Param.UnitTime_Motor2Pluse>9)&&(Param.UnitTime_Motor2Pluse<=99))
		{
		 OLED_ShowNumber(70+15,7,Param.UnitTime_Motor2Pluse,2,12);
		 OLED_ShowString(82+15,7,"    ",12);
		}
		else if(Param.UnitTime_Motor2Pluse>99)
		{
		 OLED_ShowNumber(70+15,7,Param.UnitTime_Motor2Pluse,3,12);
		}
	}	
}

/*****************************************************END OF FILE*********************************************************/	
