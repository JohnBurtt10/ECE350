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
Kernel_Variables kernelVariables = {.currentRunningTID = -1,
									.kernelInitRan = 0,
									.numAvaliableTasks = 0,
									.kernelStarted = 0,
									.totalStackUsed = MAIN_STACK_SIZE + NULL_TASK_STACK_SIZE,
									.endOfHeap = 0,
									.startOfHeap = 0,
									.buddyHeapInit = 0};

BuddyHeap buddyHeap;


void osKernelInit(void) {
	osInitTCBArray();
	kernelVariables.kernelInitRan = 1;
	kernelVariables.startOfHeap = (unsigned int)&_img_end;
	kernelVariables.endOfHeap = (unsigned int)&_estack - (unsigned int)&_Min_Stack_Size;
	return;
}

void osInitTCBArray(void) {
	// Initializing null task
	kernelVariables.tcbList[0].ptask = (void*) &Null_Task_Function;
	kernelVariables.tcbList[0].stack_high = (U32) Get_Thread_Stack(0x400);
	kernelVariables.tcbList[0].tid = TID_NULL;
	kernelVariables.tcbList[0].state = READY;
	kernelVariables.tcbList[0].stack_size = NULL_TASK_STACK_SIZE;
	kernelVariables.tcbList[0].current_sp = kernelVariables.tcbList[0].stack_high;
	kernelVariables.tcbList[0].original_stack_size = NULL_TASK_STACK_SIZE;
	kernelVariables.tcbList[0].args = NULL;

	Init_Thread_Stack((U32*)kernelVariables.tcbList[0].stack_high, kernelVariables.tcbList[0].ptask, 0);

	// Initialize each task 
	for (int i = 1; i < MAX_TASKS; i++) {
		kernelVariables.tcbList[i].ptask = NULL;
		kernelVariables.tcbList[i].stack_high = 0x0;
		kernelVariables.tcbList[i].tid = i;
		kernelVariables.tcbList[i].state = CREATED;
		kernelVariables.tcbList[i].stack_size = 0;
		kernelVariables.tcbList[i].current_sp = 0x0;
		kernelVariables.tcbList[i].original_stack_size = 0;
		kernelVariables.tcbList[i].args = NULL;
	}

	return;
}

int k_mem_init(void) {
	if (kernelVariables.buddyHeapInit || !kernelVariables.kernelInitRan)
		return RTX_ERR;

	osInitBuddyHeap();

	kernelVariables.buddyHeapInit = 1;
	return RTX_OK;
}


void osInitBuddyHeap(void) {
	buddyHeap.currentBlockListSize = 0;

	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		buddyHeap.blockList[i] = NULL;
	}

	for (int i = 0; i < HEIGHT_OF_TREE; i++) {
		buddyHeap.freeList[i] = NULL;
	}

	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		buddyHeap.bitArray[i] = 0;
	}

	Create_Block(kernelVariables.endOfHeap - kernelVariables.startOfHeap, (U32*)kernelVariables.startOfHeap, FREE, -1);
}

Block* Create_Block(U32 size, void* heapAddress, U32 type, int tidOwner) {
	Block temp = {
				.type = type,
				.TIDofOwner = tidOwner,
				.next = NULL,
				.size = (((size + sizeof(Block)) + 31)/32) * 32 //https://piazza.com/class/lvlcv9pc4496o8/post/177
	};

	*(Block*) heapAddress = temp;
	buddyHeap.blockList[buddyHeap.currentBlockListSize] = (Block*) heapAddress;

	buddyHeap.currentBlockListSize += 1;

	return (Block*)heapAddress;
}
