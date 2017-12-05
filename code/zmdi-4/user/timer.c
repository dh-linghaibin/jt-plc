/* ����ͷ�ļ� *****************************************************************/
#include "timer.h"
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
/* ˽�ж����� *****************************************************************/
/* ˽�б��� *******************************************************************/
__IO uint32_t sys_heartbeat = 0;

__IO uint16_t CCR3_Val = 6827;//13254
uint16_t PrescalerValue = 0;
/* ȫ�ֱ��� */
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/

void TimerInit(void) {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    /*  TIM3 �ж�Ƕ�����*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* -----------------------------------------------------------------------
    TIM3 ����: ����Ƚ�ʱ��ģʽ:
    
    �������� TIM3����ʱ��(TIM3CLK) ����ΪAPB1 ʱ�� (PCLK1),  
    => TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
    
    �� TIM3 ����ʱ�� ��Ϊ6 MHz, Ԥ��Ƶ�����԰������湫ʽ���㣺
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (PCLK1 /6 MHz) - 1
    
    CC3 ��ת�� = TIM3 counter clock / CCR3_Val = 439.4 Hz
    ==> Toggling frequency = 219.7 Hz
    
    CC4 ��ת��= TIM3 counter clock / CCR4_Val = 878.9 Hz
    ==> Toggling frequency = 439.4 Hz
    ----------------------------------------------------------------------- */   
    
    /* ����Ԥ��Ƶֵ */
    PrescalerValue = (uint16_t) (SystemCoreClock  / 6000000) - 1;
    
    /* Time ��ʱ���������� */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* Ԥ��Ƶ������ */
    TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
    
    /* ����Ƚ�ʱ��ģʽ�������� */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    /* ����Ƚ�ʱ��ģʽ����: Ƶ��3*/
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
    
    //    /* ����Ƚ�ʱ��ģʽ����: Ƶ��4 */
    //    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    //    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    //    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
    //    
    //    /* TIM �ж�ʹ�� */
    //    TIM_ITConfig(TIM3, TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
    
    TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
    /* TIM3 ʹ�� */
    TIM_Cmd(TIM3, ENABLE);
}

uint32_t TimerGetClock(void) {
    return sys_heartbeat;
}
/***************************************************************END OF FILE****/
