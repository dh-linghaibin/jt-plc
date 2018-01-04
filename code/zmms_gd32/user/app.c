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
#include "wdog.h"
#include "rtc.h"
#include "only_id.h"
#include "sha1.h"
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define DELAY_mS(t) vTaskDelay(t/portTICK_RATE_MS)
#define DELAY_S(t) DELAY_mS(1000*t)
#define DELAY_M(t) DELAY_S(60*t)

static wdog_obj wdog = {
	.init = wdog_init,
	.reload = wdog_reload,
};

static usart_obj usart = {
	.init 	  = usart_init,
	.sen_byte = usart_sen_byte,
};

static only_id_obj only_id = {
	.id		= {0,},
	.get_id = only_id_get_id,
};

 rtc_obj rtc = {
	.init = rtc_init,
	.read = rtc_read,
	.set  = rtc_set,
};

static rs485_obj rs485 = {
	.init 		= rs485_init,
	.get_packet = rs485_get_packet,
};

static modbus_obj modbus = {
	.ipv4_ip 			  = {192,168,1,250},
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

extern int modbus_Holding[120];
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

		rs485_packet_obj* rs485_packet = rs485.get_packet(&rs485);
		for(int i = 0;i < RS485_PACKAGE_NUM;i++) {
			if(rs485_packet->package[i].flag == RF_USE) {
				modbus_Holding[99] = rs485_packet->package[i].back_number*8 + rs485_packet->package[i].number;
				rs485_packet->package[i].flag = RF_NO_USE;
				led.tager(&led,L_RS485);
			}
		}
		wdog.reload(&wdog);
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

void time_task(void *p) {
	for(;;){
		DELAY_mS(950);
		rtc_t time = rtc.read(&rtc);
		modbus_Holding[100] = time.year;
		modbus_Holding[101] = time.month;
		modbus_Holding[102] = time.mday;
		modbus_Holding[103] = time.wday;
		modbus_Holding[104] = time.hour;
		modbus_Holding[105] = time.min;
		modbus_Holding[106] = time.sec;
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


//static const char program[] =
//"1 i = 2000\n\
//2 j = 3\n\
//3 k = ii+j\n\
//4 k = i+j\n\
//5 print k\n\
//";

//static const char program2[] =
//"1 v=1\n\
//2 l=1500\n\
//3 for g = 0 to 5\n\
//4 for p = 0 to 7\n\
//5 write \"do_8\"\,g,p,v\n\
//6 read \"di_4\"\,g,p,v\n\
//7 v = get\n\
//7 wait l\n\
//8 next p\n\
//9 next g\n\
//10 if v=0 then goto 1\n\
//11 if v=1 then v=0\n\
//12 goto 2 ";


//static const char program3[] =
//"1 b=0\n\
//2 read \"holding\"\,10\n\
//3 if z=0 then goto 1\n\
//4 if z=1 then goto 8\n\
//5 if z=2 then goto 21\n\
//8 if b = 1 then goto 2\n\
//51 print b\n\
//10 write \"do_8\"\,1,8,255\n\
//11 write \"do_8\"\,2,8,255\n\
//12 write \"do_8\"\,3,8,255\n\
//13 write \"do_8\"\,4,8,255\n\
//14 write \"do_8\"\,5,8,255\n\
//15 write \"do_8\"\,6,8,255\n\
//16 write \"do_8\"\,7,8,255\n\
//17 write \"do_8\"\,8,8,255\n\
//18 write \"do_8\"\,14,8,255\n\
//19 write \"do_8\"\,15,8,255\n\
//20 goto 2\n\
//21 if b = 2 then goto 2\n\
//22 b=2\n\
//23 write \"do_8\"\,1,8,0\n\
//24 write \"do_8\"\,2,8,0\n\
//25 write \"do_8\"\,3,8,0\n\
//26 write \"do_8\"\,4,8,0\n\
//27 write \"do_8\"\,5,8,0\n\
//28 write \"do_8\"\,6,8,0\n\
//29 write \"do_8\"\,7,8,0\n\
//30 write \"do_8\"\,8,8,0\n\
//31 write \"do_8\"\,14,8,0\n\
//32 write \"do_8\"\,15,8,0\n\
//33 goto 2\n\
//50 read \"holding\"\,99\n\
//51 if z=1 then goto 1\n\
//52 if z=2 then goto 1\n\
//53 if z=3 then goto 1\n\
//54 if z=4 then goto 1\n\
//55 if z=5 then goto 1\n\
//56 if z=6 then goto 1\n\
//57 if z=7 then goto 1\n\
//58 if z=8 then goto 1\n\
//59 if z=9 then goto 1\n\
//60 if z=10 then goto 1\n\
//61 if z=11 then goto 1\n\
//62 if z=12 then goto 1\n\
//63 if z=13 then goto 1\n\
//64 if z=14 then goto 1\n\
//65 if z=15 then goto 1\n\
//66 if z=16 then goto 1\n\
//67 goto 2\n\
//70 write \"do_8\"\,1,8,255\n\
//75 goto 2\n\
//76 write \"do_8\"\,1,8,0\n\
//80 goto 2\n\
//81 write \"do_8\"\,1,8,255\n\
//85 goto 2\n\
//86 write \"do_8\"\,1,8,0\n\
//90 goto 2\n\
//91 write \"do_8\"\,1,8,255\n\
//95 goto 2\n\
//96 write \"do_8\"\,1,8,0\n\
//100 goto 2\n\
//101 write \"do_8\"\,1,8,255\n\
//105 goto 2\n\
//106 write \"do_8\"\,1,8,0\n\
//110 goto 2\n\
//111 write \"do_8\"\,1,8,255\n\
//115 goto 2\n\
//116 write \"do_8\"\,1,8,0\n\
//120 goto 2\n\
//121 write \"do_8\"\,1,8,255\n\
//125 goto 2\n\
//";

//static const char program4[] =
//"2 read \"holding\"\,99\n\
//51 if z=1 then goto 70\n\
//52 if z=2 then goto 76\n\
//53 if z=3 then goto 81\n\
//54 if z=4 then goto 86\n\
//55 if z=5 then goto 90\n\
//56 if z=6 then goto 96\n\
//57 if z=7 then goto 101\n\
//58 if z=8 then goto 106\n\
//59 if z=9 then goto 111\n\
//60 if z=10 then goto 116\n\
//61 if z=11 then goto 121\n\
//62 if z=12 then goto 126\n\
//63 if z=13 then goto 131\n\
//64 if z=14 then goto 140\n\
//65 if z=15 then goto 146\n\
//66 if z=16 then goto 151\n\
//67 goto 2\n\
//70 if c = 1 them goto 2\n\
//71 c = 1\n\
//72 write \"do_8\"\,1,8,255\n\
//75 goto 2\n\
//76 write \"do_8\"\,1,8,0\n\
//80 goto 2\n\
//81 write \"do_8\"\,1,8,255\n\
//85 goto 2\n\
//86 write \"do_8\"\,1,8,0\n\
//90 goto 2\n\
//91 write \"do_8\"\,1,8,255\n\
//95 goto 2\n\
//96 write \"do_8\"\,1,8,0\n\
//100 goto 2\n\
//101 write \"do_8\"\,1,8,255\n\
//105 goto 2\n\
//106 write \"do_8\"\,1,8,0\n\
//110 goto 2\n\
//111 write \"do_8\"\,1,8,255\n\
//115 goto 2\n\
//116 write \"do_8\"\,1,8,0\n\
//120 goto 2\n\
//121 write \"do_8\"\,1,8,255\n\
//125 goto 2\n\
//126 write \"do_8\"\,1,8,255\n\
//130 goto 2\n\
//131 write \"do_8\"\,1,8,255\n\
//135 goto 2\n\
//140 write \"do_8\"\,1,8,255\n\
//145 goto 2\n\
//146 write \"do_8\"\,1,8,255\n\
//150 goto 2\n\
//151 write \"do_8\"\,1,8,255\n\
//156 goto 2\n\
//";

extern uint8_t modbus_coil_r[100];
extern uint8_t modbus_input[100];

void write_coil(uint8_t addr,uint8_t num,uint8_t val) {
	if(val == 0) {
		modbus_coil_r[addr] &= ~(1 << num);
	} else {
		modbus_coil_r[addr] |= (1 << num);
	}
}

void ubasic_task(void *p){
	rtc_t time = rtc.read(&rtc);
	modbus_Holding[100] = time.year;
	modbus_Holding[101] = time.month;
	modbus_Holding[102] = time.mday;
	modbus_Holding[103] = time.wday;
	modbus_Holding[104] = time.hour;
	modbus_Holding[105] = time.min;
	modbus_Holding[106] = time.sec;
    for(;;){
//		ubasic_init(program4);
//		//ubasic_init(ReadBuffer);
//		do {
//			ubasic_run();
//		} while(!ubasic_finished());
		/*键盘控制*/
		switch(modbus_Holding[99]) {
			case 0:{
				
			}break;
			case 1:{
				modbus_coil_r[1] = 255;
				modbus_coil_r[2] = 255;
				modbus_coil_r[3] = 255;
				modbus_coil_r[4] = 255;
				modbus_coil_r[5] = 255;
				modbus_coil_r[6] = 255;
				modbus_coil_r[7] = 255;
				modbus_coil_r[8] = 255;
				modbus_coil_r[14] = 255;
				modbus_coil_r[15] = 255;
			}break;
			case 2:{
				write_coil(1,5,1);
				write_coil(1,6,1);
				write_coil(2,0,1);
				write_coil(3,0,1);
				write_coil(3,2,1);
				write_coil(3,3,1);
				write_coil(3,4,1);
				write_coil(4,3,1);
				write_coil(5,1,1);
				write_coil(5,2,1);
				write_coil(5,3,1);
				write_coil(5,5,1);
				write_coil(6,0,1);
				write_coil(7,3,1);
				write_coil(7,4,1);
				write_coil(7,6,1);
				write_coil(8,0,1);
				write_coil(8,1,1);
				write_coil(8,5,1);
				write_coil(8,6,1);
				write_coil(14,4,1);
				write_coil(14,7,1);
			}break;
			case 3:{
				write_coil(1,2,1);
				write_coil(1,3,1);
				write_coil(1,4,1);
				write_coil(2,1,1);
				write_coil(3,6,1);
				write_coil(4,1,1);
				write_coil(7,0,1);
				write_coil(7,2,1);
			}break;
			case 4:{
				write_coil(1,0,1);
				write_coil(1,1,1);
				write_coil(1,7,1);
				write_coil(2,2,1);
				write_coil(3,1,1);
				write_coil(3,5,1);
				write_coil(3,7,1);
				write_coil(4,0,1);
				write_coil(4,2,1);
				write_coil(5,0,1);
				write_coil(5,4,1);
				write_coil(5,6,1);
				write_coil(5,7,1);
				write_coil(7,5,1);
				write_coil(8,2,1);
				write_coil(8,3,1);
				write_coil(8,4,1);
				write_coil(14,0,1);
				write_coil(14,1,1);
				write_coil(14,2,1);
				write_coil(14,3,1);
				write_coil(14,5,1);
				write_coil(14,6,1);
				write_coil(15,1,1);
				write_coil(15,2,1);
				write_coil(15,3,1);
				write_coil(15,4,1);
				write_coil(15,5,1);
			}break;
			case 5:{
				modbus_coil_r[1] = 0;
				modbus_coil_r[2] = 0;
				modbus_coil_r[3] = 0;
				modbus_coil_r[4] = 0;
				modbus_coil_r[5] = 0;
				modbus_coil_r[6] = 0;
				modbus_coil_r[7] = 0;
				modbus_coil_r[8] = 0;
				modbus_coil_r[14] = 0;
				modbus_coil_r[15] = 0;
			}break;
			case 6:{
				write_coil(1,5,0);
				write_coil(1,6,0);
				write_coil(2,0,0);
				write_coil(3,0,0);
				write_coil(3,2,0);
				write_coil(3,3,0);
				write_coil(3,4,0);
				write_coil(4,3,0);
				write_coil(5,1,0);
				write_coil(5,2,0);
				write_coil(5,3,0);
				write_coil(5,5,0);
				write_coil(6,0,0);
				write_coil(7,3,0);
				write_coil(7,4,0);
				write_coil(7,6,0);
				write_coil(8,0,0);
				write_coil(8,1,0);
				write_coil(8,5,0);
				write_coil(8,6,0);
				write_coil(14,4,0);
				write_coil(14,7,0);
			}break;
			case 7:{
				write_coil(1,2,0);
				write_coil(1,3,0);
				write_coil(1,4,0);
				write_coil(2,1,0);
				write_coil(3,6,0);
				write_coil(4,1,0);
				write_coil(7,0,0);
				write_coil(7,2,0);
			}break;
			case 8:{
				write_coil(1,0,0);
				write_coil(1,1,0);
				write_coil(1,7,0);
				write_coil(2,2,0);
				write_coil(3,1,0);
				write_coil(3,5,0);
				write_coil(3,7,0);
				write_coil(4,0,0);
				write_coil(4,2,0);
				write_coil(5,0,0);

				write_coil(5,4,0);
				write_coil(5,6,0);
				write_coil(5,7,0);
				write_coil(7,5,0);
				write_coil(8,2,0);
				write_coil(8,3,0);
				write_coil(8,4,0);
				write_coil(14,0,0);
				write_coil(14,1,0);
				write_coil(14,2,0);
				write_coil(14,3,0);
		
				write_coil(14,5,0);
				write_coil(14,6,0);
				write_coil(15,1,0);
				write_coil(15,2,0);
				write_coil(15,3,0);
				write_coil(15,4,0);
				write_coil(15,5,0);
			}break;
			case 9:{
				write_coil(9,0,1);
				write_coil(9,1,1);
				write_coil(9,2,1);
			}break;
			case 10:{
				write_coil(9,3,1);
				write_coil(9,4,1);
				write_coil(9,5,1);
				write_coil(9,6,1);
				write_coil(9,7,1);
				write_coil(10,0,1);
				write_coil(11,0,1);
				write_coil(11,1,1);
				write_coil(11,2,1);
				write_coil(11,3,1);
				write_coil(11,4,1);
				write_coil(11,5,1);
				write_coil(12,0,1);
				write_coil(12,1,1);
				write_coil(12,2,1);
				write_coil(12,3,1);
				write_coil(12,4,1);
				write_coil(12,5,1);
				write_coil(13,0,1);
				write_coil(13,1,1);
				write_coil(13,2,1);
				write_coil(13,3,1);
				write_coil(13,4,1);
				write_coil(13,5,1);
			}break;
			case 11:{
				write_coil(8,0,1);
				write_coil(8,1,1);
				write_coil(8,2,1);
				write_coil(8,3,1);
				write_coil(8,4,1);
				write_coil(8,5,1);
				write_coil(8,6,1);
				write_coil(7,0,1);
				write_coil(7,2,1);
				write_coil(7,3,1);
				write_coil(7,4,1);
				write_coil(7,5,1);
				write_coil(7,6,1);
			}break;
			case 12:{
				write_coil(3,0,1);
				write_coil(3,1,1);
				write_coil(3,2,1);
				write_coil(3,3,1);
				write_coil(3,4,1);
				write_coil(3,5,1);
				write_coil(3,6,1);
				write_coil(3,7,1);
				write_coil(4,0,1);
				write_coil(4,1,1);
				write_coil(4,2,1);
				write_coil(4,3,1);
				write_coil(5,0,1);
				write_coil(5,1,1);
				write_coil(5,2,1);
				write_coil(5,3,1);
				write_coil(5,4,1);
				write_coil(5,5,1);
				write_coil(5,6,1);
				write_coil(5,7,1);
				write_coil(6,0,1);
			}break;
			case 13:{
				write_coil(9,0,0);
				write_coil(9,1,0);
				write_coil(9,2,0);
			}break;
			case 14:{
				write_coil(9,3,0);
				write_coil(9,4,0);
				write_coil(9,5,0);
				write_coil(9,6,0);
				write_coil(9,7,0);
				write_coil(10,0,0);
				write_coil(11,0,0);
				write_coil(11,1,0);
				write_coil(11,2,0);
				write_coil(11,3,0);
				write_coil(11,4,0);
				write_coil(11,5,0);
				write_coil(12,0,0);
				write_coil(12,1,0);
				write_coil(12,2,0);
				write_coil(12,3,0);
				write_coil(12,4,0);
				write_coil(12,5,0);
				write_coil(13,0,0);
				write_coil(13,1,0);
				write_coil(13,2,0);
				write_coil(13,3,0);
				write_coil(13,4,0);
				write_coil(13,5,0);
			}break;
			case 15:{
				write_coil(8,0,0);
				write_coil(8,1,0);
				write_coil(8,2,0);
				write_coil(8,3,0);
				write_coil(8,4,0);
				write_coil(8,5,0);
				write_coil(8,6,0);
				write_coil(7,0,0);
				write_coil(7,2,0);
				write_coil(7,3,0);
				write_coil(7,4,0);
				write_coil(7,5,0);
				write_coil(7,6,0);
			}break;
			case 16:{
				write_coil(3,0,0);
				write_coil(3,1,0);
				write_coil(3,2,0);
				write_coil(3,3,0);
				write_coil(3,4,0);
				write_coil(3,5,0);
				write_coil(3,6,0);
				write_coil(3,7,0);
				write_coil(4,0,0);
				write_coil(4,1,0);
				write_coil(4,2,0);
				write_coil(4,3,0);
				write_coil(5,0,0);
				write_coil(5,1,0);
				write_coil(5,2,0);
				write_coil(5,3,0);
				write_coil(5,4,0);
				write_coil(5,5,0);
				write_coil(5,6,0);
				write_coil(5,7,0);
				write_coil(6,0,0);
			}break;
		}
		if(modbus_Holding[99] > 0) {
			modbus_Holding[99] = 0;
		}
		if(modbus_Holding[11] == 1) {
			/*时间控制*/
			static uint8_t time_flag = 0;
			if( (modbus_Holding[104] == modbus_Holding[1]) && (modbus_Holding[105] == modbus_Holding[2]) ) {
				if(time_flag == 0) {
					modbus_coil_r[1] = 255;
					modbus_coil_r[2] = 255;
					modbus_coil_r[3] = 255;
					modbus_coil_r[4] = 255;
					modbus_coil_r[5] = 255;
					modbus_coil_r[6] = 255;
					modbus_coil_r[7] = 255;
					modbus_coil_r[8] = 255;
					modbus_coil_r[14] = 255;
					modbus_coil_r[15] = 255;
				}
				time_flag = 1;
			} else if( (modbus_Holding[104] == modbus_Holding[3]) && (modbus_Holding[105] == modbus_Holding[4]) ) {
				if(time_flag == 0) {
					modbus_coil_r[1] = 0;
					modbus_coil_r[2] = 0;
					modbus_coil_r[3] = 0;
					modbus_coil_r[4] = 0;
					modbus_coil_r[5] = 0;
					modbus_coil_r[6] = 0;
					modbus_coil_r[7] = 0;
					modbus_coil_r[8] = 0;
					modbus_coil_r[14] = 0;
					modbus_coil_r[15] = 0;
				}
				time_flag = 1;
			} else {
				time_flag = 0;
			}
		}
		if(modbus_Holding[12] == 1) {
			/*光电控制*/
			static uint8_t shangshengyan = 0;
			if(modbus_input[1] == 0x08) {
				if(shangshengyan == 0) {
					modbus_coil_r[1] = 255;
					modbus_coil_r[2] = 255;
					modbus_coil_r[3] = 255;
					modbus_coil_r[4] = 255;
					modbus_coil_r[5] = 255;
					modbus_coil_r[6] = 255;
					modbus_coil_r[7] = 255;
					modbus_coil_r[8] = 255;
					modbus_coil_r[14] = 255;
					modbus_coil_r[15] = 255;
				}
				shangshengyan = 1;
			} else {
				if(shangshengyan == 1) {
					modbus_coil_r[1] = 0;
					modbus_coil_r[2] = 0;
					modbus_coil_r[3] = 0;
					modbus_coil_r[4] = 0;
					modbus_coil_r[5] = 0;
					modbus_coil_r[6] = 0;
					modbus_coil_r[7] = 0;
					modbus_coil_r[8] = 0;
					modbus_coil_r[14] = 0;
					modbus_coil_r[15] = 0;
				}
				shangshengyan = 0;
			}
		}
		
		/*全开*/
		switch(modbus_Holding[10]) {
			case 1:{
				modbus_coil_r[1] = 255;
				modbus_coil_r[2] = 255;
				modbus_coil_r[3] = 255;
				modbus_coil_r[4] = 255;
				modbus_coil_r[5] = 255;
				modbus_coil_r[6] = 255;
				modbus_coil_r[7] = 255;
				modbus_coil_r[8] = 255;
				modbus_coil_r[14] = 255;
				modbus_coil_r[15] = 255;
			}break;
			case 2:{
				modbus_coil_r[1] = 0;
				modbus_coil_r[2] = 0;
				modbus_coil_r[3] = 0;
				modbus_coil_r[4] = 0;
				modbus_coil_r[5] = 0;
				modbus_coil_r[6] = 0;
				modbus_coil_r[7] = 0;
				modbus_coil_r[8] = 0;
				modbus_coil_r[14] = 0;
				modbus_coil_r[15] = 0;
			}break;
		}
		if(modbus_Holding[10] > 0) {
			modbus_Holding[10] = 0;
		}
		DELAY_mS(50);
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


//static TimerHandle_t xTimers = NULL;

//static void vTimerCallback(xTimerHandle pxTimer) {
//	configASSERT(pxTimer);
//	printf("time\n");
//}

//static void AppObjCreate(void) {
//	uint8_t i;
//	const TickType_t  xTimerPer = 100;

//	xTimers = xTimerCreate("Timer",          
//	xTimerPer,   
//	pdTRUE,        
//	(void *) 0,    
//	vTimerCallback); 

//	if(xTimers == NULL) {

//	} else {
//		if(xTimerStart(xTimers, 0) != pdPASS) {
//		
//		}
//	}
//}


void delay() {
	for(int i = 0;i < 200;i++) 
		for(int j = 0;j < 0xffff;j++);
}

int main(void) {
	delay();
	usart.init(&usart,115200);
	rs485.init(&rs485);
	led.init(&led);
	rtc.init(&rtc);
	test();
	only_id.get_id(&only_id);
	can_bus.init(&can_bus);
	/* mac ID */
	modbus.enc28.mac[3] = only_id.id[0];
	modbus.enc28.mac[4] = only_id.id[1];
	modbus.enc28.mac[5] = only_id.id[2];
	modbus.init(&modbus);
	uip_listen(HTONS(1200));
	wdog.init(&wdog);
	xTaskCreate(modbus_task, (const char*)"modbus_task", 1024, NULL, 4, NULL);
	xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
	xTaskCreate(ubasic_task, (const char*)"ubasic_task", 512, NULL, 4, &xhande_task_basic);
	//xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
	xTaskCreate(time_task, (const char*)"time_task", 512, NULL, 4, NULL);
	vTaskStartScheduler();
}

