.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global PendSV_Handler
.thumb_func
PendSV_Handler:
	.global contextSwitch
	.global save_new_psp
	TST lr, #4
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP

	// Checks if on MSP (task has not started running) or PSP
	TST lr, #4
	BEQ switch

	// Store current running task state.
	STMDB r0!, {r4-r11}
	MSR PSP, r0
switch:
	PUSH {LR}
	BL contextSwitch
	POP {LR}
	
	// Run next task
	MRS r0, PSP
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	BL save_new_psp // Set current_sp back to stack_high as we should have popped all registers from stack after exiting interrupt

	MOV LR, #0xFFFFFFFD // Put magic number into LR register that indicates exit from interrupt and to use the PSP as the SP
	BX LR // A branch to this "magic number" will restore 8 hardware-saved registers and jump to PC. (PSR, PC, LR, R12, R3, R2, R1, R0)
