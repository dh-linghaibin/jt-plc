/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "can.h"
#include "tm1650.h"
#include "outsignal.h"
#include "flash.h"
#include "wdog.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

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

void flash_task(void *p);
void net_task(void *p);
void display_task(void *p);
void display_flash_task(void *p);

int main(void) {
    OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n); /* 继电器初始化 */
    TM1650.init(&TM1650.tm1650_n); 			/* 显示初始化 */
    bxcan_init();							/* can总线初始化 */
    uint32_t addr = 0;						/* 读取地址 */
    flash_read(C_FLAG,&addr);
    if(addr != 0x5555) {					/* 判断是否第一次上电 */
        addr = 0x5555; 
        flash_write(C_FLAG,addr);
        addr = 1;
        flash_write(C_ADDR,addr);
        addr = 0x00;
        flash_write(C_DEVICE_VAL,addr);
    }
    flash_read(C_ADDR,&addr);
    if(addr > 100) { 						/* 保护地址合法范围 */
        addr = 1;
    }
    TM1650.show_nex(&TM1650.tm1650_n,0,addr/10);/* 显示设备地址 */
    TM1650.show_nex(&TM1650.tm1650_n,1,addr%10);
    bxcan_set_id(addr);				/* 设置设备ID */
    if(OUTSIGNAL.readstop(&OUTSIGNAL.outsignal_n) == 1) {
        uint8_t coil_val = 0;
        flash_read(C_DEVICE_VAL,&addr);
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

    BaseType_t xReturn = xTaskCreate(flash_task, (const char*)"flash_task", 300, NULL, 4, NULL);
    if (xReturn != pdPASS) {
        xReturn = pdPASS;
    }
    xReturn = xTaskCreate(net_task, (const char*)"net_task", 300, NULL, 4, NULL);
    if (xReturn != pdPASS) {
        xReturn = pdPASS;
    }
    xReturn = xTaskCreate(display_task, (const char*)"display_task", 300, NULL, 4, NULL);
    if (xReturn != pdPASS) {
        xReturn = pdPASS;
    }
    /* Start the scheduler. */
    vTaskStartScheduler();
    return 0;
}

void flash_task(void *p) {
	while(1) {
		switch(bxcan_get_state()) {
			case LS_ACK_1 : {
				TM1650.show_led(&TM1650.tm1650_n,10,0);
				TM1650.show_led(&TM1650.tm1650_n,9,0);
				vTaskDelay(100/portTICK_RATE_MS);
				TM1650.show_led(&TM1650.tm1650_n,9,1);
				vTaskDelay(100/portTICK_RATE_MS);
			} break;
			case LS_ACK_2 : {
				TM1650.show_led(&TM1650.tm1650_n,9,0);
				vTaskDelay(500/portTICK_RATE_MS);
				TM1650.show_led(&TM1650.tm1650_n,9,1);
				vTaskDelay(500/portTICK_RATE_MS);
			} break;
			case LS_ACK_OK: {
				TM1650.show_led(&TM1650.tm1650_n,10,0);
				vTaskDelay(500/portTICK_RATE_MS);
				TM1650.show_led(&TM1650.tm1650_n,10,1);
				vTaskDelay(500/portTICK_RATE_MS);
			} break;
			default :{
				vTaskDelay(500/portTICK_RATE_MS);
			} break;
		}
	}
}


void bxcan_rx_callback(can_packr_obj *pacckr) {
    switch(pacckr->device_id) {
        case 0xD0: { /* 八位数字输出 */
            switch(pacckr->cmd) {
                case 0xac:
                    
                break;
                case 1: {
                    uint8_t dat = pacckr->arr[0];
                    for(int i = 0;i < 8;i++) {
                        OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,i,(dat&0x01)<<7); 
                        TM1650.show_led(&TM1650.tm1650_n,i,(dat&0x01)<<7);
                        dat>>=1;
                    }
                } break;	
                case 2:

                break;
                case 3:

                break;
            }
        }
        break;
    }
    vTaskDelay(1/portTICK_RATE_MS); /* 休息一下 */
    /* 保存设备继电器信息 */
    {
    //uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
    //flash.write(C_DEVICE_VAL,val);
    }
    /*更新设备情况*/
    can_message_obj send_msg;
    send_msg.send_id = 0xff;	  /* 目标设备地址 */
    send_msg.id = bxcan_get_id(); /* 设备地址 */
    send_msg.device_id = 0xd0;	  /* 设备类型 */
    send_msg.cmd = 0x01;		  /* 命令 */
    send_msg.len = 1;			
    send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
    bxcan_send(send_msg);
}

void net_task(void *p) {
	bxcan_set_rx_callback(bxcan_rx_callback);
	while(1) {
		vTaskDelay(5/portTICK_RATE_MS);
		bxcan_lb_poll();
	}
}

