/*
 * k_task.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */
#include "k_task.h"
#include "main.h"
#include "common.h"
#include <stdio.h>

// Do not perform de-fragmentation for lab1 https://piazza.com/class/lvlcv9pc4496o8/post/58
/*
 * 1) Iterate through all TCB's and determine if there exists an uninitialized TCB.
 * 		a) If there exists an uninitialized TCB, then use it.
 * 			i) Before writing to it, make sure there exists enough memory to allocate to it!
 * 		b) If there does not, check if one is DORMANT, and use its stack IF AND ONLY IF, the task->stack_size < dormant task size
 */

int osCreateTask(TCB* task) {
	int output;
	TRIGGER_SVC(OS_CREATE_TASK);
	__asm("MOV %0, R0": "=r"(output));
	return output;
}

void osYield(void) {
	TRIGGER_SVC(OS_YIELD);
}

int osKernelStart(void) {
	int output;
	TRIGGER_SVC(OS_KERNEL_START);
	__asm("MOV %0, R0": "=r"(output));

	if (output == RTX_OK){
		TRIGGER_SVC(OS_YIELD);
	}
	return output;
}

int osTaskInfo(task_t TID, TCB* task_copy) {
	if (task_copy == NULL){
		return RTX_ERR;
	}

	int output;
	TRIGGER_SVC(OS_TASK_INFO);
	__asm("MOV %0, R0": "=r"(output));
	return output;
}

/**
 * Returns the TID of a task, used by the user application.
 * Returns 0 if the Kernel has not started
*/
task_t osGetTID (void) {
	task_t tid;
	TRIGGER_SVC(OS_GET_TID);
	__asm("MOV %0, R0": "=r"(tid));
	return tid;
}

uint32_t* Get_Thread_Stack(unsigned int stack_size){
	// ARM Cortex architecture grows stack grows downwards (high address to low address)

	if (stack_size < MIN_THREAD_STACK_SIZE){
		DEBUG_PRINTF("Failed to get starting address for thread stack due to stack size being too small\r\n");
		return NULL;
	}

	if (kernelVariables.totalStackUsed + stack_size > MAX_STACK_SIZE){
		DEBUG_PRINTF("Failed to get starting address for thread stack due not enough memory\r\n");
		return NULL;
	}

	uint32_t newStackStart = (unsigned int)Get_MSP_INIT_VAL() - MAIN_STACK_SIZE; // Starting position
	for (int i = 0; i < MAX_TASKS; i++) {
		newStackStart -= kernelVariables.tcbList[i].original_stack_size;
	}

	DEBUG_PRINTF("Found starting address for thread stack: %p. Size: %d\r\n", (uint32_t*)newStackStart, stack_size);
	return (uint32_t*) newStackStart;
}

void Null_Task_Function(void) {
	DEBUG_PRINTF("  IN NULL TASK :(\r\n");

	while (1);

	return;
}

void Kill_Thread() {
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk; // Kills thread
	return;
}


/**
 * Immediately causes the current task to exit and calls the scheduler (similar to osYield)
 * When called, the calling task is removed from the scheduler and any resources it was using are returned to the operating system
 * Resources returned: can re-use the same TID and/or stack for subsequent tasks
 * Set the status to DORMANT or memory allocation
 * Returns RTX_OK if it was called by a running task
 * Else returns RTX_ERR and does nothing
 */
int osTaskExit(void){	
	int taskExitStatus;
	TRIGGER_SVC(OS_TASK_EXIT);
	
	__asm("MOV %0, R0": "=r"(taskExitStatus));
	
	return taskExitStatus;
}
