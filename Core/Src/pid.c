/**
 * @file pid.c
 * @brief pid控制器
 */
#include <stdlib.h>
#include <math.h>
#include "pid.h"


PID_Datatype pid_stand; // 直立环
PID_Datatype pid_speed; // 速度环
PID_Datatype pid_spin; // 转向环
PID_Datatype pid_position; // 位置环

double target_degree = -3.3; // 目标角度
double target_speed = 0; // 目标速度
double target_angular_speed = 0; // 目标角速度 CW 顺时针
double target_position = 0; // 目标位置

// 各个环pid输出控制量
double u1 = 0; // 直立环
double u2 = 0; // 速度环
double u3 = 0; // 转向环
double u4 = 0; // 位置环

// MPU6050
MPU6050_t MPU6050;
double Z = 0; // Z 轴积分值，有累计误差

// 编码器
int16_t motor1, motor2;
int16_t avg_speed;
int16_t now_position;

// 平衡任务运行状态
extern char pause_state;

// 控制模式
extern char control_mode; // 0 = speed control    1 = position control

/**
 * @brief 获取pid的控制量u
 * @param pid
 * @param target
 * @param real
 * @return u
 */
double pid_get_u(PID_Datatype *pid, double target, double real) {
    double error = target - real; // 误差
    pid->integral += error; // 积分误差
    if (pid->integral > pid->max_integral) { // 设置积分误差的上下限，防止过大或过小
        pid->integral = pid->max_integral;
    } else if (pid->integral < pid->min_integral) {
        pid->integral = pid->min_integral;
    }
    double differential = error - pid->last_error; // 差分误差
    //printf("%f\n", differential);

    double u = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * differential;  // 控制量
    if (u > pid->max_u) { // 设置控制量u的上下限，防止过大或过小
        u = pid->max_u;
    } else if (u < pid->min_u) {
        u = pid->min_u;
    }

    pid->last_error = error; // 记录下本轮误差供下次差分使用

    return u;
}

void pid_init() {
    // stand loop
    pid_stand.Kp = 105;// 100
    pid_stand.Ki = 0;
    pid_stand.Kd = 1000; // 1000

    pid_stand.max_integral = 0;
    pid_stand.min_integral = 0;
    pid_stand.max_u = 900;
    pid_stand.min_u = -900;

    pid_stand.integral = 0;
    pid_stand.last_error = 0;

    // speed loop
    pid_speed.Kp = -50;// -50
    pid_speed.Ki = 0; // 0
    pid_speed.Kd = 0; //

    pid_speed.max_integral = 900;
    pid_speed.min_integral = -900;
    pid_speed.max_u = 900;
    pid_speed.min_u = -900;

    pid_speed.integral = 0;
    pid_speed.last_error = 0;

    // spin loop
    pid_spin.Kp = 100; // 80
    pid_spin.Ki = 0;
    pid_spin.Kd = 0; //

    pid_spin.max_integral = 500;
    pid_spin.min_integral = -500;
    pid_spin.max_u = 500;
    pid_spin.min_u = -500;

    pid_spin.integral = 0;
    pid_spin.last_error = 0;

    // 位置环
    pid_position.Kp = 0.01;
    pid_position.Ki = 0;
    pid_position.Kd = 0;

    pid_position.max_integral = 500;
    pid_position.min_integral = -500;
    pid_position.max_u = 5;
    pid_position.min_u = -5;

    pid_position.integral = 0;
    pid_position.last_error = 0;
}

void pid_compute() {
    /* 读取MPU6050的数据，并转换为欧拉角 */
    MPU6050_Read_All(&hi2c2, &MPU6050);
    Z += MPU6050.Gz * 0.01; // IMU Z轴，对时间积分，有累计误差

    /* 读取编码器速度 */
    encoder_get_speed(&motor1, &motor2); // 两轮的速度
    avg_speed = (motor1 + motor2) / 2; // 计算两轮平均速度
    now_position += avg_speed; // 积分计算当前位置

    /* 超速保护 */
    if (abs(motor1) > 60 || abs(motor2) > 60) {
        printf("Pause: out of speed!\n");
        pause_state = 1;
    }

    /* 倾倒保护 */
    if (fabs(MPU6050.KalmanAngleX > 28)) {
        printf("Pause: out of angle!\n");
        pause_state = 1;
    }

    // PID 计算
    u1 = pid_get_u(&pid_stand, target_degree, MPU6050.KalmanAngleX); // 直立环
    //if (u1 < 50 && u1 > -50) u1 = 0; // 死区, 增加稳定性

    if (control_mode == 1) { // position control
        target_speed = pid_get_u(&pid_position, target_position, now_position); // 位置环
    }
    u2 = pid_get_u(&pid_speed, target_speed, avg_speed); // 速度环

    u3 = pid_get_u(&pid_spin, target_angular_speed, motor1 - motor2); // 转向环


    motor1_set_value((int16_t) (u1 + u2 + u3));
    motor2_set_value((int16_t) (u1 + u2 - u3));

    // printf("%d,%d,%d\n", (int) MPU6050.KalmanAngleX, now_position, (int) target_position);
}

