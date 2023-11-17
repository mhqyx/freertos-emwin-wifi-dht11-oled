#include "task_ui.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "GUI.h"
#include "oled.h"


extern QueueHandle_t  gDHT112EmWin_Queue;
extern xSemaphoreHandle gDHT112EmWin_Queue_Mutex;

static uint8_t  u8EmwinDisplayBuffer[50] = {0};
void task_ui(void *p)
{
    float dht11_data[2] = {0};
    //初始化oled, emwin
    OLED_Init();
    GUI_Init();
    float temperature;      //温度值
    float humidity;         //湿度
    
    while(1){
        xSemaphoreTake(gDHT112EmWin_Queue_Mutex, portMAX_DELAY);   //上锁

        BaseType_t  wait_cnt = uxQueueMessagesWaiting(gDHT112EmWin_Queue);
        if (wait_cnt <= 0){
            
            xSemaphoreGive(gDHT112EmWin_Queue_Mutex);  //解锁
            vTaskDelay(10);
            continue;  
        }
        
        xQueueReceive(gDHT112EmWin_Queue, dht11_data, portMAX_DELAY);   //从队列中接受数据
        xSemaphoreGive(gDHT112EmWin_Queue_Mutex);  //解锁
        
        temperature  =  dht11_data[0];
        humidity     =  dht11_data[1];
        
//        printf("ui   temperature:   %0.2f  humidity:   %0.2f \r\n", temperature, humidity);
//        vTaskDelay(1000);

        GUI_Clear();
        
        memset(u8EmwinDisplayBuffer, 0, sizeof(u8EmwinDisplayBuffer));
        sprintf((char *)u8EmwinDisplayBuffer, "T: %0.2f", temperature);  //12.25
        GUI_DispStringAt((const char *)u8EmwinDisplayBuffer, 0, 0);
        
        memset(u8EmwinDisplayBuffer, 0, sizeof(u8EmwinDisplayBuffer));
        sprintf((char *)u8EmwinDisplayBuffer, "H: %0.2f", humidity);  //53
        GUI_DispStringAt((const char *)u8EmwinDisplayBuffer, 0, 20);
        
        GUI_Delay(50);
        OLED_Refresh();
    }
}











