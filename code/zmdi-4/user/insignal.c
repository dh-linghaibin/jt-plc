/* 引用头文件 *****************************************************************/
#include "insignal.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
#define ADC1_DR_Address    0x40012440
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
static void InsignalClock(GPIO_TypeDef* port){
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

void InsignedInit(Stdinsignal_n* insignal) {
    GPIO_InitTypeDef    GPIO_InitStructure;
    ADC_InitTypeDef     ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
    
    InsignalClock(insignal->DI1.port);
    InsignalClock(insignal->DI2.port);
    InsignalClock(insignal->DI3.port);
    InsignalClock(insignal->DI4.port);
    InsignalClock(insignal->AI1.port);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_InitStructure.GPIO_Pin = insignal->DI1.pin;
    GPIO_Init(insignal->DI1.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = insignal->DI2.pin;
    GPIO_Init(insignal->DI2.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = insignal->DI3.pin;
    GPIO_Init(insignal->DI3.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = insignal->DI4.pin;
    GPIO_Init(insignal->DI4.port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    
    GPIO_InitStructure.GPIO_Pin = insignal->AI1.pin;
    GPIO_Init(insignal->AI1.port, &GPIO_InitStructure);
    
    /* ADC1 DeInit */  
    ADC_DeInit(ADC1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
    ADC_Init(ADC1, &ADC_InitStructure); 
    ADC_ChannelConfig(ADC1, ADC_Channel_5 , ADC_SampleTime_55_5Cycles); 
    /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
    ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
    ADC_TempSensorCmd(ENABLE);

    /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
    ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles); 
    ADC_VrefintCmd(ENABLE);

    /* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */ 
    ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);  
    ADC_VbatCmd(ENABLE);

    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);

    /* ADC DMA request in circular mode */
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

    /* Enable ADC_DMA */
    ADC_DMACmd(ADC1, ENABLE);  

    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);     

    /* Wait the ADRDY flag */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

    /* ADC1 regular Software Start Conv */ 
    ADC_StartOfConversion(ADC1);
    
    /* DMA1 clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

    /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)insignal->Data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* DMA1 Channel1 enable */
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

uint8_t InsignedRead(Stdlinsignal_pin_TypeDef* insignal) {
    return GPIO_ReadInputDataBit(insignal->port,insignal->pin);
}
/***************************************************************END OF FILE****/
