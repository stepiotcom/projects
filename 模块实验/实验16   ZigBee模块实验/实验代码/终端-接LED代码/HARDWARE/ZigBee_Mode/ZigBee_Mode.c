#include "ZigBee_Mode.h"
#include "stm32f1xx.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"

#define ZBM_RESET_GPIO_PIN   GPIO_PIN_14
#define ZBM_RESET_GPIO_PORT  GPIOB
#define ZBM_IO_RCC_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE() 
uint8_t RspLenght = 0;
ZBM_Info_TypeDef ZBM_Info;
uint8_t ZBM_RecvDataBuffer[ZBM_RECVDATA_LEN],ZBM_RecvDataIdx = 0;
uint8_t ZBM_RecvNwkStatePrompt[10],ZBM_RecvNwkStatePromptIdx = 0;
//==========================================================
//	函数名称：	ZBM_RecvHandler
//
//	函数功能：	检测是否有数据返回
//
//	入口参数：	uint16_t waittime,等待的时间
//              uint32_t *pcheckobj,检测对象
//              uint8_t expected_threshold，检测对象的阈值，
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ZBM_RecvHandler(uint8_t *precvbuf,uint8_t buflen)
{
    RspLenght = buflen;
//****************处理模块返回的网络状态******************************************<<<
    if((precvbuf[0] == 0xFF)&&(precvbuf[1] == 0xAA)&&(buflen == 2))
    {//只有终端设备才会收到这个数据
      ZBM_Info.nwk_state = ZBM_NETWORK_STATE_JOINED;
    }
    else if(((precvbuf[0] == 0xFF)&&(precvbuf[1] == 0x00))&&(buflen == 2))
    {//只有终端设备才会收到这个数据
      ZBM_Info.nwk_state = ZBM_NETWORK_STATE_NO_NETWORK;
    }
    else if(((precvbuf[0] == 0xFF)&&(precvbuf[1] == 0xFF))&&(buflen == 2))
    {//只协调器节点才会收到这个数据
      ZBM_Info.nwk_state = ZBM_NETWORK_STATE_BUILD_NETWORK;
    }
//****************处理模块返回的网络状态******************************************>>> 

//****************读取、设置参数的回馈********************************************<<<
    if((precvbuf[0] == 0xFA)&&(buflen >= 2))
    {//设置参数的反馈
      ZBM_Info.get_writersp = 1;
    }
    else if((precvbuf[0] == 0xFB)&&(buflen >= 2))
    {//读取参数的反馈
      ZBM_Info.get_readrsp = 1;
    }      
//****************读取、设置参数的回馈********************************************>>>  
    
//****************接收到数据******************************************************<<<
    //帧格式：C0 C1 C2 + 数据长度+用户数据+发送的个消息的ZigBee模块short addr(由模块自己写)
    if((precvbuf[0] == 0xC0)&&((precvbuf[1] == 0xC1)&&(precvbuf[2] == 0xC2)))
    {//收到数据
      ZBM_Info.get_data = 1;
    }    
//****************接收到数据******************************************************>>>     
}
//==========================================================
//	函数名称：	ZBM_CheckRsp
//
//	函数功能：	检测是否有数据返回
//
//	入口参数：	uint16_t waittime,等待的时间
//              uint32_t *pcheckobj,检测对象
//              uint8_t expected_threshold，检测对象的阈值，
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ZBM_CheckRsp(uint16_t waittime,uint32_t *pcheckobj,uint8_t expected_threshold)
{
    RspLenght = 0;
    while(waittime)
    {
        if(ZBM_Info.get_readrsp || ZBM_Info.get_writersp)
        {
            break;
        }
        delay_ms(1);
        waittime--;
    }
    ZBM_Info.get_readrsp  = 0;
    ZBM_Info.get_writersp = 0;
}
//==========================================================
//	函数名称：	swaps
//
//	函数功能：	将一个两字节的数据，高低字节交换
//
//	入口参数：	uint16_t i
//
//	返回参数：	返回类型uint16_t，返回转换后的结果
//
//	说明：		
//==========================================================
uint16_t swaps(uint16_t i)
{
    uint16_t ret=0;
    ret = (i & 0xFF) << 8;
    ret |= ((i >> 8)& 0xFF);
    return ret;	
}
//==========================================================
//	函数名称：	ZBM_SendData
//
//	函数功能：	向ZigBee模块发送数据
//
//	入口参数：	uint8_t *pdata,指向期望发送的数据
//               uint16_t len ,将发送的数据长度
//	返回参数：	无
//
//	说明：		使用串口2进行发送 波特率115200
//==========================================================
uint8_t gtemp[9],ii;
void ZBM_SendData(uint8_t *pdata,uint16_t len)
{
    ii = 0;
    while(len)
    {
        while((USART2->SR&0X40)==0)
        {;} //循环发送,直到发送完毕 
        USART2->DR = *pdata; 
        len--;
        pdata++;
    }      
}
//==========================================================
//	函数名称：	ZBM_SetNetworkPAN
//
//	函数功能：	设置网络的PAN
//
//	入口参数：	uint16_t panid,PAN id
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 03 03 pan_id FF  返回 FA 03	
//==========================================================
enum ZBM_ErrorCode ZBM_SetNetworkPAN(uint16_t panid)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x03;
    zbm_cmd[index++] = 0x03; 
    zbm_cmd[index++] = (uint8_t)(panid>>8); 
    zbm_cmd[index++] = (uint8_t)panid;  
    zbm_cmd[index++] = 0xFF;    
    
    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(3000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x03))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetDevType
