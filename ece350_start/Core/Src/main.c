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
#include "common.h"
#include "k_task.h"
#include "k_mem.h"
#include "circular_queue.h"
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!

/**
  * @brief  The application entry point.
  * @retval int
  */
#ifndef DEBUG_ENABLE
int main(void)
{
  /* MCU Configuration: Don't change this or the whole chip won't work!*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* MCU Configuration is now complete. Start writing your code below this line */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  osKernelInit();

  // Main stack pointer
  // 0x0 pointer is a pointer that points to a pointer storing vector table.
  // Must dereference once to get the address of the start of the MSP stack.
  uint32_t* MSP_INIT_VAL = Get_MSP_INIT_VAL();
  printf("MSP Init is: %p\r\n", MSP_INIT_VAL); //note the %p to print apointer. It will be in hex

  //Testing svc_hjanlder call
  Trigger_System_Call(0);
  Create_Thread();
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
#endif
