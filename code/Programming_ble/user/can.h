#ifndef __CAN_H
#define __CAN_H
/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

/* 外部数据类型 ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdlcan_pin_TypeDef;

typedef struct {
    Stdlcan_pin_TypeDef rx;
    Stdlcan_pin_TypeDef tx;
    Stdlcan_pin_TypeDef s;
    uint16_t id;//can地址
    uint32_t ext_id;//扩展ID
    CanTxMsg TxMessage;
}Stdcan_n; 

typedef struct canbus{   
    Stdcan_n can_n;
    void (*Init)(Stdcan_n* can);
    void (*Send)(Stdcan_n* can);
    void (*setid)(Stdcan_n* can,uint8_t id);
}Stdcanbus; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void CanInit(Stdcan_n* can);
void CanSend(Stdcan_n* can);
void CanSetID(Stdcan_n* can,uint8_t id);

#endif
/***************************************************************END OF FILE****/
