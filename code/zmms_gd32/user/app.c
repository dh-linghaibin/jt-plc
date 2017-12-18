/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "w25qxx.h"
#include "ff.h"

void test(void) {
	FATFS fs; /* FatFs文件系统对象 */
	FIL fnew; /* 文件对象 */
	FRESULT res_sd; /* 文件操作结果 */
	UINT fnum; /* 文件成功读写数量 */
	BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
	BYTE WriteBuffer[] = "linghaibin very best ha\r\n";
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd = f_mount(&fs,"0:",0);
	/*----------------------- 格式化测试 ---------------------------*/
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if (res_sd == FR_NO_FILESYSTEM) {
		//printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
		/* 格式化 */
		res_sd=f_mkfs("0:",FM_FAT,0,work, sizeof work );
		if (res_sd == FR_OK) {
			//printf("》SD卡已成功格式化文件系统。\r\n");
			/* 格式化后，先取消挂载 */
			res_sd = f_mount(0,"0:",0);
			/* 重新挂载 */
			res_sd = f_mount(&fs,"0:",0);
		} else {
			//printf("《《格式化失败。》》\r\n");
			while (1);
		}
	} else if (res_sd!=FR_OK) {
		//printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
		//printf("！！可能原因：SD卡初始化不成功。\r\n");
		while (1);
	} else {
		//printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}
	/*--------------------- 文件系统测试：写测试 -----------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	//printf("\r\n****** 即将进行文件写入测试... ******\r\n");
	res_sd=f_open(&fnew,"0:lhb.txt",FA_CREATE_ALWAYS|FA_WRITE);
	if ( res_sd == FR_OK ) {
		//printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
		/* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
		if (res_sd==FR_OK) {
			//printf("》文件写入成功，写入字节数据：%d\n",fnum);
			//printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
		} else {
			//printf("！！文件写入失败：(%d)\n",res_sd);
		}
		/* 不再读写，关闭文件 */
		f_close(&fnew);
	} else {
		//printf("！！打开/创建文件失败。\r\n");

	}
	/*------------------ 文件系统测试：读测试 --------------------------*/
	//printf("****** 即将进行文件读取测试... ******\r\n");
	res_sd=f_open(&fnew,"0:lhb.txt",FA_OPEN_EXISTING|FA_READ);
	if (res_sd == FR_OK) {
		//printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
		if (res_sd==FR_OK) {
			//printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
			//printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
		} else {
			//printf("！！文件读取失败：(%d)\n",res_sd);
		}
	} else {
		//printf("！！打开文件失败。\r\n");
	}

	/* 不再读写，关闭文件 */
	f_close(&fnew);
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(0,"0:",0);
	/* 操作完成，停机 */
	while (1) {

	}
}


int main(void) {
	test();
//	SPI_Flash_Init();
//	uint8_t dd[4];
//	SPI_Flash_Write(dd,4,4);
//	 FATFS fs;      /* File system object (volume work area) */
//    FIL fil;       /* File object */
//    FRESULT res;   /* API result code */
//    UINT bw;       /* Bytes written */
//	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
//	
//	SPI_Flash_Init();
//    /* Register work area */
//    f_mount(&fs, "0", 0);

//    /* Create FAT volume with default cluster size */
//    res = f_mkfs("0", FM_ANY, 0, work, sizeof work );
//    if (res == FR_OK){
//		work[0] = 1;
//	}
//	
//    /* Create a file as new */
//    res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
//    if (res == FR_OK){
//		work[0] = 1;
//	}

//    /* Write a message */
//    f_write(&fil, "Hello, World!\r\n", 15, &bw);
//    if (bw != 15){
//		work[0] = 1;
//	}

//    /* Close the file */
//    f_close(&fil);

//    /* Unregister work area */
//    f_mount(0, "", 0);
	while(1);
}
