/*
 * svc_handler_main.c
 *
 *  Created on: May 29, 2024
 *      Author: ethanromero
 */

#include "svc_handler_main.h"
#include "stm32f4xx_it.h"
#include "k_task.h"
#include "main.h"
#include <stdio.h>

/**
  * @brief Handles the actual system call by first retrieving the system call number and then doing whatever we want with it.
  */
int SVC_Handler_Main( unsigned int *svc_args )
{
  unsigned int svc_number;

  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  printf("System call number: %d\r\n", svc_number );
  switch( svc_number )
  {
    case TEST_ERROR:  /* EnablePrivilegedMode */
      break;
    case CREATE_TASK:

    	/*
    	 *  Using location of the top of thread's stack (i.e. last value of stackptr)
    	 *  pop 8 values from the stack into registers R4-R11.
    	 */
//    	__set_PSP((uint32_t) p_threadStacks[0]); //Setting the PSP register
//
//    	// https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-control-block/interrupt-control-and-state-register
//    	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
//    	__asm("isb");
    	printf("SVC CREATE TASK\r\n");
    	return createTask((TCB*)svc_args[0]);
    	break;
    case YIELD:
    	// Push current task registers to its thread stack
    	__set_PSP((U32)kernelVariables.tcbList[kernelVariables.currentRunningTID].stack_high);

    	// Iterate through TCB's, determine next run to run.
    	int TIDtaskToRun = -1;
    	for (int i = 0; i < kernelVariables.currentRunningTID; i++) {
    		if (kernelVariables.tcbList[i].state == READY) {
    			TIDtaskToRun = i;
    			break;
    		}
    	}

    	for (int i = kernelVariables.currentRunningTID; i < MAX_TASKS; i++) {
    		if (kernelVariables.tcbList[i].state == READY) {
    			TIDtaskToRun = i;
    			break;
    		}
    	}

    	// TODO: null task case

    	__set_PSP((U32)kernelVariables.tcbList[TIDtaskToRun].current_sp);



    	kernelVariables.currentRunningTID = TIDtaskToRun;
    	break;
    default:    /* unknown SVC */
      break;
  }

  return 0;
}

int createTask(TCB* task) {
	TCB* tcbs = kernelVariables.tcbList;

	if (task->stack_size < MIN_THREAD_STACK_SIZE || task->ptask == NULL){
		DEBUG_PRINTF("Failed to create task. Stack size too small or missing ptr to function\r\n");
		return RTX_ERR;
	}

	if (kernelVariables.numAvaliableTasks == MAX_TASKS || kernelVariables.totalStackUsed + task->stack_size > MAX_STACK_SIZE){
		DEBUG_PRINTF("Failed to create task. Not enough memory or reached maximum allowed tasks\r\n");
		return RTX_ERR;
	}

	int TIDtoOverwrite = -1;
	int TIDofEmptyTCB = 2147483647;
	int TCBStackSmallest = 2147483647;
	for (int i = 1; i < MAX_TASKS; i++) {
		// Found terminated task. Check if we can fit the new TCB into it.
		if (tcbs[i].state == DORMANT) {
			if (tcbs[i].original_stack_size >= task->stack_size){
				if (tcbs[i].original_stack_size < TCBStackSmallest){
					TCBStackSmallest = tcbs[i].original_stack_size;
					TIDtoOverwrite = i;
				}
			}
		}

		// Found uninitialized TCB
		int currentTID = i;
		if (tcbs[i].state == CREATED){
			if (currentTID <= TIDofEmptyTCB) {
				TIDofEmptyTCB = i;
			}
		}
	}

	if (TIDtoOverwrite != -1) {
		DEBUG_PRINTF("Found TCB To Overwrite with TID: %d. Stack size: %d\r\n", TIDtoOverwrite, tcbs[TIDtoOverwrite].stack_size);

		tcbs[TIDtoOverwrite].ptask = task->ptask;
		tcbs[TIDtoOverwrite].state = READY;
		tcbs[TIDtoOverwrite].current_sp = tcbs[TIDtoOverwrite].stack_high;
		tcbs[TIDtoOverwrite].stack_size = task->stack_size;
		tcbs[TIDtoOverwrite].args = task->args;
		kernelVariables.numAvaliableTasks++;
		return RTX_OK;
	}

	if (TIDofEmptyTCB != 2147483647) {
		tcbs[TIDofEmptyTCB].ptask = task->ptask;
		tcbs[TIDofEmptyTCB].stack_high = (U32)Get_Thread_Stack(task->stack_size);
		tcbs[TIDofEmptyTCB].tid = TIDofEmptyTCB;
		tcbs[TIDofEmptyTCB].state = READY;
		tcbs[TIDofEmptyTCB].stack_size = task->stack_size;
		tcbs[TIDofEmptyTCB].current_sp = tcbs[TIDofEmptyTCB].stack_high;
		tcbs[TIDofEmptyTCB].original_stack_size = task->stack_size;
		tcbs[TIDofEmptyTCB].args = task->args;

		task->tid = TIDofEmptyTCB;
		kernelVariables.totalStackUsed += task->stack_size;
		kernelVariables.numAvaliableTasks++;

		DEBUG_PRINTF("Found Empty TCB with TID: %d\r\n", TIDofEmptyTCB);
		return RTX_OK;
	}

	// All free TCB's are currently in use or there is no TCB with enough space to accommodate new task.
	DEBUG_PRINTF("Failed to create new task. All tasks are currently in use, or there is no TCB with enough space to accommodate new task\r\n");
	return RTX_ERR;
}

