@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@Word16 G_pitch     (    /* o : Gain of pitch lag saturated to 1.2       */
@    enum Mode mode,     /* i : AMR mode                                 */
@    Word16 xn[],        /* i : Pitch target.                            */
@    Word16 y1[],        /* i : Filtered adaptive codebook.              */
@    Word16 g_coeff[],   /* i : Correlations need for gain quantization  */
@    Word16 L_subfr      /* i : Length of subframe.                      */
@)
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _G_pitch_asm
	.globl   _div_s_v7

_G_pitch_asm:

        stmfd          r13!, {r4 - r12, r14}  
        vld1.s16       {q0, q1}, [r1]!  
        vld1.s16       {q2, q3}, [r1]!
        vld1.s16       {q4}, [r1]!
        vld1.s16       {q5, q6}, [r2]!
        vld1.s16       {q7, q8}, [r2]!
        vld1.s16       {q9}, [r2]!

        vqdmull.s16    q11, d0, d10 
        vqdmlal.s16    q11, d1, d11 
        vqdmlal.s16    q11, d2, d12 
        vqdmlal.s16    q11, d3, d13 
        vqdmlal.s16    q11, d4, d14 
        vqdmlal.s16    q11, d5, d15 
        vqdmlal.s16    q11, d6, d16 
        vqdmlal.s16    q11, d7, d17 
        vqdmlal.s16    q11, d8, d18 
        vqdmlal.s16    q11, d9, d19
        mov            r8, #0x8000                                   @ set 0x8000 for round operate 
        mov            r9, #1                                        @ s = 1L

        vqadd.s32      d22, d22, d23
        vmov.s32       r12, d22[0]
        vmov.s32       r14, d22[1]
        qadd           r4, r12, r14
        qadd           r4, r4, r9                                    @ get the result s

@********************
@ while num = 0x4A , the r4 result is diff with c code
@********************
        eor            r5, r4, r4, lsl #1
        clz            r7, r5                                        @ exp_xy = norm_l(s)
        mov            r11, r4, lsl r7
        qadd           r6, r8, r11                                   
        mov            r6, r6, asr #16                               @ xy = round(s << exp_xy)
        cmp            r6, #4
        blt            Zero_end 

        vqdmull.s16    q10, d10,  d10
        vqdmlal.s16    q10, d11,  d11
        vqdmlal.s16    q10, d12,  d12
        vqdmlal.s16    q10, d13,  d13
        vqdmlal.s16    q10, d14,  d14
        vqdmlal.s16    q10, d15,  d15
        vqdmlal.s16    q10, d16,  d16
        vqdmlal.s16    q10, d17,  d17
        vqdmlal.s16    q10, d18,  d18
        vqdmlal.s16    q10, d19,  d19

        vqadd.s32      d20, d20, d21
        vmov.s32       r12, d20[0]
        vmov.s32       r14, d20[1]
        qadd           r4, r12, r14           
        qadd           r4, r4, r9                                   @ get the result s        
 
        eor            r5, r4, r4, lsl #1
        clz            r9, r5                                       @ exp_yy = norm_l (s)
 
        mov            r11, r4, lsl r9                 
        qadd           r10, r8, r11                                 
        mov            r10, r10, asr #16                            @ yy=round(s << exp_yy)
         
@ used register
@ r6 --- xy, r7 --- exp_xy, r10 --- yy, r9 --- exp_yy

        strh           r10, [r3], #2                @g_coeff[0] = yy@
        rsb            r5, r9, #15                  @15 - exp_yy
        rsb            r8, r7, #15                  @15 - exp_xy
        strh           r5, [r3], #2                 @g_coeff[1] = 15 - exp_yy
        strh           r6, [r3], #2                 @g_coeff[2] = xy
        strh           r8, [r3]                     @g_coeff[3] = 15 - exp_xy

        mov            r6, r6, asr #1               @ xy = (xy >>1)
        
        mov            r1, r10
        mov            r0, r6
        bl             _div_s_v7
    
        cmp            r7, r9
        subge          r8, r7, r9                   @ i = exp_xy - exp_yy
        movge          r0, r0, asr r8               @ gain = shr(gain, i)
        sublt          r8, r9, r7
        movlt          r0, r0, lsl r8
        
        ldr            r11, Cvalue4
        cmp            r0, r11
        movgt          r0, r11
       
        bic            r0, r0, #1
        bic            r0, r0, #2
        b              G_pitch_asm_end


Zero_end: 
        mov            r0, #0                        @return ((Word16) 0)

G_pitch_asm_end: 
 
        ldmfd          r13!, {r4 - r12, r15}
 
Cvalue4:
        .word          19661
       
	@.ENd
