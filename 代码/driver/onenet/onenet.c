#include "onenet.h"


//onenet��ʼ����ʱ�򣬲��������������֮������
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
//ʹ��httpЭ����onenetƽ̨��������
/*
POST /devices/727994078/datapoints?type=3 HTTP/1.1
api-key:N7ivRHnQ66ILih4gRF8vy3f3fMw=
Host:api.heclouds.com
Content-Length:200      //���洫����ֽ�����

{"temp":0,"humi":20}  //���������ڵ�����ݣ�һ���¶ȣ�һ��ʪ��
*/

//��httpЭ����onenetƽ̨�������ݣ�һ�ο��Դ���������
//�������ݵ��ַ�����ʽ��cnt��ʾһ��������ٸ�����
//����֮ǰ���ȹ���json��
//json��ʽ   {"temp":0,"humi":20}
int OneNet_SendDataHttp(Onenet_t* onenet,char* json,int json_len)
{
		if(onenet == NULL || onenet->init != true) return -1;
	  int res = 0;
	  
    char* send_buffer = (char *)OneNet_Malloc(json_len + 50);  //ÿһ�������ַ������20�ֽ�
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


//����
/*
	static Onenet_t  gOneNet;
	OneNetInit(&gOneNet,NULL,API_Key,DeviceID);
	int i = 0;
	while(1)
	{
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"{\"temp\":%d,\"humi\":%d}",i,i);  //������ʪ��
		  OneNet_SendDataHttp(&gOneNet,buffer,strlen(buffer));
		  i++;
		  delay_ms(1000);
	}


*/
