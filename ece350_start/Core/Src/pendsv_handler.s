.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global PendSV_Handler
.thumb_func
PendSV_Handler:
	MRS r0, PSP
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	MOV LR, #0xFFFFFFFD // put magic number into LR register that indicates exit from interrupt and to use the PSP as the SP
	BX LR // A branch to this "magic number" will restore 8 hardware-saved registers and jump to PC. (PSR, PC, LR, R12, R3, R2, R1, R0)
