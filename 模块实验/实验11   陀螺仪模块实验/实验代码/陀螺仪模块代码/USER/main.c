#include "stm32f1xx.h"
#include "delay.h"
#include "MPU6050.h"
#include "Usart.h"
#include "Rs485.h"
#include "math.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
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
*	说明： 		陀螺仪库函数操作实验-HAL库版本
*
*	修改记录：	
************************************************************
************************************************************
************************************************************
**/

int main(void)
{ 
	float pitch,roll,yaw; 		//欧拉角
	int number1 = 0,number2 = 0,number3 = 0;
	HAL_Init();             	//初始化HAL库   
  Rs485_Init();  						//初始化485
	UART1_Init(115200);				//初始化串口1
	
	if(mpu_dmp_init()) 
	{
			delay_ms(1000);
      mpu_dmp_init();	
	}	
	while(1)
	{
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
				
			number1 = pitch/1;
			Rs485_Config(1); 									//485发送模式
			printf("pit->lcd%d\r\n",number1);	//将数据发送到485总线中
			delay_ms(2);
			Rs485_Config(0); 									//485等待模式
			
			delay_ms(2);

			number2 = roll/1;
			Rs485_Config(1); 									//485发送模式
			printf("rol->lcd%d\r\n",number2);	//将数据发送到485总线中
			delay_ms(2);
			Rs485_Config(0); 									//485等待模式
			
			delay_ms(2);
			
			number3 = yaw/1;
			Rs485_Config(1); 									//485发送模式
			printf("yaw->lcd%d\r\n",number3);	//将数据发送到485总线中
			delay_ms(2);
			Rs485_Config(0); 									//485等待模式
			
			delay_ms(2);
		}            
	}

}


