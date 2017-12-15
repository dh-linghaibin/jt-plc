/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f10x.h"

typedef enum {
	P_HEAD = 2,
	P_CMD = 5,
	P_LEN = 1,
	P_ADDR = 2,
	P_O0 = 6,
	P_O1 = 7,
	P_O2 = 8,
	P_O3 = 10,
	P_O4 = 11,
	P_O5 = 12,
	P_O6 = 13,
	P_O7 = 14,
}packet_cmd_e;

typedef struct _can_package_obj {
	#define PACKAGE_NUM 20
	uint8_t package[PACKAGE_NUM][69];
	uint8_t (*get_cmd)( struct _can_package_obj* pack,uint8_t num);
}can_package_obj;

typedef struct _can_message_obj {
	uint8_t send_id;
	uint8_t id;
	uint8_t device_id;
	uint8_t len;
	uint8_t cmd;
	uint8_t arr[64];
}can_message_obj;

typedef struct _can_obj{   
	uint16_t id;
    uint32_t ext_id;
    uint8_t send_packed[8];
	can_message_obj send_msg;
    void (*init)(struct _can_obj* can);
    void (*send)(struct _can_obj* can);
    void (*set_id)(struct _can_obj* can,uint8_t id);
	can_package_obj* (*get_packget)(struct _can_obj* can);
}can_obj; 


void bxcan_init(struct _can_obj* can);
void bxcan_send(struct _can_obj* can);
void bxcan_set_id(struct _can_obj* can,uint8_t id);
can_package_obj*  bxcan_get_packget(struct _can_obj* can);

#ifdef __cplusplus
}
#endif

#endif

