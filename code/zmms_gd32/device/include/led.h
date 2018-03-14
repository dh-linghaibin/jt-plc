/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef enum _led_type_e{
	L_RUN = 0,
	L_CAN,
	L_RS485,
	L_RS232,
}led_type_e;

void led_init(void);
void led_set(led_type_e led_type ,BitAction val);
void led_tager(led_type_e led_type);

/*
led_obj led = {
	.init = led_init,
	.set = led_set,
	.tager = led_tager,
};
*/

#ifdef __cplusplus
}
#endif

#endif
