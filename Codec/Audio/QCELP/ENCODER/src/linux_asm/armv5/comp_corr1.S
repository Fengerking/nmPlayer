@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void comp_corr( 
@			   short *scal_sig,   /* i   : scaled signal.                          */
@			   short *target,
@			   short L_frame,     /* i   : length of frame to compute pitch        */
@			   short lag_max,     /* i   : maximum lag                             */
@			   short lag_min,     /* i   : minimum lag                             */
@			   int	*corr)       /* o   : correlation of selected lag             */

@*************************************************
@ ARM register 
@*************************************************
@ short *scal_sig               RN           0
@ short *target                 RN           1
@ L_frame                       RN           2
@ lag_max                       RN           3
@ lag_min                       RN           4
@ *corr                         RN           5

        .section  .text
        .global  comp_corr10
	.global  comp_corr40


comp_corr10:

        STMFD         r13!, {r4 - r12, r14}  
        MOV           r6,  #0x80000000
        @for(i = lag_max@ i >= lag_min@ i--)
LOOP10:
        MOV           r4, r1                         @ p = target
        RSB           r5, r3, #0                     @ -i
        ADD           r7, r0, r5, LSL #1             @ p1 = &scal_sig[-i]
        MOV           r11, r2
        MOV           r14, #0                        @ t0 = 0
LOOP21:
        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        SUBS          r11, r11, #10                  @ j -= 10
        BNE           LOOP21
        
        CMP           r14,  r6
        MOVGT         r6,  r14
        MOVGT         r12, r3 
        SUB           r3, r3, #1
        CMP           r3, #1
        BGE           LOOP10

comp_corr10_end:                    
        MOV           r0, r12
        LDMFD      r13!, {r4 - r12, r15} 


comp_corr40:

        STMFD         r13!, {r4 - r12, r14}  
        MOV           r6,  #0x80000000
        @for(i = lag_max@ i >= lag_min@ i--)
LOOP40:
        MOV           r4, r1                         @ p = target
        RSB           r5, r3, #0                     @ -i
        ADD           r7, r0, r5, LSL #1             @ p1 = &scal_sig[-i]
        MOV           r11, r2
        MOV           r14, #0                        @ t0 = 0
LOOP22:
        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        LDR           r8,  [r4], #4
        LDRSH         r9,  [r7], #2
        LDRSH         r10, [r7], #2
        SMLABB        r14, r8, r9, r14
        SMLATB        r14, r8, r10, r14

        SUBS          r11, r11, #10                  @ j -= 10
        BNE           LOOP22
        
        CMP           r14,  r6
        MOVGT         r6,  r14
        MOVGT         r12, r3 
        SUB           r3, r3, #1
        CMP           r3, #1
        BGE           LOOP40

comp_corr40_end:
                        
        MOV           r0, r12
        LDMFD      r13!, {r4 - r12, r15} 
        @ENDP

        .END

