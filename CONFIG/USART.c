/*******************************************************************
 *MPU6050
 *@brief 
 *@brief 
 *@time  2017.1.8
 *@editorС��&zin
 *�ɿذ���QQȺ551883670,����759421287@qq.com
 *����Ȩʹ����Ա����ֹʹ�á���ֹ���ģ�Υ��һ�����֣���Ȩ����
 ******************************************************************/

#include "usart.h"
#include "misc.h"
#include "stdio.h"
#include "delay.h"
/*
 * ��������USART1_Config
 * ����  ��USART1 GPIO ����,����ģʽ���á�
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
 void DMA_Uart_Init(void)
{

    DMA_InitTypeDef DMA_InitStructure;
    /* DMA clock enable */

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
//=DMA_Configuration==============================================================================//

/*--- LUMMOD_UART_Tx_DMA_Channel DMA Config ---*/

    DMA_Cmd(DMA1_Channel4, DISABLE);                           // ��DMAͨ��  USART1_TX   DMA1_Channel4

    DMA_DeInit(DMA1_Channel4);                                 // �ָ�ȱʡֵ

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// ���ô��ڷ������ݼĴ���

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;         // ���÷��ͻ������׵�ַ

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���

    DMA_InitStructure.DMA_BufferSize = 0;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�

    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���

    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�

    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ

    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����

    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ

    DMA_Init(DMA1_Channel4, &DMA_InitStructure);               // д������

    DMA_ClearFlag( DMA1_FLAG_GL4/*DMA1_FLAG_TC4 | DMA1_FLAG_TE4 */);                                 // ���DMA���б�־

		DMA_Cmd(DMA1_Channel4, DISABLE); // �ر�DMA

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
}
uint8_t datatest[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* ���ô���1 ��USART1�� ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Configure the NVIC Preemption Priority Bits */  

	
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//USART1  ����1ȫ���ж� 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ�1
	/*IRQͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/*����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1*/
	NVIC_Init(&NVIC_InitStructure);
	
		 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // ����DMAͨ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	

	/*����GPIO�˿�����*/
  /* ���ô���1 ��USART1 Tx (PA.09)��*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	/* ���ô���1 USART1 Rx (PA.10)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = 230400;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	
	
	DMA_Uart_Init();

	USART_Cmd(USART1, ENABLE); //ʹ�ܴ��� 
	 USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  // ��������DMA����

//	while(1)//���Է���
//	{
//		while (!(USART1->SR & USART_FLAG_TXE));	 // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//		USART_SendData(USART1,0xAA);	 	
//		delay_ms(10);
//	}
//	while(1)//���Է���
//	{
//		  DMA_ClearFlag( DMA1_FLAG_GL4/*DMA1_FLAG_TC4 | DMA1_FLAG_TE4 */);   
//		Uart_Start_DMA_Tx(datatest,25);
//		delay_ms(12);
//		
//	}
}

void DMA1_Channel4_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_FLAG_TC4))
    {
				DMA_ClearFlag( DMA1_FLAG_GL4/*DMA1_FLAG_TC4 | DMA1_FLAG_TE4 */);

				DMA_Cmd(DMA1_Channel4, DISABLE);   // �ر�DMAͨ��
    }

}


void Uart_Start_DMA_Tx(const int8_t *data ,uint8_t size)
{
//		DMA1_Channel4->CCR &= ~DMA_CCR1_EN;
//delay_ms(1);

	DMA1_Channel4->CPAR = (uint32_t)(&USART1->DR);
	DMA1_Channel4->CMAR = (uint32_t)data;
	
    DMA1_Channel4->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ

  //  DMA_Cmd(DMA1_Channel4, ENABLE);        //��ʼDMA����
	DMA1_Channel4->CCR |= DMA_CCR1_EN;
}



void USART1_SendByte(const int8_t *Data,uint8_t len)
{ 
	uint8_t i;
	
	for(i=0;i<len;i++)
	{
		while (!(USART1->SR & USART_FLAG_TXE));	 // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		USART_SendData(USART1,*(Data+i));	 
	}
}




int8_t CheckSend[7]={0xAA,0XAA,0xEF,2,0,0,0};

void USART1_setBaudRate(uint32_t baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate =  baudRate;
	USART_Init(USART1, &USART_InitStructure);
}






/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputc(int ch, FILE *f)
{
	/* ��Printf���ݷ������� */
	USART_SendData(USART1, (unsigned char) ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
	return (ch);
}

