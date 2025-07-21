#ifndef __ENCODER_H
#define __ENCODER_H


#include "stdint.h"


void Encoder_R_Init(void);
void Encoder_L_Init(void);

int16_t encoder_L_get(void);
int16_t encoder_R_get(void);


#endif