/* 引用头文件 *****************************************************************/
#include "led.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
static void OutLedClock(GPIO_TypeDef* port){
    if (port == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if(port == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if(port == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    else if(port == GPIOD)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    else if(port == GPIOE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    else if(port == GPIOF)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
}

void LedInit(StdLed_n* Led_n) {
   GPIO_InitTypeDef GPIO_InitStructure;
    
    OutLedClock(Led_n->RUN.port);
    OutLedClock(Led_n->CAN.port);
    OutLedClock(Led_n->RS232.port);
    OutLedClock(Led_n->RS484.port);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = Led_n->RUN.pin;
    GPIO_Init(Led_n->RUN.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Led_n->CAN.pin;
    GPIO_Init(Led_n->CAN.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Led_n->RS232.pin;
    GPIO_Init(Led_n->RS232.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Led_n->RS484.pin;
    GPIO_Init(Led_n->RS484.port, &GPIO_InitStructure);
    
    GPIO_WriteBit(Led_n->RUN.port,Led_n->RUN.pin,(BitAction)(1));
    GPIO_WriteBit(Led_n->CAN.port,Led_n->CAN.pin,(BitAction)(1));
    GPIO_WriteBit(Led_n->RS232.port,Led_n->RS232.pin,(BitAction)(1));
    GPIO_WriteBit(Led_n->RS484.port,Led_n->RS484.pin,(BitAction)(1));
}

void LedSetRun(StdLed_n* Led_n,uint8_t var) {
    GPIO_WriteBit(Led_n->RUN.port,Led_n->RUN.pin,(BitAction)(var));
}

void LedSetCAN(StdLed_n* Led_n,uint8_t var) {
    GPIO_WriteBit(Led_n->CAN.port,Led_n->CAN.pin,(BitAction)(var));
}

void LedSetRS232(StdLed_n* Led_n,uint8_t var) {
    GPIO_WriteBit(Led_n->RS232.port,Led_n->RS232.pin,(BitAction)(var));
}

void LedSetRS484(StdLed_n* Led_n,uint8_t var) {
    GPIO_WriteBit(Led_n->RS484.port,Led_n->RS484.pin,(BitAction)(var));
}

void LedRunTiger(StdLed_n* Led_n) {
    GPIO_WriteBit(Led_n->RUN.port, Led_n->RUN.pin, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(Led_n->RUN.port, Led_n->RUN.pin))));
}

void LedCANTiger(StdLed_n* Led_n) {
    GPIO_WriteBit(Led_n->CAN.port, Led_n->CAN.pin, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(Led_n->CAN.port, Led_n->CAN.pin))));
}

void LedRS232Tiger(StdLed_n* Led_n) {
    GPIO_WriteBit(Led_n->RS232.port, Led_n->RS232.pin, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(Led_n->RS232.port, Led_n->RS232.pin))));
}

void LedRS484Tiger(StdLed_n* Led_n) {
    GPIO_WriteBit(Led_n->RS484.port, Led_n->RS484.pin, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(Led_n->RS484.port, Led_n->RS484.pin))));
}
/***************************************************************END OF FILE****/
