#ifndef __Pressure_Sensor_H
#define __Pressure_Sensor_H
#include "stm32f1xx_hal.h"

#define SDA   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)

unsigned long ReadCount(void);
void Pressure_Sensor_Init(void); //初始化压力传感器

#endif

