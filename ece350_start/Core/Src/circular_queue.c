/*
 * queue.c
 *
 *  Created on: May 23, 2024
 *      Author: ethanromero
 */

#include "circular_queue.h"
#include "common.h"
#include <stddef.h>

Circular_Queue circularQueue;

void Queue_Init(TCB tcbs[]){
	circularQueue.front = -1;
	circularQueue.rear = -1;
	circularQueue.size = 0;

	for (int i = 0; i < MAX_TASKS + 1; i++) {
		Queue_Push(&tcbs[i]);
	}
}

int Queue_Check_Full() {
	if (circularQueue.size == MAX_TASKS) {
		return 1; // TRUE
	}

	return 0; // FALSE
}

int Queue_Check_Empty() {
	if (circularQueue.size == 0) {
		return 1; // TRUE
	}

	return 0; // FALSE
}

int Queue_Size() {
	return circularQueue.size;
}

TCB* Queue_Pop() {
	if (Queue_Check_Empty()) {
		return NULL;
	}

	circularQueue.front = (circularQueue.front + 1) % MAX_TASKS;
	circularQueue.size = circularQueue.size - 1;

	return circularQueue.queue[circularQueue.front];
}

void Queue_Push(TCB *tcb) {
	if (Queue_Check_Full()) {
		return;
	}

	circularQueue.rear = (circularQueue.rear + 1) % MAX_TASKS;
	circularQueue.queue[circularQueue.rear] = tcb;
	circularQueue.size = circularQueue.size + 1;
}


