/*
 * k_task.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */
#include "k_task.h"
#include "main.h"
#include <stdio.h>

// Do not perform de-fragmentation for lab1 https://piazza.com/class/lvlcv9pc4496o8/post/58
/*
 * 1) Iterate through all TCB's and determine if there exists an uninitialized TCB.
 * 		a) If there exists an uninitialized TCB, then use it.
 * 			i) Before writing to it, make sure there exists enough memory to allocate to it!
 * 		b) If there does not, check if one is DORMANT, and use its stack IF AND ONLY IF, the task->stack_size < dormant task size
 */
int osCreateTask(TCB* task) {
	TCB* tcbs = kernelVariables.tcbList;

	if (task->stack_size < MIN_THREAD_STACK_SIZE || task->ptask == NULL){
		DEBUG_PRINTF("Failed to create task. Stack size too small or missing ptr to function\r\n");
		return RTX_ERR;
	}

	if (kernelVariables.numAvaliableTasks == MAX_TASKS || kernelVariables.totalStackUsed + task->stack_size > MAX_STACK_SIZE){
		DEBUG_PRINTF("Failed to create task. Not enough memory or reached maximum allowed tasks\r\n");
		return RTX_ERR;
	}

	int TIDtoOverwrite = -1;
	int TCBStackSmallest = 2147483647;
	for (int i = 0; i < MAX_TASKS; i++) {
		// Found uninitialized TCB
		if (tcbs[i].state == CREATED){
			tcbs[i].ptask = task->ptask;
			tcbs[i].stack_high = (U32)Get_Thread_Stack(task->stack_size);
			tcbs[i].tid = i;
			tcbs[i].state = READY;
			tcbs[i].stack_size = task->stack_size;
			tcbs[i].current_sp = tcbs[i].stack_high;
			tcbs[i].original_stack_size = task->stack_size;
			tcbs[i].args = task->args;

			task->tid = i;
			kernelVariables.totalStackUsed += task->stack_size;
			kernelVariables.numAvaliableTasks++;

			DEBUG_PRINTF("Found Empty TCB with TID: %d\r\n", i);
			return RTX_OK;
		}

		// Found terminated task. Check if we can fit the new TCB into it.
		if (tcbs[i].state == DORMANT) {
			if (tcbs[i].original_stack_size >= task->stack_size){
				if (tcbs[i].original_stack_size < TCBStackSmallest){
					TCBStackSmallest = tcbs[i].original_stack_size;
					TIDtoOverwrite = i;
				}
			}
		}
	}

	if (TIDtoOverwrite != -1) {
		DEBUG_PRINTF("Found TCB To Overwrite with TID: %d. Stack size: %d\r\n", TIDtoOverwrite, tcbs[TIDtoOverwrite].stack_size);

		tcbs[TIDtoOverwrite].ptask = task->ptask;
		tcbs[TIDtoOverwrite].state = READY;
		tcbs[TIDtoOverwrite].current_sp = tcbs[TIDtoOverwrite].stack_high;
		tcbs[TIDtoOverwrite].stack_size = task->stack_size;
		tcbs[TIDtoOverwrite].args = task->args;
		kernelVariables.numAvaliableTasks++;
		return RTX_OK;
	}

	// All free TCB's are currently in use or there is no TCB with enough space to accommodate new task.
	DEBUG_PRINTF("Failed to create new task. All tasks are currently in use, or there is no TCB with enough space to accommodate new task\r\n");
	return RTX_ERR;
}

int osTaskInfo(task_t TID, TCB* task_copy) {
	if (task_copy == NULL){
		return RTX_ERR;
	}

	if (TID >= 0 && TID <= MAX_TASKS){
		TCB task = kernelVariables.tcbList[TID];

		task_copy->args = task.args;
		task_copy->current_sp = task.current_sp;
		task_copy->ptask = task.ptask;
		task_copy->stack_high = task.stack_high;
		task_copy->stack_size = task.stack_size;
		task_copy->state = task.state;
		task_copy->tid = task.tid;

		return RTX_OK;
	}

	return RTX_ERR;
}

uint32_t* Create_Thread() {
	// Offset from main stack (portion of stack containing interrupts, setups, etc)
	kernelVariables.numAvaliableTasks += 1;
	uint32_t* p_threadStack = Get_Thread_Stack_OLD(kernelVariables.numAvaliableTasks);

	// Once we have the new pointer for the thread stack, we can now setup its stack and context
	// This function will make the stack pointer, point to bottom of stack (technically top since its the last value pushed to it)
	Init_Thread_Stack(&p_threadStack, &print_continuously);
	p_threadStacks[kernelVariables.numAvaliableTasks - 1] = p_threadStack;

	Trigger_System_Call(CREATE_THREAD);

	return p_threadStack;
}

void Init_Thread_Stack(uint32_t** p_threadStack, void (*callback)()){
	*(--*p_threadStack) = 1 << 24; // xPSR register, setting chip to "Thumb" mode
	*(--*p_threadStack) = (uint32_t)callback; // PC Register storing next instruction
	for (int i = 0; i < 14; i++){
		*(--*p_threadStack) = 0xA; //An arbitrary number
	}
}

uint32_t* Get_Thread_Stack_OLD(unsigned int threadNum){
    // printf("MSP_INIT: %p\r\n", Get_MSP_INIT_VAL());
	// ARM Cortex architecture grows stack grows downwards (high address to low address)
	return (uint32_t*)(( (unsigned int)Get_MSP_INIT_VAL() - MAIN_STACK_SIZE ) - (threadNum-1)*MIN_THREAD_STACK_SIZE);
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
		newStackStart -= kernelVariables.tcbList[i].stack_size;
	}

	DEBUG_PRINTF("Found starting address for thread stack: %p. Size: %d\r\n", (uint32_t*)newStackStart, stack_size);
	return (uint32_t*) newStackStart;
}

void print_continuously(void){
	while(1){
		printf("Thread2222\r\n");
	}
}

void Kill_Thread() {
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk; // Kills thread
	return;
}
