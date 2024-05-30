.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb


// This shit is fucking retarded. DON'T ask me how it works.
.global SVC_Handler //indicates to the linker that this function exists
.thumb_func
SVC_Handler:
	.global SVC_Handler_Main
	TST lr, #4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP
	PUSH {R4-R11} // Treating it as a context switch
	PUSH {R0, R1, R2, R3} // Need these 4 registers for C function call. Nerd shit tbh
	PUSH {LR} // Want to maintain magic number in LR
	BL SVC_Handler_Main
	MOV R4, R0
	POP {LR}
	POP {R0, R1, R2, R3}
	MOV R0, R4
	POP {R4-R11}

	STR R0, [SP]
	BX LR
