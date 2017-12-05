/* 引用头文件 *****************************************************************/
#include "lcd.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
static void LcdClock(GPIO_TypeDef* port){
    if (port == GPIOA)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    else if(port == GPIOB)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    else if(port == GPIOC)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    else if(port == GPIOD)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    else if(port == GPIOE)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    else if(port == GPIOF)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
}

//void LcdSetBitNum(StdLcd_n* lcd) {
//    
//    GPIO_WriteBit(lcd->A.port,lcd->A.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 0)&0x01 ));
//    GPIO_WriteBit(lcd->B.port,lcd->B.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 1)&0x01 ));
//    GPIO_WriteBit(lcd->C.port,lcd->C.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 2)&0x01 ));
//    GPIO_WriteBit(lcd->D.port,lcd->D.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 3)&0x01 ));
//    GPIO_WriteBit(lcd->E.port,lcd->E.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 4)&0x01 ));
//    GPIO_WriteBit(lcd->F.port,lcd->F.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 5)&0x01 ));
//    GPIO_WriteBit(lcd->G.port,lcd->G.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 6)&0x01 ));
//    GPIO_WriteBit(lcd->DP.port,lcd->DP.pin,       \
//                  (BitAction)( ( (lcd->show_code[lcd->show[lcd->choice_num]]) >> 7)&0x01 ));
//}
void LcdSetBitNum(StdLcd_n* lcd,uint8_t val) {
    
    GPIO_WriteBit(lcd->A.port,lcd->A.pin,       \
                  (BitAction)( ( (val) >> 0)&0x01 ));
    GPIO_WriteBit(lcd->B.port,lcd->B.pin,       \
                  (BitAction)( ( (val) >> 1)&0x01 ));
    GPIO_WriteBit(lcd->C.port,lcd->C.pin,       \
                  (BitAction)( ( (val) >> 2)&0x01 ));
    GPIO_WriteBit(lcd->D.port,lcd->D.pin,       \
                  (BitAction)( ( (val) >> 3)&0x01 ));
    GPIO_WriteBit(lcd->E.port,lcd->E.pin,       \
                  (BitAction)( ( (val) >> 4)&0x01 ));
    GPIO_WriteBit(lcd->F.port,lcd->F.pin,       \
                  (BitAction)( ( (val) >> 5)&0x01 ));
    GPIO_WriteBit(lcd->G.port,lcd->G.pin,       \
                  (BitAction)( ( (val) >> 6)&0x01 ));
    GPIO_WriteBit(lcd->DP.port,lcd->DP.pin,       \
                  (BitAction)( ( (val) >> 7)&0x01 ));
}

static void LcdMotionDig(StdLcd_n* lcd) {
    switch(lcd->choice_num) {
        case 0:
        GPIO_WriteBit(lcd->DG2.port,lcd->DG2.pin,Bit_SET);
        GPIO_WriteBit(lcd->DG1.port,lcd->DG1.pin,Bit_RESET);
        break;
        case 1:
        GPIO_WriteBit(lcd->DG1.port,lcd->DG1.pin,Bit_SET);
        GPIO_WriteBit(lcd->DG2.port,lcd->DG2.pin,Bit_RESET);
        break;
    }
}

void LcdInit(StdLcd_n * lcd) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    LcdClock(lcd->A.port);
    LcdClock(lcd->B.port);
    LcdClock(lcd->C.port);
    LcdClock(lcd->D.port);
    LcdClock(lcd->E.port);
    LcdClock(lcd->F.port);
    LcdClock(lcd->G.port);
    LcdClock(lcd->DP.port);
    LcdClock(lcd->DG1.port);
    LcdClock(lcd->DG2.port);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = lcd->A.pin;
    GPIO_Init(lcd->A.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->B.pin;
    GPIO_Init(lcd->B.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->C.pin;
    GPIO_Init(lcd->C.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->D.pin;
    GPIO_Init(lcd->D.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->E.pin;
    GPIO_Init(lcd->E.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->F.pin;
    GPIO_Init(lcd->F.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->G.pin;
    GPIO_Init(lcd->G.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->DP.pin;
    GPIO_Init(lcd->DP.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->DG1.pin;
    GPIO_Init(lcd->DG1.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = lcd->DG2.pin;
    GPIO_Init(lcd->DG2.port, &GPIO_InitStructure);
    
    GPIO_WriteBit(lcd->DG1.port,lcd->DG1.pin,Bit_SET);
    GPIO_WriteBit(lcd->DG2.port,lcd->DG2.pin,Bit_SET);
}

void LcdRefresh(StdLcd_n* lcd) {
    if(lcd->point[lcd->choice_num] == 0) {
        LcdSetBitNum(lcd,lcd->show_code[lcd->show[lcd->choice_num]] & 0x7f);
    } else {
        LcdSetBitNum(lcd,lcd->show_code[lcd->show[lcd->choice_num]] | 0x80);
    }
    
    LcdMotionDig(lcd);
    if(lcd->choice_num < 1) {
        lcd->choice_num++;
    } else {
        lcd->choice_num = 0;
    }
}

void LcdSetNum(StdLcd_n* lcd,uint8_t bit,uint8_t num) {
    if(bit > 1) {
        bit  = 1;
    }
    lcd->show[bit] = num;
}
void LcdTogglePoint(StdLcd_n* lcd,uint8_t point_num,uint8_t val) {
    if(point_num > 1) {
        point_num  = 1;
    }
    if(val == 0) {
        lcd->point[point_num] = 0;
    } else {
        lcd->point[point_num] = 1;
    }
}
/***************************************************************END OF FILE****/
