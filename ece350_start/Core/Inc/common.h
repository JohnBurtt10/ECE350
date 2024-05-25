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

#define DEBUG_ENABLE // Comment me out to disable debugging

#ifdef DEBUG_ENABLE
	#define DEBUG_PRINTF(fmt, ...) printf("DEBUG_PRINTF<<" fmt, ##__VA_ARGS__)
#else
	 #define DEBUG_PRINTF(fmt, ...)
#endif

// ----------- SVC CALLS -----------
#define TEST_ERROR 0
#define CREATE_THREAD 1

// Treat stack as giant array of integers. Break up stack and keep track of multiple stacks for threads
#define MAX_STACK_SIZE 0x4000 // Must match _Min_Stack_Size in the linker script

// ------ SUBJECT TO CHANGE ----------
#define MAIN_STACK_SIZE 0x400 // 1024 bytes. This stores interrupts, setup, and os-specific stuff
#define MIN_THREAD_STACK_SIZE 0x200 // 1024 bytes. Defined in prelab 1

#define TID_NULL 0 //predefined Task ID for the NULL task
#define MAX_TASKS 16 //maximum number of tasks in the system

#define RTX_OK 1
#define RTX_ERR 0

// ---- Types -------
typedef unsigned int U32;
typedef unsigned short U16;
typedef char U8;
typedef unsigned int task_t;
// ---- End Of Types ------

// ---- Thread States -----
#define DORMANT 0 //state of terminated task
#define READY  1 //state of task that can be scheduled but is not running
#define RUNNING 2 //state of running task
#define CREATED 3
// ----- End Of Thread States --------

typedef struct task_control_block{
	void (*ptask)(void* args); //entry address
	U32 stack_high; //starting address of stack (high address)
	task_t tid; //task ID
	U8 state; //task's state
	U16 stack_size; //stack size. Must be a multiple of 8
	U32 current_sp; // top of stack
	void* args; // Arguments for function
} TCB;

typedef struct kernel_variables {
	unsigned int numCreatedTasks; // Num of running and ready TCBs
	unsigned int totalStackUsed;
	TCB tcbList[MAX_TASKS];
	int currentRunningTID;
} Kernel_Variables;

extern Kernel_Variables kernelVariables;

uint32_t* Get_MSP_INIT_VAL();

// Process Stack Pointer register (PSP) represents thread stack pointer
uint32_t* Get_Process_Stack_PTR();

void Trigger_System_Call(unsigned int systemCall);

// Checks how much of the whole stack is currently used
unsigned int Get_Total_Memory_Used();

#endif /* INC_COMMON_H_ */
