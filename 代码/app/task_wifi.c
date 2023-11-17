#include "task_wifi.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "esp8266.h"
#include "onenet.h"


extern QueueHandle_t  gDHT112WIFI_Queue;
extern xSemaphoreHandle gDHT112WIFI_Queue_Mutex;


static char buffer[1024];
static Onenet_t  gOneNet;
void task_wifi(void *p)
{
    float dht11_data[2] = {0};

    float temperature = 12;      //�¶�ֵ
    float humidity = 45;         //ʪ��
    int ret = OneNetInit(&gOneNet, NULL, API_Key, DeviceID);
    printf("OneNetInit:  %d\r\n", ret);
    int cnt = 0;
    while(1){
         xSemaphoreTake(gDHT112WIFI_Queue_Mutex, portMAX_DELAY);   //����
        
         BaseType_t  wait_cnt = uxQueueMessagesWaiting(gDHT112WIFI_Queue);
        if (wait_cnt <= 0){
            
            xSemaphoreGive(gDHT112WIFI_Queue_Mutex);  //����
            vTaskDelay(10);
            continue;  
        }
        xQueueReceive(gDHT112WIFI_Queue, dht11_data, portMAX_DELAY);  
        xSemaphoreGive(gDHT112WIFI_Queue_Mutex);  //����
        
        temperature  =  dht11_data[0];
        humidity     =  dht11_data[1];
        
        vTaskDelay(50);
        cnt++;
        
        if (cnt == 20 * 10){ //ÿ10�뷢��һ�����ݴ�onenet������
            cnt = 0;
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer,"{\"temperature\":%0.2f,\"humidity\":%0.2f}", temperature++, humidity++);
            OneNet_SendDataHttp(&gOneNet, buffer, strlen(buffer));
        }

    }
}

//�����жϣ�����wifi����
void USART2_IRQHandler(void)                    //����1�жϷ������
{
    uint8_t Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART2);    //��ȡ���յ�������

        if(gOneNet.esp8266->ESP8266_ReceiveByte != NULL)
        {
            gOneNet.esp8266->ESP8266_ReceiveByte(gOneNet.esp8266, (char)Res);
            
        }
        
  }  
}