//
//	函数功能：	设置节点类型，路由，协调器，终端
//
//	入口参数：	enum ZBM_DevType type,节点的类型
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 01 type FF  返回 FA 01	
//==========================================================
enum ZBM_ErrorCode ZBM_SetDevType(enum ZBM_DevType type)
{
    static uint8_t  zbm_cmd[10],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x01; 
    zbm_cmd[index++] = type; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x01))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetNetworkKey
//
//	函数功能：	设置网络密钥
//
//	入口参数：	uint8_t *pkey,网络密钥，
//              uint8_t len，密钥长度
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 11 04 key(16位) FF  返回 FA 04	
//==========================================================
enum ZBM_ErrorCode ZBM_SetNetworkKey(uint8_t *pkey,uint8_t len)
{
    uint8_t zbm_cmd[20],index=0,i=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x11;
    zbm_cmd[index++] = 0x04; 
    for(i=0;i<len;i++)
    {
        zbm_cmd[index++] = *(pkey++); 
    }
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x04))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetNetworkGrounp
//
//	函数功能：	设置网络组
//
//	入口参数：	uint8_t grounp,网络组号(1~99)
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 11 04 key(16位) FF  返回 FA 04	
//==========================================================
enum ZBM_ErrorCode ZBM_SetNetworkGrounp(uint8_t grounp)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x09; 
    zbm_cmd[index++] = grounp; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x09))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetChannel
//
//	函数功能：	设置网络组
//
//	入口参数：	uint8_t ch,通信信道 (11~26)
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 0A ch FF  返回 FA 0A	
//==========================================================
enum ZBM_ErrorCode ZBM_SetChannel(uint8_t ch)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    if((ch<11)||(ch>26))
    {
        error = ZBM_FAILED;
    }
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x0A; 
    zbm_cmd[index++] = ch; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x0A))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetTxPower
//
//	函数功能：	设置发送功率
//
//	入口参数：	uint8_t pow,功率值 (00~05)
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 0B pow FF  返回 FA 0B	
//==========================================================
enum ZBM_ErrorCode ZBM_SetTxPower(uint8_t pow)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x0B; 
    zbm_cmd[index++] = pow; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x0B))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetSerialBaud
