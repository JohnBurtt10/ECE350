/*
 * k_task.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */
#include "k_task.h"
#include "main.h"
#include "common.h"
#include <stdio.h>
#include <stdint.h>

int osCreateTask(TCB* task) {
	int output;

	TRIGGER_SVC(OS_CREATE_TASK);
	__asm("MOV %0, R0": "=r"(output));

	return output;
}

int osCreateDeadlineTask(int deadline, TCB* task) {
	int output;
	TRIGGER_SVC(OS_CREATE_DEADLINE_TASK);
	__asm("MOV %0, R0": "=r"(output));
	return output;
}

void osYield(void) {
	TRIGGER_SVC(OS_YIELD);
}

int osKernelStart(void) {
	int output;

	TRIGGER_SVC(OS_KERNEL_START);
	__asm("MOV %0, R0": "=r"(output));
	/* If the kernel was successful started, call osYield to run the first task */
	if (output == RTX_OK){
		TRIGGER_SVC(OS_YIELD);
	}

	return output;
}

int osTaskInfo(task_t TID, TCB* task_copy) {
	// Checks that the TCB at the pointer exists
	if (task_copy == NULL){
		return RTX_ERR;
	}

	int output;

	TRIGGER_SVC(OS_TASK_INFO);
	__asm("MOV %0, R0": "=r"(output));

	return output;
}

task_t osGetTID (void) {
	task_t tid;

	TRIGGER_SVC(OS_GET_TID);
	__asm("MOV %0, R0": "=r"(tid));

	return tid;
}

int osTaskExit(void){
	int taskExitStatus;

	TRIGGER_SVC(OS_TASK_EXIT);
	__asm("MOV %0, R0": "=r"(taskExitStatus));

	return taskExitStatus;
}

void osSleep(int timeInMs) {
	TRIGGER_SVC(OS_SLEEP);
}

int osSetDeadline(int deadline, task_t TID) {

	int taskFound = 0;

	if (!(deadline < 0) || TID == keneralVariables.currentRunningTID) {
		DEBUG_PRINTF("  osSetDeadline received an invalid deadline or TID, returning RTX_ERROR\r\n");
		return RTX_ERR;
	}

	for (int i = 0; i < MAX_TASKS; i++) {
		if (TID == kernelVariables.tcbList[i].TID) {
			DEBUG_PRINTF("  osSetDeadline found a TCB with the specified TID\r\n");
			taskFound = 1;
			kernelVariables.tcbList[i].deadline = deadline;
			Scheduler();
			break;
		}
	}

	return (RTX_OK ? taskFound: RTX_ERR);

	// iterate through tasks until the one with TID equal to TID is found
	// call scheduler

}

void Null_Task_Function(void) {
	DEBUG_PRINTF("  IN NULL TASK :(\r\n");

	while (1);

	return;
}

