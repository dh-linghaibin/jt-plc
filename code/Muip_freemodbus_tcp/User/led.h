#ifndef __LED_H
#define __LED_H

/* 引用头文件 *****************************************************************/
#include "stm32f10x.h"

typedef enum typeLedSet{
    ON = 1,
    OFF = 0
}typeLedSet;
/* 外部数据类型 ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} StdLed_pin_TypeDef;

typedef struct {
    StdLed_pin_TypeDef RUN;
    StdLed_pin_TypeDef CAN;
    StdLed_pin_TypeDef RS484;
    StdLed_pin_TypeDef RS232;
}StdLed_n; 

typedef struct led{   
    StdLed_n Led_n;
    void (*Init)(StdLed_n* Led_n);
    void (*setrun)(StdLed_n* Led_n,uint8_t var);
    void (*setcan)(StdLed_n* Led_n,uint8_t var);
    void (*setrs485)(StdLed_n* Led_n,uint8_t var);
    void (*setrs232)(StdLed_n* Led_n,uint8_t var);
    
    void (*tagrun)(StdLed_n* Led_n);
    void (*tagcan)(StdLed_n* Led_n);
    void (*tagrs485)(StdLed_n* Led_n);
    void (*tagrs232)(StdLed_n* Led_n);
}StdLed; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void LedInit(StdLed_n* Led_n);
void LedSetRun(StdLed_n* Led_n,uint8_t var);
void LedSetCAN(StdLed_n* Led_n,uint8_t var);
void LedSetRS232(StdLed_n* Led_n,uint8_t var);
void LedSetRS484(StdLed_n* Led_n,uint8_t var);
void LedRunTiger(StdLed_n* Led_n);
void LedCANTiger(StdLed_n* Led_n);
void LedRS232Tiger(StdLed_n* Led_n);
void LedRS484Tiger(StdLed_n* Led_n);


#endif
/***************************************************************END OF FILE****/