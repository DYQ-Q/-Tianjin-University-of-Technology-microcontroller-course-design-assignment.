#include "stm32f10x.h"                  // Device header
#include "delay.h"

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);//将GPIOB1初始化为推挽输出，用于蜂鸣器
    GPIO_SetBits(GPIOB,GPIO_Pin_1);//蜂鸣器初始化为关闭
}

void Buzzer_On(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
}

void Buzzer_Off(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

void Buzzer_Beep(uint16_t ms)
{
    Buzzer_On();
    delay_ms(ms);
    Buzzer_Off();
}