/*
 * common.c
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */

#include "common.h"
#include <stdio.h>

uint32_t* current_MSP = NULL;

// Count leading zeros
__attribute__((always_inline))
inline U32 CLZ(U32 num) {
	U32 output = 0;
	__asm("CLZ %0, %1": "=r" (output): "r" (num));
	return output;
}

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

	// Ensure size is not greater than our max order, and round to nearest power of 2 and multiple of 32.
	U32 newSize = 0;
	if (size > (1 << (MAX_ORDER + MIN_BLOCK_ORDER))){
		newSize = (1 << (MAX_ORDER + MIN_BLOCK_ORDER));
	} else {
		newSize = 2 << (32 - CLZ(size - 1) - 1);
	}

	DEBUG_PRINTF("Given Size: %d. Rounded Size: %d\r\n", size, newSize);

	Block temp = {
				.type = type,
				.TIDofOwner = tidOwner,
				.startingAddress = (U32) heapAddress,
				.next = NULL,
				.prev = NULL,
				.size = newSize, //https://piazza.com/class/lvlcv9pc4496o8/post/177
				.magicNum = MAGIC_NUMBER_BLOCK
	};

	DEBUG_PRINTF("Address: %p\r\n", heapAddress);
	*(Block*) heapAddress = temp;

//	buddyHeap.blockList[buddyHeap.currentBlockListSize] = (Block*) heapAddress;
//
//	buddyHeap.currentBlockListSize += 1;

	return (Block*)heapAddress;
}

U32 Calculate_Order(U32 num) {
	U32 result = 0;
	while (num >>= 1) result++;
	return result;
}

U32 Calculate_Free_List_Idx(U32 order) {
	U32 index = MAX_ORDER + MIN_BLOCK_ORDER - order;

	if(index > MAX_ORDER){
		index = MAX_ORDER;
	}
	
	return index;
}

void Free_List_Push(Block* newBlock, U32 freeListIdx){
	// Push created buddy block to the free list
	newBlock->prev = NULL;

	newBlock->next = buddyHeap.freeList[freeListIdx];

	// list contains more than 0 elements
	if(buddyHeap.freeList[freeListIdx] != NULL){
		buddyHeap.freeList[freeListIdx]->prev = newBlock;
	}

	buddyHeap.freeList[freeListIdx] = newBlock;
}

Block* Free_List_Pop(U32 freeListIdx){
	Block *popped_block;

	if(buddyHeap.freeList[freeListIdx] == NULL){
		return NULL;
	}

	popped_block = buddyHeap.freeList[freeListIdx];
	buddyHeap.freeList[freeListIdx] = buddyHeap.freeList[freeListIdx]->next;
	// buddyHeap.freeList[freeListIdx]->prev = NULL;

	return popped_block;
}

Block* Split_Block(Block* parentBlock){
	U32 parentOrder = Calculate_Order(parentBlock->size);
//	U32 buddy_addr = (int)parentBlock ^ (1 << (parentOrder-1));
	U32 newSize = parentBlock->size/2;
//	DEBUG_PRINTF("Starting address: %p\r\n", parentBlock->startingAddress);
	Block* createdBlock = Create_Block(newSize, (void*) (parentBlock->startingAddress + newSize), FREE, kernelVariables.currentRunningTID);

//	Block* createdBlock = Create_Block(parentBlock->size/2, buddy_addr, FREE, kernelVariables.currentRunningTID);

	// Find corresponding free list index using ordere
	U32 parentFreeListIdx = Calculate_Free_List_Idx(parentOrder);
	DEBUG_PRINTF("Splitting parent with free list index: %d\r\n", parentFreeListIdx);

	// DEBUG_PRINTF("Current head of free list: %d has size %d\r\n", parentFreeListIdx, buddyHeap.freeList[parentFreeListIdx]);

	U32 createdIndex = Calculate_Free_List_Idx(parentOrder-1);
	// Push created buddy block to the free list
	Free_List_Push(createdBlock, createdIndex);

	// Set parent as used and remove from free list
	parentBlock->type = USED;
	parentBlock->size = (parentBlock->size)/2;
	Block* poppedBlock = Free_List_Pop(parentFreeListIdx);

//	DEBUG_PRINTF("Pushed buddy address: %d\r\n", buddy_addr);

	// Return pointer to allocated block
	return parentBlock;
}
