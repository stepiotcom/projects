#include "key.h"
#include "delay.h"
#include "stm32f1xx.h"


//==========================================================
//	函数名称：	KEY_Init
//
//	函数功能：	按键初始化函数
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
    
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;    //PA4、PA5、PA6、PA7
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
}

//==========================================================
//	函数名称：	KEY_Scan
//
//	函数功能：	按键检测
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
unsigned char KEY_Scan(void)
{
    if(S1==0||S2==0||S3==0||S4==0)  //检测到任意按键按下
    {
        delay_ms(10);    // 延时去抖
                     
        if(S1==0)       return S1_PRES;
        else if(S2==0)  return S2_PRES;
        else if(S3==0)  return S3_PRES; 
		else if(S4==0)  return S4_PRES;  			
    }
    return 0;   //无按键按下
}




