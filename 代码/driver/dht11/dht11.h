#ifndef __DHT11_H
#define __DHT11_H 

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifndef  NULL
#define NULL (void*)0
#endif

#if defined (STM32F40_41xxx)
typedef    GPIO_TypeDef*  DHT11_GPIO_Port_t;

#elif defined (STM32F10X_HD)

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


typedef    GPIO_TypeDef*  DHT11_GPIO_Port_t;

#else 

    #error dht11.h: No processor defined!

#endif

typedef struct dht11_t
{
    DHT11_GPIO_Port_t     port;             //��ʹ�õĶ˿�
    uint32_t              pin;              //��ʹ�õ�����
    double                temperature;      //�¶�ֵ
    double                humidity;         //ʪ��
    bool                  init;             //��ʼ����־
}DHT11_t;


int   DHT11_Init(DHT11_t* dht11, DHT11_GPIO_Port_t port, uint32_t pin);      //��ʼ��DHT11
int   DHT11_ReadData(DHT11_t* dht11);                                //��ȡDHT11������


#endif















