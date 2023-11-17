#include "led.h"


//高电平灭，低电平亮
//#define     LED_PIN_ON                          0
//#define     LED_PIN_OFF                         1 

#if  defined  (STM32F10X_HD)

#include "stm32f10x_gpio.h"


#define    __LED_CONFIG_IO_OUTPUT(led)         {GPIO_InitTypeDef GPIO_InitStructure; \
                                                GPIO_InitStructure.GPIO_Mode     =  GPIO_Mode_Out_PP; \
                                                GPIO_InitStructure.GPIO_Speed    =  GPIO_Speed_50MHz; \
                                                GPIO_InitStructure.GPIO_Pin      =  led->pin ; \
                                                GPIO_Init(led->port, &GPIO_InitStructure);}

#define    __LED_IO_SET(port, pin, value)       { if (1 == value) \
                                                      GPIO_SetBits(port, pin); \
                                                    else \
                                                      GPIO_ResetBits(port, pin);    \
                                                }
#endif




/******************************************************************************
 * @brief     初始化LED   //LED_Init(&gled,GPIOC, GPIO_Pin_13);
 *
 * @param[in]  led   :  led结构体指针
 * @param[in]  port  :  引脚端口
 * @param[in]  pin   :  引脚
 *
 * @return     0, 表示初始化成功, 其他值表示失败
 *
 ******************************************************************************/
int LED_Init(LED_t* led, LED_GPIO_Port_t port, uint32_t pin, uint8_t led_on_level) //IO初始化
{
    if (!led)
        return -1;

    //配置引脚，默认输出
#if  defined  (STM32F10X_MD)    

    assert_param(IS_GPIO_ALL_PERIPH(port));

    if (port  == GPIOA)       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
    else if (port  == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    else if (port  == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
    else if (port  == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 
    else if (port  == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
    else if (port  == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); 
    else if (port  == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); 
    else printf("LED_Init port no\r\n");    

#endif

    led->port   = port;
    led->pin    = pin;
    led->init   = true;
    led->status = false;

    __LED_CONFIG_IO_OUTPUT(led);
    LED_Off(led);

    return 0;
}




/******************************************************************************
 * @brief      打开led
 *
 * @param[in]  led   :  led结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_On(LED_t* led)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后
    
    __LED_IO_SET(led->port, led->pin, led->led_on_level);   //低电平亮
    led->status = true;

    return 0;
}




/******************************************************************************
 * @brief      关闭led
 *
 * @param[in]  led   :  led结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_Off(LED_t* led)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后
    
    __LED_IO_SET(led->port, led->pin, !led->led_on_level);   //高电平灭
    led->status = false;

    return 0;
}




/******************************************************************************
 * @brief      设置led状态
 *
 * @param[in]  led   :  led结构体指针
 * @param[in]  status:  led开关状态
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_SetStatus(LED_t *led, bool status)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后
    
    led->status = status;

    return 0;
}




/******************************************************************************
 * @brief      刷新led状态
 *
 * @param[in]  led   :  led结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_RefreshStatus(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后
    
    if (led->status == true) {
        __LED_IO_SET(led->port, led->pin, led->led_on_level);   //低电平亮
    } else {
        __LED_IO_SET(led->port, led->pin, !led->led_on_level);   //高电平灭
    }

    return 0;
}

