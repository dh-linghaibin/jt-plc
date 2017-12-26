/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _MODBUS_H_
#define _MODBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"

typedef struct _modbus_coil_obj {
	uint16_t id;
	uint8_t val;
}modbus_coil_obj;

typedef struct _modbus_obj {
	uint8_t ipv4_ip[4];
	uint8_t gateway_ip[4];
	uint8_t mask_ip[4];
	struct uip_eth_addr uip_mac;
	en28j60_obj enc28;
	void (*init)(struct _modbus_obj* modbus);
	void (*loop)(struct _modbus_obj* modbus);
	void (*heart)(struct _modbus_obj* modbus);
	void (*set_coil)(struct _modbus_obj* modbus,uint16_t num,uint8_t val);
	void (*set_input)(struct _modbus_obj* modbus,uint16_t num,uint8_t val);
	modbus_coil_obj (*up_coil)(struct _modbus_obj* modbus);
}modbus_obj;

void modbus_init(struct _modbus_obj* modbus);
void modbus_loop(struct _modbus_obj* modbus);
void modbus_heart(struct _modbus_obj* modbus);
void modbus_set_coil(struct _modbus_obj* modbus,uint16_t num,uint8_t val);
void modbus_set_input(struct _modbus_obj* modbus,uint16_t num,uint8_t val);
modbus_coil_obj modbus_up_coil(struct _modbus_obj* modbus);

#ifdef __cplusplus
}
#endif

#endif
