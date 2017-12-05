#ifndef __LCD_H
#define __LCD_H

/* ����ͷ�ļ� *****************************************************************/
#include "stm32f0xx.h"
/* �ⲿ�������� ***************************************************************/
typedef struct {
	GPIO_TypeDef* 	port;
	uint16_t        pin;
} Stdlcd_pin_TypeDef;

typedef struct {   
    Stdlcd_pin_TypeDef A;
    Stdlcd_pin_TypeDef B;
    Stdlcd_pin_TypeDef C;
    Stdlcd_pin_TypeDef D;
    Stdlcd_pin_TypeDef E;
    Stdlcd_pin_TypeDef F;
    Stdlcd_pin_TypeDef G;
    Stdlcd_pin_TypeDef DP;
    
    Stdlcd_pin_TypeDef DG1;
    Stdlcd_pin_TypeDef DG2;
    
    const uint8_t show_code[11];//����ܱ���
    uint8_t show[2];//��ʾ����
    uint8_t choice_num;//��ѯ��־
    uint8_t point[2];//С����
}StdLcd_n; 

typedef struct lcd{  
    StdLcd_n Lcd_n;
    void (*init)(StdLcd_n * lcd);
    void (*refresh)(StdLcd_n * lcd);
    void (*set)(StdLcd_n* lcd,uint8_t bit,uint8_t num);
    void (*point)(StdLcd_n* lcd,uint8_t point_num,uint8_t val);
}StdLcd; 
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �������� *******************************************************************/
void LcdInit(StdLcd_n * lcd);
void LcdRefresh(StdLcd_n* lcd);
void LcdSetNum(StdLcd_n* lcd,uint8_t bit,uint8_t num);
void LcdTogglePoint(StdLcd_n* lcd,uint8_t point_num,uint8_t val);

#endif
/***************************************************************END OF FILE****/