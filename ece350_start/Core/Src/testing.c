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
void Print_Free_List();

void Task_Yield_Exit(void);
void Task_Yield(void);
void Task_Create(void);

TCB task;

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
	k_mem_init();

//	TCB tasks[8];
//
//	for (int i = 0; i < 8; i++){
//		tasks[i].stack_size = 0x800;
//		if (i == 2) {
//			tasks[i].ptask = (void*) &Task_Yield_Exit;
//		} else if (i == 3){
//			tasks[i].ptask = (void*) &Task_Create;
//		}
//		else {
//			tasks[i].ptask = (void*) &Task_Yield;
//		}
//		int result = osCreateTask(&tasks[i]);
//		if (!result)
//			printf("osCreateTask failed\r\n");
//	}
//
//	osKernelStart();

//	Block* firstBlock = buddyHeap.blockList[0];

//	DEBUG_PRINTF("Image end: %x\r\n", &_img_end);
//	DEBUG_PRINTF("EStack: %x\r\n", &_estack);
//	DEBUG_PRINTF("Min Stack Size: %x\r\n", &_Min_Stack_Size);
//	DEBUG_PRINTF("Start address: %x\r\n", kernelVariables.startOfHeap);
//	DEBUG_PRINTF("End address: %x\r\n", kernelVariables.endOfHeap);
//	DEBUG_PRINTF("Size of Heap: %d bytes\r\n", kernelVariables.endOfHeap - kernelVariables.startOfHeap);
//	DEBUG_PRINTF("Size of KernelVariables: %d\r\n", sizeof(Kernel_Variables));
//	DEBUG_PRINTF("Size of TCB: %d\r\n", sizeof(TCB));
//	DEBUG_PRINTF("Size of Block: %d\r\n", sizeof(Block));
//	DEBUG_PRINTF("Size of BuddyHeap: %d\r\n", sizeof(BuddyHeap));
//	DEBUG_PRINTF("First Block Address: %x\r\n", firstBlock);
//	DEBUG_PRINTF("First Block Type: %d\r\n", firstBlock->type);
//	DEBUG_PRINTF("First Block Size: %d bytes \r\n", firstBlock->size);
//	DEBUG_PRINTF("First Block TID of Owner: %d\r\n", firstBlock->TIDofOwner);
//	DEBUG_PRINTF("First Block Next: %x\r\n", firstBlock->next);
//	DEBUG_PRINTF("First Block Magic Num: %x\r\n", firstBlock->magicNum);
    int block_size = 32;
//
	int test = Calculate_Order(block_size);
	DEBUG_PRINTF("Order of 32: %d\r\n", test);

	Calculate_Nearest_Order(1);
	Calculate_Nearest_Order(0);
	Calculate_Nearest_Order(32767);
	Calculate_Nearest_Order(32768);
	Calculate_Nearest_Order(32769);
	Calculate_Nearest_Order(33);
	Calculate_Nearest_Order(32);
	Calculate_Nearest_Order(31);
	Calculate_Nearest_Order(63);
	Calculate_Nearest_Order(64);
	Calculate_Nearest_Order(65);

    int test_index = Calculate_Free_List_Idx(test);
    DEBUG_PRINTF("Free list index of %d: %d\r\n", block_size, test_index);

    void* test1 =  k_mem_alloc(block_size);
    void* test2 = k_mem_alloc(block_size);
    void* test3 = k_mem_alloc(7000);
    DEBUG_PRINTF("test1 = %p\r\n", test1);
    DEBUG_PRINTF("test2 = %p\r\n", test2);
    DEBUG_PRINTF("test2 = %p\r\n", test3);

    Print_Free_List();
//    k_mem_alloc(block_size);
//    k_mem_alloc(62);

    DEBUG_PRINTF("Finished Mem_Alloc\r\n");
//    buddyHeap.freeList[test_index];

    // Block* test_block = Create_Block(32, , FREE, kernelVariables.currentRunningTID);

    // Split_Block();
	while (1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

void Print_Free_List() {
	for (int i = 0; i < MAX_ORDER; i++) {
		Block* currentBlock = buddyHeap.freeList[i];
		while (currentBlock != NULL) {
			DEBUG_PRINTF("Block Size: %d, Order: %d\r\n", currentBlock->size, MAX_ORDER - i);
			currentBlock = currentBlock->next;
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

void Task_Yield(void){
	printf("task-%d\r\n", osGetTID());
	while (1) {
		osYield();
	}

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
	task->original_stack_size = 0x200;
	task->args = NULL;
}

void Init_Task_2(TCB* task){
	task->ptask = (void*)&obiwan;
	task->stack_high = (U32)Get_Thread_Stack(0x200);
	task->state = READY;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
	task->original_stack_size = 0x200;
	task->args = NULL;
}

void Init_Task_3(TCB* task){
	task->ptask = (void*)&luke;
	task->stack_high = (U32)Get_Thread_Stack(0x200);
	task->state = READY;
	task->stack_size = 0x200;
	task->current_sp = task->stack_high;
	task->original_stack_size = 0x200;
	task->args = NULL;
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

