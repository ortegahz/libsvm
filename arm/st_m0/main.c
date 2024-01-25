/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

#include "svm.h"
#include "data.h"

extern size_t strlen(const char *);

int print_null(const char *s, ...) { return 0; }

static int (*info)(const char *fmt, ...) = &printf;

struct svm_node *x;
struct svm_node *x_space_new;
double *sv_coef_new;
int max_nr_attr = 32;

struct svm_model *model;
int predict_probability = 0;

static char *line = NULL;
static int max_line_len;

const int a = 0;

void predict_hard_code()
{
  int correct = 0;
  int total = 0;
  double error = 0;
  double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

  int svm_type = 0;
  int nr_class = 2;
  double *prob_estimates = NULL;
  int j;

  int i = 0;
  double target_label, predict_label;
  char *idx, *val, *label, *endptr;
  int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

  printf("start ... \n");
  predict_label = svm_predict_hard_code(model, x);
  printf("end ... \n");
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("hello stm32 ! \n");
  /* USER CODE END 2 */

  printf("addr predict_probability -> %04x \n", &predict_probability);

  x_space_new = (struct svm_node *)x_space_hex;
  // for (int i = 0; i < TOTAL_SV_FEAT; i++)
  // {
  //   printf("x_space_new[%d] --> %d, %lf \n", i, (x_space_new + i)->index, (x_space_new + i)->value);
  // }

  sv_coef_new = (double *)sv_coef_hex;
  for (int i = 0; i < TOTAL_SV; i++)
  {
    printf("sv_coef_new[%d] --> %lf \n", i, sv_coef_new[i]);
  }

  model = svm_load_model_hard_code();
  printf("svm_load_model_hard_code() done ... \n");

  // x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
  // if (x == NULL)
  //   printf("x Malloc error !!! \n");
  x = (struct svm_node *)x_hex;
  printf("addr x -> %04x \n", x);

  for (int i = 0; i < (NUM_OF_FEATS + 1); i++)
  {
    printf("x[%d] --> %d, %lf \n", i, (x + i)->index, (x + i)->value);
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    printf("start ... \n");
    TIM2->CNT = 0x00;
    HAL_TIM_Base_Start(&htim2);
    predict_hard_code();
    float elapse = (float)TIM2->CNT * 1 / 64000000;
    HAL_TIM_Base_Stop(&htim2);
    printf("TIM2->CNT -> %d \n", TIM2->CNT);
    printf("elapse -> %f s \n", elapse);
    printf("end ... \n");
  }

  /* USER CODE END 3 */
  // free(x);
  // free(line);
  return 0;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
