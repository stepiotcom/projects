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
*	说明： 		库函数操作实验-HAL库版本
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
//与LED模块底座连接的ZigBee模块MAC地址，通过MAC地址取得其short addr,以short addr为方向发送数据。
const uint8_t RemoteZBM_MAC[8] = {0x6e,0x4B,0xF5,0x19,0x00,0x4B,0x12,0x00};
      uint16_t RemoteZBM_ShortAddr;

#define MSG_HEAD_LEN  8
const uint8_t MSG_HEAD[]="mrk->zbm";
#define READ_LOCAL_MAC 0
/*
底座导光板灯为绿色，此节点为绿色，连接矩阵键盘模块底座
*/
int main(void)
{ 
    uint8_t i = 0,pos = 0;
    HAL_Init();             					//初始化HAL库
	UART1_Init(115200);						    //初始化串口1
	Rs485_Init();   						    //初始化485
    Rs485_Config(0);                            //Rs485 接收    
#if READ_LOCAL_MAC //用于读本设备的MAC地址
     ZBM_ResetIO_Init();
     ZBM_Reset();
    
    //中断频率5kz(200us)，如修改ZigBee模块的通信波特率，
    //定时器中断频率也要修改，中断周期应该为两个字节传输时间,
    TIM2_Init(64-1,200) ; //用于计算是否接收完成，当串口收到第一个字节，使能定时串口器，每收到一个字节清零定时器计数。当中数据发生时接收完成。    
     delay_ms(3000);                           //模块复位后，延时3秒
     UART2_Init(115200); 
     ZBM_GetLocalMAC(&ZBM_Info.MAC_ADDR[0]);    //获取本机MAC
     while(1);
#else
    ZigBee_Mode_Init();       
#endif  
    ZBM_Info.nwk_state = ZBM_NETWORK_STATE_JOINED; 
    while(ZBM_IsJoinedNetwork() != ZBM_NETWORK_STATE_JOINED);
    delay_ms(200);   
    while(!RemoteZBM_ShortAddr)
    {
        delay_ms(1000); 
        ZBM_GetShortaddr_throung_MAC(&RemoteZBM_MAC[0],&RemoteZBM_ShortAddr);
    }
	while(1)
    {
        if((GetMsg == 1) && (ZBM_Info.nwk_state == ZBM_NETWORK_STATE_JOINED))
        {
                for(i=0;i < MSG_HEAD_LEN;i++)
            {//查找数据的头是否为MSG_HEAD指定的
                if(USART1_RX_BUF[i] != MSG_HEAD[i])
                {
                    break;
                }
            }
            
            pos = i;
            if(i == MSG_HEAD_LEN)//如果i等于MSG_HEAD_LEN，表明收到的数据头与MSG_HEAD指向的相同。则进入数据分析									
            {	
                switch(USART1_RX_BUF[pos+1])
                {
                    case '1':
                             ZBM_Send_P2P(RemoteZBM_ShortAddr,"led1,toggle",strlen("led1,toggle"));
                    break;
                    case '2':
                             ZBM_Send_P2P(RemoteZBM_ShortAddr,"led2,toggle",strlen("led2,toggle"));
                    break;
                    case '3':
                             ZBM_Send_P2P(RemoteZBM_ShortAddr,"led3,toggle",strlen("led3,toggle"));
                    break;
                    case '4':
                             ZBM_Send_P2P(RemoteZBM_ShortAddr,"led4,toggle",strlen("led4,toggle"));
                    break;                             
                    default:break;
                }//switch(USART1_RX_BUF[pos+2])
            }//if(i == MSG_HEAD_LEN)            
            GetMsg = 0;
            USART1_RX_STA = 0;
        }//if(GetMsg == 1) 
        
    }
}





