#include "stm32f10x.h"  
#include "Serial.h" 

// void ESP8266_Init(void)
// {
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

//     // GPIO初始化
//     GPIO_InitTypeDef GPIO_InitStructure;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//TX配置为复用推挽输出
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOB, &GPIO_InitStructure);

//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//RX配置为上拉输入
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//     GPIO_Init(GPIOB, &GPIO_InitStructure);

//     // USART初始化
//     USART_InitTypeDef USART_InitStructure;
//     USART_InitStructure.USART_BaudRate = 115200;
//     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//     USART_InitStructure.USART_Parity = USART_Parity_No;
//     USART_InitStructure.USART_StopBits = USART_StopBits_1;
//     USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//     USART_Init(USART3, &USART_InitStructure);

//     USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//接收中断使能

// 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//     NVIC_InitTypeDef NVIC_InitStructure;
//     NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//     NVIC_Init(&NVIC_InitStructure);

//     USART_Cmd(USART3, ENABLE);
// }
void ESP8266_Init(void)
{
    Serial_Init(&Serial3);//串口初始化
}

