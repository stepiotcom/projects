#include "delay.h"
#include "Usart.h"
#include "Rs485.h"
#include "delay.h"
#include "timer.h"

uint16_t i = 0;																		//清空数组时For循环使用
uint8_t fag = 1;																	//printf  函数转换(默认串口二发送)
uint8_t USART_AT_STA = 0;													//AT指令发送完成标志位

#if UART1   																			//使能接收

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
unsigned char  USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

unsigned int USART1_RX_STA=0;       							//接收状态标记	  

unsigned char UART1_RxBuffer[UART1_RXBUFFERSIZE];	//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART1_Handler; 								//UART句柄
  

//==========================================================
//	函数名称：	UART1_Init
//
//	函数功能：	串口1初始化
//
//	入口参数：	bound:波特率
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void UART1_Init(int bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    				//USART1
	UART1_Handler.Init.BaudRate=bound;				    			//波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	   		//一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    	//无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    		//收发模式
	HAL_UART_Init(&UART1_Handler);					    				//HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (unsigned char *)UART1_RxBuffer, UART1_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}


//==========================================================
//	函数名称：	USART1_IRQHandler
//
//	函数功能：	串口1中断服务程序
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART1_IRQHandler(void)                	
{ 
	unsigned char Res;
	//HAL_StatusTypeDef err;

	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res=USART1->DR; 
		if((USART1_RX_STA&0x8000)==0)			//接收未完成
		{
			if(USART1_RX_STA&0x4000)				//接收到了0x0d
			{
				if(Res!=0x0a)USART1_RX_STA=0;	//接收错误,重新开始
				else USART1_RX_STA|=0x8000;		//接收完成了 
			}
			else 														//还没收到0X0D
			{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	}
	HAL_UART_IRQHandler(&UART1_Handler);	
} 

#endif

#if UART2

//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
unsigned char  USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

unsigned int USART2_RX_STA=0;       							//接收状态标记	  

unsigned char UART2_RxBuffer[UART2_RXBUFFERSIZE];	//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART2_Handler; 								//UART句柄

  
//==========================================================
//	函数名称：	UART2_Init
//
//	函数功能：	串口2初始化
//
//	入口参数：	bound:波特率
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void UART2_Init(int bound)
{	
	//UART 初始化设置
	UART2_Handler.Instance=USART2;					    								//USART1
	UART2_Handler.Init.BaudRate=bound;				    							//波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   				//字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    					//一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    					//无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   				//无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    						//收发模式
    UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART2_Handler);					    								//HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART2_Handler, (unsigned char *)UART2_RxBuffer, UART2_RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2中断服务程序
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)                	
{ 
	unsigned char Res;

	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断
	{
		Res=USART2->DR; 
		if((USART2_RX_STA&0x8000)==0)				//接收未完成
		{
			if(USART_AT_STA)									//AT指令发送完成
			{
				if(USART2_RX_STA&0x4000)				//接收到了0x30
				{
					if(Res!=0x30)USART2_RX_STA=0;	//接收错误,重新开始
					else USART2_RX_STA|=0x8000;		//接收完成了 
				}
				else 														//还没收到0X30
				{	
					if(Res==0x30)USART2_RX_STA|=0x4000;
					else
					{
						USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
						USART2_RX_STA++;
						if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
			else																	//AT指令未发送完成
			{
				TIM_ESP8266_FIG(1);									//开始计数
				number = 0;													//清零计数值
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
				USART2_RX_STA++;
				if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收		
			}
		}   		 
	}
	HAL_UART_IRQHandler(&UART2_Handler);	
} 

#endif


//==========================================================
//	函数名称：	HAL_UART_MspInit
//
//	函数功能：	UART底层初始化，时钟使能，引脚配置，中断配置
//
//	入口参数：	huart:串口句柄
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
    
    if (huart->Instance==USART1)   								//如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();							//使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();						//使能USART1时钟
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin=GPIO_PIN_9;							//PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;				//复用推挽输出
        GPIO_Initure.Pull=GPIO_PULLUP;						//上拉
        GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;	//高速
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   		//初始化PA9

        GPIO_Initure.Pin=GPIO_PIN_10;							//PA10
        GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;			//模式要设置为复用输入模式！	
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   		//初始化PA10
        
    #if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);					//使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn,3,3);		//抢占优先级3，子优先级3
    #endif	
    }
  else if(huart->Instance==USART2)								//如果是串口2，进行串口2 MSP初始化
	{

				__HAL_RCC_GPIOA_CLK_ENABLE();							//使能GPIOA时钟
				__HAL_RCC_USART2_CLK_ENABLE();						//使能USART2时钟
				
			 // __HAL_RCC_AFIO_CLK_ENABLE();

				GPIO_Initure.Pin=GPIO_PIN_2;							//PA2
				GPIO_Initure.Mode=GPIO_MODE_AF_PP;				//复用推挽输出
				GPIO_Initure.Pull=GPIO_PULLUP;						//上拉
				GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;	//高速
				HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   		//初始化PA2

				GPIO_Initure.Pin=GPIO_PIN_3;							//PA3
				GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;			//模式要设置为复用输入模式！	
				HAL_GPIO_Init(GPIOA,&GPIO_Initure);	  		//初始化PA3
				
		#if EN_USART2_RX
				HAL_NVIC_EnableIRQ(USART2_IRQn);					//使能USART2中断通道
				HAL_NVIC_SetPriority(USART2_IRQn,3,3);		//抢占优先级3，子优先级3
		#endif	
    }
	
}



//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
 if(fag){
 	while((USART2->SR&0X40)==0); //循环发送,直到发送完毕   
    USART2->DR = (uint8_t) ch;  
 }else{
	 	while((USART1->SR&0X40)==0); //循环发送,直到发送完毕   
    USART1->DR = (uint8_t) ch;  
 }

	
	return ch;
}
#endif 


//==========================================================
//	函数名称：	atk_8266_check_cmd
//
//	函数功能：	ATK-ESP8266发送命令后,检测接收到的应答
//
//	入口参数：	str:期待的应答结果
//
//	返回参数：	
//						返回值:0,没有得到期待的应答结果
//    				其他,期待应答结果的位置(str的位置)
//
//	说明：		
//==========================================================
uint8_t* atk_8266_check_cmd(uint8_t *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_STA=0;
		TIM_ESP8266_FIG(0);				//关闭计数
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}

//==========================================================
//	函数名称：	atk_8266_send_cmd
//
//	函数功能：	向ATK-ESP8266发送命令
//
//	入口参数：	waittime:等待时间(单位:ms)
//							cmd:发送的命令字符串
//							ack:期待的应答结果,如果为空,则表示不需要等待应答
//
//	返回参数：	
//						返回值:	0,发送成功(得到了期待的应答结果)
//       							1,发送失败
//
//	说明：		
//==========================================================
void atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	do
	{
		for(i = 0;i<600;i++){
			USART2_RX_BUF[i] = 0;						//清空数组
		}
		printf("%s",cmd);									//发送命令
		delay_ms(waittime);
		if(number>=10)
		{
			USART2_RX_STA|=0x8000;					//接收完成了 
		}
	}
	while(!atk_8266_check_cmd(ack));
} 


