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

// ------ Globals ------
extern uint32_t* p_threadStacks[]; // Array to store pointers to the top of each thread stack. (aka last value pushed to stack)
// ------ End of Globals -------


/*
 * brief: Creates a new thread stack. Returns pointer to top of stack.
 */
uint32_t* Create_Thread();

/*
 * brief: Simple function used to test thread functionality
 */
void anakin(void);

/*
 * brief: Simple function used to test thread functionality
 */
void obiwan(void);

/*
 * @brief: Find space to create a new thread stack
 * @retval: returns stack_high address of new stack on success. NULL on failure due to not enough memory
 * @param: Size of stack to allocate
 */
uint32_t* Get_Thread_Stack(unsigned int stack_size);

/*
 * brief: Kills a running thread by clearing the PendSV interrupt
 */
void Kill_Thread(void);

/*
 * @brief: Create a new task and register it with the RTX if possible
 * @param: TCB of task to create.
 * @retval: RTX_OK on success, RTX_ERR on failure
 */
int osCreateTask(TCB* task);

/*
 * @brief: Retrieve the informaƟon from the TCB of the task with id TID, and fill the TCB pointed to by task_copy
 *   		with all of its fields, if a task with the given TID exists
 * @param: TID and pointer to a task to update.
 * @retval: RTX_ERR on failure, RTX_OK on success
 */
int osTaskInfo(task_t TID, TCB* task_copy);

/* Make every os____ a svc call. osKernelStart should start a task by BX LR into it using pendSV. (set stack for null task)
 * @brief:  immediately halts the execution of one task, saves it contexts, runs the scheduler, and loads the context of the next task to run.
 * 1) When Yielding from current running task to new task, push old task context to thread stack
 * 		a) New tasks should have stack initalized with arbitary values done as prelab.
 * 		b) Store args into R0
 * 2) Iterate through scheduler to determine next task to run.
 * 3) Pop registers from new task stack and update registers on microcontroller.
 */
void osYield(void);

int osTaskExit(void);

int osKernelStart(void);

/*
 * brief: Function to be ran by null task. Continuously calls osYield to yield
 */
void Null_Task_Function(void);

#endif /* INC_K_TASK_H_ */
