#ifndef __LED_H
#define __LED_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

typedef enum typeLedSet{
    ON = 1,
    OFF = 0
}typeLedSet;

typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdled_pin_TypeDef;

typedef struct {   
    Stdled_pin_TypeDef Led;
}StdLed_n; 

/* 外部数据类型 ***************************************************************/
typedef struct {   
    StdLed_n Led_n;
    void (*init)(StdLed_n * led);
    void (*Set)(StdLed_n * led,typeLedSet var);
    void (*Tiger)(StdLed_n * led);
}StdLed; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void LedInit(StdLed_n * led);
void LedSet(StdLed_n * led,typeLedSet var);
void LedTiger(StdLed_n * led);

#endif
/***************************************************************END OF FILE****/