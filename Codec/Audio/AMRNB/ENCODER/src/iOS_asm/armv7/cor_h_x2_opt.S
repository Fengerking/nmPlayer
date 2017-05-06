@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void cor_h_x2 (
@    Word16 h[],    /* (i): impulse response of weighted synthesis filter */
@    Word16 x[],    /* (i): target                                        */
@    Word16 dn[],   /* (o): correlation between target and h[]            */
@    Word16 sf,     /* (i): scaling factor: 2 for 12.2, 1 for others      */
@    Word16 nb_track,/* (i): the number of ACB tracks                     */
@    Word16 step    /* (i): step size from one pulse position to the next
@                           in one track                                  */
@)
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _voAMRNBEnccor_h_x2_asm

@******************************
@ constant
@******************************
.set    Y32          , 0
.set    TEMP1        , Y32 + 40 * 4
.set    H_PTR        , TEMP1 + 4
.set    X_PTR        , H_PTR + 4
.set    dN_PTR       , X_PTR + 4
.set    SF           , dN_PTR + 4
.set    NB_TRACK     , SF + 10*4 + 4
.set    STEPM        , NB_TRACK + 4
@******************************
@ ARM register 
@******************************
@ h[]                RN          0
@ x[]                RN          1
@ dn[]               RN          2
@ sf                 RN          3
@ nb_track           RN          4
@ step               RN          5
         
@******************************
@ Neon register 
@******************************

_voAMRNBEnccor_h_x2_asm:

        stmfd      r13!, {r0 - r12, r14}  
        sub        r13, r13, #164               @ y32 temp spqce in stack
        ldr        r10, [r13, #STEPM]           @ r10 = step
        ldr        r11, [r13, #NB_TRACK]        @ r11 = nb_track
        mov        r9, #5                       @ r9 --- tot = 5
        mov        r8, #0                       @ r8 --- k = 0

LOOPK:
        mov        r7, #0                       @ r7 = max
        mov        r4, r8                       @ r4 --- k -- i
        str        r9, [r13, #TEMP1]

LOOPI:
        ldr        r0, [r13, #H_PTR]            @ get h[] address
        ldr        r1, [r13, #X_PTR]            @ get x[] address
        mov        r9, #0                       @ s = 0
        vmov.s32   q10, #0                      @ sum = 0

        rsb        r12, r4, #40                 @ r12 = loop times  L_COdE - i
        add        r1, r1, r4, lsl #1           @ r1 --- x[i]

        ands       r2, r12, #3
        beq        LOOPJ_4x

PRE_LOOPJ: 
        ldrsh      r5, [r1], #2                 @ x[j]
        ldrsh      r6, [r0], #2                 @ h[j-i]
        subs       r2, r2, #1
        mla        r9, r5, r6, r9
        bne        PRE_LOOPJ

LOOPJ_4x:

        movs       r12, r12, LSR #2
        beq        SKIPLOOPJ

LOOPJ:
         ldrsh      r5, [r1], #2
         ldrsh      r6, [r0], #2
         ldrsh      r3, [r1], #2
         ldrsh      r14,[r0], #2
         mla        r9, r5, r6, r9
         ldrsh      r5, [r1], #2
         ldrsh      r6, [r0], #2
         mla        r9, r3, r14, r9
         ldrsh      r3, [r1], #2
         ldrsh      r14,[r0], #2
         mla        r9, r5, r6, r9
         subs       r12, r12, #1
         mla        r9, r3, r14, r9
         bne        LOOPJ

SKIPLOOPJ:
        mov        r14, r9, lsl #1                  @ s = s << 1 
        str        r14, [r13, r4, lsl #2]           @ y32[i] = s

        cmp        r9, #0
        rsblt      r9, r9, #0

        cmp        r9, r7                          @ r9 --- s   
        movgt      r7, r9
    
        add        r4, r4, r10                     @ r10 --- step 
        cmp        r4, #40
        blt        LOOPI

        ldr        r9, [r13, #TEMP1]               @get tot = 5
        mov        r7, r7, asr #1                  @r7----max   L_shr(max, 1)
        subs       r11, r11, #1
        qadd       r9, r9, r7
        
        add        r8, r8, #1
        bne        LOOPK

        ldr        r1, [r13,#SF]
        clz        r0, r9                  @r9>0
        sub        r0, r0,#1
        sub        r0, r0, r1               @r0 ---> j = sub(norm(tot), sf)@
        
        mov        r5, #0x8000
        vmov.s32   q13, #0x8000
        ldr        r2, [r13,#dN_PTR]       @r2->dn[i]
        mov        r1, r13                 @r1->y32[i]
        mov        r3, #10
        
        cmp        r0,#0
        vdup.s32   q12, r0
        bgt        dN_L                   @j>0
        beq        dN_H                   @j=0,hold
        rsb        r0,r0,#0               @j<0
        vdup.s32   q12, r0

dN_R:
        ldr        r12,[r1],#4          @r12=y32[i]
        ldr        r11,[r1],#4
        ldr        r10,[r1],#4
        ldr        r9,[r1],#4

        mov        r12,r12,asr r0       @L_shl (y32[i], j) j<0
        mov        r11,r11,asr r0
        mov        r10,r10,asr r0
        mov        r9,r9,asr r0

        qadd       r12,r12,r5
        qadd       r11,r11,r5
        qadd       r10,r10,r5
        qadd     r9,r9,r5

        mov        r12, r12, asr #16
        mov        r11, r11, asr #16
        mov        r10, r10, asr #16
        mov        r9, r9, asr #16

        subs       r3,r3,#1             @loop times
      
        strh       r12,[r2], #2
        strh       r11,[r2], #2
        strh       r10,[r2], #2
        strh       r9, [r2], #2         @dn[i] = round (L_shl (y32[i], j))    

        bne        dN_R
        b          cor_h_x2_asm_end
        
dN_L:
        vld1.s32     {q6, q7}, [r1]!
        vqshl.s32    q10, q6, q12
        vqadd.s32    q6, q10, q13
        vqshl.s32    q10, q7, q12
        vqadd.s32    q7, q10, q13
        vshrn.s32    d8, q6, #16
        vshrn.s32    d9, q7, #16  
 
        subs         r3,r3,#2             @loop times
        vst1.S16     {d8, d9}, [r2]!      
        bne          dN_L
        b            cor_h_x2_asm_end

dN_H:
        vld1.s32     {q6, q7}, [r1]!
        vqadd.s32    q6, q6, q13
        vqadd.s32    q7, q7, q13
        vshrn.s32    d8, q6, #16
        vshrn.s32    d9, q7, #16  
 
        subs         r3,r3,#2             @loop times
        vst1.S16     {d8, d9}, [r2]!      
        bne          dN_H
     
cor_h_x2_asm_end: 
 
        add        r13, r13, #180 
        ldmfd      r13!, {r4 - r12, r15}

        @.ENd