//==========================================================
//	函数名称：	USART1_IRQHand
//
//	函数功能：	串口一处理串口中断数据
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
	uint32_t number1 = 0;				//存放接收到的数据
void USART1_IRQHand(void)
{
	char *strx=0;								//接收对比结果

	Rs485_Config(0);   					//设置485为接收模式

  if(USART1_RX_STA&0x8000)
	{

		strx=strstr((const char*)USART1_RX_BUF,(const char*)"->wif");			//判断总线数据是否为传输给wif的数据
		
		if(strx)	//如果数据为传输给wif的数据则取出数据显示
		{	
			number1 = ((USART1_RX_BUF[15]-0x30)+(USART1_RX_BUF[14]-0x30)*10 +(USART1_RX_BUF[13]-0x30)*100 
			+ (USART1_RX_BUF[12]-0x30)*1000+(USART1_RX_BUF[11]-0x30)*10000 + (USART1_RX_BUF[10]-0x30)*100000 
			+ (USART1_RX_BUF[9]-0x30)*1000000+(USART1_RX_BUF[8]-0x30)*10000000);
			if(USART_AT_STA)
			{
				delay_ms(500);						//延时500ms使数据稳定
				printf("%d\r\n",number1);	//发送温湿度数据
			}
		}

		USART1_RX_STA=0;
		for(i = 0;i<200;i++){
			USART1_RX_BUF[i] = 0;						//清空数组
		}
	}
}

//==========================================================
//	函数名称：	USART2_IRQHand
//
//	函数功能：	串口二处理串口中断数据
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHand(void)
{
	int number2=  0;  
	uint8_t len;
	
  if(USART2_RX_STA&0x8000)
	{

		len=USART2_RX_STA&0x3fff;	//得到此次接收到的数据长度

		number2 = ((USART2_RX_BUF[len-1]-0x30)+(USART2_RX_BUF[len-2]-0x30)*10);
		
		fag = 0;  								//串口一发送数据
		Rs485_Config(1);   				//设置485为发送模式
		printf("%d\r\n",number2);	//将串口二接收到的数据发送到485中
		delay_ms(10);
		Rs485_Config(0);   				//设置485为接收模式
		delay_ms(1);
		fag = 1;  								//串口二发送数据

		printf("%d\r\n",number2);	//将串口二接收到的数据发送到485中
		
		USART2_RX_STA=0;
		for(i = 0;i<600;i++){
			USART2_RX_BUF[i] = 0;						//清空数组
		}
	}
}


