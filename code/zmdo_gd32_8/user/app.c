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

static Stdoutsignal OUTSIGNAL = {
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

static flash_obj flash = {
	&flash_write,
	&flash_read,
};

static can_obj can_bus = {
	
		4,
		0x1800f001,
		{0,0,0,0,0,0,0,0},
		{0},
	&bxcan_init,
	&bxcan_send,
	&bxcan_set_id,
	&bxcan_get_packget,
};

static Stdtm1650 TM1650 = {
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
static time_obj time = {
	&time_init,
	&time_get_1ms,
	&time_set_1ms,
};

simple_fsm(LedTask,
	)
fsm_init_name(LedTask)
	while(1) {
		TM1650.show_led(&TM1650.tm1650_n,10,0);
		WaitX(200);
		TM1650.show_led(&TM1650.tm1650_n,10,1);
		WaitX(200);
	}
fsm_end

simple_fsm(tm1650_task,
	uint8_t dr;
	uint8_t lcd_out_num;)

simple_fsm(menu_task,
		   uint8_t but_key;
		   uint8_t menu_flag;
		   uint8_t menu_addr;)
fsm_init_name(menu_task)
	while(1) {
		WaitX(500);
		me.but_key = TM1650.readkey(&TM1650.tm1650_n);
		if(me.but_key == 0x67) {
			TM1650.tm1650_n.key_down_num = 0;
		} else if(me.but_key == 0x5f) {
			TM1650.tm1650_n.key_down_num = 1;
		} else if(me.but_key == 0x57) {
			TM1650.tm1650_n.key_down_num = 2;
		} else if(me.but_key == 0x6f) {
			TM1650.tm1650_n.key_down_num = 3;
		} else if(me.but_key == 0x47) {
			TM1650.tm1650_n.key_down_num = 4;
		} else if(me.but_key == 0x4f) {
			TM1650.tm1650_n.key_down_num = 5;
		} else if(me.but_key == 0x77) {	
			TM1650.tm1650_n.key_down_num = 6;
		} else if(me.but_key == 0x76) {
			TM1650.tm1650_n.key_down_num = 7;
		} else if(me.but_key == 0x66) {
			TM1650.tm1650_n.key_down_num = 9;
		} else if(me.but_key == 0x5e) {
			TM1650.tm1650_n.key_down_num = 10;
		} else if(me.but_key == 0x56) {
			TM1650.tm1650_n.key_down_num = 8;
		} else {
			if(TM1650.tm1650_n.key_down_num == 9) {
				if( (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] >= 1) &&
				   (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 34) ){
						switch(me.menu_flag) {
						case 0:

						break;
						case 1:
						ltm1650_task.lcd_out_num = 0;
						if(me.menu_addr < 99) {
							me.menu_addr++;
						} else {
							me.menu_addr = 0;
						}
						break;
						}
				   }
			}
			TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] = 0;
			TM1650.tm1650_n.key_down_num = 20;
		}
		if(TM1650.tm1650_n.key_down_num <= 8) { 
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 2)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 1) {
				uint8_t var = 0;
				var = (OUTSIGNAL.readout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num) == 0)?1:0;
				OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num,var);
				TM1650.show_led(&TM1650.tm1650_n,TM1650.tm1650_n.key_down_num,var); 
				/* 保存设备继电器信息 */
				{
					uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
					flash.write(C_DEVICE_VAL,val);
				}
				/*更新设备情况*/
				can_bus.send_msg.send_id = 0xff;	  /* 目标设备地址 */
				can_bus.send_msg.id = can_bus.id; /* 设备地址 */
				can_bus.send_msg.device_id = 0xd0;	  /* 设备类型 */
				can_bus.send_msg.cmd = 0x01;		  /* 命令 */
				can_bus.send_msg.len = 8;			
				uint8_t coil_val = OUTSIGNAL.outsignal_n.coil_val;
				for(uint8_t i = 0;i < 8;i++) {
					if ((coil_val & 0x80) == 0) {
						can_bus.send_msg.arr[7-i] = 0;
					} else {
						can_bus.send_msg.arr[7-i] = 1;
					}
					coil_val <<= 1;
				}
				can_bus.send(&can_bus);
			}
		} else if(TM1650.tm1650_n.key_down_num == 9) {
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 25)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 24) {
				switch(me.menu_flag) {
					case 0:
					me.menu_flag = 1;
					me.menu_addr = can_bus.id;
					fsm_task_init(tm1650_task);
					fsm_task_on(tm1650_task);
					break;
					case 1:
					can_bus.set_id(&can_bus,me.menu_addr);	/* 设置设备ID */
					TM1650.show_nex(&TM1650.tm1650_n,0,can_bus.id/10);
					TM1650.show_nex(&TM1650.tm1650_n,1,can_bus.id%10);
					flash.write(C_ADDR,can_bus.id);
					fsm_task_off(tm1650_task);
					break;
				}
			}
		} else if(TM1650.tm1650_n.key_down_num == 10) {
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 10)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 1) {
				switch(me.menu_flag) {
					case 0:
					
					break;
					case 1:
					ltm1650_task.lcd_out_num = 0;
					if(me.menu_addr > 0) {
						me.menu_addr--;
					} else {
						me.menu_addr = 99;
					}
					break;
				}
			}
		}
	}
fsm_end

