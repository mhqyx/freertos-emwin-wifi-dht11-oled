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
   
    LED_Init(&gLed, GPIOA, GPIO_Pin_8, 1);//IO��ʼ��
    
    while(1){
        
        LED_On(&gLed);      //ÿ500ms��˸һ��
        vTaskDelay(500);
        
        LED_Off(&gLed);    
        vTaskDelay(500);
    }
}























