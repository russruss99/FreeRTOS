/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */
/* USER CODE BEGIN Variables */
extern ADC_HandleTypeDef hadc1;
extern osMessageQueueId_t TempQueueHandle; // Connects to your thread-safe queue
extern osSemaphoreId_t ButtonSemHandle;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartTempTask(void *argument)
{
  /* USER CODE BEGIN StartTempTask */
  uint32_t raw_adc = 0;
  float temperature = 0.0f;

  const float VREF = 3.3f;
  const float AVG_SLOPE = 0.0025f;
  const float V25 = 0.76f;

  for(;;)
  {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
      raw_adc = HAL_ADC_GetValue(&hadc1);
      float voltage = ((float)raw_adc / 4095.0f) * VREF;
      temperature = ((voltage - V25) / AVG_SLOPE) + 25.0f;
    }
    HAL_ADC_Stop(&hadc1);

    // MILESTONE 3: Send the live temperature value to the queue.
    // Wait 0 ticks if the queue is full (don't block here).
    osMessageQueuePut(TempQueueHandle, &temperature, 0, 0);

    // Check the sensor once every 1000ms
    osDelay(1000);
  }
  /* USER CODE END StartTempTask */
}


/* USER CODE BEGIN Header_StartLedTask */
/**
* @brief Function implementing the LedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
  float rx_temperature = 0.0f;

  /* Infinite loop */
  for(;;)
  {
    // MILESTONE 3: Block indefinitely (osWaitForever) until data lands in the queue.
    if (osMessageQueueGet(TempQueueHandle, &rx_temperature, NULL, osWaitForever) == osOK)
    {
      // MILESTONE 1 (Heartbeat): Toggle the Green LED (PD12) to show the system is active
      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);

      // MILESTONE 3 (Threshold Check): If the chip is warm, turn on a warning.
      if (rx_temperature > 32.0f) // Set this slightly above your resting room temp
      {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);   // Turn ON Red LED (PD14)
      }
      else
      {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // Turn OFF Red LED (PD14)
      }
    }
  }
  /* USER CODE END StartLedTask */
}

void StartButtonTask(void *argument)
{
	  /* USER CODE BEGIN StartButtonTask */
	  /* Infinite loop */
for(;;)
{
    // This task sits frozen, consuming 0% CPU, waiting for the semaphore
	if (osSemaphoreAcquire(ButtonSemHandle, osWaitForever) == osOK)
	{
	// Toggle the Orange LED (PD13) to prove asynchronous execution
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		// Hardware Debouncing: A brief task delay to absorb mechanical noise
		// inside the button contacts so one click doesn't trigger multiple times.
		osDelay(200);
	}
    /* USER CODE BEGIN 3 */
/* USER CODE END FunctionPrototypes */
}
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	  // Check if the interrupt came from Pin 0 (the blue button pin)
	if (GPIO_Pin == GPIO_PIN_0)
{
	// Instantly release the semaphore to unlock our background button task
	osSemaphoreRelease(ButtonSemHandle);
}

}
/* USER CODE END Application */

