/* ����ͷ�ļ� *****************************************************************/
#include "stm32f10x.h"
#include "bsp_can.h"
#include "ringbuf.h"
/* �ⲿ�������� ***************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �ⲿ�������� ***************************************************************/
#define GPIO_CAN                   GPIOA
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA
#define GPIO_Pin_RX                GPIO_Pin_11
#define GPIO_Pin_TX                GPIO_Pin_12
#define GPIO_Remap_CAN             GPIO_Remap1_CAN1

uint16_t std_id =254;  
uint32_t ext_id =0x1800f001;  

uint8_t can_package_merry[800] = {0};
/** -- ���ζ��г�ʼ�� -- by lhb_steven -- 2017/6/30**/
Stdringbuf RINGBUF = {
    {
        0
    },
    &RingbufInit,
    &RingbufPut,
    &RingbufGut
};

void CAN_config_init(void)
{
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    RINGBUF.init(&RINGBUF.ringbuf_n, can_package_merry, 800);
    
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
    CAN_FilterInitStructure.CAN_FilterIdHigh=(std_id<<5) ;  //Ϊʲô����5λ����������ͬ�������ﲻ���ظ�����  
    CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�,CAN_FilterIdLow��IDλ�����������ã��ڴ�ģʽ�²�����Ч��  
    
    //���ñ�ʶ���Ĵ���  
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�  
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�  
    
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  //�˹����������������FIFO0  
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //����˹�������  
    
//    CAN_FilterInitStructure.CAN_FilterIdHigh=0;
//    CAN_FilterInitStructure.CAN_FilterIdLow= 0;
//    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;
//    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;
//    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
//    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure); 
}

void CAN_init(void)
{
    
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
    // ӳ����
   // GPIO_PinRemapConfig(GPIO_Remap_CAN , ENABLE);
    // �����ʹ�������ʼ��
    CAN_config_init();
    CAN_ITConfig(CAN1, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);  // fifo0�ж�
    CAN_ITConfig(CAN1, CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1, ENABLE);  // fifo1�ж�
    CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);                // �����ж�
}

void CAN_tx_data(CanTxMsg TxMessage)
{
    //CanTxMsg TxMessage;
    uint8_t TransmitMailbox = 0;
    u32 i;
    TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
    i = 0;
    while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFFFFFF)) {
        i++;
    }
}


void USB_LP_CAN1_RX0_IRQHandler(void) {
    CanRxMsg RxMessage;
    if(SET == CAN_GetITStatus(CAN1,CAN_IT_FF0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    } else if(SET == CAN_GetITStatus(CAN1,CAN_IT_FOV0)) {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    } else {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        for(uint8_t i = 0;i < 8;i++) {
        RINGBUF.put(&RINGBUF.ringbuf_n,RxMessage.Data[i]);
    }
    }  
}



