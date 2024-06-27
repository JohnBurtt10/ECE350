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

	Block* initial_block = Create_Block(kernelVariables.endOfHeap - kernelVariables.startOfHeap, (U32*)kernelVariables.startOfHeap, FREE, -1);
	Free_List_Push(initial_block);
}


void* k_mem_alloc(size_t size)
{
	// check that k_mem_init was called and successfully initialized the heap
	// Return null if the number of bytes requested is 0 or if heap is not initialized
	if(!kernelVariables.buddyHeapInit || !kernelVariables.kernelInitRan || size == 0){
		return NULL;
	}

	uint32_t required_size = size + sizeof(Block);

	U32 required_order = Calculate_Order(required_size); 
	U32 required_idx = Calculate_Free_List_Idx(required_order);

	// the requested size is too large
	if(required_idx < 0){
		return NULL;
	}

	U32 smallest_all_block_idx = required_idx;

	// Iterate through the free list to find the smallest available block already allocated/ not empty
	while(smallest_all_block_idx >= 0){
		// Save the index of the first block that is free
		if(buddyHeap.freeList[smallest_all_block_idx]!= NULL && buddyHeap.freeList[smallest_all_block_idx].type == FREE){
			break;
		}

		// Decrease level until non-empty list is found
		smallest_all_block_idx--;
	}

	// If there is no free block, allocation fails
	if(smallest_all_block_idx == -1){
		return NULL;
	}

	Block* curr_block = buddyHeap.freeList[smallest_all_block_idx];
	U32 num_splits_req = required_idx - smallest_all_block_idx;

	// Split the head of the list until the level of the required index is reached
	for(U32 i = 0; i< num_splits_req; i++){
		curr_block = Split_Block(curr_block);

		if(i == num_splits_req){
			// return pointer to the allocated memory block
			return (void*)curr_block;
		}
		curr_block = curr_block->next;
	}

	return NULL;
}
