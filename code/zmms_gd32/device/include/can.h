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
	F_USE = 1,
	F_PACK_OK = 2,
}packet_flag_e;

typedef enum {
    LS_ACK_1 = 0, /* 发送握手信号 */
    LS_ACK_2 = 1, /* 发送握手确认信号 */
    LS_ACK_OK = 2, /* 握手成功 */
} lb_state;

#define PACKAGE_NUM 40
/* 接收 */
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
	uint8_t arr[32];
}can_packr_obj;

/* 发送 */
typedef struct _can_message_obj {
	uint8_t send_id;
	uint8_t id;
	uint8_t device_id;
	uint8_t len;
	uint8_t cmd;
	uint8_t arr[32];
}can_message_obj;

typedef struct _can_device_obj {
	lb_state state; /* 设备状态 */
	uint8_t address; /* 设备地址 */
	uint8_t over_time;
} can_device_obj;

void bxcan_init(void);
void bxcan_send(can_message_obj send_msg);
void bxcan_set_id(uint8_t id);
void bxcan_lb_poll(void);
void bxcan_set_rx_callback(void (*rx_callback)(can_packr_obj *pacckr));

#ifdef __cplusplus
}
#endif

#endif
