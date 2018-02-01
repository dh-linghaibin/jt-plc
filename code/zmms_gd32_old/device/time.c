/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "time.h"

static uint8_t flag_1ms = 0;

void time_init(struct _time_obj* time) {
	/* Systick时钟每s触发一次CLOCK_SECOND次 */
    if (SysTick_Config(SystemCoreClock / 1000)) { 
        while (1);
    }
}

uint8_t time_get_1ms(struct _time_obj* time) {
	return flag_1ms;
}

void time_set_1ms(struct _time_obj* time,uint8_t var) {
	flag_1ms = var;
}

void SysTick_Handler2(void) {
	flag_1ms++;
}
