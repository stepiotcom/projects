#ifndef _LED_H
#define _LED_H

#include "stm32f1xx_hal.h"

#define  LED_GPIO_PORT  GPIOB

#define  LED1_GPIO_PIN  GPIO_PIN_0
#define  LED2_GPIO_PIN  GPIO_PIN_1
#define  LED3_GPIO_PIN  GPIO_PIN_13
#define  LED4_GPIO_PIN  GPIO_PIN_14

extern uint8_t number0;

void LED_Init(void);

void LED_Config(void);



#endif
