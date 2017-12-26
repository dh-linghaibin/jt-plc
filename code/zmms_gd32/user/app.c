/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include <stdio.h>
#include <stdarg.h>

#include "ff.h"
#include "led.h"
#include "can.h"
#include "modbus.h"
#include "ubasic.h"
#include "usart.h"
#include "only_id.h"
#include "sha1.h"
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define DELAY_mS(t) vTaskDelay(t/portTICK_RATE_MS)
#define DELAY_S(t) DELAY_mS(1000*t)
#define DELAY_M(t) DELAY_S(60*t)

static usart_obj usart = {
	.init = usart_init,
};

static only_id_obj only_id = {
	.id = {0,},
	.get_id = only_id_get_id,
};

static modbus_obj modbus = {
	.ipv4_ip 			  = {192,168,1,200},
	.gateway_ip 		  = {192,168,1,1},
	.mask_ip 			  = {255,255,255,0},
	.uip_mac.addr 	      = {0,},
	.enc28.mac 			  = {0x05,0x01,0x35,0x01,0x00,0x01},
	.enc28.init 		  = en28j60_init,
	.enc28.packet_receive = en28j60_packet_receive,
	.enc28.packet_send 	  = en28j60_packet_send,
	.enc28.set_mac 		  = en28j60_set_mac,
	.init 				  = modbus_init,
	.loop 				  = modbus_loop,
	.heart 				  = modbus_heart,
	.set_coil			  = modbus_set_coil,
	.set_input			  = modbus_set_input,
	.up_coil 			  = modbus_up_coil,
};

static led_obj led = {
	.init  = led_init,
	.set   = led_set,
	.tager = led_tager,
};

static can_obj can_bus = {
	.id = 0xff,
	.send_packed = {0,0,0,0,0,0,0,0},
	.send_msg = {0},
	.init = bxcan_init,
	.send = bxcan_send,
	.set_id = bxcan_set_id,
	.get_packget = bxcan_get_packget,
};

can_packr_obj pacckr[PACKAGE_NUM];

void can_task(void *p){
    for(;;) {
        DELAY_mS(200);
		/* ´ò°ü½âÎö */
		can_package_obj *pack = can_bus.get_packget(&can_bus);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			uint8_t can_rx_flag = 0;
			if(pack->package[i].flag == F_USE) { /* »ñÈ¡Êý¾Ý */
				for(int j = 0;j < PACKAGE_NUM;j++) {
					if(pacckr[j].flag == F_USE) { /* ÅÐ¶ÏÊÇ·ñÊ¹ÓÃ */
						if(pacckr[j].id == pack->package[i].dat[0]) { /* ÅÐ¶ÏIDÊÇ·ñÏàÍ¬ */
							for(int k = 0;k < 7;k++) { /* ´ò°ü */
								pacckr[j].arr[pacckr[j].pack_bum + k] = pack->package[i].dat[1+k];
							}
							pacckr[j].pack_bum += 7;
							if(pacckr[j].pack_bum >= pacckr[j].len) { /* ÅÐ¶Ï´ò°üÊÇ·ñÍê³É */
								pacckr[j].flag = F_PACK_OK; /* ´ò°üÍê³É */
							}
							can_rx_flag = 1;
						}
						break;
					}
				}
				if(can_rx_flag == 0) {
					if(pack->package[i].dat[1] == 0x3a) { /* ÅÐ¶ÏÕâ¸öÒ»Ö¡ÊÇ²»ÊÇÍ·Õë */
						for(int j = 0;j < PACKAGE_NUM;j++) {
							if(pacckr[j].flag == F_NO_USE) { /* Ñ°ÕÒÎ´Ê¹ÓÃ°ü */
								pacckr[j].id = pack->package[i].dat[0]; /* »ñÈ¡ID */
								pacckr[j].device_id = pack->package[i].dat[2];
								pacckr[j].len = pack->package[i].dat[3];
								pacckr[j].cmd = pack->package[i].dat[4];
								for(int k = 0;k < 3;k++) { /* ´ò°ü */
									pacckr[j].arr[k] = pack->package[i].dat[5+k];
								}
								if(pacckr[j].len <= 3) { 
									pacckr[j].flag = F_PACK_OK; /* ´ò°üÍê³É */
								} else {
									pacckr[j].pack_bum = 3;
									pacckr[j].flag = F_USE; /* ÌáÊ¾µÚÒ»¸ö°üÒÑ¾­´ò°üÍê³É */
								}
								break;
							}
						}
					}
				}
				pack->package[i].flag = F_NO_USE;//±íÊ¾Õâ¸öÒÑ¾­´ò°üÍê³É
			}
		}
		
		/* ÔËÐÐÃüÁî */
		for(int i = 0;i < PACKAGE_NUM;i++) { 
			if(pacckr[i].flag == F_PACK_OK) {
				switch(pacckr[i].device_id) {
					case 0xD0: { /* °ËÎ»Êý×ÖÊä³ö */
						switch(pacckr[i].cmd) {
							case 0: 
							
							break;
							case 1: 
								led.tager(&led,L_CAN);
								modbus.set_coil(&modbus,pacckr[i].id,pacckr[i].arr[0]);
							break;	
							case 2:

							break;
							case 3:

							break;
						}
					}
						break;
					case 0xD1: { /* ËÄÎ»Êý×ÖÊäÈë */
						switch(pacckr[i].cmd) {
								case 0: 
								
								break;
								case 1: 
									led.tager(&led,L_CAN);
									modbus.set_input(&modbus,pacckr[i].id,pacckr[i].arr[0]);
								break;	
								case 2:

								break;
								case 3:

								break;
							}
					}
						break;
				}
				pacckr[i].flag = F_NO_USE;/* Êý¾Ý´¦ÀíÍê±Ï */
			}
		}
		DELAY_mS(200);
		modbus_coil_obj coil = modbus.up_coil(&modbus);
		if(coil.id != 0xffff) {
			can_bus.send_msg.send_id = coil.id;	 
			can_bus.send_msg.id = can_bus.id;
			can_bus.send_msg.device_id = 0xf0;	 
			can_bus.send_msg.cmd = 0x01;	
			can_bus.send_msg.len = 1;			
			can_bus.send_msg.arr[0] = coil.val;
			can_bus.send(&can_bus);
		}
    }
}


