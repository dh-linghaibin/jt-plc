#ifndef __LED_H
#define __LED_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

typedef enum typeLedSet{
    ON = 1,
    OFF = 0
}typeLedSet;
/* 外部数据类型 ***************************************************************/
typedef struct led{   
    void (*init)(void);
    void (*Set)(typeLedSet var);
    void (*Tiger)(void);
}StdLed; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void LedInit(void);
void LedSet(typeLedSet var);
void LedTiger(void);

#endif
/***************************************************************END OF FILE****/