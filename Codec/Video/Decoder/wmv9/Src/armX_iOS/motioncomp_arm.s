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


    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h"
 
    @AREA MOTIONCOMP, CODE, READONLY
		 .text
		 .align 4

    .if WMV_OPT_MOTIONCOMP_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 
     
    .globl  _g_MotionCompZeroMotion_WMV 

    .globl  _MotionCompMixed010Complete
    .globl  _MotionCompMixedHQHIComplete


    .if ARCH_V3 == 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro LOADONE16bitsLo @srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        ldr     $2, [$0, $1]
        mov     $2, $2, lsl #16
        mov     $2, $2, asr #16
        .endmacro
    .endif @ //ARCH_V3

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro AndAddError @pErrorBuf, err_even, err_odd, u0, u1, err_overflow, scratch

@// err_even == pErrorBuf[0]@

    ldr $1, [$0], #0x10

@// err_odd  == pErrorBuf[0 + 32]@

    ldr $2, [$0, #0x70]


@//pErrorBuf += 4@


@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    and $6, $1, #0x8000
    sub $6, $1, $6, lsl #1
    add $3, $3, $6

@//err_overflow  |= u0@
    orr $5, $5, $3

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    and $6,$2,#0x8000
    sub $6,$2,$6, lsl #1
    add $4, $4, $6

@//err_overflow  |= u1@
    orr $5, $5, $4

    .endmacro

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .macro AndAddErrorPartial @err_even, err_odd, u0, u1, err_overflow, scratch
@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    and $5, $0, #0x8000
    sub $5, $0, $5, lsl #1
    add $2, $2, $5

@//err_overflow  |= u0@
    orr $4, $4, $2

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    and $5,$1,#0x8000
    sub $5,$1,$5, lsl #1
    add $3, $3, $5

@//err_overflow  |= u1@
    orr $4, $4, $3

    .endmacro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilter @u0, t0, t1, t2, t3, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@
    
    add $6, $2, $3  
    add $6, $6, $6, lsl #3
    sub $0, $6, $1
    sub $0, $0, $4
    add $0, $0, $5

    .endmacro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilterShort @u0, t0, t1, t2, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 ) + 0x00080008)@
    
    add $5, $2, $3  
    add $5, $5, $5, lsl #3
    sub $0, $5, $1
    add $0, $0, $4

    .endmacro
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
