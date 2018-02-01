/*void en28j60_set_mac(struct _en28j60_obj* en28) 
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _ENC28J60_H_
#define _ENC28J60_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct _en28j60_obj {
	uint8_t mac[6];
	void (*init)(struct _en28j60_obj* en28);
	uint16_t (*packet_receive)(struct _en28j60_obj* en28,uint8_t * packet,uint16_t maxlen);
	void (*packet_send)(struct _en28j60_obj* en28,uint8_t * packet,uint16_t len);
	void (*set_mac)(struct _en28j60_obj* en28);
}en28j60_obj;

void en28j60_init(struct _en28j60_obj* en28);
uint16_t en28j60_packet_receive(struct _en28j60_obj* en28,uint8_t * packet,uint16_t maxlen);
void en28j60_packet_send(struct _en28j60_obj* en28,uint8_t * packet,uint16_t len);
void en28j60_set_mac(struct _en28j60_obj* en28);

//static en28j60_obj enc28 = {
//	{0x05,0x01,0x35,0x01,0x00,0x01},//MACµÿ÷∑    
//	&en28j60_init,
//	&en28j60_packet_receive,
//	&en28j60_packet_send,
//	&en28j60_set_mac,
//};

#ifdef __cplusplus
}
#endif

#endif

