#include "stm32f1xx.h"
#include "delay.h"
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
    HAL_Init();             //初始化HAL库    

    LED_Init();				//初始化LED	

	while(1)
	{
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); 	//LED1对应引脚PB0拉低，亮，等同于LED1(0)	
        delay_ms(1000);		
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);   //LED2对应引脚PB1拉低，亮，等同于LED2(0)
        delay_ms(1000);			
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET); 	//LED3对应引脚PB13拉低，亮，等同于LED3(0)	
        delay_ms(1000);	    
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);  //LED4对应引脚PB14拉低，亮，等同于LED04(0)	

        delay_ms(1000);											//延时500ms

        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); 	//LED1对应引脚PB0拉高，灭，等同于LED1(1)
        delay_ms(1000);			
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);   //LED2对应引脚PB1拉高，灭，等同于LED2(1)
        delay_ms(1000);	
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET); 	//LED3对应引脚PB2拉高，灭，等同于LED3(1)
        delay_ms(1000);	
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);  //LED4对应引脚PB3拉高，灭，等同于LED4(1)
        delay_ms(1000); 
	 }
}

