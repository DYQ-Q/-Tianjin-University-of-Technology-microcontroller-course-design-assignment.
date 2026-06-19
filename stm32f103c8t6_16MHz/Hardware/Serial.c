#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

//uint8_t Serial_RxData;//接收数据
//uint8_t Serial_RxFlag;//接收标志

// 定义串口数据结构
typedef struct {
    USART_TypeDef* USARTx;
    uint8_t RxData;
    uint8_t RxFlag;
    GPIO_TypeDef* GPIOx;
    uint16_t TxPin;
    uint16_t RxPin;
    IRQn_Type IRQn;
    uint8_t PreemptionPriority;//抢占优先级
    uint8_t SubPriority;//子优先级
} Serial_TypeDef;
// 串口实例
Serial_TypeDef Serial1 = {USART1, 0, 0, GPIOA, GPIO_Pin_9, GPIO_Pin_10, USART1_IRQn, 0, 1};
Serial_TypeDef Serial2 = {USART2, 0, 0, GPIOA, GPIO_Pin_2, GPIO_Pin_3, USART2_IRQn, 0, 3};
Serial_TypeDef Serial3 = {USART3, 0, 0, GPIOB, GPIO_Pin_10, GPIO_Pin_11, USART3_IRQn, 0, 2};

void Serial_Init(Serial_TypeDef* serial)
{
    // 使能时钟
    if(serial->USARTx == USART1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    } else if(serial->USARTx == USART2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    } else if(serial->USARTx == USART3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//TX配置为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = serial->TxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(serial->GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//RX配置为上拉输入
    GPIO_InitStructure.GPIO_Pin = serial->RxPin;
    GPIO_Init(serial->GPIOx, &GPIO_InitStructure);

    // USART初始化
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(serial->USARTx, &USART_InitStructure);

    USART_ITConfig(serial->USARTx, USART_IT_RXNE, ENABLE);//接收中断使能

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = serial->IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = serial->PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = serial->SubPriority;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(serial->USARTx, ENABLE);
}

void Serial_SendByte(Serial_TypeDef* serial, uint8_t Byte)
{
    USART_SendData(serial->USARTx, Byte);
    while (USART_GetFlagStatus(serial->USARTx, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(Serial_TypeDef* serial, uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(serial, Array[i]);
    }
}

void Serial_SendString(Serial_TypeDef* serial, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte(serial, String[i]);
    }
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

void Serial_SendNumber(Serial_TypeDef* serial, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(serial, Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

void Serial_Printf(Serial_TypeDef* serial, char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(serial, String);
}

uint8_t Serial_GetRxFlag(Serial_TypeDef* serial)
{
    if (serial->RxFlag == 1)
    {
        serial->RxFlag = 0;
        return 1;
    }
    return 0;
}

uint8_t Serial_GetRxData(Serial_TypeDef* serial)
{
    return serial->RxData;
}

// 通用中断处理函数
void Serial_IRQHandler(Serial_TypeDef* serial)
{
    if (USART_GetITStatus(serial->USARTx, USART_IT_RXNE) == SET)
    {
        serial->RxData = USART_ReceiveData(serial->USARTx);
        serial->RxFlag = 1;
        USART_ClearITPendingBit(serial->USARTx, USART_IT_RXNE);
    }
}

// 各个串口的中断服务函数
void USART1_IRQHandler(void)
{
    Serial_IRQHandler(&Serial1);
}

// void USART2_IRQHandler(void)
// {
//     Serial_IRQHandler(&Serial2);
// }

void USART3_IRQHandler(void)
{
    Serial_IRQHandler(&Serial3);
}
