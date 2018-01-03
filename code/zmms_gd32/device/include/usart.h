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
#include "stdio.h"	

typedef struct _usart_obj {
	void (*init)(struct _usart_obj* usart,uint32_t baud_rate);
	void (*sen_byte)(struct _usart_obj* usart,uint8_t dat);
}usart_obj;

void usart_init(struct _usart_obj* usart,uint32_t baud_rate);
void usart_sen_byte(struct _usart_obj* usart,uint8_t dat);

typedef enum {
	R_HEAD = 0x77,
	R_TYPE = 0X00,
	R_CMD  = 0X03,
}rs485_cmd_e;

#define RS485_PACKAGE_NUM 5

typedef struct _rs485_packet_obj {
	uint8_t flag;
//	uint8_t head;
//	uint8_t type;
	uint8_t num;
	uint8_t cmd;
	uint8_t back_number;
	uint8_t number;
}rs485_packet_obj;

typedef struct _rs485_obj {
	void (*init)(struct _rs485_obj *re485);
}rs485_obj;

void rs485_init(struct _rs485_obj *re485);

#ifdef __cplusplus
}
#endif

#endif
