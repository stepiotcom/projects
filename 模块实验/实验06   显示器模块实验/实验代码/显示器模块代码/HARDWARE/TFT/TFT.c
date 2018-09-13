#include "stm32f1xx.h"
#include "stdlib.h"
#include "delay.h"
#include "TFT.h"

																			//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数
u16 vx=15242,vy=11131;  							//比例因子，此值除以1000之后表示多少个AD值代表一个像素点
u16 chx=3898,chy=145;									//默认像素点坐标为0时的AD起始值
																			//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数

struct tp_pix_  tp_pixad,tp_pixlcd;	 	//当前触控坐标的AD值,前触控坐标的像素值   

uint8_t ffiigg = 0;										//校准完成标志位
uint32_t px = 0,py = 0;								//存放获取到的坐标信息
uint8_t Computing[16];								//存放按键数据信息  下标0-9 代表 数字0-9   下标10-13带边加减乘除   下标14，15分别代表  C 和  =						

u16 BACK_COLOR, POINT_COLOR;   				//背景色，画笔色	  

//==========================================================
//	函数名称：	TFT_Init
//
//	函数功能：	TFT显示器端口初始化
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void TFT_Init(void)
{
        GPIO_InitTypeDef GPIO_Initure;

        __HAL_RCC_GPIOA_CLK_ENABLE();           	//开启GPIOA时钟
        __HAL_RCC_GPIOB_CLK_ENABLE();           	//开启GPIOB时钟	

        __HAL_RCC_AFIO_CLK_ENABLE();
        __HAL_AFIO_REMAP_SWJ_NOJTAG();

        //TFT显示器模块
        GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_7;   //TFT_CS,TFT_RST,TFT_CLK,
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;	//低速
        GPIO_Initure.Mode =  GPIO_MODE_OUTPUT_PP;//推挽输出
        GPIO_Initure.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
        GPIO_Initure.Pin=GPIO_PIN_6;    //PA6
        GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
        GPIO_Initure.Pull=GPIO_NOPULL;         //浮空
        GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);

        GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1;   //TFT_DC  T_CS
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Initure.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
        GPIO_Initure.Pin=GPIO_PIN_14;    //PB14
        GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
        GPIO_Initure.Pull=GPIO_PULLDOWN;         //上拉
        GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    


}

//==========================================================
//	函数名称：	LCD_Writ_Bus
//
//	函数功能：	串行数据写入
//
//	入口参数：	da:需要写入的数据
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void  LCD_Writ_Bus(char da)
{
     char i;
     for(i=0;i<8;i++)
     {
			 if(da &0x80)
			 {
				 LCD_SDI_H;
			 }
			 else LCD_SDI_L;
			 LCD_SCK_L;
			 LCD_SCK_H;
			 da <<=1;
     }	
}

//==========================================================
//	函数名称：	LCD_WR_DATA8
//
//	函数功能：	发送数据-8位参数
//
//	入口参数：	da:需要写入的数据
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_WR_DATA8(char da) //发送数据-8位参数
{
  LCD_DC_H;
	LCD_Writ_Bus(da);
}  

//==========================================================
//	函数名称：	LCD_WR_DATA
//
//	函数功能：	发送数据参数
//
//	入口参数：	da:需要写入的数据
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_WR_DATA(int da)
{
  LCD_DC_H;
	LCD_Writ_Bus(da>>8);
	LCD_Writ_Bus(da);
}	  

//==========================================================
//	函数名称：	LCD_WR_REG
//
//	函数功能：	REG参数写入
//
//	入口参数：	da:需要写入的数据
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_WR_REG(char da)	 
{
  LCD_DC_L;
	LCD_Writ_Bus(da);
}

//==========================================================
//	函数名称：	LCD_WR_REG_DATA
//
//	函数功能：	REG参数写入
//
//	入口参数：	da:需要写入的数据
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
 void LCD_WR_REG_DATA(int reg,int da)
{
  LCD_WR_REG(reg);
	LCD_WR_DATA(da);
}

