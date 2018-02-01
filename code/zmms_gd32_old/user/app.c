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
#include "rtc.h"
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "ringbuf.h"

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

 rtc_obj rtc = {
	.init = rtc_init,
	.read = rtc_read,
	.set  = rtc_set,
};

static modbus_obj modbus = {
	.ipv4_ip 			  = {192,168,1,203},
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
	.id 		 = 0xfe,
	.send_packed = {0,0,0,0,0,0,0,0},
	.send_msg	 = {0},
	.init 		 = bxcan_init,
	.send 		 = bxcan_send,
	.set_id 	 = bxcan_set_id,
	.get_packget = bxcan_get_packget,
};


volatile CanTxMsg TxMessage;
extern Stdringbuf RINGBUF;

uint8_t plc_coil_val[100];

can_packr_obj pacckr[PACKAGE_NUM];

extern uint8_t plc_set_coil[20];

void can_task(void *p){
	uint8_t buf[8];
	while(1) {
		if(RINGBUF.get(&RINGBUF.ringbuf_n,buf,8)) {   
			switch(buf[1]) {
				case 0x61:{
					plc_coil_val[buf[0]] = buf[7];
				} break;
			}
		}
		DELAY_mS(1);
		switch(plc_set_coil[1]) {
            case 1:{
                TxMessage.StdId=plc_set_coil[2];
                TxMessage.RTR=CAN_RTR_DATA;
                TxMessage.IDE=CAN_ID_STD;
                TxMessage.DLC=8;
                TxMessage.Data[0]=plc_set_coil[3];
                TxMessage.Data[1]=plc_set_coil[4];
                TxMessage.Data[2]=plc_set_coil[5];
                TxMessage.Data[3]=plc_set_coil[6];
                TxMessage.Data[4]=plc_set_coil[7];
                TxMessage.Data[5]=plc_set_coil[8];
                TxMessage.Data[6]=plc_set_coil[9];
                TxMessage.Data[7]=plc_set_coil[10];
                CAN_tx_data(TxMessage);
				DELAY_mS(1);
                plc_set_coil[1] = 0;
            }
                break;
            case 2:{
                TxMessage.StdId=plc_set_coil[11];
                TxMessage.RTR=CAN_RTR_DATA;
                TxMessage.IDE=CAN_ID_STD;
                TxMessage.DLC=8;
                TxMessage.Data[0]=plc_set_coil[12];
                TxMessage.Data[1]=plc_set_coil[13];
                TxMessage.Data[2]=plc_set_coil[14];
                TxMessage.Data[3]=plc_set_coil[15];
                TxMessage.Data[4]=plc_set_coil[16];
                TxMessage.Data[5]=plc_set_coil[17];
                TxMessage.Data[6]=plc_set_coil[18];
                TxMessage.Data[7]=plc_set_coil[19];
                CAN_tx_data(TxMessage);
				DELAY_mS(1);
                plc_set_coil[1] = 1;
            }
            break;
        }       
	}
//    for(;;) {
//		can_package_obj *pack = can_bus.get_packget(&can_bus);
//		for(int i = 0;i < PACKAGE_NUM;i++) {
//			DELAY_mS(1);
//			uint8_t can_rx_flag = 0;
//			if(pack->package[i].flag == F_USE) {
//				for(int j = 0;j < PACKAGE_NUM;j++) {
//					if(pacckr[j].flag == F_USE) {
//						if(pacckr[j].id == pack->package[i].dat[0]) {
//							for(int k = 0;k < 7;k++) {
//								pacckr[j].arr[pacckr[j].pack_bum + k] = pack->package[i].dat[1+k];
//							}
//							pacckr[j].pack_bum += 7;
//							if(pacckr[j].pack_bum >= pacckr[j].len) { 
//								pacckr[j].flag = F_PACK_OK; 
//							}
//							can_rx_flag = 1;
//						}
//						break;
//					}
//				}
//				if(can_rx_flag == 0) {
//					if(pack->package[i].dat[1] == 0x3a) {
//						for(int j = 0;j < PACKAGE_NUM;j++) {
//							if(pacckr[j].flag == F_NO_USE) { 
//								pacckr[j].id = pack->package[i].dat[0];
//								pacckr[j].device_id = pack->package[i].dat[2];
//								pacckr[j].len = pack->package[i].dat[3];
//								pacckr[j].cmd = pack->package[i].dat[4];
//								for(int k = 0;k < 3;k++) { 
//									pacckr[j].arr[k] = pack->package[i].dat[5+k];
//								}
//								if(pacckr[j].len <= 3) { 
//									pacckr[j].flag = F_PACK_OK;
//								} else {
//									pacckr[j].pack_bum = 3;
//									pacckr[j].flag = F_USE; 
//								}
//								break;
//							}
//						}
//					}
//				}
//				pack->package[i].flag = F_NO_USE;
//			}
//		}
//		
//		for(int i = 0;i < PACKAGE_NUM;i++) { 
//			DELAY_mS(1);
//			if(pacckr[i].flag == F_PACK_OK) {
//				switch(pacckr[i].device_id) {
//					case 0xD0: { 
//						switch(pacckr[i].cmd) {
//							case 0: 
//							
//							break;
//							case 1: 
//								led.tager(&led,L_CAN);
//								modbus.set_coil(&modbus,pacckr[i].id,pacckr[i].arr[0]);
//							break;	
//							case 2:

//							break;
//							case 3:

//							break;
//						}
//					}
//						break;
//					case 0xD1: { 
//						switch(pacckr[i].cmd) {
//								case 0: 
//								
//								break;
//								case 1: 
//									led.tager(&led,L_CAN);
//									modbus.set_input(&modbus,pacckr[i].id,pacckr[i].arr[0]);
//								break;	
//								case 2:

//								break;
//								case 3:

//								break;
//							}
//					}
//						break;
//				}
//				pacckr[i].flag = F_NO_USE;
//			}
//		}
//		
//		for(int adr = 0;adr < 100;adr++) {
//			DELAY_mS(1);
//			modbus_coil_obj coil = modbus.up_coil(&modbus,adr);
//			if(coil.id != 0xffff) {
//				can_bus.send_msg.send_id = coil.id;	 
//				can_bus.send_msg.id = can_bus.id;
//				can_bus.send_msg.device_id = 0xf0;	 
//				can_bus.send_msg.cmd = 0x01;	
//				can_bus.send_msg.len = 1;			
//				can_bus.send_msg.arr[0] = coil.val;
//				can_bus.send(&can_bus);
//				DELAY_mS(1);
//			} 
//		}
//	}
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

extern uint8_t tcp_server_databuf[200];   	//发送数据缓存	  
extern uint8_t tcp_server_sta;				//服务端状态
uint8_t tcp_server_tsta=0XFF;
extern uint16_t pack_len;

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
				tcp_server_sta&=~(1<<6);		//标记数据已经被处理
			}
			tcp_server_tsta=tcp_server_sta;
		}
    }
}

void ubasic_task(void *p){
    while(1) {
//		ubasic_init(ReadBuffer);
//		do {
//			ubasic_run();
//		} while(!ubasic_finished());
		DELAY_mS(500);
		led.tager(&led,L_RUN);
    }
}

void delay() {
	for(int i = 0;i < 200;i++) 
		for(int j = 0;j < 0xffff;j++);
}

int main(void) {
	delay();

	usart.init(&usart,115200);
	led.init(&led);
	rtc.init(&rtc);
	//test();
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
	xTaskCreate(ubasic_task, (const char*)"ubasic_task", 512, NULL, 4, &xhande_task_basic);
	xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
	vTaskStartScheduler();
}

