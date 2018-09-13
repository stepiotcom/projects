#include "Usart.h"
#include "Rs485.h"	

#if UART1   //使能接收

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
unsigned char  USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

unsigned int USART1_RX_STA=0;       //接收状态标记	  

unsigned char UART1_RxBuffer[UART1_RXBUFFERSIZE];//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART1_Handler; //UART句柄
  

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
	UART1_Handler.Instance=USART1;					    			//USART1
	UART1_Handler.Init.BaudRate=bound;				    		//波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B; //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE; //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    	//收发模式
	HAL_UART_Init(&UART1_Handler);					    			//HAL_UART_Init()会使能UART1
	
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
unsigned char  USART2_TX_BUF[USART2_MAX_SEND_LEN];//发送缓冲,最大USART2_MAX_SEND_LEN字节

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
	UART2_Handler.Instance          = USART2;					    			//USART1
	UART2_Handler.Init.BaudRate     = bound;				    		//波特率
	UART2_Handler.Init.WordLength   = UART_WORDLENGTH_8B; //字长为8位数据格式
	UART2_Handler.Init.StopBits     = UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity       = UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE; //无硬件流控
	UART2_Handler.Init.Mode         = UART_MODE_TX_RX;		    	//收发模式
    UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    
	HAL_UART_Init(&UART2_Handler);					    			//HAL_UART_Init()会使能UART1
	

    __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE);
}
//==========================================================
//	函数名称：	StrnCmp
//
//	函数功能：	比较两个字符的 n 位是否相等
//
//	入口参数：	uint8_t *p1,指向数据
//              uint8_t *p2,指向数据
//              uint8_t n  ,期望比较的位数
//	返回参数：	无
//
//	说明：		
//==========================================================
uint8_t StrnCmp(uint8_t *p1,uint8_t *p2,uint8_t n)
{
    uint8_t i=0,ret =1;
    
    for(i=0;i<n;i++)
    {
        if((*p1) != (*p2))
        {
            ret = 0;
            break;
        }
    }
    
    return ret;
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
unsigned char Res;
uint8_t  RecvSta = 0;
uint8_t  GetGGA_Msg=0;
void USART2_IRQHandler(void)                	
{ 
	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //USART2 的接收数据寄存器 非空
	{
        __HAL_USART_CLEAR_FLAG(&UART2_Handler,UART_FLAG_RXNE);
		Res = USART2->DR;
       
        switch(RecvSta)
        {
            case 1:
                   USART2_RX_BUF[USART2_RX_STA++]=Res;
                   if(USART2_RX_STA == 6)
                   {
                       if(StrnCmp(USART2_RX_BUF,(uint8_t*)"$GNGGA",sizeof("$GNGGA")))
                       {//如果收到的消息头是GNGGA，格式的。
                            RecvSta       = 2;   
                       }
                       else
                       {//如果不是GNGGA,格式的重新接收
                            USART2_RX_STA = 0;
                            RecvSta       = 0; 
                       }    
                   }
            break;
            case 2://如是GNGGA格式的消息，余下的数据在这里接收，
                   USART2_RX_BUF[USART2_RX_STA++] = Res;
                   if(Res == '\n')//消息以 “\r\n”结尾
                   {//接收完成
                        GetGGA_Msg = 1;                    //置位GetGGA_Msg，在main()->while(1){...},检测这个标志位
                        RecvSta    = 0;                    //RecvSta = 0,继续等待下一条协议到来
                        __HAL_UART_DISABLE(&UART2_Handler);//收到期望的数据，关闭串口2。数据处理完成后main()->while(1){...}中打开
                   }
                   
                   if(USART2_RX_STA >= USART2_REC_LEN)
                   {//USART2_RX_STA 大于 USART2_RX_BUF数据的长度，异常情况下会发生这种情况。
                        USART2_RX_STA = 0;
                   }
            break;
            default:break;
        }
        
        if(Res == '$')//GPS消息的的头均于 $ 开头，\r\n 结尾
        {
            USART2_RX_STA                  = 0;
            USART2_RX_BUF[USART2_RX_STA++] = Res;
            RecvSta                        = 1; //接收到 $ ,RecvSta 进入 状态 1进行数据接收。 
        } 
	}
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
    
    if (huart->Instance==USART1)   							//如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();						//使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();					//使能USART1时钟
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin=GPIO_PIN_9;						//PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;			//复用推挽输出
        GPIO_Initure.Pull=GPIO_PULLUP;					//上拉
        GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

        GPIO_Initure.Pin=GPIO_PIN_10;						//PA10
        GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;		//模式要设置为复用输入模式！	
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
        
    #if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn,3,3);	//抢占优先级3，子优先级3
    #endif	
    }
    else if(huart->Instance==USART2)						//如果是串口2，进行串口2 MSP初始化
	{

            __HAL_RCC_GPIOA_CLK_ENABLE();				//使能GPIOA时钟
            __HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
            
           // __HAL_RCC_AFIO_CLK_ENABLE();

            GPIO_Initure.Pin=GPIO_PIN_2;				//PA2
            GPIO_Initure.Mode=GPIO_MODE_AF_PP;	//复用推挽输出
            GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
            GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
            HAL_GPIO_Init(GPIOA,&GPIO_Initure);	//初始化PA2

            GPIO_Initure.Pin=GPIO_PIN_3;				//PA3
            GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
            HAL_GPIO_Init(GPIOA,&GPIO_Initure);	//初始化PA3
            
        #if EN_USART2_RX
            HAL_NVIC_EnableIRQ(USART2_IRQn);		//使能USART2中断通道
            HAL_NVIC_SetPriority(USART2_IRQn,3,3);			//抢占优先级3，子优先级3
        #endif	
    }
}

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
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
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (unsigned char) ch;      
	return ch;
}
#endif 

