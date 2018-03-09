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

typedef enum _coc_mode {
	F_CONTRAL = 0X00,//消防
	M_CONTRAL = 0x01, //手动
	T_CONTRAL = 0x02,//时间控制
	L_CONTRAL = 0X03,//光 控
} coc_mode;

typedef struct _co_configure_time {
	uint16_t open_light_time1;
	uint16_t close_light_time1;
} co_configure_time;

typedef struct _co_configure_group {
	co_configure_time time[3]; /* 3组时间 */
	uint8_t priority : 4; /* 优先级 */
	uint16_t start_time; /* 开始延时值 */
	uint16_t centre_time; /* 中间延时值 */
	uint8_t light_sen_open;
	uint8_t light_sen_close;
	uint16_t coil_adr[32];/*32 组回路*/
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
