#include "stm32f1xx_hal.h"  
#include "Usart.h"					   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "delay.h"
#include "math.h"
#include "GPS.h"

//==========================================================
//	函数名称：	NMEA_Pow
//
//	函数功能：	buf指向的数组中，查找第cx个“,”逗号并返回此“,”逗号的相对buf地址偏移位置
//
//	入口参数：	u8 *buf，输入，底数。
//              u8 cx ， 输入，指数
//	返回参数：	返回相对buf地址偏移位置
//
//	说明：		
//==========================================================						  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//==========================================================
//	函数名称：	NMEA_Pow
//
//	函数功能：	求m^n次方，m为unsigned char
//
//	入口参数：	u8 m，输入，底数。
//              u8    n ，输入，指数
//	返回参数：	返回转换后的结果
//
//	说明：		
//==========================================================
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
//==========================================================
//	函数名称：	pow_f
//
//	函数功能：	求m^n次方，m为浮点型数据
//
//	入口参数：	float m，输入，底数。
//              u8    n ，输入，指数
//	返回参数：	返回转换后的结果
//
//	说明：		
//==========================================================
float pow_f(float m,u8 n)
{
	float result = 1;	 
	while(n--)
    {
        result*=m; 
    }        
	return result;
}
//==========================================================
//	函数名称：	NMEA_Str2num
//
//	函数功能：	将字符串的数据，转换成int型。遇到“，”(逗号)，“*”(结束)
//
//	入口参数：	u8 *buf，输入，待转换的字符串的指针。
//              u8 *dx ，输出，返回小数点之后的位数
//	返回参数：	返回转换后的结果
//
//	说明：		
//==========================================================
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*'))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))		//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;		//最多取5位小数
	*dx=flen;	 					//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}
//==========================================================
//	函数名称：	GPS_PosInfoConvert
//
//	函数功能：	将GPS数据中纬度、经度的格式,转成单位为度的格式
//
//	入口参数    double pos ，输入，位置消息。
//	返回参数：	返回0，数据无效，返回1，数据有效
//
//	GPS的数据格式是DDMM.MMMMMM
//获取到的GPS帧数据比如是：$GNRMC,112317.000,A,3438.1633,N,11224.4992,E,0.19,186.95,240916,,,A*7D
//说明
//经度112°24.4992′
//纬度34°38.1633′
//转为以度为单位DD.DDDDD,因此需要转换
//经度为 112+24.4992/60 = 112.40832
//纬度为 34+38.1633/60 = 34.636055
//==========================================================
double GPS_PosInfoConvert(double pos)
{//pos=12345,78901234
  
    double temp1,temp2;
    uint32_t    i,j;
    
    temp1  = pos;            //temp1 = 12345.78901234
    j     = (uint32_t)temp1; //j     = 12345
    temp1 = temp1/100;       //temp1 = 123.4578901234
    i    = (uint32_t)temp1;  //i     = 123
    i    = i*100;            //i     = 12300;
    temp2 = pos - i;         //temp2 = 12345.78901234 - 12300 = 45.78901234
    
    //j/100    = 123
    //temp2/60 = 45.78901234/60
    //123 + (45.78901234)/60
    return (j/100 + temp2/60);
    
}
//==========================================================
//	函数名称：	GPS_GGA_Analysis
//
//	函数功能：	解析GGA格式数据，结果保存在gpsx结构体中。
//
//	入口参数：	GPS_MsgTypeDef *gpsx，输出，保存解析后的数据。
//              u8 *buf             ，输入，接收到的GGA格式的数据。
//	返回参数：	返回0，数据无效，返回1，数据有效
//
//	说明：		造成数据无效的原因，设备处于开机之后的1分钟内，或者天线无法接收到信号
//==========================================================
uint8_t GPS_GGA_Analysis(GPS_MsgTypeDef *gpsx,u8 *buf)
{
    uint8_t ret = 0;
    
	u8 *p1,dx,i=0,j=0;			 
	u8 posx;     
	p1 = buf;
    
	posx=NMEA_Comma_Pos(p1,6);								
    GPS_GGA_Msg.MsgSta = NMEA_Str2num(p1+posx,&dx);	//得到数据是否有效
    
    if(GPS_GGA_Msg.MsgSta == 1)
    {//定位有效,则进行位置信息提取
//******************提取纬度信息*****************************************************<   
//例：在消息格式中的字符串“12345.7890123”转成double 数据类型 12345.7890123       
        posx            = NMEA_Comma_Pos(p1,2);	
        j = 1;
        GPS_GGA_Msg.latitude = 0;        
        while(buf[posx + i] != '.')//获取.号前的12345
        {
            GPS_GGA_Msg.latitude = GPS_GGA_Msg.latitude*10;
            GPS_GGA_Msg.latitude = GPS_GGA_Msg.latitude + buf[posx + i] - 0x30;
            i++;
        }
        i++;
        while(buf[posx + i] != ',')//获取.号后的7890123
        {
            GPS_GGA_Msg.latitude = GPS_GGA_Msg.latitude + (buf[posx + i] - 0x30)*pow_f(0.1,j);
            i++;
            j++;
        }        
        GPS_GGA_Msg.latitude = GPS_PosInfoConvert(GPS_GGA_Msg.latitude);//进行单位转换
        
        posx=NMEA_Comma_Pos(p1,3);	
        GPS_GGA_Msg.nshemi    = buf[posx];                //南/北球标识 N/S
//******************提取纬度信息*****************************************************>

//******************提取经度信息*****************************************************< 
////例：在消息格式中的字符串“12345.7890123”转成double 数据类型 12345.7890123            
        posx=NMEA_Comma_Pos(p1,4);	
        i = 0;
        j = 1;
        GPS_GGA_Msg.longitude = 0;
        while(buf[posx + i] != '.')//获取.号前的12345
        {
            GPS_GGA_Msg.longitude = GPS_GGA_Msg.longitude*10;
            GPS_GGA_Msg.longitude = GPS_GGA_Msg.longitude + buf[posx + i] - 0x30;
            i++;
        }
        i++;
        while(buf[posx + i] != ',')//获取.号后的7890123
        {
            GPS_GGA_Msg.longitude = GPS_GGA_Msg.longitude + (buf[posx + i] - 0x30)*pow_f(0.1,j);
            i++;
            j++;
        }  
        
        GPS_GGA_Msg.longitude = GPS_PosInfoConvert(GPS_GGA_Msg.longitude);//进行单位转换
        
        posx=NMEA_Comma_Pos(p1,5);	
         GPS_GGA_Msg.ewhemi    = buf[posx];                //东/酉球标识   E/S
//******************提取经度信息*****************************************************>
        
        posx=NMEA_Comma_Pos(p1,7);								
        GPS_GGA_Msg.PossLocationNum  = NMEA_Str2num(p1+posx,&dx); //得到用于定位的卫星数   
        ret  = 1;
    }
    return ret;    
}






