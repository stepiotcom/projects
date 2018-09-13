#ifndef __Ble_H
#define __Ble_H
#include "stm32f1xx_hal.h"

#define AT         	 	"AT"											/*检测串口是否正常工作*/
#define ROLE        	"AT+ROLE=S"								/*设置主从角色  M(主)    S(从) 默认为S*/
#define NAME        	"AT+NAME=BaCheng"					/*修改蓝牙名称*/
#define ADDR 					"AT+ADDR=FFFFFDFAFDAF"		/*修改蓝牙地址*/
#define BAUD      		"AT+BAUD=9600"						/*修改波特率*/
#define CONT      		"AT+CONT=0"								/*设置模块是否可连接   0(可连接)  1(不可连接)   默认可连接*/
#define LUUID      		"AT+LUUID=1211"						/*设置UUID*/

/*
*		初始化配置HC-08蓝牙模块
*
*/
void HC_08_Init(void);


#endif

