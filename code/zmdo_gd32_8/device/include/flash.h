/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef __FLASH_H
#define __FLASH_H

/* 引用头文件 *****************************************************************/
#include "gd32f10x.h"

/* 宏定义 --------------------------------------------------------------------*/
typedef enum {
	C_FLAG = 0,
	C_ADDR,
	C_DEVICE_VAL,
}flash_code_e;
/* 外部数据类型 ***************************************************************/
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
int flash_read(uint32_t address,uint32_t *read_data) ;
int flash_write(uint32_t address,uint32_t data);

#endif
/***************************************************************END OF FILE****/
