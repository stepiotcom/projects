#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "delay.h"
#include "Usart.h"
#include "Rs485.h"	
#include "led.h"

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
*	说明： 		跑马灯库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/


int main(void)
{ 
	uint8_t fag = 0;  								//接收数据
	
  HAL_Init();             					//初始化HAL库  
	UART1_Init(115200);								//初始化串口
	Rs485_Init();   									//初始化485
  LED_Init();												//初始化LED	

	while(1)
	{
		fag = USART1_IRQHand(); 				//等待接收总线数据 返回一个标志位
		
		if(fag == 1)
		{
			/*		密码正确		*/
			LED_Config();  								//流水灯
		}
		if(fag == 2)
		{
			/*		密码错误		*/
			HAL_GPIO_WritePin(LED_GPIO_PORT,LED1_GPIO_PIN|LED2_GPIO_PIN|LED3_GPIO_PIN|LED4_GPIO_PIN,GPIO_PIN_SET);
			delay_ms(500);
			HAL_GPIO_WritePin(LED_GPIO_PORT,LED1_GPIO_PIN|LED2_GPIO_PIN|LED3_GPIO_PIN|LED4_GPIO_PIN,GPIO_PIN_RESET); 
			delay_ms(500);
		}  
	}
}

