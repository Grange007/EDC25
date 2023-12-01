/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define Encoder_rra_Pin GPIO_PIN_0
#define Encoder_rra_GPIO_Port GPIOA
#define Encoder_rrb_Pin GPIO_PIN_1
#define Encoder_rrb_GPIO_Port GPIOA
#define Encoder_fra_Pin GPIO_PIN_6
#define Encoder_fra_GPIO_Port GPIOA
#define Encoder_frb_Pin GPIO_PIN_7
#define Encoder_frb_GPIO_Port GPIOA
#define F_in4_Pin GPIO_PIN_12
#define F_in4_GPIO_Port GPIOB
#define F_in3_Pin GPIO_PIN_13
#define F_in3_GPIO_Port GPIOB
#define F_in2_Pin GPIO_PIN_14
#define F_in2_GPIO_Port GPIOB
#define F_in1_Pin GPIO_PIN_15
#define F_in1_GPIO_Port GPIOB
#define R_in4_Pin GPIO_PIN_6
#define R_in4_GPIO_Port GPIOC
#define R_in3_Pin GPIO_PIN_7
#define R_in3_GPIO_Port GPIOC
#define R_in2_Pin GPIO_PIN_8
#define R_in2_GPIO_Port GPIOC
#define R_in1_Pin GPIO_PIN_9
#define R_in1_GPIO_Port GPIOC
#define PWM_FL_Pin GPIO_PIN_8
#define PWM_FL_GPIO_Port GPIOA
#define PWM_FR_Pin GPIO_PIN_9
#define PWM_FR_GPIO_Port GPIOA
#define PWM_RL_Pin GPIO_PIN_10
#define PWM_RL_GPIO_Port GPIOA
#define PWM_RR_Pin GPIO_PIN_11
#define PWM_RR_GPIO_Port GPIOA
#define Encoder_fla_Pin GPIO_PIN_15
#define Encoder_fla_GPIO_Port GPIOA
#define Encoder_flb_Pin GPIO_PIN_3
#define Encoder_flb_GPIO_Port GPIOB
#define Encoder_rla_Pin GPIO_PIN_6
#define Encoder_rla_GPIO_Port GPIOB
#define Encoder_rlb_Pin GPIO_PIN_7
#define Encoder_rlb_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
