#ifndef __KEY_H
#define __KEY_H

/* ����ͷ�ļ� *****************************************************************/
#include "stm32f0xx.h"
/* �ⲿ�������� ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdkey_pin_TypeDef;

typedef struct {   
    Stdkey_pin_TypeDef key1;
    Stdkey_pin_TypeDef key2;
    uint16_t key1_count;
    uint16_t key2_count;
}StdKey_n; 

typedef struct {  
    StdKey_n Key_n;
    void (*init)(StdKey_n* key);
    uint8_t (*read)(Stdkey_pin_TypeDef* key);
}StdKey; 

/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �������� *******************************************************************/
void KeyInit(StdKey_n* key);
uint8_t KeyRead(Stdkey_pin_TypeDef* key);

#endif
/***************************************************************END OF FILE****/
