#include "GSM.h"

uint8_t ffiigg = 1;
/**
  * @brief  WIFI连接ONE.NET平台AT指令
  * @param  void
  * @retval void
  **/
void GSM_Init()
{
	if(ffiigg)
	{
		ffiigg = 0;
		HAL_Delay(5000);
	
		gsm_send_cmd(AT,"OK",2000); 					/*进入AT命令模式*/
		
		gsm_send_cmd(IPR,"OK",2000); 					/*设置波特率*/
	
		gsm_send_cmd(QIFGCNT,"OK",3000);			/*设置为前台Context*/
	
		gsm_send_cmd(CSQ,"OK",3000);					/*返回信号质量，值越大信号越好*/
		
		gsm_send_cmd(CREG,"OK",5000);					/*返回值为1则改卡已经开通GPRS服务*/
	
		gsm_send_cmd(CGATT,"OK",5000);				/*返回OK  附着网络*/
	
		gsm_send_cmd(CGACT,"OK",5000);				/*激活网络，之后可以使用TCPIP  AT指令*/
	
		gsm_send_cmd(QIOPEN,"OK",6000);				/*建立TCP连接*/
		
		gsm_send_cmd(QIOSEND,"0",3000);				/*开始传输*/
	
		printf("%s",CIPSTATUS);		                   /*ONE.NET平台个人账号识别码*/

		USART_AT_STA = 1;           					/*AT指令发送完成标志位*/
	}
		
}




