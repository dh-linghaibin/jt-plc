/*
* File      : application.c
* This file is part of RT-Thread RTOS
* COPYRIGHT (C) 2006, RT-Thread Development Team
*
* The license and distribution terms for this file may be
* found in the file LICENSE in this distribution or at
* http://www.rt-thread.org/license/LICENSE
*
* Change Logs:
* Date           Author       Notes
* 2009-01-05     Bernard      the first version
* 2013-07-12     aozima       update for auto initial.
*/

/**
* @addtogroup STM32
*/
/*@{*/
#include <rtthread.h>


int rt_application_init(void) {    
    extern int flash_task_init(void);
    flash_task_init();
    
    extern int rt_can_app_init(void);
    rt_can_app_init();
    
    extern int mb_tatsk_init(void);
    mb_tatsk_init();
    
    extern int rtc_task_init(void);
    rtc_task_init();
    
    return 0;
}

/*@}*/
