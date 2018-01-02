#ifndef __LCD_H
#define __LCD_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"
/* 外部数据类型 ***************************************************************/
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
    
    const uint8_t show_code[12];//数码管编码
    uint8_t show[2];//显示数字
    uint8_t choice_num;//轮询标志
    uint8_t point[2];//小数点
}StdLcd_n; 

typedef struct lcd{  
    StdLcd_n Lcd_n;
    void (*init)(StdLcd_n * lcd);
    void (*refresh)(StdLcd_n * lcd);
    void (*set)(StdLcd_n* lcd,uint8_t bit,uint8_t num);
    void (*point)(StdLcd_n* lcd,uint8_t point_num,uint8_t val);
}StdLcd; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void LcdInit(StdLcd_n * lcd);
void LcdRefresh(StdLcd_n* lcd);
void LcdSetNum(StdLcd_n* lcd,uint8_t bit,uint8_t num);
void LcdTogglePoint(StdLcd_n* lcd,uint8_t point_num,uint8_t val);

#endif
/***************************************************************END OF FILE****/