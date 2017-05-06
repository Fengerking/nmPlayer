@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@Word16 G_pitch     (    /* o : Gain of pitch lag saturated to 1.2       */
@    enum Mode mode,     /* i : AMR mode                                 */
@    Word16 xn[],        /* i : Pitch target.                            */
@    Word16 y1[],        /* i : Filtered adaptive codebook.              */
@    Word16 g_coeff[],   /* i : Correlations need for gain quantization  */
@    Word16 L_subfr      /* i : Length of subframe.                      */
@)
        #include "voAMRNBEncID.h"
        .section  .text
	.global   G_pitch_asm
	.extern   div_s_v7

G_pitch_asm:

        STMFD          r13!, {r4 - r12, r14}  
        VLD1.S16       {Q0, Q1}, [r1]!  
        VLD1.S16       {Q2, Q3}, [r1]!
        VLD1.S16       {Q4}, [r1]!
        VLD1.S16       {Q5, Q6}, [r2]!
        VLD1.S16       {Q7, Q8}, [r2]!
        VLD1.S16       {Q9}, [r2]!

        VQDMULL.S16    Q11, D0, D10 
        VQDMLAL.S16    Q11, D1, D11 
        VQDMLAL.S16    Q11, D2, D12 
        VQDMLAL.S16    Q11, D3, D13 
        VQDMLAL.S16    Q11, D4, D14 
        VQDMLAL.S16    Q11, D5, D15 
        VQDMLAL.S16    Q11, D6, D16 
        VQDMLAL.S16    Q11, D7, D17 
        VQDMLAL.S16    Q11, D8, D18 
        VQDMLAL.S16    Q11, D9, D19
        MOV            r8, #0x8000                                   @ set 0x8000 for round operate 
        MOV            r9, #1                                        @ s = 1L

        VQADD.S32      D22, D22, D23
        VMOV.S32       r12, D22[0]
        VMOV.S32       r14, D22[1]
        QADD           r4, r12, r14
        QADD           r4, r4, r9                                    @ get the result s

@********************
@ while num = 0x4A , the r4 result is diff with c code
@********************
        EOR            r5, r4, r4, LSL #1
        CLZ            r7, r5                                        @ exp_xy = norm_l(s)
        MOV            r11, r4, LSL r7
        QADD           r6, r8, r11                                   
        MOV            r6, r6, ASR #16                               @ xy = round(s << exp_xy)
        CMP            r6, #4
        BLT            Zero_end 

        VQDMULL.S16    Q10, D10,  D10
        VQDMLAL.S16    Q10, D11,  D11
        VQDMLAL.S16    Q10, D12,  D12
        VQDMLAL.S16    Q10, D13,  D13
        VQDMLAL.S16    Q10, D14,  D14
        VQDMLAL.S16    Q10, D15,  D15
        VQDMLAL.S16    Q10, D16,  D16
        VQDMLAL.S16    Q10, D17,  D17
        VQDMLAL.S16    Q10, D18,  D18
        VQDMLAL.S16    Q10, D19,  D19

        VQADD.S32      D20, D20, D21
        VMOV.S32       r12, D20[0]
        VMOV.S32       r14, D20[1]
        QADD           r4, r12, r14           
        QADD           r4, r4, r9                                   @ get the result s        
 
        EOR            r5, r4, r4, LSL #1
        CLZ            r9, r5                                       @ exp_yy = norm_l (s)
 
        MOV            r11, r4, LSL r9                 
        QADD           r10, r8, r11                                 
        MOV            r10, r10, ASR #16                            @ yy=round(s << exp_yy)
         
@ used register
@ r6 --- xy, r7 --- exp_xy, r10 --- yy, r9 --- exp_yy

        STRH           r10, [r3], #2                @g_coeff[0] = yy@
        RSB            r5, r9, #15                  @15 - exp_yy
        RSB            r8, r7, #15                  @15 - exp_xy
        STRH           r5, [r3], #2                 @g_coeff[1] = 15 - exp_yy
        STRH           r6, [r3], #2                 @g_coeff[2] = xy
        STRH           r8, [r3]                     @g_coeff[3] = 15 - exp_xy

        MOV            r6, r6, ASR #1               @ xy = (xy >>1)
        
        MOV            r1, r10
        MOV            r0, r6
        BL             div_s_v7
    
        CMP            r7, r9
        SUBGE          r8, r7, r9                   @ i = exp_xy - exp_yy
        MOVGE          r0, r0, ASR r8               @ gain = shr(gain, i)
        SUBLT          r8, r9, r7
        MOVLT          r0, r0, LSL r8
        
        LDR            r11, =19661
        CMP            r0, r11
        MOVGT          r0, r11
       
        BIC            r0, r0, #1
        BIC            r0, r0, #2
        B              G_pitch_asm_end


Zero_end: 
        MOV            r0, #0                        @return ((Word16) 0)

G_pitch_asm_end: 
 
        LDMFD          r13!, {r4 - r12, r15}
        
	.END