//
//	函数功能：	设置串口波特率
//
//	入口参数：	uint8_t baud_index,功率值 (00~0F)
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 0C baud FF  返回 FA 0C	
//==========================================================
enum ZBM_ErrorCode ZBM_SetSerialBaud(uint8_t baud_index)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x0C; 
    zbm_cmd[index++] = baud_index; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);    
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x0C))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetSleepMode
//
//	函数功能：	设置终端的休眠模式
//
//	入口参数：	uint8_t mode
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 0D sleepmode FF  返回 FA 0D	
//==========================================================
enum ZBM_ErrorCode ZBM_SetSleepMode(uint8_t mode)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x0D; 
    zbm_cmd[index++] = mode; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);    
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x0D))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_SetDataSaveTime
//
//	函数功能：	设置路由器、终端的数据保存时间
//
//	入口参数：	uint8_t time
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FD 02 0E baud FF  返回 FA 0E	
//==========================================================
enum ZBM_ErrorCode ZBM_SetDataSaveTime(uint8_t time)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFD;
    zbm_cmd[index++] = 0x02;
    zbm_cmd[index++] = 0x0E; 
    zbm_cmd[index++] = time; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);    
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if((USART2_RX_BUF[0] == 0xFA)&&(USART2_RX_BUF[1] == 0x0E))
    {
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetDevType
//
//	函数功能：	获取节点类型
//
//	入口参数：	uint8_t *ptype
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 01 FF  返回 FB type	
//==========================================================
enum ZBM_ErrorCode ZBM_GetDevType(uint8_t *ptype)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x01; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,4);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
       *ptype = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetNwkState
//
//	函数功能：	获取网络状态
//
//	入口参数：	uint8_t *pstate
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 02 FF  返回 FB state	
//==========================================================
enum ZBM_ErrorCode ZBM_GetNwkState(uint8_t *pstate)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x02; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
       *pstate = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetPAN
//
//	函数功能：	获取网络PAN
//
//	入口参数：	uint16_t *ppan
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 03 FF  返回 FB pan	
//==========================================================
enum ZBM_ErrorCode ZBM_GetPAN(uint16_t *ppan)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x03; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *ppan = (USART2_RX_BUF[1]<<8) + USART2_RX_BUF[2];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetNwkPAN
//
//	函数功能：	获取网络密钥
//
//	入口参数：	uint8_t *ppan
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 04 FF  返回 FB key	
//==========================================================
enum ZBM_ErrorCode ZBM_GetNwkKey(uint8_t *pkey)
{
    uint8_t zbm_cmd[20],index=0,i;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x04; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        for(i=1;i < RspLenght;i++)
        {
            *(pkey++) = USART2_RX_BUF[i];
        }
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetNwkShortAddr
//
//	函数功能：	获取网络short addr
//
//	入口参数：	uint8_t *pshortaddr
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 05 FF  返回 FB short addr
//==========================================================
enum ZBM_ErrorCode ZBM_GetNwkShortAddr(uint16_t *pshortaddr)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x05; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *pshortaddr = (USART2_RX_BUF[1]<<8) + USART2_RX_BUF[2];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetLocalMAC
//
//	函数功能：	获取本机的MAC地址
//
//	入口参数：	uint8_t *pmac
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 06 FF  返回 FB mac addr	
//==========================================================
enum ZBM_ErrorCode ZBM_GetLocalMAC(uint8_t *pmac)
{
    uint8_t zbm_cmd[20],index=0,i;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x06; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        for(i=1;i < RspLenght;i++)
        {
            *(pmac++) = USART2_RX_BUF[i];
        }
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetParentsShortAddr
//
//	函数功能：	获取父节点的short addr
//
//	入口参数：	uint16_t *pshortaddr
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 07 FF  返回 FB short addr	
//==========================================================
enum ZBM_ErrorCode ZBM_GetParentsShortAddr(uint16_t *pshortaddr)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x07; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *pshortaddr = (USART2_RX_BUF[1]<<8) + USART2_RX_BUF[2];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetParentsMACAddr
//
//	函数功能：	获取父节点的MAC地址
//
//	入口参数：	uint8_t *pmacaddr
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 08 FF  返回 FB mac addr	
//==========================================================
enum ZBM_ErrorCode ZBM_GetParentsMACAddr(uint8_t *pmacaddr)
{
    uint8_t zbm_cmd[20],index=0,i;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x08; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        for(i=0;i<RspLenght;i++)
        {
            *pmacaddr = USART2_RX_BUF[i];
        }
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetNwkGrounNo
//
//	函数功能：	获取网络组号
//
//	入口参数：	uint8_t *pgroundno
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 09 FF  返回 FB groundno	
//==========================================================
enum ZBM_ErrorCode ZBM_GetNwkGrounNo(uint8_t *pgroundno)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x09; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *pgroundno = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetChannel
//
//	函数功能：	获取通信信道
//
//	入口参数：	uint8_t *pchannel
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 0A FF  返回 FB channel	
//==========================================================
enum ZBM_ErrorCode ZBM_GetChannel(uint8_t *pchannel)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x0A; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *pchannel = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetTxPower
//
//	函数功能：	获取发射功率
//
//	入口参数：	uint8_t *ptxpower
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 0B FF  返回 FB power	
//==========================================================
enum ZBM_ErrorCode ZBM_GetTxPower(uint8_t *ptxpower)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x0B; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *ptxpower = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetSerialBaud
//
//	函数功能：	获取串口波特率
//
//	入口参数：	uint8_t *pbaud
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 0C FF  返回 FB baud	
//==========================================================
enum ZBM_ErrorCode ZBM_GetSerialBaud(uint8_t *pbaud)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x0C; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *pbaud = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetSerialBaud
//
//	函数功能：	获取终端设备的休眠状态
//
//	入口参数：	uint8_t *pbaud
//
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 0D FF  返回 FB dormant status	
//==========================================================
enum ZBM_ErrorCode ZBM_GetDormantStatus(uint8_t *psta)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x01;
    zbm_cmd[index++] = 0x0D; 
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *psta = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetShortaddr_throung_MAC
//
//	函数功能：	获取MAC地址设备对应的短地址
//
//	入口参数：	uint8_t *pmacaddr,
//              uint16_t *pshortaddr,返回设备的短地址
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 09 10 FF  返回 FB short addr
//==========================================================
enum ZBM_ErrorCode ZBM_GetShortaddr_throung_MAC(const uint8_t *pmacaddr,uint16_t *pshortaddr)
{
    uint8_t zbm_cmd[20],index=0,i,retry=3;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x09;
    zbm_cmd[index++] = 0x10;
    for(i=0;i < 8;i++)
    {
        zbm_cmd[index++] = *(pmacaddr++);   
    }
    
