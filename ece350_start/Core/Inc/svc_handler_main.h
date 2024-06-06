/*
 * svc_handler_main.h
 *
 *  Created on: May 29, 2024
 *      Author: ethanromero
 */

#ifndef INC_SVC_HANDLER_MAIN_H_
#define INC_SVC_HANDLER_MAIN_H_

#include "common.h"

/**
 * @brief:
 * 1) Iterate through all TCB's and determine if there exists an uninitialized TCB.
 * 		a) If there exists an uninitialized TCB, then use it.
 * 			i) Before writing to it, make sure there exists enough memory to allocate to it!
 * 		b) If there does not, check if one is DORMANT, and use its stack IF AND ONLY IF, the task->stack_size < dormant task size
 * Assigns a unique task id to the created task
 * @param: TCB to be allocated with task, whose TID with be updated
 * @retval: RTX_OK on success, RTX_ERR if failed to created task
 */
int createTask(TCB* task);

/**
 * @brief: Initializes thread stack with given function call and arbitrary values.
 */
void Init_Thread_Stack(U32* stack_pointer, void (*callback)(void* args), int TID);

/**
 * @brief: Gets the next available task to run and calls the scheduler to run it
*/
void contextSwitch(void);

/**
 * @brief: Used by the pendsv handler to set the psp back to the starting address after popping all registers
*/
void save_new_psp(void);

/**
  * @brief Handles the actual system call by first retrieving the system call number and then doing whatever we want with it.
  */
int SVC_Handler_Main( unsigned int *svc_args );

#endif /* INC_SVC_HANDLER_MAIN_H_ */