fsm_init_name(tm1650_task)
	me.lcd_out_num = 0;
	while(1) {
		WaitX(1000);
		if(me.dr == 0) {
			me.dr = 1;
			TM1650.show_nex(&TM1650.tm1650_n,0,13);
			TM1650.show_nex(&TM1650.tm1650_n,1,13);
		} else {
			me.dr = 0;
			TM1650.show_nex(&TM1650.tm1650_n,0,lmenu_task.menu_addr/10);
			TM1650.show_nex(&TM1650.tm1650_n,1,lmenu_task.menu_addr%10);
		}
		if(me.lcd_out_num < 30) {
			me.lcd_out_num++;
		} else {
			lmenu_task.menu_flag = 0;
			me.lcd_out_num = 0;
			TM1650.show_nex(&TM1650.tm1650_n,0,can_bus.id/10);
			TM1650.show_nex(&TM1650.tm1650_n,1,can_bus.id%10);
			fsm_task_off(tm1650_task);
		}
	}
fsm_end

simple_fsm(can_rx_task,)
fsm_init_name(can_rx_task)
	while(1) {
		WaitX(100);
		can_package_obj *pack = can_bus.get_packget(&can_bus);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			if(pack->package[i][0] == 0xff) {
				if(pack->package[i][P_ADDR] == 0X3A) {
					switch(pack->get_cmd(pack,i)) {
						case 0: /* 保留 */

						break;
						case 1: /* 设置继电器 */
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,0,pack->package[i][P_O0]); 
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,1,pack->package[i][P_O1]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,2,pack->package[i][P_O2]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,3,pack->package[i][P_O3]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,4,pack->package[i][P_O4]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,5,pack->package[i][P_O5]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,6,pack->package[i][P_O6]);
							OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,7,pack->package[i][P_O7]);
							TM1650.show_led(&TM1650.tm1650_n,0,pack->package[i][P_O0]);
							TM1650.show_led(&TM1650.tm1650_n,1,pack->package[i][P_O1]);
							TM1650.show_led(&TM1650.tm1650_n,2,pack->package[i][P_O2]);
							TM1650.show_led(&TM1650.tm1650_n,3,pack->package[i][P_O3]);
							TM1650.show_led(&TM1650.tm1650_n,4,pack->package[i][P_O4]);
							TM1650.show_led(&TM1650.tm1650_n,5,pack->package[i][P_O5]);
							TM1650.show_led(&TM1650.tm1650_n,6,pack->package[i][P_O6]);
							TM1650.show_led(&TM1650.tm1650_n,7,pack->package[i][P_O7]);
						break;	
						case 2:

						break;
						case 3:

						break;
					}
					pack->package[i][0] = 0x00;
					WaitX(1); /* 休息一下 */
					/* 保存设备继电器信息 */
					{
						uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
						flash.write(C_DEVICE_VAL,val);
					}
					/*更新设备情况*/
					can_bus.send_msg.send_id = 0xff;	  /* 目标设备地址 */
					can_bus.send_msg.id = can_bus.id; /* 设备地址 */
					can_bus.send_msg.device_id = 0xd0;	  /* 设备类型 */
					can_bus.send_msg.cmd = 0x01;		  /* 命令 */
					can_bus.send_msg.len = 8;			
					uint8_t coil_val = OUTSIGNAL.outsignal_n.coil_val;
					for(uint8_t i = 0;i < 8;i++) {
						if ((coil_val & 0x80) == 0) {
							can_bus.send_msg.arr[7-i] = 0;
						} else {
							can_bus.send_msg.arr[7-i] = 1;
						}
						coil_val <<= 1;
					}
					can_bus.send(&can_bus);
				} else {
					pack->package[i][0] = 0x00;
				}
			}
		}
	}
fsm_end

int main(void) {
	OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n); /* 继电器初始化 */
	TM1650.init(&TM1650.tm1650_n); 			/* 显示初始化 */
	can_bus.init(&can_bus); 				/* can总线初始化 */
	time.init(&time); 						/* 系统节拍初始化 */
	uint32_t addr = 0;						/* 读取地址 */
	flash.read(C_FLAG,&addr);
	if(addr != 0x5555) {					/* 判断是否第一次上电 */
		addr = 0x5555; 
		flash.write(C_FLAG,addr);
		addr = 99;
		flash.write(C_ADDR,addr);
		addr = 0x00;
		flash.write(C_DEVICE_VAL,addr);
	}
	flash.read(C_ADDR,&addr);
	if(addr > 100) { 						/* 保护地址合法范围 */
		addr = 99;
	}
	TM1650.show_nex(&TM1650.tm1650_n,0,addr/10);/* 显示设备地址 */
	TM1650.show_nex(&TM1650.tm1650_n,1,addr%10);
	can_bus.set_id(&can_bus,addr);				/* 设置设备ID */
    if(OUTSIGNAL.readstop(&OUTSIGNAL.outsignal_n) == 1) {
        uint8_t coil_val = 0;
        flash.read(C_DEVICE_VAL,&addr);
		OUTSIGNAL.outsignal_n.coil_val = addr;
        coil_val = OUTSIGNAL.outsignal_n.coil_val;
        for(uint8_t i = 0;i < 8;i++) {
            uint8_t val = 1;
            if ((coil_val & 0x80) == 0) {
                val = 0;
            }
            coil_val <<= 1;
            OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,7-i,val);
            TM1650.show_led(&TM1650.tm1650_n,7-i,val);
        }
    }

	fsm_task_on(LedTask);
	fsm_task_on(menu_task);
	fsm_task_on(can_rx_task);
	while(1) {
		if(time.get_1ms(&time) == 1) {
			time.set_1ms(&time,0);
			fsm_going(LedTask);
			fsm_going(menu_task);
			fsm_going(can_rx_task);
			fsm_going(tm1650_task);
		}
	}
	//return 0;
}



