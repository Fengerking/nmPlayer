@**************************************************************
@* Copyright 2008 by VisualOn software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn software, Inc. ALL RIGHTs REsERVEd.
@****************************************************************
@void comp_corr ( 
@    Word16 scal_sig[],  /* i   : scaled signal.                          */
@    Word16 lag_max,     /* i   : maximum lag                             */
@    Word16 lag_min,     /* i   : minimum lag                             */
@    Word32 corr[])      /* o   : correlation of selected lag             */
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _comp_corr_asm

_comp_corr_asm:
        stmfd       r13!, {r4 - r12, r14} 
        mov         r4, r1                     @r4 = lag_max
  
First_loop:

        vmov.s32        q12, #0                              @ Initialise partial accumulators to zero
        mov             r9, r0
        sub             r10, r9, r4, lsl #1
       
       
        vld1.s16        {d0, d1, d2, d3}, [r9]!  
        vld1.s16        {d4, d5, d6, d7}, [r9]!  
        vld1.s16        {d8, d9}, [r9]!
                   
        vld1.s16        {d10, d11, d12, d13}, [r10]!  
        vld1.s16        {d14, d15, d16, d17}, [r10]! 
        vld1.s16        {d18, d19}, [r10]!  
                      
                         
        vqdmlal.s16     q12, d0, d10
        vqdmlal.s16     q12, d1, d11
        vqdmlal.s16     q12, d2, d12
        vqdmlal.s16     q12, d3, d13
        vqdmlal.s16     q12, d4, d14
        vqdmlal.s16     q12, d5, d15
        vqdmlal.s16     q12, d6, d16
        vqdmlal.s16     q12, d7, d17
        vqdmlal.s16     q12, d8, d18
        vqdmlal.s16     q12, d9, d19  
                
        vld1.s16        {d0, d1, d2, d3}, [r9]!  
        vld1.s16        {d4, d5, d6, d7}, [r9]!  
        vld1.s16        {d8, d9}, [r9]!
                   
        vld1.s16        {d10, d11, d12, d13}, [r10]!  
        vld1.s16        {d14, d15, d16, d17}, [r10]! 
        vld1.s16        {d18, d19}, [r10]!  
                      
                         
        vqdmlal.s16     q12, d0, d10
        vqdmlal.s16     q12, d1, d11
        vqdmlal.s16     q12, d2, d12
        vqdmlal.s16     q12, d3, d13
        vqdmlal.s16     q12, d4, d14
        vqdmlal.s16     q12, d5, d15
        vqdmlal.s16     q12, d6, d16
        vqdmlal.s16     q12, d7, d17
        vqdmlal.s16     q12, d8, d18
        vqdmlal.s16     q12, d9, d19  
           

        vqadd.s32       d24, d24, d25

        vpadd.s32       d24, d24, d24
        vmov.s32        r5,  d24[0]
        
        sub             r11, r3, r4, lsl #2
        sub             r4, r4, #1        
        str             r5, [r11]
        cmp             r4, r2
        bge             First_loop

comp_corr_end:  
        ldmfd           r13!, {r4 - r12, r15}
        @.ENd
