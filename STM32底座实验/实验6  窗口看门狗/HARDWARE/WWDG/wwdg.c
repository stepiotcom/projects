#include "wwdg.h"
#include "led.h"

WWDG_HandleTypeDef WWDG_Handler;     //窗口看门狗句柄

//保存WWDG计数器的设置值，默认为最大
unsigned char WWDG_CNT=0X7F;

unsigned char  led_flag = 0;  //LED灯状态标志

//==========================================================
//	函数名称：	WWDG_Init
//
//	函数功能：	初始化窗口看门狗 	
//
//	入口参数：	
//              tr   :T[6:0],计数器值 
//              wr   :W[6:0],窗口值 
//              fprer:分频系数（WDGTB）,仅最低2位有效 
//
//	返回参数：	无
//
//	说明：		Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
//==========================================================
void WWDG_Init(unsigned char tr,unsigned char wr,unsigned int fprer)
{
    WWDG_Handler.Instance=WWDG;
    WWDG_Handler.Init.Prescaler=fprer; 		//设置分频系数
    WWDG_Handler.Init.Window=wr;       		//设置窗口值
    WWDG_Handler.Init.Counter=tr;     		//设置计数器值
    HAL_WWDG_Init(&WWDG_Handler);        	//初始化WWDG
    HAL_WWDG_Start_IT(&WWDG_Handler);    	//开启窗口看门狗  
}

//==========================================================
//	函数名称：	HAL_WWDG_MspInit
//
//	函数功能：	WWDG底层驱动，时钟配置，中断配置
//
//	入口参数：	hwwdg:窗口看门狗句柄
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{   
    __HAL_RCC_WWDG_CLK_ENABLE();    //使能窗口看门狗时钟
        
    HAL_NVIC_SetPriority(WWDG_IRQn,2,3);    //抢占优先级2，子优先级为3
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          //使能窗口看门狗中断
}

//==========================================================
//	函数名称：	WWDG_IRQHandler
//
//	函数功能：	窗口看门狗中断服务函数
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler);//调用WWDG共用中断处理函数
}


//==========================================================
//	函数名称：	HAL_WWDG_WakeupCallback
//
//	函数功能：	中断服务函数处理过程
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void HAL_WWDG_WakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
    HAL_WWDG_Refresh(&WWDG_Handler,WWDG_CNT);//更新窗口看门狗值
    
    led_flag = !led_flag; //标志取反
    
    
    if(led_flag)
    {
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET); 	//LED4对应引脚PB0拉低，亮
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET); 	    //LED4对应引脚PB0拉高，灭	
    } 
}
