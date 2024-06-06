.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global SVC_Handler //indicates to the linker that this function exists
.thumb_func
SVC_Handler:
	.global SVC_Handler_Main
	TST lr, #4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP

	PUSH {R4-R11} // Treating it as a context switch
	MOV r4, r0
	PUSH {LR} // Want to maintain magic number in LR
	BL SVC_Handler_Main
	POP {LR}
	STR R0, [R4] // SVC call will pop 8 registers R0 - xPSR on exit. Store value of R0 into SP
	POP {R4-R11}
	BX LR
