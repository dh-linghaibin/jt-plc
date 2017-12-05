/* 引用头文件 *****************************************************************/
#include "timer.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
__IO uint32_t sys_heartbeat = 0;

__IO uint16_t CCR3_Val = 6827;//13254
uint16_t PrescalerValue = 0;
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/

void TimerInit(void) {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    /*  TIM3 中断嵌套设计*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* -----------------------------------------------------------------------
    TIM3 配置: 输出比较时序模式:
    
    本试验中 TIM3输入时钟(TIM3CLK) 被设为APB1 时钟 (PCLK1),  
    => TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
    
    当 TIM3 计数时钟 设为6 MHz, 预分频器可以按照下面公式计算：
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (PCLK1 /6 MHz) - 1
    
    CC3 翻转率 = TIM3 counter clock / CCR3_Val = 439.4 Hz
    ==> Toggling frequency = 219.7 Hz
    
    CC4 翻转率= TIM3 counter clock / CCR4_Val = 878.9 Hz
    ==> Toggling frequency = 439.4 Hz
    ----------------------------------------------------------------------- */   
    
    /* 计算预分频值 */
    PrescalerValue = (uint16_t) (SystemCoreClock  / 6000000) - 1;
    
    /* Time 定时器基础设置 */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* 预分频器配置 */
    TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
    
    /* 输出比较时序模式配置设置 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    /* 输出比较时序模式配置: 频道3*/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
    
    //    /* 输出比较时序模式配置: 频道4 */
    //    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    //    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    //    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
    //    
    //    /* TIM 中断使能 */
    //    TIM_ITConfig(TIM3, TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
    
    TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
    /* TIM3 使能 */
    TIM_Cmd(TIM3, ENABLE);
}

uint32_t TimerGetClock(void) {
    return sys_heartbeat;
}
/***************************************************************END OF FILE****/
