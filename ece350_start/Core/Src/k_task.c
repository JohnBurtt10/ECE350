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
#include <stdint.h>

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

	/* If the kernel was successful started, call osYield to run the first task */
	if (output == RTX_OK){
		TRIGGER_SVC(OS_YIELD);
	}

	return output;
}

int osTaskInfo(task_t TID, TCB* task_copy) {
	// Checks that the TCB at the pointer exists
	if (task_copy == NULL){
		return RTX_ERR;
	}

	int output;

	TRIGGER_SVC(OS_TASK_INFO);
	__asm("MOV %0, R0": "=r"(output));

	return output;
}

task_t osGetTID (void) {
	task_t tid;

	TRIGGER_SVC(OS_GET_TID);
	__asm("MOV %0, R0": "=r"(tid));

	return tid;
}

uint32_t* Get_Thread_Stack(unsigned int stack_size){
	// Ensuring that there is enough space in the thread stack
	if (stack_size < STACK_SIZE){
		DEBUG_PRINTF("  Failed to get starting address for thread stack due to stack size being too small\r\n");
		return NULL;
	}

	if (kernelVariables.totalStackUsed + stack_size > MAX_STACK_SIZE){
		DEBUG_PRINTF("  Failed to get starting address for thread stack due not enough memory\r\n");
		return NULL;
	}

	// ARM Cortex architecture grows stack grows downwards (high address to low address)
	uint32_t newStackStart = (unsigned int)Get_MSP_INIT_VAL() - MAIN_STACK_SIZE; // Starting position

	// Iterate through the stacks of each tcb to get the starting address
	for (int i = 0; i < MAX_TASKS; i++) {
		newStackStart -= kernelVariables.tcbList[i].original_stack_size;
	}

	DEBUG_PRINTF("  Found starting address for thread stack: %p. Size: %d\r\n", (uint32_t*)newStackStart, stack_size);
	return (uint32_t*) newStackStart;
}

void Null_Task_Function(void) {
	DEBUG_PRINTF("  IN NULL TASK :(\r\n");

	while (1);

	return;
}

int osTaskExit(void){	
	int taskExitStatus;

	TRIGGER_SVC(OS_TASK_EXIT);
	__asm("MOV %0, R0": "=r"(taskExitStatus));
	
	return taskExitStatus;
}
