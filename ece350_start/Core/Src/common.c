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
	for (int i = kernelVariables.currentRunningTID + 1; i <= MAX_TASKS + kernelVariables.currentRunningTID ; i++) {
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

__attribute__((always_inline))
inline int EDFScheduler(void) {
	int TIDTaskToRun = 0;
	U32 shortestDeadline = kernelVariables.tcbList[kernelVariables.currentRunningTID].remainingTime;

	for (int i = 1; i < MAX_TASKS; i++) {
		TCB currentTCB = kernelVariables.tcbList[i];

		if (currentTCB.state == SLEEPING && currentTCB.remainingTime == 0) {
			currentTCB.state = READY;
			currentTCB.remainingTime = currentTCB.deadline_ms;
		}

		if (currentTCB.state == READY) {
			if (currentTCB.remainingTime <= shortestDeadline) {
				shortestDeadline = currentTCB.deadline_ms;
				TIDTaskToRun = i;
			}
		}
	}

//	DEBUG_PRINTF(" TID TO SCHEDULE: %d\r\n", TIDTaskToRun);
	return TIDTaskToRun;
}
