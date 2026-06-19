#include "stm32f10x.h"                  // Device header
#include <stdio.h>                       // 添加sprintf所需的头文件
#include "delay.h"
#include "Serial.h"    //串口:串口屏、USB转TTL、ESP8266
#include "HMI_TJC.h"    // 包含对应的头文件

int fputc(int ch, FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (uint8_t)ch);
    return ch;
}

void TJC_Init(void)
{
    Serial_Init(&Serial1);
}


// 显示文本
void TJC_ShowText(char *name, char *text)
{
    printf("%s.txt=\"%s\"\xff\xff\xff", name, text);
}

// 显示数字
void TJC_ShowNumber(char *name, int num)
{
    printf("%s.val=%d\xff\xff\xff", name, num);
}

void TJC_ShowFloat(char *name, float num)
{
    printf("%s.val=%d\xff\xff\xff", name, (int)(num*100));
}

void TJC_ShowWave(char* name, int ch, int val)
{
    printf("add %s,%d,%d\xff\xff\xff", name, ch, val);
}

void TJC_ClearWave(char* name, int ch)
{
    printf("cle %s,%d\xff\xff\xff", name, ch);
}

