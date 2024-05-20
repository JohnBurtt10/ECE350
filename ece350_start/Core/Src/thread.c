/*
 * thread.c
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */
#include "thread.h"
#include "common.h"

unsigned int numThreads = 0;

uint32_t* Create_Thread_Stack() {
	// To create a new thread stack, we will take a chunk from our main stack.
	uint32_t* main_stack_pointer = Get_MSP_INIT_VAL();

	// ARM Cortex architecture grows stack grows downwards (high address to low address)


	numThreads += 1;

	return 0x0;
}
