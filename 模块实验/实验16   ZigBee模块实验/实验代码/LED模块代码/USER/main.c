#include "stm32f1xx.h"
#include "delay.h"
#include "led.h"
#include "rs485.h"
#include "usart.h"

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
    UART1_Init(115200);
    Rs485_Init();
    Rs485_Config(0);
    
	while(1)
	{
        if(GetMsg == 1)
        {
            if(strstr((const char*)USART1_RX_BUF,(const char*)"led1,toggle"))
            {
                HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0); 	//LED1对应引脚PB0取反，亮/灭	            
            }
            else if(strstr((const char*)USART1_RX_BUF,(const char*)"led2,toggle"))
            {	
                HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);   //LED2对应引脚PB1取反，亮/灭	           
            }
            else if(strstr((const char*)USART1_RX_BUF,(const char*)"led3,toggle"))
            {	
                HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_13); 	//LED3对应引脚PB13取反，亮/灭           
            }
            else if(strstr((const char*)USART1_RX_BUF,(const char*)"led4,toggle"))
            {
                 HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);  //LED4对应引脚PB14取反，亮/灭             
            }            
            GetMsg        = 0;
            USART1_RX_STA = 0;
        }
	 }
}

