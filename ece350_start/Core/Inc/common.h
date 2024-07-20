/*
 * common.h
 *
 *  Created on: Jan 5, 2024
 *      Author: nexususer
 *
 *      NOTE: If you feel that there are common
 *      C functions corresponding to this
 *      header, then any C functions you write must go into a corresponding c file that you create in the Core->Src folder
 */
#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdint.h>
#define DEBUG_ENABLE // Comment me out to disable debugging

#ifdef DEBUG_ENABLE
	#define DEBUG_PRINTF(fmt, ...) printf("DEBUG_PRINTF<<" fmt, ##__VA_ARGS__)
#else
	 #define DEBUG_PRINTF(fmt, ...)
#endif

#define TRIGGER_SVC(X) __asm("SVC %0" : : "i" (X))

#define SYST_CSR (uint32_t*) 0xE000E010


#define DISABLE_SYSTICK_INT *SYST_CSR ^= 1 << 1
#define ENABLE_SYSTICK_INT *SYST_CSR |= 1 << 1


// ----------- SVC CALLS -----------
#define TEST_ERROR 0
#define OS_CREATE_TASK 1
#define OS_YIELD 2
#define OS_KERNEL_START 3
#define OS_TASK_EXIT 4
#define OS_TASK_INFO 5
#define OS_GET_TID 6
#define OS_SET_DEADLINE 7
#define OS_SLEEP 8
#define OS_CREATE_DEADLINE_TASK 9
#define OS_PERIOD_YIELD 10

// Treat stack as giant array of integers. Break up stack and keep track of multiple stacks for threads
#define MAX_STACK_SIZE 0x4000 // Must match _Min_Stack_Size in the linker script

// ------ SUBJECT TO CHANGE ----------
#define MAIN_STACK_SIZE 0x400 // This stores interrupts, setup, and os-specific stuff
#define STACK_SIZE 0x200

#define TID_NULL 0 //predefined Task ID for the NULL task
#define MAX_USER_TASKS MAX_TASKS - 1 // Maximum number of user tasks.
#define NULL_TASK_STACK_SIZE 0x400

#define RTX_OK 1
#define RTX_ERR 0
#define MAX_SIGNED_INT_VALUE 2147483647

// ---- Types -------
typedef unsigned int U32;
typedef unsigned short U16;
typedef char U8;
typedef unsigned int task_t;
// ---- End Of Types ------

// ---- Thread States -----
#define DORMANT 0 //state of terminated task
#define READY  1 //state of task that can be scheduled but is not running
#define RUNNING 2 //state of running task
#define SLEEPING 3
#define CREATED 4
// ----- End Of Thread States --------

// ----- MALLOC stuff -----------
#define LOWEST_RAM_ADDRESS 0x20000000
#define MIN_BLOCK_ORDER 5 // Min size of a block.
#define MIN_BLOCK_SIZE (1 << MIN_BLOCK_ORDER) // 32 Bytes. ALSO INCLUDES METADATA
#define MAX_ORDER 10
#define HEIGHT_OF_TREE (MAX_ORDER + 1)
#define NUMBER_OF_NODES ((1 << HEIGHT_OF_TREE) - 1)
#define USED 1
#define FREE 0

typedef struct task_control_block{
	void (*ptask)(void* args); //entry address
	U32 stack_high; //starting address of stack (high address)
	U16 tid; //task ID
	U8 state; //task's state
	U16 stack_size; //stack size. Must be a multiple of 8
	U32 current_sp; // top of stack
	U32 remainingTime;
	U32 deadline_ms;
} TCB;

#define SMALLEST_TASK_ALLOC 128 // Assuming sizeof(TCB) = 32
#define MAX_TASKS 16 //maximum number of tasks in the system including null task

typedef struct kernel_variables {
//	unsigned int numAvaliableTasks; // Num of running and ready TCBs
	unsigned int totalStackUsed;
	TCB tcbList[MAX_TASKS]; //TODO: Try to implement this as a linked list. Store our TCB's in the heap along with the thread stack
	int currentRunningTID;
	U8 kernelInitRan;
	U8 kernelStarted;
	U8 buddyHeapInit;
	U32 endOfHeap; // never write to an address beyond this
	U32 startOfHeap; // When allocating memory, never allocate to an address less than this. Remember heap grows from small to large address
} Kernel_Variables;

typedef struct Block {
	U8 type; // FREE/USED
	uint16_t size; // Block size including sizeof(Block)
	task_t TIDofOwner;
	struct Block* next; // Points to the start of the next block
	struct Block* prev;
} Block;

typedef struct BuddyHeap {
	U32 currentBlockListSize;
//	Block* blockList[NUMBER_OF_NODES];
	Block* freeList[HEIGHT_OF_TREE];  // Eg, 2^5 = 32, 2^6 = 64, ....
//	U8 bitArray[NUMBER_OF_NODES];
} BuddyHeap;

#define ACTUAL_BLOCK(ADDR) ((U32) ADDR - sizeof(Block))

extern Kernel_Variables kernelVariables;
extern BuddyHeap buddyHeap;
extern uint32_t _img_end;
extern uint32_t _estack;
extern uint32_t _Min_Stack_Size;
extern U32 timerCounter;
/**
 * @brief: Returns the MSP address
 */
uint32_t* Get_MSP_INIT_VAL();

/**
 * @brief: Checks how much of the whole stack is currently used
*/
unsigned int Get_Total_Memory_Used();

/**
 * @brief: Finds the next TCB to run and returns the TID
*/
int Scheduler(void);

/**
 * @brief: Finds the next TCB to run and returns the TID
*/
int EDFScheduler(void);

#endif /* INC_COMMON_H_ */
