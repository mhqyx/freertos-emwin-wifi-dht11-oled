#ifndef MAIN_H
#define MAIN_H

//��׼��ͷ�ļ�
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


//����ͷ�ļ�
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


//���ͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "GUI.h"


#include "task_dht11.h"
#include "task_ui.h"
#include "task_wifi.h"
#include "task_led.h"

#endif
