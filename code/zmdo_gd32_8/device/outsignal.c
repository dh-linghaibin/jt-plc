/* 引用头文件 *****************************************************************/
#include "outsignal.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
#define ADC1_DR_Address    0x40012440
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
static void OutsignalClock(uint32_t port){
	if (port == GPIOA)
        rcu_periph_clock_enable(RCU_GPIOA);
    else if(port == GPIOB)
        rcu_periph_clock_enable(RCU_GPIOB);
    else if(port == GPIOC)
		rcu_periph_clock_enable(RCU_GPIOC);
    else if(port == GPIOD)
        rcu_periph_clock_enable(RCU_GPIOD);
    else if(port == GPIOE)
        rcu_periph_clock_enable(RCU_GPIOE);
    else if(port == GPIOF)
        rcu_periph_clock_enable(RCU_GPIOF);
}

void OutsignedInit(Stdoutsignal_n* outsignal) {   
    OutsignalClock(outsignal->DO1.port);
    OutsignalClock(outsignal->DO2.port);
    OutsignalClock(outsignal->DO3.port);
    OutsignalClock(outsignal->DO4.port);
    OutsignalClock(outsignal->DO5.port);
    OutsignalClock(outsignal->DO6.port);
    OutsignalClock(outsignal->DO7.port);
    OutsignalClock(outsignal->DO8.port);
    
    OutsignalClock(outsignal->urgent.port);

	gpio_init(outsignal->DO1.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO1.pin);
	gpio_init(outsignal->DO2.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO2.pin);
	gpio_init(outsignal->DO3.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO3.pin);
	gpio_init(outsignal->DO4.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO4.pin);
	gpio_init(outsignal->DO5.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO5.pin);
	gpio_init(outsignal->DO6.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO6.pin);
	gpio_init(outsignal->DO7.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO7.pin);
	gpio_init(outsignal->DO8.port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,outsignal->DO8.pin);

	gpio_init(outsignal->urgent.port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,outsignal->urgent.pin);
}

void OutsignalSetout(Stdoutsignal_n* outsignal,uint8_t num,uint8_t var) {
    Stdoutsignal_pin_TypeDef out_pin;
    switch(num) {
        case 0:
            out_pin = outsignal->DO1;
        break;
        case 1:
            out_pin = outsignal->DO2;
        break;
        case 2:
            out_pin = outsignal->DO3;
        break;
        case 3:
            out_pin = outsignal->DO4;
        break;
//        case 4:
//            out_pin = outsignal->DO5;
//        break;
//        case 5:
//            out_pin = outsignal->DO6;
//        break;
//        case 6:
//            out_pin = outsignal->DO7;
//        break;
//        case 7:
//            out_pin = outsignal->DO8;
//        break;
        case 4:
            outsignal->all_pin_var = var;
            out_pin = outsignal->DO1;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO2;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO3;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO4;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO5;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO6;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO7;
            gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
            out_pin = outsignal->DO8;
            break;
    }
    if(num < 8) {
        if(var == 0) {
            outsignal->coil_val &= ~(1 << num);
        } else {
            outsignal->coil_val |= (1 << num);
        }
    } else {
        if(var == 0) {
            outsignal->coil_val = 0x00;
        } else {
            outsignal->coil_val = 0xff;
        }
    }
    gpio_bit_write(out_pin.port,out_pin.pin,(FlagStatus)(var));
}

uint8_t OutsignalReadout(Stdoutsignal_n* outsignal,uint8_t num) {
    Stdoutsignal_pin_TypeDef out_pin;
    switch(num) {
        case 0:
            out_pin = outsignal->DO1;
        break;
        case 1:
            out_pin = outsignal->DO2;
        break;
        case 2:
            out_pin = outsignal->DO3;
        break;
        case 3:
            out_pin = outsignal->DO4;
        break;
//        case 4:
//            out_pin = outsignal->DO5;
//        break;
//        case 5:
//            out_pin = outsignal->DO6;
//        break;
//        case 6:
//            out_pin = outsignal->DO7;
//        break;
//        case 7:
//            out_pin = outsignal->DO8;
//        break;
        case 4:
            return outsignal->all_pin_var;
       // break;
    }
    return gpio_output_bit_get(out_pin.port,out_pin.pin);
}

uint8_t OutsignalEmergencyStop(Stdoutsignal_n* outsignal) {
    return gpio_input_bit_get(outsignal->urgent.port,outsignal->urgent.pin);
}

/***************************************************************END OF FILE****/
