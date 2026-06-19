#ifndef __SERIAL_H
#define __SERIAL_H

// 串口数据结构定义
typedef struct {
    USART_TypeDef* USARTx;
    uint8_t RxData;
    uint8_t RxFlag;
    GPIO_TypeDef* GPIOx;
    uint16_t TxPin;
    uint16_t RxPin;
    IRQn_Type IRQn;
    uint8_t PreemptionPriority;
    uint8_t SubPriority;
} Serial_TypeDef;
// 串口实例声明
extern Serial_TypeDef Serial1;
extern Serial_TypeDef Serial2;
extern Serial_TypeDef Serial3;
// 函数声明
void Serial_Init(Serial_TypeDef* serial);
void Serial_SendByte(Serial_TypeDef* serial, uint8_t Byte);
void Serial_SendArray(Serial_TypeDef* serial, uint8_t *Array, uint16_t Length);
void Serial_SendString(Serial_TypeDef* serial, char *String);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void Serial_SendNumber(Serial_TypeDef* serial, uint32_t Number, uint8_t Length);
void Serial_Printf(Serial_TypeDef* serial, char *format, ...);
uint8_t Serial_GetRxFlag(Serial_TypeDef* serial);
uint8_t Serial_GetRxData(Serial_TypeDef* serial);
void Serial_IRQHandler(Serial_TypeDef* serial);

/*
void Serial1_Init(void);
void Serial1_SendByte(uint8_t Byte);
void Serial1_SendArray(uint8_t *Array, uint16_t Length);
void Serial1_SendString(char *String);
void Serial1_SendNumber(uint32_t Number, uint8_t Length);
void Serial1_Printf(char *format, ...);

uint8_t Serial1_GetRxFlag(void);
uint8_t Serial1_GetRxData(void);
*/

#endif
