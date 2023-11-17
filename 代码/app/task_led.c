#include "task_led.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "led.h"

static LED_t gLed;
void task_led(void *p)
{
   
    LED_Init(&gLed, GPIOA, GPIO_Pin_8, 1);//IO初始化
    
    while(1){
        
        LED_On(&gLed);      //每500ms闪烁一次
        vTaskDelay(500);
        
        LED_Off(&gLed);    
        vTaskDelay(500);
    }
}























