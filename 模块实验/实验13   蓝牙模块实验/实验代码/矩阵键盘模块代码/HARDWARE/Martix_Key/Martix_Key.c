#include "Martix_Key.h"	
#include "Rs485.h"
#include "delay.h"

//==========================================================
//	函数名称：	KEY_Init
//
//	函数功能：	初始化按键
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
	
			/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	
		GPIO_Initure.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;		//PA4、PA5、PA6、PA7
		GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW; //低速
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	
	  /*Configure GPIO pins : PB0 PB1 PB13 PB14 */
		GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_13|GPIO_PIN_14;
		GPIO_Initure.Mode = GPIO_MODE_INPUT;   //上拉输入
		GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_RESET);	//PA4、5、6、7置0

}


//==========================================================
//	函数名称：	KEY_ConFig
//
//	函数功能：	接收按键输入
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void KEY_ConFig(void)
{
	static uint8_t cnt = 0;  	//计数遍历
	static uint8_t fig = 0;  	//按键按下标志位
	
	HAL_Delay(10);
	switch(cnt%4){
    case 0:
      {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
				
/*
*				key 7 push down		
*/
        if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){
					
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) && fig == 0){
					//key 7 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",7);  	//发送数字7
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
/*
*				key 4 push down
*/
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && fig == 0){
					//key 4 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",4);  	//发送数字4
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
/*
*				key 1 push down
*/					
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) && fig == 0){
					//key 1 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",1);  	//发送数字1
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
/*
*				key Enter push down
*/		
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) && fig == 0){
					//key Enter push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",10);  //发送数字10
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
        }else{
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
        }
        
        break;
      }
    case 1:
      {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
				
/*
*				key 8 push down
*/	
        if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) && fig == 0){
					//key 8 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",8);  //发送数字8
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
/*
*				key 5 push down
*/		
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && fig == 0){
					//key 5 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",5);  //发送数字5
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
/*
*				key 2 push down
*/		
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) && fig == 0){
					//key 2 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",2);  //发送数字2
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
 /*
*				key 0 push down
*/	         
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) && fig == 0){
					//key 2 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",0);  //发送数字0
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
        }else{
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
        }
        break;
      }
    case 2:
      {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
				
 /*
*				key 9 push down
*/	    
        if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) && fig == 0){
					//key 9 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",9);  //发送数字9
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					
 /*
*				key 6 push down
*/	          
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && fig == 0){
					//key 6 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",6);  //发送数字6
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
							
 /*
*				key 3 push down
*/	 	
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) && fig == 0){
					//key 6 push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",3);  //发送数字3
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}
					if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) && fig)  fig = 0;		//重置标志位

 /*
*				key Canel push down
*/	           
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)){
          
					delay_ms(10);  						//延时消抖
					
					if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) && fig == 0){
					//key Canel push down
							Rs485_Config(1);			//设置485为发送模式
							printf("mrk->ble%d\r\n",11);  //发送数字11
							HAL_Delay(10);
							Rs485_Config(0);			//设置485为等待接收模式
							fig = 1;							//按键按下标志位
					}

        }else{

        }
        
        break;
      }
    case 3:
      {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
				
 /*
*				key up push down
*/	  
        if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){
          //up push down

 /*
*				key down push down
*/	            
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){
          //down push down
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
					
 /*
*				key left push down
*/	  
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)){
          //left push donw
 
 /*
*				key right push down
*/	  					
        }else if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)){
          //right 16 push down
        }else{
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
        }
        break;
      }
    }
		if(((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)) || (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))  || 
				(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13))  || (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))) &&  fig )  fig = 0;		//重置标志位
    cnt++;   //计数累加
}

    

