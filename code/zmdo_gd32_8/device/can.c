/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "can.h"

/* select CAN baudrate */
/* 1MBps */
#define CAN_BAUDRATE  1000
/* 500kBps */
/* #define CAN_BAUDRATE  500 */
/* 250kBps */
/* #define CAN_BAUDRATE  250 */
/* 125kBps */
/* #define CAN_BAUDRATE  125 */
/* 100kBps */ 
/* #define CAN_BAUDRATE  100 */
/* 50kBps */ 
/* #define CAN_BAUDRATE  50 */
/* 20kBps */ 
/* #define CAN_BAUDRATE  20 */

/*!
    \brief      initialize CAN and filter
    \param[in]  can_parameter
      \arg        can_parameter_struct
    \param[in]  can_filter
      \arg        can_filter_parameter_struct
    \param[out] none
    \retval     none
*/
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
    /* initialize CAN register */
    can_deinit(CAN0);
    can_deinit(CAN1);
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    can_parameter.prescaler = 6;
    /* 500KBps */
#elif CAN_BAUDRATE == 500
    can_parameter.prescaler = 12;
    /* 250KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 24;
    /* 125KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = 48;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100
    can_parameter.prescaler = 60;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50
    can_parameter.prescaler = 120;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20
    can_parameter.prescaler = 300;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    can_init(CAN1, &can_parameter);
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x3000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x3000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    
    can_filter_init(&can_filter);
    
    /* CAN1 filter number */
    can_filter.filter_number = 15;
    can_filter_init(&can_filter);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX1_IRQn,0,0);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    gpio_pin_remap_config(GPIO_CAN_FULL_REMAP,ENABLE);

	gpio_init(GPIOA,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
}

