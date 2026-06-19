#ifndef __PWM_H
#define __PWM_H

void Servo_PWM_Init(void);
void Motor_PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare3(uint16_t Compare);

#endif