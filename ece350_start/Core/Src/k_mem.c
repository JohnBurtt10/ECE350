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
	kernelVariables.endOfHeap = (unsigned int)&_estack - MAX_STACK_SIZE;
	kernelVariables.startOfHeap = (unsigned int)&_img_end;
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
	if (kernelVariables.buddyHeapInit || kernelVariables.kernelInitRan)
		return RTX_ERR;

	osInitBuddyHeap();

	kernelVariables.buddyHeapInit = 1;
	return RTX_OK;
}


void osInitBuddyHeap(void) {
	buddyHeap.currBLIdx = 0;
	for (int i = 0; i < HEIGHT_OF_TREE; i++) {
		buddyHeap.freeList[i] = NULL;
	}

	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		buddyHeap.bitArray[i] = 0;
	}

	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		buddyHeap.blockList[i] = NULL;
	}
}


void* k_mem_alloc(size_t size)
{
	// check that k_mem_init was called and successfully initialized the heap
	// Return null if the number of bytes requested is 0 or if heap is not initialized
	if(!kernelVariables.buddyHeapInit || !kernelVariables.kernelInitRan|| size == 0){
		return NULL;
	}

	// buddyHeap.blockList[newIdx].startingAddress = buddyHeap.blockList[currBLIdx].size - buddyHeap.blockList[newIdx].size + 1;

	// Block* curr_block = (Block *());
	// curr_block.address = (U32)block_address;

	Block curr_block = buddyHeap.blockList[buddyHeap.currBLIdx];

	// Make block size a multiple of 4 four address alignment
	uint32_t mult4size= size +sizeof(Block)- ((size+sizeof(Block))%4);

	while(buddyHeap.blockList[buddyHeap.currBLIdx] != NULL){		
		size_t next_order_size = buddyHeap.blockList[buddyHeap.currBLIdx].size/2; //TODO: fix to correct variable for Block size
		
		// If current block size is larger than requested size and it is free, split
		if (!buddyHeap.blockList[buddyHeap.currBLIdx].isAllocated && buddyHeap.blockList[buddyHeap.currBLIdx].size >= size){
			// if the size fits into the order's block size, allocate a new block 
			uint32_t newBlockIdx = buddyHeap.currBLIdx +1; // TODO: +1 for now

			// Add first buddy block to free list, initalized as free
			buddyHeap.blockList[newBlockIdx].type = FREE;
			buddyHeap.blockList[newBlockIdx].size = next_order_size;
			buddyHeap.blockList[newBlockIdx].TIDofOwner = buddyHeap.currBLIdx; // fix
			buddyHeap.blockList[newBlockIdx].startingAddress = buddyHeap.blockList[buddyHeap.currBLIdx].size - buddyHeap.blockList[newIdx].size + 1;
			buddyHeap.blockList[newBlockIdx]->next = buddyHeap.blockList[buddyHeap.currBLIdx];
			
			buddyHeap.bitArray[newBlockIdx].type = FREE;
			buddyHeap.freeList[newBlockIdx] = buddyHeap.blockList[newBlockIdx];

			buddyHeap.blockList[buddyHeap.currBLIdx].size = mult4size;

			// Second buddy block is allocated and linked
			buddyHeap.blockList[buddyHeap.currBLIdx].isAllocated = USED;
			buddyHeap.bitArray[buddyHeap.currBLIdx].type = USED;
			buddyHeap.blockList[buddyHeap.currBLIdx]->next = buddyHeap.blockList[newBlockIdx];

			// remove from free list

			// returns pointer to the start of the usable memory in the block/ allocated memory
			return (void *)buddyHeap.blockList[buddyHeap.currBLIdx];
		}

		buddyHeap.currBLIdx++; // or instead move to the next element in the block list. 
	}
	return NULL;
}