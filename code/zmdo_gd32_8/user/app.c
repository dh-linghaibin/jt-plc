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

int main(void) {
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
	
    TM1650.init(&TM1650.tm1650_n);
	TM1650.show_nex(&TM1650.tm1650_n,0,2);
	TM1650.show_nex(&TM1650.tm1650_n,1,3);

	OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n);
	OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,0,0);
	while(1) {
		
	}
	//return 0;
}
