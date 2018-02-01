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

#include "stm32f10x.h"

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

typedef enum {
	F_NO_USE = 0,
	F_USE,
	F_PACK_OK,
}packet_flag_e;

#define PACKAGE_NUM 10
/* Ω” ’ */
typedef struct _pacck_obj {
	uint8_t flag;
	uint8_t dat[8];
}pack_obj;

typedef struct _can_package_obj {
	pack_obj package[PACKAGE_NUM];
}can_package_obj;

typedef struct _can_packr_obj {
	uint8_t flag;
	uint8_t pack_bum;
	uint8_t id;
	uint8_t device_id;
	uint8_t cmd;
	uint8_t len;
	uint8_t arr[16];
}can_packr_obj;

/* ∑¢ÀÕ */
typedef struct _can_message_obj {
	uint8_t send_id;
	uint8_t id;
	uint8_t device_id;
	uint8_t len;
	uint8_t cmd;
	uint8_t arr[16];
}can_message_obj;

typedef struct _can_obj{   
	uint16_t id;
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

void CAN_tx_data(CanTxMsg TxMessage);

#ifdef __cplusplus
}
#endif

#endif
