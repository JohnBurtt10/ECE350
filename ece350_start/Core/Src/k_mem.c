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
#include <assert.h>

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

// Count leading zeros
__attribute__((always_inline))
inline U32 CLZ(U32 num) {
	U32 output = 0;
	__asm("CLZ %0, %1": "=r" (output): "r" (num));
	return output;
}

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

int k_mem_count_extfrag(size_t size) {
	// Determine Order
	size_t count = 0;
	for (size_t i = 0; i < HEIGHT_OF_TREE; i++){
		Block* currBlock = buddyHeap.freeList[i];
		if (currBlock) {
			if (currBlock->size < size) {
				count++;
			}
		}
	}
	return count;
}


void osInitBuddyHeap(void) {
	buddyHeap.currentBlockListSize = 0;

//	for (int i = 0; i < NUMBER_OF_NODES; i++) {
//		buddyHeap.blockList[i] = NULL;
//	}

	for (int i = 0; i < HEIGHT_OF_TREE; i++) {
		buddyHeap.freeList[i] = NULL;
	}

//	for (int i = 0; i < NUMBER_OF_NODES; i++) {
//		buddyHeap.bitArray[i] = 0;
//	}

	Block* initial_block = Create_Block(kernelVariables.endOfHeap - kernelVariables.startOfHeap, (U32*)kernelVariables.startOfHeap, FREE, -1);
	Free_List_Push(initial_block, 0);
}


void* k_mem_alloc(size_t size)
{
	// check that k_mem_init was called and successfully initialized the heap
	// Return null if the number of bytes requested is 0 or if heap is not initialized
	if(!kernelVariables.buddyHeapInit || !kernelVariables.kernelInitRan || size <= 0 || size > (1 << (MAX_ORDER + MIN_BLOCK_ORDER))){
		return NULL;
	}

	uint32_t required_size = size + sizeof(Block);
	DEBUG_PRINTF("Required size: %d, sizeof(Block)= %d\r\n", required_size, sizeof(Block));

	// Round to nearest order. Eg, size to allocate = 42 bytes, order should be 6. 2^6 = 64 bytes;
	U32 required_order = 32 - CLZ(required_size - 1);

	U32 required_idx = CALCULATE_FREE_LIST_IDX(required_order);

	Block* freeBlock = buddyHeap.freeList[required_idx];

	if (freeBlock != NULL) {
		freeBlock->type = USED;
		freeBlock->TIDofOwner = kernelVariables.currentRunningTID;
		Free_List_Pop(required_idx);
		return (void*) ((U32)freeBlock + sizeof(Block));
	}

	// Iterate through the free list to find the smallest available block already allocated/ not empty
	int currIndex = required_idx;
	while(currIndex >= 0){
		// Save the index of the first block that is free
		Block* currBlock = buddyHeap.freeList[currIndex];
		if(currBlock != NULL){
			DEBUG_PRINTF("CurrIndex: %d, required_idx = %d\r\n", currIndex, required_idx);
			for (size_t i = currIndex; i < required_idx; i++) {
				DEBUG_PRINTF("Current 'i' Value: %d\r\n", i);

				currBlock = Split_Block(currBlock, i);

				DEBUG_PRINTF("Splitting %d\r\n", i);
				DEBUG_PRINTF("current freelist index: %d\r\n", i);

//				DEBUG_PRINTF("Current block: %p\r\n", currBlock);
				size_t temp = i + 1;
				currBlock = buddyHeap.freeList[temp];
//				DEBUG_PRINTF("Next block: %p\r\n", currBlock);
			}

			currBlock = Free_List_Pop(required_idx);
			currBlock->type = USED;
			currBlock->TIDofOwner = kernelVariables.currentRunningTID;
			return (void*) ((U32) currBlock + sizeof(Block));
		}

		// Decrease level until non-empty list is found
		currIndex--;
	}
	DEBUG_PRINTF("Smallest free block order: %d, index: %d, smallest free block index: %d\r\n", required_order, required_idx, currIndex);

	return NULL;
}

