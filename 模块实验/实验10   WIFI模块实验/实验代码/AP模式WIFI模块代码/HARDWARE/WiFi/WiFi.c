#include "WiFi.h"

/**
  * @brief  WIFI连接ONE.NET平台AT指令
  * @param  void
  * @retval void
  **/
void ESP8266_Init()
{
		atk_8266_send_cmd(AT,"OK",2000); 					/*进入AT命令模式*/
	
		atk_8266_send_cmd(CWMODE,"OK",3000);			/*设置为AP模式*/
	
		atk_8266_send_cmd(RST,"ready",3000);			/*重置模块*/
	
		atk_8266_send_cmd(CWSAP,"OK",3000);				/*设置该模块的SID和PWD*/
	
		atk_8266_send_cmd(CIFSR,"OK",3000);				/*查询IP */
	
		atk_8266_send_cmd(CIPMUX,"OK",3000);			/*使能多连接*/
	
		atk_8266_send_cmd(CIPSERVER,"OK",3000);		/*配置为服务器-设置端口*/
	
		atk_8266_send_cmd(CIPAPMAC,"OK",2000);		/*设置MAC地址*/
		
		atk_8266_send_cmd(CIPAP,"OK",3000);				/*设置IP地址*/
	
		USART_AT_STA = 1;           							/*AT指令发送完成标志位*/
}




