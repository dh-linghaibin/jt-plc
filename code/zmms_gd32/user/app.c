/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "ff.h"
#include "led.h"
#include "fsm.h"
#include "can.h"
#include "time.h"
#include "modbus.h"
#include "ubasic.h"
#include "usart.h"
#include "stdio.h"

static time_obj time = {
	&time_init,
	&time_get_1ms,
	&time_set_1ms,
};

static usart_obj usart = {
	.init = usart_init,
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
	.up_coil 			  = modbus_up_coil,
};

static led_obj led = {
	.init  = led_init,
	.set   = led_set,
	.tager = led_tager,
};

static can_obj can_bus = {
	0xff,
	{0,0,0,0,0,0,0,0},
	{0},
	&bxcan_init,
	&bxcan_send,
	&bxcan_set_id,
	&bxcan_get_packget,
};

simple_fsm(can_rx_task,)
fsm_init_name(can_rx_task)
	while(1) {
		WaitX(100);
		can_package_obj *pack = can_bus.get_packget(&can_bus);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			if(pack->package[i][0] == 0xff) {
				if(pack->package[i][P_ADDR] == 0X3A) {
					switch(pack->package[i][3]) {
						case 0xD0: {
							switch(pack->get_cmd(pack,i)) {
								case 0: 
								
								break;
								case 1: 
									led.tager(&led,L_RUN);
									modbus.set_coil(&modbus,(pack->package[i][1]*8),pack->package[i][P_O0]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+1),pack->package[i][P_O1]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+2),pack->package[i][P_O2]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+3),pack->package[i][P_O3]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+4),pack->package[i][P_O4]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+5),pack->package[i][P_O5]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+6),pack->package[i][P_O6]);
									modbus.set_coil(&modbus,(pack->package[i][1]*8+7),pack->package[i][P_O7]);
								break;	
								case 2:

								break;
								case 3:

								break;
							}
						}
							break;
						case 0xD1: {
							switch(pack->get_cmd(pack,i)) {
								case 0: 
								
								break;
								case 1: 
									led.tager(&led,L_RUN);
								break;	
								case 2:

								break;
								case 3:

								break;
							}
						}	
							break;
					}
					pack->package[i][0] = 0x00;
					WaitX(1);
				} else {
					pack->package[i][0] = 0x00;
				}
			}
		}
		modbus_coil_obj coil = modbus.up_coil(&modbus);
		if(coil.id != 0xffff) {
			can_bus.send_msg.send_id = coil.id;	 
			can_bus.send_msg.id = can_bus.id;
			can_bus.send_msg.device_id = 0xf0;	 
			can_bus.send_msg.cmd = 0x01;	
			can_bus.send_msg.len = 8;			
			uint8_t coil_val = coil.val;
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
	}
fsm_end

simple_fsm(modbus_task,
uint16_t time_count;)
fsm_init_name(modbus_task)
	while(1) {
		WaitX(1);
		modbus.loop(&modbus);
		if(me.time_count < 400) {
			me.time_count++;
		} else {
			me.time_count = 0;
			modbus.heart(&modbus);
		}
	}
fsm_end

static const char program[] =
"10 gosub 100\n\
//20 for i = 1 to 10\n\
//30 print i\n\
//40 next i\n\
//50 print \"end\"\n\
//60 end\n\
//100 print \"subroutine\"\n\
//101 if 10 > 0 then print 1000\n\
110 return\n";

int main(void) {
	time.init(&time);
	usart.init(&usart,115200);
	led.init(&led);
	led.tager(&led,L_RUN);
	can_bus.init(&can_bus);
	modbus.init(&modbus);
	fsm_task_on(can_rx_task);
	fsm_task_on(modbus_task);

//	ubasic_init(program);
//	do {
//		ubasic_run();
//	} while(!ubasic_finished());

	while(1) {
		if(time.get_1ms(&time) == 1) {
			time.set_1ms(&time,0);
			fsm_going(can_rx_task);
			fsm_going(modbus_task);
		}
	}
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1) {
  }
}

