/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "usart.h"
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"

void usart_init(uint32_t baud_rate) {
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

void usart_sen_byte(uint8_t dat) {
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


FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
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

void rs485_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 第1步：打开GPIOA和USART2时钟 */
	/* 使能GPIOA时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	/* 使能USART2时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	/* 第2步：将USART2 Tx@PA2的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART2 Rx@PA3的GPIO配置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)0);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 第4步：配置USART2参数
	波特率   = 9600
	数据长度 = 8
	停止位   = 1
	校验位   = No
	禁止硬件流控(即禁止RTS和CTS)
	使能接收和发送
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* 第5步：使能 USART2， 配置完毕 */
	USART_Cmd(USART2, ENABLE);

	/* 清除发送完成标志，Transmission Complete flag */
	USART_ClearFlag(USART2, USART_FLAG_TC);	
}

static rs485_packet_obj rs485_packet = {0,};
static uint8_t packet_dat[2] = {0,0};

rs485_packet_obj* rs485_get_packet(void) {
	return &rs485_packet;
}

void USART2_IRQHandler(void) {
	UBaseType_t uxSavedInterruptStatus;
	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		uint8_t dat = USART_ReceiveData(USART2);//接收到数据
		static uint8_t flag = 0;
		switch(flag) {
			case 0:
				if(dat == R_HEAD) {
					flag = 1;
				} 
			break;
			case 1:
				if(dat == R_TYPE) {
					flag = 2;
				} else {
					flag = 0;
				}
			break;
			case 2:
				if(dat == R_TYPE) {
					flag = 3;
				} else {
					flag = 0;
				}
			break;
			case 3:
				if(dat == R_CMD) {
					flag = 4;
				} else {
					flag = 0;
				}
			break;
			case 4:
				switch(dat) {
					case 0x00:packet_dat[0] = 1;break;
					case 0x10:packet_dat[0] = 2;break;
					case 0x20:packet_dat[0] = 3;break;
					case 0x30:packet_dat[0] = 4;break;
					case 0x40:packet_dat[0] = 5;break;
					case 0x50:packet_dat[0] = 6;break;
					case 0x60:packet_dat[0] = 7;break;
					case 0x70:packet_dat[0] = 8;break;
				}
				flag = 5;
			break;
			case 5:
				packet_dat[1] = dat;
				flag = 6;
			break;
			case 6:
				if(dat == 0x7d) {
					flag = 7;
				} else {
					flag = 0;
				}
			break;
			case 7:
				flag = 0;
				for(int i = 0;i < RS485_PACKAGE_NUM;i++) {
					if(rs485_packet.package[i].flag == RF_NO_USE) {
						rs485_packet.package[i].back_number = packet_dat[1];
						rs485_packet.package[i].number = packet_dat[0];
						rs485_packet.package[i].flag = RF_USE;
						break;
					}
				}
			break;
		}
	}

	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );  
}


