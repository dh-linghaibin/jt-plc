#ifndef __USART_H
#define __USART_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

/* 外部数据类型 ***************************************************************/
typedef struct {
    uint8_t Ble_en;
    uint8_t lock;//解锁
    uint8_t pro_lock;//下载解锁
    uint8_t package[16];//缓存包
}StdUart_n;

typedef struct {   
    StdUart_n Uart_n;
    void (*init)(void);
    void (*sendbyte)(uint8_t byte);
    uint8_t (*getpack)(StdUart_n* Uart);
    uint8_t (*readpack)(StdUart_n* Uart, uint8_t len);
    void (*packinit)(void);
}StdUart; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void UsartInit(void);
void UsartSendByte(uint8_t byte);
uint8_t UARTReadPackageP(StdUart_n* Uart);
uint8_t UARTReadPackage(StdUart_n* Uart,uint8_t len);
void UsartPackInit(void);

#endif
/***************************************************************END OF FILE****/