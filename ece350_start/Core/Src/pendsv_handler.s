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
	MOV LR, #0xFFFFFFFD
	BX LR
