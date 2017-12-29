/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "stdio.h"
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
//lua
#include "lua.h"
#include <lauxlib.h>

#define DELAY_mS(t) vTaskDelay(t/portTICK_RATE_MS)
#define DELAY_S(t) DELAY_mS(1000*t)
#define DELAY_M(t) DELAY_S(60*t)

static usart_obj usart = {
	.init 	  = usart_init,
	.sen_byte = usart_sen_byte,
};

static only_id_obj only_id = {
	.id		= {0,},
	.get_id = only_id_get_id,
};

static modbus_obj modbus = {
	.ipv4_ip 			  = {192,168,1,201},
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
	.id 		 = 0xff,
	.send_packed = {0,0,0,0,0,0,0,0},
	.send_msg	 = {0},
	.init 		 = bxcan_init,
	.send 		 = bxcan_send,
	.set_id 	 = bxcan_set_id,
	.get_packget = bxcan_get_packget,
};

can_packr_obj pacckr[PACKAGE_NUM];

void can_task(void *p){
    for(;;) {
		can_package_obj *pack = can_bus.get_packget(&can_bus);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			DELAY_mS(1);
			uint8_t can_rx_flag = 0;
			if(pack->package[i].flag == F_USE) {
				for(int j = 0;j < PACKAGE_NUM;j++) {
					if(pacckr[j].flag == F_USE) {
						if(pacckr[j].id == pack->package[i].dat[0]) {
							for(int k = 0;k < 7;k++) {
								pacckr[j].arr[pacckr[j].pack_bum + k] = pack->package[i].dat[1+k];
							}
							pacckr[j].pack_bum += 7;
							if(pacckr[j].pack_bum >= pacckr[j].len) { 
								pacckr[j].flag = F_PACK_OK; 
							}
							can_rx_flag = 1;
						}
						break;
					}
				}
				if(can_rx_flag == 0) {
					if(pack->package[i].dat[1] == 0x3a) {
						for(int j = 0;j < PACKAGE_NUM;j++) {
							if(pacckr[j].flag == F_NO_USE) { 
								pacckr[j].id = pack->package[i].dat[0];
								pacckr[j].device_id = pack->package[i].dat[2];
								pacckr[j].len = pack->package[i].dat[3];
								pacckr[j].cmd = pack->package[i].dat[4];
								for(int k = 0;k < 3;k++) { 
									pacckr[j].arr[k] = pack->package[i].dat[5+k];
								}
								if(pacckr[j].len <= 3) { 
									pacckr[j].flag = F_PACK_OK;
								} else {
									pacckr[j].pack_bum = 3;
									pacckr[j].flag = F_USE; 
								}
								break;
							}
						}
					}
				}
				pack->package[i].flag = F_NO_USE;
			}
		}
		
		for(int i = 0;i < PACKAGE_NUM;i++) { 
			DELAY_mS(1);
			if(pacckr[i].flag == F_PACK_OK) {
				switch(pacckr[i].device_id) {
					case 0xD0: { 
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
					case 0xD1: { 
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
				pacckr[i].flag = F_NO_USE;
			}
		}
		
		for(int adr = 0;adr < 100;adr++) {
			DELAY_mS(1);
			modbus_coil_obj coil = modbus.up_coil(&modbus,adr);
			if(coil.id != 0xffff) {
				can_bus.send_msg.send_id = coil.id;	 
				can_bus.send_msg.id = can_bus.id;
				can_bus.send_msg.device_id = 0xf0;	 
				can_bus.send_msg.cmd = 0x01;	
				can_bus.send_msg.len = 1;			
				can_bus.send_msg.arr[0] = coil.val;
				can_bus.send(&can_bus);
				DELAY_mS(1);
			} 
		}
	}
}

void modbus_task(void *p) {
	uint16_t count = 0;
    for(;;){
		modbus.loop(&modbus);
		if(count < 200) {
			count++;
		} else {
			count = 0;
			modbus.heart(&modbus);
		}
    }
}


FATFS fs; /* FatFs文件系统对象 */
FIL fnew; /* 文件对象 */
FRESULT res_sd; /* 文件操作结果 */
UINT fnum; /* 文件成功读写数量 */
BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
BYTE WriteBuffer[1024] = {0,};//
BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */

extern uint8_t tcp_server_databuf[200];   	//发送数据缓存	  
extern uint8_t tcp_server_sta;				//服务端状态
uint8_t tcp_server_tsta=0XFF;
extern uint16_t pack_len;

uint8_t file_flag = 0;

uint16_t r_len;


static TaskHandle_t xhande_task_basic = NULL;

void ubasic_task(void *p);

void can_up_task(void *p){
    for(;;){
		if(tcp_server_tsta!=tcp_server_sta)//TCP Server状态改变
		{
			if(tcp_server_sta&(1<<7)) {
				
			} else {
				
			}
 			if(tcp_server_sta&(1<<6))	//收到新数据
			{
				if(pack_len == 1) {
					if(tcp_server_databuf[0] == 'c') {
						file_flag = 1;
						printf("start \n");
						//vTaskSuspend(xhande_task_basic); /* 挂起任务 */
						if(xhande_task_basic!=NULL) {
							vTaskDelete(xhande_task_basic);
							xhande_task_basic=NULL;
						}
					} if(tcp_server_databuf[0] == 'q') {
						file_flag = 2;
						printf("end %s \n",WriteBuffer);
						/*--------------------- 文件系统测试：写测试 -----------------------*/
						res_sd=f_open(&fnew,"0:lhb6.txt",FA_CREATE_ALWAYS|FA_WRITE);
						if ( res_sd == FR_OK ) {
							res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
							f_close(&fnew);
						}
						//f_mount(0,"0:",0);

						res_sd=f_open(&fnew,"0:lhb6.txt",FA_OPEN_EXISTING|FA_READ);
						if (res_sd == FR_OK) {
							printf("open ok\r\n");
							res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
							if (res_sd==FR_OK) {
								printf("read: %d\r\n",fnum);
								printf("read-a: \r\n%s \r\n", ReadBuffer);
							} else {
								printf("file (%d)\n",res_sd);
							}
						} else {
							printf("file\r\n");
						}

						/* 不再读写，关闭文件 */
						f_close(&fnew);
						//vTaskResume(xhande_task_basic);/* 回复任务 */
						xTaskCreate(ubasic_task, 
									(const char*)"ubasic_task", 
									1024, 
									NULL, 
									4, 
									&xhande_task_basic);
					}
				} else {
					switch(file_flag) {
						case 1:
							printf("write %d \n",pack_len);
							for(int i = 0;i < pack_len;i++) {
								WriteBuffer[r_len] = tcp_server_databuf[i];
								r_len++;
							}
							break;
						case 2:	
							
							break;
						case 3:	
							
							break;
					}
				}
				tcp_server_sta&=~(1<<6);		//标记数据已经被处理
			}
			tcp_server_tsta=tcp_server_sta;
		}
    }
}

void theTimerCallback(TimerHandle_t pxTimer) {
//	uint8_t pcWriteBuffer[500];
//	printf("=================================================\r\n");
//	printf("ÈÎÎñÃû      ÈÎÎñ×´Ì¬ ÓÅÏÈ¼¶   Ê£ÓàÕ» ÈÎÎñÐòºÅ\r\n");
//	vTaskList((char *)&pcWriteBuffer);
//	printf("%s\r\n", pcWriteBuffer);

//	printf("\r\nÈÎÎñÃû       ÔËÐÐ¼ÆÊý         Ê¹ÓÃÂÊ\r\n");
//	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//	printf("%s\r\n", pcWriteBuffer);
}

void theTimerInit(int msCount)
{
	TickType_t timertime = (msCount/portTICK_PERIOD_MS);
	TimerHandle_t theTimer = xTimerCreate("theTimer", timertime , pdTRUE, 0, theTimerCallback );
	if( xTimerStart(theTimer, 0) != pdPASS )
	{
		//debugU("Timer failed to start");
	}
}


void ubasic_task(void *p){
    for(;;){
		ubasic_init(ReadBuffer);
		do {
			ubasic_run();
		} while(!ubasic_finished());
		DELAY_mS(1000);
    }
}

void test(void) {
	
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd = f_mount(&fs,"0:",0);
	/*----------------------- 格式化测试 ---------------------------*/
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if (res_sd == FR_NO_FILESYSTEM) {
		//printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
		/* 格式化 */
		res_sd=f_mkfs("0:",FM_FAT,0,work, sizeof work );
		if (res_sd == FR_OK) {
			printf("gsh ok\r\n");
			/* 格式化后，先取消挂载 */
			res_sd = f_mount(0,"0:",0);
			/* 重新挂载 */
			res_sd = f_mount(&fs,"0:",0);
		} else {
			printf("gsh file\r\n");
			while (1);
		}
	} else if (res_sd!=FR_OK) {
		printf("file\r\n");
		while (1);
	} else {
		printf("ok\r\n");
	}
		/*--------------------- 文件系统测试：写测试 -----------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	//printf("\r\n****** 即将进行文件写入测试... ******\r\n");
//	res_sd=f_open(&fnew,"0:lhb6.txt",FA_CREATE_ALWAYS|FA_WRITE);
//	if ( res_sd == FR_OK ) {
//		printf("open ok write \r\n");
//		/* 将指定存储区内容写入到文件内 */
//		res_sd=f_write(&fnew,"linghaibin haha",30,&fnum);
//		if (res_sd==FR_OK) {
//			printf("ok %d\n",fnum);
//		} else {
//			printf("fale\n");
//		}
//		/* 不再读写，关闭文件 */
//		f_close(&fnew);
//	} else {
//		printf("open file\r\n");
//	}
	/*------------------ 文件系统测试：读测试 --------------------------*/
	printf("file read\r\n");
	res_sd=f_open(&fnew,"0:lhb6.txt",FA_OPEN_EXISTING|FA_READ);
	if (res_sd == FR_OK) {
		printf("open ok\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
		if (res_sd==FR_OK) {
			printf("read: %d\r\n",fnum);
			printf("read-a: \r\n%s \r\n", ReadBuffer);
		} else {
			printf("file (%d)\n",res_sd);
		}
	} else {
		printf("file\r\n");
	}

	/* 不再读写，关闭文件 */
	f_close(&fnew);
	/* 不再使用文件系统，取消挂载文件系统 */
	//f_mount(0,"0:",0);
}

void delay() {
	for(int i = 0;i < 200;i++) 
		for(int j = 0;j < 0xffff;j++);
}

int main(void) {
	delay();

	usart.init(&usart,115200);
	led.init(&led);
		
	printf("------------\n");

	test();
	
	only_id.get_id(&only_id);
	can_bus.init(&can_bus);
	/* mac ID */
	modbus.enc28.mac[3] = only_id.id[0];
	modbus.enc28.mac[4] = only_id.id[1];
	modbus.enc28.mac[5] = only_id.id[2];
	modbus.init(&modbus);
	uip_listen(HTONS(1200));

//	lua_State * L=luaL_newstate();
//	if( !L )
//		return 1;
//	//lua_close(L);

//	//L = luaL_newstate();
//	//luaL_openlibs(L);
//	luaL_dostring(L, "print('Hello World.')");
//	lua_close(L);

	lua_State *L = luaL_newstate();  /* create state */
	if (L == NULL) {
		printf("lua,cannot create state: not enough memory\n");
		return 0;
	}
	luaL_openlibs(L);
	//print_version();
	//dostring(L,"print('hello')","Test_lua");
	//dofile(L, NULL);  /* executes stdin as a file */
	//doREPL(L);

	lua_close(L);

	theTimerInit(500);
	xTaskCreate(modbus_task, (const char*)"modbus_task", 1024, NULL, 4, NULL);
	xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
	//xTaskCreate(ubasic_task, (const char*)"ubasic_task", 1024, NULL, 4, &xhande_task_basic);
	xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
	vTaskStartScheduler();
}