//==========================================================
//	函数名称：	Address_set
//
//	函数功能：	REG参数写入
//
//	入口参数：	x1,y1,x2,y2为坐标点
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{  
   LCD_WR_REG(0x2a);
   LCD_WR_DATA8(x1>>8);
   LCD_WR_DATA8(x1);
   LCD_WR_DATA8(x2>>8);
   LCD_WR_DATA8(x2);
  
   LCD_WR_REG(0x2b);
   LCD_WR_DATA8(y1>>8);
   LCD_WR_DATA8(y1);
   LCD_WR_DATA8(y2>>8);
   LCD_WR_DATA8(y2);

   LCD_WR_REG(0x2C);

}

//==========================================================
//	函数名称：	Lcd_Init
//
//	函数功能：	TFT寄存器初始化
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void Lcd_Init(void)
{

//调用一次这些函数，免得编译的时候提示警告
//   	LCD_CS_H;
// 	if(LCD_CS==0)
// 	{
// 	   LCD_WR_REG_DATA(0,0);
// 	   LCD_ShowString(0,0," ");
// 	   LCD_ShowNum(0,0,0,0);
// 	   LCD_Show2Num(0,0,0,0);
// 	   LCD_DrawPoint_big(0,0);
// 	   LCD_DrawRectangle(0,0,0,0);
// 	   Draw_Circle(0,0,0);
//  	 }    
	LCD_REST_L;
	delay_ms(10);
	LCD_REST_H;
	delay_ms(10);
	LCD_CS_L;  //打开片选使能

	LCD_WR_REG(0xCB);  
	LCD_WR_DATA8(0x39); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x34); 
	LCD_WR_DATA8(0x02); 

	LCD_WR_REG(0xCF);  
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0XC1); 
	LCD_WR_DATA8(0X30); 
 	
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA8(0x85); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x78); 

	LCD_WR_REG(0xEA);  
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x00); 
 	
	LCD_WR_REG(0xED);  
	LCD_WR_DATA8(0x64); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0X12); 
	LCD_WR_DATA8(0X81); 

	LCD_WR_REG(0xF7);  
	LCD_WR_DATA8(0x20); 
 
	LCD_WR_REG(0xC0);    	//Power control 
	LCD_WR_DATA8(0x23);   //VRH[5:0] 
 
	LCD_WR_REG(0xC1);    	//Power control 
	LCD_WR_DATA8(0x10);   //SAP[2:0];BT[3:0] 

	LCD_WR_REG(0xC5);    	//VCM control 
	LCD_WR_DATA8(0x3e); 	//对比度调节
	LCD_WR_DATA8(0x28); 

	LCD_WR_REG(0xC7);    	//VCM control2 
	LCD_WR_DATA8(0x86); 	//--

	LCD_WR_REG(0x36);    	// Memory Access Control 
	LCD_WR_DATA8(0x48); 	//	   //48 68竖屏//28 E8 横屏

	LCD_WR_REG(0x3A);    
	LCD_WR_DATA8(0x55); 

	LCD_WR_REG(0xB1);    
	LCD_WR_DATA8(0x00);  
	LCD_WR_DATA8(0x18); 

	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x82);
	LCD_WR_DATA8(0x27);  
 	
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA8(0x00); 
 	
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA8(0x01); 
 
 	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_DATA8(0x31); 
	LCD_WR_DATA8(0x2B); 
	LCD_WR_DATA8(0x0C); 
	LCD_WR_DATA8(0x0E); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x4E); 
	LCD_WR_DATA8(0xF1); 
	LCD_WR_DATA8(0x37); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x10); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x0E); 
	LCD_WR_DATA8(0x09); 
	LCD_WR_DATA8(0x00); 

	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x0E); 
	LCD_WR_DATA8(0x14); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x11); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x31); 
	LCD_WR_DATA8(0xC1); 
	LCD_WR_DATA8(0x48); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_DATA8(0x0C); 
	LCD_WR_DATA8(0x31); 
	LCD_WR_DATA8(0x36); 
	LCD_WR_DATA8(0x0F); 
 
	LCD_WR_REG(0x2A);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xEF);

	LCD_WR_REG(0x2B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x3F);
	LCD_WR_REG(0x11);    	//Exit Sleep 
	delay_ms(60); 
			
  LCD_WR_REG(0x29);    	//Display on 
  LCD_WR_REG(0x2c); 
  BACK_COLOR=WHITE; 		//设置TFT液晶背景色
  POINT_COLOR=RED;  		//设置TFT液晶前景色
}

