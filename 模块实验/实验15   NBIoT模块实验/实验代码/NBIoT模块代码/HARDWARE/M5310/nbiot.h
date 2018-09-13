

#ifndef ONENET_NBIOT_H_
#define ONENET_NBIOT_H_
#include "stm32f1xx.h"
#include "stdbool.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * value type
**/
#define NBIOT_UNKNOWN       0x0
#define NBIOT_BOOLEAN       0x5
#define NBIOT_INTEGER       0x3
#define NBIOT_FLOAT         0x4
#define NBIOT_STRING        0x1
#define NBIOT_BINARY        0x2
/**
 * value flag
**/
#define NBIOT_READABLE      0x1
#define NBIOT_WRITABLE      0x2
#define NBIOT_EXECUTABLE    0x4
#define NBIOT_UPDATED       0x8

#define NBIOT_ERR_OK             0
    
/**
* resource uri flag
**/
#define NBIOT_SET_OBJID     0x1
#define NBIOT_SET_INSTID    0x2
#define NBIOT_SET_RESID     0x4
/**
 * value定义
**/

typedef struct _nbiot_uri_t
{
    int16_t objid;
    int16_t instid;
    int16_t resid;
    uint8_t  flag;
} nbiot_uri_t;

/**
 * device声明
**/
typedef struct _nbiot_device_t nbiot_device_t;


/**
 * 销毁OneNET接入设备实例
 * @param dev 设备实例
**/
void nbiot_device_destroy( nbiot_device_t *dev );

/**
 * 连接OneNET服务
 * @param dev        设备实例
 *        server_uri 服务链接地址（例如coap://127.0.0.1:5683）
 *        timeout    超时时长（秒）
 * @return 成功返回NBIOT_ERR_OK
**/
int nbiot_device_connect( nbiot_device_t *dev,
                          int             timeout );

/**
* 关闭与OneNET服务的连接
* @param dev 设备实例
**/
void nbiot_device_close( nbiot_device_t *dev,
                         int             timeout );

/**
 * 驱动设备的数据收发和逻辑处理
 * @param dev     设备实例
 *        timeout 超时时长（秒）
 * @return 成功返回NBIOT_ERR_OK
**/
int nbiot_device_step( nbiot_device_t *dev,
                       int             timeout );

/**
 * 设备资源删除（只修改状态，未通知server）
 * @param dev  设备实例
 *        uri  资源地址信息
 * @return 成功返回NBIOT_ERR_OK
**/
int nbiot_resource_del( nbiot_device_t *dev,
                        uint16_t        objid,
                        uint16_t        instid,
                        uint16_t        resid );

/**
 * 主动同步设备信息（资源改变等）
 * @param dev     设备实例
**/
void nbiot_device_sync( nbiot_device_t *dev );

int nbiot_send_buffer(const nbiot_uri_t * uri,
	                     uint8_t *buffer,
                       size_t        buffer_len,
											 uint16_t       ackid,
                       bool          updated );
												
int nbiot_recv_buffer( uint8_t           *buffer,
                       size_t             buffer_len );

extern  void handle_request( nbiot_device_t    *dev,
                            uint16_t           code,
                            uint8_t           *buffer,
                            size_t             buffer_len,
                            size_t             max_buffer_len );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* ONENET_NBIOT_H_ */
