#include "WiFi.h"

/**
  * @brief  WIFI连接ONE.NET平台AT指令
  * @param  void
  * @retval void
  **/
void ESP8266_Init()
{
		atk_8266_send_cmd(AT,"OK",2000); 					/*进入AT命令模式*/
	
		atk_8266_send_cmd(CWMODE,"OK",3000);			/*设置模块应用模式*/
	
		atk_8266_send_cmd(RST,"0",3000);					/*重置模块*/
	
		atk_8266_send_cmd(CIFSR,"OK",3000);				/*查询设备IP可放在需要的位置*/
	
		atk_8266_send_cmd(CWJAP,"OK",12000);			/*配置要连接的路由器的SSID和密码*/
	
		atk_8266_send_cmd(TCP_CIPSTART,"OK",3000);/*与服务器建立TCP连接*/
	
		atk_8266_send_cmd(CIPMODE,"OK",3000);			/*透传模式*/
	
		atk_8266_send_cmd(CIPSEND,"OK",2000);			/*开始传输*/
	
		printf("22\r\n");
	
		USART_AT_STA = 1;           							/*AT指令发送完成标志位*/
}




