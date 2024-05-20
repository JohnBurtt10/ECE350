/*
 * common.c
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */

#include "common.h"
#include <stdio.h>

uint32_t* current_MSP = NULL;

uint32_t* Get_MSP_INIT_VAL(){
	return *(uint32_t**)0x0;
}

void Trigger_System_Call(unsigned int systemCall) {;
	if (systemCall == TEST_ERROR){
		__asm("SVC 0");
	} else if (systemCall == CREATE_THREAD) {
		__asm("SVC 1");
	}
}
