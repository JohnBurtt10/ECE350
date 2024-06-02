/*
 * svc_handler_main.h
 *
 *  Created on: May 29, 2024
 *      Author: ethanromero
 */

#ifndef INC_SVC_HANDLER_MAIN_H_
#define INC_SVC_HANDLER_MAIN_H_

#include "common.h"

int createTask(TCB* task);

/*
 * brief: Initializes thread stack with given function call and arbitrary values.
 */
void Init_Thread_Stack(uint32_t* stack_pointer, void (*callback)(void* args), int TID);

void contextSwitch(void);

void save_new_psp(void);

int SVC_Handler_Main( unsigned int *svc_args );

#endif /* INC_SVC_HANDLER_MAIN_H_ */