/*
 * thread.h
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */

#ifndef INC_THREAD_H_
#define INC_THREAD_H_

#include <stdint.h>

extern unsigned int numThreads;

uint32_t* Create_Thread_Stack(); // Each thread should have its own stack.

#endif /* INC_THREAD_H_ */
