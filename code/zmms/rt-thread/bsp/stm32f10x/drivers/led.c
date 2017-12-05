/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include <stm32f10x.h>
#include "led.h"
// led define

#define led1_rcc                    RCC_APB2Periph_GPIOB
#define led1_gpio                   GPIOB
#define led1_pin                    (GPIO_Pin_11)

#define led2_rcc                    RCC_APB2Periph_GPIOB
#define led2_gpio                   GPIOB
#define led2_pin                    (GPIO_Pin_0)

#define led3_rcc                    RCC_APB2Periph_GPIOB
#define led3_gpio                   GPIOB
#define led3_pin                    (GPIO_Pin_10)

#define led4_rcc                    RCC_APB2Periph_GPIOB
#define led4_gpio                   GPIOB
#define led4_pin                    (GPIO_Pin_1)


void rt_hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(led1_rcc|led2_rcc|led3_rcc|led4_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led2_pin;
    GPIO_Init(led2_gpio, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = led3_pin;
    GPIO_Init(led4_gpio, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = led3_pin;
    GPIO_Init(led4_gpio, &GPIO_InitStructure);
    
    for(int i = 0;i < 4;i++) {
        rt_hw_led_on(i);
    }
}

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    case 1:
        GPIO_SetBits(led2_gpio, led2_pin);
        break;
    case 2:
        GPIO_SetBits(led3_gpio, led3_pin);
        break;
    case 3:
        GPIO_SetBits(led4_gpio, led4_pin);
        break;
    default:
        break;
    }
}

void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    case 1:
        GPIO_ResetBits(led2_gpio, led2_pin);
        break;
    case 2:
        GPIO_ResetBits(led3_gpio, led3_pin);
        break;
    case 3:
        GPIO_ResetBits(led4_gpio, led4_pin);
        break;
    default:
        break;
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;
void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 0 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(0);
            break;
        case 1:
            rt_hw_led_on(0);
            break;
        default:
            break;
        }
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }
}
FINSH_FUNCTION_EXPORT(led, set led[0 - 1] on[1] or off[0].)
#endif




//#ifdef RT_USING_SPI
//#include "rt_stm32f10x_spi.h"
//
//static int rt_hw_spi_init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    /* register spi bus */
//    {
//        static struct stm32_spi_bus stm32_spi;
//
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 				
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//        stm32_spi_register(SPI1, &stm32_spi, "spi1");
//    }
//
//    /* register spi bus */
//    {
//        static struct stm32_spi_bus stm32_spi;
//
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); 
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 				
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//        stm32_spi_register(SPI2, &stm32_spi, "spi2");
//    }
//
//    return RT_EOK;
//}
//
//
//eth_system_device_init();
//enc28j60_attach("spi1");	
//
//INIT_BOARD_EXPORT(rt_hw_spi_init);
//
//#endif /* RT_USING_SPI */












