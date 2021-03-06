#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "delay.h"
#include "TFT.h"

/**
************************************************************
************************************************************
************************************************************
*
*	作者： 		重庆八城科技
*
*	日期： 		2018-01-01
*
*	版本： 		V1.0
*
*	说明： 		TFT操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/
	
int main(void)
{ 
    HAL_Init();             				//初始化HAL库
		TFT_Init();             				//TFT显示器端口初始化
		Lcd_Init();											//初始化LCD
		TFT_UI();												//计算器UI界面
	
		while(1)
		{
				TFT_Computing();						//计算器处理(计算结果最大值4294967295)
		}
}

