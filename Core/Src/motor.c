/**
 * @file motor.c
 * @brief 直流电机驱动模块底层驱动
 */

#include "motor.h"
#include "tim.h"

/**
 * @brief motor初始化函数
 * @details
 * @return
 */
void motor_init() {
    // 初始化电机1的两个通道
    HAL_TIM_PWM_Start(MOTOR_1A_TIM, MOTOR_1A_CHANNEL);
    HAL_TIM_PWM_Start(MOTOR_1B_TIM, MOTOR_1B_CHANNEL);
    // 初始化电机2的两个通道
    HAL_TIM_PWM_Start(MOTOR_2A_TIM, MOTOR_2A_CHANNEL);
    HAL_TIM_PWM_Start(MOTOR_2B_TIM, MOTOR_2B_CHANNEL);
}

/**
 * @brief 设置电机1的驱动电压，正负表示方向
 * @param value -MOTOR_1_COUNTER_PERIOD ~ MOTOR_1_COUNTER_PERIOD
 */
void motor1_set_value(int16_t value) {
    if (value >= 0) {
#ifdef MOTOR_ELIMINATE_DEAD_TIME_VOLTAGE
        value += MOTOR_1_DEAD_TIME_VALUE;
#endif
        __HAL_TIM_SET_COMPARE(MOTOR_1A_TIM, MOTOR_1A_CHANNEL, value);
        __HAL_TIM_SET_COMPARE(MOTOR_1B_TIM, MOTOR_1B_CHANNEL, 0);
    } else {
#ifdef MOTOR_ELIMINATE_DEAD_TIME_VOLTAGE
        value -= MOTOR_1_DEAD_TIME_VALUE;
#endif
        __HAL_TIM_SET_COMPARE(MOTOR_1A_TIM, MOTOR_1A_CHANNEL, 0);
        __HAL_TIM_SET_COMPARE(MOTOR_1B_TIM, MOTOR_1B_CHANNEL, -value);
    }

    return;
}

/**
 * @brief 设置电机2的驱动电压，正负表示方向
 * @param value -MOTOR_2_COUNTER_PERIOD ~ MOTOR_2_COUNTER_PERIOD
 */
void motor2_set_value(int16_t value) {
    if (value >= 0) {
#ifdef MOTOR_ELIMINATE_DEAD_TIME_VOLTAGE
        value += MOTOR_2_DEAD_TIME_VALUE;
#endif
        __HAL_TIM_SET_COMPARE(MOTOR_2A_TIM, MOTOR_2A_CHANNEL, 0);
        __HAL_TIM_SET_COMPARE(MOTOR_2B_TIM, MOTOR_2B_CHANNEL, value);
    } else {
#ifdef MOTOR_ELIMINATE_DEAD_TIME_VOLTAGE
        value -= MOTOR_2_DEAD_TIME_VALUE;
#endif
        __HAL_TIM_SET_COMPARE(MOTOR_2A_TIM, MOTOR_2A_CHANNEL, -value);
        __HAL_TIM_SET_COMPARE(MOTOR_2B_TIM, MOTOR_2B_CHANNEL, 0);
    }

    return;
}

/** @brief motor去初始化函数
 *
 */
void motor_deinit() {
    // 去初始化电机1的两个通道
    HAL_TIM_PWM_Stop(MOTOR_1A_TIM, MOTOR_1A_CHANNEL);
    HAL_TIM_PWM_Stop(MOTOR_1B_TIM, MOTOR_1B_CHANNEL);
    // 去初始化电机2的两个通道
    HAL_TIM_PWM_Stop(MOTOR_2A_TIM, MOTOR_2A_CHANNEL);
    HAL_TIM_PWM_Stop(MOTOR_2B_TIM, MOTOR_2B_CHANNEL);
}