//==========================================================
//	函数名称：	LCD_Clear
//
//	函数功能：	清屏函数
//
//	入口参数：	Color:要清屏的填充色
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_Clear(u16 Color)
{
	u8 VH,VL;
	u16 i,j;
	VH=Color>>8;
	VL=Color;	
	Address_set(0,0,LCD_W-1,LCD_H-1);
	for(i=0;i<LCD_W;i++)
	 {
		for (j=0;j<LCD_H;j++)
			{
					LCD_WR_DATA8(VH);
					LCD_WR_DATA8(VL);	
			}
		}
}

//==========================================================
//	函数名称：	LCD_DrawPoint
//
//	函数功能：	画点
//
//	入口参数：	位置光标
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_DrawPoint(u16 x,u16 y)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(POINT_COLOR); 	    
} 	 

//==========================================================
//	函数名称：	LCD_DrawPoint_big
//
//	函数功能：	画一个大点
//
//	入口参数：	位置光标
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_DrawPoint_big(u16 x,u16 y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 

//  (xend-xsta)*(yend-ysta)
//==========================================================
//	函数名称：	LCD_Fill
//
//	函数功能：	在指定区域内填充指定颜色
//
//	入口参数：	区域大小: (xend-xsta)*(yend-ysta)
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Address_set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置 	    
	} 					  	    
}  

//==========================================================
//	函数名称：	LCD_DrawLine
//
//	函数功能：	画线
//
//	入口参数：	x1,y1:起点坐标
//              x2,y2:终点坐标  
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    

//==========================================================
//	函数名称：	LCD_DrawRectangle
//
//	函数功能：	画矩形
//
//	入口参数：	区域大小: (x2-x1)*(y2-y1)
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

//==========================================================
//	函数名称：	Draw_Circle
//
//	函数功能：	在指定位置画一个指定大小的圆
//
//	入口参数：	(x,y):中心点
//              r    :半径
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 

//==========================================================
//	函数名称：	LCD_ShowChar
//
//	函数功能：	在指定位置显示一个字符
//
//	入口参数：	(x,y):显示坐标
//              num:要显示的字符:" "--->"~"
//              mode:叠加方式(1)还是非叠加方式(0)
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>LCD_W-16||y>LCD_H-16)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	Address_set(x,y,x+8-1,y+16-1);      //设置光标位置 
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_WR_DATA(POINT_COLOR);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 

//==========================================================
//	函数名称：	LCD_ShowNum
//
//	函数功能：	显示多个数字
//
//	入口参数：	x,y :起点坐标	 
//              len :数字的位数
//              num:数值(0~4294967295);
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	num=(u32)num;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0); 
	}
} 

//==========================================================
//	函数名称：	LCD_Show2Num
//
//	函数功能：	显示2个数字
//
//	入口参数：	x,y :起点坐标	 
//              len :数字的位数
//              num:数值(0~99);
//              
//	返回参数：	无
//
//	说明：		
//==========================================================
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len)
{         	
	u8 t,temp;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
	 	LCD_ShowChar(x+8*t,y,temp+'0',0); 
	}
} 

//==========================================================
//	函数名称：	LCD_ShowString
//
//	函数功能：	显示字符串
//
//	入口参数：	x,y:起点坐标  
//              *p:字符串起始地址
//              
//	返回参数：	无
//
//	说明：	    用16字体	
//==========================================================
void  LCD_ShowString(u16 x,u16 y,const u8 *p)
{
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;}
        LCD_ShowChar(x,y,*p,0);
        x+=8;
        p++;
    } 
}


