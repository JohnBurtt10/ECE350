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
#include "svc_handler_main.h"

//extern TCB* readyTCBs[MAX_TASKS];

/*
 * @brief  initializes all global kernel-level data structures and other variables as required by our kernel
 * @param None
 * @retval None
 */
void osKernelInit(void);

/*
 * @brief: Init TCB array that stores all stored TCBs.
 * @param None.
 * @retval None.
 */
void osInitTCBArray();

#endif /* INC_K_MEM_H_ */
