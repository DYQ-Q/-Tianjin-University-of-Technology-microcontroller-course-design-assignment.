#include "stm32f10x.h"     
#include "PWM.h"
#include "delay.h"

void Servo_Init(void)
{
  Servo_PWM_Init(); //初始化PWM
}

void Servo_SetAngle(uint8_t angle)
{
  PWM_SetCompare1(angle);
}