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

void osSetDeadline(int deadline, task_t TID) {
	int output;
	TRIGGER_SVC(OS_SET_DEADLINE);
	__asm("MOV %0, R0": "=r"(output));

}


void osPeriodYield(){
	TRIGGER_SVC(OS_PERIOD_YIELD);
}

void Null_Task_Function(void) {
	// DEBUG_PRINTF("  IN NULL TASK :(\r\n");

	while (1);

	return;
}

