#include "stm32f1xx.h"
#include "delay.h"
#include "led.h"
#include "key.h"
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
*	说明： 		按键函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/

uint8_t Message[21] = "This is 485 Message\r\n";

int main(void)
{
    
    HAL_Init();             //初始化HAL库

    LED_Init();				//初始化LED
    IO_Init();
    KEY_Init();				//初始化按键
    UART2_Init(9600);		//初始化串口
    UART1_Init(115200);		//初始化串口
    delay_ms(200);	
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);	//PA8拉低
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);	//PA8拉低

	while(1)
	{
              
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);	//PA8拉高  使能485发送
            delay_ms(10);
            
            HAL_UART_Transmit(&UART1_Handler,Message,21,1000);	//发送接收到的数据
            
            delay_ms(10);
            
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);	//PA8拉低   使能485处于接收状太
              
            delay_ms(1000);

	 }
}

