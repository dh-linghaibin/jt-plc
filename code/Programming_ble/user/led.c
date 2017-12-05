/* 引用头文件 *****************************************************************/
#include "led.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
static void LedClock(GPIO_TypeDef* port){
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

void LedInit(StdLed_n * led) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    LedClock(led->Led.port);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = led->Led.pin;
    GPIO_Init(led->Led.port, &GPIO_InitStructure);
    
    GPIO_WriteBit(led->Led.port,led->Led.pin,Bit_SET);
}

void LedSet(StdLed_n * led,typeLedSet var) {
    GPIO_WriteBit(led->Led.port,led->Led.pin,(BitAction)var);
}

void LedTiger(StdLed_n * led) {
     GPIO_WriteBit(led->Led.port,led->Led.pin, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(led->Led.port,led->Led.pin))));
}
/***************************************************************END OF FILE****/
