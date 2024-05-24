/*
 * queue.h
 *
 *  Created on: May 23, 2024
 *      Author: ethanromero
 */

#ifndef INC_CIRCULAR_QUEUE_H_
#define INC_CIRCULAR_QUEUE_H_

#include "common.h"

extern unsigned int front;
extern unsigned int rear;

typedef struct circularQueue {
	TCB* queue[MAX_TASKS];
	unsigned int front;
	unsigned int rear;
	unsigned int size;
} Circular_Queue;

extern Circular_Queue circularQueue;


void Queue_Init(TCB tcbs[]);
int Queue_Check_Full();
int Queue_Check_Empty();
int Queue_Size();
TCB* Queue_Pop();
void Queue_Push(TCB* tcb);


#endif /* INC_CIRCULAR_QUEUE_H_ */
