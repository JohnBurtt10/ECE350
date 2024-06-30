/*
 * k_mem.h
 *
 *  Created on: Jan 5, 2024
 *      Author: nexususer
 *
 *      NOTE: any C functions you write must go into a corresponding c file that you create in the Core->Src folder
 */

#ifndef INC_K_MEM_H_
#define INC_K_MEM_H_

#include "k_task.h"
#include <stddef.h>
#include "svc_handler_main.h"

#define CALCULATE_FREE_LIST_IDX(ORDER) (MAX_ORDER + MIN_BLOCK_ORDER - ORDER)
#define CALCULATE_ORDER_FROM_FREELIST_IDX(X) (MIN_BLOCK_ORDER + MAX_ORDER - X);

/**
 * @brief  Initializes all global kernel-level data structures and other variables as required by the kernel
 * @retval None
 */
void osKernelInit(void);

/**
 * @brief: Init TCB array that stores all stored TCBs.
 * @retval None.
 */
void osInitTCBArray();

/**
 * @brief: Init Buddy Heap Object
 * @retval None.
 */
void osInitBuddyHeap();

int k_mem_init(void);

int k_mem_count_(size_t size);

void* k_mem_alloc(size_t size);

U32 Calculate_Order(U32 num);

U32 Calculate_Nearest_Order(U32 num);

U32 Calculate_Free_List_Idx(U32 order);

/**
 * @brief: Creates a block given heap address, size of requested malloc, current type and owner.
 * 			NOTE: If requested size will be converted to a multiple of 32 bytes!
 */
Block* Create_Block(U32 size, void* heapAddress, U32 type, int tidOwner);

Block* Split_Block(Block* parentBlock, U32 currFreeListIdx);

void Free_List_Push(Block* newBlock, U32 freeListIdx);

Block* Free_List_Pop(U32 freeListIdx);

#endif /* INC_K_MEM_H_ */
