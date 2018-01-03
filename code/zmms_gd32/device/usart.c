/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "usart.h"

void usart_init(struct _usart_obj* usart,uint32_t baud_rate) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ��1������GPIO��USARTʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	/* ��2������USART1 Tx@PA9��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART1 Rx@PA10��GPIO����Ϊ��������ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��4��������USART1����
	������   = 115200
	���ݳ��� = 8
	ֹͣλ   = 1
	У��λ   = No
	��ֹӲ������(����ֹRTS��CTS)
	ʹ�ܽ��պͷ���
	*/
	USART_InitStructure.USART_BaudRate = baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* ��5����ʹ�� USART1�� ������� */
	USART_Cmd(USART1, ENABLE);

	/* ���������ɱ�־��Transmission Complete flag */
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

void usart_sen_byte(struct _usart_obj* usart,uint8_t dat) {
	/* дһ���ֽڵ�USART1 */
  USART_SendData(USART1, (uint8_t) dat);
  /* �ȴ����ͽ��� */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
}


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 


FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	/* дһ���ֽڵ�USART1 */
  USART_SendData(USART1, (uint8_t) ch);
  /* �ȴ����ͽ��� */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  return ch;
}

int fgetc(FILE *f) {
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
#endif 

void rs485_init(struct _rs485_obj *re485) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ��1������GPIOA��USART2ʱ�� */
	/* ʹ��GPIOAʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	/* ʹ��USART2ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	/* ��2������USART2 Tx@PA2��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART2 Rx@PA3��GPIO����Ϊ��������ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ��4��������USART2����
	������   = 9600
	���ݳ��� = 8
	ֹͣλ   = 1
	У��λ   = No
	��ֹӲ������(����ֹRTS��CTS)
	ʹ�ܽ��պͷ���
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* ��5����ʹ�� USART2�� ������� */
	USART_Cmd(USART2, ENABLE);

	/* ���������ɱ�־��Transmission Complete flag */
	USART_ClearFlag(USART2, USART_FLAG_TC);	
}

static rs485_packet_obj rs485_packet[RS485_PACKAGE_NUM] = {0,};

void USART2_IRQHandler(void) {
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		uint8_t dat = USART_ReceiveData(USART2);//���յ�����
		static uint8_t flag = 0;
		switch(flag) {
			case 0:
				if(dat == R_HEAD) {
					flag = 1;
				}
			break;
			case 1:
				if(dat == R_HEAD) {
					flag = 1;
				}
			break;
			case 2:
				if(dat == R_HEAD) {
					flag = 1;
				}
			break;
			case 3:
				
			break;
			case 4:

			break;
			case 5:

			break;
			case 6:

			break;
			case 7:

			break;
		}
	}
}


