#include "task_dht11.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "dht11.h"



QueueHandle_t      gDHT112EmWin_Queue;
xSemaphoreHandle   gDHT112EmWin_Queue_Mutex;

QueueHandle_t      gDHT112WIFI_Queue;
xSemaphoreHandle   gDHT112WIFI_Queue_Mutex;

static DHT11_t sgDHT11;

void task_dht11(void *p)
{
    float   dht11_data[2] = {0};
    float   temperature;      //温度值
    float   humidity;         //湿度
    //初始化DHT11传感器
    int ret = DHT11_Init(&sgDHT11, GPIOA, GPIO_Pin_7);
    
    printf("ret %d\r\n", ret);

    //初始化队列
    gDHT112EmWin_Queue = xQueueCreate(10, sizeof(float) * 2);
    if (!gDHT112EmWin_Queue)
        printf("create emwin queue fail\r\n");
    
    gDHT112WIFI_Queue = xQueueCreate(10, sizeof(float) * 2);
    if (!gDHT112WIFI_Queue)
        printf("create wifi queue fail\r\n");
    
    gDHT112EmWin_Queue_Mutex = xSemaphoreCreateMutex();
    if (!gDHT112EmWin_Queue_Mutex)
        printf("create emwin queue mutex  fail\r\n");
    
    gDHT112WIFI_Queue_Mutex = xSemaphoreCreateMutex();
    if (!gDHT112WIFI_Queue_Mutex)
        printf("create wifi queue mutex fail\r\n");
    
    while(1){
        DHT11_ReadData(&sgDHT11);
        
        dht11_data[0] = sgDHT11.temperature;
        dht11_data[1] = sgDHT11.humidity;
    
        temperature  =  sgDHT11.temperature;
        humidity     =  sgDHT11.humidity;

        printf("dht11   temperature:   %0.2f  humidity:   %0.2f \r\n", temperature, humidity);
        
        xSemaphoreTake(gDHT112EmWin_Queue_Mutex, portMAX_DELAY);   //上锁
        xQueueSend(gDHT112EmWin_Queue, dht11_data, portMAX_DELAY);   //向队列中发送数据
        xSemaphoreGive(gDHT112EmWin_Queue_Mutex);  //解锁
        
        xSemaphoreTake(gDHT112WIFI_Queue_Mutex, portMAX_DELAY);   //上锁
        xQueueSend(gDHT112WIFI_Queue, dht11_data, portMAX_DELAY);   //向队列中发送数据
        xSemaphoreGive(gDHT112WIFI_Queue_Mutex);  //解锁
                
        vTaskDelay(500);
    }
}











