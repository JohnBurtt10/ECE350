.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global SVC_Handler //indicates to the linker that this function exists
.thumb_func //ensures that the address of the interrupt function is properly aligned
SVC_Handler:
	.global SVC_Handler_Main
	TST lr, #4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP
	B SVC_Handler_Main
