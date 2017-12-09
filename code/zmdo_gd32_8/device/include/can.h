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

void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter);
void nvic_config(void);
void can_gpio_config(void);

#ifdef __cplusplus
}
#endif

#endif