//==========================================================
//	函数名称：	Drow_Touch_Point
//
//	函数功能：	与LCD部分有关的函数  
//
//	入口参数：	x,y:起点坐标  
//              
//	返回参数：	无
//
//	说明：	   	用来校准用的
//==========================================================
void Drow_Touch_Point(u16 x,u16 y)
{
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
//	Draw_Circle(x,y,6);//画中心圈
}	
u16 ReadFromCharFrom7843()             //SPI 读数据
{
	u8 count=0;
	u16 Num=0;
	for(count=0;count<12;count++)
	{
		Num<<=1;		
		LCD_SCK_H;               //下降沿有效
		LCD_SCK_L; 
		if(T_OUT)
		{
			Num|=1;
		}
		
	}

return(Num);
}
//从7846/7843/XPT2046/UH7843/UH7846读取adc值	  0x90=y   0xd0-x
u16 ADS_Read_AD(unsigned char CMD)          
{
    u16 l;
    T_CS_L;     
    LCD_Writ_Bus(CMD);//送控制字即用差分方式读X坐标 详细请见有关资料
    LCD_SCK_H;
    LCD_SCK_L;
    l=ReadFromCharFrom7843();
    T_CS_H;
    return l;
}	

//==========================================================
//	函数名称：	ADS_Read_XY
//
//	函数功能：	读取一个坐标值
//
//	入口参数：	 
//              
//	返回参数：	无
//
//	说明：	   	连续读取READ_TIMES次数据,对这些数据升序排列,
//							然后去掉最低和最高LOST_VAL个数,取平均值 
//==========================================================
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

//==========================================================
//	函数名称：	Read_ADS
//
//	函数功能：	带滤波的坐标读取
//
//	入口参数：	 
//              
//	返回参数：	无
//
//	说明：	   	最小值不能少于100.
//==========================================================
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	 									   
	if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

//==========================================================
//	函数名称：	Read_ADS2
//
//	函数功能：	2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//
//	入口参数：	 
//              
//	返回参数：	无
//
//	说明：	   	该函数能大大提高准确度
//==========================================================
#define ERR_RANGE 20 //误差范围 
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-ERR_RANGE内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)>>1;
        *y=(y1+y2)>>1;		
        return 1;
    }else return 0;	  
} 

  
//==========================================================
//	函数名称：	Read_TP_Once
//
//	函数功能：	精确读取一次坐标,校准的时候用的	 
//
//	入口参数：	 
//              
//	返回参数：	无
//
//	说明：	   	
//==========================================================
u8 Read_TP_Once(void)
{
	u8 re=0;
	u16 x1,y1;
	while(re==0)
	{
		while(!Read_ADS2(&tp_pixad.x,&tp_pixad.y));
		delay_ms(10);
		while(!Read_ADS2(&x1,&y1));
		if(tp_pixad.x==x1&&tp_pixad.y==y1)
		{
			re=1; 
		}
	} 
	return re;
}


