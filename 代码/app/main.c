#include "main.h"



int main(void)
{     
    int ret = 10;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4     
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);     //配置CRC校验，emWin用的
    DEBUG_Init(115200);
    DELAY_Init();

    printf("start\r\n");

       
    //初始化OLED屏幕
    OLED_Init();
    OLED_ClearDispaly(0x00);
    OLED_Refresh();

    //创建任务
    xTaskCreate(task_dht11,   "task dht11",  512,   NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(task_wifi,    "task wifi",   2048,  NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(task_ui,      "task ui",     512,   NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(task_led,     "task led",    128,   NULL, tskIDLE_PRIORITY + 1, NULL);
    // 启动调度器
    vTaskStartScheduler();
    while(1);   //不可能运行到这里，运行到这里说明系统出问题了
}


