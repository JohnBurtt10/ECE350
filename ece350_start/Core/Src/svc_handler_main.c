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
#include "common.h"
#include "k_mem.h"
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
//  DEBUG_PRINTF("System call number: %d\r\n", svc_number );

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
//    	DEBUG_PRINTF(" PERFORMING OS_YIELD\r\n");
    	if (kernelVariables.kernelStarted == 0) {
    		break;
    	}

		// Reset task's time remaining back to its deadline
    	kernelVariables.tcbList[kernelVariables.currentRunningTID].remainingTime = kernelVariables.tcbList[kernelVariables.currentRunningTID].deadline_ms;

    	// Save current task state.
    	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
    	__asm("isb");

    	break;
    case OS_KERNEL_START:
		// Check if the kernel is already initialized or running
    	if (kernelVariables.kernelInitRan == 0){
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
		if(kernelVariables.currentRunningTID == -1){
			return RTX_ERR;
		}

		TCB* currentTask = &kernelVariables.tcbList[kernelVariables.currentRunningTID];
		if(currentTask->state == RUNNING){
			// Reset the exiting tasks stackpointer to the top of the stack to be reused
			__set_PSP(currentTask->stack_high);
			
			// Change state to DORMANT removes the task from the scheduler */
			currentTask->state = DORMANT;

			k_mem_dealloc((void*) (currentTask->stack_high - currentTask->stack_size));
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
			task_copy->current_sp = task.current_sp;
			task_copy->ptask = task.ptask;
			task_copy->stack_high = task.stack_high;
			task_copy->stack_size = task.stack_size;
			task_copy->state = task.state;
			task_copy->tid = task.tid;
			task_copy->deadline_ms = task.deadline_ms;
			task_copy->remainingTime = task.remainingTime;

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
	case OS_SET_DEADLINE:
		int deadline = (int) svc_args[0];
		TID = (task_t) svc_args[1];

		if ((deadline < 0) || TID == &kernelVariables.tcbList[kernelVariables.currentRunningTID] || TID == 0) {
		DEBUG_PRINTF("  osSetDeadline received an invalid deadline or TID, returning RTX_ERROR\r\n");
		return RTX_ERR;
		}

		if (kernelVariables.tcbList[TID].state != READY) {
			return RTX_ERR;
		}

		kernelVariables.tcbList[TID].deadline_ms = deadline;

		return RTX_OK;

		/*
		 * After updating deadline, run EDF Scheduler
		 * If the current running TID differs from scheduler, pre-empt current running task.
		*/

		break;
	case OS_SLEEP:
		int timeInMs = (int) svc_args[0];

		// Set current running task to status of sleep, and set its remainingTIme to timeInMs
		TCB* currentTCB = &kernelVariables.tcbList[kernelVariables.currentRunningTID];
		currentTCB->state = SLEEPING;
		currentTCB->remainingTime = timeInMs;

		// Save current task state.
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
		__asm("isb");

		break;
	case OS_CREATE_DEADLINE_TASK:
		if ((U32)svc_args[0] <= 0) {
			return RTX_ERR;
		}

		TCB* tcb = (TCB*)svc_args[1];
		int result = createTask(tcb);
		kernelVariables.tcbList[tcb->tid].deadline_ms = (U32) svc_args[0];
		kernelVariables.tcbList[tcb->tid].remainingTime = (U32) svc_args[0];

		if (result == RTX_OK) {
			// If the caller of the function has a longer deadline, then we should preempt it with the newly created task.
			if (kernelVariables.tcbList[kernelVariables.currentRunningTID].remainingTime > ((TCB*)svc_args[1])->remainingTime
					&& kernelVariables.kernelStarted) {
				SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
				__asm("isb");
			}

			return RTX_OK;
		}

		break;
	case OS_PERIOD_YIELD:
		// Check that kernel has started and a task has started
		if(kernelVariables.currentRunningTID == -1){
			return RTX_ERR;
		}
		
		TCB* currentTCB2 = &kernelVariables.tcbList[kernelVariables.currentRunningTID];
		// Verify that periodic task has completed the current instance
		// Check if remaining period time is <0 (current time period elapses) (at deadline or deadline is missed), so soft deadline so it will be reset
		// DEBUG_PRINTF("Task %d, remaining time: %d, deadline: %d, state: %d\r\n", kernelVariables.currentRunningTID, currentTCB2->remainingTime, currentTCB2->deadline_ms, currentTCB2->state);
		if(currentTCB2->remainingTime <= 0){ 
			// Task is only ready when the current period is completed
			currentTCB2->state = READY;
			// Reset task's time remaining back to its deadline
			currentTCB2->remainingTime = currentTCB2->deadline_ms;
			// DEBUG_PRINTF("Current time period elapses, adding task to scheduler\r\n");
		}
		else{
			currentTCB2->state = SLEEPING;
			// DEBUG_PRINTF("Period not elapsed\r\n");
		}

		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Trigger PendSV_Handler
			__asm("isb");
		
		break;
    default:    /* unknown SVC */
    	break;
  }

  return RTX_ERR;
}

void contextSwitch(void) {
	// If there is a current running task, set it to ready
	if (kernelVariables.currentRunningTID != -1) {
		// Find next task to run
		TCB* currentTCB = &kernelVariables.tcbList[kernelVariables.currentRunningTID];
		currentTCB->current_sp = __get_PSP();

		// Update current task to READY if yielding from task, if exiting, state remains dormant
		if (currentTCB->state == RUNNING){
			currentTCB->state = READY;
		}
	}
	

	// Calls scheduler to run the next task 
	int nextTID = EDFScheduler();
	__set_PSP(kernelVariables.tcbList[nextTID].current_sp);

	kernelVariables.currentRunningTID = nextTID;
	kernelVariables.tcbList[nextTID].state = RUNNING;
	return;
}

void save_new_psp(void){
	kernelVariables.tcbList[kernelVariables.currentRunningTID].current_sp = kernelVariables.tcbList[kernelVariables.currentRunningTID].stack_high;
}

int createTask(TCB* task) {
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

	/*
	 * Since we our using our new heap to allocate/deallocate. We can just attempt to allocate a new memory region. If it fails, we do not have space.
	 */

	void* block = k_mem_alloc(task->stack_size);

	if (block != NULL) {
		// Iterate through all blocks and find a free block.
		for (int i = 1; i < MAX_TASKS; i++) {
			TCB* currentTCB = &kernelVariables.tcbList[i];
			if (currentTCB->state == DORMANT || currentTCB->state == CREATED) {
				currentTCB->ptask = task->ptask;
				currentTCB->stack_high = (U32) block + task->stack_size;
				currentTCB->state = READY;
				currentTCB->current_sp = currentTCB->stack_high;
				currentTCB->deadline_ms = 5;
				currentTCB->stack_size = task->stack_size;
				currentTCB->remainingTime = 5;

				Block* aucBlock = (Block*) ACTUAL_BLOCK(block);
				aucBlock->TIDofOwner = i;

				task->tid = i;

				Init_Thread_Stack((U32*)currentTCB->current_sp, task->ptask, i);

				return RTX_OK;
			}
		}
	}

	// All free TCB's are currently in use or there is no TCB with enough space to accommodate new task.
	DEBUG_PRINTF("Failed to create new task. All tasks are currently in use, or there is no TCB with enough space to accommodate new task\r\n");
	return RTX_ERR;
}


inline void Init_Thread_Stack(U32* stack_pointer, void (*callback)(void* args), int TID){
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
