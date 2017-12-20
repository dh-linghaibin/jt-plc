/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _WDOG_H_
#define _WDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct _wdog_obj {
	void (*init)(struct _wdog_obj* wdog);
	void (*reload)(struct _wdog_obj* wdog);
}wdog_obj;

#ifdef __cplusplus
}
#endif

#endif
