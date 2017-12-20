/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "can.h"

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
    // 首先打开电源及时钟
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
    // 然后配置pin
    
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
    
    CAN_InitStructure.CAN_TTCM=DISABLE;	  //MCR-TTCM  时间触发通信模式使能
    CAN_InitStructure.CAN_ABOM=DISABLE;	  //MCR-ABOM  自动离线管理 
    CAN_InitStructure.CAN_AWUM=DISABLE;	  //MCR-AWUM  自动唤醒模式
    CAN_InitStructure.CAN_NART=DISABLE;	  //MCR-NART  禁止报文自动重传	 0-自动重传   1-报文只传一次
    CAN_InitStructure.CAN_RFLM=DISABLE;	  //MCR-RFLM  接收FIFO 锁定模式  0-溢出时新报文会覆盖原有报文  1-溢出时，新报文丢弃
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	  //BTR-SJW 重新同步跳跃宽度 1个时间单元
    CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;	  //BTR-TS1 时间段1 占用了2个时间单元
    CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;	  //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler =72;
    if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)           
    {  
        return;                                                       
    }  
    
    CAN_FilterInitStructure.CAN_FilterNumber=0;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
    
    //设置屏蔽寄存器，这里当标识符寄存器用  
    CAN_FilterInitStructure.CAN_FilterIdHigh=(can->id<<5) ;  //为什么左移5位？与上面相同道理，这里不再重复解释  
    CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //设置标识符寄存器低字节,CAN_FilterIdLow的ID位可以随意设置，在此模式下不会有效。  
    
    //设置标识符寄存器  
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((ext_id<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节  
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节  
    
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //此过滤器组关联到接收FIFO0  
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活此过滤器组  
    
    CAN_FilterInit(&CAN_FilterInitStructure); 

    CAN_ITConfig(CAN1, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);  // fifo0中断
    CAN_ITConfig(CAN1, CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1, ENABLE);  // fifo1中断
    CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);                // 发送中断
}

void bxcan_send(struct _can_obj* can) {
	CanTxMsg transmit_message;
	 /* initialize transmit message */
	transmit_message.StdId=can->send_msg.send_id;
	transmit_message.RTR=CAN_RTR_DATA;
	transmit_message.IDE=CAN_ID_STD;
	transmit_message.Data[0] = can->send_msg.id;
	
	uint8_t send_len = can->send_msg.len+4; /* 发送的长度 */
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

	//设置屏蔽寄存器，这里当标识符寄存器用  
	CAN_FilterInitStructure.CAN_FilterIdHigh=(can->id<<5) ;  //为什么左移5位？与上面相同道理，这里不再重复解释  
	CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //设置标识符寄存器低字节,CAN_FilterIdLow的ID位可以随意设置，在此模式下不会有效。  

	//设置标识符寄存器  
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((ext_id<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节  
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节  

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //此过滤器组关联到接收FIFO0  
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活此过滤器组  

	CAN_FilterInit(&CAN_FilterInitStructure); 
}

static uint8_t bxcan_package_cmd( struct _can_package_obj* pack,uint8_t num);

static can_package_obj can_rx_package = {
	{0,},
	&bxcan_package_cmd,
};

can_package_obj*  bxcan_get_packget(struct _can_obj* can) {
	return &can_rx_package;
}

static uint8_t bxcan_package_cmd( struct _can_package_obj* pack,uint8_t num) {
	return pack->package[num][P_CMD];
}
/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USB_LP_CAN1_RX0_IRQHandler(void) {
    /* check the receive message */
	CanRxMsg receive_message;
	if(SET == CAN_GetITStatus(CAN1,CAN_IT_FF0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    } else if(SET == CAN_GetITStatus(CAN1,CAN_IT_FOV0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    } else {
        CAN_Receive(CAN1, CAN_FIFO0, &receive_message);
        uint8_t can_rx_flag = 0;/* 没有包的标志 */
		for(int i = 0;i < PACKAGE_NUM;i++) {
			if(can_rx_package.package[i][0] > 0) {
				if(can_rx_package.package[i][1] == receive_message.Data[0]) { /* 判断地址是否相同 */
					uint8_t pack_len = can_rx_package.package[i][4]/7;
					if(can_rx_package.package[i][3]%7 != 0) {
						pack_len += 1;
					}
					if(can_rx_package.package[i][0] < pack_len) { /* 一个包的最大数据长度 */
						uint8_t pack_num = 1 + can_rx_package.package[i][0] * 8;
						for(int j = 0;j < 8;j++) { /* 打包 */
							can_rx_package.package[i][pack_num+j] = receive_message.Data[j];
						}
						can_rx_package.package[i][0]++;
						if(can_rx_package.package[i][0] >= pack_len) {
							/* 打包完成 */
							can_rx_package.package[i][0] = 0xff;
						}
					} else {
						/* 打包完成 */
						can_rx_package.package[i][0] = 0xff;
					}
					can_rx_flag = 1;
					break;
				}
			}
		}
		if(0 == can_rx_flag) {
			if(receive_message.Data[1] == 0x3a) { /* 头帧保护 */
				for(int i = 0;i < PACKAGE_NUM;i++) {
					if(can_rx_package.package[i][0] == 0) {
						for(int j = 0;j < 8;j++) { /* 打包 */
							can_rx_package.package[i][1+j] = receive_message.Data[j];
						}
						if(can_rx_package.package[i][4] <= 4) {
							/* 打包完成 */
							can_rx_package.package[i][0] = 0xff;
						} else {
							can_rx_package.package[i][0]++;
						}
						break;
					}
				}
			}
		}
    }  
}



