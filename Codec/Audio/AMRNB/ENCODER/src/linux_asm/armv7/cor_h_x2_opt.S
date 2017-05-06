@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
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
        .section  .text
	.global   voAMRNBEnccor_h_x2_asm

@******************************
@ constant
@******************************
.equ    Y32          , 0
.equ    TEMP1        , Y32 + 40 * 4
.equ    H_PTR        , TEMP1 + 4
.equ    X_PTR        , H_PTR + 4
.equ    DN_PTR       , X_PTR + 4
.equ    SF           , DN_PTR + 4
.equ    NB_TRACK     , SF + 10*4 + 4
.equ    STEPM        , NB_TRACK + 4
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

voAMRNBEnccor_h_x2_asm:

        STMFD      r13!, {r0 - r12, r14}  
        SUB        r13, r13, #164               @ y32 temp spqce in stack
        LDR        r10, [r13, #STEPM]           @ r10 = step
        LDR        r11, [r13, #NB_TRACK]        @ r11 = nb_track
        MOV        r9, #5                       @ r9 --- tot = 5
        MOV        r8, #0                       @ r8 --- k = 0

LOOPK:
        MOV        r7, #0                       @ r7 = max
        MOV        r4, r8                       @ r4 --- k -- i
        STR        r9, [r13, #TEMP1]

LOOPI:
        LDR        r0, [r13, #H_PTR]            @ get h[] address
        LDR        r1, [r13, #X_PTR]            @ get x[] address
        MOV        r9, #0                       @ s = 0
        VMOV.S32   Q10, #0                      @ sum = 0

        RSB        r12, r4, #40                 @ r12 = loop times  L_CODE - i
        ADD        r1, r1, r4, LSL #1           @ r1 --- x[i]

        ANDS       r2, r12, #3
        BEQ        LOOPJ_4x

PRE_LOOPJ: 
        LDRSH      r5, [r1], #2                 @ x[j]
        LDRSH      r6, [r0], #2                 @ h[j-i]
        SUBS       r2, r2, #1
        MLA        r9, r5, r6, r9
        BNE        PRE_LOOPJ

LOOPJ_4x:

        MOVS       r12, r12, LSR #2
        BEQ        SKIPLOOPJ

LOOPJ:
         LDRSH      r5, [r1], #2
         LDRSH      r6, [r0], #2
         LDRSH      r3, [r1], #2
         LDRSH      r14,[r0], #2
         MLA        r9, r5, r6, r9
         LDRSH      r5, [r1], #2
         LDRSH      r6, [r0], #2
         MLA        r9, r3, r14, r9
         LDRSH      r3, [r1], #2
         LDRSH      r14,[r0], #2
         MLA        r9, r5, r6, r9
         SUBS       r12, r12, #1
         MLA        r9, r3, r14, r9
         BNE        LOOPJ

SKIPLOOPJ:
        MOV        r14, r9, LSL #1                  @ s = s << 1 
        STR        r14, [r13, r4, LSL #2]           @ y32[i] = s

        CMP        r9, #0
        RSBLT      r9, r9, #0

        CMP        r9, r7                          @ r9 --- s   
        MOVGT      r7, r9
    
        ADD        r4, r4, r10                     @ r10 --- step 
        CMP        r4, #40
        BLT        LOOPI

        LDR        r9, [r13, #TEMP1]               @get tot = 5
        MOV        r7, r7, ASR #1                  @r7----max   L_shr(max, 1)
        SUBS       r11, r11, #1
        QADD       r9, r9, r7
        
        ADD        r8, r8, #1
        BNE        LOOPK

        LDR        r1, [r13,#SF]
        CLZ        r0, r9                  @r9>0
        SUB        r0, r0,#1
        SUB        r0, r0, r1               @r0 ---> j = sub(norm(tot), sf)@
        
        MOV        r5, #0x8000
        VMOV.S32   Q13, #0x8000
        LDR        r2, [r13,#DN_PTR]       @r2->dn[i]
        MOV        r1, r13                 @r1->y32[i]
        MOV        r3, #10
        
        CMP        r0,#0
        VDUP.S32   Q12, r0
        BGT        DN_L                   @j>0
        BEQ        DN_H                   @j=0,hold
        RSB        r0,r0,#0               @j<0
        VDUP.S32   Q12, r0

DN_R:
        LDR        r12,[r1],#4          @r12=y32[i]
        LDR        r11,[r1],#4
        LDR        r10,[r1],#4
        LDR        r9,[r1],#4

        MOV        r12,r12,ASR r0       @L_shl (y32[i], j) j<0
        MOV        r11,r11,ASR r0
        MOV        r10,r10,ASR r0
        MOV        r9,r9,ASR r0

        QADD       r12,r12,r5
        QADD       r11,r11,r5
        QADD       r10,r10,r5
        QADD     r9,r9,r5

        MOV        r12, r12, ASR #16
        MOV        r11, r11, ASR #16
        MOV        r10, r10, ASR #16
        MOV        r9, r9, ASR #16

        SUBS       r3,r3,#1             @loop times
      
        STRH       r12,[r2], #2
        STRH       r11,[r2], #2
        STRH       r10,[r2], #2
        STRH       r9, [r2], #2         @dn[i] = round (L_shl (y32[i], j))    

        BNE        DN_R
        B          cor_h_x2_asm_end
        
DN_L:
        VLD1.S32     {Q6, Q7}, [r1]!
        VQSHL.S32    Q10, Q6, Q12
        VQADD.S32    Q6, Q10, Q13
        VQSHL.S32    Q10, Q7, Q12
        VQADD.S32    Q7, Q10, Q13
        VSHRN.S32    D8, Q6, #16
        VSHRN.S32    D9, Q7, #16  
 
        SUBS         r3,r3,#2             @loop times
        VST1.S16     {D8, D9}, [r2]!      
        BNE          DN_L
        B            cor_h_x2_asm_end

DN_H:
        VLD1.S32     {Q6, Q7}, [r1]!
        VQADD.S32    Q6, Q6, Q13
        VQADD.S32    Q7, Q7, Q13
        VSHRN.S32    D8, Q6, #16
        VSHRN.S32    D9, Q7, #16  
 
        SUBS         r3,r3,#2             @loop times
        VST1.S16     {D8, D9}, [r2]!      
        BNE          DN_H
     
cor_h_x2_asm_end: 
 
        ADD        r13, r13, #180 
        LDMFD      r13!, {r4 - r12, r15}

        .END
