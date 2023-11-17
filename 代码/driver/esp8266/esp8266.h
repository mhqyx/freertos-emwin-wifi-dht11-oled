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



//wifi模块模式
#define ESP8266_ModeClient                    1         // 客户端模式
#define ESP8266_Mode_Ap                       2         // 热点模式
#define ESP8266_Mode_ApAndClient              3         // 客户端 + 热点模式

//配置的wifi参数
#define ESP8266_DefaultDebug                  printf         
#define ESP8266_DefaultUsart                  USART2         //默认的串口
#define ESP8266_DefaultBaud                   115200         //默认的波特率
#define ESP8266_DefaultReceiveLength          (2 * 1024)            //默认wifi缓冲区大小，单位字节
#define ESP8266_DefaultMode                   ESP8266_ModeClient// ESP8266_Mode_ApAndClient    //默认的wifi模式


//作为热点时的wifi名字和密码
#define ESP8266_DefaultApName                  "WiFi" 	
#define ESP8266_DefaultApPassword              "12345678"	


//作为client时，连接的wifi用户名和密码
#define ESP8266_DefaultWiFiName                 "htq" 	
#define ESP8266_DefaultWiFiPassword             "17775481921"

#define ESP8266_DefaultServerIp                 "183.230.40.33"  //onenet平台的ip地址     //"192.168.1.12" 
#define ESP8266_DefaultServerPort               "80" 


//定义了GUI
//#define Stemwin_OLED 

#if (defined Stemwin_OLED)

#define  OLED_EmWin_Enable
			
#endif

#if !(defined OLED_EmWin_Enable)

#define  ESP8266_DefaultDebug

#endif





typedef enum Esp8266AtCommand{
	Esp8266AtCommand_AT = 0,                        //测试AT启动
	Esp8266AtCommand_AT_RST,                        //复位wifi模块 
	Esp8266AtCommand_AT_GMR,                        //查询版本信息
	Esp8266AtCommand_AT_CWMODE,                     //设置热点、客户端双模式
	

  //作为AP
    Esp8266AtCommand_AT_CWSAP,                      //打开热点
	Esp8266AtCommand_AT_CIPMUX,                     //设置透传模式  = 1开启，，= 0关闭透传模式
	Esp8266AtCommand_AT_CIPSERVER,                  //建立服务器
	Esp8266AtCommand_AT_SEND_DATA_TO_CLIENT,        //向客户端发送数据

  //作为client
	Esp8266AtCommand_AT_CIFSR,                      //查询设备ip地址
	Esp8266AtCommand_AT_CIPSTART,                   //设置wifi连接TCP服务器
	
	Esp8266AtCommand_AT_CWJAP,                      //连接wifi
    Esp8266AtCommand_AT_CIPMODE,                    //开启透传模式
	Esp8266AtCommand_AT_CIPSEND,                    //开始透传
	 //Esp8266AtCommand_AT_EXIT,                       //退出透传模式
	
	
}Esp8266AtCommand_t;


typedef struct ESP8266_t
{
#if defined (STM32F10X_HD) || defined (STM32F40_41xxx)
	ESP8266_USART_t     usart;             //所使用的串口
#elif defined(Linux_System)
	int                 (*uart_send_byte)(char ch);	
#endif	
	bool                init;              //初始化标志
	int                 mode;              //wifi模式
	int                 (*ESP8266_ReceiveByte)(struct ESP8266_t *esp8266,char rx_data);
	uint32_t            rx_max_length;        //接受缓冲区大小
 	uint32_t            rx_length;     //当前缓冲区有多少字节的数据
	char                rx_buffer[ESP8266_DefaultReceiveLength];         //接受缓冲区
}ESP8266_t;



#if defined (STM32F10X_HD) || defined (STM32F40_41xxx)
int ESP8266_Init(ESP8266_t *esp8266,ESP8266_USART_t usart,uint32_t baud);    //初始化一个wifi模块
#else
int ESP8266_Init(ESP8266_t *esp8266,uint32_t baud);
#endif

int ESP8266_PrintReceiveData(ESP8266_t *esp8266);                             //打印接收到的数据
int ESP8266_SendAtCommand(ESP8266_t *esp8266,Esp8266AtCommand_t at);          //发送at指令
int ESP8266_FindString(ESP8266_t *esp8266,Esp8266AtCommand_t at);             //在wifi模块接收到的数据中查找某个指令
int ESP8266_SetUpServer(ESP8266_t *esp8266,char* port);                       //wifi模块建立服务器

int ESP8266_ConnectWiFi(ESP8266_t *esp8266);
int ESP8266_SendDataToServer(ESP8266_t *esp8266,char* buffer,int len); 
unsigned char *ESP8266_GetIPD(ESP8266_t *esp8266,unsigned short timeOut);

#endif
