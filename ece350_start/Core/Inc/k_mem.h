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

extern uint32_t currentMainStackSize;
extern TCB tcbs[MAX_TASKS];

/*
 * @brief  initializes all global kernel-level data structures and other variables as required by our kernel
 * @param None
 * @retval None
 */
void osKernelInit(void);

/*
 * @brief: Write to main stack.
 * @param uint32_t for size of data, uint32_t* for data.
 * @retval 1 on success, -1 else.
 */
unsigned int osInitTCBArray();

#endif /* INC_K_MEM_H_ */
