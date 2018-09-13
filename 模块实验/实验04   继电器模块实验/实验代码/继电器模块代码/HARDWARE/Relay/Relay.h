#ifndef __Relay_H
#define __Relay_H

#include "stm32f1xx_hal.h"

#define  RELAY1_OPEN  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); 			//RL1开启
#define  RELAY1_CLOSE   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	  //RL1关闭
#define  RELAY2_OPEN  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);  		//RL2开启
#define  RELAY2_CLOSE   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  	//RL2关闭

void Relay_Config(uint8_t fag);  //控制继电器开关

void Relay_Init(void);  //继电器初始化

#endif /*__Bsp_Relay_H */


