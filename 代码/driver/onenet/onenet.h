#ifndef __ONENET_H
#define __ONENET_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "esp8266.h"

#ifndef  NULL
#define NULL (void*)0
#endif

//ֻ֧��httpЭ��



//ʹ��onenetƽ̨
#define DeviceID     "1162165017"
#define API_Key      "8r=9PE2T4=ehU=fNcJq7T9htNYQ="

#define OneNet_Malloc   pvPortMalloc
#define OneNet_Free     vPortFree
    

#if defined (STM32F40_41xxx)



#elif defined (STM32F10X_HD)


#else 

#error onenet.h: No processor defined!

#endif

#define  OneNet_DataStreamDefaultNumber       5    //Ĭ�ϵĲ�Ʒ����������





typedef struct onenet_t
{
      ESP8266_t*    esp8266;                                                 //wifiģ��ڵ�
      bool          init;                                                    //��ʼ����־
      char*         product_id;                                              //��ƷID �����û���
      char*         api_key;                                                 //��Կ ��������  
      char*         device_id;                                               //�豸ID
      char*         data_stream[OneNet_DataStreamDefaultNumber];               //�ϴ�������
      int           data_stream_cnt;   //��ǰ����ӵ�����������  
}Onenet_t;


int OneNetInit(Onenet_t* onenet,char* product_id,char* api_key,char* device_id);
int OneNet_SendDataHttp(Onenet_t* onenet,char* json,int json_len);
#endif
