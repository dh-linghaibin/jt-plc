/* ����ͷ�ļ� *****************************************************************/
#include "bsp_usart.h"
#include <stdio.h>
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
#define DEBUG_USART1  (1)     /* ʹ��USART1 ���е��� */
#define DEBUG_USART2  (0)     /* ʹ��USART2 ���е��� */
#define USART1_ENISR  (0)     /* ʹ��USART1�ķ��ͺͽ����ж� */
#define USART2_ENISR  (0)     /* ʹ��USART2�ķ��ͺͽ����ж� */
/* ˽�ж����� *****************************************************************/
/* ˽�б��� *******************************************************************/
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/

/*
********************************************************************************
* �� �� ��: BSP_ConfigUSART1
* ����˵��: ��ʼ��USART1
* ��    ������
* �� �� ֵ: ��
* ʹ��˵����ϵͳ�ϵ��ʼ��ʱ���ã�
* ���÷�����BSP_ConfigUSART1();
********************************************************************************
*/
void BSP_ConfigUSART1(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
    
  /* ��1������GPIO��USARTʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

  /* ��2������USART1 Tx@PA9��GPIO����Ϊ���츴��ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ��3������USART1 Rx@PA10��GPIO����Ϊ��������ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ��4��������USART1����
      ������   = 115200
      ���ݳ��� = 8
      ֹͣλ   = 1
      У��λ   = No
      ��ֹӲ������(����ֹRTS��CTS)
      ʹ�ܽ��պͷ���
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);

  /* ��5����ʹ�� USART1�� ������� */
  USART_Cmd(USART1, ENABLE);

  /* ���������ɱ�־��Transmission Complete flag */
  USART_ClearFlag(USART1, USART_FLAG_TC);
  
}

/*
********************************************************************************
* �� �� ��: BSP_ConfigUSART2
* ����˵��: ��ʼ��USART2
* ��    ������
* �� �� ֵ: ��
* ʹ��˵����ϵͳ�ϵ��ʼ��ʱ���ã�
* ���÷�����BSP_ConfigUSART2();
********************************************************************************
*/
void BSP_ConfigUSART2(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  /* ��1������GPIOA��USART2ʱ�� */
  /* ʹ��GPIOAʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
  /* ʹ��USART2ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  
  /* ��2������USART2 Tx@PA2��GPIO����Ϊ���츴��ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ��3������USART2 Rx@PA3��GPIO����Ϊ��������ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
  /* ��4��������USART2����
      ������   = 9600
      ���ݳ��� = 8
      ֹͣλ   = 1
      У��λ   = No
      ��ֹӲ������(����ֹRTS��CTS)
      ʹ�ܽ��պͷ���
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  /* ��5����ʹ�� USART2�� ������� */
  USART_Cmd(USART2, ENABLE);
  
  /* ���������ɱ�־��Transmission Complete flag */
  USART_ClearFlag(USART2, USART_FLAG_TC);
}

/*
********************************************************************************
* �� �� ��: fputc
* ����˵��: ϵͳ��������������������
* ��    ������
* �� �� ֵ: ��
* ʹ��˵�������ݺ궨��ѡ�� DEBUG_USART1 ѡ�񴮿�1����
                           DEBUG_USART2 ѡ�񴮿�2����
            �����û�ж���Ϊ1����ʹ��fputc���ܡ�
* ���÷�����ͨ��ʹ��printf������ӵ���
********************************************************************************
*/
#if DEBUG_USART1 || DEBUG_USART2
int fputc(int ch, FILE *f)
{

#if (DEBUG_USART1 == 1) && (DEBUG_USART2 == 1)
#warning ����ͬʱ�趨USART1��USART2�����޸�DEBUG_USARTx����
#endif
  
#if (DEBUG_USART1 == 1) && (DEBUG_USART2 == 0)
  /* дһ���ֽڵ�USART1 */
  USART_SendData(USART1, (uint8_t) ch);
  /* �ȴ����ͽ��� */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  return ch;
#endif
  
#if (DEBUG_USART1 == 0) && (DEBUG_USART2 == 1)
  /* дһ���ֽڵ�USART2 */
  USART_SendData(USART2, (uint8_t) ch);
  /* �ȴ����ͽ��� */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
  return ch;
#endif
  
}
#endif
/*
********************************************************************************
* �� �� ��: fgetc
* ����˵��: ϵͳ������ͨ�����ڽ�������
* ��    ������
* �� �� ֵ: ��
* ʹ��˵�������ݺ궨��ѡ�� DEBUG_USART1 ѡ�񴮿�1����
                           DEBUG_USART2 ѡ�񴮿�2����
* ���÷�����ͨ��ʹ��scanf������ӵ���
********************************************************************************
*/
#if DEBUG_USART1 || DEBUG_USART2
int fgetc(FILE *f)
{
#if (DEBUG_USART1 == 1) && (DEBUG_USART2 == 0)
  /* �ȴ�����1�������� */
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
  return (int)USART_ReceiveData(USART1);
#endif
 
#if (DEBUG_USART1 == 0) && (DEBUG_USART2 == 1)
  /* �ȴ�����2�������� */
  while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  return (int)USART_ReceiveData(USART2);  
#endif 
}
#endif

