/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _ONLY_ID_H_
#define _ONLY_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct _only_id_obj {
	uint8_t id[12]; 
}only_id_obj;

void only_id_get_id(struct _only_id_obj* id);

#ifdef __cplusplus
}
#endif

#endif