inline Block* Create_Block(U32 size, void* heapAddress, U32 type, int tidOwner) {

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
				.next = NULL,
				.prev = NULL,
				.size = newSize, //https://piazza.com/class/lvlcv9pc4496o8/post/177
				.magicNum = MAGIC_NUMBER_BLOCK
	};

	DEBUG_PRINTF("Address: %p\r\n", heapAddress);
	*(Block*) heapAddress = temp;

	return (Block*)heapAddress;
}

__attribute__((always_inline))
inline void Free_List_Push(Block* newBlock, U32 freeListIdx){
	// Push created buddy block to the free list
	newBlock->prev = NULL;

	newBlock->next = buddyHeap.freeList[freeListIdx];

	// list contains more than 0 elements
	if(buddyHeap.freeList[freeListIdx] != NULL){
		buddyHeap.freeList[freeListIdx]->prev = newBlock;
	}

	buddyHeap.freeList[freeListIdx] = newBlock;
}

__attribute__((always_inline))
inline Block* Free_List_Pop(U32 freeListIdx){
	Block *popped_block;

	popped_block = buddyHeap.freeList[freeListIdx];
	buddyHeap.freeList[freeListIdx] = buddyHeap.freeList[freeListIdx]->next;
	buddyHeap.freeList[freeListIdx]->prev = NULL;

	return popped_block;
}

__attribute__((always_inline))
inline Block* Split_Block(Block* parentBlock, U32 parentFreeListIdx){
//	U32 parentOrder = CALCULATE_ORDER_FROM_FREELIST_IDX(parentFreeListIdx);
	U32 newSize = parentBlock->size/2;
	DEBUG_PRINTF("Starting address: %p\r\n", parentBlock);

	// ---- Create block ----
	Block* createdBlock = (void*) ((U32)parentBlock + newSize);
	Block temp = {
					.type = FREE,
					.TIDofOwner = kernelVariables.currentRunningTID,
					.next = NULL,
					.prev = NULL,
					.size = newSize, //https://piazza.com/class/lvlcv9pc4496o8/post/177
					.magicNum = MAGIC_NUMBER_BLOCK
		};

	*(Block*) createdBlock = temp; // Store block in heap
	DEBUG_PRINTF("Address: %p\r\n", createdBlock);
	// ---------------------

	// Find corresponding free list index using order
	DEBUG_PRINTF("Splitting parent with free list index: %d\r\n", parentFreeListIdx);


	U32 createdIndex = parentFreeListIdx + 1;
	// Push created buddy block to the free list
	Free_List_Push(createdBlock, createdIndex);

	parentBlock->size = newSize;
	Free_List_Pop(parentFreeListIdx);
	Free_List_Push(parentBlock, createdIndex);

//	DEBUG_PRINTF("Pushed buddy address: %d\r\n", buddy_addr);

	// Return pointer to allocated block
	return parentBlock;
}


__attribute__((always_inline))
inline U32 Calculate_Order(U32 num) {
	U32 result = 0;
	while (num >>= 1) result++;
	return result;
}


__attribute__((always_inline))
inline U32 Calculate_Nearest_Order(U32 num) {
	U32 order;
	if (num == 0) {
		order = 0;
	} else {
		order = (32 - CLZ(num - 1));
	}
	DEBUG_PRINTF("CALCULATED NEAREST ORDER: %d\r\n", order);
	return order;
}

__attribute__((always_inline))
inline U32 Calculate_Free_List_Idx(U32 order) {
	U32 index = MAX_ORDER + MIN_BLOCK_ORDER - order;

	if(index > MAX_ORDER){
		index = MAX_ORDER;
	}

	return index;
}

