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

U32 Calculate_Order(U32 num) {
	U32 result = 0;
	while (num >>= 1) result++;
	return result;
}

int Calculate_Free_List_Idx(U32 order) {
	int index = MAX_ORDER + MIN_BLOCK_ORDER - order;

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
	buddyHeap.freeList[freeListIdx]->prev = NULL;

	return popped_block;
}

void Coalesce_Block(Block* parentBlock, Block* buddyBlock) {
	// Remove the buddyBlock from the free list
	U32 buddyOrder = Calculate_Order(buddyBlock->size);
	U32 buddyIdx = Calculate_Free_List_Idx(buddyOrder);
	Block* prevBlockOfBuddy = buddyBlock->prev;
	Block* nextBlockOfBuddy = buddyBlock->next;

	// Check if the buddy block is not the head
	if (prevBlockOfBuddy != NULL) {
		prevBlockOfBuddy->next = nextBlockOfBuddy;
	} else {
		Free_List_Pop(buddyIdx);
	}

	// Check if the buddy block is not the tail of the list
	if (nextBlockOfBuddy != NULL) {
		nextBlockOfBuddy->prev = prevBlockOfBuddy;
	}

	Empty_Block(buddyBlock);

	// Update the size of the parentBlock
	U32 oldParentOrder = Calculate_Order(parentBlock->size);
	U32 oldParentIdx = Calculate_Free_List_Idx(oldParentOrder);
	parentBlock->size = parentBlock->size * 2;
	U32 newParentOrder = Calculate_Order(parentBlock->size);
	U32 newParentIdx = Calculate_Free_List_Idx(newParentOrder);

	// Update the parentBlock in the free list
	//   1) Remove the parentBlock in the free list
	//   2) Push the parentBlock to the new index in the free list
	Block* tempBlock = parentBlock;
	Block* prevBlockOfParent = parentBlock->prev;
	Block* nextBlockOfParent = parentBlock->next;

	// Check if the parent block is the head
	if (prevBlockOfParent != NULL) {
		prevBlockOfParent->next = nextBlockOfParent;
	} else {
		Free_List_Pop(oldParentIdx);
	}

	if (nextBlockOfParent != NULL) {
		nextBlockOfParent->prev = prevBlockOfParent;
	}

	Free_List_Push(tempBlock, newParentIdx);
}

void Empty_Block(Block* block) {
	block->type = FREE;
	block->size = 0;
	block->TIDofOwner = 0;
	block->startingAddress = 0;
	block->next = NULL;
	block->prev = NULL;
	block->magicNum = 0;
}

inline Block* Get_Buddy(Block* block) {
	U32 order = Calculate_Order(block->size);
	DEBUG_PRINTF("  INFO: Order to get the block address: %d\r\n", order);
	U32 buddyAddress = 0x0;

	if ((U32)block == kernelVariables.startOfHeap) {
		buddyAddress = kernelVariables.startOfHeap + block->size;
	}
	else if ((U32)block == kernelVariables.endOfHeap - block->size) {
		buddyAddress = kernelVariables.endOfHeap - block->size;
	}
	else {
		buddyAddress = (U32)(block) ^ (1 << order);
	}

//	if (buddyAddress > kernelVariables.endOfHeap || buddyAddress < kernelVariables.startOfHeap) {
//		DEBUG_PRINTF("  ERROR: The buddy address is out of range of the heap: %x\r\n", buddyAddress);
//		DEBUG_PRINTF("  ERROR: Start Address = %x, End Address = %x, Size = %d", kernelVariables.startOfHeap, kernelVariables.endOfHeap, kernelVariables.endOfHeap - kernelVariables.startOfHeap);
//		return NULL;
//	}

	Block* buddy = (Block *)(buddyAddress);
	DEBUG_PRINTF("  INFO: Block to dealloc address: %x, Buddy address: %x, Block to dealloc size: %d. XOR size value: %d.\r\n", block, buddyAddress, block->size, 1 << order);

//	if (buddy->magicNum == MAGIC_NUMBER_BLOCK) {
//		DEBUG_PRINTF("  INFO: Valid buddy address!\r\n");
//		return buddy;
//	} else {
//		DEBUG_PRINTF("  INFO: Invalid buddy address >:(\r\n");
//		return NULL;
//	}

	return buddy;
}
