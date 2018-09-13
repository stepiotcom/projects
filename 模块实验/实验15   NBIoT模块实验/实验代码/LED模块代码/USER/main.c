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
            if(strstr((const char*)USART1_RX_BUF,(const char*)"led3,on"))
            {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET); 	//LED1对应引脚PB0取反，亮/灭	            
            } 
            if(strstr((const char*)USART1_RX_BUF,(const char*)"led3,off"))
            {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET); 	//LED1对应引脚PB0取反，亮/灭	            
            } 
            if(strstr((const char*)USART1_RX_BUF,(const char*)"led2,on"))
            {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); 	//LED1对应引脚PB0取反，亮/灭	            
            }             
            GetMsg        = 0;
            USART1_RX_STA = 0;
        }
	 }
}

