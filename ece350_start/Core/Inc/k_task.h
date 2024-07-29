/*
 * k_task.h
 *
 *  Created on: Jan 5, 2024
 *      Author: nexususer
 *
 *      NOTE: any C functions you write must go into a corresponding c file that you create in the Core->Src folder
 */

#ifndef INC_K_TASK_H_
#define INC_K_TASK_H_

#include <stdint.h>
#include "common.h"

#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 //PendSV is bits 23-16

// https://developer.arm.com/documentation/101407/0540/Debugging/Debug-Windows-and-Dialogs/Core-Peripherals/Armv7-M-cores/Armv7-M--System-Tick-Timer
#define ST_CURRENT *(uint32_t*) 0xE000E018 // SysTick Current Value Register

/**
 * @brief: Creates a new thread stack. Returns pointer to top of stack.
 */
uint32_t* Create_Thread();

/**
 * @brief: Find space to create a new thread stack
 * @retval: returns stack_high address of new stack on success. NULL on failure due to not enough memory
 * @param: Size of stack to allocate
 */
uint32_t* Get_Thread_Stack(unsigned int stack_size);

/**
 * @brief: Create a new task and register it with the RTX if possible
 * @param: TCB of task to create.
 * @retval: RTX_OK on success, RTX_ERR on failure
 */
int osCreateTask(TCB* task);

/**
 * @brief: Retrieve the informaƟon from the TCB of the task with id TID, and fill the TCB pointed to by task_copy
 *   		with all of its fields, if a task with the given TID exists
 * @param: TID and pointer to a task to update.
 * @retval: RTX_ERR on failure, RTX_OK on success
 */
int osTaskInfo(task_t TID, TCB* task_copy);

/** Make every os____ a svc call. osKernelStart should start a task by BX LR into it using pendSV. (set stack for null task)
 * @brief:  immediately halts the execution of one task, saves it contexts, runs the scheduler, and loads the context of the next task to run.
 * 1) When Yielding from current running task to new task, push old task context to thread stack
 * 		a) New tasks should have stack initalized with arbitary values done as prelab.
 * 		b) Store args into R0
 * 2) Iterate through scheduler to determine next task to run.
 * 3) Pop registers from new task stack and update registers on microcontroller.
 */
void osYield(void);

/**
 * @brief: Causes a running task to exit and uses the scheduler to call the next task ready to run. 
 * Resets the stack pointer to be reused and sets the task to DORMANT
 * @retval: RTX_OK if called by a running task, else returns RTX_ERR
*/
int osTaskExit(void);

/**
 * @brief: Starts the first task by calling the scheduler. 
*/
int osKernelStart(void);

/**
 * @brief: Function to be ran by null task. Continuously calls osYield to yield
 */
void Null_Task_Function(void);

/**
 * @brief: Returns the TID of a task, used by the user application. 
 * Returns 0 if the Kernel has not started
 */
task_t osGetTID(void);

void osSleep(int timeInMs);

int osCreateDeadlineTask(int deadline, TCB* task);

/**
 * @brief: Schedules the periodic task only if the current period has elapsed
 */
void osPeriodYield();

int osSetDeadline(int deadline, task_t TID);

#endif /* INC_K_TASK_H_ */
