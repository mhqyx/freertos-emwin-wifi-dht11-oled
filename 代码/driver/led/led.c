#include "led.h"


//�ߵ�ƽ�𣬵͵�ƽ��
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
 * @brief     ��ʼ��LED   //LED_Init(&gled,GPIOC, GPIO_Pin_13);
 *
 * @param[in]  led   :  led�ṹ��ָ��
 * @param[in]  port  :  ���Ŷ˿�
 * @param[in]  pin   :  ����
 *
 * @return     0, ��ʾ��ʼ���ɹ�, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_Init(LED_t* led, LED_GPIO_Port_t port, uint32_t pin, uint8_t led_on_level) //IO��ʼ��
{
    if (!led)
        return -1;

    //�������ţ�Ĭ�����
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
 * @brief      ��led
 *
 * @param[in]  led   :  led�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_On(LED_t* led)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��
    
    __LED_IO_SET(led->port, led->pin, led->led_on_level);   //�͵�ƽ��
    led->status = true;

    return 0;
}




/******************************************************************************
 * @brief      �ر�led
 *
 * @param[in]  led   :  led�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_Off(LED_t* led)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��
    
    __LED_IO_SET(led->port, led->pin, !led->led_on_level);   //�ߵ�ƽ��
    led->status = false;

    return 0;
}




/******************************************************************************
 * @brief      ����led״̬
 *
 * @param[in]  led   :  led�ṹ��ָ��
 * @param[in]  status:  led����״̬
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_SetStatus(LED_t *led, bool status)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��
    
    led->status = status;

    return 0;
}




/******************************************************************************
 * @brief      ˢ��led״̬
 *
 * @param[in]  led   :  led�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_RefreshStatus(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��
    
    if (led->status == true) {
        __LED_IO_SET(led->port, led->pin, led->led_on_level);   //�͵�ƽ��
    } else {
        __LED_IO_SET(led->port, led->pin, !led->led_on_level);   //�ߵ�ƽ��
    }

    return 0;
}

