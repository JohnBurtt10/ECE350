/*
 * common.c
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */

#include "common.h"
#include <stdio.h>

uint32_t* current_MSP = NULL;

uint32_t* Get_MSP_INIT_VAL(){
	return *(uint32_t**)0x0;
}

unsigned int Get_Total_Memory_Used() {
	unsigned int totalMem = MAIN_STACK_SIZE;

	for (int i = 0; i < MAX_TASKS; i++){
		totalMem += kernelVariables.tcbList[i].stack_size;
	}

	return totalMem;
}

int Scheduler(void) {
	int TIDTaskToRun = 0;
	// Start from current running tid and find next one to run
	for (int i = kernelVariables.currentRunningTID + 1; i <= MAX_TASKS + kernelVariables.currentRunningTID; i++) {
		int TID = i % MAX_TASKS;

		// Ignore null task. Find a TID that is in the ready state.
		if (kernelVariables.tcbList[TID].state == READY && TID != 0) {
			DEBUG_PRINTF(" TID TO SCHEDULE: %d\r\n", TID);
			return TID;
		}
	}
	
	DEBUG_PRINTF(" TID TO SCHEDULE: %d\r\n", TIDTaskToRun);
	return TIDTaskToRun;
}

Block* Create_Block(U32 size, void* heapAddress, U32 type, int tidOwner) {
	Block temp = {
				.type = type,
				.TIDofOwner = tidOwner,
				.next = NULL,
				.size = (((size + sizeof(Block)) + 31)/32) * 32, //https://piazza.com/class/lvlcv9pc4496o8/post/177
				.magicNum = MAGIC_NUMBER_BLOCK
	};

	*(Block*) heapAddress = temp;
	buddyHeap.blockList[buddyHeap.currentBlockListSize] = (Block*) heapAddress;

	buddyHeap.currentBlockListSize += 1;

	return (Block*)heapAddress;
}

U32 Calculate_Order(U32 num) {
	U32 result = 0;
	while (num >>= 1) result++;
	return result;
}

U32 Calculate_Free_List_Idx(U32 num) {
	U32 order = CalculateOrder(num);
	U32 index = MAX_ORDER + MIN_BLOCK_ORDER - order;

	if(index >MAX_ORDER){
		index = MAX_ORDER;
	}
	return index;
}

Block* Split_Block(Block* parentBlock){
	Block* createdBlock = (Block*)(FREE, parentBlock.size/2 + sizeof (Block), kernelVariables.currentRunningTID, parentBlock.startingAddress+ parentBlock.size/2 + sizeof(Block), parentBlock);
	// Block* newBlock = Create_Block(parentBlock->size, parentBlock->startingAddress + parentBlock->size/2, FREE, kernelVariables.currentRunningTID);

	// Find corresponding free list index using order
	U32 parentFreeListIdx = Calculate_Free_List_Idx(parentBlock->size);
	// U32 parentOrder = CalculateOrder(parentBlock.size);
	// U32 parentFreeListIdx = -parentOrder + MAX_ORDER + MIN_BLOCK_ORDER;

	// TODO: turn into doubly linked list

	// Push created buddy block to the free list
	createdBlock->next = buddyHeap.freeList[parentFreeListIdx];
	buddyHeap.freeList[parentFreeListIdx] = createdBlock;

	// createdBlock->next = buddyHeap.freeList[parentFreeListIdx]->next;
	// buddyHeap.freeList[parentFreeListIdx]->next = createdBlock;

	// Set parent as used and remove from free list
	parentBlock->type = USED;
	parentBlock->size = (parentBlock->size)/2;

	// Remove parent block for free list
	// Block* temp = buddyHeap.freeList[parentFreeListIdx];
	buddyHeap.freeList[parentFreeListIdx] = buddyHeap.freeList[parentFreeListIdx]->next;

	// Return pointer to allocated block
	return parentBlock;
}
