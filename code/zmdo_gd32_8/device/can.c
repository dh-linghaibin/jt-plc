/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "can.h"
#include <cstring>
/*!
    \brief      initialize CAN and filter
    \param[in]  can_parameter
      \arg        can_parameter_struct
    \param[in]  can_filter
      \arg        can_filter_parameter_struct
    \param[out] none
    \retval     none
*/
static void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter) {
    /* initialize CAN register */
    can_deinit(CAN0);
    
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    
    can_filter_init(&can_filter);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_config(void) {
    /* configure CAN0 NVIC */
    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn,0,1);
	//nvic_irq_enable(USBD_HP_CAN0_TX_IRQn,0,2);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void can_gpio_config(void) {
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);

	gpio_init(GPIOA,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);

	gpio_bit_write(GPIOA,GPIO_PIN_10,RESET);
}

static uint8_t can_id = 0x04;

void bxcan_init(void) {
	can_parameter_struct can_init_parameter;
	can_filter_parameter_struct can_filter_parameter;

	/* initialize CAN parameters */
    can_init_parameter.time_triggered = DISABLE;
    can_init_parameter.auto_bus_off_recovery = DISABLE;
    can_init_parameter.auto_wake_up = DISABLE;
    can_init_parameter.auto_retrans = DISABLE;
    can_init_parameter.rec_fifo_overwrite = DISABLE;
    can_init_parameter.trans_fifo_order = DISABLE;
    can_init_parameter.working_mode = CAN_NORMAL_MODE;
    can_init_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_init_parameter.time_segment_1 = CAN_BT_BS1_16TQ;
    can_init_parameter.time_segment_2 = CAN_BT_BS2_1TQ;
	can_init_parameter.prescaler = 60;
	
	/* initialize filter */ 
    can_filter_parameter.filter_number=0;
    can_filter_parameter.filter_mode = CAN_FILTERMODE_LIST;
    can_filter_parameter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter_parameter.filter_list_high = (can_id<<5);
    can_filter_parameter.filter_list_low = 0|0x00000000;
    can_filter_parameter.filter_mask_high = ((0x1800f001<<3)>>16) & 0xffff;
    can_filter_parameter.filter_mask_low = ((0x1800f001<<3)& 0xffff) | 0x00000004;
    can_filter_parameter.filter_fifo_number = CAN_FIFO0;
    can_filter_parameter.filter_enable = ENABLE;

	/* configure GPIO */
    can_gpio_config();
	/* configure NVIC */
    nvic_config();
    /* initialize CAN and filter */
    can_config(can_init_parameter, can_filter_parameter);
    /* enable can receive FIFO0 not empty interrupt */
	can_interrupt_enable(CAN0, CAN_INT_RFNE0);
	can_interrupt_enable(CAN0, CAN_INT_RFO0);
}

void bxcan_send(can_message_obj send_msg) {
	can_trasnmit_message_struct transmit_message;
	 /* initialize transmit message */
    transmit_message.tx_sfid = send_msg.send_id;
    transmit_message.tx_efid = 0x00;
	transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
	transmit_message.tx_data[0] = send_msg.id;
	
	uint8_t send_len = send_msg.len+4; /* 发送的长度 */
	uint8_t byte[68];
	byte[0] = 0x3a;
	byte[1] = send_msg.device_id;
	byte[2] = send_msg.len;
	byte[3] = send_msg.cmd;
	for(int i = 4;i < send_len;i++) {
		byte[i] = send_msg.arr[i-4];
	}	
	uint8_t qj_j = 0;
	do {
		if(send_len < 7) {
			transmit_message.tx_dlen = send_len+1;
			for(int i = 0;i < send_len;i++) {
				transmit_message.tx_data[i+1] = byte[qj_j++];
			}
			send_len = 0;
		} else {
			transmit_message.tx_dlen = 8;
			for(int i = 0;i < 7;i++) {
				transmit_message.tx_data[i+1] = byte[qj_j++];
			}
			send_len -= 7;
		}
		can_message_transmit(CAN0, &transmit_message);
		uint32_t timeout = 0xFFFF;
		while((can_transmit_states(CAN0, CAN_MAILBOX0) != CAN_TRANSMIT_OK) && (timeout != 0)){
			timeout--;
		}
//		timeout = 0xFFFF;
//		while((can_transmit_states(CAN0, CAN_MAILBOX1) != CAN_TRANSMIT_OK) && (timeout != 0)){
//			timeout--;
//		}
//		timeout = 0xFFFF;
//		while((can_transmit_states(CAN0, CAN_MAILBOX2) != CAN_TRANSMIT_OK) && (timeout != 0)){
//			timeout--;
//		}
	}while(send_len > 0);
}

