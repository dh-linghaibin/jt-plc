/* 引用头文件 *****************************************************************/
#include "key.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/

static void KeyClock(GPIO_TypeDef* port){
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

void KeyInit(StdKey_n* key) {
    GPIO_InitTypeDef GPIO_InitStructure;
     
    KeyClock(key->key1.port);
    KeyClock(key->key2.port);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    
    GPIO_InitStructure.GPIO_Pin = key->key1.pin;
    GPIO_Init(key->key1.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = key->key2.pin;
    GPIO_Init(key->key2.port, &GPIO_InitStructure);
}

uint8_t KeyRead(Stdkey_pin_TypeDef* key) {
    return GPIO_ReadInputDataBit(key->port,key->pin);
}

/***************************************************************END OF FILE****/
