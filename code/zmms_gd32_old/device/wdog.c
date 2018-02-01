/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "wdog.h"

void wdog_init(struct _wdog_obj* wdog) {
     /* ���ϵͳ�Ƿ��ɶ������Ź�ι����λ */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) { 
        /* �����λ��־λ */
        RCC_ClearFlag();
    }
    /* ʹ��д����IWDG_PR and IWDG_RLR �Ĵ��� */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* IWDG��������ʱ��: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    /* �����ص���������ֵ ���250ms��IWDG��ʱ���԰�������ʽ���㣺  
    Counter Reload Value = 250ms/IWDG counter clock period
    = 250ms / (LSI/32)
    = 0.25s / (LsiFreq/32)
    = LsiFreq/(32 * 4)
    = LsiFreq/128
    */
    IWDG_SetReload(4095);
    /* ���µ���IWDG������ */
    IWDG_ReloadCounter();
    /* ʹ�� IWDG (LSI ��Ӳ��ʹ��) */
    IWDG_Enable();
}

void wdog_reload(struct _wdog_obj* wdog) {
     /* ���µ���IWDG������ */
    IWDG_ReloadCounter(); 
}
/***************************************************************END OF FILE****/