    zbm_cmd[index++] = 0xFF;    
    
    while(retry--)
    {
        USART2_RX_STA = 0;
        memset((void*)USART2_RX_BUF,0,20);
        ZBM_SendData(zbm_cmd,index);
        ZBM_CheckRsp(2000,&USART2_RX_STA,1);
        if(USART2_RX_BUF[0] == 0xFB)
        {
            *pshortaddr = (USART2_RX_BUF[1]<<8) + USART2_RX_BUF[2];
            error = ZBM_SUCCESS;
            break;
        }
        else
        {
            error = ZBM_FAILED;
        }
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetShortaddr_throung_MAC
//
//	函数功能：	获取MAC地址协调器、路由器的数据保存时间。
//
//	入口参数：	uint8_t *ptime,
//              
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	发送 //FE 01 0E FF  返回 FB save time
//==========================================================
enum ZBM_ErrorCode ZBM_GetDataSaveTimeC(uint8_t *ptime)
{
    uint8_t zbm_cmd[20],index=0;
    enum ZBM_ErrorCode error;
    
    zbm_cmd[index++] = 0xFE;
    zbm_cmd[index++] = 0x09;
    zbm_cmd[index++] = 0x0E;
    zbm_cmd[index++] = 0xFF;    

    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(zbm_cmd,index);
    ZBM_CheckRsp(2000,&USART2_RX_STA,1);
    if(USART2_RX_BUF[0] == 0xFB)
    {
        *ptime = USART2_RX_BUF[1];
        error = ZBM_SUCCESS;
    }
    else
    {
        error = ZBM_FAILED;
    }
    return error; 
}
//==========================================================
//	函数名称：	ZBM_GetShortaddr_throung_MAC
//
//	函数功能：	获取MAC地址协调器、路由器的数据保存时间。
//
//	入口参数：	uint8_t *ptime,
//              
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	指令： 03+ type +addr+data
//==========================================================
#define ZBM_SEND_BUF_LEN 50
uint8_t ZBM_SendBuffer[ZBM_SEND_BUF_LEN];
void ZBM_Send_P2P(uint16_t shortaddr,uint8_t *pdata,uint8_t len)
{
    uint8_t index=0,i=0;
    
    ZBM_SendBuffer[index++] = 0xFC;
    ZBM_SendBuffer[index++] = len + 4;
    ZBM_SendBuffer[index++] = 0x03;
    ZBM_SendBuffer[index++] = 0x02;
    ZBM_SendBuffer[index++] = (uint8_t)(shortaddr>>8);
    ZBM_SendBuffer[index++] = (uint8_t)shortaddr;  
    for(i=0;i < len;i++)
    {
        ZBM_SendBuffer[index++] = *(pdata+i);
    }
    USART2_RX_STA = 0;
    memset((void*)USART2_RX_BUF,0,20);
    ZBM_SendData(ZBM_SendBuffer,index); 
    delay_ms(10);    
}
//==========================================================
//	函数名称：	ZBM_IsJoinedNetwork
//
//	函数功能：	检测是否加入网络成功
//
//	入口参数：	无
//              
//	返回参数：	返回类型ZBM_ErrorCode。
//
//	说明：	指令： 03+ type +addr+data
//==========================================================
enum ZBM_ErrorCode ZBM_IsJoinedNetwork(void)
{
    return ZBM_Info.nwk_state;
}
//==========================================================
//	函数名称：	ZBM_ResetIO_Init(void)
//
//	函数功能：	复位ZigBee模块管脚初始化
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ZBM_ResetIO_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    ZBM_IO_RCC_ENABLE();                  	     //开启GPIOB时钟
	 
    GPIO_Initure.Pin   = ZBM_RESET_GPIO_PIN;     //PB14
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;    //推挽输出
    GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;    //低速
    HAL_GPIO_Init(ZBM_RESET_GPIO_PORT, &GPIO_Initure);
}
//==========================================================
//	函数名称：	ZBM_Reset
//
//	函数功能：	复位ZigBee 模块
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ZBM_Reset(void)
{
    HAL_GPIO_WritePin(ZBM_RESET_GPIO_PORT,ZBM_RESET_GPIO_PIN,GPIO_PIN_SET);	//PB14置1 输出高  
    delay_ms(1);    
    HAL_GPIO_WritePin(ZBM_RESET_GPIO_PORT,ZBM_RESET_GPIO_PIN,GPIO_PIN_RESET);	//PB14置0 输出底 
    delay_ms(1); 
    HAL_GPIO_WritePin(ZBM_RESET_GPIO_PORT,ZBM_RESET_GPIO_PIN,GPIO_PIN_SET);	//PB14置1 输出高    
    delay_ms(10);    
}
//==========================================================
//	函数名称：	ZigBee_Mode_Init
//
//	函数功能：	初始化ZigBee 模块
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ZigBee_Mode_Init(void)
{
    ZBM_ResetIO_Init();
    ZBM_Reset();
    UART2_Init(115200);                          //初始化串口(与ZigBee模块通信)
    
    //中断频率5kz(200us)，如修改ZigBee模块的通信波特率，
    //定时器中断频率也要修改，中断周期应该为两个字节传输时间
    TIM2_Init(64-1,200) ;                        
    delay_ms(3000);
    if(ZBM_SetDevType(ZBM_Setting.dev_type) == ZBM_FAILED)    //设备类型
    {
        
    }   
    if(ZBM_SetNetworkPAN(ZBM_Setting.pan_id) == ZBM_FAILED)   //网络 PAN
    {
           
    }
    if(ZBM_SetNetworkKey(ZBM_Setting.pKey,16) == ZBM_FAILED)  //网络密匙
    {
 
    }
    if(ZBM_SetNetworkGrounp(ZBM_Setting.group) == ZBM_FAILED)//网络组号
    {
        
    }
    if(ZBM_SetChannel(ZBM_Setting.channel) == ZBM_FAILED)    //通信信道
    {
        
    }
    if(ZBM_SetTxPower(ZBM_Setting.txpower) == ZBM_FAILED)     //发送功率
    {

    }
    if(ZBM_SetSerialBaud(ZBM_Setting.baud) == ZBM_FAILED)      //模块默认的波特率是115200，如改成了其他的波特，记得MCU的波特率也跟着修改
    {
   
    }
    if(ZBM_SetSleepMode(ZBM_Setting.sleepmode))
    {
    
    }
    
    ZBM_Reset();
}






















