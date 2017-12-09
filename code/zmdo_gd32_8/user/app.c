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


FlagStatus can0_receive_flag;
FlagStatus can0_error_flag;
can_parameter_struct can_init_parameter;
can_filter_parameter_struct can_filter_parameter;
can_trasnmit_message_struct transmit_message;
can_receive_message_struct receive_message;

static void Tm1650DelayMs(uint16_t ms) {						
    uint16_t i;
    while(ms--) {
        for(i=0;i<1125;i++);//2M crystal cycle 1us, i = 140; just 1ms, when 16M, i = 1125
    }
}

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

	 /* configure GPIO */
    can_gpio_config();
	/* configure NVIC */
    nvic_config();
    /* initialize CAN and filter */
    can_config(can_init_parameter, can_filter_parameter);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);
	

	 /* initialize transmit message */
    transmit_message.tx_sfid = 0x321;
    transmit_message.tx_efid = 0x01;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = 8;

	transmit_message.tx_data[0] = 100;
	
	while(1) {
		/* transmit message */
		can_message_transmit(CAN0, &transmit_message);
		Tm1650DelayMs(1000);
	}
	//return 0;
}

/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CAN0_RX0_IRQHandler(void)
{
    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
    
    if((0x300>>1 == receive_message.rx_sfid)&&(CAN_FF_STANDARD == receive_message.rx_ff)&&(2 == receive_message.rx_dlen)){
        can0_receive_flag = SET; 
    }else{
        can0_error_flag = SET; 
    }
}
/*!
    \brief      this function handles CAN1 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CAN1_RX0_IRQHandler(void)
{
    /* check the receive message */
    can_message_receive(CAN1, CAN_FIFO0, &receive_message);
}

