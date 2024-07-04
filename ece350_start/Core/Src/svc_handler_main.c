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

int SVC_Handler_Main( unsigned int *svc_args )
{
  unsigned int svc_number;

  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  DEBUG_PRINTF("System call number: %d\r\n", svc_number );

  switch( svc_number )
  {
    case TEST_ERROR:  /* EnablePrivilegedMode */
      break;
    case OS_CREATE_TASK:
    	DEBUG_PRINTF(" SVC CREATE TASK\r\n");

    	// Create task. Then check if newly created task has a sooner deadline. If so, yield to it

    	return createTask((TCB*)svc_args[0]);

    	break;
    case OS_YIELD:
    	DEBUG_PRINTF(" PERFORMING OS_YIELD\r\n");
    	if (kernelVariables.kernelStarted == 0) {
    		break;
    	}

    	// Save current task state.
    	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
    	__asm("isb");

    	break;
    case OS_KERNEL_START:
		// Check if the kernel is already initialized or running
    	if (kernelVariables.currentRunningTID != -1 || kernelVariables.kernelInitRan != 1){
    		DEBUG_PRINTF(" The kernel has not been initialized\r\n");

    		return RTX_ERR;
    	}

    	// Return, then perform yield
    	kernelVariables.kernelStarted = 1;
    	return RTX_OK;

    	break;
	case OS_TASK_EXIT:
		DEBUG_PRINTF(" TASK EXIT\r\n");

		// Check that the kernel has started and a task has been running
		int current_TID = kernelVariables.currentRunningTID;
		if(current_TID == -1){
			return RTX_ERR;
		}

		if(kernelVariables.tcbList[current_TID].state == RUNNING){
			// Reset the exiting tasks stackpointer to the top of the stack to be reused
			__set_PSP(kernelVariables.tcbList[kernelVariables.currentRunningTID].stack_high);
			
			// Change state to DORMANT removes the task from the scheduler */
			kernelVariables.tcbList[current_TID].state = DORMANT;
//			kernelVariables.numAvaliableTasks--;
			// Call the scheduler to yield and run the next task
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
			__asm("isb");

			return RTX_OK;
		}

		return RTX_ERR;

        break;
	case OS_TASK_INFO:
		DEBUG_PRINTF(" OS_TASK_INFO CALLED\r\n");

		int TID = (int) svc_args[0];
		TCB* task_copy = (TCB*) svc_args[1];
		// Check that the TID value is valid and exists
		if (TID >= 0 && TID < MAX_TASKS){
			TCB task = kernelVariables.tcbList[TID];

			// If in created state then TCB does not have wanted values, return error
			// Return error if task has already been exited and is dormant
			if(task.state == CREATED || task.state == DORMANT){
				return RTX_ERR;
			}

			/* Fill the TCB pointed to by task_copy with the fields TID specified */
			task_copy->args = task.args;
			task_copy->current_sp = task.current_sp;
			task_copy->ptask = task.ptask;
			task_copy->stack_high = task.stack_high;
			task_copy->stack_size = task.original_stack_size;
			task_copy->state = task.state;
			task_copy->tid = task.tid;
			task_copy->deadline_ms = task.deadline_ms;

			return RTX_OK;
		}

		return RTX_ERR;
		break;
	case OS_GET_TID:
		// Check that the kernel has started
		if(kernelVariables.currentRunningTID == -1){
			return RTX_ERR;
		}

		return kernelVariables.currentRunningTID;
		break;
    default:    /* unknown SVC */
    	break;
  }

  return RTX_ERR;
}

void contextSwitch(void) {
	// If there is a current running task, set it to running
	if (kernelVariables.currentRunningTID != -1) {
		// Find next task to run
		kernelVariables.tcbList[kernelVariables.currentRunningTID].current_sp = __get_PSP();

		// Update current task to READY if yielding from task, if exiting, state remains dormant
		if (kernelVariables.tcbList[kernelVariables.currentRunningTID].state == RUNNING){
			kernelVariables.tcbList[kernelVariables.currentRunningTID].state = READY;
		}
	}
	

	// Calls scheduler to run the next task 
	int nextTID = Scheduler();
	__set_PSP(kernelVariables.tcbList[nextTID].current_sp);

	kernelVariables.currentRunningTID = nextTID;
	kernelVariables.tcbList[nextTID].state = RUNNING;
	return;
}

void save_new_psp(void){
	kernelVariables.tcbList[kernelVariables.currentRunningTID].current_sp = kernelVariables.tcbList[kernelVariables.currentRunningTID].stack_high;
}

