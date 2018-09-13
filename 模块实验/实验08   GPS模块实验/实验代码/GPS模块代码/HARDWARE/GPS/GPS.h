#ifndef __GPS_H
#define __GPS_H 			   

#include "stm32f1xx_hal.h"

#define u8  	uint8_t
#define u16  	uint16_t
#define u32  	uint32_t
		
typedef struct 
{
    double longitude;          
    uint8_t nshemi;
    double latitude;
    uint8_t ewhemi;
    uint8_t MsgSta;
    uint8_t PossLocationNum;   
}GPS_MsgTypeDef;

uint8_t  GPS_GGA_Analysis(GPS_MsgTypeDef *gpsx,u8 *buf);
extern GPS_MsgTypeDef GPS_GGA_Msg;
#endif  





