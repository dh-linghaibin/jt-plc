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

//FILE __stdout;   

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

typedef int FILEHANDLE;
#pragma import(__use_no_semihosting_swi)
#pragma import(_main_redirection)
const char __stdin_name[150];
const char __stdout_name[150];
const char __stderr_name[150];


FILEHANDLE _sys_open(const char *name, int openmode) {
	return 0;
}

int _sys_close(FILEHANDLE fh) {
	return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode) {
	return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode) {
	return 0;
}

int _sys_istty(FILEHANDLE fh) {
	return 0;
}

int _sys_seek(FILEHANDLE fh, long pos) {
	return 0;
}

int _sys_ensure(FILEHANDLE fh) {
	return 0;
}

long _sys_flen(FILEHANDLE fh) {
	return 0;
}

void _sys_exit(int status) {

}

int _sys_tmpnam(char *name, int fileno, unsigned maxlength) {
	return 0;
}

void _ttywrch(int ch) {

}

int remove(const char *filename) {	
	return 0;
}

char *_sys_command_string(char *cmd, int len) {
	return NULL;
}

int time(int *t) {  
    return 0;  
}

unsigned int clock(void) {
	return 0;
}

int system(const char* a) {
	return 0;
}

int rename(const char *a, const char *b) {
	return 0;
}

