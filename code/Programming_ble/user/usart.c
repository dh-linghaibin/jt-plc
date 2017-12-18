/* ����ͷ�ļ� *****************************************************************/
#include "usart.h"
#include "ringbuf.h"
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
/* ˽�ж����� *****************************************************************/
/* ˽�б��� *******************************************************************/
/* ȫ�ֱ��� */
uint8_t uart_package_merry[320] = {0};
/** -- ���ζ��г�ʼ�� -- by lhb_steven -- 2017/6/30**/
Stdringbuf UARTRINGBUF = {
    {
        0
    },
    &RingbufInit,
    &RingbufPut,
    &RingbufGut,
    &RingbufNum
};
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/
void UsartPackInit(void) {
    /** -- ���г�ʼ�� -- by lhb_steven -- 2017/6/30**/
    UARTRINGBUF.init(&UARTRINGBUF.ringbuf_n, uart_package_merry, 320);
}
/** -- ���ڳ�ʼ�� -- by lhb_steven -- 2017/6/14**/
void UsartInit(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef nvic_init_struct;
    
    /** -- ���г�ʼ�� -- by lhb_steven -- 2017/6/30**/
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
    
    USART_InitStructure.USART_BaudRate = 115200;//���ô��ڲ�����115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����Ч��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
    USART_Init(USART2, &USART_InitStructure); //������ṹ��
    
    /***** NVIC�ж����� *****/
    nvic_init_struct.NVIC_IRQChannel = USART2_IRQn;
    nvic_init_struct.NVIC_IRQChannelPriority = 0;
    nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init_struct);
    
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART2,USART_IT_TC,DISABLE);
    USART_Cmd(USART2, ENABLE);//ʹ�ܴ���1
}

/** -- ����һ���ֽ� -- by lhb_steven -- 2017/6/14**/
void UsartSendByte(uint8_t byte) {
    while(!((USART2->ISR)&(1<<7)));
    USART2->TDR=byte;	
}

/** -- �����ַ��� -- by lhb_steven -- 2017/6/14**/
void UsartSend(uint8_t *Buffer, uint32_t Length) {
    while(Length != 0) {
        while(!((USART2->ISR)&(1<<7)));//�ȴ�������
        USART2->TDR= *Buffer;
        Buffer++;
        Length--;
    }
}

/** -- ��ȡ���յ������� -- by lhb_steven -- 2017/6/14**/
uint8_t UART_Recive(void) {	
    while(!(USART2->ISR & (1<<5)));//�ȴ����յ�����
    return(USART2->RDR);   //��������
}

uint8_t UARTReadPackageP(StdUart_n* Uart) {
    switch(UARTRINGBUF.num(&UARTRINGBUF.ringbuf_n)) {
        case 0:
            return false;
        break;
        case 16://�Ͽ�����
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
        case 9://��������
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
        default://��Ҫ����
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
