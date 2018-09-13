#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "TM1640.h"	
#include "SHT20.h"	
#include "delay.h"
#include "Rs485.h"
#include "Usart.h"
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

uint32_t Light_AD = 0;							//光照强度
uint8_t temp = 0 , humi = 0;				//温湿度

int main(void)
{ 
  HAL_Init();             					//初始化HAL库  
	TM1640_Init();										//初始化TM1640
	MY_ADC_Init();										//初始化ADC
	MX_I2C1_Init();										//初始化I2C
	UART1_Init(115200);								//初始化串口一
	Rs485_Init();											//初始化485

	while(1)
	{	
		SHT2x_GetTempHumi(); 															//获取温湿度值
		Light_AD = Get_Adc_Average(ADC_CHANNEL_0,50);  		//获取光照强度
		temp = g_sht2x_param.TEMP_HM;											//获取温度
		humi = g_sht2x_param.HUMI_HM;											//获取湿度
		
		send_LED_Display(0xC0,Light_AD,3);								//显示光强
		HAL_Delay(1000);
			
		send_LED_Display(0xC0,humi,2);  									//显示湿度
		HAL_Delay(1000);
		
		send_LED_Display(0xC0,temp,1);  									//显示温度
		HAL_Delay(1000);
		
		Rs485_Config(1);																	//开启485发送
		printf("wsd->gsm%d%d%d\r\n",temp,humi,Light_AD);
		delay_ms(20);
		Rs485_Config(0);																	//关闭485发送
	}
}





