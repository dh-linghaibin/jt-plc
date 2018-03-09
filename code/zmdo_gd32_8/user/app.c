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
#include "fsm.h"
#include "time.h"

static time_obj time = {
	.init	= time_init,
	.get_1ms	= time_get_1ms,
	.set_1ms	= time_set_1ms,
};

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


//static can_obj can_bus = {
//	
//		4,
//		0x1800f001,
//		{0,0,0,0,0,0,0,0},
//		{0},
//	&bxcan_init,
//	&bxcan_send,
//	&bxcan_set_id,
//	&bxcan_get_packget,
//};

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

static uint8_t ws_flag = 0;

simple_fsm(LedTask,
	)
fsm_init_name(LedTask)
	while(1) {
		if(ws_flag == 0) {
			TM1650.show_led(&TM1650.tm1650_n,9,0);
			WaitX(2000);
			TM1650.show_led(&TM1650.tm1650_n,9,1);
			WaitX(2000);
		} else {
			TM1650.show_led(&TM1650.tm1650_n,10,0);
			WaitX(2000);
			TM1650.show_led(&TM1650.tm1650_n,10,1);
			WaitX(2000);
		}
		switch(ws_flag) {
			case 0: {
				can_message_obj send_msg;
				send_msg.send_id = 0xff;	  /* Ŀ���豸��ַ */
				send_msg.id = bxcan_get_id(); /* �豸��ַ */
				send_msg.device_id = 0xd0;	  /* �豸���� */
				send_msg.cmd = 0xf1;		  /* ���� */
				send_msg.len = 0;			
				bxcan_send(send_msg);
			} break;
			case 1: {

			} break;
		}
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
				/* �����豸�̵�����Ϣ */
				{
					//uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
					//flash.write(C_DEVICE_VAL,val);
				}
				/*�����豸���*/
				can_message_obj send_msg;
				send_msg.send_id = 0xff;	  /* Ŀ���豸��ַ */
				send_msg.id = bxcan_get_id(); /* �豸��ַ */
				send_msg.device_id = 0xd0;	  /* �豸���� */
				send_msg.cmd = 0x01;		  /* ���� */
				send_msg.len = 1;			
				send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
				bxcan_send(send_msg);
			}
		} else if(TM1650.tm1650_n.key_down_num == 9) {
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 25)
				TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
			if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 24) {
				switch(me.menu_flag) {
					case 0:
					me.menu_flag = 1;
					me.menu_addr = bxcan_get_id();
					fsm_task_init(tm1650_task);
					fsm_task_on(tm1650_task);
					break;
					case 1:
					bxcan_set_id(me.menu_addr);	/* �����豸ID */
					TM1650.show_nex(&TM1650.tm1650_n,0,bxcan_get_id()/10);
					TM1650.show_nex(&TM1650.tm1650_n,1,bxcan_get_id()%10);
					flash_write(C_ADDR,bxcan_get_id());
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
			TM1650.show_nex(&TM1650.tm1650_n,0,bxcan_get_id()/10);
			TM1650.show_nex(&TM1650.tm1650_n,1,bxcan_get_id()%10);
			fsm_task_off(tm1650_task);
		}
	}
fsm_end



simple_fsm(can_rx_task,)
fsm_init_name(can_rx_task)
	while(1) {
		WaitX(100);
		bxcan_lb_poll();
		/* �������� */
		can_packr_obj *pacckr = bxcan_lb_get_msg();
		for(int i = 0;i < PACKAGE_NUM;i++) { 
			if(pacckr[i].flag == F_PACK_OK) {
				switch(pacckr[i].cmd) {
					case 0xf1: {
						ws_flag = 1;
					} break;
					default: {
							switch(pacckr[i].device_id) {
								case 0xf0: { /* ��λ������� */
									switch(pacckr[i].cmd) {
										case 0:
										
										break;
										case 1: {
											uint8_t dat = pacckr[i].arr[0];
											for(int i = 0;i < 8;i++) {
												OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,i,(dat&0x01)<<7); 
												TM1650.show_led(&TM1650.tm1650_n,i,(dat&0x01)<<7);
												 dat>>=1;
											}
										}
											break;	
										case 2:

										break;
										case 3:

										break;
									}
								}
									break;
							}
							//WaitX(1); /* ��Ϣһ�� */
							/* �����豸�̵�����Ϣ */
							{
								//uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
								//flash.write(C_DEVICE_VAL,val);
							}
							/*�����豸���*/
							can_message_obj send_msg;
							send_msg.send_id = 0xff;	  /* Ŀ���豸��ַ */
							send_msg.id = bxcan_get_id(); /* �豸��ַ */
							send_msg.device_id = 0xd0;	  /* �豸���� */
							send_msg.cmd = 0x01;		  /* ���� */
							send_msg.len = 1;			
							send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
							bxcan_send(send_msg);
					} break;
				}
				pacckr[i].flag = F_NO_USE;/* ���ݴ������ */
			}
		}
	}
fsm_end

int main(void) {
	OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n); /* �̵�����ʼ�� */
	TM1650.init(&TM1650.tm1650_n); 			/* ��ʾ��ʼ�� */
	time.init(&time); 						/* ϵͳ���ĳ�ʼ�� */
	bxcan_init();				/* can���߳�ʼ�� */
	uint32_t addr = 0;						/* ��ȡ��ַ */
	flash_read(C_FLAG,&addr);
	if(addr != 0x5555) {					/* �ж��Ƿ��һ���ϵ� */
		addr = 0x5555; 
		flash_write(C_FLAG,addr);
		addr = 99;
		flash_write(C_ADDR,addr);
		addr = 0x00;
		flash_write(C_DEVICE_VAL,addr);
	}
	flash_read(C_ADDR,&addr);
	if(addr > 100) { 						/* ������ַ�Ϸ���Χ */
		addr = 99;
	}
	TM1650.show_nex(&TM1650.tm1650_n,0,addr/10);/* ��ʾ�豸��ַ */
	TM1650.show_nex(&TM1650.tm1650_n,1,addr%10);
	bxcan_set_id(addr);				/* �����豸ID */
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
	//wdog_init();
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



