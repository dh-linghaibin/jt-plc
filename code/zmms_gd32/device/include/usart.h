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

typedef enum {
	RF_NO_USE = 0,
	RF_USE,
	RF_PACK_OK,
}rs485_packet_flag_e;

#define RS485_PACKAGE_NUM 5

typedef struct _rs485_cmd_obj {
	uint8_t flag;
	uint8_t back_number;
	uint8_t number;
}rs485_cmd_obj;

typedef struct _rs485_packet_obj {
	rs485_cmd_obj package[RS485_PACKAGE_NUM];
}rs485_packet_obj;

typedef struct _rs485_obj {
	void (*init)(struct _rs485_obj *re485);
	rs485_packet_obj* (*get_packet)(struct _rs485_obj *re485);
}rs485_obj;

void rs485_init(struct _rs485_obj *re485);
rs485_packet_obj* rs485_get_packet(struct _rs485_obj *re485);

#ifdef __cplusplus
}
#endif

#endif
