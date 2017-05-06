@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void comp_corr ( 
@		Word16 scal_sig[],  /* i   : scaled signal.                          */
@		Word16 lag_max,     /* i   : maximum lag                             */
@		Word16 lag_min,     /* i   : minimum lag                             */
@		Word32 corr[])      /* o   : correlation of selected lag             */

@*************************************************
@ ARM register 
@*************************************************
@ short *scal_sig               RN           0
@ lag_max                       RN           1
@ lag_min                       RN           2
@ *corr                         RN           3
        #include "voAMRNBEncID.h"
        .text   .align   4
        .globl   _comp_corr_asm

_comp_corr_asm:	

        stmfd         r13!, {r4 - r12, r14}       
        @for(i = lag_max@ i >= lag_min@ i--)

        sub           r12, r3, r1, lsl #2            @ corr[-i] address
             
LOOP1:
        mov           r4, r0                         @ p = scal_sig       
        sub           r7, r0, r1, lsl #1             @ p1 = &scal_sig[-i]
        mov           r11, #80
        mov           r14, #0                        @ t0 = 0

LOOP2:
        ldrsh         r8,  [r4], #2
        ldrsh         r9,  [r7], #2
        ldrsh         r6,  [r4], #2
        ldrsh         r10, [r7], #2
	pkhbt         r6, r8, r6, lsl #16
        ldrsh         r8,  [r4], #2
	pkhbt         r10, r9, r10, lsl #16
        ldrsh         r9,  [r7], #2
	smlad         r14, r6, r10, r14

        ldrsh         r6,  [r4], #2
        ldrsh         r10, [r7], #2
	pkhbt         r6, r8, r6, lsl #16
        ldrsh         r8,  [r4], #2
	pkhbt         r10, r9, r10, lsl #16
        ldrsh         r9,  [r7], #2
	smlad         r14, r6, r10, r14

        ldrsh         r6,  [r4], #2
        ldrsh         r10, [r7], #2
	pkhbt         r6, r8, r6, lsl #16
        ldrsh         r8,  [r4], #2
	pkhbt         r10, r9, r10, lsl #16
        ldrsh         r9,  [r7], #2
	smlad         r14, r6, r10, r14

        ldrsh         r6,  [r4], #2
        ldrsh         r10, [r7], #2
	pkhbt         r6, r8, r6, lsl #16
        ldrsh         r8,  [r4], #2
	pkhbt         r10, r9, r10, lsl #16
        ldrsh         r9,  [r7], #2
	smlad         r14, r6, r10, r14

        ldrsh         r6,  [r4], #2
        ldrsh         r10, [r7], #2
	pkhbt         r8, r8, r6, lsl #16
        subs          r11, r11, #10 
	pkhbt         r9, r9, r10, lsl #16
	smlad         r14, r8, r9, r14
      
        bne           LOOP2
        
        mov           r14, r14, lsl #1
        sub           r1, r1, #1
        str           r14, [r12], #4
        cmp           r1,  r2
        bge           LOOP1

comp_corr_asm_end:
                        
        ldmfd         r13!, {r4 - r12, r15} 
        @.END
