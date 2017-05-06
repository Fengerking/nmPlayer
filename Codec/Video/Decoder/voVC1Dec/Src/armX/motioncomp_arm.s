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
     
    EXPORT  g_MotionCompZeroMotion_WMV_ARMV4 



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
    WMV_LEAF_ENTRY g_MotionCompZeroMotion_WMV_ARMV4
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
