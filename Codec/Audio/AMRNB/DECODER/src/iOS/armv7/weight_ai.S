@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void Weight_Ai (
@    Word16 a[],         /* (i)     : a[M+1]  LPC coefficients   (M=10)    */
@    const Word16 fac[], /* (i)     : Spectral expansion factors.          */
@    Word16 a_exp[]      /* (o)     : Spectral expanded LPC coefficients   */
@)
        #include "voAMRNBDecID.h"
        .text   
        .align 4
        .globl _weight_asm

@******************************
@ constant
@******************************

@******************************
@ ARM register 
@******************************
@ a[]   ---   r0
@ fac[] ---   r1
@ a_exp[] --- r2
        
@******************************
@ Neon register 
@******************************
@src1           .req            d0.S16      @a[i]
@src2           .req            d1.S16
@src3           .req            d2.S16      @fac[]
@src4           .req            d3.S16   
@dest1          .req            d4.S16      @a_exp[]
@dest2          .req            d5.S16

@re1            .req            q4.S32
@re2            .req            q5.S32  

@******************************
@ Constants
@******************************

_weight_asm:

        stmfd        r13!, {r4 - r12, r14} 
        mov          r12, #0x8000 
        ldrsh        r4, [r0], #2                         @r4---a[0]
        ldrsh        r7, [r1], #2                         @r7---fac[0]
        ldrsh        r5, [r0], #2                         @r5---a[1]
        ldrsh        r8, [r1], #2                         @r8---fac[1]
        ldrsh        r6, [r0], #2                         @r6---a[2]
        vmov.S32     q4, #0x8000
        mul          r9,  r5, r7
        strh         r4, [r2], #2                         @a_exp[0] = a[0]
        vld2.S16     {d2, d3}, [r1]!                      @ get 8 fac[]
        add          r10, r12, r9, LSL #1
        mul          r11, r6, r8
        mov          r10, r10, LSR #16             
        vld2.S16     {d0, d1}, [r0]!                      @ get 8 a[]
        add          r4, r12, r11, LSL #1
        vmov.S32     q5, #0x8000
        mov          r4, r4, LSR #16
        strh         r10, [r2], #2
        vqdmlal.S16  q4, d0, d2                          @ get the mul<<1 + 0x8000 result
        vqdmlal.S16  q5, d1, d3 
        strh         r4 , [r2], #2

        vshrn.S32    d4, q4, #16
        vshrn.S32    d5, q5, #16                        @ c1, c2 have right shift 16 bits 
        vst2.S16     {d4, d5}, [r2]! 

weight_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}
        
        @.END
