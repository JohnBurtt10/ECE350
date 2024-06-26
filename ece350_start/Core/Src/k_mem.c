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

Block* blockList[NUMBER_OF_NODES];

Block* freeList[HEIGHT_OF_TREE];

U8 bitArray[NUMBER_OF_NODES];


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
	buddyHeap.currBLIdx = 0;
	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		blockList[i] = NULL;
	}

	for (int i = 0; i < HEIGHT_OF_TREE; i++) {
		freeList[i] = NULL;
	}

	for (int i = 0; i < NUMBER_OF_NODES; i++) {
		bitArray[i] = 0;
	}

	((Block *)kernelVariables.startOfHeap)->type = FREE;
	((Block *)kernelVariables.startOfHeap)->size = kernelVariables.endOfHeap - kernelVariables.startOfHeap;
	((Block *)kernelVariables.startOfHeap)->TIDofOwner = TID_NULL;
	((Block *)kernelVariables.startOfHeap)->next = NULL;

	blockList[0] = (Block *)kernelVariables.startOfHeap;
}


void* k_mem_alloc(size_t size)
{
	// check that k_mem_init was called and successfully initialized the heap
	// Return null if the number of bytes requested is 0 or if heap is not initialized
	if(!kernelVariables.buddyHeapInit || !kernelVariables.kernelInitRan|| size == 0){
		return NULL;
	}


	// blockList[newIdx].startingAddress = blockList[currBLIdx].size - blockList[newIdx].size + 1;

	// Block* curr_block = (Block *());
	// curr_block.address = (U32)block_address;

	Block curr_block = blockList[buddyHeap.currBLIdx];

	// Make block size a multiple of 4 four address alignment
	// uint32_t mult4size= size +sizeof(Block)- ((size+sizeof(Block))%4);
	uint32_t required_size = size+ sizeof(Block);

	/* Start at free list 
	// Calculate order of required block
	While loop: Check which size is large enough to fit the requested size to get the order
	- Calc corresponding index in free list

	While: use the free list index and subtract to get to the 
	- Iterate up the free list to find smallest available block
	- check if the required size is able to fit into that block (choosing the smallest satisfactory)
	- If block is free then split

	Var: counts number of orders moved up to get the number of splits required or compare req size to current order level block size to know when to stop splitting and create the block
	*/

	
	
	
	// while( freeList[]!= NULL){		
	// 	size_t next_order_size = blockList[buddyHeap.currBLIdx].size/2; //TODO: fix to correct variable for Block size
		
	// 	// If current block size is larger than requested size and it is free, split
	// 	if (!blockList[buddyHeap.currBLIdx].isAllocated && blockList[buddyHeap.currBLIdx].size >= size){
	// 		// if the size fits into the order's block size, allocate a new block 
	// 		uint32_t newBlockIdx = buddyHeap.currBLIdx +1; // TODO: +1 for now

	// 		// Add first buddy block to free list, initalized as free
	// 		blockList[newBlockIdx].type = FREE;
	// 		blockList[newBlockIdx].size = next_order_size;
	// 		blockList[newBlockIdx].TIDofOwner = kernelVariables.currentRunningTID; // fix
	// 		blockList[newBlockIdx].startingAddress = blockList[buddyHeap.currBLIdx].size - blockList[newBlockIdx].size + 1;
	// 		blockList[newBlockIdx]->next = blockList[buddyHeap.currBLIdx];
			
	// 		bitArray[newBlockIdx].type = FREE;
	// 		freeList[newBlockIdx] = blockList[newBlockIdx];

	// 		blockList[buddyHeap.currBLIdx].size = mult4size;

	// 		// Second buddy block is allocated and linked
	// 		blockList[buddyHeap.currBLIdx].isAllocated = USED;
	// 		bitArray[buddyHeap.currBLIdx].type = USED;
	// 		blockList[buddyHeap.currBLIdx]->next = blockList[newBlockIdx];

	// 		// remove from free list

	// 		// returns pointer to the start of the usable memory in the block/ allocated memory
	// 		return (void *)blockList[buddyHeap.currBLIdx];
	// 	}

	// 	buddyHeap.currBLIdx++; // or instead move to the next element in the block list. 
	// }
	// return NULL;
}
