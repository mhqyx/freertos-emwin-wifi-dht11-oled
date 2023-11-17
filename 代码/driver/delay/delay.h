#ifndef  DELAY_H
#define  DELAY_H

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

int DELAY_Init(void);
void LYY_DelayUs(uint32_t us);
void LYY_DelayMs(uint32_t ms);

#endif
