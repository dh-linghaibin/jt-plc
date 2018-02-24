/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2010 linghaibin
 *
 */

#ifndef _CO_CONFIGURE_H_
#define _CO_CONFIGURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct _co_configure_time {
	uint16_t open_light_time1;
	uint16_t close_light_time1;
} co_configure_time;

typedef struct _co_configure_group {
	co_configure_time time[3]; /* 3��ʱ�� */
	uint8_t priority : 4; /* ���ȼ� */
	uint16_t start_time; /* ��ʼ��ʱֵ */
	uint16_t centre_time; /* �м���ʱֵ */
	uint8_t light_sen_open;
	uint8_t light_sen_close;
	uint16_t coil_adr[32];/*32 ���·*/
} co_configure_group;

typedef struct _co_configure_mode {
	uint8_t mode;
	co_configure_group manual[3];
	co_configure_group time_control[3];
	co_configure_group light_control[3];
}co_configure_mode;

#ifdef __cplusplus
}
#endif

#endif
