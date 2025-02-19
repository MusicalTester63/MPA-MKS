/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "sct.h"
#include <stdlib.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DEBOUNCE_DELAY 100  // čas v milisekundách na debounce
#define LED_TOGGLE_DELAY 100 // čas medzi prepínaním LED pri bliknutí

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t elapsed_time = 0;
uint32_t stop_time = 0;
uint16_t display_value = 0;
uint32_t start_time;
uint8_t led_timer = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static enum {
	running, stopped, reset
} state = stopped;

static enum {
	sw_running, sw_stopped, sw_error
} control_led_state = sw_stopped;

static uint32_t last_toggle_time = 0;  // čas posledného prepnutia LED
static uint8_t led_toggle_count = 0;   // počítadlo, koľkokrát bola LED prepnutá

static uint16_t debounce1 = 0xFFFF;
static uint16_t debounce2 = 0xFFFF;

uint8_t get_dot_position(uint32_t seconds) {
	if (seconds < 10) {
		return 1; // Bodka na prvom displeji
	} else if (seconds < 60) {
		return 2; // Bodka na druhom displeji
	} else if (seconds < 600) {
		return 1; //bodka na prvom displeji
	} else {
		return 2; // Bodka na druhom displeji
	}
}

void button_handler() {
	debounce1 = (debounce1 << 1) | HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin);
	debounce2 = (debounce2 << 1) | HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin);

	if (debounce1 == 0x7FFF) {
		if (state == stopped) {
			state = reset;

			for (uint8_t i = 0; i < 4; i++) {
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				HAL_Delay(100);
			}

			start_time = HAL_GetTick();
			stop_time = 0;
			elapsed_time = 0;

		} else if (state == running) {

			for (uint8_t i = 0; i < 4; i++) {
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				HAL_Delay(100);
			}
		}
	}
	if (debounce2 == 0x7FFF) {
		if (state == stopped) {
			state = running;

			start_time = HAL_GetTick() - stop_time; // Zachovanie kontinuity času
			stop_time = 0; // Resetovanie stop_time
		} else if (state == running) {
			state = stopped;
			stop_time = HAL_GetTick() - start_time; // Uloženie uplynutého času
		}
	}
}

void button_handler2() {
	debounce1 = (debounce1 << 1) | HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin);
	debounce2 = (debounce2 << 1) | HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin);

	if (debounce1 == 0x7FFF) {
		if (state == stopped) {
			state = reset;
			start_time = HAL_GetTick();
			stop_time = 0;
			elapsed_time = 0;

		} else if (state == running) {
			control_led_state = sw_error;
		}
	}
	if (debounce2 == 0x7FFF) {
		if (state == stopped) {
			state = running;
			control_led_state = sw_running;
			start_time = HAL_GetTick() - stop_time; // Zachovanie kontinuity času
			stop_time = 0; // Resetovanie stop_time
		} else if (state == running) {
			state = stopped;
			control_led_state = sw_stopped;
			stop_time = HAL_GetTick() - start_time; // Uloženie uplynutého času
		}
	}
}

void led_controller() {
	uint32_t current_time = HAL_GetTick();

	switch (control_led_state) {
	case sw_error:
		if (current_time - last_toggle_time >= LED_TOGGLE_DELAY) {
			last_toggle_time = current_time;
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
			if (led_toggle_count >= 5) {
				control_led_state = sw_running;
				last_toggle_time = 0;
				led_toggle_count = 0;
			} else {
				led_toggle_count++;
			}
		}
		break;
	case sw_running:
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);

		break;
	case sw_stopped:
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		break;
	}
}

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
	/* USER CODE BEGIN 2 */
	start_time = HAL_GetTick(); // Začiatočný čas
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		/* USER CODE BEGIN 3 */
		button_handler2();
		led_controller();

		switch (state) {

		case running:
			// Prepočítanie času
			elapsed_time = HAL_GetTick() - start_time + stop_time;
			uint32_t total_centiseconds = elapsed_time / 10;

			// Ak čas presiahol limit 15 minút
			if (total_centiseconds >= 90200) {
				state = stopped;
				break;
			}

			// Výpočet LED animácie
			led_timer = (total_centiseconds % 100) / 10;

			// Nastavenie hodnoty pre displej podľa času
			if (total_centiseconds < 1000) {
				// Menej ako 10 sekúnd
				display_value = total_centiseconds;

			} else if (total_centiseconds < 6000) {

				// Menej ako 60 sekúnd
				display_value = ((total_centiseconds / 100) * 10) + ((total_centiseconds % 100) / 10);

			} else if (total_centiseconds < 60000) {
				// Menej ako 10 minút
				uint8_t minutes = total_centiseconds / 6000;
				uint8_t seconds = (total_centiseconds / 100) % 60;
				uint8_t tens_of_seconds = seconds / 10;
				uint8_t units_of_seconds = seconds % 10;

				display_value = (minutes * 100) + (tens_of_seconds * 10)
						+ units_of_seconds;
			} else {
				// Nad 10 minút
				led_timer = ((total_centiseconds / 100) % 60) % 10;
				display_value = ((total_centiseconds / 6000) * 10)
						+ (((total_centiseconds / 100) / 10) % 6);
			}

			// Nastavenie bodky a aktualizácia displeja
			uint8_t dot_position = get_dot_position(total_centiseconds / 100);
			sct_value(display_value, dot_position, led_timer);
			break;

		case stopped:
			elapsed_time = stop_time;
			sct_value(display_value, dot_position, led_timer); // Aktualizácia displeja s bodkou
			break;

		case reset:
			start_time = HAL_GetTick();
			stop_time = 0;
			elapsed_time = 0;
			display_value = 0;
			led_timer = 0;
			state = stopped;
			break;
		}
		/* USER CODE END 3 */
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 38400;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LED1_Pin | LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
	LED2_Pin | SCT_NOE_Pin | SCT_CLK_Pin | SCT_SDI_Pin | SCT_NLA_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : S2_Pin S1_Pin */
	GPIO_InitStruct.Pin = S2_Pin | S1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : LED1_Pin LD2_Pin */
	GPIO_InitStruct.Pin = LED1_Pin | LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LED2_Pin SCT_NOE_Pin SCT_CLK_Pin SCT_SDI_Pin
	 SCT_NLA_Pin */
	GPIO_InitStruct.Pin = LED2_Pin | SCT_NOE_Pin | SCT_CLK_Pin | SCT_SDI_Pin
			| SCT_NLA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
