#include "timer.h"
#include "string.h"
#include "GUI.h"
 
volatile unsigned long long gTimerCount = 0;


//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    
    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级4级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

    TIM_Cmd(TIM3, DISABLE);  //关闭TIMx 
    //TIM_Cmd(TIM3, ENABLE);  //使能TIMx                     
}

//初始化us为单位的定时器延时函数
int DELAY_Init(void)
{
    TIM3_Int_Init(100, 0);
    return 0;
}


void LYY_DelayUs(uint32_t us)
{
    unsigned long long now_time = gTimerCount;

    TIM_Cmd(TIM3, ENABLE);  //打开定时器TIMx 
    while(gTimerCount - now_time > us);
    TIM_Cmd(TIM3, DISABLE);  //关闭TIMx 
}

void LYY_DelayMs(uint32_t ms)
{
    LYY_DelayUs(ms * 1000);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
    {
        gTimerCount++; //1us        //OS_TimeMS++;
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}



