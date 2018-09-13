#include "stm32f1xx.h"
#include "delay.h"
#include "math.h"
#include "Usart.h"
#include "MsgHandler.h"
#include "M5310.h"
#include "nbiot.h"
#include "utils.h"
#include "Rs485.h"
#include "coap.h"
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
*	说明： 		NBIoT库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/
char coap_uri[] = "coap://183.230.40.40:5683"; //
char *serv_addr="183.230.40.40";
//****************************IMEI码         ;IMSI码         应与创建的应用的IMEI码，IMSI码一致
//const char endpoint_name[] = "865820031095552;460042437101012"; 
const char endpoint_name[] = "865820031043842;460042437101013";
//obj 的ID 资源(res)的ID要符合IPSO标准，具体请参考如下网站。
//http://www.openmobilealliance.org/wp/OMNA/LwM2M/LwM2MRegistry.html#x_label
#define OBJ_SENSOR_Temp_ID   3303
#define INST_ID_Temp    0
#define RES_ID_SENSOR   5701

#define OBJ_SENSOR_Humi_ID   3304
#define INST_ID_Humi    0

#define OBJ_SENSOR_illum_ID   3301
#define INST_ID_illum   0

#define OBJ_LED_ID      3311
#define INST_ID_LED     0
#define RES_ID_LED      5850
nbiot_uri_t temp_uri;
nbiot_uri_t humi_uri;
nbiot_uri_t illum_uri;
nbiot_uri_t led_uri;

