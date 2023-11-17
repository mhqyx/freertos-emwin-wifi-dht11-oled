#ifndef  __ESP8266_H
#define  __ESP8266_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>



#ifndef  NULL
#define NULL (void*)0
#endif



#if defined (STM32F10X_HD)


#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"


typedef	USART_TypeDef*  ESP8266_USART_t;

#else 

#error esp8266.h: No processor defined!

#endif



//wifiģ��ģʽ
#define ESP8266_ModeClient                    1         // �ͻ���ģʽ
#define ESP8266_Mode_Ap                       2         // �ȵ�ģʽ
#define ESP8266_Mode_ApAndClient              3         // �ͻ��� + �ȵ�ģʽ

//���õ�wifi����
#define ESP8266_DefaultDebug                  printf         
#define ESP8266_DefaultUsart                  USART2         //Ĭ�ϵĴ���
#define ESP8266_DefaultBaud                   115200         //Ĭ�ϵĲ�����
#define ESP8266_DefaultReceiveLength          (2 * 1024)            //Ĭ��wifi��������С����λ�ֽ�
#define ESP8266_DefaultMode                   ESP8266_ModeClient// ESP8266_Mode_ApAndClient    //Ĭ�ϵ�wifiģʽ


//��Ϊ�ȵ�ʱ��wifi���ֺ�����
#define ESP8266_DefaultApName                  "WiFi" 	
#define ESP8266_DefaultApPassword              "12345678"	


//��Ϊclientʱ�����ӵ�wifi�û���������
#define ESP8266_DefaultWiFiName                 "htq" 	
#define ESP8266_DefaultWiFiPassword             "17775481921"

#define ESP8266_DefaultServerIp                 "183.230.40.33"  //onenetƽ̨��ip��ַ     //"192.168.1.12" 
#define ESP8266_DefaultServerPort               "80" 


//������GUI
//#define Stemwin_OLED 

#if (defined Stemwin_OLED)

#define  OLED_EmWin_Enable
			
#endif

#if !(defined OLED_EmWin_Enable)

#define  ESP8266_DefaultDebug

#endif





typedef enum Esp8266AtCommand{
	Esp8266AtCommand_AT = 0,                        //����AT����
	Esp8266AtCommand_AT_RST,                        //��λwifiģ�� 
	Esp8266AtCommand_AT_GMR,                        //��ѯ�汾��Ϣ
	Esp8266AtCommand_AT_CWMODE,                     //�����ȵ㡢�ͻ���˫ģʽ
	

  //��ΪAP
    Esp8266AtCommand_AT_CWSAP,                      //���ȵ�
	Esp8266AtCommand_AT_CIPMUX,                     //����͸��ģʽ  = 1��������= 0�ر�͸��ģʽ
	Esp8266AtCommand_AT_CIPSERVER,                  //����������
	Esp8266AtCommand_AT_SEND_DATA_TO_CLIENT,        //��ͻ��˷�������

  //��Ϊclient
	Esp8266AtCommand_AT_CIFSR,                      //��ѯ�豸ip��ַ
	Esp8266AtCommand_AT_CIPSTART,                   //����wifi����TCP������
	
	Esp8266AtCommand_AT_CWJAP,                      //����wifi
    Esp8266AtCommand_AT_CIPMODE,                    //����͸��ģʽ
	Esp8266AtCommand_AT_CIPSEND,                    //��ʼ͸��
	 //Esp8266AtCommand_AT_EXIT,                       //�˳�͸��ģʽ
	
	
}Esp8266AtCommand_t;


typedef struct ESP8266_t
{
#if defined (STM32F10X_HD) || defined (STM32F40_41xxx)
	ESP8266_USART_t     usart;             //��ʹ�õĴ���
#elif defined(Linux_System)
	int                 (*uart_send_byte)(char ch);	
#endif	
	bool                init;              //��ʼ����־
	int                 mode;              //wifiģʽ
	int                 (*ESP8266_ReceiveByte)(struct ESP8266_t *esp8266,char rx_data);
	uint32_t            rx_max_length;        //���ܻ�������С
 	uint32_t            rx_length;     //��ǰ�������ж����ֽڵ�����
	char                rx_buffer[ESP8266_DefaultReceiveLength];         //���ܻ�����
}ESP8266_t;



#if defined (STM32F10X_HD) || defined (STM32F40_41xxx)
int ESP8266_Init(ESP8266_t *esp8266,ESP8266_USART_t usart,uint32_t baud);    //��ʼ��һ��wifiģ��
#else
int ESP8266_Init(ESP8266_t *esp8266,uint32_t baud);
#endif

int ESP8266_PrintReceiveData(ESP8266_t *esp8266);                             //��ӡ���յ�������
int ESP8266_SendAtCommand(ESP8266_t *esp8266,Esp8266AtCommand_t at);          //����atָ��
int ESP8266_FindString(ESP8266_t *esp8266,Esp8266AtCommand_t at);             //��wifiģ����յ��������в���ĳ��ָ��
int ESP8266_SetUpServer(ESP8266_t *esp8266,char* port);                       //wifiģ�齨��������

int ESP8266_ConnectWiFi(ESP8266_t *esp8266);
int ESP8266_SendDataToServer(ESP8266_t *esp8266,char* buffer,int len); 
unsigned char *ESP8266_GetIPD(ESP8266_t *esp8266,unsigned short timeOut);

#endif
