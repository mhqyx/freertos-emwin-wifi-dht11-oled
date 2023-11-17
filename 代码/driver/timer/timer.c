#include "timer.h"
#include "string.h"
#include "GUI.h"
 
volatile unsigned long long gTimerCount = 0;


//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
    
    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ    
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�4��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

    TIM_Cmd(TIM3, DISABLE);  //�ر�TIMx 
    //TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx                     
}

//��ʼ��usΪ��λ�Ķ�ʱ����ʱ����
int DELAY_Init(void)
{
    TIM3_Int_Init(100, 0);
    return 0;
}


void LYY_DelayUs(uint32_t us)
{
    unsigned long long now_time = gTimerCount;

    TIM_Cmd(TIM3, ENABLE);  //�򿪶�ʱ��TIMx 
    while(gTimerCount - now_time > us);
    TIM_Cmd(TIM3, DISABLE);  //�ر�TIMx 
}

void LYY_DelayMs(uint32_t ms)
{
    LYY_DelayUs(ms * 1000);
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
    {
        gTimerCount++; //1us        //OS_TimeMS++;
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}



