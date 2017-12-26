/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "usart.h"
#include <stdio.h>

void usart_init(struct _usart_obj* usart,uint32_t baud_rate) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* µÚ1²½£º´ò¿ªGPIOºÍUSARTÊ±ÖÓ */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	/* µÚ2²½£º½«USART1 Tx@PA9µÄGPIOÅäÖÃÎªÍÆÍì¸´ÓÃÄ£Ê½ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* µÚ3²½£º½«USART1 Rx@PA10µÄGPIOÅäÖÃÎª¸¡¿ÕÊäÈëÄ£Ê½ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* µÚ4²½£ºÅäÖÃUSART1²ÎÊý
	²¨ÌØÂÊ   = 115200
	Êý¾Ý³¤¶È = 8
	Í£Ö¹Î»   = 1
	Ð£ÑéÎ»   = No
	½ûÖ¹Ó²¼þÁ÷¿Ø(¼´½ûÖ¹RTSºÍCTS)
	Ê¹ÄÜ½ÓÊÕºÍ·¢ËÍ
	*/
	USART_InitStructure.USART_BaudRate = baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* µÚ5²½£ºÊ¹ÄÜ USART1£¬ ÅäÖÃÍê±Ï */
	USART_Cmd(USART1, ENABLE);

	/* Çå³ý·¢ËÍÍê³É±êÖ¾£¬Transmission Complete flag */
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */ 
    /* standard output using printf() for debugging, no file handling */ 
    /* is required. */ 
};
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout; 
int _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f) {
  /* Ð´Ò»¸ö×Ö½Úµ½USART1 */
  USART_SendData(USART1, (uint8_t) ch);
  /* µÈ´ý·¢ËÍ½áÊø */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  return ch;
}

int fgetc(FILE *f) {
	/* µÈ´ý´®¿Ú1ÊäÈëÊý¾Ý */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}