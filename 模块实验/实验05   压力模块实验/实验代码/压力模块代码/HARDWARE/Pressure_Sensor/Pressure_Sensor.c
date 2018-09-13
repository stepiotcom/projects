#include "Pressure_Sensor.h"
#include "delay.h"


char  i;  						//for循环使用
long  val;						//返回的称重值
float Newval = 0.0; 	//用于填补0下的误差
int a = 0; 						//用于计数  记录进入次数  用于初始化校准
	
//==========================================================
//	函数名称：	ReadCount_Init
//
//	函数功能：	初始化压力传感器
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================	
void Pressure_Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           	//开启GPIOB时钟
	
		GPIO_Initure.Pin = GPIO_PIN_0;						//PB0
		GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW; //低速
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
	  GPIO_Initure.Pin = GPIO_PIN_1;						//PB1
		GPIO_Initure.Mode = GPIO_MODE_INPUT;			//输入
		GPIO_Initure.Pull = GPIO_NOPULL;					//浮空
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);	//PA0置0   初始化等待模式

}

//==========================================================
//	函数名称：	Calibration
//
//	函数功能：	校准压力值
//
//	入口参数：	采样值
//
//	返回参数：	无
//
//	说明：		
//==========================================================	
void Calibration(void)
{
	if((val<0 || val>0) && a<=5) 
	{
		Newval = 0-val;																					//进行开机校准
		val = 0;   																					
	}
	else val += Newval;      																	//校准
	
	if(a>5)  a = 10;																					//开机校准后固定a的值
}

	
//==========================================================
//	函数名称：	ReadCount
//
//	函数功能：	获取压力值
//
//	入口参数：	无
//
//	返回参数：	重量数据（单位 g）
//
//	说明：		
//==========================================================	
unsigned long ReadCount(void)  
{ 
	a++; //计数累加
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);	   		//DOUT=1 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);	   	//SCK=0 
	val = 0;
	
	while( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1));  							//等待DOUT=0  
	__NOP(); 
	for(i=0;i<24;i++) 
	{ 
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);	   	//SCK=1 
		__NOP(); 
		if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1))   							//DOUT=1 
			val++; 
		val=val<<1;   
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);	  //SCK=0 
		__NOP(); 

	} 
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); 			//SCK=1 
	__NOP();  
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);			//SCK=0 
	__NOP();
	
	val = val^0x800000; 
	
	val = (val/429.4967296-19610)/1.701; 											//计算实物的实际重量
	
	Calibration();																						//校准
	
	delay_ms(10);
	
	return (val);																							//返回压力值（单位g）
}  

