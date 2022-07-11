/**
 * @file motor.h
 * @brief 直流电机驱动模块底层驱动
 */

#ifndef COLLEGE_TEAM_BALANCE_CAR_MOTOR_H
#define COLLEGE_TEAM_BALANCE_CAR_MOTOR_H

#include "main.h"

// 消除电机死区电压模式，若要关闭请注释
//
// #define MOTOR_ELIMINATE_DEAD_TIME_VOLTAGE

// 电机1相关设置
#define MOTOR_1A_TIM &htim1
#define MOTOR_1A_CHANNEL TIM_CHANNEL_1
#define MOTOR_1B_TIM &htim1
#define MOTOR_1B_CHANNEL TIM_CHANNEL_4
#define MOTOR_1_DEAD_TIME_VALUE 40 // 40
#define MOTOR_1_COUNTER_PERIOD 1000

// 电机2相关设置
#define MOTOR_2A_TIM &htim4
#define MOTOR_2A_CHANNEL TIM_CHANNEL_3
#define MOTOR_2B_TIM &htim4
#define MOTOR_2B_CHANNEL TIM_CHANNEL_4
#define MOTOR_2_DEAD_TIME_VALUE 45  // 40
#define MOTOR_2_COUNTER_PERIOD 1000



void motor_init();
void motor1_set_value(int16_t value);
void motor2_set_value(int16_t value);

void motor_deinit();


#endif //COLLEGE_TEAM_BALANCE_CAR_MOTOR_H
