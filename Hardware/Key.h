#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY_1  1
#define KEY_2  2

#define KEY_DOWN 1
#define KEY_UP   0

void Key_Init(void);
void Key_Scan(void);
uint8_t Key_Get(uint8_t key_id);

#endif
