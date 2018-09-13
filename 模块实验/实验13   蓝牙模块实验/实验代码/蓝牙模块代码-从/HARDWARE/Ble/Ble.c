#include "Ble.h"	
#include "delay.h"
#include "Usart.h"

void HC_08_Init()
{
	delay_ms(3000);															/*延时等待*/		
	
	printf("blefig\r\n");												/*发送检测数据*/
	
	delay_ms(5000);															/*延时等待*/	
	
	if(BLE_FIG)																	/*如果蓝牙没有连接执行AT指令*/
	{
		atk_8266_send_cmd(AT,"OK",3000); 					/*检测串口是否正常工作*/
	
		atk_8266_send_cmd(ROLE,"OK",3000);				/*设置主从角色*/
	
		atk_8266_send_cmd(NAME,"OK",5000);				/*修改蓝牙名称*/
	
		atk_8266_send_cmd(ADDR,"OK",4000);				/*修改蓝牙地址*/
	
		atk_8266_send_cmd(BAUD,"OK",2000);				/*修改波特率*/
	
		atk_8266_send_cmd(CONT,"OK",2000);				/*设置模块是否可连接*/
	
		atk_8266_send_cmd(LUUID,"OK",2000);				/*设置UUID*/
		
	}
	
	USART_AT_STA = 1;													//AT指令发送完成标志位
}


