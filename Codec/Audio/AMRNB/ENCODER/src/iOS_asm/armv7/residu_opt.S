@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void Residu (
@    Word16 a[], /* (i)     : prediction coefficients                      */
@    Word16 x[], /* (i)     : speech signal                                */
@    Word16 y[], /* (o)     : residual signal                              */
@    Word16 lg   /* (i)     : size of filtering                            */
@)
        #include "voAMRNBEncID.h"
        .text     .align   4
	.globl   _Vo_Residu_asm

_Vo_Residu_asm:
        stmfd          r13!, {r4 - r12, r14} 
        sub            r7, r3, #4                       @i = lg - 4
        
        vld1.s16       {d0, d1, d2}, [r0]!              @get all a[]  
        vmov.s32       q8,  #0x8000
        
        vdup.s16       d4, d0[0]                       @load a[0]
        vdup.s16       d5, d0[1]                       @load a[1]
        vdup.s16       d6, d0[2]                       @load a[2]
        vdup.s16       d7, d0[3]                       @load a[3]
        vdup.s16       d8, d1[0]                       @load a[4]
        vdup.s16       d9, d1[1]                       @load a[5]
        vdup.s16       d10, d1[2]                      @load a[6]
        vdup.s16       d11, d1[3]                      @load a[7]
        vdup.s16       d12, d2[0]                      @load a[8]
        vdup.s16       d13, d2[1]                      @load a[9]
        vdup.s16       d14, d2[2]                      @load a[10]
                               
LOOP1:
        add            r9, r1, r7, lsl #1              @copy the address
        add            r10, r2, r7, lsl #1
        mov            r8, r9
        vld1.s16       d3, [r8]!                       @get x[i], x[i+1], x[i+2], x[i+3]
        vqdmull.s16    q10, d3, d4                     @finish the first L_mult

        sub            r8, r9, #2                      @get the x[i-1] address
        vld1.s16       d3, [r8]! 
        vqdmlal.s16    q10, d3, d5

        sub            r8, r9, #4                      @load the x[i-2] address
        vld1.s16       d3, [r8]!
        vqdmlal.s16    q10, d3, d6

        sub            r8, r9, #6                      @load the x[i-3] address
        vld1.s16       d3, [r8]!     
        vqdmlal.s16    q10, d3, d7                    

        sub            r8, r9, #8                      @load the x[i-4] address
        vld1.s16       d3, [r8]!     
        vqdmlal.s16    q10, d3, d8  

        sub            r8, r9, #10                     @load the x[i-5] address
        vld1.s16       d3, [r8]!     
        vqdmlal.s16    q10, d3, d9 

        sub            r8, r9, #12                     @load the x[i-6] address
        vld1.s16            d3, [r8]!     
        vqdmlal.s16    q10, d3, d10  

        sub            r8, r9, #14                     @load the x[i-7] address
        vld1.s16            d3, [r8]!     
        vqdmlal.s16    q10, d3, d11  

        sub            r8, r9, #16                     @load the x[i-8] address
        vld1.s16            d3, [r8]!     
        vqdmlal.s16    q10, d3, d12  

        sub            r8, r9, #18                     @load the x[i-9] address
        vld1.s16       d3, [r8]!     
        vqdmlal.s16    q10, d3, d13         
           
        sub            r8, r9, #20                     @load the x[i-10] address
        vld1.s16       d3, [r8]!     
        vqdmlal.s16    q10, d3, d14  

        sub            r7, r7, #4                      @i-=4
        vqshl.s32      q10, q10, #3
        vqadd.s32      q10, q10, q8
        vshrn.s32      d3, q10, #16
        vst1.s16       d3, [r10]!
        cmp            r7,  #0

        bge            LOOP1

Residu_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}

        @.ENd
