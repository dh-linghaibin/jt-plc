/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _USART_H_
#define _USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct _usart_obj {
	void (*init)(struct _usart_obj* usart,uint32_t baud_rate);
}usart_obj;

void usart_init(struct _usart_obj* usart,uint32_t baud_rate);

#ifdef __cplusplus
}
#endif

#endif
