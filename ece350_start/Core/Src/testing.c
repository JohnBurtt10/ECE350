/*
 * testing.c
 *
 *  Created on: May 24, 2024
 *      Author: Ethan Romero
 */


#include "main.h"
#include "common.h"
#include "k_task.h"
#include "k_mem.h"
#include "circular_queue.h"
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!
#include <stddef.h>

/**
  * @brief  The application entry point.
  * @retval int
  */

#ifdef DEBUG_ENABLE
void Test_Generate_Thread_Stack();
void Init_Task(TCB* task);
void Test_osCreateTask(); // Will fill all tcb's with the same tcb.
void Print_All_TCBs();

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
	printf("IN DEBUGGING MODE. TO DISABLE GO TO: common.h AND COMMENT OUT #define DEBUG_ENABLE\r\n");
	osKernelInit();

	// Main stack pointer
	// 0x0 pointer is a pointer that points to a pointer storing vector table.
	// Must dereference once to get the address of the start of the MSP stack.
	uint32_t* MSP_INIT_VAL = Get_MSP_INIT_VAL();
	printf("MSP Init is: %p\r\n", MSP_INIT_VAL); //note the %p to print apointer. It will be in hex

//	Test_Generate_Thread_Stack();
	Test_osCreateTask();

	while (1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

void Test_Generate_Thread_Stack(){
	Get_Thread_Stack(0x200); // Should be 0x20017c00
	kernelVariables.tcbList[0].stack_size = 0x200;

	Get_Thread_Stack(0x500); // Should be 0x20017A00
	kernelVariables.tcbList[1].stack_size = 0x500;

	Get_Thread_Stack(0x400); // Should be 0x20017500
	kernelVariables.tcbList[2].stack_size = 0x500;

	Get_Thread_Stack(0x100); // Should fail

	Get_Thread_Stack(0x4000); // Should fail

	osInitTCBArray(); // Reset TCBs
}

void Init_Task(TCB* task){
	task->ptask = NULL;
	task->stack_high = 0x0;
	task->state = DORMANT;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
}

void Test_osCreateTask() {
	// First test that all empty TCB's are written to.
	TCB tcbTests[MAX_TASKS];
	for (int i = 0; i < MAX_TASKS; i++){
		Init_Task(&tcbTests[i]);
		tcbTests[i].stack_size += i * 32;
		osCreateTask(&tcbTests[i]);
	}

	// All TCB's should now be filled. Let's set one TCB to be dormant and attempt to update it
	kernelVariables.tcbList[5].state = DORMANT;
	kernelVariables.tcbList[10].state = DORMANT;
	tcbTests[0].state = 10;
	tcbTests[0].stack_size = 700;
	osCreateTask(&tcbTests[0]); // Should pick tcbList[10]

	tcbTests[0].state = 10;
	tcbTests[0].stack_size = 900;
	osCreateTask(&tcbTests[0]);// Should fail

	Print_All_TCBs();
}

void Print_All_TCBs() {
	for (int i = 0; i < MAX_TASKS; i++) {
		printf("TID: %d. State: %d. stack_high: %x. Stack size: %d\r\n",
				kernelVariables.tcbList[i].tid, kernelVariables.tcbList[i].state, kernelVariables.tcbList[i].stack_high, kernelVariables.tcbList[i].stack_size);
	}
}

#endif
