#include "stm32f10x.h"     
#include "delay.h"

void Servo_PWM_Init(void)
{ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//将PA0引脚映射为定时器2的通道1
    //定时器配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定时周期=72MHz/PSC+1/ARR+1
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; //ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; //PSC
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//定时周期=72MHz/72-1/1000-1+1=1ms
    //PWM配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; //CCR1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);//TIM2_CH1,对应引脚PA0
    //启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

void Motor_PWM_Init(void)//电机PWM引脚：PB0，TIM3的CH3
{ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//将PB0引脚复用为定时器3的通道3
    //定时器配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定时周期=72MHz/PSC+1/ARR+1
    TIM_TimeBaseStructure.TIM_Period = 100 - 1; //ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; //PSC
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//定时周期=72MHz/72-1/1000-1+1=1ms
    //PWM配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; //CCR1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);//TIM3_CH3,对应引脚PB0
    //启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)//设置CCR1的值
{
    if(Compare > 1000)//占空比=CCR1/（ARR+1）
        Compare = 1000;
    TIM_SetCompare1(TIM2, Compare);
}//输出高电平时间=占空比*定时周期=CCR1*1ms

void PWM_SetCompare3(uint16_t Compare)//设置CCR3的值
{
    if(Compare > 100)//占空比=CCR3/（ARR+1）
        Compare = 100;
    TIM_SetCompare3(TIM3, Compare);
}//输出高电平时间=占空比*定时周期=CCR3*1ms