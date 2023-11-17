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

//只支持http协议



//使用onenet平台
#define DeviceID     "1162165017"
#define API_Key      "8r=9PE2T4=ehU=fNcJq7T9htNYQ="

#define OneNet_Malloc   pvPortMalloc
#define OneNet_Free     vPortFree
    

#if defined (STM32F40_41xxx)



#elif defined (STM32F10X_HD)


#else 

#error onenet.h: No processor defined!

#endif

#define  OneNet_DataStreamDefaultNumber       5    //默认的产品数据流数量





typedef struct onenet_t
{
      ESP8266_t*    esp8266;                                                 //wifi模块节点
      bool          init;                                                    //初始化标志
      char*         product_id;                                              //产品ID 或者用户名
      char*         api_key;                                                 //密钥 或者密码  
      char*         device_id;                                               //设备ID
      char*         data_stream[OneNet_DataStreamDefaultNumber];               //上传数据流
      int           data_stream_cnt;   //当前已添加的数据量数量  
}Onenet_t;


int OneNetInit(Onenet_t* onenet,char* product_id,char* api_key,char* device_id);
int OneNet_SendDataHttp(Onenet_t* onenet,char* json,int json_len);
#endif