//==========================================================
//	函数名称：	TFT_UI
//
//	函数功能：	TFT计算器界面
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：	
//==========================================================
void TFT_UI(void)
{
	LCD_Clear(WHITE);													//清屏
	POINT_COLOR = BLACK;											//画笔颜色
	LCD_DrawRectangle(10,10,230,90);					//显示屏
	
/*
	*
	*			第一排
	*
*/	
	
	LCD_DrawRectangle(16,110,56,150);							
	LCD_ShowChar(34,123,'+',1);					//	+
	
	LCD_DrawRectangle(72,110,112,150);	
	LCD_ShowChar(90,123,'-',1);					//	-
	
	LCD_DrawRectangle(128,110,168,150);	
	LCD_ShowChar(146,123,'*',1);				//	*
	
	LCD_DrawRectangle(184,110,224,150);	
	LCD_ShowChar(202,123,'/',1);				//	/
	
/*
	*
	*			第二排
	*
*/		
	
	LCD_DrawRectangle(16,160,56,200);							
	LCD_ShowChar(34,175,'9',1);					//	9
	
	LCD_DrawRectangle(72,160,112,200);							
	LCD_ShowChar(90,175,'8',1);					//	8
	
	LCD_DrawRectangle(128,160,168,200);						
	LCD_ShowChar(146,175,'7',1);				//	7
	
	LCD_DrawRectangle(184,160,224,200);							
	LCD_ShowChar(202,175,'6',1);				//	6
	
	
/*
	*
	*			第三排
	*
*/
	LCD_DrawRectangle(16,210,56,250);							
	LCD_ShowChar(34,224,'5',1);					//	5
	
	LCD_DrawRectangle(72,210,112,250);							
	LCD_ShowChar(90,224,'4',1);					//	4
	
	LCD_DrawRectangle(128,210,168,250);						
	LCD_ShowChar(146,224,'3',1);				//	3
	
	LCD_DrawRectangle(184,210,224,250);							
	LCD_ShowChar(202,224,'2',1);				//	2
	
	
/*
	*
	*			第四排
	*
*/
	LCD_DrawRectangle(16,260,56,300);								
	LCD_ShowChar(34,275,'1',1);					//	1
	
	LCD_DrawRectangle(72,260,112,300);							
	LCD_ShowChar(90,275,'0',1);					//	0
	
	LCD_DrawRectangle(128,260,168,300);						
	LCD_ShowChar(146,275,'C',1);				//	C
	
	LCD_DrawRectangle(184,260,224,300);							
	LCD_ShowChar(202,275,'=',1);				//	=
	
}

//==========================================================
//	函数名称：	TFT_Computing
//
//	函数功能：	TFT计算器处理
//
//	入口参数：	无
//              
//	返回参数：	无
//
//	说明：	
//==========================================================


