#ifndef __HMI_TJC_H
#define __HMI_TJC_H

#include "stm32f10x.h"
#include <stdint.h>

// 函数声明
void TJC_Init(void);

void TJC_ShowText(char *name, char *text);
void TJC_ShowNumber(char *name, int num);
void TJC_ShowFloat(char *name, float num);
void TJC_ShowWave(char* name, int ch, int val);
void TJC_ClearWave(char* name, int ch);


#endif /* __HMI_TJC_H */
