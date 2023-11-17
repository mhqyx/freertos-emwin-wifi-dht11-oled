#include "debug.h"




int DEBUG_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;  //���崮�ڳ�ʼ���ṹ��
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;       //ѡ�д���Ĭ������ܽ�         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�������������� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����ܽ�9��ģʽ  
    GPIO_Init(GPIOA, &GPIO_InitStructure);           //���ú������ѽṹ�����������г�ʼ��   


    USART_InitStructure.USART_BaudRate = baud_rate; //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܽ��պͷ�������

    USART_Init(USART1, &USART_InitStructure); //�����ϸ���ֵ�Ľṹ�����⺯��USART_Init���г�ʼ��
    USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_Cmd(USART1, ENABLE);//����USART1��ע��������RCC_APB2PeriphClockCmd()���õ�����


    return 0;
}



#pragma import(__use_no_semihosting)
FILE __stdout;


//��׼����Ҫ��֧�ֺ���
struct __FILE 
{ 
    int handle; 
}; 





//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{
    x = x; 
}




int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
    return (ch);
}


