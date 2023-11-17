#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#if defined (STM32F10X_HD)

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"


#else 
    #error delay.h: No processor defined!
    
#endif

void TIM3_Int_Init(uint16_t arr,uint16_t psc);
void ConfigureTimeForRunTimeStats(void);



extern volatile unsigned long long FreeRTOSRunTimeTicks;



#endif
