/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "can.h"
#include <cstring>
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"


#define GPIO_CAN                   GPIOA
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA
#define GPIO_Pin_RX                GPIO_Pin_11
#define GPIO_Pin_TX                GPIO_Pin_12
#define GPIO_Remap_CAN             GPIO_Remap1_CAN1

uint32_t ext_id =0x1800f001;  

void bxcan_init(struct _can_obj* can) {
	 NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;   
    GPIO_InitTypeDef GPIO_InitStruct;
    // ���ȴ򿪵�Դ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    // Ȼ������pin
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
 

	CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    
    CAN_InitStructure.CAN_TTCM=DISABLE;	  //MCR-TTCM  ʱ�䴥��ͨ��ģʽʹ��
    CAN_InitStructure.CAN_ABOM=DISABLE;	  //MCR-ABOM  �Զ����߹��� 
    CAN_InitStructure.CAN_AWUM=DISABLE;	  //MCR-AWUM  �Զ�����ģʽ
    CAN_InitStructure.CAN_NART=DISABLE;	  //MCR-NART  ��ֹ�����Զ��ش�	 0-�Զ��ش�   1-����ֻ��һ��
    CAN_InitStructure.CAN_RFLM=DISABLE;	  //MCR-RFLM  ����FIFO ����ģʽ  0-���ʱ�±��ĻḲ��ԭ�б���  1-���ʱ���±��Ķ���
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	  //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;	  //BTR-TS1 ʱ���1 ռ����2��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;	  //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_Prescaler =72;
    if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)           
    {  
        return;                                                       
    }  
    
    CAN_FilterInitStructure.CAN_FilterNumber=0;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
    
    //�������μĴ��������ﵱ��ʶ���Ĵ�����  
    CAN_FilterInitStructure.CAN_FilterIdHigh=(can->id<<5) ;  //Ϊʲô����5λ����������ͬ�������ﲻ���ظ�����  
    CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�,CAN_FilterIdLow��IDλ�����������ã��ڴ�ģʽ�²�����Ч��  
    
    //���ñ�ʶ���Ĵ���  
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�  
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�  
    
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //�˹����������������FIFO0  
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //����˹�������  
    
    CAN_FilterInit(&CAN_FilterInitStructure); 

    CAN_ITConfig(CAN1, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);  // fifo0�ж�
    CAN_ITConfig(CAN1, CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1, ENABLE);  // fifo1�ж�
    CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);                // �����ж�
}

void bxcan_send(struct _can_obj* can) {
	CanTxMsg transmit_message;
	 /* initialize transmit message */
	transmit_message.StdId=can->send_msg.send_id;
	transmit_message.RTR=CAN_RTR_DATA;
	transmit_message.IDE=CAN_ID_STD;
	transmit_message.Data[0] = can->send_msg.id;
	
	uint8_t send_len = can->send_msg.len+4; /* ���͵ĳ��� */
	uint8_t byte[68];
	byte[0] = 0x3a;
	byte[1] = can->send_msg.device_id;
	byte[2] = can->send_msg.len;
	byte[3] = can->send_msg.cmd;
	for(int i = 4;i < send_len;i++) {
		byte[i] = can->send_msg.arr[i-4];
	}	
	uint8_t qj_j = 0;
	do {
		if(send_len < 7) {
			transmit_message.DLC = send_len+1;
			for(int i = 0;i < send_len;i++) {
				transmit_message.Data[i+1] = byte[qj_j++];
			}
			send_len = 0;
		} else {
			transmit_message.DLC = 8;
			for(int i = 0;i < 7;i++) {
				transmit_message.Data[i+1] = byte[qj_j++];
			}
			send_len -= 7;
		}
		uint8_t TransmitMailbox = CAN_Transmit(CAN1, &transmit_message);
		uint32_t timeout = 0xFFFF;
		while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (timeout != 0xFFFFFF)) {
			timeout++;
		}
	} while(send_len > 0);
}

void bxcan_send_s(struct _can_obj* can) {
	CanTxMsg transmit_message;
	 /* initialize transmit message */
	transmit_message.StdId=can->send_msg.send_id;
	transmit_message.RTR=CAN_RTR_DATA;
	transmit_message.IDE=CAN_ID_STD;
	transmit_message.Data[0] = can->send_msg.id;
	
	uint8_t send_len = can->send_msg.len+4; /* ���͵ĳ��� */
	uint8_t byte[68];
	byte[0] = 0x3a;
	byte[1] = can->send_msg.device_id;
	byte[2] = can->send_msg.len;
	byte[3] = can->send_msg.cmd;
	for(int i = 4;i < send_len;i++) {
		byte[i] = can->send_msg.arr[i-4];
	}	
	uint8_t qj_j = 0;
	do {
		if(send_len < 7) {
			transmit_message.DLC = send_len+1;
			for(int i = 0;i < send_len;i++) {
				transmit_message.Data[i+1] = byte[qj_j++];
			}
			send_len = 0;
		} else {
			transmit_message.DLC = 8;
			for(int i = 0;i < 7;i++) {
				transmit_message.Data[i+1] = byte[qj_j++];
			}
			send_len -= 7;
		}
		uint8_t TransmitMailbox = CAN_Transmit(CAN1, &transmit_message);
		uint32_t timeout = 0xFFFF;
		while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (timeout != 0xFFFFFF)) {
			timeout++;
		}
	} while(send_len > 0);
}

void bxcan_set_id(struct _can_obj* can,uint8_t id) {
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	can->id = id;

	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;

	//�������μĴ��������ﵱ��ʶ���Ĵ�����  
	CAN_FilterInitStructure.CAN_FilterIdHigh=(can->id<<5) ;  //Ϊʲô����5λ����������ͬ�������ﲻ���ظ�����  
	CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�,CAN_FilterIdLow��IDλ�����������ã��ڴ�ģʽ�²�����Ч��  

	//���ñ�ʶ���Ĵ���  
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�  
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�  

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //�˹����������������FIFO0  
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //����˹�������  

	CAN_FilterInit(&CAN_FilterInitStructure); 
}

static can_package_obj can_rx_package = {
	.package = {0,},
};

can_package_obj*  bxcan_get_packget(struct _can_obj* can) {
	return &can_rx_package;
}

/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USB_LP_CAN1_RX0_IRQHandler(void) {
	UBaseType_t uxSavedInterruptStatus;
	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

	/* check the receive message */
	CanRxMsg receive_message;
	if(SET == CAN_GetITStatus(CAN1,CAN_IT_FF0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    } else if(SET == CAN_GetITStatus(CAN1,CAN_IT_FOV0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    } else {
        CAN_Receive(CAN1, CAN_FIFO0, &receive_message);
		for(int i = 0;i < PACKAGE_NUM;i++) {
			if(can_rx_package.package[i].flag == F_NO_USE) {
				memcpy(can_rx_package.package[i].dat,receive_message.Data,8);
				can_rx_package.package[i].flag  = F_USE;
				break;
			}
		}
    }

	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );  
}



