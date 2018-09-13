#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "TM1640.h"	
#include "SHT20.h"	
#include "delay.h"
#include "ADC.h"
#include "I2C.h"


/**
************************************************************
************************************************************
************************************************************
*
*	作者： 		重庆八城科技
*
*	日期： 		2018-01-01
*
*	版本： 		V1.0
*
*	说明： 		SHT20库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/

uint32_t Light_AD = 0;

int main(void)
{ 
  HAL_Init();             					//初始化HAL库  
	TM1640_Init();										//初始化TM1640
	MY_ADC_Init();										//初始化ADC
	MX_I2C1_Init();										//初始化I2C

	while(1)
	{
		SHT2x_GetTempHumi(); 															//获取温湿度值
		Light_AD = Get_Adc_Average(ADC_CHANNEL_0,50);  		//获取光照强度
		
		send_LED_Display(0xC0,Light_AD,3);								//显示光强
		HAL_Delay(1000);
			
		send_LED_Display(0xC0,g_sht2x_param.HUMI_HM ,2);  //显示湿度
		HAL_Delay(1000);
		
		send_LED_Display(0xC0,g_sht2x_param.TEMP_HM,1);  	//显示温度
		HAL_Delay(1000);
	}
}

