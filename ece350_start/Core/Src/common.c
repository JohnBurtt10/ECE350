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
				.next = NULL,
				.size = newSize, //https://piazza.com/class/lvlcv9pc4496o8/post/177
				.magicNum = MAGIC_NUMBER_BLOCK
	};

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
