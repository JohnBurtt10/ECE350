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

int k_mem_dealloc(void* ptr);

/**
 * @brief: Creates a block given heap address, size of requested malloc, current type and owner.
 * 			NOTE: If requested size will be converted to a multiple of 32 bytes!
 */
Block* Create_Block(U32 size, void* heapAddress, U32 type, int tidOwner);

Block* Split_Block(Block* parentBlock);

__attribute__((always_inline))
inline U32 Calculate_Nearest_Order(U32 num);

#endif /* INC_K_MEM_H_ */
