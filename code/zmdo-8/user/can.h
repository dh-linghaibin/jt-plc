#ifndef __CAN_H
#define __CAN_H
/* ����ͷ�ļ� *****************************************************************/
#include "stm32f0xx.h"

/* �ⲿ�������� ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdlcan_pin_TypeDef;

typedef struct {
    Stdlcan_pin_TypeDef rx;
    Stdlcan_pin_TypeDef tx;
    Stdlcan_pin_TypeDef s;
    uint16_t id;//can��ַ
    uint32_t ext_id;//��չID
    uint8_t package[8];//�����
    CanTxMsg TxMessage;
}Stdcan_n; 

typedef struct canbus{   
    Stdcan_n can_n;
    void (*Init)(Stdcan_n* can);
    void (*Send)(Stdcan_n* can);
    void (*setid)(Stdcan_n* can,uint8_t id);
    uint8_t (*readpack)(Stdcan_n* can);
}Stdcanbus; 
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �������� *******************************************************************/
void CanInit(Stdcan_n* can);
void CanSend(Stdcan_n* can);
void CanSetID(Stdcan_n* can,uint8_t id);
uint8_t CanReadPackage(Stdcan_n* can);

#endif
/***************************************************************END OF FILE****/
