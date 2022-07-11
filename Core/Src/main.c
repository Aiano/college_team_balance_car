/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "retarget.h"
#include "motor.h"
#include "encoder.h"
#include "mpu6050.h"
#include "uart_parser.h"
#include "pid.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern double target_speed;
extern double target_position;
extern double target_angular_speed;
char pause_state = 0;
char control_mode = 1; // 0 = speed control(default)    1 = position control
extern int16_t now_position;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
void change_control_val(char *cmd);

void change_angular_speed(char *cmd);

void change_P(char *cmd);

void change_I(char *cmd);

void change_D(char *cmd);

void change_P2(char *cmd);

void change_P3(char *cmd);

void change_target_degree(char *cmd);

void pause(char *cmd);

void resume(char *cmd);

void change_mode(char *cmd); // switch speed control and position control
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    HAL_Delay(1000);
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    /* USER CODE BEGIN 2 */
    RetargetInit(&huart3);
    motor_init();
    encoder_init();
    MPU6050_Init(&hi2c2);
    pid_init();

    uart_parser_init(&huart3);
    uart_parser_add_command('S', change_control_val, "change control value");
    uart_parser_add_command('R', change_angular_speed, "change angular speed");
    uart_parser_add_command('P', change_P, "change P");
    uart_parser_add_command('I', change_I, "change I");
    uart_parser_add_command('D', change_D, "change D");
    uart_parser_add_command('C', change_P2, "change P2");
    uart_parser_add_command('E', change_P3, "change P3");
    uart_parser_add_command('A', pause, "pause");
    uart_parser_add_command('B', resume, "resume");
    uart_parser_add_command('F', change_target_degree, "change target degree");
    uart_parser_add_command('G', change_mode, "change mode"); // switch speed mode and position mode

    /* USER CODE END 2 */

    /* Call init function for freertos objects (in freertos.c) */
    MX_FREERTOS_Init();
    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void change_control_val(char *cmd) {
    if (control_mode == 0) { // speed mode
        uart_parser_scalar(&target_speed, cmd);
    } else if (control_mode == 1) { // position mode
        uart_parser_scalar(&target_position, cmd);
    }

}

void change_angular_speed(char *cmd) {
    uart_parser_scalar(&target_angular_speed, cmd);
}

void change_P(char *cmd) {
    uart_parser_scalar(&pid_stand.Kp, cmd);

}

void change_I(char *cmd) {

}

void change_D(char *cmd) {
    uart_parser_scalar(&pid_stand.Kd, cmd);
}

void change_P2(char *cmd) {
    uart_parser_scalar(&pid_speed.Kp, cmd);
}

void change_P3(char *cmd) {
    uart_parser_scalar(&pid_spin.Kp, cmd);
}

void change_target_degree(char *cmd) {
    uart_parser_scalar(&target_degree, cmd);
}

void pause(char *cmd) {
    pause_state = 1;
}

extern TaskHandle_t BalanceTaskHandle;

void resume(char *cmd) {
    xTaskResumeFromISR(BalanceTaskHandle);
}

void change_mode(char *cmd) {
    if (control_mode == 1) {
        control_mode = 0; // speed control
        target_speed = 0;

        printf("Mode: speed control\n");
    } else if (control_mode == 0){
        control_mode = 1; // position control
        now_position = 0;
        target_speed = 0;

        printf("Mode: position control\n");
    }
}
/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM8 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM8) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
