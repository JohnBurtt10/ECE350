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
void print_continuously(void);

/*
 * brief: Initializes thread stack with given function call and arbitrary values.
 */
void Init_Thread_Stack(uint32_t** p_threadStack, void (*callback)());

/*
 * @brief: Find space to create a new thread stack
 * @retval: returns stack_high address of new stack on success. NULL on failure due to not enough memory
 * @param: Size of stack to allocate
 */
uint32_t* Get_Thread_Stack(unsigned int stack_size);
uint32_t* Get_Thread_Stack_OLD(unsigned int threadNum);

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

/*
 * @brief:  immediately halts the execution of one task, saves it contexts, runs the scheduler, and loads the context of the next task to run.
 * 1) When Yielding from current running task, push task registers to thread stack
 * 2) iterate through scheduler to determine next task to run.
 * 3) Pop registers from new task and update registers on microcontroller.
 * 4)
 */
void osYield(void);


#endif /* INC_K_TASK_H_ */
