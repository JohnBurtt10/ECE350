/*
 * common.c
 *
 *  Created on: May 19, 2024
 *      Author: Ethan Romero
 */

#include "common.h"

// Definitions for various system calls
#define TEST_ERROR 0

uint32_t* Get_MSP_INIT_VAL(){
	return *(uint32_t**)0x0;
}

void Trigger_System_Call(unsigned int systemCall) {
	if (systemCall == TEST_ERROR){
		__asm("SVC 0");
	}
}
