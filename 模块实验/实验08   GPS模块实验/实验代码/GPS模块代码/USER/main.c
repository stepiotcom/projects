#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "delay.h"
#include "Usart.h"
#include "Rs485.h"	
#include "main.h"
#include "GPS.h"
#include "string.h"
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
*	说明： 		GPS库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/

GPS_MsgTypeDef GPS_GGA_Msg;
uint8_t GGA_MsgBuffer[200];
int main(void) 
{ 
  HAL_Init();             			//初始化HAL库  
	Rs485_Init();   							//初始化485
	UART1_Init(115200);						//初始化串口1,用于与其他底座通信
	UART2_Init(9600);							//初始化串口2，接收GPS数据
	
	while(1)
	{	
        if(GetGGA_Msg == 1)
        {//接收到GGA数据
           memcpy((void*)GGA_MsgBuffer,(const void*)USART2_RX_BUF,USART2_RX_STA);
            
            if(GPS_GGA_Analysis(&GPS_GGA_Msg,GGA_MsgBuffer))
            {
								delay_ms(10);
							
								Rs485_Config(1);
                printf("nsh->lcd%c\r\n",GPS_GGA_Msg.nshemi);			//南北纬	
                delay_ms(10);
                Rs485_Config(0);
							
								Rs485_Config(1);
                printf("ewh->lcd%c\r\n",GPS_GGA_Msg.ewhemi);			//东西经
                delay_ms(10);
                Rs485_Config(0);
							
                Rs485_Config(1);
								printf("lat->lcd%f\r\n",GPS_GGA_Msg.latitude);		//纬度  
                delay_ms(10);
                Rs485_Config(0);
							
							  Rs485_Config(1);
                printf("lon->lcd%f\r\n",GPS_GGA_Msg.longitude);		//经度
                delay_ms(10);
                Rs485_Config(0);
							
            }
            
            __HAL_UART_ENABLE(&UART2_Handler);//打开串口2，串口2功能的关闭在Usart.c->
            USART2_RX_STA = 0;
            GetGGA_Msg    = 0;								//清零，准备下次接收
        }
	}
}



