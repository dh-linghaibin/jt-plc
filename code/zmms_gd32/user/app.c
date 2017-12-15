/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "ff.h"

void fs_test(void) {
	FATFS fs;           /* Filesystem object */
	FIL fil;            /* File object */
	FRESULT res;  /* API result code */
	UINT bw;            /* Bytes written */
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	BYTE mm[50];
	UINT i;

	res = f_mkfs("0:", FM_ANY, 0, work, sizeof work);
	if (res) {

	return ;
	} else {

	}

	res = f_mount(&fs, "0:", 0);
	if (res)
	{

	}
	else
	{

	}
//	/* Create a file as new */
//	res = f_open(&fil, "0:/lhb.txt", FA_CREATE_NEW|FA_WRITE|FA_READ);
//	if (res)
//	{

//	}
//	else
//	{

//	}
//	/* Write a message */
//	res = f_write(&fil, "Hello,World!", 12, &bw);
//	//uart_printf("res write:%d\r\n",res);
//	if (bw == 12)
//	{

//	}
//	else
//	{

//	}
//	res = f_read(&fil,mm,12,&i);
//	if (res == FR_OK)
//	{

//	}
//	else
//	{

//	}
//	/* Close the file */
//	f_close(&fil);
//	f_mount(0, "0:", 0);
}

int main(void) {

	while(1);
}
