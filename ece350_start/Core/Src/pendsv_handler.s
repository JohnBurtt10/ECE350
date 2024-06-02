.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global PendSV_Handler
.thumb_func
PendSV_Handler:
	.global contextSwitch
	TST lr, #4
	PUSH {LR}
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP

	POP {LR}
	TST lr, #4
	BEQ switch

	// Store current running task state.
	MRS r0, PSP
	STMDB r0!, {r4-r11}
	MRS r0, PSP

switch:
	BL contextSwitch

	// Run next task
	MRS r0, PSP
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	MOV LR, #0xFFFFFFFD // put magic number into LR register that indicates exit from interrupt and to use the PSP as the SP
	BX LR // A branch to this "magic number" will restore 8 hardware-saved registers and jump to PC. (PSR, PC, LR, R12, R3, R2, R1, R0)
