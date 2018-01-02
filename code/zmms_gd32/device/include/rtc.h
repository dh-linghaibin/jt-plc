/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _RTC_H_
#define _RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct {
    uint16_t year;  /* 1..4095 */
    uint8_t  month; /* 1..12 */
    uint8_t  mday;  /* 1..31 */
    uint8_t  wday;  /* 0..6, Sunday = 0*/
    uint8_t  hour;  /* 0..23 */
    uint8_t  min;   /* 0..59 */
    uint8_t  sec;   /* 0..59 */
    uint8_t  dst;   /* 0 Winter, !=0 Summer */
}rtc_t;

typedef struct _rtc_obj {
	void (*init)(struct _rtc_obj *t_rtc);
	rtc_t (*read)(struct _rtc_obj *t_rtc);
	void (*set)(struct _rtc_obj *t_rtc,rtc_t time);
}rtc_obj;

void rtc_init(struct _rtc_obj *t_rtc);
rtc_t rtc_read(struct _rtc_obj *t_rtc);
void rtc_set(struct _rtc_obj *t_rtc,rtc_t time);

#ifdef __cplusplus
}
#endif

#endif
