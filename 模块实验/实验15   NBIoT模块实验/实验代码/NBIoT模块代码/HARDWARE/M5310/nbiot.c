/**
 * Copyright (c) 2017 China Mobile IOT.
 * All rights reserved.
**/
#include "stm32f1xx.h"
#include "stdio.h"
#include "M5310.h"
#include "coap.h"
#include "nbiot.h"
#include "utils.h"
#include "usart.h"
#include "delay.h"
#include "MsgHandler.h"
static void handle_read( nbiot_device_t    *dev,
                         nbiot_uri_t       *uri,
                         uint8_t           *buffer,
                         size_t             buffer_len,
                         uint16_t           ackid)
{
    uint8_t ret = 0;
    nbiot_send_buffer(uri,buffer,ret,ackid,false);//false 是回复平台下发的写操作。
}

static void handle_write( nbiot_device_t        *dev,
                          nbiot_uri_t            *uri,
                          uint16_t               ackid,
                          uint8_t                *buffer,
                          size_t                 buffer_len)
{
    m5310_write_rsp(1,ackid);
}

static void handle_execute( nbiot_device_t          *dev,
                            nbiot_uri_t             *uri,
                            uint16_t                ackid,
                            uint8_t                *buffer,
                            size_t                 buffer_len)
{
	   m5310_execute_rsp(1,ackid);
}


uint8_t *pMsgHeader;
void handle_request( nbiot_device_t    *dev,
                            uint16_t           code,
                            uint8_t           *buffer,
                            size_t             buffer_len,
                            size_t             max_buffer_len )
{
    nbiot_uri_t uri;
    uint16_t  ackid;
    uint8_t  RspMsgID[10],MsgID_Index=0;
    char tmp[10],i=0;
    /* initialize response */
    char *msg=NULL;
	msg=strstr((char*)buffer,":0,");
	if(msg==NULL)
    {
        return ;
    }
	msg=msg+3;
    pMsgHeader = (uint8_t*)msg;
	while(*msg!=',')
    {
        RspMsgID[MsgID_Index++] = *(msg);
        tmp[i++]=*(msg++);
        
    }
	tmp[i]='\0';
	i=0;
	msg=msg+1;
	ackid=nbiot_atoi(tmp,strlen(tmp));
    
    while(*msg!=',')
    {
	   tmp[i++]=*(msg++);
    }
	tmp[i]='\0';
	i=0;
	msg=msg+1;
	uri.objid=nbiot_atoi(tmp,strlen(tmp));
    
	while(*msg!=',')
    {
        tmp[i++]=*(msg++);
    }
	tmp[i]='\0';
	i=0;
	msg=msg+1;
	uri.instid=nbiot_atoi(tmp,strlen(tmp));
    
	while((*msg!=',')&&(*msg!=0x0D))
    {
        tmp[i++]=*(msg++);
    }
	tmp[i]='\0';
	i=0;
	msg=msg+1;
	uri.resid=nbiot_atoi(tmp,strlen(tmp));
    
	if(uri.objid!=-1)
    {
        uri.flag|= NBIOT_SET_OBJID;
    }
    if(uri.instid!=-1)
    {
        uri.flag|= NBIOT_SET_INSTID;
    }
	if(uri.resid!=-1)
    {
        uri.flag|= NBIOT_SET_RESID;
    }
    
   if( COAP_READ == code )
    {
        delay_ms(50);
        memcpy((void*)&ReadRspTemp[12],(const void*)&RspMsgID[0],5);
        AT_SendCmd((char*)ReadRspTemp,"OK",0,500);
        memcpy((void*)&ReadRspHumi[12],(const void*)&RspMsgID[0],5);
        AT_SendCmd((char*)ReadRspHumi, "OK",0,500);
        memcpy((void*)&ReadRspIllum[12],(const void*)&RspMsgID[0],5);
        AT_SendCmd((char*)ReadRspIllum, "OK",0,500);
        memcpy((void*)&ReadRspLight[12],(const void*)&RspMsgID[0],5);
        AT_SendCmd((char*)ReadRspLight, "OK",0,500);
//        handle_read(dev,&uri,(uint8_t*) pMsgHeader,strlen(msg),ackid);
      
    }
	if ( COAP_WRITE == code )
    {
         light_control = (*msg)  - 0x30;
         handle_write(  dev,
                        &uri,
                        ackid,
                       (uint8_t*) msg,
                        strlen(msg) );
     }

	 if ( COAP_EXECUTE == code )
     {
          handle_execute( dev,
                          &uri,
                          ackid,
                          (uint8_t*)msg,
                          strlen(msg));
     }
	

}



uint8_t buf[1024];
uint8_t *pp;
int nbiot_send_buffer(const nbiot_uri_t * uri,
	                    uint8_t            *buffer,
                        size_t              buffer_len,
					    uint16_t            ackid,
                        bool                updated )
{
  char tmp[8];
	
	size_t  len=0;
	uint8_t type=0;
	uint8_t trigger=0;
	nbiot_uri_t uri1;
	uri1.objid=uri->objid;
	pp=strstr((void*)USART2_RecvBuffer,":0,") + 3;
	while(1)
	{
	 if(pp!=NULL){  
     while(*pp!=',')
     {
			tmp[len++]=*pp++;
     }
			tmp[len]='\0';
			pp = pp + 1;
			len=0;
			uri1.instid=nbiot_atoi(tmp,strlen(tmp));
     pp=pp;
		while(*pp!=',')
        {
			tmp[len++]=*pp++;
        }
			tmp[len]='\0';
			pp=pp+1;
		  len=0;
			uri1.resid=nbiot_atoi(tmp,strlen(tmp));
			 
    while(*pp!=',')
			tmp[len++]=*pp++;
			tmp[len]='\0';
			pp=pp+1;
		  len=0;
			type=nbiot_atoi(tmp,strlen(tmp));
	 while(*pp!=';')
       buf[len++]=*pp++;
	     buf[len]='\0';
		    pp=pp+1;
	      len=0;
	  	if(*pp=='\0')
		     trigger=1;
        if(updated==true){
           m5310_notify_upload(&uri1,type,(char*)buf,trigger,ackid);
		   }	
        else
		       m5310_read_upload(&uri1,type,(char*)buf,trigger,ackid);
				if(1==trigger)
					break;
		
	   }else{
         break;
	   }
      
	}
    return NBIOT_ERR_OK;
}

int nbiot_recv_buffer( uint8_t           *buffer,
                       size_t             buffer_len )
{
    return 0;
}

