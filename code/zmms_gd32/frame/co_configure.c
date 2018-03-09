/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2018 linghaibin
 *
 */

#include "co_configure.h"

//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "fsm.h"

#define COIL_BESAC_ADR 0

extern uint8_t modbus_coil_r[100];
extern uint8_t modbus_input[100];

void write_coil_adr(uint16_t address,uint8_t val) {
	uint8_t addr;
	uint8_t num;
	addr = address/8;
	num = address%8;
	if(val == 0) {
		modbus_coil_r[addr] &= ~(1 << num);
	} else {
		modbus_coil_r[addr] |= (1 << num);
	}
}

uint16_t m_reg[300];

coc_mode get_mode(void) {
	return m_reg[COIL_BESAC_ADR + 1];
}

uint16_t get_m_group(uint8_t num) {
	return m_reg[COIL_BESAC_ADR + (6+num)];
}

uint16_t get_t_group(uint8_t num) {
	return m_reg[COIL_BESAC_ADR + (8+num)];
}

uint16_t get_l_group(uint8_t num) {
	return m_reg[COIL_BESAC_ADR + (10+num)];
}

uint16_t get_f_group(uint8_t num) {
	return m_reg[COIL_BESAC_ADR + (12+num)];
}

uint16_t get_b_group(uint8_t b_num,uint8_t num) {
	return m_reg[COIL_BESAC_ADR + (14+(b_num*2)+num)];
}

uint16_t get_gropu_adr(uint8_t num) {
	return m_reg[COIL_BESAC_ADR + 18+(31*num)];
}

uint16_t get_gropu_time_adr(uint8_t group,uint8_t num) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+num];
}

uint16_t get_gropu_grade_adr(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+6];
}

uint16_t get_gropu_delay_start_adr(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+7];
}

uint16_t get_gropu_delay_cen_adr(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+8];
}

uint16_t get_gropu_l_open_adr(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+9];
}

uint16_t get_gropu_l_open_close(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+10];
}

uint16_t get_gropu_coil_num(uint8_t group) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+11];
}

uint16_t get_gropu_coil(uint8_t group,uint8_t num) {
	return m_reg[COIL_BESAC_ADR + 18+(30*group)+12+num];
}

typedef struct _co_c_adrv {
	uint8_t val;
	uint16_t adr;
} co_c_adrv;

co_c_adrv get_coil_adr(uint16_t val) {
	co_c_adrv adr;
	adr.adr = val & 0x0fff;
	adr.val = val >> 12;
}

void co_configure_loop(void) {
	switch(get_mode()) {
		case F_CONTRAL: { /* 手动控制 */
//			for(int i = 0;i < 2;i++) {
//				if(get_m_group(i) != 3) {
//					DELAY_mS(get_gropu_delay_start_adr(i)); /* 打开延时 */
//					for(int j = 0;j < get_gropu_coil_num(i);j++) {
//						co_c_adrv adr = get_coil_adr(get_gropu_coil(i,j)); /* 解析地址 */
//						write_coil_adr(adr.adr,adr.val); /* 控制 */
//					}
//				}
//			}
		} break;
		case M_CONTRAL: { /* 消防控制 */
			
		} break;
		case T_CONTRAL: { /* 时间控制 */
			
		} break;
		case L_CONTRAL: { /* 光控 */
			
		} break;
	}
}


