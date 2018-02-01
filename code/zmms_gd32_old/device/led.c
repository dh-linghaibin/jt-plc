/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "led.h"

void led_init(struct _led_obj* led) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)(1));
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,(BitAction)(1));
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)(1));
	GPIO_WriteBit(GPIOB,GPIO_Pin_1,(BitAction)(1));
}

void led_set(struct _led_obj* led,led_type_e led_type ,BitAction val) {
	switch(led_type) {
		case L_RUN:
			GPIO_WriteBit(GPIOB,GPIO_Pin_11,val);
			break;
		case L_CAN:
			GPIO_WriteBit(GPIOB,GPIO_Pin_0,val);
			break;
		case L_RS485:
			GPIO_WriteBit(GPIOB,GPIO_Pin_10,val);
			break;
		case L_RS232:
			GPIO_WriteBit(GPIOB,GPIO_Pin_1,val);
			break;
	}
}

void led_tager(struct _led_obj* led,led_type_e led_type) {
	switch(led_type) {
		case L_RUN:
			GPIO_WriteBit(GPIOB, GPIO_Pin_11, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11))));
			break;
		case L_CAN:
			GPIO_WriteBit(GPIOB, GPIO_Pin_0, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0))));
			break;
		case L_RS485:
			GPIO_WriteBit(GPIOB, GPIO_Pin_10, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10))));
			break;
		case L_RS232:
			GPIO_WriteBit(GPIOB, GPIO_Pin_1, 
		               (BitAction)((1-GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1))));
			break;
	}
}
