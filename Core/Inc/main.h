/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IMU_INT_Pin GPIO_PIN_13
#define IMU_INT_GPIO_Port GPIOC
#define ADC3_IN10_M2_B_Pin GPIO_PIN_0
#define ADC3_IN10_M2_B_GPIO_Port GPIOC
#define ADC2_IN11_M2_A_Pin GPIO_PIN_1
#define ADC2_IN11_M2_A_GPIO_Port GPIOC
#define ADC2_IN12_M1_A_Pin GPIO_PIN_2
#define ADC2_IN12_M1_A_GPIO_Port GPIOC
#define ADC3_IN13_M1_B_Pin GPIO_PIN_3
#define ADC3_IN13_M1_B_GPIO_Port GPIOC
#define M2_PWM_C_Pin GPIO_PIN_1
#define M2_PWM_C_GPIO_Port GPIOA
#define M2_PWM_B_Pin GPIO_PIN_2
#define M2_PWM_B_GPIO_Port GPIOA
#define M2_PWM_A_Pin GPIO_PIN_3
#define M2_PWM_A_GPIO_Port GPIOA
#define ADC1_IN4_VCC_Pin GPIO_PIN_4
#define ADC1_IN4_VCC_GPIO_Port GPIOA
#define M1_PWM_A_Pin GPIO_PIN_6
#define M1_PWM_A_GPIO_Port GPIOA
#define M1_PWM_B_Pin GPIO_PIN_7
#define M1_PWM_B_GPIO_Port GPIOA
#define Key_Pin_Pin GPIO_PIN_5
#define Key_Pin_GPIO_Port GPIOC
#define Key_Pin_EXTI_IRQn EXTI9_5_IRQn
#define M1_PWM_C_Pin GPIO_PIN_0
#define M1_PWM_C_GPIO_Port GPIOB
#define M2_Encoder_SCL_Pin GPIO_PIN_10
#define M2_Encoder_SCL_GPIO_Port GPIOB
#define M2_Encoder_SDA_Pin GPIO_PIN_11
#define M2_Encoder_SDA_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_13
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_14
#define OLED_SDA_GPIO_Port GPIOB
#define Led_Pin_Pin GPIO_PIN_8
#define Led_Pin_GPIO_Port GPIOC
#define COMMEND_TX_Pin GPIO_PIN_9
#define COMMEND_TX_GPIO_Port GPIOA
#define COMMEND_RX_Pin GPIO_PIN_10
#define COMMEND_RX_GPIO_Port GPIOA
#define M1_Encoder_SDA_Pin GPIO_PIN_10
#define M1_Encoder_SDA_GPIO_Port GPIOC
#define M1_Encoder_SCL_Pin GPIO_PIN_11
#define M1_Encoder_SCL_GPIO_Port GPIOC
#define IMU_MODE_Pin GPIO_PIN_12
#define IMU_MODE_GPIO_Port GPIOC
#define IMU_SDA_Pin GPIO_PIN_8
#define IMU_SDA_GPIO_Port GPIOB
#define IMU_SCL_Pin GPIO_PIN_9
#define IMU_SCL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
