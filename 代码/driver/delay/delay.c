#include "delay.h"
#include "timer.h"
#include "string.h"
#include "GUI.h"
 
volatile unsigned int gTimerCount = 0;




/******************************************************************************
 * @brief      初始化TIM3     定时时间 = (arr + 1) * (psc + 1) / 36M 
 *
 * @param[in]  arr    :  自动重装值
 * @param[in]  psc    :  时钟预分频数
 *
 * @return     无
 *
 ******************************************************************************/
static void TIM3_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period =  arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    
    TIM_TimeBaseStructure.TIM_Prescaler =  psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    TIM_Cmd(TIM3, DISABLE);  //关闭TIMx 
}




/******************************************************************************
 * @brief      初始化us为单位的定时器延时函数
 *
 * @param[in]  无
 *
 * @return     无
 *
 ******************************************************************************/
int DELAY_Init(void)
{
    //TIM3_Init(10-1, 72 - 1); // 10 * 72 / 72000 000 * 1000 0000  us=  10 * 72 / 72 = 10 us
    TIM3_Init(50000 - 1, 72 - 1);
    return 0;
}




/******************************************************************************
 * @brief      微秒级延时
 *
 * @param[in]  us    :  延时微秒数
 *
 * @return     无
 *
 ******************************************************************************/
void LYY_DelayUs(uint32_t us)
{
    TIM_Cmd(TIM3, ENABLE); //启动定时器
    while(TIM3->CNT < us);
    TIM3->CNT = 0;
    TIM_Cmd(TIM3, DISABLE); //关闭定时器

}




/******************************************************************************
 * @brief      毫秒级延时,不建议在OS中使用
 *
 * @param[in]  us    :  延时毫秒数
 *
 * @return     无
 *
 ******************************************************************************/
void LYY_DelayMs(uint32_t ms)
{
    for(int i = 0;i < ms; i++){
        LYY_DelayUs(1000);
    }
}