uint8_t ReadRspTemp[]={"+MIPLREAD:0,62551,3302,0,5751,1,\"123\",1\r\n"};
uint8_t ReadRspHumi[]={"+MIPLREAD:0,62551,3304,0,5751,1,\"456\",1\r\n"};
uint8_t ReadRspIllum[]={"+MIPLREAD:0,62551,3301,0,5751,1,\"789\",1\r\n"};
uint8_t ReadRspLight[]={"+MIPLREAD:0,62551,3311,0,5850,5,\"true\",1\r\n"};
#define MSG_HEAD_LEN 10
const uint8_t MSG_HEAD[]="wsd->nbiot";
uint8_t AppStart = 0;
uint8_t SendBuffer[20],msgcode;
uint8_t MemCpyEndComma(uint8_t *pdst,uint8_t *psrc,uint8_t maxlen);
uint8_t light_control = 1;
uint16_t PreTempValue = 0,PreHumiValue = 0,PreIllumValue = 0,u16Temp;
int main(void)
{
    uint8_t flag = 0,pos=0,i=0,cpylen;
    HAL_Init();
    Rs485_Init();
    M5310_Power_Init(); //M5310的复位控制IO初始化
    Rs485_Config(0);     //
    UART2_Init(9600);  //初始化USART2，该串口与M5310通信,波特率9600

    netdev_init();      //初始化M5310
    init_miplconf(86400,coap_uri,endpoint_name);//上报注册码
//***************************温度********************************************<<<    
    m5310_addobj(OBJ_SENSOR_Temp_ID,INST_ID_Temp);          //添加对象object,温度
       
	temp_uri.objid  = OBJ_SENSOR_Temp_ID;
	temp_uri.instid = INST_ID_Temp;
	temp_uri.resid  = RES_ID_SENSOR;
	temp_uri.flag  |= NBIOT_SET_OBJID;
	temp_uri.flag  |= NBIOT_SET_INSTID;   
    
    m5310_notify_upload(&temp_uri,NBIOT_STRING,"1",0,0); //订阅资源
//***************************温度********************************************>>>  
    delay_ms(1000);
//***************************湿度********************************************<<<    
    m5310_addobj(OBJ_SENSOR_Humi_ID,INST_ID_Humi);               //添加对象object 湿度
	humi_uri.objid  = OBJ_SENSOR_Humi_ID;
	humi_uri.instid = INST_ID_Humi;
	humi_uri.resid  = RES_ID_SENSOR;
	humi_uri.flag  |= NBIOT_SET_OBJID;
	humi_uri.flag  |= NBIOT_SET_INSTID;   
    m5310_notify_upload(&humi_uri,NBIOT_STRING,"2",0,0); //订阅资源 
//***************************湿度********************************************>>>   
   delay_ms(1000);
//***************************光照强度****************************************<<< 
    m5310_addobj(OBJ_SENSOR_illum_ID,INST_ID_illum);               //添加对象object 光照强度
	illum_uri.objid  = OBJ_SENSOR_illum_ID;
	illum_uri.instid = INST_ID_illum;
	illum_uri.resid  = RES_ID_SENSOR;
	illum_uri.flag  |= NBIOT_SET_OBJID;
	illum_uri.flag  |= NBIOT_SET_INSTID;   
    m5310_notify_upload(&illum_uri,NBIOT_STRING,"3",0,0); //订阅资源 
//***************************光照强度*****************************************>>> 
   delay_ms(1000);
//***************************LED**********************************************<<< 
    m5310_addobj(OBJ_LED_ID,INST_ID_LED);                  //添加对象object LED
	led_uri.objid  = OBJ_LED_ID;
	led_uri.instid = INST_ID_LED;
	led_uri.resid  = RES_ID_LED;
	led_uri.flag  |= NBIOT_SET_OBJID;
	led_uri.flag  |= NBIOT_SET_INSTID;   
    m5310_notify_upload(&led_uri,NBIOT_BOOLEAN,"4",0,0); //订阅资源    
//***************************LED**********************************************>>>  

    m5310_register_request();                           //发出登陆请求
    UART1_Init(115200); //

    while(1)
    {        
        msgcode = Msg_Handler();
        if(msgcode == COAP_READ)
        {
            Rs485_Config(1);
            printf("led2,on\r\n"); //led on 表明连接成功
            Rs485_Config(0);
            AppStart = 1;
        }
        if(msgcode == COAP_WRITE)
        {
            Rs485_Config(1);
            if(light_control)
            {
                printf("led3,on\r\n");
            }
            else
            {
                printf("led3,off\r\n");
            }
            Rs485_Config(0);
        }
        if(GetSensor == 1)
        {
            GetSensor = 0;
            USART1_RX_STA = 0;
            for(i=0;i < MSG_HEAD_LEN;i++)
            {//查找数据的头是否为MSG_HEAD指定的
                if(USART1_RX_BUF[i] != MSG_HEAD[i])
                {
                    break;
                }
            }            
            pos = i;
            if((i == MSG_HEAD_LEN)&&(AppStart))//如果i等于MSG_HEAD_LEN，表明收到的数据头与MSG_HEAD指向的相同。则进入数据分析									
            {
                memset((void*)SendBuffer,0,20);
                cpylen = MemCpyEndComma(SendBuffer,&USART1_RX_BUF[++pos],20);
                u16Temp = nbiot_atoi((const char*)SendBuffer,strlen((char*)SendBuffer));
                
                if(u16Temp != PreTempValue)
                {
                    m5310_notify_upload(&temp_uri,NBIOT_STRING,(char*)&SendBuffer[0],1,0);
                    PreTempValue = u16Temp;
                }
                pos = pos + cpylen;
                
                memset((void*)SendBuffer,0,20);
                cpylen = MemCpyEndComma(SendBuffer,&USART1_RX_BUF[++pos],20);
                u16Temp = nbiot_atoi((const char*)SendBuffer,strlen((char*)SendBuffer)); 
                if(u16Temp != PreHumiValue)
                {                    
                    m5310_notify_upload(&humi_uri,NBIOT_STRING,(char*)&SendBuffer[0],1,0);
                    PreHumiValue = u16Temp;
                }
                pos = pos + cpylen;
                
                memset((void*)SendBuffer,0,20);
                MemCpyEndComma(SendBuffer,&USART1_RX_BUF[++pos],20);                  
                u16Temp = nbiot_atoi((const char*)SendBuffer,strlen((char*)SendBuffer)); 

                if(u16Temp != PreIllumValue)   
                {                   
                    m5310_notify_upload(&illum_uri,NBIOT_STRING,(char*)&SendBuffer[0],1,0);
                    PreIllumValue = u16Temp;
                }
                USART1_RX_STA = 0;
            }
            
        }           
    }
}
uint8_t MemCpyEndComma(uint8_t *pdst,uint8_t *psrc,uint8_t maxlen)
{
    uint8_t j=0;
    while(maxlen--)
    {
        pdst[j] = psrc[j];
        if(psrc[++j] == ',')
        {
            break;
        }
    }
    return j;
}
