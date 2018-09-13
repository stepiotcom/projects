#include "led.h"
#include "delay.h"
#include "stm32f1xx.h"

//==========================================================
//	函数名称：	LED_Init
//
//	函数功能：	LED灯初始化
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================

uint8_t number0 = 1;  //初始化

void LED_Init(void)
{
	
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           	//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_13|GPIO_PIN_14; 	//PB0,PB1,PB13,PB14
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_13|GPIO_PIN_14,GPIO_PIN_SET);	//PB0,PB1,PB13,PB14置1，默认初始化后灯灭

}

//==========================================================
//	函数名称：	LED_Config
//
//	函数功能：	LED操作
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void LED_Config(uint32_t num)
{
	switch(num)
		{
			case 1:
			{
				//翻转LED1状态
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);
				delay_ms(200); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);  		
				break;
			}				
			case 2:
			{
				//翻转LED2状态
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN);
				delay_ms(200); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);  
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN);  
				delay_ms(200); 				
				break;
			}	
			case 3:
			{
				//翻转LED3状态
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED3_GPIO_PIN);
				delay_ms(200); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);  
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED3_GPIO_PIN);   				
				delay_ms(200); 	
				
				break;
			}		
			case 4: 
			{
				//翻转LED4状态
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED3_GPIO_PIN);
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED4_GPIO_PIN);
				delay_ms(200); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED1_GPIO_PIN);  
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED2_GPIO_PIN); 
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED3_GPIO_PIN);  
				HAL_GPIO_TogglePin(LED_GPIO_PORT,LED4_GPIO_PIN);  				
				delay_ms(200); 
				  
				break;
			}	
			
			default: break;
		}
}