int createTask(TCB* task) {
	TCB* tcbs = kernelVariables.tcbList;

	// Check that task exists, there is enough stack space, and other criterias
	if (task == NULL) {
		DEBUG_PRINTF(" Failed to create task. User passed in NULL task.\r\n");
		return RTX_ERR;
	}

	if (task->stack_size < STACK_SIZE){
		DEBUG_PRINTF(" Failed to create task. Stack size too small.\r\n");
		return RTX_ERR;
	}

	if (task->ptask == NULL){
		DEBUG_PRINTF(" Failed to create task. Missing pointer to function\r\n");
		return RTX_ERR;
	}

//	if (kernelVariables.numAvaliableTasks == MAX_TASKS){
//		DEBUG_PRINTF(" Failed to create task. Reached maximum allowed tasks\r\n");
//		return RTX_ERR;
//	}

	int TIDtoOverwrite = -1;
	int TIDofEmptyTCB = MAX_SIGNED_INT_VALUE;
	int TCBStackSmallest = MAX_SIGNED_INT_VALUE;
	for (int i = 1; i < MAX_TASKS; i++) {
		// Found terminated task. Check if we can fit the new TCB into it.
		if (tcbs[i].state == DORMANT) {
			// Fragmentation, use TCB with smallest valid stack size
			if (task->stack_size <= tcbs[i].original_stack_size){
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

	// Check if there is an existing TCB that can be used for the task
	if (TIDtoOverwrite != -1) {
		DEBUG_PRINTF("Found TCB To Overwrite with TID: %d. Stack size: %d\r\n", TIDtoOverwrite, tcbs[TIDtoOverwrite].stack_size);

		tcbs[TIDtoOverwrite].ptask = task->ptask;
		tcbs[TIDtoOverwrite].state = READY;
		tcbs[TIDtoOverwrite].current_sp = tcbs[TIDtoOverwrite].stack_high;
		tcbs[TIDtoOverwrite].stack_size = task->stack_size;
		tcbs[TIDtoOverwrite].args = task->args;

		task->tid = TIDtoOverwrite;
//		kernelVariables.numAvaliableTasks++;

		Init_Thread_Stack((U32*)tcbs[TIDtoOverwrite].current_sp, task->ptask, TIDtoOverwrite);
		return RTX_OK;
	}

	// Check if there's a new TCB for the new task, copying so that task becomes ready to use
	if (TIDofEmptyTCB != 2147483647) {
		if (kernelVariables.totalStackUsed + task->stack_size > MAX_STACK_SIZE){
			DEBUG_PRINTF(" Failed to create task. Not enough memory\r\n");
			return RTX_ERR;
		}

		tcbs[TIDofEmptyTCB].ptask = task->ptask;
		tcbs[TIDofEmptyTCB].stack_high = (U32)Get_Thread_Stack(task->stack_size);
		tcbs[TIDofEmptyTCB].tid = TIDofEmptyTCB;
		tcbs[TIDofEmptyTCB].state = READY;
		tcbs[TIDofEmptyTCB].stack_size = task->stack_size;
		tcbs[TIDofEmptyTCB].current_sp = tcbs[TIDofEmptyTCB].stack_high;
		tcbs[TIDofEmptyTCB].original_stack_size = task->stack_size;
		tcbs[TIDofEmptyTCB].args = task->args;
		tcbs[TIDofEmptyTCB].deadline_ms = 5;

		task->tid = TIDofEmptyTCB;

		kernelVariables.totalStackUsed += task->stack_size;
//		kernelVariables.numAvaliableTasks++;

		Init_Thread_Stack((U32*)tcbs[TIDofEmptyTCB].current_sp, task->ptask, TIDofEmptyTCB);
		DEBUG_PRINTF("Found Empty TCB with TID: %d\r\n", TIDofEmptyTCB);
		return RTX_OK;
	}


	// All free TCB's are currently in use or there is no TCB with enough space to accommodate new task.
	DEBUG_PRINTF("Failed to create new task. All tasks are currently in use, or there is no TCB with enough space to accommodate new task\r\n");
	return RTX_ERR;
}


void Init_Thread_Stack(U32* stack_pointer, void (*callback)(void* args), int TID){
	DEBUG_PRINTF(" CURRENT_SP ADDRESS: %p\r\n", stack_pointer);
	*(--stack_pointer) = 1 << 24; // xPSR register, setting chip to "Thumb" mode
	*(--stack_pointer) = (uint32_t)callback; // PC Register storing next instruction
	
	// Initialize stack with arbitrary values (r4 - r11)
	for (int i = 0; i < 14; i++){
		*(--stack_pointer) = 0xA; //An arbitrary number
	}
	
	DEBUG_PRINTF(" NEW CURRENT_SP ADDRESS: %p\r\n", stack_pointer);
	kernelVariables.tcbList[TID].current_sp = (U32)stack_pointer;
}
