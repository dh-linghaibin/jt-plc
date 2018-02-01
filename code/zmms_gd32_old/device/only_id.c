/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "only_id.h"


void only_id_get_id(struct _only_id_obj* id) {
	u32 temp0,temp1,temp2;
	temp0 = *(__IO u32*)(0x1FFFF7E8);  
	temp1 = *(__IO u32*)(0x1FFFF7EC);
	temp2 = *(__IO u32*)(0x1FFFF7F0);
	//ID0x1FFFF7E8   0x1FFFF7EC  0x1FFFF7F0 
	id->id[0] = (u8)(temp0 & 0x000000FF);
	id->id[1] = (u8)((temp0 & 0x0000FF00)>>8);
	id->id[2] = (u8)((temp0 & 0x00FF0000)>>16);
	id->id[3] = (u8)((temp0 & 0xFF000000)>>24);
	id->id[4] = (u8)(temp1 & 0x000000FF);
	id->id[5] = (u8)((temp1 & 0x0000FF00)>>8);
	id->id[6] = (u8)((temp1 & 0x00FF0000)>>16);
	id->id[7] = (u8)((temp1 & 0xFF000000)>>24);
	id->id[8] = (u8)(temp2 & 0x000000FF);
	id->id[9] = (u8)((temp2 & 0x0000FF00)>>8);
	id->id[10] = (u8)((temp2 & 0x00FF0000)>>16);
	id->id[11] = (u8)((temp2 & 0xFF000000)>>24);      
}
