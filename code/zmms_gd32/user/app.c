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


static modbus_obj modbus = {
    .ipv4_ip                = {192,168,1,250},
    .gateway_ip             = {192,168,1,1},
    .mask_ip                = {255,255,255,0},
    .uip_mac.addr           = {0,},
    .enc28.mac              = {0x05,0x01,0x35,0x01,0x00,0x01},
    .enc28.init             = en28j60_init,
    .enc28.packet_receive   = en28j60_packet_receive,
    .enc28.packet_send      = en28j60_packet_send,
    .enc28.set_mac          = en28j60_set_mac,
    .init                   = modbus_init,
    .loop                   = modbus_loop,
    .heart                  = modbus_heart,
    .set_coil               = modbus_set_coil,
    .set_input              = modbus_set_input,
    .up_coil                = modbus_up_coil,
};


void bxcan_rx_callback(can_packr_obj *pacckr) {
    switch(pacckr->device_id) {
        case 0xD0: { 
            switch(pacckr->cmd) {
                case 0: 

                break;
                case 1: 
                    if(pacckr->id > 99) {
                        led_tager(L_CAN);
                    } else {
                        printf("di %d  %d \n",pacckr->id,pacckr->arr[0]);
                        //modbus.set_coil(&modbus,pacckr->id,pacckr->arr[0]);
                    }
                break;    
                case 2:

                break;
            }
        } break;
        case 0xD1: { 
            switch(pacckr->cmd) {
                case 0: 

                break;
                case 1: 
                    led_tager(L_CAN);
                    modbus.set_input(&modbus,pacckr->id,pacckr->arr[0]);
                break;    
                case 2:

                break;
                case 3:

                break;
            }
        } break;
    }
}

void can_task(void *p){
    bxcan_set_rx_callback(bxcan_rx_callback);
    while(1) {
        vTaskDelay(5/portTICK_RATE_MS);
        bxcan_lb_poll();
//        for(int adr = 0;adr < 100;adr++) {
//            DELAY_mS(1);
//            modbus_coil_obj coil = modbus.up_coil(&modbus,adr);
//            if(coil.id != 0xffff) {
//                can_bus.send_msg.send_id = coil.id;     
//                can_bus.send_msg.id = can_bus.id;
//                can_bus.send_msg.device_id = 0xf0;     
//                can_bus.send_msg.cmd = 0x01;    
//                can_bus.send_msg.len = 1;            
//                can_bus.send_msg.arr[0] = coil.val;
//                can_bus.send(&can_bus);
//                DELAY_mS(1);
//            } 
//        }

//        rs485_packet_obj* rs485_packet = rs485.get_packet(&rs485);
//        for(int i = 0;i < RS485_PACKAGE_NUM;i++) {
//            if(rs485_packet->package[i].flag == RF_USE) {
//                modbus_Holding[99] = rs485_packet->package[i].back_number*8 + rs485_packet->package[i].number;
//                rs485_packet->package[i].flag = RF_NO_USE;
//                led.tager(&led,L_RS485);
//            }
//        }
//        wdog.reload(&wdog);
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
        rtc_t time = rtc_read();
    }
}

void delay() {
    for(int i = 0;i < 200;i++) 
        for(int j = 0;j < 0xffff;j++);
}

int main(void) {
    //delay();
    usart_init(115200);
    printf("system start \n");

    rs485_init();
    led_init();
    rtc_init();
    bxcan_init();

    only_id_obj only_id;
    only_id_get_id(&only_id);
    /* mac ID */
    modbus.enc28.mac[3] = only_id.id[0];
    modbus.enc28.mac[4] = only_id.id[1];
    modbus.enc28.mac[5] = only_id.id[2];
    modbus.init(&modbus);
    uip_listen(HTONS(1200));
    //wdog.init(&wdog);
    //xTaskCreate(modbus_task, (const char*)"modbus_task", 512, NULL, 4, NULL);
    xTaskCreate(can_task, (const char*)"can_task", 512, NULL, 4, NULL);
    //xTaskCreate(ubasic_task, (const char*)"ubasic_task", 512, NULL, 4, &xhande_task_basic);
    //xTaskCreate(can_up_task, (const char*)"can_up_task", 1024, NULL, 4, NULL);
    xTaskCreate(time_task, (const char*)"time_task", 512, NULL, 4, NULL);
    //over_time_create();
    vTaskStartScheduler();
}

