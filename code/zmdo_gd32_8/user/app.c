/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

//#include "timer.h"
#include "can.h"
//#include "sys.h"
#include "tm1650.h"
#include "outsignal.h"
#include "flash.h"
//#include "menu.h"
#include "wdog.h"
#include "fsm.h"
#include "time.h"

Stdoutsignal OUTSIGNAL = {
	{
		{GPIOA, GPIO_PIN_0},
		{GPIOA, GPIO_PIN_1},
		{GPIOA, GPIO_PIN_2},
		{GPIOA, GPIO_PIN_3},
		{GPIOA, GPIO_PIN_4},
		{GPIOA, GPIO_PIN_5},
		{GPIOA, GPIO_PIN_6},
		{GPIOA, GPIO_PIN_7},
		
		{GPIOB, GPIO_PIN_0},
		0,
		0,
		0,
	},
	&OutsignedInit,
	&OutsignalSetout,
	&OutsignalReadout,
	&OutsignalEmergencyStop,
};
can_obj can_bus = {
	
		4,
		0x1800f001,
		{0,0,0,0,0,0,0,0},
		{0},
	&bxcan_init,
	&bxcan_send,
	&bxcan_set_id,
};
Stdtm1650 TM1650 = {
	{
		{GPIOB,GPIO_PIN_10},
		{GPIOB,GPIO_PIN_11},
		{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,
		0x07, 0x7F, 0x6F, 0x76, 0x40,0x79, 0x00},
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		{0x00,0x00,0x00,0x00},
	},
	&Tm1650Init,
	&Tm1650ShowNex,
	&Tm1650ShowLed,
	&TM1650ScanKey,
};
time_obj time = {
	&time_init,
	&time_get_1ms,
	&time_set_1ms,
};

simple_fsm(LedTask,	)
fsm_init_name(LedTask)
	while(1) {
		TM1650.show_led(&TM1650.tm1650_n,1,0);
		WaitX(200);
		TM1650.show_led(&TM1650.tm1650_n,1,1);
		WaitX(200);
		can_bus.send_msg.send_id = 0xff;
		can_bus.send_msg.id = 5;
		can_bus.send_msg.device_id = 0x01;
		can_bus.send_msg.cmd = 0xD0;
		can_bus.send_msg.len = 64;
		for(int i = 0;i < 64;i++) {
			can_bus.send_msg.arr[i] = 5+i;
		}
		can_bus.send(&can_bus);
	}
fsm_end

simple_fsm(but_task,	)
fsm_init_name(but_task)
	while(1) {
		WaitX(10);
		switch(TM1650.readkey(&TM1650.tm1650_n)) {
			case 0xe6:
			TM1650.tm1650_n.key_down_num = 0;
			break;
			case 0xfa:
			TM1650.tm1650_n.key_down_num = 1;
			break;
			case 0xea:
			TM1650.tm1650_n.key_down_num = 2;
			break;
			case 0xf6:
			TM1650.tm1650_n.key_down_num = 3;
			break;
			case 0xe2:
			TM1650.tm1650_n.key_down_num = 4;
			break;
			case 0xf2:
			TM1650.tm1650_n.key_down_num = 5;
			break;
			case 0xee:
			TM1650.tm1650_n.key_down_num = 6;
			break;
			case 0x6e:
			TM1650.tm1650_n.key_down_num = 7;
			break;
			case 0x66:
			TM1650.tm1650_n.key_down_num = 9;
			break;
			case 0x7a:
			TM1650.tm1650_n.key_down_num = 10;
			break;
			case 0x6a:
			TM1650.tm1650_n.key_down_num = 8;
			break;
			default:
			
//			if(TM1650.tm1650_n.key_down_num == 9) {
//				if( (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] >= 1) &&
//				   (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 34) ){
//					   switch(MENU.getmenu(&MENU.menu_n)) {
//						   case 0:
//						   
//						   break;
//						   case 1:
//						   if(MENU.getvar(&MENU.menu_n) < 99) {
//							   MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)+1);
//						   } else {
//							   MENU.setvar(&MENU.menu_n,0);
//						   }
//						   break;
//					   }
//				   }
//			}
			TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] = 0;
			TM1650.tm1650_n.key_down_num = 20;
			break;
		}
		if(TM1650.tm1650_n.key_down_num <= 8) { 
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 10)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 1) {
				uint8_t var = 0;
				var = (OUTSIGNAL.readout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num) == 0)?1:0;
				OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num,var);
				TM1650.show_led(&TM1650.tm1650_n,TM1650.tm1650_n.key_down_num,var); 
				
				{
					uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
				  //  SFLASH.write(&val, FLASH_ADDR_FLAG(2), 1);
				}
			}
		} 
	}
fsm_end

int main(void) {
    TM1650.init(&TM1650.tm1650_n);
	TM1650.show_nex(&TM1650.tm1650_n,0,2);
	TM1650.show_nex(&TM1650.tm1650_n,1,3);
	OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n);
	OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,0,0);
	can_bus.init(&can_bus);
	time.init(&time);
	
	fsm_task_on(LedTask);
	fsm_task_on(but_task);
	while(1) {
		if(time.get_1ms(&time) == 1) {
			time.set_1ms(&time,0);
			fsm_going(LedTask);
			fsm_going(but_task);
		}
	}
	//return 0;
}



