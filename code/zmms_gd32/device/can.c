/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "can.h"
#include <stdio.h>
#include <cstring>
//FreeRTOS Define
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define BXPLL_TEST 0

#define GPIO_CAN                   GPIOA
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA
#define GPIO_Pin_RX                GPIO_Pin_11
#define GPIO_Pin_TX                GPIO_Pin_12
#define GPIO_Remap_CAN             GPIO_Remap1_CAN1

static void over_time_create(void); /* 定时通知设备 */
static uint8_t bxcan_id = 0xff;
static can_packr_obj pacckr[PACKAGE_NUM];
static can_device_obj device[100]; /* 设备状态 */
static TimerHandle_t th_over_time; /* heart线程 */
static void (*bxcan_rx_callback)(can_packr_obj *pacckr) = NULL;
static can_package_obj can_rx_package = {
    .package = {0,},
};

void bxcan_init(void) {
    for(int i = 10; i < 99; i++) {
        #if BXPLL_TEST 
        device[i].state = LS_ACK_OK;
        #else
        device[i].state = LS_ACK_1;
        #endif
    }

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

    /* CAN RX interrupt */
    NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);     

    /* CAN TX interrupt */
    NVIC_InitStructure.NVIC_IRQChannel=USB_HP_CAN1_TX_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
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
    //    CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;	  //BTR-TS1 时间段1 占用了2个时间单元
    //    CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;	  //BTR-TS1 时间段2 占用了3个时间单元
    //    CAN_InitStructure.CAN_Prescaler =72;
    CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;	  //BTR-TS1 时间段1 占用了2个时间单元
    CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;	  //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler =48;
    if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)           
    {  
        return;                                                       
    }  

    CAN_FilterInitStructure.CAN_FilterNumber=0;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;

    //设置屏蔽寄存器，这里当标识符寄存器用  
    CAN_FilterInitStructure.CAN_FilterIdHigh=(0xff<<5) ;  //为什么左移5位？与上面相同道理，这里不再重复解释  
    CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //设置标识符寄存器低字节,CAN_FilterIdLow的ID位可以随意设置，在此模式下不会有效。  

    //设置标识符寄存器  
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((0xf001<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节  
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=((0xf001<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节  

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //此过滤器组关联到接收FIFO0  
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活此过滤器组  

    CAN_FilterInit(&CAN_FilterInitStructure); 

    /* CAN FIFO0 message pending interrupt enable */ 
    CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    CAN_ClearITPendingBit(CAN1,CAN_IT_FF1);
    CAN_ClearITPendingBit(CAN1,CAN_IT_FOV1);
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP1);
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP2);
    CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); 

    over_time_create();
}

static uint8_t send_flag = 0;

void bxcan_send(can_message_obj send_msg) {
    CanTxMsg transmit_message;
    /* initialize transmit message */
    transmit_message.StdId=send_msg.send_id;
    transmit_message.RTR=CAN_RTR_DATA;
    transmit_message.IDE=CAN_ID_STD;
    transmit_message.Data[0] = send_msg.id;

    uint8_t send_len = send_msg.len+4; /* 发送的长度 */
    uint8_t byte[68];
    byte[0] = 0x3a;
    byte[1] = send_msg.device_id;
    byte[2] = send_msg.len;
    byte[3] = send_msg.cmd;
    for(int i = 4;i < send_len;i++) {
        byte[i] = send_msg.arr[i-4];
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
        send_flag = 0;
        uint8_t TransmitMailbox = CAN_Transmit(CAN1, &transmit_message);
        CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE); 
        while(send_flag == 0) {
            uint32_t val = CAN1->TSR&0x00000008;
            if ( val == 0x00000008 ) {
                bxcan_init();
                if(val == 0) {}
                    send_flag = 1;
            }
        }
    } while(send_len > 0);
}

