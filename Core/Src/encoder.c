/**
 * @file encoder.c
 * @brief 读取电机编码器速度
 */

#include "encoder.h"

int16_t motor1_speed;
int16_t motor2_speed;

/**
 * @brief 初始化编码器和中断的TIM定时器
 */
void encoder_init() {
    //Init TIM
    HAL_TIM_Encoder_Start(ENCODER_1_TIM, TIM_CHANNEL_ALL); // encoderA
    HAL_TIM_Encoder_Start(ENCODER_2_TIM, TIM_CHANNEL_ALL); // encoderB
//	HAL_TIM_Base_Start_IT(PERIOD_TIM); // timer:generate 10ms period
}

/**
 * @brief 计算转速，每10ms调用一次本函数，可以在RTOS中开一个任务循环调用本函数
 * @param period
 */
void encoder_callback() { // tim callback function
//	if (period == PERIOD_TIM) { // PERIOD_TIM: generate 10ms signal

    int16_t counts1 = (int16_t) __HAL_TIM_GET_COUNTER(ENCODER_1_TIM); // ENCODER_A: counts the AB encoder
    int16_t counts2 = (int16_t) __HAL_TIM_GET_COUNTER(ENCODER_2_TIM); // ENCODER_B: counts the AB encoder
    motor1_speed = counts1;
    motor2_speed = counts2;
    __HAL_TIM_SET_COUNTER(ENCODER_1_TIM, 0); // ENCODER_A: clear
    __HAL_TIM_SET_COUNTER(ENCODER_2_TIM, 0); // ENCODER_B: clear
//	}
}

/**
 * @brief 使用指针获取电机转速
 * @param motor1
 * @param motor2
 */
void encoder_get_speed(int16_t *motor1, int16_t *motor2) {
    *motor1 = motor1_speed; // 加负号是为了统一两个电机的方向
    *motor2 = -motor2_speed;

    return;
}
