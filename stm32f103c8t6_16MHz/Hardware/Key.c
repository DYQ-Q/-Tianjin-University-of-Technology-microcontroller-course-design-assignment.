#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "delay.h"

int16_t Motor_Speed=0;

typedef struct {
    GPIO_TypeDef* GPIOx;      // GPIO端口
    uint16_t Pin;             // 引脚号
    uint32_t EXTI_Line;        // EXTI线
    IRQn_Type IRQn;           // 中断源
    uint8_t PreemptionPriority;  // 抢占优先级
    uint8_t SubPriority;      // 子优先级
} Key_TypeDef;

Key_TypeDef Keys[4] = {
    {GPIOB, GPIO_Pin_12, EXTI_Line12, EXTI15_10_IRQn, 0, 0},  // 反转
    {GPIOB, GPIO_Pin_13, EXTI_Line13, EXTI15_10_IRQn, 0, 0},  // 加速
    {GPIOB, GPIO_Pin_14, EXTI_Line14, EXTI15_10_IRQn, 0, 0},  // 减速
    {GPIOB, GPIO_Pin_15, EXTI_Line15, EXTI15_10_IRQn, 0, 0}   // 停止
};

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    // 配置所有按键的GPIO
    for(int i = 0; i < 4; i++) {
        GPIO_InitStructure.GPIO_Pin = Keys[i].Pin;
        GPIO_Init(Keys[i].GPIOx, &GPIO_InitStructure); 
        // 配置EXTI线
        //GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, Keys[i].Pin & 0x00FF);
    }
    // EXTI初始化
    // EXTI_InitTypeDef EXTI_InitStructure;
    // EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    // EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    // // 配置所有EXTI线
    // EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
    // EXTI_Init(&EXTI_InitStructure);
    // // NVIC初始化
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // NVIC_InitTypeDef NVIC_InitStructure;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // // 配置所有中断
    // //for(int i = 0; i < 4; i++) {
    // NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//Keys[i].IRQn
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//Keys[i].PreemptionPriority
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//Keys[i].SubPriority
    // NVIC_Init(&NVIC_InitStructure);
    //}
}

uint8_t Key_GetNum()
{
	uint8_t Key_Num=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
		delay_ms(20);
		Key_Num=1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)
		delay_ms(20);
		Key_Num=2;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)==0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)==0)
		delay_ms(20);
		Key_Num=3;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)==0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)==0)
		delay_ms(20);
		Key_Num=4;
	}

	return Key_Num;
}

/*void EXTI15_10_IRQHandler(void)	
{
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
	{
		Motor_Speed=-Motor_Speed;
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
	{
		Motor_Speed+=20;
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
	{
		Motor_Speed-=20;
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
	{
		Motor_Speed=0;
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
	Motor_SetSpeed(Motor_Speed);
}*/

/*
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);//B12，B13,B14,B15

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI12_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//子优先级为0，反转
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;//子优先级为1，加速
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;//子优先级为2，停止
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;//子优先级为3，停止
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}*/


