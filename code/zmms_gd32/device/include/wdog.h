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

void wdog_init(void);
void wdog_reload(void);

#ifdef __cplusplus
}
#endif

#endif