void modbus_task(void *p){
	uint16_t count = 0;
    for(;;){
		modbus.loop(&modbus);
		if(count < 200) {
			count++;
		} else {
			count = 0;
			modbus.heart(&modbus);
		}
		//DELAY_mS(10);
    }
}

// add by user named lux
int f_printf_0 (                        /* Put a formatted string to the file */
                FIL* fil, 
                const TCHAR* str,
                ...)                
{
  INT len;
  char buff[512];  
  
  va_list args;
  va_start(args,str);
  len = vsnprintf(buff,512,str,args);
  if (len < 0 ) 
  {
    va_end(args);
    return len;
  }
  len = f_puts ((TCHAR *)buff,fil);
  va_end(args);
  
  return len;  
}

void csv_test(void) {
	FATFS fs; /* FatFs文件系统对象 */
	FIL fnew; /* 文件对象 */
	FRESULT res_sd; /* 文件操作结果 */
	UINT fnum; /* 文件成功读写数量 */
	BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
	BYTE WriteBuffer[] = {255,192,168,1,0};//
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
	res_sd=f_open(&fnew,"0:lhb6.txt",FA_CREATE_ALWAYS|FA_WRITE);
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
	res_sd=f_open(&fnew,"0:lhb6.txt",FA_OPEN_EXISTING|FA_READ);
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

	u8 oldMin=20;
	int r;
	res_sd=f_open(&fnew,"0:TEMP.TXT",  FA_OPEN_ALWAYS |FA_WRITE);  // open file

	f_lseek(&fnew, f_size(&fnew));   // point to the end of a file

	res_sd=f_printf_0(&fnew, "%4d/\r\n",oldMin); 

	res_sd=f_sync (&fnew);  // close file
	f_close(&fnew);

	res_sd=f_open(&fnew,"0:TEMP.TXT",FA_OPEN_EXISTING|FA_READ);
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
}

int main(void) {
	usart.init(&usart,115200);
	led.init(&led);
	only_id.get_id(&only_id);
	can_bus.init(&can_bus);
	/* mac ID */
	modbus.enc28.mac[3] = only_id.id[0];
	modbus.enc28.mac[4] = only_id.id[1];
	modbus.enc28.mac[5] = only_id.id[2];
	modbus.init(&modbus);
		
	csv_test();

	xTaskCreate(modbus_task, (const char*)"modbus_task", 1024, NULL, 4, NULL);
	xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
	vTaskStartScheduler();
}

