#ifndef __LED_H
#define __LED_H     


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef  NULL
#define NULL (void*)0
#endif

#if defined (STM32F10X_HD)


#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

typedef    GPIO_TypeDef*  LED_GPIO_Port_t;

#else 

#error led.h: No processor defined!

#endif



typedef struct led_t {
    LED_GPIO_Port_t     port;             //所使用的端口
    uint32_t            pin;              //所使用的引脚
    uint8_t             led_on_level;     //led亮的时候，IO的电平状态
    bool                status;           //led状态，默认灭  status == false 灭    
	                                      //status == true 亮
    bool                init;             //初始化标志
}LED_t;




int LED_Init(LED_t* led, LED_GPIO_Port_t port, uint32_t pin, uint8_t led_on_level);//IO初始化
int LED_On(LED_t* led);
int LED_Off(LED_t* led);
int LED_SetStatus(LED_t* led, bool status);
int LED_RefreshStatus(LED_t* led);


#endif
