/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include <stdio.h>
#include <math.h>
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Constants for NTC Thermistor & Voltage Divider Setup */
#define V_REF         3.3f      // STM32 ADC Reference Voltage (3.3V)
#define ADC_MAX       4095.0f   // 12-bit ADC Max Value (2^12 - 1)
#define R_FIXED       10000.0f  // Fixed Resistor Value (10,000 Ohms)

/* Thermistor Parameters (Standard 10k NTC parameters) */
#define R_25          10000.0f  // Resistance at 25°C (10,000 Ohms)
#define BETA          3940.0f   // Beta Value (Check your NTC datasheet, usually 3950)
#define T_25_K        298.15f   // 25°C converted to Kelvin (25 + 273.15)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  uint32_t adc_value = 0;
	      float voltage = 0.0f;
	      float current_mA = 0.0f;
	      float r_ntc = 0.0f;
	      float temp_kelvin = 0.0f;
	      float temp_celsius = 0.0f;

	      /* 1. Start ADC Conversion */
	      HAL_ADC_Start(&hadc1);

	      /* 2. Poll ADC for completion */
	      if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
	      {
	          adc_value = HAL_ADC_GetValue(&hadc1);
	      }
	      HAL_ADC_Stop(&hadc1);

	      /* 3. Calculate Voltage at ADC Pin (0V to 3.3V) */
	      voltage = ((float)adc_value * V_REF) / ADC_MAX;

	      /* 4. Calculate Circuit Current (in Milliamperes)
	            Current = V_fixed / R_fixed = (V_REF - V_NTC) / R_fixed */
	      float v_fixed = V_REF - voltage;
	      current_mA = (v_fixed / R_FIXED) * 1000.0f;

	      /* 5. Calculate Thermistor Resistance (R_NTC) */
	      if (voltage > 0.0f)
	      {
	          r_ntc = (voltage * R_FIXED) / v_fixed;
	      }

	      /* 6. Calculate Temperature using Beta Parameter Equation */
	      if (r_ntc > 0.0f)
	      {
	          // 1/T = 1/T0 + (1/B) * ln(R/R0)
	          temp_kelvin = 1.0f / ((1.0f / T_25_K) + (logf(r_ntc / R_25) / BETA));
	          temp_celsius = temp_kelvin - 273.15f;
	      }

	      /* 7. Format string and send over UART (huart2) */
	      char msg[256];
	      int len = snprintf(msg, sizeof(msg),"ADC: %lu | V: %.2f V | I: %.3f mA | R_NTC: %.1f Ohm | Temp: %.2f C\r\n",
	                         (unsigned long)adc_value,  voltage, current_mA, r_ntc, temp_celsius);
          if(len>0)
          {
	      HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)len,  HAL_MAX_DELAY);
          }
	      HAL_ADC_Stop(&hadc1);

	      HAL_Delay(500); // Read every 1 second

	      /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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































