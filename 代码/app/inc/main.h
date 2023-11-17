#ifndef MAIN_H
#define MAIN_H

//标准库头文件
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


//驱动头文件
#include "oled.h"
#include "font.h"
#include "timer.h"
#include "led.h"
#include "esp8266.h"
#include "onenet.h"
#include "debug.h"
#include "delay.h"
#include "dht11.h"
#include "esp8266.h"


//组件头文件
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "GUI.h"


#include "task_dht11.h"
#include "task_ui.h"
#include "task_wifi.h"
#include "task_led.h"

#endif
