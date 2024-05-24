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

// ------ GLobals ------
extern unsigned int numCreatedTasks;
extern uint32_t* p_threadStacks[]; // Array to store pointers to the top of each thread stack. (aka last value pushed to stack)
// ------ Globals -------


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
 * brief: Given some nth thread, we use this function to determine its starting stack address. Assumes all thread stacks are of same size.
 */
uint32_t* Get_Thread_Stack(unsigned int threadNum);


/*
 * brief: Kills a running thread by clearing the PendSV interrupt
 */
void Kill_Thread();


#endif /* INC_K_TASK_H_ */
