#include "stm32f10x.h" 
#include "PWM.h"    
#include "delay.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //PA4,PA5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//将PA4,PA5配置为推挽输出模式
	
    Motor_PWM_Init();
}

void Motor_SetSpeed(int16_t speed)
{
    if(speed >= 0)
    {
        // Forward
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        PWM_SetCompare3(speed);
    }
    else if(speed < 0)
    {
        // Reverse
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        PWM_SetCompare3(-speed);
    }
}
