/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "w25qxx.h"
#include "ff.h"

int main(void) {
	 FATFS fs;      /* File system object (volume work area) */
    FIL fil;       /* File object */
    FRESULT res;   /* API result code */
    UINT bw;       /* Bytes written */
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	
	SPI_Flash_Init();
    /* Register work area */
    f_mount(&fs, "0", 0);

    /* Create FAT volume with default cluster size */
//    res = f_mkfs("0", FM_ANY, 0, work, sizeof work);
//    if (res == FR_OK){
//		work[0] = 1;
//	}
	
    /* Create a file as new */
    res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK){
		work[0] = 1;
	}

    /* Write a message */
    f_write(&fil, "Hello, World!\r\n", 15, &bw);
    if (bw != 15){
		work[0] = 1;
	}

    /* Close the file */
    f_close(&fil);

    /* Unregister work area */
    f_mount(0, "", 0);
	while(1);
}
