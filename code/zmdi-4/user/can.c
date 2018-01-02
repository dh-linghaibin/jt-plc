/* ����ͷ�ļ� *****************************************************************/
#include "can.h"
#include "ringbuf.h"
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
const static uint32_t ext_id =0x1800f001;  

void bxcan_init(struct _can_obj* can) {
    GPIO_InitTypeDef                    GPIO_InitStructure;
    CAN_InitTypeDef                     CAN_InitStructure;
    CAN_FilterInitTypeDef               CAN_FilterInitStructure;
    NVIC_InitTypeDef                    NVIC_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    
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
    switch(can->btl) {
        case B_250K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
            CAN_InitStructure.CAN_Prescaler = 12;
        break;
        case B_125K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
            CAN_InitStructure.CAN_Prescaler = 96;
        break;
        case B_50K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
            CAN_InitStructure.CAN_Prescaler = 160;
        break;
        case B_20K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
            CAN_InitStructure.CAN_Prescaler = 240;
        break;
        case B_10K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
            CAN_InitStructure.CAN_Prescaler = 600;
        break;
        case B_5K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
            CAN_InitStructure.CAN_Prescaler = 686;
        break;
    }
    CAN_Init(CAN, &CAN_InitStructure);
    
    //CAN������FIFO0����
    CAN_FilterInitStructure.CAN_FilterNumber = 0;//���ù�������0����ΧΪ0~13
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; //���ù�������0Ϊ����ģʽ  CAN_FilterMode_IdMask CAN_FilterMode_IdList
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//���ù�������0λ��Ϊ32λ
    //��ʶλ�Ĵ���������
    //ext_id<<3���룬����ͼ9����>>16ȡ��16λ
    CAN_FilterInitStructure.CAN_FilterIdHigh = (can->id<<5) ;  //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterIdLow = 0|CAN_ID_STD; //���ñ�ʶ���Ĵ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((ext_id<<3)>>16) & 0xffff; //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((ext_id<<3)& 0xffff) | CAN_ID_EXT;   //�������μĴ������ֽ�
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    //ʹ��CAN�ж�
    CAN_ClearITPendingBit(CAN, CAN_IT_FF0);
    CAN_ITConfig(CAN, CAN_IT_TME, DISABLE);
    CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN, CAN_IT_FF0, ENABLE);
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
		uint8_t TransmitMailbox = CAN_Transmit(CAN, &transmit_message);
		uint32_t timeout = 0xFFFF;
		while((CAN_TransmitStatus(CAN, TransmitMailbox) != CANTXOK) && (timeout != 0xFFFFFF)) {
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

void bxcan_set_blt(struct _can_obj* can,btl_e btl) {
    CAN_InitTypeDef                     CAN_InitStructure;
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
    can->btl = btl;
    switch(can->btl) {
        case B_250K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
            CAN_InitStructure.CAN_Prescaler = 12;
        break;
        case B_125K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
            CAN_InitStructure.CAN_Prescaler = 96;
        break;
        case B_50K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
            CAN_InitStructure.CAN_Prescaler = 160;
        break;
        case B_20K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
            CAN_InitStructure.CAN_Prescaler = 240;
        break;
        case B_10K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
            CAN_InitStructure.CAN_Prescaler = 600;
        break;
        case B_5K:
            CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
            CAN_InitStructure.CAN_Prescaler = 686;
        break;
    }
    CAN_Init(CAN, &CAN_InitStructure);
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






/***************************************************************END OF FILE****/
