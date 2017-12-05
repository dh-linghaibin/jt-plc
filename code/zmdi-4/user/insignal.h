#ifndef __INSIGNAL_H
#define __INSIGNAL_H

/* ����ͷ�ļ� *****************************************************************/
#include "stm32f0xx.h"

/* �ⲿ�������� ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdlinsignal_pin_TypeDef;

typedef struct {
    Stdlinsignal_pin_TypeDef DI1;
    Stdlinsignal_pin_TypeDef DI2;
    Stdlinsignal_pin_TypeDef DI3;
    Stdlinsignal_pin_TypeDef DI4;
    Stdlinsignal_pin_TypeDef AI1;
    __IO uint16_t di_count[4];
    __IO uint16_t Data[4];
}Stdinsignal_n; 

typedef struct {   
    Stdinsignal_n insignal_n;
    void (*Init)(Stdinsignal_n* insignal);
    uint8_t (*read)(Stdlinsignal_pin_TypeDef* insignal);
}Stdinsignal; 
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �������� *******************************************************************/
void InsignedInit(Stdinsignal_n* insignal);
uint8_t InsignedRead(Stdlinsignal_pin_TypeDef* insignal);

#endif
/***************************************************************END OF FILE****/
