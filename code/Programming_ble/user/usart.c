/* 引用头文件 *****************************************************************/
#include "usart.h"
#include "ringbuf.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
uint8_t uart_package_merry[320] = {0};
/** -- 环形队列初始化 -- by lhb_steven -- 2017/6/30**/
Stdringbuf UARTRINGBUF = {
    {
        0
    },
    &RingbufInit,
    &RingbufPut,
    &RingbufGut,
    &RingbufNum
};
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
void UsartPackInit(void) {
    /** -- 队列初始化 -- by lhb_steven -- 2017/6/30**/
    UARTRINGBUF.init(&UARTRINGBUF.ringbuf_n, uart_package_merry, 320);
}
/** -- 串口初始化 -- by lhb_steven -- 2017/6/14**/
void UsartInit(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef nvic_init_struct;
    
    /** -- 队列初始化 -- by lhb_steven -- 2017/6/30**/
    UARTRINGBUF.init(&UARTRINGBUF.ringbuf_n, uart_package_merry, 320);
    
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);        
    /*
    *  USART1_TX -> PA9 , USART1_RX ->        PA3
    */                                
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;                 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);        
    
    USART_InitStructure.USART_BaudRate = 9600;//设置串口波特率115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//设置效验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
    USART_Init(USART2, &USART_InitStructure); //配置入结构体
    
    /***** NVIC中断设置 *****/
    nvic_init_struct.NVIC_IRQChannel = USART2_IRQn;
    nvic_init_struct.NVIC_IRQChannelPriority = 0;
    nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_struct);
    
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART2,USART_IT_TC,DISABLE);
    USART_Cmd(USART2, ENABLE);//使能串口1
}

/** -- 发送一个字节 -- by lhb_steven -- 2017/6/14**/
void UsartSendByte(uint8_t byte) {
    while(!((USART2->ISR)&(1<<7)));
    USART2->TDR=byte;	
}

/** -- 发送字符串 -- by lhb_steven -- 2017/6/14**/
void UsartSend(uint8_t *Buffer, uint32_t Length) {
    while(Length != 0) {
        while(!((USART2->ISR)&(1<<7)));//等待发送完
        USART2->TDR= *Buffer;
        Buffer++;
        Length--;
    }
}

/** -- 读取接收到的数据 -- by lhb_steven -- 2017/6/14**/
uint8_t UART_Recive(void) {	
    while(!(USART2->ISR & (1<<5)));//等待接收到数据
    return(USART2->RDR);   //读出数据
}

uint8_t UARTReadPackageP(StdUart_n* Uart) {
    switch(UARTRINGBUF.num(&UARTRINGBUF.ringbuf_n)) {
        case 0:
            return false;
        break;
        case 16://断开蓝牙
            if(Uart->Ble_en == 1) {
                UARTRINGBUF.get(&UARTRINGBUF.ringbuf_n,Uart->package,16);
                if( (Uart->package[2] == 0x4d) && (Uart->package[7] == 0x43) \
                                            && (Uart->package[13] == 0x0d) ){
                    Uart->Ble_en = 0;
                } else {
                    goto need_handle;
                }
            } else {
                goto need_handle;
            }
        break;
        case 9://连接蓝牙
            goto need_handle;
            if(Uart->Ble_en == 0) {
                UARTRINGBUF.get(&UARTRINGBUF.ringbuf_n,Uart->package,9);
                if( (Uart->package[0] == 0x54) && (Uart->package[4] == 0x4f) \
                                            && (Uart->package[7] == 0x0A) ){
                    Uart->Ble_en = 1;
                } else {
                    goto need_handle;
                }
            } else {
                goto need_handle;
            }
        break;
        default://需要处理
        need_handle:
            return true;
        break;
    }
    return 0;
}

uint8_t UARTReadPackage(StdUart_n* Uart,uint8_t len) {
    if(UARTRINGBUF.get(&UARTRINGBUF.ringbuf_n,Uart->package,len)) {
        return true;
    }
    return false;
}
/***************************************************************END OF FILE****/
