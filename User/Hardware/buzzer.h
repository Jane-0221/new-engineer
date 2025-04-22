#ifndef __BUZZER_TASK_H
#define __BUZZER_TASK_H

#include "tim.h"
#include "gpio.h"

/* ��ģ�����ⲿ�ṩ�ĺ궨�� -------------------------------------------------*/
#define BUZZER_TIM  htim12  //240hz
#define BUZZER_CHANNEL TIM_CHANNEL_2

void Buzzer_init();
void Buzzer_set_frequency(float fre);
void Buzzer_on();
void Buzzer_off();
#endif
