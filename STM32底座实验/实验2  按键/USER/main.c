#include "stm32f1xx.h"
#include "delay.h"
#include "led.h"
#include "key.h"


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
*	说明： 		按键函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/


int main(void)
{ 
	unsigned char key = 0;
    
    HAL_Init();             //初始化HAL库    

    LED_Init();				//初始化LED	
    KEY_Init();				//初始化按键

	while(1)
	{
		
		key = KEY_Scan();  //按键值读取
		
		switch(key)
		{
			case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); break;  //LED1对应引脚PB0拉低，亮
			case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);  break;   //LED1对应引脚PB0拉高，灭
			case 3:
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); 	//LED1对应引脚PB0拉低，亮
					delay_ms(200);		
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);     //LED2对应引脚PB1拉低，亮
					delay_ms(200);			
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET); 	//LED3对应引脚PB13拉低，亮
					delay_ms(200);	    
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);    //LED4对应引脚PB14拉低，亮
				}
				break;
			case 4:
				{
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); 	//LED1对应引脚PB0拉高，灭	
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);   //LED2对应引脚PB1拉高，灭
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET); 	//LED3对应引脚PB2拉高，灭
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);  //LED4对应引脚PB3拉高，灭
				}
				break;
			default :
				break;
		}

	 }
}

