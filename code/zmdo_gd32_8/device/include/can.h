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
}can_obj; 


void bxcan_init(struct _can_obj* can);
void bxcan_send(struct _can_obj* can);
void bxcan_set_id(struct _can_obj* can,uint8_t id);

#ifdef __cplusplus
}
#endif

#endif

