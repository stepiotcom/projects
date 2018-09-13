#include "stm32f1xx.h"
#include "usart.h"	
#include "Rs485.h"
#include "delay.h"
#include "TFT.h"

uint8_t pitch[6] = 		"pitch:";					//pitch
uint8_t roll[6] = 		"roll :";					//roll
uint8_t yaw[6] = 			"y a w:";					//yaw

uint8_t ffhh[2] = 			" -";					//yaw
uint8_t ffhh2[2] = 			"  ";					//yaw


int number1 = 0,number2 = 0,number3 = 0;					//存放接收到的数据

#if UART1   //使能接收

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
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
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
uint8_t GetMsg = 0;
void USART1_IRQHandler(void)                	
{ 
	unsigned char Res;
	//HAL_StatusTypeDef err;

	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res=USART1->DR; 
        __HAL_UART_CLEAR_FLAG(&UART1_Handler,UART_FLAG_RXNE);
        USART1_RX_BUF[USART1_RX_STA] = Res;
        USART1_RX_STA++;
#if 1  //接收方法1      
        if((USART1_RX_STA>2)&&(USART1_RX_BUF[USART1_RX_STA-2] == '\r')&&(USART1_RX_BUF[USART1_RX_STA-1] == '\n'))
        {//收到“\r\n”表明一个接收一个完成数据
            GetMsg        = 1;
            //Rs485_Config(1);
        }  
#else  //接收方法 2      
        switch(USART1_RX_STA)
        {
            case 0:
                 USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == 'r')?1:0;
                 break;
            case 1:
                  USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == 'f')?2:0;
                  break;
            case 2:
                 USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == 'i')?3:0;
                break;
            case 3:
                 USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == 'd')?4:0;
                break;
            case 4:
                 USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == '-')?5:0;
                break;
            case 5:
                 USART1_RX_STA = (USART1_RX_BUF[USART1_RX_STA] == '>')?6:0;
                break;            
            default:
                 USART1_RX_STA++;
                if((USART1_RX_STA>2)&&(USART1_RX_BUF[USART1_RX_STA-2] == '\r')&&(USART1_RX_BUF[USART1_RX_STA-1] == '\n'))
                {
                    GetMsg        = 1;
                    Rs485_Config(1);
                }  
                break;
        }	
#endif
	}	
} 

#endif

#if UART2

//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
unsigned char  USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

unsigned int USART2_RX_STA=0;       //接收状态标记	  

unsigned char UART2_RxBuffer[UART2_RXBUFFERSIZE];//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART2_Handler; //UART句柄

  
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
	UART2_Handler.Instance=USART2;					    //USART1
	UART2_Handler.Init.BaudRate=bound;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
    UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1
	
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
//	HAL_StatusTypeDef err;

	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res=USART2->DR; 
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
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
    
    if (huart->Instance==USART1)   //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
        __HAL_RCC_AFIO_CLK_ENABLE();

        GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
        GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
        GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

        GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
        GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
        
    #if EN_USART1_RX
        HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn,3,3);			//抢占优先级3，子优先级3
    #endif	
    }
    else if(huart->Instance==USART2)//如果是串口2，进行串口2 MSP初始化
	{

            __HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
            __HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
            
           // __HAL_RCC_AFIO_CLK_ENABLE();

            GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
            GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
            GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
            GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
            HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2

            GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
            GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
            HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3
            
        #if EN_USART2_RX
            HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
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

//==========================================================
//	函数名称：	USART1_IRQHand
//
//	函数功能：	串口一485数据处理函数
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
#define MSG_HEAD_LEN   9
const uint8_t MSG_HEAD[MSG_HEAD_LEN]="rfid->lcd";
uint8_t IC_Card_Data[17]  = {0};
uint8_t UserInputData[17] = {0};
uint8_t UserInputLenght = 0;
uint8_t UpdataGUI = 1;
void USART1_IRQHand(void)
{
    uint8_t i=0,pos;
	uint8_t len = 0;  								//存放接收到的数据长度
	Rs485_Config(0);  								//开启485接收

    if(GetMsg == 1)
    {
        GetMsg = 0;

        for(i=0;i < MSG_HEAD_LEN;i++)
        {//查找数据的头是否为MSG_HEAD指定的
            if(USART1_RX_BUF[i] != MSG_HEAD[i])
            {
                break;
            }
        }
        
        pos = i;
        if(i == MSG_HEAD_LEN)//如果i等于MSG_HEAD_LEN，表明收到的数据头与MSG_HEAD指向的相同。则进入数据分析									
        {	
            UpdataGUI = 1;
            if(USART1_RX_BUF[pos+1] == 'I')
            {
                memset((void*)UserInputData,'*',16);
                for(i=0;i<16;i++)
                {
                    if(USART1_RX_BUF[pos+3+i] != '\r')
                    {
                        UserInputData[i]=USART1_RX_BUF[pos+3+i];  
                        
                    }
                    else
                    {
                        break;
                    }
                }
                UserInputLenght = i;                
            }
            if(USART1_RX_BUF[pos+1] == 'N')
            {
                for(i=0;i<16;i++)
                {
                    IC_Card_Data[i]=USART1_RX_BUF[pos+3+i];  
                }                
            }            
        }
        USART1_RX_STA = 0;
        GetMsg = 0;
        Rs485_Config(0);
    }
}


//==========================================================
//	函数名称：	TFT_Update
//
//	函数功能：	更新显示屏数据
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：	UpdataGUI 等于0，表明不需要更新数据，
//          UpdataGUI 等于1，表明需要更新数据，	
//由于LCD_ShowString()输入的显示信息是按字符串的形式，所要保证，显示信息存储在数据组中
//最后一位为零。例如我们要显示 abc,
//UserInputData[0]='a',UserInputData[1]='b',UserInputData[3]='c',UserInputData[4]=0;
//==========================================================
void TFT_Update(void)
{			
    if(UpdataGUI == 0)														
    {
        return;
    }

    UpdataGUI = 0;                            //清零

    LCD_ShowString(75,50,UserInputData); 	 //在屏幕的坐标(75,50)上显示UserInputData(用户输入的数据)	 		

    if(strlen((char*)IC_Card_Data))
    {
            LCD_ShowString(75,100,(const uint8_t*)"                 "); //清理屏幕区				
            LCD_ShowString(75,100,(const uint8_t*)IC_Card_Data); 	    //显示读出的IC卡，卡号。			
    }	
}

