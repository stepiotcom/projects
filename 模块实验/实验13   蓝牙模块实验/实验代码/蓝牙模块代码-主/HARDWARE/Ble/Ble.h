#ifndef __Ble_H
#define __Ble_H
#include "stm32f1xx_hal.h"

#define AT         	 	"AT"											/*检测串口是否正常工作*/
#define CLEAR       	"AT+CLEAR"								/*主机清除从机地址*/
#define ROLE        	"AT+ROLE=M"								/*设置主从角色  M(主)    S(从) 默认为S*/
#define BAUD      		"AT+BAUD=9600"						/*修改波特率*/
#define CONT      		"AT+CONT=0"								/*设置模块是否可连接   0(可连接)  1(不可连接)   默认可连接*/
#define LUUID      		"AT+LUUID=1211"						/*设置UUID*/

/*
*		初始化配置HC-08蓝牙模块
*
*/
void HC_08_Init(void);


#endif

