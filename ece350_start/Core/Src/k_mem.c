/*
 * k_mem.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */

#include "common.h"
#include "k_task.h"
#include "k_mem.h"
#include "circular_queue.h"
#include <stdint.h>
#include <stddef.h>

// ------- Globals --------
unsigned int numCreatedTasks;
uint32_t* p_threadStacks[MAX_TASKS];
TCB tcbs[MAX_TASKS];

/*
 * Questions to ask ourselves during design:
 * 1) How we are going to store the TCB's?: Since our memory is so limited, we can utilize an array. It would basically result in instant lookup.
 * 2) How will the Kernel know which thread/task is currently executing?: Based on the thread's/task's state.
 * 3) How does the scheduler know which threads/tasks are available for scheduling?: Based on the thread's/task's state.
 * 4) What data does the kernel need to store and access in order to load/unload tasks?: We can utilize the main stack to store the TCB's.
 */

void osKernelInit(void) {
	numCreatedTasks = 0;
	osInitTCBArray();
	Queue_Init(tcbs);
	return;
}

void osInitTCBArray() {
	for (int i = 0; i < MAX_TASKS; i++) {
		tcbs[i].ptask = NULL;
		tcbs[i].stack_high = (U32)Get_Thread_Stack(i);
		tcbs[i].tid = i;
		tcbs[i].state = DORMANT;
		tcbs[i].stack_size = THREAD_STACK_SIZE;
		tcbs[i].current_sp = tcbs[i].stack_high;
	}

	return;
}


