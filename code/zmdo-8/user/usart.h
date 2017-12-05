#ifndef __USART_H
#define __USART_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

/* 外部数据类型 ***************************************************************/
typedef struct usart{   
    void (*init)(void);
    void (*sendbyte)(uint8_t byte);
}StdUsaer; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void UsartInit(void);
void UsartSendByte(uint8_t byte);


#endif
/***************************************************************END OF FILE****/