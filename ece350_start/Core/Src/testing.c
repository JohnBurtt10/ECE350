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
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!
#include <stddef.h>

/**
  * @brief  The application entry point.
  * @retval int
  */

void Test_Generate_Thread_Stack();
void Init_Task(TCB* task);
void Init_Task_2(TCB* task);
void Init_Task_3(TCB* task);
void Test_osCreateTask(); // Will fill all tcb's with the same tcb.
void Print_All_TCBs();

void Task_Yield_Exit(void);
void Task_Yield(void);
void Hello_World(void);
void Task_Create(void);

void Task1_Function(void);
void Task2_Function(void);

void Print_Free_List();

TCB task;

#ifdef DEBUG_ENABLE
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
	TCB task = {.ptask = &Task1_Function, .stack_size = 0x200};
	TCB task2 = {.ptask = &Task2_Function, .stack_size = 0x200};
	osCreateDeadlineTask(4, &task);
	osCreateDeadlineTask(11, &task2);

	osKernelStart();
	while(1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}
#endif

void Print_Free_List() {
    for (int i = 0; i < HEIGHT_OF_TREE; i++) {
        Block* currentBlock = buddyHeap.freeList[i];

        if (currentBlock == NULL) {
        	DEBUG_PRINTF("Block is null for i = %d.\r\n", i);
        } else {
        	while (currentBlock != NULL) {
        		DEBUG_PRINTF("Block Size: %d, Order: %d, Metadata: %x\r\n", currentBlock->size, MAX_ORDER - i, currentBlock);
        		currentBlock = currentBlock->next;
        	}
        }
    }
}

void Task_Create(void) {
	printf("task-%d\r\n", osGetTID());
	task.stack_size = 0x800;
	task.ptask = (void*) &Task_Yield;
	int status = osCreateTask(&task);
	printf("creating new task\r\nstack_high of newly created task: %p\r\n", (uint32_t*) kernelVariables.tcbList[task.tid].stack_high);
	if (!status)
		printf("osCreateTask Failed\r\n");
	else
		printf("PASS: task stack reused\r\n");
	osYield();
}

void Task1_Function(void){
	osSetDeadline(5, 2);
	osPeriodYield();
	return;
}

void Task2_Function(void){
	osPeriodYield();
	return;
}

void Task_Yield(void){
		printf("task-%d\r\n", osGetTID());
//		osPeriodYield();
		osYield();

	return;
}

void Task_Yield_Exit(void){
	printf("task-%d\r\n", osGetTID());
	printf("task exiting...\r\n");
	osTaskExit();
	return;
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

void anakin(void){
	DEBUG_PRINTF("  You underestimate my power Obi-Wan\r\n");

	osYield();

	DEBUG_PRINTF("  *Gets chopped in half and almost dies like a loser*\r\n");

	osTaskExit();
}

void obiwan(void) {
	DEBUG_PRINTF("Hello, there!\r\n");

	osTaskExit();
}


void luke(void){
	DEBUG_PRINTF("Dad?\r\n");

	osYield();

	DEBUG_PRINTF("Oh\r\n");

	osYield();
}

void Init_Task(TCB* task){
	task->ptask = (void*)&anakin;
	task->stack_high = (U32)Get_Thread_Stack(0x200);
	task->state = READY;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
}

void Init_Task_2(TCB* task){
	task->ptask = (void*)&obiwan;
	task->stack_high = (U32)Get_Thread_Stack(0x200);
	task->state = READY;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
}

void Init_Task_3(TCB* task){
	task->ptask = (void*)&luke;
	task->stack_high = (U32)Get_Thread_Stack(0x200);
	task->state = READY;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
}

void Test_osCreateTask() {
	// First test that all empty TCB's are written to.
	TCB tcbTests[MAX_TASKS];
	for (int i = 1; i < MAX_TASKS; i++){
		Init_Task(&tcbTests[i]);
		tcbTests[i].stack_size += i * 32;
		osCreateTask(&tcbTests[i]);
	}

//	// All TCB's should now be filled. Let's set one TCB to be dormant and attempt to update it
//	kernelVariables.tcbList[5].state = DORMANT;
//	kernelVariables.tcbList[10].state = DORMANT;
//	tcbTests[0].state = 10;
//	tcbTests[0].stack_size = 700;
//	osCreateTask(&tcbTests[0]); // Should pick tcbList[10]
//
//	tcbTests[0].state = 10;
//	tcbTests[0].stack_size = 900;
//	osCreateTask(&tcbTests[0]);// Should fail

	Print_All_TCBs();
}

void Print_All_TCBs() {
	for (int i = 0; i < MAX_TASKS; i++) {
		printf("TID: %d. State: %d. stack_high: %x. Stack size: %d\r\n",
				kernelVariables.tcbList[i].tid, kernelVariables.tcbList[i].state, kernelVariables.tcbList[i].stack_high, kernelVariables.tcbList[i].stack_size);
	}
}

