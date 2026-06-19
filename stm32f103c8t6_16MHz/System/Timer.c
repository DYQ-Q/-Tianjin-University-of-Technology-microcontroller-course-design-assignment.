#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//定时器输入时钟频率=72MHz
	TIM_InternalClockConfig(TIM2);
	/*
	定时时间 = (Prescaler + 1) × (Period + 1) / 定时器输入时钟频率
	步骤:
	1.定时器工作频率 = 定时器输入时钟频率 / (Prescaler + 1)
		72/720=0.01MHz=10000Hz=100KHz
	2.单次计数时间 = 1 / 定时器工作频率
		1/100KHz=0.00001s
	3.总定时时间 = (Period + 1) × 单次计数时间
		100*0.00001=1ms
	即该代码每1s进一次定时器中断
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;//Period（计数器）
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;//Prescaler（预分频器）
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

/*
void TIM2_IRQHandler(void)//每1ms进一次TIM2的中断服务函数
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		//写自己的代码
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
*/
