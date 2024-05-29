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

void Trigger_System_Call(unsigned int systemCall) {;
	if (systemCall == TEST_ERROR){
		__asm("SVC 0");
	} else if (systemCall == CREATE_THREAD) {
		__asm("SVC 1");
	}
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

	for (int i = 1; i < kernelVariables.currentRunningTID; i++) {
		if (kernelVariables.tcbList[i].state == READY) {
			TIDtaskToRun = i;
			break;
		}
	}

	for (int i = kernelVariables.currentRunningTID; i < MAX_TASKS; i++) {
		if (kernelVariables.tcbList[i].state == READY) {
			TIDtaskToRun = i;
			break;
		}
	}

	return TIDTaskToRun;
}
