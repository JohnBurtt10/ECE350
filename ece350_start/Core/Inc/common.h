/*
 * common.h
 *
 *  Created on: Jan 5, 2024
 *      Author: nexususer
 *
 *      NOTE: If you feel that there are common
 *      C functions corresponding to this
 *      header, then any C functions you write must go into a corresponding c file that you create in the Core->Src folder
 */
#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdint.h>

// ----------- SVC CALLS -----------
#define TEST_ERROR 0
#define CREATE_THREAD 1

// Treat stack as giant array of integers. Break up stack and keep track of multiple stacks for threads
#define MAX_STACK_SIZE 0x4000 // Must match _Min_Stack_Size in the linker script

// ------ SUBJECT TO CHANGE ----------
#define MAIN_STACK_SIZE 0x400 // 1024 bytes. This stores interrupts, setup, and os-specific stuff
#define THREAD_STACK_SIZE 0x400 // 1024 bytes. Defined in prelab 1

#define TID_NULL 0 //predefined Task ID for the NULL task
#define MAX_TASKS (MAX_STACK_SIZE - MAIN_STACK_SIZE)/THREAD_STACK_SIZE //maximum number of tasks in the system
#define DORMANT 0 //state of terminated task
#define READY 1 //state of task that can be scheduled but is not running
#define RUNNING 2 //state of running task


uint32_t* Get_MSP_INIT_VAL();

// Process Stack Pointer register (PSP) represents thread stack pointer
uint32_t* Get_Process_Stack_PTR();

void Trigger_System_Call(unsigned int systemCall);

#endif /* INC_COMMON_H_ */
