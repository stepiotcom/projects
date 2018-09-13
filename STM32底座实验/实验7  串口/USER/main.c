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
uint8_t tets[17] = "www.stepiot.com\r\n";

uint8_t Message[22] = "The Message you sent :";

uint8_t Enter[2] = "\r\n";

int main(void)
{
	unsigned char len = 0;
    
    HAL_Init();             //初始化HAL库

    LED_Init();				//初始化LED
    KEY_Init();				//初始化按键
    UART2_Init(9600);		//初始化串口
    delay_ms(200);	

	while(1)
	{

		if(KEY_Scan())  //任意按键按下，发送串口数据
		{
            while(1)
            {
                if(S1&S2&S3&S4) break;  //等待按键放开
            }
             HAL_UART_Transmit(&UART2_Handler,tets,17,1000);	                //发送接收到的数据
                while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);	//等待发送结束
        }
         if(USART2_RX_STA&0x8000)
		{					   
			len=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度
            
			HAL_UART_Transmit(&UART2_Handler,Message,22,1000);
            
			HAL_UART_Transmit(&UART2_Handler,(uint8_t*)USART2_RX_BUF,len,1000);	//发送接收到的数据
            
			while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
            
			HAL_UART_Transmit(&UART2_Handler,Enter,2,1000);//插入换行
            
			USART2_RX_STA=0;
		}

	 }
}

