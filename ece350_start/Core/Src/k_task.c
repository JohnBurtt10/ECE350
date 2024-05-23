/*
 * k_task.c
 *
 *  Created on: May 21, 2024
 *      Author: Ethan Romero
 */
#include "k_task.h"
#include "common.h"
#include "main.h"
#include <stdio.h>

//unsigned int numThreads = 0;
//uint32_t* p_threadStacks[(MAX_STACK_SIZE-MAIN_STACK_SIZE)/THREAD_STACK_SIZE];

uint32_t* Create_Thread() {
	// Offset from main stack (portion of stack containing interrupts, setups, etc)
	numCreatedTasks += 1;
	uint32_t* p_threadStack = Get_Thread_Stack(numCreatedTasks);

	// Once we have the new pointer for the thread stack, we can now setup its stack and context
	// This function will make the stack pointer, point to bottom of stack (technically top since its the last value pushed to it)
	Init_Thread_Stack(&p_threadStack, &print_continuously);
	p_threadStacks[numCreatedTasks - 1] = p_threadStack;

	Trigger_System_Call(CREATE_THREAD);

	return p_threadStack;
}

void Init_Thread_Stack(uint32_t** p_threadStack, void (*callback)()){
	*(--*p_threadStack) = 1 << 24; // xPSR register, setting chip to "Thumb" mode
	*(--*p_threadStack) = (uint32_t)callback; // PC Register storing next instruction
	for (int i = 0; i < 14; i++){
		*(--*p_threadStack) = 0xA; //An arbitrary number
	}
}

uint32_t* Get_Thread_Stack(unsigned int threadNum){
    // printf("MSP_INIT: %p\r\n", Get_MSP_INIT_VAL());
	// ARM Cortex architecture grows stack grows downwards (high address to low address)
	return (uint32_t*)(( (unsigned int)Get_MSP_INIT_VAL() - MAIN_STACK_SIZE ) - (threadNum-1)*THREAD_STACK_SIZE);
}

void print_continuously(void){
	while(1){
		printf("Thread2222\r\n");
	}
}

void Kill_Thread() {
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk; // Kills thread
	return;
}
