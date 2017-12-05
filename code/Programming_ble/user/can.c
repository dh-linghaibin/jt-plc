/* ����ͷ�ļ� *****************************************************************/
#include "can.h"
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
/* ˽�ж����� *****************************************************************/
#define HAL_REMAP_PA11_PA12     (SYSCFG_CFGR1_PA11_PA12_RMP) 
#define __HAL_REMAP_PIN_ENABLE(__PIN_REMAP__) \
do {assert_param(IS_HAL_REMAP_PIN((__PIN_REMAP__))); \
    SYSCFG->CFGR1 |= (__PIN_REMAP__);       \
}while(0)
/* ˽�б��� *******************************************************************/
/* ȫ�ֱ��� */
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/
static void CanClock(GPIO_TypeDef* port){
    if (port == GPIOA)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    else if(port == GPIOB)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    else if(port == GPIOC)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    else if(port == GPIOD)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    else if(port == GPIOE)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    else if(port == GPIOF)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
}

void CanInit(Stdcan_n* can)
{
    GPIO_InitTypeDef                    GPIO_InitStructure;
    CAN_InitTypeDef                     CAN_InitStructure;
    CAN_FilterInitTypeDef               CAN_FilterInitStructure;
    NVIC_InitTypeDef                    NVIC_InitStructure;
    
    CanClock(can->s.port);
    GPIO_InitStructure.GPIO_Pin = can->s.pin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(can->s.port, &GPIO_InitStructure);
    GPIO_ResetBits(can->s.port,can->s.pin);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    __HAL_REMAP_PIN_ENABLE(HAL_REMAP_PA11_PA12);
    
    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* Enable CAN clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
    
    /* Connect the involved CAN pins to AF */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);
    
    /* Configure CAN RX and TX pins */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //CAN�ж�ʹ�ܺ����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;         
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //CAN��Ԫ����
    CAN_InitStructure.CAN_TTCM = DISABLE; 
    CAN_InitStructure.CAN_ABOM = DISABLE;  
    CAN_InitStructure.CAN_AWUM = DISABLE; 
    CAN_InitStructure.CAN_NART = ENABLE; 
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_Normal; 
    //CAN����������,500KHz@48MHz
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
    CAN_InitStructure.CAN_Prescaler = 160;
    CAN_Init(CAN, &CAN_InitStructure);
    
    //CAN������FIFO0����
    CAN_FilterInitStructure.CAN_FilterNumber = 0;//���ù�������0����ΧΪ0~13
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; //���ù�������0Ϊ����ģʽ  CAN_FilterMode_IdMask CAN_FilterMode_IdList
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//���ù�������0λ��Ϊ32λ
    //��ʶλ�Ĵ���������
    //ext_id<<3���룬����ͼ9����>>16ȡ��16λ
    CAN_FilterInitStructure.CAN_FilterIdHigh = (can->id<<5) ;  //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterIdLow = 0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((can->ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((can->ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    //ʹ��CAN�ж�
    CAN_ClearITPendingBit(CAN, CAN_IT_FF0);
    CAN_ITConfig(CAN, CAN_IT_TME, DISABLE);
    CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN, CAN_IT_FF0, ENABLE);
}

void CanSend(Stdcan_n* can) {
    uint8_t  i;
    uint8_t TransmitMailbox = 0;
    
    /* transmit */
    //can->TxMessage.StdId = 0x01;
    //TxMessage.ExtId = luwStdID;
    can->TxMessage.RTR   = CAN_RTR_DATA;     //����֡/Զ��֡
    can->TxMessage.IDE   = CAN_ID_STD;       //��׼֡
    //TxMessage.IDE = CAN_ID_EXT;       //��չ֡
    can->TxMessage.DLC   = 8;
    
    TransmitMailbox = CAN_Transmit(CAN, &can->TxMessage); 
    i = 0;
    while((CAN_TransmitStatus(CAN, TransmitMailbox) != CANTXOK) && (i != 0xFF)) {
        i++;
    }
}

void CanSetID(Stdcan_n* can,uint8_t id) {
    CAN_FilterInitTypeDef      CAN_FilterInitStructure;
    
    can->id = id;
    
    //CAN������FIFO0����
    CAN_FilterInitStructure.CAN_FilterNumber = 0;//���ù�������0����ΧΪ0~13
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; //���ù�������0Ϊ����ģʽ  CAN_FilterMode_IdMask CAN_FilterMode_IdList
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//���ù�������0λ��Ϊ32λ
    //��ʶλ�Ĵ���������
    //ext_id<<3���룬����ͼ9����>>16ȡ��16λ
    CAN_FilterInitStructure.CAN_FilterIdHigh = (can->id<<5) ;  //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterIdLow = 0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((can->ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((can->ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}
/***************************************************************END OF FILE****/
