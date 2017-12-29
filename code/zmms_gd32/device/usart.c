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

	/* 第1步：打开GPIO和USART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	/* 第2步：将USART1 Tx@PA9的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART1 Rx@PA10的GPIO配置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第4步：配置USART1参数
	波特率   = 115200
	数据长度 = 8
	停止位   = 1
	校验位   = No
	禁止硬件流控(即禁止RTS和CTS)
	使能接收和发送
	*/
	USART_InitStructure.USART_BaudRate = baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* 第5步：使能 USART1， 配置完毕 */
	USART_Cmd(USART1, ENABLE);

	/* 清除发送完成标志，Transmission Complete flag */
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

void usart_sen_byte(struct _usart_obj* usart,uint8_t dat) {
	/* 写一个字节到USART1 */
  USART_SendData(USART1, (uint8_t) dat);
  /* 等待发送结束 */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
}


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

//FILE __stdout;   

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	/* 写一个字节到USART1 */
  USART_SendData(USART1, (uint8_t) ch);
  /* 等待发送结束 */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  return ch;
}

int fgetc(FILE *f) {
	/* 等待串口1输入数据 */
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

