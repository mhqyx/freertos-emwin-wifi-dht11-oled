#include "dht11.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"


#define    DHT11_DelayMs(t)                      vTaskDelay(t)  //LYY_DelayMs(t)  //vTaskDelay(t)
#define    DHT11_DelayUs(t)                      LYY_DelayUs(t)   


#define    DHT11_PIN_HIGH                        1
#define    DHT11_PIN_LOW                         0 


#if  defined  (STM32F10X_HD)

#include "stm32f10x_gpio.h"

#define    __DHT11_CONFIG_IO_INPUT(DHT11)        {  GPIO_InitTypeDef GPIO_InitStructure; \
                                                   GPIO_InitStructure.GPIO_Mode     =  GPIO_Mode_IN_FLOATING; \
                                                   GPIO_InitStructure.GPIO_Speed    =  GPIO_Speed_50MHz; \
                                                   GPIO_InitStructure.GPIO_Pin      =  DHT11->pin ; \
                                                   GPIO_Init(DHT11->port,&GPIO_InitStructure); \
                                                 }

#define    __DHT11_CONFIG_IO_OUTPUT(DHT11)       {  GPIO_InitTypeDef GPIO_InitStructure; \
                                                    GPIO_InitStructure.GPIO_Mode     =  GPIO_Mode_Out_PP; \
                                                    GPIO_InitStructure.GPIO_Speed    =  GPIO_Speed_50MHz; \
                                                    GPIO_InitStructure.GPIO_Pin      =  DHT11->pin ; \
                                                    GPIO_Init(DHT11->port, &GPIO_InitStructure); \
                                                  }

#define    __DHT11_IO_SET(port,pin,value)         {  if (value == DHT11_PIN_HIGH) \
                                                         GPIO_SetBits(port, pin); \
                                                     else \
                                                         GPIO_ResetBits(port, pin);    \
                                                   }

#define    DHT11_IO_H(DHT11)                      {__DHT11_IO_SET(DHT11->port, DHT11->pin, DHT11_PIN_HIGH)}
#define    DHT11_IO_L(DHT11)                      {__DHT11_IO_SET(DHT11->port, DHT11->pin, DHT11_PIN_LOW)}
#define    DHT11_IO_IN(DHT11)                     GPIO_ReadInputDataBit(DHT11->port, DHT11->pin)

#endif




/******************************************************************************
 * @brief      ��λDHT11
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
static int DHT11_Rst(DHT11_t *dht11)       
{
    if (!dht11 || !dht11->init) 
        return -1;  //��ʼ������֮��

    __DHT11_CONFIG_IO_OUTPUT(dht11);     //SET OUTPUT
    DHT11_IO_L(dht11);     //����DQ
    DHT11_DelayMs(20);        //��������18ms
    DHT11_IO_H(dht11);     //DQ=1 
    DHT11_DelayUs(30);        //��������20~40us
    __DHT11_CONFIG_IO_INPUT(dht11);

    return 0;
}




/******************************************************************************
 * @brief      �ȴ�DHT11�Ļ�Ӧ
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     0, ����, ����1:δ��⵽DHT11�Ĵ���
 *
 ******************************************************************************/
static int DHT11_Check(DHT11_t *dht11)         
{
    if(!dht11 || !dht11->init) 
        return -1;  //��ʼ������֮��

    int retry = 0; 
    while (DHT11_IO_IN(dht11) && (retry < 100)) {    //DHT11������40~80us
        retry++;
        DHT11_DelayUs(1);
    };

    if (retry >= 100)
        return -2;
    else 
        retry = 0;

    while (!DHT11_IO_IN(dht11) && (retry < 100)) {//DHT11���ͺ���ٴ�����40~80us
        retry++;
        DHT11_DelayUs(1);
    };
    if (retry >= 100)
        return -3;

    return 0;
}




/******************************************************************************
 * @brief      ��DHT11��ȡһ��λ
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     0, 1
 *
 ******************************************************************************/
static uint8_t DHT11_ReadBit(DHT11_t *dht11)     
{
    uint8_t retry=0;
    while (DHT11_IO_IN(dht11) && (retry < 100)) {//�ȴ���Ϊ�͵�ƽ
        retry++;
        DHT11_DelayUs(1);
    }

    retry=0;
    while (!DHT11_IO_IN(dht11) && (retry < 100)) {//�ȴ���ߵ�ƽ
        retry++;
        DHT11_DelayUs(1);
    }

    DHT11_DelayUs(40);//�ȴ�40us
    if (DHT11_IO_IN(dht11))
        return 1;
    else 
        return 0;
}




/******************************************************************************
 * @brief      ��DHT11��ȡһ���ֽ�
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     ����������
 *
 ******************************************************************************/
static uint8_t DHT11_ReadByte(DHT11_t *dht11)
{
    uint8_t i,dat;
    dat = 0;
    for (i = 0;i < 8;i++) {
        dat <<= 1; 
        dat |= DHT11_ReadBit(dht11);
    }

    return dat;
}




/******************************************************************************
 * @brief      ��DHT11��ȡһ������
 *                   temp:�¶�ֵ(��Χ:0~50��)
 *                   humi:ʪ��ֵ(��Χ:20%~90%)
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int DHT11_ReadData(DHT11_t *dht11)    
{
    if (!dht11 || !dht11->init) 
        return -1;  //��ʼ������֮��
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst(dht11);

    if (DHT11_Check(dht11) == 0){
        for (i = 0;i < 5;i++) {//��ȡ40λ����
            buf[i] = DHT11_ReadByte(dht11);
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]){
            dht11->humidity = buf[0];
            dht11->temperature = buf[2];
        }
    }else
        return -2;

    return 0;
}




/******************************************************************************
 * @brief      ��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���.  DHT11_Init(&gDht11, GPIOA, GPIO_Pin_6);
 *
 * @param[in]  dht11  :  dht11�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int DHT11_Init(DHT11_t *dht11, DHT11_GPIO_Port_t port, uint32_t pin)  
{
    if(!dht11) 
        return -1;

    //�������ţ�Ĭ�����
#if defined (STM32F10X_HD)    

    assert_param(IS_GPIO_ALL_PERIPH(port));

    if (port  == GPIOA)      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
    else if (port  == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    else if (port  == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 
    else if (port  == GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 
    else if (port  == GPIOE) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
    else if (port  == GPIOF) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); 
    else if (port  == GPIOG) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); 
    else printf("DHT11_Init port no\r\n");

    __DHT11_CONFIG_IO_OUTPUT(dht11); 

#endif

    dht11->port = port;
    dht11->pin = pin;
    dht11->temperature = 0.0;
    dht11->humidity = 0.0;
    dht11->init = true;

    DHT11_IO_H(dht11); 
    DHT11_Rst(dht11);

    return DHT11_Check(dht11);
}