int k_mem_dealloc(void* ptr) {
	/**
	 *  VALIDATE BLOCK AND SET TO FREE
	 *
	 *  Checks:
	 *  	1) Check if the pointer is NULL
	 *  	2) Check if the magic number is set correctly
	 *  	3) Check if current TID matches the TID of block owner
	 *  	4) Check to ensure the block hasn't been freed before
	 */

	// If the pointer is NULL, log and return an error.
	if (ptr == NULL) {
		DEBUG_PRINTF("  ERROR: The pointer to deallocate is NULL.\r\n");
		return RTX_ERR;
	}

	Block* block = (Block *)((U32)ptr - sizeof(Block));

	// If the magic number is not correct, log and return an error.
	if (block->magicNum != MAGIC_NUMBER_BLOCK) {
		DEBUG_PRINTF("  ERROR: The pointer does not point to a valid block, magic number: %x.\r\n", block->magicNum);
		return RTX_ERR;
	}

	// If the current TID does not match TID of owner, log and return error.
	if (kernelVariables.currentRunningTID != block->TIDofOwner) {
		DEBUG_PRINTF("  ERROR: Malicious task, current TID: %d | owner TID: %d.\r\n", kernelVariables.currentRunningTID, block->TIDofOwner);
		return RTX_ERR;
	}

	// If the block is already deallocated, it can't be freed again so log and return error.
	if (block->type == FREE) {
		DEBUG_PRINTF("  ERROR: The block is already freed, can't be freed again.\r\n");
		return RTX_ERR;
	}


	// TODO: Check if they will demalloc the root

	// Once the block passes all check, set it to FREE!
	block->type = FREE;
	U32 blockOrder = Calculate_Order(block->size);
	U32 blockIdx = Calculate_Free_List_Idx(blockOrder);
	DEBUG_PRINTF("Block Idx: %d\r\n", blockIdx);
	Free_List_Push(block, blockIdx);

	/**
	 *  COALESCE BLOCKS
	 *
	 *  Iterate through loop until buddy address returned is NULL or buddy can't be coalesced.
	 *
	 *  Before we coalesce the blocks we check the following:
	 *    1) Size of the buddy
	 *    	If the size of the buddy doesn't match the size of the block, it indicates the buddy has split.
	 *    	The buddy can't be coalesced in this case as the buddy is partially full.
	 *    2) The type of the buddy
	 *    	If the type of the buddy is USED, the buddy can't be coalesced.
	 */

	Block* buddy = Get_Buddy(block);

	while (blockIdx > 0) {
		DEBUG_PRINTF("  INFO: The block has a buddy :) (Address = %x).\r\n", buddy);

		/*
		 * If the size of the buddy matches the size of the current block, the buddy hasn't been split.
		 * This means the buddy is either fully free or fully used.
		 */
		if (block->size == buddy->size) {
			DEBUG_PRINTF("  INFO: The buddy hasn't been split!\r\n");

			// If the buddy is FREE, the buddy is fully FREE so it can be coalesced.
			if (buddy->type == FREE) {
				DEBUG_PRINTF("  INFO: The buddy is fully free!\r\n");

				// If the buddy address is greater than the block address, the current block is the parent,
				// Else, the buddy is the parent and the block is the created buddy.
				if (buddy > block) {
					DEBUG_PRINTF("  INFO: Parent = Block.\r\n");
					Coalesce_Block(block, buddy);
					buddy = Get_Buddy(block);
				} else {
					DEBUG_PRINTF("  INFO: Parent = Buddy.\r\n");
					Coalesce_Block(buddy, block);
					block = buddy;
					buddy = Get_Buddy(buddy);
				}

				blockIdx = blockIdx - 1;
				DEBUG_PRINTF("Block Idx: %d\r\n", blockIdx);
			} else {
				DEBUG_PRINTF("  INFO: The buddy is fully used.\r\n");
				break;
			}
		} else {
			DEBUG_PRINTF("  INFO: The buddy has been split. Block Size: %x. Buddy Size: %x.\r\n", block->size, buddy->size);
			break;
		}
	}

	return RTX_OK;
}
