@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/
@@asxscce -g -mcpu 2 $(InputPath) -o $(IntDir)\$(InputName).obj

	.section .text
	.global  unsigned_divid_arm
	.type    unsigned_divid_arm, function
unsigned_divid_arm:
.ALIGN 4
	stmdb		sp!, {lr}		@ save regs used
	mov			r2, r1
	mov			r1, r0
	mov			r0, r2
	bl		__rt_udiv
    ldmia		sp!, {pc}		@ restore and return
	
	.section .text
	.global  signed_divid_arm
	.type    signed_divid_arm, function
signed_divid_arm:
.ALIGN 4
	stmdb		sp!, {lr}		@ save regs used
	mov			r2, r1
	mov			r1, r0
	mov			r0, r2
	bl			__rt_sdiv
    ldmia		sp!, {pc}		@ restore and return    