typedef struct _menu_val {
	uint8_t menu_flag;
	uint8_t menu_addr;
	uint8_t flash;
} menu_val;

void display_task(void *p) {
   uint8_t but_key;
	menu_val menu;
	menu.menu_addr = 0;
	menu.menu_flag = 0;
	menu.flash = 0;
	while(1) {
		vTaskDelay(50/portTICK_RATE_MS);
		but_key = TM1650.readkey(&TM1650.tm1650_n);
		if(but_key == 0x67) {
			TM1650.tm1650_n.key_down_num = 0;
		} else if(but_key == 0x5f) {
			TM1650.tm1650_n.key_down_num = 1;
		} else if(but_key == 0x57) {
			TM1650.tm1650_n.key_down_num = 2;
		} else if(but_key == 0x6f) {
			TM1650.tm1650_n.key_down_num = 3;
		} else if(but_key == 0x47) {
			TM1650.tm1650_n.key_down_num = 4;
		} else if(but_key == 0x4f) {
			TM1650.tm1650_n.key_down_num = 5;
		} else if(but_key == 0x77) {	
			TM1650.tm1650_n.key_down_num = 6;
		} else if(but_key == 0x76) {
			TM1650.tm1650_n.key_down_num = 7;
		} else if(but_key == 0x66) {
			TM1650.tm1650_n.key_down_num = 9;
		} else if(but_key == 0x5e) {
			TM1650.tm1650_n.key_down_num = 10;
		} else if(but_key == 0x56) {
			TM1650.tm1650_n.key_down_num = 8;
		} else {
			if(TM1650.tm1650_n.key_down_num == 9) {
				if( (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] >= 1) &&
				   (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 34) ){
						switch(menu.menu_flag) {
						case 0:

						break;
						case 1:
						//ltm1650_task.lcd_out_num = 0;
						if(menu.menu_addr < 99) {
							menu.menu_addr++;
						} else {
							menu.menu_addr = 0;
						}
						menu.flash = 0;
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
					//uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
					//flash.write(C_DEVICE_VAL,val);
				}
				/*更新设备情况*/
				can_message_obj send_msg;
				send_msg.send_id = 0xff;	  /* 目标设备地址 */
				send_msg.id = bxcan_get_id(); /* 设备地址 */
				send_msg.device_id = 0xd0;	  /* 设备类型 */
				send_msg.cmd = 0x01;		  /* 命令 */
				send_msg.len = 1;			
				send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
				bxcan_send(send_msg);
			}
		} else if(TM1650.tm1650_n.key_down_num == 9) {
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 25)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 24) {
				switch(menu.menu_flag) {
					case 0:
					menu.menu_flag = 1;
					menu.menu_addr = bxcan_get_id();
					BaseType_t xReturn = xTaskCreate(display_flash_task, (const char*)"tled_task", 300, &menu, 4, NULL);
					if (xReturn != pdPASS) {
						xReturn = pdPASS;
					}
					break;
					case 1:
					menu.flash = 30;
					bxcan_set_id(menu.menu_addr);	/* 设置设备ID */
					TM1650.show_nex(&TM1650.tm1650_n,0,bxcan_get_id()/10);
					TM1650.show_nex(&TM1650.tm1650_n,1,bxcan_get_id()%10);
					flash_write(C_ADDR,bxcan_get_id());
					break;
				}
			}
		} else if(TM1650.tm1650_n.key_down_num == 10) {
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 10)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 1) {
				switch(menu.menu_flag) {
					case 0:
					
					break;
					case 1:
					//ltm1650_task.lcd_out_num = 0;
					if(menu.menu_addr > 0) {
						menu.menu_addr--;
					} else {
						menu.menu_addr = 99;
					}
					menu.flash = 0;
					break;
				}
			}
		}
	}
}

void display_flash_task(void *p) {
	uint8_t dr = 0;
	menu_val *menu = (menu_val *)(p);
	while(1) {
		vTaskDelay(250/portTICK_RATE_MS);
		if(dr == 0) {
			dr = 1;
			TM1650.show_nex(&TM1650.tm1650_n,0,13);
			TM1650.show_nex(&TM1650.tm1650_n,1,13);
		} else {
			dr = 0;
			TM1650.show_nex(&TM1650.tm1650_n,0,menu->menu_addr/10);
			TM1650.show_nex(&TM1650.tm1650_n,1,menu->menu_addr%10);
		}
		if(menu->flash < 30) {
			menu->flash++;
		} else {
			menu->menu_flag = 0;
			menu->flash = 0;
			TM1650.show_nex(&TM1650.tm1650_n,0,bxcan_get_id()/10);
			TM1650.show_nex(&TM1650.tm1650_n,1,bxcan_get_id()%10);
			vTaskDelete(NULL);
		}
	}
}




