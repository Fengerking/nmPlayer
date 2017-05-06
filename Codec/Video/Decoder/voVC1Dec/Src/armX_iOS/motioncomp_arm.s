@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@
@  THIS IS ASSEMBLY VERSION OF ROUTINES IN MOTIONCOMP_CE_COMM.CPP WHEN 
@  MOTIONCOMP_COMPLETE and PLATFORM_32REGS ARE NOT DEFINED
@
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		#include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h"
 
    @AREA MOTIONCOMP, CODE, READONLY
		 .text
		 .align 4

    .if WMV_OPT_MOTIONCOMP_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .globl  _g_MotionCompZeroMotion_WMV_ARMV4 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
@Void_WMV MotionCompZeroMotion_WMV (
@    U8_WMV* ppxliCurrQYMB, 
@    U8_WMV* ppxliCurrQUMB, 
@    U8_WMV* ppxliCurrQVMB,
@    const U8_WMV* ppxliRefYMB, 
@    const U8_WMV* ppxliRefUMB, 
@    const U8_WMV* ppxliRefVMB,
@    I32_WMV iWidthY,
@    I32_WMV iWidthUV
@)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY g_MotionCompZeroMotion_WMV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      
    stmfd   sp!, {r4 - r11, r14} 
    FRAME_PROFILE_COUNT
    add     r12, sp, #36
    ldmia   r12, {r4 - r7}
    mov     r12, #8                
    .if PLD_ENABLE == 1
	pld [r3, r6]
	pld [r4, r7]
	pld [r5, r7]  
	.endif
loopCopy:
	.if PLD_ENABLE==1
	@	pld [r3, #32]
		pld [r3, r6, lsl #1]
	.endif 
    ldmia   r3, {r8 - r11}
    subs    r12, r12, #1   
    add     r3, r3, r6
	.if PLD_ENABLE==1
	@	pld [r3, #32]
		pld [r3, r6, lsl #1]
	.endif            
    stmia   r0, {r8 - r11}          
    add     r0, r0, r6             
    ldmia   r3, {r8 - r11}
    add     r3, r3, r6  
    stmia   r0, {r8 - r11}               
	.if PLD_ENABLE==1
	@	pld [r4, #32]
		pld [r4, r7, lsl #1]
	.endif 	         
    add     r0, r0, r6
    ldmia   r4, {r8 - r9}    
    add     r4, r4, r7    
    stmia   r1, {r8 - r9}            
	.if PLD_ENABLE==1
	@	pld [r5, #32]
		pld [r5, r7, lsl #1]
	.endif 	          
    add     r1, r1, r7   
    ldmia   r5, {r8 - r9}
    add     r5, r5, r7            
    stmia   r2, {r8 - r9}      
    add     r2, r2, r7             
    bne     loopCopy
    ldmfd   sp!, {r4 - r11, PC}
 
    WMV_ENTRY_END

    .endif @WMV_OPT_MOTIONCOMP_ARM

    @@.end 