void TFT_Computing(void)
{
	uint16_t i = 0;																			//for循环使用
	uint8_t cls[50] = "                          ";			//清显示屏	
	static uint8_t count1 = 0,count2 = 0;								//记录按下数字次数
	static uint8_t touch_fig = 0;												//接触完成标志位
	static char Symbol2;																//存放运算符
	static uint8_t number = 0;													//存放数据
	static uint32_t num1 = 0,num2 = 0,Symbol = 0;				//num1：数值1   num2：数值2   Symbol：运算符号
	static uint32_t Result = 0;													//计算结果
	
	/*
	*
	*					获取按键值
	*
	*/
		if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0)			//按键按下了
		{
			if(Read_TP_Once())															//得到单次按键值
			{  								   
					px=tp_pixad.x;
					py=tp_pixad.y;
					LCD_ShowString(20,43,cls);									//清屏
			}			 
			while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))  		//等待松手
			{	
				LCD_ShowString(20,43,cls);										//清屏	
			}
			
			touch_fig = 1;																	//一次接触完成

		}

	
	/*
	*
	*					获取并处理数据
	*
	*/
	if(touch_fig)
	{
		//清空接收数组
		for(i = 0;i<16;i++){
			Computing[i] = 0;
		}
		
		if(px>2179 && px<2804 && py>3081 && py<3523)				//	0
		{
			Computing[0] = 1;
		}
		else if(px>3051 && px<3661 && py>3091 && py<3537)		//	1
		{
			Computing[1] = 1;
		}
		else if(px>459 && px<1083 && py>2491 && py<2959)		//	2
		{
			Computing[2] = 1;
		}
		else if(px>1312 && px<1932 && py>2552 && py<3001)		//	3
		{
			Computing[3] = 1;
		}
		else if(px>2195 && px<2815 && py>2531 && py<2989)		//	4
		{
			Computing[4] = 1;
		}
		else if(px>3052 && px<3671 && py>2545 && py<2968)		//	5
		{
			Computing[5] = 1;
		}
		else if(px>476 && px<1052 && py>1967 && py<2428)		//	6
		{
			Computing[6] = 1;
		}
		else if(px>1314 && px<1941 && py>1934 && py<2387)		//	7
		{
			Computing[7] = 1;
		}
		else if(px>2196 && px<2818 && py>1960 && py<2423)		//	8
		{
			Computing[8] = 1;
		}
		else if(px>3064 && px<3661 && py>1994 && py<2437)		//	9
		{
			Computing[9] = 1;
		}	
		else if(px>3122 && px<3618 && py>1374 && py<1864)		//	+
		{
			Computing[10] = 1;
		}
		else if(px>2141 && px<2863 && py>1404 && py<1851)		//	-
		{
			Computing[11] = 1;
		}
		else if(px>1312 && px<1926 && py>1420 && py<1841)		//	*
		{
			Computing[12] = 1;
		}
		else if(px>520 && px<1093 && py>1394 && py<1820)		//	/
		{
			Computing[13] = 1;
		}
		else if(px>1312 && px<1959 && py>3110 && py<3545)		//	C
		{
			Computing[14] = 1;
		}
		else if(px>441 && px<1042 && py>3075 && py<3544)		//	=
		{
			Computing[15] = 1;
		}
		
		touch_fig = 0;						//清零标志位
		
		for(i = 0;i<16;i++)
		{
			if(Computing[i] == 1)
			{
				number = i;						//获取到按键数据
			}
		}

		
	/*
	*
	*					进行计算
	*
	*/
		if(Symbol == 0 && number<10)									//还没有获取到运算符		
		{
			count1+=1;
			if(count1<10)
				num1 = num1*10+number;
			LCD_ShowNum(20,43,num1,9);									//显示第一个数值

		}
		else																					//已经获取到运算符
		{		
			switch(number)															//获取符号
			{
				case 10: 																	//	+
						Symbol = 10;
						Symbol2 = '+';
						LCD_ShowNum(20,43,num1,9);						//显示第一个数值
						LCD_ShowChar(100,43,Symbol2,1);				//显示运算符
				break;
				case 11: 																	//	- 
						Symbol = 11;
						Symbol2 = '-';
						LCD_ShowNum(20,43,num1,9);						//显示第一个数值
						LCD_ShowChar(100,43,Symbol2,1);				//显示运算符
				break;
				case 12: 																	//	* 
						Symbol = 12;
						Symbol2 = '*';
						LCD_ShowNum(20,43,num1,9);						//显示第一个数值
						LCD_ShowChar(100,43,Symbol2,1);				//显示运算符
				break;
				case 13: 																	//	/
						Symbol = 13;
						Symbol2 = '/';
						LCD_ShowNum(20,43,num1,9);						//显示第一个数值
						LCD_ShowChar(100,43,Symbol2,1);				//显示运算符
				break;
				default : break;
			}
			
			if(Symbol>9&&Symbol<14&&number<10)					//已经获取到运算符以后
			{
				count2+=1;
				if(count2<10)
					num2 = num2*10+number;									//获取第二个数值
				LCD_ShowNum(20,43,num1,9);								//显示第一个数值
				LCD_ShowChar(100,43,Symbol2,1);						//显示运算符
				LCD_ShowNum(120,43,num2,9);								//显示第二个数值
			}
			
			if(number ==14)															//获取到C  清零数值
			{
				num1=num2=number=Symbol=Result=count1=count2=0;	//清空所有数值
				LCD_ShowString(20,43,cls);								//清屏	
			}
			
			if(number == 15)														//获取到等于符号
			{
				switch(Symbol)														//获取符号
				{
					case 10: 																//	+
							Result = num1+num2;
					break;
					case 11: 																//	- 
							Result = num1-num2;
					break;
					case 12: 																//	* 
							Result = num1*num2;
					break;
					case 13: 																//	/
							Result = num1/num2;
					break;
					default : 
							Result = num1;				
					break;
				}
				LCD_ShowString(20,43,cls);								//清屏	
				if(((count1+count2)>=12 && Symbol ==12))
					LCD_ShowString(20,43,"Out Of Range !");	//超出值域
				else	
					LCD_ShowNum(30,43,Result,10);						//将结果打印到屏幕上
												
					

				num1=num2=number=Symbol=Result=count1=count2=0;	//清空所有数值
				
			}
		
		}		
		
	}

}






