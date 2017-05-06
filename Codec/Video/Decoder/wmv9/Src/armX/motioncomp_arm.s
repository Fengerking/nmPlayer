;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;  THIS IS ASSEMBLY VERSION OF ROUTINES IN MOTIONCOMP_CE_COMM.CPP WHEN 
;  MOTIONCOMP_COMPLETE AND PLATFORM_32REGS ARE NOT DEFINED
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF   
 
    AREA |.text|, CODE, READONLY

    IF WMV_OPT_MOTIONCOMP_ARM = 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
     
    EXPORT  g_MotionCompZeroMotion_WMV 

    IMPORT  MotionCompMixed010Complete
    IMPORT  MotionCompMixedHQHIComplete


    IF ARCH_V3 = 1
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        MACRO             
        LOADONE16bitsLo $srcRN, $offset, $dstRN
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ldr     $dstRN, [$srcRN, $offset]
        mov     $dstRN, $dstRN, LSL #16
        mov     $dstRN, $dstRN, ASR #16
        MEND
    ENDIF ; //ARCH_V3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

;// err_even = pErrorBuf[0];

    ldr $err_even, [$pErrorBuf], #0x10

;// err_odd  = pErrorBuf[0 + 32];

    ldr $err_odd, [$pErrorBuf, #0x70]


;//pErrorBuf += 4;



;//u0 = u0 + err_even-((err_even & 0x8000) << 1);

    AND $scratch, $err_even, #0x8000
    SUB $scratch, $err_even, $scratch, LSL #1
    ADD $u0, $u0, $scratch

;//err_overflow  |= u0;
    ORR $err_overflow, $err_overflow, $u0

;//u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);

    AND $scratch,$err_odd,#0x8000
    SUB $scratch,$err_odd,$scratch, LSL #1
    ADD $u1, $u1, $scratch

;//err_overflow  |= u1;
    ORR $err_overflow, $err_overflow, $u1

    MEND

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    MACRO
    AndAddErrorPartial $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
;//u0 = u0 + err_even-((err_even & 0x8000) << 1);

    AND $scratch, $err_even, #0x8000
    SUB $scratch, $err_even, $scratch, LSL #1
    ADD $u0, $u0, $scratch

;//err_overflow  |= u0;
    ORR $err_overflow, $err_overflow, $u0

;//u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);

    AND $scratch,$err_odd,#0x8000
    SUB $scratch,$err_odd,$scratch, LSL #1
    ADD $u1, $u1, $scratch

;//err_overflow  |= u1;
    ORR $err_overflow, $err_overflow, $u1

    MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch
;   //u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
    
    ADD $scratch, $t1, $t2  
    ADD $scratch, $scratch, $scratch, LSL #3
    SUB $u0, $scratch, $t0
    SUB $u0, $u0, $t3
    ADD $u0, $u0, $const

    MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    CubicFilterShort $u0, $t0, $t1, $t2, $const, $scratch
;   //u0 = ((t1 + t2)*9-(t0 ) + 0x00080008);
    
    ADD $scratch, $t1, $t2  
    ADD $scratch, $scratch, $scratch, LSL #3
    SUB $u0, $scratch, $t0
    ADD $u0, $u0, $const

    MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  


;Void_WMV MotionCompZeroMotion_WMV (
;    U8_WMV* ppxliCurrQYMB, 
;    U8_WMV* ppxliCurrQUMB, 
;    U8_WMV* ppxliCurrQVMB,
;    const U8_WMV* ppxliRefYMB, 
;    const U8_WMV* ppxliRefUMB, 
;    const U8_WMV* ppxliRefVMB,
;    I32_WMV iWidthY,
;    I32_WMV iWidthUV
;)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_MotionCompZeroMotion_WMV
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;      
    STMFD   sp!, {r4 - r11, r14} 
    FRAME_PROFILE_COUNT
    ADD     r12, sp, #36
    LDMIA   r12, {r4 - r7}
    mov     r12, #8                
	PLD [r3, r6]
	PLD [r4, r7]
	PLD [r5, r7]  
loopCopy
	IF _XSC_=1
	;	PLD [r3, #32]
		PLD [r3, r6, lsl #1]
	ENDIF 
    LDMIA   r3, {r8 - r11}
    SUBS    r12, r12, #1   
    ADD     r3, r3, r6
	IF _XSC_=1
	;	PLD [r3, #32]
		PLD [r3, r6, lsl #1]
	ENDIF            
    STMIA   r0, {r8 - r11}          
    ADD     r0, r0, r6             
    LDMIA   r3, {r8 - r11}
    ADD     r3, r3, r6  
    STMIA   r0, {r8 - r11}               
	IF _XSC_=1
	;	PLD [r4, #32]
		PLD [r4, r7, lsl #1]
	ENDIF 	         
    ADD     r0, r0, r6
    LDMIA   r4, {r8 - r9}    
    ADD     r4, r4, r7    
    STMIA   r1, {r8 - r9}            
	IF _XSC_=1
	;	PLD [r5, #32]
		PLD [r5, r7, lsl #1]
	ENDIF 	          
    ADD     r1, r1, r7   
    LDMIA   r5, {r8 - r9}
    ADD     r5, r5, r7            
    STMIA   r2, {r8 - r9}      
    ADD     r2, r2, r7             
    BNE     loopCopy
    LDMFD   sp!, {r4 - r11, PC}
 
    WMV_ENTRY_END

    ENDIF ;WMV_OPT_MOTIONCOMP_ARM

    END 
