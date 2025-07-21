#ifndef __XUNJI_H
#define __XUNJI_H

void  Xunji_Init(void);
void read_sensors(int*read_senors);
void update_redSensor(void);
void trait(int* Motor_leftSetSpeed, int* Motor_rightSetSpeed, int base_v);
float calculate_line_deviation(int sensors[8]);
int constrain(int value, int min, int max);
#endif