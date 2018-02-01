/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "wdog.h"

void wdog_init(struct _wdog_obj* wdog) {
     /* 检测系统是否由独立看门狗喂狗复位 */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) { 
        /* 清除复位标志位 */
        RCC_ClearFlag();
    }
    /* 使能写访问IWDG_PR and IWDG_RLR 寄存器 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* IWDG计数器的时钟: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    /* 设置重导计数器的值 获得250ms的IWDG超时可以按下面算式计算：  
    Counter Reload Value = 250ms/IWDG counter clock period
    = 250ms / (LSI/32)
    = 0.25s / (LsiFreq/32)
    = LsiFreq/(32 * 4)
    = LsiFreq/128
    */
    IWDG_SetReload(4095);
    /* 从新导入IWDG计数器 */
    IWDG_ReloadCounter();
    /* 使能 IWDG (LSI 被硬件使能) */
    IWDG_Enable();
}

void wdog_reload(struct _wdog_obj* wdog) {
     /* 从新导入IWDG计数器 */
    IWDG_ReloadCounter(); 
}
/***************************************************************END OF FILE****/
