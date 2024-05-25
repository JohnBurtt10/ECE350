/*
 * k_mem.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */

#include "common.h"
#include "k_task.h"
#include "k_mem.h"
#include <stdint.h>
#include <stddef.h>

// ------- Globals --------
uint32_t* p_threadStacks[MAX_TASKS];

Kernel_Variables kernelVariables;

// Instead of using a circular queue, we will utilize the currentRunningTID by incrementing it by one each time we want to switch to another tcb.
// This will behave closely like a queue, but will remove any complex data structures

void osKernelInit(void) {
	kernelVariables.numCreatedTasks = 0;
	kernelVariables.currentRunningTID = -1;
	kernelVariables.totalStackUsed = MAIN_STACK_SIZE;
	osInitTCBArray();
	return;
}

void osInitTCBArray() {
	for (int i = 0; i < MAX_TASKS; i++) {
		kernelVariables.tcbList[i].ptask = NULL;
		kernelVariables.tcbList[i].stack_high = 0x0;
		kernelVariables.tcbList[i].tid = i;
		kernelVariables.tcbList[i].state = CREATED;
		kernelVariables.tcbList[i].stack_size = 0;
		kernelVariables.tcbList[i].current_sp = 0x0;
	}
	return;
}