//#include "main.h"
//#include <stdio.h>
//
///* Private variables ---------------------------------------------------------*/
//
//ADC_HandleTypeDef hadc1;
//UART_HandleTypeDef huart2;
//
///* Private function prototypes -----------------------------------------------*/
//
//void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
//static void MX_ADC1_Init(void);
//static void MX_USART2_UART_Init(void);
//
//void Error_Handler(void);
//
//
///* Main ----------------------------------------------------------------------*/
//
//int main(void)
//{
//	    uint32_t adc_value;
//	    float voltage;
//	    float sensitivity = 0.066f; // 0.066 V/A for 30A model
//	    float current;
//
//	    HAL_Init();
//	    SystemClock_Config();
//
//	    MX_GPIO_Init();
//	    MX_ADC1_Init();
//	    MX_USART2_UART_Init();
//
//	    /* Infinite loop */
//	    while (1)
//	    {
//	        /* Start ADC conversion */
//	        HAL_ADC_Start(&hadc1);
//
//	        /* Wait for conversion to complete */
//	        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//
//	        /* Read ADC value: 0 to 4095 */
//	        adc_value = HAL_ADC_GetValue(&hadc1);
//
//	        /* Convert ADC value to voltage in Volts */
//	        voltage = ((float)adc_value * 3.3f*2) / 4095.0f;
//
//	        /* Calculate current using sensor formula */
//	        current = (voltage - 2.5f) / sensitivity;
//
//	        /* Send ADC value through UART */
//	        char message[100];
//	        int length = snprintf(message,sizeof(message), "ADC Value: %lu | Voltage: %.2f V | Current: %.2f A\r\n",
//	                             (unsigned long)adc_value, voltage, current);
//
//	        HAL_UART_Transmit(&huart2, (uint8_t *)message, length, HAL_MAX_DELAY);
//
//	        /* Stop ADC */
//	        HAL_ADC_Stop(&hadc1);
//
//            /*Wait 500 ms*/
//	        HAL_Delay(500);
//	    }
//	}
//
///* System Clock Configuration -----------------------------------------------*/
//
//void SystemClock_Config(void)
//{
//    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//    /* Enable power clock */
//    __HAL_RCC_PWR_CLK_ENABLE();
//
//    /* Voltage scaling */
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
//
//    /* Configure HSI */
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//
//    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//    {
//        Error_Handler();
//    }
//
//    /* Configure clocks */
//    RCC_ClkInitStruct.ClockType =
//        RCC_CLOCKTYPE_HCLK |
//        RCC_CLOCKTYPE_SYSCLK |
//        RCC_CLOCKTYPE_PCLK1 |
//        RCC_CLOCKTYPE_PCLK2;
//
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//
//    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}
//
//
///* ADC1 Initialization ------------------------------------------------------*/
//
//static void MX_ADC1_Init(void)
//{
//    ADC_ChannelConfTypeDef sConfig = {0};
//
//    /* ADC configuration */
//    hadc1.Instance = ADC1;
//
//    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
//    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
//
//    /* Single channel */
//    hadc1.Init.ScanConvMode = DISABLE;
//
//    /* We manually start each conversion */
//    hadc1.Init.ContinuousConvMode = DISABLE;
//
//    hadc1.Init.DiscontinuousConvMode = DISABLE;
//
//    hadc1.Init.ExternalTrigConvEdge =
//        ADC_EXTERNALTRIGCONVEDGE_NONE;
//
//    hadc1.Init.ExternalTrigConv =
//        ADC_SOFTWARE_START;
//
//    hadc1.Init.DataAlign =
//        ADC_DATAALIGN_RIGHT;
//
//    hadc1.Init.NbrOfConversion = 1;
//
//    hadc1.Init.DMAContinuousRequests = DISABLE;
//
//    hadc1.Init.EOCSelection =
//        ADC_EOC_SINGLE_CONV;
//
//    if (HAL_ADC_Init(&hadc1) != HAL_OK)
//    {
//        Error_Handler();
//    }
//
//    /* ADC Channel 0 = PA0 */
//    sConfig.Channel = ADC_CHANNEL_0;
//
//    sConfig.Rank = 1;
//
//    sConfig.SamplingTime =
//        ADC_SAMPLETIME_3CYCLES;
//
//    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}
//
//
///* USART2 Initialization ----------------------------------------------------*/
//
//static void MX_USART2_UART_Init(void)
//{
//    huart2.Instance = USART2;
//
//    huart2.Init.BaudRate = 115200;
//
//    huart2.Init.WordLength =
//        UART_WORDLENGTH_8B;
//
//    huart2.Init.StopBits =
//        UART_STOPBITS_1;
//
//    huart2.Init.Parity =
//        UART_PARITY_NONE;
//
//    huart2.Init.Mode =
//        UART_MODE_TX_RX;
//
//    huart2.Init.HwFlowCtl =
//        UART_HWCONTROL_NONE;
//
//    huart2.Init.OverSampling =
//        UART_OVERSAMPLING_16;
//
//    if (HAL_UART_Init(&huart2) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}
//
//
///* GPIO Initialization ------------------------------------------------------*/
//
//static void MX_GPIO_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//    /* Enable GPIOA clock */
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//
//
//    /* -------------------------------------------------------
//       PA0 = ADC1_IN0
//       ------------------------------------------------------- */
//
//    GPIO_InitStruct.Pin = GPIO_PIN_0;
//
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//
//    /* -------------------------------------------------------
//       PA2 = USART2_TX
//       PA3 = USART2_RX
//       ------------------------------------------------------- */
//
//    GPIO_InitStruct.Pin =
//        GPIO_PIN_2 | GPIO_PIN_3;
//
//    GPIO_InitStruct.Mode =
//        GPIO_MODE_AF_PP;
//
//    GPIO_InitStruct.Pull =
//        GPIO_NOPULL;
//
//    GPIO_InitStruct.Speed =
//        GPIO_SPEED_FREQ_VERY_HIGH;
//
//    GPIO_InitStruct.Alternate =
//        GPIO_AF7_USART2;
//
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//}
//
//
///* Error Handler -------------------------------------------------------------*/
//
//void Error_Handler(void)
//{
//    __disable_irq();
//
//    while (1)
//    {
//    }
//}
//
//
//#ifdef USE_FULL_ASSERT
//
//void assert_failed(uint8_t *file, uint32_t line)
//{
//    /* User can add implementation here */
//}
//
//#endif
