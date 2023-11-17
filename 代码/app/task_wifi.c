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

    float temperature = 12;      //温度值
    float humidity = 45;         //湿度
    int ret = OneNetInit(&gOneNet, NULL, API_Key, DeviceID);
    printf("OneNetInit:  %d\r\n", ret);
    int cnt = 0;
    while(1){
         xSemaphoreTake(gDHT112WIFI_Queue_Mutex, portMAX_DELAY);   //上锁
        
         BaseType_t  wait_cnt = uxQueueMessagesWaiting(gDHT112WIFI_Queue);
        if (wait_cnt <= 0){
            
            xSemaphoreGive(gDHT112WIFI_Queue_Mutex);  //解锁
            vTaskDelay(10);
            continue;  
        }
        xQueueReceive(gDHT112WIFI_Queue, dht11_data, portMAX_DELAY);  
        xSemaphoreGive(gDHT112WIFI_Queue_Mutex);  //解锁
        
        temperature  =  dht11_data[0];
        humidity     =  dht11_data[1];
        
        vTaskDelay(50);
        cnt++;
        
        if (cnt == 20 * 10){ //每10秒发送一次数据打onenet服务器
            cnt = 0;
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer,"{\"temperature\":%0.2f,\"humidity\":%0.2f}", temperature++, humidity++);
            OneNet_SendDataHttp(&gOneNet, buffer, strlen(buffer));
        }

    }
}

//串口中断，处理wifi数据
void USART2_IRQHandler(void)                    //串口1中断服务程序
{
    uint8_t Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART2);    //读取接收到的数据

        if(gOneNet.esp8266->ESP8266_ReceiveByte != NULL)
        {
            gOneNet.esp8266->ESP8266_ReceiveByte(gOneNet.esp8266, (char)Res);
            
        }
        
  }  
}










