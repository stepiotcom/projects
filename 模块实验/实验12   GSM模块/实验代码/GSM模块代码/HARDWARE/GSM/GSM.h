#ifndef __GSM_H__
#define __GSM_H__

#include "stm32f1xx_hal.h"
#include "Usart.h"

#define AT         	 	"AT\r\n"
#define IPR         	"AT+IPR=9600\r\n"																		//设置波特率
#define QIFGCNT       "AT+QIFGCNT=0\r\n"																	//设置为前台Context
#define CSQ         	"AT+CSQ\r\n"																				//返回信号质量，值越大信号越好
#define CREG         	"AT+CREG=1\r\n"																			//返回值为1则改卡已经开通GPRS服务
#define CGATT         "AT+CGATT=1\r\n"																		//返回OK  附着网络
#define CGACT         "AT+CGACT=1,1\r\n"																	//激活网络，之后可以使用TCPIP  AT指令
#define QIOPEN      	"AT+QIOPEN=\"TCP\",\"183.230.40.40\",\"1811\"\r\n"	//建立TCP连接
#define QIOSEND       "AT+QISEND=20\r\n"																	//开始发送
#define CIPSTATUS    "*134679#0001#sample*"													                //ONE.NET平台个人账号识别码


/*

*/

/**
  * @brief  初始化ESP8266,并配置路由和连接平台
  * @param  server:按照AT指令配置服务器地址和端口字符串
    * @param  ssid_pwd: 按照AT指令配置路由器的SSID和PWD
  * @retval NONE
  **/
void GSM_Init(void);

#endif


