/* 引用头文件 *****************************************************************/
#include "can.h"
#include "ringbuf.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
uint8_t can_package_merry[80] = {0};
/** -- 环形队列初始化 -- by lhb_steven -- 2017/6/30**/
Stdringbuf RINGBUF = {
    {
        0
    },
    &RingbufInit,
    &RingbufPut,
    &RingbufGut
};
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/

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
void CanInit(Stdcan_n* can) {
    GPIO_InitTypeDef                    GPIO_InitStructure;
    CAN_InitTypeDef                     CAN_InitStructure;
    CAN_FilterInitTypeDef               CAN_FilterInitStructure;
    NVIC_InitTypeDef                    NVIC_InitStructure;
    
    RINGBUF.init(&RINGBUF.ringbuf_n, can_package_merry, 80);
    
    CanClock(can->s.port);
    GPIO_InitStructure.GPIO_Pin = can->s.pin;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(can->s.port, &GPIO_InitStructure);
    GPIO_ResetBits(can->s.port,can->s.pin);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
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
    
     //CAN中断使能和优先级设置
    NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;         
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
        
    //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE; 
    CAN_InitStructure.CAN_ABOM = DISABLE;  
    CAN_InitStructure.CAN_AWUM = DISABLE; 
    CAN_InitStructure.CAN_NART = ENABLE; 
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_Normal; 
    //CAN波特率设置,500KHz@48MHz
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
    CAN_InitStructure.CAN_Prescaler = 160;
    CAN_Init(CAN, &CAN_InitStructure);
    
    //CAN过滤器FIFO0配置
    CAN_FilterInitStructure.CAN_FilterNumber = 0;//设置过滤器组0，范围为0~13
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; //设置过滤器组0为屏蔽模式  CAN_FilterMode_IdMask CAN_FilterMode_IdList
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//设置过滤器组0位宽为32位
    //标识位寄存器的设置
    //ext_id<<3对齐，见上图9，再>>16取高16位
    CAN_FilterInitStructure.CAN_FilterIdHigh = (can->id<<5) ;  //设置标识符寄存器高字节
    CAN_FilterInitStructure.CAN_FilterIdLow = 0|CAN_ID_STD; //设置标识符寄存器低字节
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((can->ext_id<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((can->ext_id<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    //使能CAN中断
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
    can->TxMessage.RTR   = CAN_RTR_DATA;     //数据帧/远程帧
    can->TxMessage.IDE   = CAN_ID_STD;       //标准帧
    //TxMessage.IDE = CAN_ID_EXT;       //扩展帧
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
    
    //CAN过滤器FIFO0配置
    CAN_FilterInitStructure.CAN_FilterNumber = 0;//设置过滤器组0，范围为0~13
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; //设置过滤器组0为屏蔽模式  CAN_FilterMode_IdMask CAN_FilterMode_IdList
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//设置过滤器组0位宽为32位
    //标识位寄存器的设置
    //ext_id<<3对齐，见上图9，再>>16取高16位
    CAN_FilterInitStructure.CAN_FilterIdHigh = (can->id<<5) ;  //设置标识符寄存器高字节
    CAN_FilterInitStructure.CAN_FilterIdLow = 0|CAN_ID_STD; //设置标识符寄存器低字节
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((can->ext_id<<3)>>16) & 0xffff; //设置屏蔽寄存器高字节
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((can->ext_id<<3)& 0xffff) | CAN_ID_EXT;   //设置屏蔽寄存器低字节
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}

uint8_t CanReadPackage(Stdcan_n* can) {
    if(RINGBUF.get(&RINGBUF.ringbuf_n,can->package,8)) {
        return true;
    }
    return false;
}

/***************************************************************END OF FILE****/
