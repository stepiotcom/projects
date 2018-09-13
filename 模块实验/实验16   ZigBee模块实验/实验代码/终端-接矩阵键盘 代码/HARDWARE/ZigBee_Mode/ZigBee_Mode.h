#ifndef __ZIGBEE_MODE_H__
#define __ZIGBEE_MODE_H__
#include "ZigBee_Mode.h"
#include "stm32f1xx.h"
enum ZBM_ErrorCode{   
    ZBM_SUCCESS,
    ZBM_FAILED,
    ZBM_NETWORK_STATE_JOINED,
    ZBM_NETWORK_STATE_NO_NETWORK,
    ZBM_NETWORK_STATE_BUILD_NETWORK
};

enum ZBM_DevType{   
    COOR,
    ROUTER,
    ENDPOINT
};

typedef struct
{
    uint8_t  dev_type;         //设备类型
enum ZBM_ErrorCode  nwk_state;        //网络状态
    uint16_t  pan_id ;          //网络 PAN_ID
    uint8_t  KEY[16];          //网络密匙
    uint16_t ShortAddr;        //网络短地址
    uint8_t  MAC_ADDR[8];      //MAC 地址
    uint16_t Coor_shortAddr;   //父节点(coor)网络短地址
    uint8_t  COOR_MAC_ADDR[8] ; //父节点 (coor)MAC 地址
    uint8_t  group;             //网络组号
    uint8_t  channel;           //通信信道
    uint8_t  txpower;           //发送功率
    uint8_t  baud;              //串口波特率
    uint8_t  sleep_time;        //休眠状态    
    uint8_t  get_writersp;      //写模块参数收到反馈
    uint8_t  get_readrsp;       //读模块参数收到反馈
    uint8_t  get_data;
}ZBM_Info_TypeDef;
extern ZBM_Info_TypeDef ZBM_Info;

typedef struct
{
enum ZBM_DevType      dev_type;         //设备类型
    uint16_t  pan_id ;          //网络 PAN_ID
    uint8_t  *pKey;          //网络密匙
    uint8_t  group;             //网络组号
    uint8_t  channel;           //通信信道
    uint8_t  txpower;           //发送功率
    uint8_t  baud;              //串口波特率
    uint8_t  sleepmode;        //休眠状态    
}ZBM_Setting_TypeDef;
extern const ZBM_Setting_TypeDef ZBM_Setting;

#define ZBM_RECVDATA_LEN  20
extern uint8_t ZBM_RecvDataBuffer[];
extern uint8_t ZBM_RecvDataIdx;
extern uint8_t ZBM_RecvNwkStatePrompt[ ];
extern uint8_t ZBM_RecvNwkStatePromptIdx;

              void ZigBee_Mode_Init(void);
              void ZBM_SendData(uint8_t *pdata,uint16_t len);
enum ZBM_ErrorCode ZBM_SetNetworkPAN(uint16_t panid);
enum ZBM_ErrorCode ZBM_SetDevType(enum ZBM_DevType type);
enum ZBM_ErrorCode ZBM_SetNetworkKey(uint8_t *pkey,uint8_t len);
enum ZBM_ErrorCode ZBM_SetNetworkGrounp(uint8_t grounp);
enum ZBM_ErrorCode ZBM_SetChannel(uint8_t ch);
enum ZBM_ErrorCode ZBM_SetTxPower(uint8_t pow);
enum ZBM_ErrorCode ZBM_SetSerialBaud(uint8_t baud_index);
enum ZBM_ErrorCode ZBM_SetSleepMode(uint8_t mode);
enum ZBM_ErrorCode ZBM_SetDataSaveTime(uint8_t time);
enum ZBM_ErrorCode ZBM_GetDevType(uint8_t *ptype);
enum ZBM_ErrorCode ZBM_GetNwkState(uint8_t *pstate);
enum ZBM_ErrorCode ZBM_GetPAN(uint16_t *ppan);
enum ZBM_ErrorCode ZBM_GetNwkKey(uint8_t *pkey);
enum ZBM_ErrorCode ZBM_GetNwkShortAddr(uint16_t *pshortaddr);
enum ZBM_ErrorCode ZBM_GetLocalMAC(uint8_t *pmac);
enum ZBM_ErrorCode ZBM_GetParentsShortAddr(uint16_t *pshortaddr);
enum ZBM_ErrorCode ZBM_GetParentsMACAddr(uint8_t *pmacaddr);
enum ZBM_ErrorCode ZBM_GetNwkGrounNo(uint8_t *pgroundno);
enum ZBM_ErrorCode ZBM_GetChannel(uint8_t *pchannel);
enum ZBM_ErrorCode ZBM_GetTxPower(uint8_t *ptxpower);
enum ZBM_ErrorCode ZBM_GetSerialBaud(uint8_t *pbaud);
enum ZBM_ErrorCode ZBM_GetDormantStatus(uint8_t *psta);
enum ZBM_ErrorCode ZBM_GetShortaddr_throung_MAC(const uint8_t *pmacaddr,uint16_t *pshortaddr);
enum ZBM_ErrorCode ZBM_GetDataSaveTimeC(uint8_t *ptime);
              void ZBM_Send_P2P(uint16_t shortaddr,uint8_t *pdata,uint8_t len);
              void ZBM_Send_Broadcast(uint8_t broadcastmode,uint8_t *pdata,uint8_t len);
              void ZBM_Send_Grounp(uint8_t grounpno,uint8_t *pdata,uint8_t len);              
enum ZBM_ErrorCode ZBM_IsJoinedNetwork(void);
              void ZBM_ResetIO_Init(void);
              void ZBM_Reset(void);
              void ZBM_RecvHandler(uint8_t *precvbuf,uint8_t buflen);         
#endif


