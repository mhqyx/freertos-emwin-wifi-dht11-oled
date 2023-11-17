#include "onenet.h"


//onenet初始化的时候，不添加数据量，在之后才添加
int OneNetInit(Onenet_t* onenet,char* product_id,char* api_key,char* device_id)
{
	if(onenet == NULL) return -1;
	int res = 0; 
	ESP8266_t *OneNetEsp8266 = (ESP8266_t* )OneNet_Malloc(sizeof(ESP8266_t));
	if(OneNetEsp8266 == NULL) return -2;
	
	onenet->esp8266           =  OneNetEsp8266;	
	onenet->init              =  true;
	onenet->product_id        =  product_id;
	onenet->api_key           =  api_key;
	onenet->device_id         =  device_id;
	
	
	res = ESP8266_Init(onenet->esp8266 ,ESP8266_DefaultUsart, ESP8266_DefaultBaud);
    printf("ESP8266_Init:  %d\r\n", res);
	if(res < 0)return res;
	res = ESP8266_ConnectWiFi(onenet->esp8266);
	if(res < 0)return res;
	return 0;
	
}
//使用http协议向onenet平台发送数据
/*
POST /devices/727994078/datapoints?type=3 HTTP/1.1
api-key:N7ivRHnQ66ILih4gRF8vy3f3fMw=
Host:api.heclouds.com
Content-Length:200      //后面传输的字节数据

{"temp":0,"humi":20}  //传输两个节点的数据，一个温度，一个湿度
*/

//以http协议向onenet平台发送数据，一次可以传输多个数据
//传输数据的字符串格式，cnt表示一共传输多少个数据
//发送之前，先构建json串
//json格式   {"temp":0,"humi":20}
int OneNet_SendDataHttp(Onenet_t* onenet,char* json,int json_len)
{
		if(onenet == NULL || onenet->init != true) return -1;
	  int res = 0;
	  
    char* send_buffer = (char *)OneNet_Malloc(json_len + 50);  //每一个数据字符串最大20字节
	  memset(send_buffer,0,json_len + 50);

sprintf(send_buffer,"\
POST /devices/%s/datapoints?type=3 HTTP/1.1\r\n\
api-key:%s\r\n\
Host:api.heclouds.com\r\n\
Connection:close\r\n\
Content-Length:200\r\n\r\n\
%s\r\n",DeviceID,API_Key,json); 
	  printf("%s",send_buffer);
		res = ESP8266_SendDataToServer(onenet->esp8266,send_buffer,strlen(send_buffer));
		OneNet_Free(send_buffer);
		return res;
}


//测试
/*
	static Onenet_t  gOneNet;
	OneNetInit(&gOneNet,NULL,API_Key,DeviceID);
	int i = 0;
	while(1)
	{
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"{\"temp\":%d,\"humi\":%d}",i,i);  //传输温湿度
		  OneNet_SendDataHttp(&gOneNet,buffer,strlen(buffer));
		  i++;
		  delay_ms(1000);
	}


*/
