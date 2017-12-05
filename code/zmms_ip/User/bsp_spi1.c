/* ����ͷ�ļ� *****************************************************************/
#include "stm32f10x.h"
#include "bsp_spi1.h"
/* �ⲿ�������� ***************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ������ *****************************************************************/
/* �ⲿ���� *******************************************************************/
/* �ⲿ�������� ***************************************************************/

/*
********************************************************************************
* �� �� ��: BSP_ConfigSPI1
* ����˵��: SPI1������ò���������SPI1ģʽ��SPI1ͨ���ٶȺ����IO��
* ��    ������
* �� �� ֵ: ��
* ʹ��˵������BSP��ʼ���е���
* ���÷�����BSP_ConfigSPI1();
********************************************************************************
*/
void BSP_ConfigSPI1(void)
{
    /* GPIO�ṹ�� */
    GPIO_InitTypeDef  GPIO_InitStructure; 
    /* SPI�ṹ�� */
    SPI_InitTypeDef SPI_InitStructure; 
    
    /* ʹ��APB2�����ʱ�� */
    /* ʹ��SPIʱ�ӣ�ʹ��GPIOAʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
        RCC_APB2Periph_GPIOA ,ENABLE );
    
    /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |  GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* ����������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    /* SPI1 MISO@GPIOA.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* �������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    
    /* ˫��˫��ȫ˫�� */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/* ����ģʽ */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
    /* 8λ֡�ṹ */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
    /* ʱ�ӿ���ʱΪ�� */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        
    /* ��1�������ز������� */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      
    /* MSS �˿�������� */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;         
    /* SPIʱ�� 72Mhz / 8 = 9M */ 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
    /* ���ݴ����λ��ǰ */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    /* ��ʼ��SPI1 */
    SPI_Init(SPI1, &SPI_InitStructure);
    
	/* ��ʹ��SPI�ڵ�SS������� GPIOA.4 */
	SPI_SSOutputCmd(SPI1,ENABLE);
    /* ʹ��SPI1 */
    SPI_Cmd(SPI1, ENABLE); 
}

/*
********************************************************************************
* �� �� ��: BSP_SPI1SendByte
* ����˵��: SPI1�����ֽ�����
* ��    ����uint8_t byte  �����ֽ�
* �� �� ֵ: uint8_t       �����ֽ�
* ʹ��˵��������SPI1ͨ��ԭ�������ֽڱ��з����ֽ�
* ���÷�����BSP_SPI1SendByte(value);
********************************************************************************
*/
uint8_t BSP_SPI1SendByte(uint8_t byte)
{
    /* �ȴ����ͻ���Ĵ���Ϊ�� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* �������� */
    SPI_I2S_SendData(SPI1, byte);		
    
    /* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    
    return SPI_I2S_ReceiveData(SPI1);
    
}

/*
********************************************************************************
* �� �� ��: BSP_SPI1ReceiveByte
* ����˵��: SPI1�����ֽ�����
* ��    ������
* �� �� ֵ: uint8_t       �����ֽ�
* ʹ��˵��������SPI1ͨ��ԭ�������ֽڱ��з����ֽڣ��ú����ͷ��ͺ�����ͬ
* ���÷�����value = BBSP_SPI1ReceiveByte();
********************************************************************************
*/
uint8_t BSP_SPI1ReceiveByte(void)
{
    /* �ȴ����ͻ���Ĵ���Ϊ�� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* ��������,ͨ������0xFF,��÷������� */
    SPI_I2S_SendData(SPI1, 0xFF);		
    /* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* ���ش�SPIͨ���н��յ������� */
    return SPI_I2S_ReceiveData(SPI1);
}
/***************************************************************END OF FILE****/
