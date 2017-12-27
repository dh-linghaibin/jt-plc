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

#define DELAY_mS(t) vTaskDelay(t/portTICK_RATE_MS)
#define DELAY_S(t) DELAY_mS(1000*t)
#define DELAY_M(t) DELAY_S(60*t)

static usart_obj usart = {
	.init 	  = usart_init,
	.sen_byte = usart_sen_byte,
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
		/* ´ò°ü½âÎö */
		can_package_obj *pack = can_bus.get_packget(&can_bus);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			DELAY_mS(1);
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
			DELAY_mS(1);
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

uint8_t tcp_server_tsta=0XFF;
uint8_t tcp_client_tsta=0XFF;

extern u8 tcp_client_databuf[200];
extern u8 tcp_client_sta;
extern u8 tcp_server_databuf[200];
extern u8 tcp_server_sta;

void can_up_task(void *p){
    for(;;){
		DELAY_mS(1);
		if(tcp_server_tsta!=tcp_server_sta)//TCP Server状态改变
		{
			if(tcp_server_sta&(1<<7)) {
				//led.set_rs232(&led,0);
			} else {
				//led.set_rs232(&led,1);
			}
 			if(tcp_server_sta&(1<<6))	//收到新数据
			{
				tcp_server_sta&=~(1<<6);		//标记数据已经被处理
			}
			tcp_server_tsta=tcp_server_sta;
		}
//		if(key==1)//TCP Server 请求发送数据
//		{
//			if(tcp_server_sta&(1<<7))	//连接还存在
//			{
////				sprintf((char*)tcp_server_databuf,"TCP Server OK %d\r\n",tcnt);
//				tcp_server_sta|=1<<5;//标记有数据需要发送
//				tcnt++;
//			}
//		}
//		if(tcp_client_tsta!=tcp_client_sta)//TCP Client状态改变
//		{
//			if(tcp_client_sta&(1<<7)) {
//					//led.set_rs485(&led,0);
//			} else {
//				//ed.set_rs485(&led,1);
//			}
// 			if(tcp_client_sta&(1<<6))	//收到新数据
//			{
//				tcp_client_sta&=~(1<<6);		//标记数据已经被处理
//			}
//			tcp_client_tsta=tcp_client_sta;
//		}
//		if(key==2)//TCP Client 请求发送数据
//		{
//			if(tcp_client_sta&(1<<7))	//连接还存在
//			{
////				sprintf((char*)tcp_client_databuf,"TCP Client OK %d\r\n",tcnt);
//				tcp_client_sta|=1<<5;//标记有数据需要发送
//				tcnt++;
//			}
//		}
    }
}


static const char program2[] =
"1 v=1\n\
2 l=1500\n\
3 for g = 0 to 5\n\
4 for p = 0 to 7\n\
5 write \"do_8\"\,g,p,v\n\
6 wait l\n\
7 next p\n\
8 next g\n\
9 if v=0 then goto 1\n\
10 if v=1 then v=0\n\
11 goto 2 ";

//50 write \"do_8\"\,1,p,v\n\
//42 read \"di_4\"\,3,2,0\n\

void ubasic_task(void *p){
    for(;;){
		ubasic_init(program2);
		do {
			ubasic_run();
		} while(!ubasic_finished());
		DELAY_mS(1000);
    }
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
	uip_listen(HTONS(1200));

	xTaskCreate(modbus_task, (const char*)"modbus_task", 1024, NULL, 4, NULL);
	xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
	xTaskCreate(ubasic_task, (const char*)"ubasic_task", 1024, NULL, 4, NULL);
	//xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
	vTaskStartScheduler();
}