void bxcan_set_id(uint8_t id) {
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    bxcan_id = id;

    CAN_FilterInitStructure.CAN_FilterNumber=0;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;

    //设置屏蔽寄存器，这里当标识符寄存器用  
    CAN_FilterInitStructure.CAN_FilterIdHigh=(bxcan_id<<5) ;  //为什么左移5位？与上面相同道理，这里不再重复解释  
    CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //设置标识符寄存器低字节,CAN_FilterIdLow的ID位可以随意设置，在此模式下不会有效。  

    //设置标识符寄存器  
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((0xf001<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节  
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=((0xf001<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节  

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //此过滤器组关联到接收FIFO0  
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活此过滤器组  

    CAN_FilterInit(&CAN_FilterInitStructure); 
}

void bxcan_set_rx_callback(void (*rx_callback)(can_packr_obj *pacckr)) {
    bxcan_rx_callback = rx_callback;
}

static void over_time_callback(xTimerHandle pxTimer) {
    uint32_t ulTimerID;
    configASSERT(pxTimer);
    /* 获取那个定时器时间到 */
    ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
    /* 处理定时器0任务 */
    if(ulTimerID == 0) {
        can_message_obj send_msg;
        send_msg.send_id = 0xfe;
        send_msg.id = 0xff;
        send_msg.device_id = 0xf0;
        send_msg.cmd = 0xac;/* 返回握手信号 */
        send_msg.len = 0;
        bxcan_send(send_msg);
        for(int i = 0; i < 100; i++) {
            if(device[i].state == LS_ACK_OK) {
                if(device[i].over_time < 8) {
                    device[i].over_time++;
                } else {
                    device[i].over_time = 0;
                    device[i].state = LS_ACK_1;
                }
            }
        }
    }
}

static void over_time_create(void) {
    const TickType_t  over_time_prt = 800;  /* 2s */
    th_over_time = xTimerCreate("over_time",          /* 定时器名字 */
                                over_time_prt,    /* 定时器周期,单位时钟节拍 */
                                pdTRUE,          /* 周期性 */
                                (void *) 0,      /* 定时器ID */
                                over_time_callback); /* 定时器回调函数 */

    if(th_over_time == NULL) {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    } else {
        /* 启动定时器，系统启动后才开始工作 */
        if(xTimerStart(th_over_time, over_time_prt) != pdPASS) {
            /* 定时器还没有进入激活状态 */
        }
    }
}
void bxcan_lb_poll(void) {
    for(int i = 0;i < PACKAGE_NUM;i++) {
        uint8_t can_rx_flag = 0;
        if(can_rx_package.package[i].flag == F_USE) { /* 获取数据 */
            for(int j = 0;j < PACKAGE_NUM;j++) {
                if(pacckr[j].flag == F_USE) { /* 判断是否使用 */
                    if(pacckr[j].id == can_rx_package.package[i].dat[0]) { /* 判断ID是否相同 */
                        for(int k = 0;k < 7;k++) { /* 打包 */
                            pacckr[j].arr[pacckr[j].pack_bum + k] = can_rx_package.package[i].dat[1+k];
                        }
                        pacckr[j].pack_bum += 7;
                        if(pacckr[j].pack_bum >= pacckr[j].len) { /* 判断打包是否完成 */
                            pacckr[j].flag = F_PACK_OK; /* 打包完成 */
                        }
                        can_rx_flag = 1;
                    }
                    break;
                }
            }
            if(can_rx_flag == 0) {
                if(can_rx_package.package[i].dat[1] == 0x3a) { /* 判断这个一帧是不是头针 */
                    for(int j = 0;j < PACKAGE_NUM;j++) {
                        if(pacckr[j].flag == F_NO_USE) { /* 寻找未使用包 */
                            pacckr[j].id = can_rx_package.package[i].dat[0]; /* 获取ID */
                            pacckr[j].device_id = can_rx_package.package[i].dat[2];
                            pacckr[j].len = can_rx_package.package[i].dat[3];
                            pacckr[j].cmd = can_rx_package.package[i].dat[4];
                            for(int k = 0;k < 3;k++) { /* 打包 */
                                pacckr[j].arr[k] = can_rx_package.package[i].dat[5+k];
                            }
                            if(pacckr[j].len <= 3) { 
                                pacckr[j].flag = F_PACK_OK; /* 打包完成 */
                            } else {
                                pacckr[j].pack_bum = 3;
                                pacckr[j].flag = F_USE; /* 提示第一个包已经打包完成 */
                            }
                            break;
                        }
                    }
                }
            }
            can_rx_package.package[i].flag = F_NO_USE;//表示这个已经打包完成
        }
    }
    for(int i = 0;i < PACKAGE_NUM;i++) { 
        if(pacckr[i].flag == F_PACK_OK) {
            printf("cmd %x \n",pacckr[i].cmd);
            switch(pacckr[i].cmd) {
                case 0xf0: { /* 握手 */
                    uint8_t id = pacckr[i].id;
                    if(id <= 99) {
                        switch(device[id].state) {
                            case LS_ACK_1: {
                                    can_message_obj send_msg;
                                    send_msg.send_id = id;
                                    send_msg.id = 0xff;
                                    send_msg.device_id = 0xf0;
                                    send_msg.cmd = 0xf1;/* 返回握手信号 */
                                    send_msg.len = 1;
                                    send_msg.arr[0] = LS_ACK_1;
                                    bxcan_send(send_msg);
                                    device[id].state = LS_ACK_2;
                            } break;
                            case LS_ACK_2: {
                                if( pacckr[i].arr[0] == LS_ACK_2) {
                                    device[id].over_time = 0;
                                    device[id].state = LS_ACK_OK;
                                } else {
                                    uint8_t id = pacckr[i].id;
                                    device[id].over_time = 0;
                                    device[id].state = LS_ACK_1;
                                }
                            } break;
                            case LS_ACK_OK: {
                                device[id].over_time = 0;
                                device[id].state = LS_ACK_1;
                            } break;
                        }
                    }
                } break;
                default: {
                    if(pacckr[i].id <= 99) {
                        device[pacckr[i].id].over_time = 0;
                    }
                    #if BXPLL_TEST 
                    for(int i = 10; i < 99; i++) {
                        device[i].over_time = 0;
                        device[i].state = LS_ACK_OK;
                    }
                    #endif
                    if(device[pacckr[i].id].state == F_PACK_OK) {
                        if(bxcan_rx_callback != NULL) {
                            bxcan_rx_callback(&(pacckr[i]));
                        }
                    }
                } break;
            }
            pacckr[i].flag = F_NO_USE;
        }
    }
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
        for(int i = 0;i < PACKAGE_NUM;i++) {
            if(can_rx_package.package[i].flag == F_NO_USE) {
                memcpy(can_rx_package.package[i].dat,receive_message.Data,8);
//                for(int j = 0;j < 8;j++) {
//                    can_rx_package.package[i].dat[j] = receive_message.Data[j];
//                }
                can_rx_package.package[i].flag  = F_USE;
                break;
            }
        }
    }
    CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP1);
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP2);
    CAN_ITConfig(CAN1,CAN_IT_TME, DISABLE);
    send_flag = 1;
}

