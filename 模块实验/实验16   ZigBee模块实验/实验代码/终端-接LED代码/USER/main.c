#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "timer.h"
#include "delay.h"
#include "Usart.h"
#include "Rs485.h"	
#include "ZigBee_Mode.h"
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
*	说明： 		GSM库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/

uint8_t KEY[16]={1,2,3,4,5,6,7,8,9,01,2,3,4,5,6};
const ZBM_Setting_TypeDef ZBM_Setting={
    ENDPOINT,         //设备类型
    0x1234,       //网络 PAN_ID(0000~FFFE 固定网络 PAN_ID,FFFF 随机网络 PAN_ID) 
    &KEY[0],      //网络密匙(16位)
    0x01,         //网络组号(1~99)
    0x0B,         //通信信道
    0x04,         //发送功率
    0x09,         //串口波特率
    0x00          //休眠状态  0,休眠状态停止 其他（1~250） 休眠模式开启，休眠时间为 sleep_time 单位 S（秒） 
};

const uint8_t RemoteZBM_MAC[8] = {0x6e,0x4B,0xF5,0x19,0x00,0x4B,0x12,0x00};
      uint8_t RemoteZBM_ShortAddr;
#define READ_LOCAL_MAC 0
/*
底座导光板灯为蓝色色，此节点为终端，连接LED模块底座
*/
int main(void)
{ 
    HAL_Init();             					//初始化HAL库
	UART1_Init(115200);						    //初始化串口1
	Rs485_Init();   						    //初始化485
    Rs485_Config(0);                            //Rs485 接收    
#if READ_LOCAL_MAC  //用于读本设备的MAC地址
     ZBM_ResetIO_Init();
     ZBM_Reset();
    
    //中断频率5kz(200us)，如修改ZigBee模块的通信波特率，
    //定时器中断频率也要修改，中断周期应该为两个字节传输时间
    TIM2_Init(64-1,200) ;//用于计算是否接收完成，当串口收到第一个字节，使能定时串口器，每收到一个字节清零定时器计数。当中数据发生时接收完成。      
     delay_ms(3000);                           //模块复位后，延时3秒
     UART2_Init(115200); 
     ZBM_GetLocalMAC(&ZBM_Info.MAC_ADDR[0]);    //获取本机MAC
     while(1);
#else
    ZigBee_Mode_Init();    
#endif    
    while(ZBM_IsJoinedNetwork() == ZBM_NETWORK_STATE_NO_NETWORK);
    
    ZBM_Info.nwk_state = ZBM_NETWORK_STATE_JOINED;
    USART2_RX_STA = 0;
    RecvSta = 0;
    Rs485_Config(0);
	while(1)
    {
        if(ZBM_Info.get_data == 1)
        {
            if(strstr((const char*)USART2_RX_BUF,"led1,toggle"))
            {
                memset((void*)USART2_RX_BUF,0,20);
                USART2_RX_STA = 0;
                RecvSta = 0;
                Rs485_Config(1);
                printf("led1,toggle\r\n");
                Rs485_Config(0);
                HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_4);
            }
            else if(strstr((const char*)USART2_RX_BUF,"led2,toggle"))
            {
                memset((void*)USART2_RX_BUF,0,20);
                USART2_RX_STA = 0;
                RecvSta = 0;
                Rs485_Config(1);
                printf("led2,toggle\r\n");
                Rs485_Config(0);
            //            HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_4);
            }
            else if(strstr((const char*)USART2_RX_BUF,"led3,toggle"))
            {
                memset((void*)USART2_RX_BUF,0,20);
                USART2_RX_STA = 0;
                RecvSta = 0;
                Rs485_Config(1);
                printf("led3,toggle\r\n");
            //            HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_4);
            }
            else if(strstr((const char*)USART2_RX_BUF,"led4,toggle"))
            {
                memset((void*)USART2_RX_BUF,0,20);
                USART2_RX_STA = 0;
                RecvSta = 0;
                Rs485_Config(1);
                printf("led4,toggle\r\n");
                Rs485_Config(0);
            //            HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_4);
            }
        }
    }
}





