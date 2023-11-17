#include "delay.h"
#include "timer.h"
#include "string.h"
#include "GUI.h"
 
volatile unsigned int gTimerCount = 0;




/******************************************************************************
 * @brief      ��ʼ��TIM3     ��ʱʱ�� = (arr + 1) * (psc + 1) / 36M 
 *
 * @param[in]  arr    :  �Զ���װֵ
 * @param[in]  psc    :  ʱ��Ԥ��Ƶ��
 *
 * @return     ��
 *
 ******************************************************************************/
static void TIM3_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period =  arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
    TIM_TimeBaseStructure.TIM_Prescaler =  psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TIM_Cmd(TIM3, DISABLE);  //�ر�TIMx 
}




/******************************************************************************
 * @brief      ��ʼ��usΪ��λ�Ķ�ʱ����ʱ����
 *
 * @param[in]  ��
 *
 * @return     ��
 *
 ******************************************************************************/
int DELAY_Init(void)
{
    //TIM3_Init(10-1, 72 - 1); // 10 * 72 / 72000 000 * 1000 0000  us=  10 * 72 / 72 = 10 us
    TIM3_Init(50000 - 1, 72 - 1);
    return 0;
}




/******************************************************************************
 * @brief      ΢�뼶��ʱ
 *
 * @param[in]  us    :  ��ʱ΢����
 *
 * @return     ��
 *
 ******************************************************************************/
void LYY_DelayUs(uint32_t us)
{
    TIM_Cmd(TIM3, ENABLE); //������ʱ��
    while(TIM3->CNT < us);
    TIM3->CNT = 0;
    TIM_Cmd(TIM3, DISABLE); //�رն�ʱ��

}




/******************************************************************************
 * @brief      ���뼶��ʱ,��������OS��ʹ��
 *
 * @param[in]  us    :  ��ʱ������
 *
 * @return     ��
 *
 ******************************************************************************/
void LYY_DelayMs(uint32_t ms)
{
    for(int i = 0;i < ms; i++){
        LYY_DelayUs(1000);
    }
}



