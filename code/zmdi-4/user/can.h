#ifndef __CAN_H
#define __CAN_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

/* 外部数据类型 ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdlcan_pin_TypeDef;

typedef enum {
	B_250K = 5,
    B_125K = 4,
    B_50K = 3,
    B_20K = 2,
    B_10K = 1,
    B_5K = 0,
} btl_e;

typedef struct {
    Stdlcan_pin_TypeDef rx;
    Stdlcan_pin_TypeDef tx;
    Stdlcan_pin_TypeDef s;
    uint16_t id;//can地址
    uint32_t ext_id;//扩展ID
    btl_e btl;//波特率
    uint8_t package[8];//缓存包
    CanTxMsg TxMessage;
    uint8_t lock;//解锁
}Stdcan_n; 

typedef struct {   
    Stdcan_n can_n;
    void (*Init)(Stdcan_n* can);
    void (*Send)(Stdcan_n* can);
    void (*setid)(Stdcan_n* can,uint8_t id);
    void (*set_btl)(Stdcan_n* can,btl_e btl);
    uint8_t (*readpack)(Stdcan_n* can);
}Stdcanbus; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void CanInit(Stdcan_n* can);
void CanSend(Stdcan_n* can);
void CanSetID(Stdcan_n* can,uint8_t id);
void CanSetBlt(Stdcan_n* can,btl_e btl);
uint8_t CanReadPackage(Stdcan_n* can);

#endif
/***************************************************************END OF FILE****/
