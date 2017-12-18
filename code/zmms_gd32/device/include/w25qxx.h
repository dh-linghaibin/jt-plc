/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _W25QXX_H_
#define _W25QXX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

#define FLASH_SECTOR_SIZE 	512		 			    
#define FLASH_BLOCK_SIZE  	8     //每个BLOCK有8个扇区

typedef struct _w25qxx_obj {   
	uint16_t sector_count;
	u8 SPI_FLASH_BUF[4096];
	int (*init)(struct _w25qxx_obj* w25qxx);
	int (*write)(struct _w25qxx_obj* w25qxx,const uint8_t * buff, unsigned long sector, uint16_t count);
	int (*read)(struct _w25qxx_obj* w25qxx, uint8_t * buff, unsigned long sector, uint16_t count);
	uint16_t (*get_id)(struct _w25qxx_obj* w25qxx);
	int (*erase_chip)(struct _w25qxx_obj* w25qxx);
	int (*erase_sector)(struct _w25qxx_obj* w25qxx,unsigned long sector);
	void (*power_down)(struct _w25qxx_obj* w25qxx);
	void (*wake_up)(struct _w25qxx_obj* w25qxx);
} w25qxx_obj;

int w25qxx_init(struct _w25qxx_obj* w25qxx);
int w25qxx_write(struct _w25qxx_obj* w25qxx,const uint8_t * buff, unsigned long sector, uint16_t count);
int w25qxx_read(struct _w25qxx_obj* w25qxx, uint8_t * buff, unsigned long sector, uint16_t count);
uint16_t w25qxx_get_id(struct _w25qxx_obj* w25qxx);
int w25qxx_erase_chip(struct _w25qxx_obj* w25qxx);
int w25qxx_erase_sector(struct _w25qxx_obj* w25qxx,unsigned long sector);
void w25qxx_power_down(struct _w25qxx_obj* w25qxx);
void w25qxx_wake_up(struct _w25qxx_obj* w25qxx);

#ifdef __cplusplus
}
#endif

#endif
