@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************

@*void Convolve (
@*    Word16 x[],        /* (i)     : input vector                           */
@*    Word16 h[],        /* (i)     : impulse response                       */
@*    Word16 y[],        /* (o)     : output vector                          */
@*    Word16 L           /* (i)     : vector size                            */
@*)
        #include "voAMRWBEncID.h"
	.text   .align 4
        .globl   _voAMRWBEncConvolve_asm 

_voAMRWBEncConvolve_asm:

        stmfd          r13!, {r4 - r12, r14}  
        mov            r3,  #0                           @ n
	mov            r11, #0x8000
        
LOOP: 
        add            r4, r1, r3, lsl #1                @ tmpH address
        add            r5, r3, #1                        @ i = n + 1
        mov            r6, r0                            @ tmpX = x
        ldrsh          r9,  [r6], #2                     @ *tmpX++
        ldrsh          r10, [r4], #-2                    @ *tmpH--
        sub            r5, r5, #1
        mul            r8,  r9, r10 

LOOP1:                    
        cmp            r5, #0
        ble            L1

	ldrsh          r9,  [r6], #2                     @ *tmpX++
	ldrsh          r12, [r6], #2                     @ *tmpX++
	ldrsh          r10, [r4], #-2                    @ *tmpH--
	ldrsh          r14, [r4], #-2                    @ *tmpH--
	pkhbt          r9, r9, r12, lsl #16
	ldrsh          r12, [r6], #2
	pkhbt          r10, r10, r14, lsl #16
	ldrsh          r14, [r6], #2
	smlad          r8, r9, r10, r8
	ldrsh          r9,  [r4], #-2
	ldrsh          r10, [r4], #-2
	pkhbt          r12, r12, r14, lsl #16
	pkhbt          r9, r9, r10, lsl #16
	subs           r5, r5, #4
	smlad          r8, r12, r9, r8
        b              LOOP1    

L1:                  
        add            r5, r11, r8, lsl #1
        mov            r5, r5, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r5, [r2], #2                      @y[n]


        add            r4, r1, r3, lsl #1                @tmpH address
        add            r5, r3, #1
        mov            r6, r0
        ldrsh          r9,  [r6], #2                     @ *tmpX++
        ldrsh          r10, [r4], #-2                     
        ldrsh          r12, [r6], #2
        ldrsh          r14, [r4], #-2

        mul            r8, r9, r10
        sub            r5, r5, #2
        mla            r8, r12, r14, r8
        
LOOP2:
        cmp            r5, #0
        ble            L2
	ldrsh          r9,  [r6], #2                     @ *tmpX++
	ldrsh          r12, [r6], #2                     @ *tmpX++
	ldrsh          r10, [r4], #-2                    @ *tmpH--
	ldrsh          r14, [r4], #-2                    @ *tmpH--
	pkhbt          r9, r9, r12, lsl #16
	ldrsh          r12, [r6], #2
	pkhbt          r10, r10, r14, lsl #16
	ldrsh          r14, [r6], #2
	smlad          r8, r9, r10, r8
	ldrsh          r9,  [r4], #-2
	ldrsh          r10, [r4], #-2
	pkhbt          r12, r12, r14, lsl #16
	pkhbt          r9, r9, r10, lsl #16
	subs           r5, r5, #4
	smlad          r8, r12, r9, r8
        b              LOOP2

L2:
        add            r8, r11, r8, lsl #1
        mov            r8, r8, lsr #16                   @extract_h(s)
        add            r3, r3, #1  
        strh           r8, [r2], #2                      @y[n]


        add            r4, r1, r3, lsl #1
        add            r5, r3, #1
        mov            r6, r0
        ldrsh          r9,  [r6], #2
        ldrsh          r10, [r4], #-2
        ldrsh          r12, [r6], #2
        ldrsh          r14, [r4], #-2
        mul            r8, r9, r10
        ldrsh          r9,  [r6], #2
        ldrsh          r10, [r4], #-2
        mla            r8, r12, r14, r8 
        sub            r5, r5, #3
        mla            r8, r9, r10, r8

LOOP3:
        cmp            r5, #0
        ble            L3
	ldrsh          r9,  [r6], #2                     @ *tmpX++
	ldrsh          r12, [r6], #2                     @ *tmpX++
	ldrsh          r10, [r4], #-2                    @ *tmpH--
	ldrsh          r14, [r4], #-2                    @ *tmpH--
	pkhbt          r9, r9, r12, lsl #16
	ldrsh          r12, [r6], #2
	pkhbt          r10, r10, r14, lsl #16
	ldrsh          r14, [r6], #2
	smlad          r8, r9, r10, r8
	ldrsh          r9,  [r4], #-2
	ldrsh          r10, [r4], #-2
	pkhbt          r12, r12, r14, lsl #16
	pkhbt          r9, r9, r10, lsl #16
	subs           r5, r5, #4
	smlad          r8, r12, r9, r8
        b              LOOP3   

L3:
        add            r8, r11, r8, lsl #1
        mov            r8, r8, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r8, [r2], #2                      @y[n]

        add            r5, r3, #1                        @ i = n + 1
        add            r4, r1, r3, lsl #1                @ tmpH address
        mov            r6, r0
        mov            r8, #0

LOOP4:                    
        cmp            r5, #0
        ble            L4
	ldrsh          r9,  [r6], #2                     @ *tmpX++
	ldrsh          r12, [r6], #2                     @ *tmpX++
	ldrsh          r10, [r4], #-2                    @ *tmpH--
	ldrsh          r14, [r4], #-2                    @ *tmpH--
	pkhbt          r9, r9, r12, lsl #16
	ldrsh          r12, [r6], #2
	pkhbt          r10, r10, r14, lsl #16
	ldrsh          r14, [r6], #2
	smlad          r8, r9, r10, r8
	ldrsh          r9,  [r4], #-2
	ldrsh          r10, [r4], #-2
	pkhbt          r12, r12, r14, lsl #16
	pkhbt          r9, r9, r10, lsl #16
	subs           r5, r5, #4
	smlad          r8, r12, r9, r8       
        b              LOOP4    

L4:                  
        add            r5, r11, r8, lsl #1
        mov            r5, r5, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r5, [r2], #2                      @y[n]
        
        cmp            r3, #64
        blt            LOOP
                
Convolve_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}
    
        @ENDFUNC
        @.END