void bxcan_set_id(uint8_t id) {
	can_filter_parameter_struct can_filter_parameter;
	can_id = id;
	/* initialize filter */ 
    can_filter_parameter.filter_number=0;
    can_filter_parameter.filter_mode = CAN_FILTERMODE_LIST;
    can_filter_parameter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter_parameter.filter_list_high = (can_id<<5);
    can_filter_parameter.filter_list_low = 0|0x00000000;
    can_filter_parameter.filter_mask_high = ((0x1800f001<<3)>>16) & 0xffff;
    can_filter_parameter.filter_mask_low = ((0x1800f001<3)& 0xffff) | 0x00000004;
    can_filter_parameter.filter_fifo_number = CAN_FIFO0;
    can_filter_parameter.filter_enable = ENABLE;
	can_filter_init(&can_filter_parameter);
}

uint8_t bxcan_get_id(void) {
	return can_id;
}

static can_package_obj can_rx_package = {
	{0,},
};
static can_packr_obj pacckr[PACKAGE_NUM];

can_package_obj*  bxcan_get_packget(void) {
	return &can_rx_package;
}

void bxcan_lb_poll(void) {
	can_package_obj *pack = bxcan_get_packget();
	for(int i = 0;i < PACKAGE_NUM;i++) {
		uint8_t can_rx_flag = 0;
		if(pack->package[i].flag == F_USE) { /* 获取数据 */
			for(int j = 0;j < PACKAGE_NUM;j++) {
				if(pacckr[j].flag == F_USE) { /* 判断是否使用 */
					if(pacckr[j].id == pack->package[i].dat[0]) { /* 判断ID是否相同 */
						for(int k = 0;k < 7;k++) { /* 打包 */
							pacckr[j].arr[pacckr[j].pack_bum + k] = pack->package[i].dat[1+k];
						}
						pacckr[j].pack_bum += 7;
						if(pacckr[j].pack_bum >= pacckr[j].len) { /* 判断打包是否完成 */
							pacckr[j].flag = F_PACK_OK; /* 打包完成 */
						}
						can_rx_flag = 1;
					}
					break;
				}
			}
			if(can_rx_flag == 0) {
				if(pack->package[i].dat[1] == 0x3a) { /* 判断这个一帧是不是头针 */
					for(int j = 0;j < PACKAGE_NUM;j++) {
						if(pacckr[j].flag == F_NO_USE) { /* 寻找未使用包 */
							pacckr[j].id = pack->package[i].dat[0]; /* 获取ID */
							pacckr[j].device_id = pack->package[i].dat[2];
							pacckr[j].len = pack->package[i].dat[3];
							pacckr[j].cmd = pack->package[i].dat[4];
							for(int k = 0;k < 3;k++) { /* 打包 */
								pacckr[j].arr[k] = pack->package[i].dat[5+k];
							}
							if(pacckr[j].len <= 3) { 
								pacckr[j].flag = F_PACK_OK; /* 打包完成 */
							} else {
								pacckr[j].pack_bum = 3;
								pacckr[j].flag = F_USE; /* 提示第一个包已经打包完成 */
							}
							break;
						}
					}
				}
			}
			pack->package[i].flag = F_NO_USE;//表示这个已经打包完成
		}
	}
}

can_packr_obj* bxcan_lb_get_msg(void) {
	return pacckr;
}
/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_CAN0_RX0_IRQHandler(void) {
    /* check the receive message */
	can_receive_message_struct receive_message;
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
	for(int i = 0;i < PACKAGE_NUM;i++) {
		if(can_rx_package.package[i].flag == F_NO_USE) {
			memcpy(can_rx_package.package[i].dat,receive_message.rx_data,8);
			can_rx_package.package[i].flag  = F_USE;
			break;
		}
	}
}

void USBD_HP_CAN0_TX_IRQHandler(void) {
	
}



