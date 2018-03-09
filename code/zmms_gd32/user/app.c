/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "stdio.h"
#include <stdarg.h>

#include "led.h"
#include "can.h"
#include "modbus.h"
#include "usart.h"
#include "wdog.h"
#include "rtc.h"
#include "only_id.h"
#include "sha1.h"
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "fsm.h"


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

extern uint8_t modbus_coil_r[100];
extern uint8_t modbus_input[100];

void write_coil(uint8_t addr,uint8_t num,uint8_t val) {
	if(val == 0) {
		modbus_coil_r[addr] &= ~(1 << num);
	} else {
		modbus_coil_r[addr] |= (1 << num);
	}
}

void can_task(void *p){
    while(1) {
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
				switch(pacckr[i].cmd) {
					case 0xf1: { /* ���� */
						can_bus.send_msg.send_id = pacckr[i].id;	 
						can_bus.send_msg.id = can_bus.id;
						can_bus.send_msg.device_id = 0xf0;	 
						can_bus.send_msg.cmd = 0xf1;/* ���������ź� */	
						can_bus.send_msg.len = 1;			
						can_bus.send_msg.arr[0] = 0xff;
						can_bus.send(&can_bus);
					} break;
					default: {
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
					} break;
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
//		wdog.reload(&wdog);
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

static TaskHandle_t xhande_task_basic = NULL;

void ubasic_task(void *p);

void can_up_task(void *p){
    for(;;){
    }
}

//extern uint8_t modbus_coil_r[100];
//extern uint8_t modbus_input[100];

//void write_coil(uint8_t addr,uint8_t num,uint8_t val) {
//	if(val == 0) {
//		modbus_coil_r[addr] &= ~(1 << num);
//	} else {
//		modbus_coil_r[addr] |= (1 << num);
//	}
//}

void ubasic_task(void *p){
	rtc_t time = rtc.read(&rtc);
	modbus_Holding[100] = time.year;
	modbus_Holding[101] = time.month;
	modbus_Holding[102] = time.mday;
	modbus_Holding[103] = time.wday;
	modbus_Holding[104] = time.hour;
	modbus_Holding[105] = time.min;
	modbus_Holding[106] = time.sec;
	DELAY_mS(50);

//	can_bus.send_msg.send_id = 0x07;	 
//	can_bus.send_msg.id = can_bus.id;
//	can_bus.send_msg.device_id = 0xf0;	 
//	can_bus.send_msg.cmd = 0x01;	
//	can_bus.send_msg.len = 1;			
//	can_bus.send_msg.arr[0] = 0xff;
//	can_bus.send(&can_bus);
}

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

	only_id.get_id(&only_id);
	can_bus.init(&can_bus);
	/* mac ID */
	modbus.enc28.mac[3] = only_id.id[0];
	modbus.enc28.mac[4] = only_id.id[1];
	modbus.enc28.mac[5] = only_id.id[2];
	modbus.init(&modbus);
	uip_listen(HTONS(1200));
	//wdog.init(&wdog);
	xTaskCreate(modbus_task, (const char*)"modbus_task", 1024, NULL, 4, NULL);
	xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
	//xTaskCreate(ubasic_task, (const char*)"ubasic_task", 512, NULL, 4, &xhande_task_basic);
	//xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
	xTaskCreate(time_task, (const char*)"time_task", 512, NULL, 4, NULL);
	vTaskStartScheduler();
}

