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
    case OS_CREATE_TASK:
    	DEBUG_PRINTF(" SVC CREATE TASK\r\n");
    	return createTask((TCB*)svc_args[0]);
    	break;
    case OS_YIELD:
    	DEBUG_PRINTF(" PERFORMING OS_YIELD\r\n");
    	// Save current task state.
    	__set_PSP(kernelVariables.tcbList[kernelVariables.currentRunningTID].current_sp);
    	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
    	__asm("isb");
    	break;
    case OS_KERNEL_START:
    	if (kernelVariables.currentRunningTID != -1 || kernelVariables.kernelInitRan != 1){
    		DEBUG_PRINTF(" The kernel has not been initialized\r\n");
    		return RTX_ERR;
    	}

    	// No error, then run first avaliable task
    	return RTX_OK;
    	break;
    default:    /* unknown SVC */
      break;
  }

  return RTX_ERR;
}

void contextSwitch(void) {
	if (kernelVariables.currentRunningTID != -1) {
		// Find next task to run
		kernelVariables.tcbList[kernelVariables.currentRunningTID].current_sp = __get_PSP();
		kernelVariables.tcbList[kernelVariables.currentRunningTID].state = READY;
	}

	int nextTID = Scheduler();
	__set_PSP(kernelVariables.tcbList[nextTID].current_sp);

	kernelVariables.currentRunningTID = nextTID;
	kernelVariables.tcbList[nextTID].state = RUNNING;
	return;
}

int createTask(TCB* task) {
	TCB* tcbs = kernelVariables.tcbList;

	if (task->stack_size < MIN_THREAD_STACK_SIZE){
		DEBUG_PRINTF(" Failed to create task. Stack size too small.\r\n");
		return RTX_ERR;
	}

	if (task->ptask == NULL){
			DEBUG_PRINTF(" Failed to create task. Missing pointer to function\r\n");
			return RTX_ERR;
		}

	if (kernelVariables.numAvaliableTasks == MAX_TASKS){
		DEBUG_PRINTF(" Failed to create task. Reached maximum allowed tasks\r\n");
		return RTX_ERR;
	}

	if (kernelVariables.totalStackUsed + task->stack_size > MAX_STACK_SIZE){
			DEBUG_PRINTF(" Failed to create task. Not enough memory\r\n");
			return RTX_ERR;
		}

	int TIDtoOverwrite = -1;
	int TIDofEmptyTCB = MAX_SIGNED_INT_VALUE;
	int TCBStackSmallest = MAX_SIGNED_INT_VALUE;
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

		Init_Thread_Stack((U32*)tcbs[TIDtoOverwrite].current_sp, &task->ptask, TIDtoOverwrite);
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

		Init_Thread_Stack((U32*)tcbs[TIDofEmptyTCB].current_sp, &task->ptask, TIDofEmptyTCB);
		DEBUG_PRINTF("Found Empty TCB with TID: %d\r\n", TIDofEmptyTCB);
		return RTX_OK;
	}

	// All free TCB's are currently in use or there is no TCB with enough space to accommodate new task.
	DEBUG_PRINTF("Failed to create new task. All tasks are currently in use, or there is no TCB with enough space to accommodate new task\r\n");
	return RTX_ERR;
}

void Init_Thread_Stack(uint32_t* stack_pointer, void (*callback)(void* args), int TID){
	DEBUG_PRINTF(" CURRENT_SP ADDRESS: %p\r\n", stack_pointer);
	*(--stack_pointer) = 1 << 24; // xPSR register, setting chip to "Thumb" mode
	*(--stack_pointer) = (uint32_t)callback; // PC Register storing next instruction
	for (int i = 0; i < 14; i++){
		*(--stack_pointer) = 0xA; //An arbitrary number
	}
	DEBUG_PRINTF(" NEW CURRENT_SP ADDRESS: %p\r\n", stack_pointer);
	kernelVariables.tcbList[TID].current_sp = (U32)stack_pointer;
}

