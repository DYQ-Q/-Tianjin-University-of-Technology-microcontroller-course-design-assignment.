#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

/* 初始化 DHT11，返回 0 表示成功（可忽略返回值） */
uint8_t DHT11_Init(void);

/* 读取一次温湿度：
 * temp：温度整数部分
 * humi：湿度整数部分
 * 返回值：0 成功，其它为错误码
 */
uint8_t DHT11_ReadData(uint8_t *temp, uint8_t *humi);

#endif
