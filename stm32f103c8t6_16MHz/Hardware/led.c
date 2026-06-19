#include "stm32f10x.h"                  // Device header

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//A0
}

void LED_ON(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
}

void LED_OFF(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
}