/**
 * @file pid.h
 * @brief pid控制器
 */

#ifndef PID_H
#define PID_H

#include "main.h"
#include "mpu6050.h"
#include "motor.h"
#include "encoder.h"
#include <stdio.h>

#define DEAD_BAND

typedef struct {
    double Kp;
    double Ki;
    double Kd;

    double integral;
    double last_error;

    double max_integral;
    double min_integral;

    double max_u;
    double min_u;
} PID_Datatype;

extern PID_Datatype pid_stand;
extern PID_Datatype pid_speed;
extern PID_Datatype pid_spin;

extern double target_degree;
extern double target_speed;
extern double target_angular_speed;

double pid_get_u(PID_Datatype *pid, double target, double real);

void pid_init();

void pid_compute();

#endif //PID_H
