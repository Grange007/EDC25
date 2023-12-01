/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "jy62.h"
#include "pid.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
	float x, y;
}PosStr;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_VELOCITY 60
#define MIN_VELOCITY 60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
PosStr now = {0, 0};
PosStr goal = {0, 0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Move(uint8_t id, float pwm);
void Update_Pwm();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
		Update_Pwm();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	// Output PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //FL
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); //FR
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); //RL
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); //RR
	// Measure
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL); //FL
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL); //FR
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL); //RL
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL); //RR
	// Update PWM
	HAL_TIM_Base_Start_IT(&htim6);
	// jy62
	jy62_Init(&huart3);
	SetBaud(115200);
	SetHorizontal();
	InitAngle();
	Calibrate();
	SleepOrAwake();
	// PID
	PID_Init(&FLPid, 10.0f, 2.0f, 0.0f);
	PID_Init(&FRPid, 10.0f, 2.0f, 0.0f);
	PID_Init(&RLPid, 10.0f, 2.0f, 0.0f);
	PID_Init(&RRPid, 10.0f, 2.0f, 0.0f);
	PID_Init(&xPid, 2.5f, 0.021f, 5.0f);
	PID_Init(&yPid, 2.0f, 0.015f, 4.5f);
	PID_Init(&anglePid, 1.1f, 0.04f, 0.01f);

	u1_printf("Hello\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
		float r = GetRoll();
		float p = GetPitch();
		float y = GetYaw();
		u1_printf("ROLL:%f, PITCH:%f, YAW:%f\n", r, p, y);
		r=p=y=0;
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Move(uint8_t id, float pwm)
{
	if (id == 1)
	{
		if (pwm > 0)
		{// FL +
			HAL_GPIO_WritePin(F_in3_GPIO_Port, F_in3_Pin, 0);
			HAL_GPIO_WritePin(F_in4_GPIO_Port, F_in4_Pin, 1);
		}
		else
		{// FL -
			HAL_GPIO_WritePin(F_in3_GPIO_Port, F_in3_Pin, 1);
			HAL_GPIO_WritePin(F_in4_GPIO_Port, F_in4_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, fabs(pwm));
	}
	else if (id == 2)
	{
		if (pwm > 0)
		{// FR +
			HAL_GPIO_WritePin(F_in1_GPIO_Port, F_in1_Pin, 0);
			HAL_GPIO_WritePin(F_in2_GPIO_Port, F_in2_Pin, 1);
		}
		else
		{// FR -
			HAL_GPIO_WritePin(F_in1_GPIO_Port, F_in1_Pin, 1);
			HAL_GPIO_WritePin(F_in2_GPIO_Port, F_in2_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, fabs(pwm));
	}
	else if (id == 3)
	{
		if (pwm > 0)
		{// RL +
			HAL_GPIO_WritePin(R_in3_GPIO_Port, R_in3_Pin, 0);
			HAL_GPIO_WritePin(R_in4_GPIO_Port, R_in4_Pin, 1);
		}
		else
		{// RL -
			HAL_GPIO_WritePin(R_in3_GPIO_Port, R_in3_Pin, 1);
			HAL_GPIO_WritePin(R_in4_GPIO_Port, R_in4_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, fabs(pwm));
	}
	else if (id == 4)
	{
		if (pwm > 0)
		{// RR +
			HAL_GPIO_WritePin(R_in1_GPIO_Port, R_in1_Pin, 0);
			HAL_GPIO_WritePin(R_in2_GPIO_Port, R_in2_Pin, 1);
		}
		else
		{// RR -
			HAL_GPIO_WritePin(R_in1_GPIO_Port, R_in1_Pin, 1);
			HAL_GPIO_WritePin(R_in2_GPIO_Port, R_in2_Pin, 0);
		}
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, fabs(pwm));
	}
}

void Update_Pwm()
{

	float xVel = PID_Cal(&xPid, now.x, goal.x);
	float yVel = PID_Cal(&yPid, now.y, goal.y);
//	if (xVel > MAX_VELOCITY)
//		xVel = MAX_VELOCITY;
//	if (xVel < MIN_VELOCITY)
//		xVel = MIN_VELOCITY;
//	if (yVel > MAX_VELOCITY)
//		yVel = MAX_VELOCITY;
//	if (yVel < MIN_VELOCITY)
//		yVel = MIN_VELOCITY;

//	u1_printf("xVel=%f, yVel=%f\n", xVel, yVel);


//	float angleErr = GetYaw();
//	float angleErr = 0;
//	if (angleErr > 180.0f)
//		angleErr -= 360.0f;
//	if (angleErr < -180.0f)
//		angleErr += 360.0f;
//	float aVel = PID_Cal(&anglePid, angleErr, 0);
	float aVel = 0;
//	if (aVel > 40)
//		aVel = 40.0f;
//	if (aVel < -40)
//		aVel = -40.0f;

	int FLCnt = __HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SetCounter(&htim2, 0);
	if (FLCnt > 32767)
		FLCnt = 65535 - FLCnt;
	else
		FLCnt = 0 - FLCnt;
	float FLNow = 1.0 * FLCnt / 10.8;
	float FLPwm = PID_Cal(&FLPid, FLNow, -aVel + xVel - yVel);
	Move(1, FLPwm);

	int FRCnt = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SetCounter(&htim3, 0);
	if (FRCnt > 32767)
		FRCnt = FRCnt - 65535;
	else
		FRCnt = FRCnt - 0;
	float FRNow = 1.0 * FRCnt / 10.8;
	float FRPwm = PID_Cal(&FRPid, FRNow, -aVel + xVel + yVel);
	Move(2, FRPwm);

	int RLCnt = __HAL_TIM_GET_COUNTER(&htim4);
	__HAL_TIM_SetCounter(&htim4, 0);
	if (RLCnt > 32767)
		RLCnt = 65535 - RLCnt;
	else
		RLCnt = 0 - RLCnt;
	float RLNow = 1.0 * RLCnt / 10.8;
	float RLPwm = PID_Cal(&RLPid, RLNow, aVel + xVel + yVel);
	Move(3, RLPwm);

	int RRCnt = __HAL_TIM_GET_COUNTER(&htim5);
	__HAL_TIM_SetCounter(&htim5, 0);
	if (RRCnt > 32767)
		RRCnt = RRCnt - 65535;
	else
		RRCnt = RRCnt - 0;
	float RRNow = 1.0 * RRCnt / 10.8;
	float RRPwm = PID_Cal(&RRPid, RRNow, aVel + xVel - yVel);
	Move(4, RRPwm);

//	u1_printf("FLNow=%f FLPwm=%f ", FLNow, FLPwm);
//	u1_printf("FRNow=%f FRPwm=%f\n", FRNow, FRPwm);
//	u1_printf("RLNow=%f RLPwm=%f ", RLNow, RLPwm);
//	u1_printf("RRNow=%f RRPwm=%f\n", RRNow, RRPwm);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
