/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "uart_interface.h"
//#include "generalPurposeLibrary.h"
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
/* Const */

/* Variables */
volatile uint8_t *uartGlobalFlagIT = NULL;

volatile uint8_t uartPromptFlag = 0;
volatile uint8_t uartRx_flag = 0;

/* Buffer, arrays */
char uartTransmitBuffer[UART_BUFFER_SIZE_TX];
char uartReceiveBuffer[UART_BUFFER_SIZE_RX];

char uartTmpBuffer[UART_BUFFER_SIZE_RX];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
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

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_SPI1_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	sendString("\n\rPeriphery initialized.", &huart2);
	HAL_Delay(200);

	HAL_TIM_Base_Start(&htim1);
	sendString("\n\rTIM start counting.", &huart2);
	HAL_Delay(200);

	/* nrf24L01+ struct init*/
	nrfStruct_t *testNrfStruct;
	testNrfStruct = nRF_Init(&hspi1, &htim1, CSN_GPIO_Port, CSN_Pin, CE_GPIO_Port, CE_Pin);

	/* nrt Uart interface struct init */
	nRF_UartStruct_t *testUartStruct;
	testUartStruct = nRF_UartInit(testNrfStruct, &huart2);
	uartGlobalFlagIT = &(testUartStruct->uartIrqFlag);

	HAL_UART_Receive_IT(&huart2, (uint8_t*) (testUartStruct->uartRxBuffer), UART_READ_SIZE);
	HAL_Delay(200);
	sendString("\n\rUart set as listener.", &huart2);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* Begin UART receive interrupt  */
		if (*uartGlobalFlagIT) {
			/* copy characters to temporary buffer */
			memcpy(testUartStruct->uartTemporaryBuffer, testUartStruct->uartRxBuffer, UART_READ_SIZE);

			/* Main condition - data size */
			if (strlen(testUartStruct->uartTemporaryBuffer) >= MINIMUM_COMMAND_SIZE) {

				/* detect entrance to nRF and set prompt flag */
				if ((testUartStruct->uartPromptFlag) == 0) {
					nrfModeEnter(testUartStruct);
				}

				/* If nrf mode available check and execute command */
				if ((testUartStruct->uartPromptFlag) == 1) {
					/* Check command as exit command */
					if (!nrfModeExit(testUartStruct)) {
						/* If not exit command - check as others commands */
						/* Detect command */
						nrfModeCommand(testUartStruct);
					}
				}
				/* End of positive part of condtion */
			} else {
				sendString("\r\n#nRF command invalid.", &huart2);	//log
			}

			/* Reset buffers */
			resetChar(testUartStruct->uartRxBuffer, UART_BUFFER_SIZE_RX);
			resetChar(testUartStruct->uartTemporaryBuffer, UART_BUFFER_SIZE_TMP);
			*uartGlobalFlagIT = 0; /* Reset flag */
			/* Start UART listening */
			HAL_UART_Receive_IT(&huart2, (uint8_t*) (testUartStruct->uartRxBuffer), UART_READ_SIZE);
		}
		/* End UART receive interrupt  */

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
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
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
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	*uartGlobalFlagIT = 1;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
