#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetLeftMotorPWM(uint16_t Compare);
void PWM_SetRightMotorPWM(uint16_t Compare);

#endif
