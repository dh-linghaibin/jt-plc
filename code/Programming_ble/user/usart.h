#ifndef __USART_H
#define __USART_H

/* ����ͷ�ļ� *****************************************************************/
#include "stm32f0xx.h"

/* �ⲿ�������� ***************************************************************/
typedef struct {
    uint8_t Ble_en;
    uint8_t lock;//����
    uint8_t pro_lock;//���ؽ���
    uint8_t package[16];//�����
}StdUart_n;

typedef struct {   
    StdUart_n Uart_n;
    void (*init)(void);
    void (*sendbyte)(uint8_t byte);
    uint8_t (*getpack)(StdUart_n* Uart);
    uint8_t (*readpack)(StdUart_n* Uart, uint8_t len);
    void (*packinit)(void);
}StdUart; 
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �������� *******************************************************************/
void UsartInit(void);
void UsartSendByte(uint8_t byte);
uint8_t UARTReadPackageP(StdUart_n* Uart);
uint8_t UARTReadPackage(StdUart_n* Uart,uint8_t len);
void UsartPackInit(void);

#endif
/***************************************************************END OF FILE****/