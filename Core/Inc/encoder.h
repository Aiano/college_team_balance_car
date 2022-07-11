/**
 * @file encoder.h
 * @brief 读取电机编码器速度
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "main.h"
#include "tim.h"
#include <stdio.h>

#define ENCODER_1_TIM &htim2
#define ENCODER_2_TIM &htim3
//#define PERIOD_TIM &htim8

void encoder_init();
void encoder_callback();
void encoder_get_speed(int16_t *motor1, int16_t *motor2);

#endif /* INC_ENCODER_H_ */
