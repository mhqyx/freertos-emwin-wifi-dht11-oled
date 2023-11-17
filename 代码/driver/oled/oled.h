#ifndef __OLED_H
#define __OLED_H



#ifndef STM32F10X_MD
#define STM32F10X_MD
#endif


#ifndef  NULL
#define NULL (void*)0
#endif

#if defined (STM32F40_41xxx)

#include "stm32f4xx_gpio.h"
typedef	GPIO_TypeDef*  DHT11_GPIO_Port_t;

#elif defined (STM32F10X_HD)

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
typedef    GPIO_TypeDef*    DHT11_GPIO_Port_t;

#else 
#error oled.h: No processor defined!

#endif



/*******************************
    oled方向
    (0,63) 
      |
      |
      |
    (0,0)——————————>(127,0)
********************************/
#define OLED_SIZE_X             128     //屏幕的横坐标
#define OLED_SIZE_Y             64      //纵坐标
#define CONFIG_CS_PIN           1       //0表示没有cs引脚，1表示有


void OLED_Init(void);
void OLED_ClearDispaly(u8 color);
void OLED_Refresh(void);  //将显存中的数据刷新到oled中
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t);

#endif

