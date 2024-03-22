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
#include "motor.h"
#include "pid.h"
#include "zigbee_edc25.h"
#include "decision.h"
#include <math.h>
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
Position_edc25 tmp;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_UART4_Init();
  MX_UART5_Init();
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
	// zigbee
	zigbee_Init(&huart4);
	// PID
	PID_Init(&FLP_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&FRP_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&RLP_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&RRP_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&FLN_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&FRN_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&RLN_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);
	PID_Init(&RRN_Pid, 10.0f, 2.0f, 0.0f, 50000.0f);

	PID_Init(&xPid, 30.0f, 0.1f, 0.0f, 5000.0f);
	PID_Init(&yPid, 30.0f, 0.1f, 0.0f, 5000.0f);
	PID_Init(&anglePid, 0.5f, 2.0f, 10.0f, 100.0f);

	u1_printf("Hello\n");
	HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_Delay(300);
		getPosition(&now);
		nowGrid = pos2Grid(now);
		getPositionOpponent(&op);
		opGrid = pos2Grid(op);

		agility = getAgility();
		health = getHealth();
		strength=getStrength();
		maxHealth = getMaxHealth();
		wool = getWoolCount();
		emerald = getEmeraldCount();
		time = getGameTime();
    cd=agility<32?170-5*agility:10;

    
    
//		u1_printf("yaw:%f\n",GetYaw());
//		u1_printf("now:(%d,%d)\n", nowGrid.x, nowGrid.y);
		u1_printf("now pos:(%f,%f)\n", now.posx, now.posy);
//		u1_printf("goal:(%d,%d)\n", goalGrid.x, goalGrid.y);
		u1_printf("goal pos:(%f,%f)\n", goal.posx, goal.posy);
//		u1_printf("des:(%d,%d)\n", desGrid.x, desGrid.y);
		u1_printf("home:(%d,%d)\n", homeGrid.x, homeGrid.y);
		u1_printf("ophome:(%d,%d)\n", opHomeGrid.x, opHomeGrid.y);


		if (getGameStage() == READY)
			ready_func();
		else if (getGameStage() != FINISHED)
		{
            update_mine();
            statusChange();
			switch (status)
			{
				case init:
					u1_printf("init\n");
					init_func();
					break;
				case dead:
					u1_printf("dead\n");
					dead_func();
					break;
				case protect:
					u1_printf("protect\n");
					protect_func();
					break;
				case destroy:
					u1_printf("destroy\n");
					destroy_func();
					break;
				case attack:
					u1_printf("attack\n");
					attack_func();
					break;
				case mine:
					u1_printf("mine\n");
					mine_func();
					break;
				case get_wool:
					u1_printf("get wool\n");
					get_wool_func();
					break;
				case get_enhanced:
					u1_printf("get enhanced\n");
					get_enhanced_func();
					break;
				default:
					break;
			}
		}
		else
			;
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
//		u1_printf("motor\n");
		Mecanum_Pos(now, goal);
		Update_Pwm();
	}
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
