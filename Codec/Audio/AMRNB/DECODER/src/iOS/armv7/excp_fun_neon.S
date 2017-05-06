@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void excp_fun(Word16 *ptr, 
@	       Word16 excp[],
@	       Word16 pit_sharp,
@	       Word16 gain_pit)
        #include "voAMRNBDecID.h"
        .text   
        .align 4
        .globl    _excp_fun 

@******************************
@ Neon register 
@******************************
@temp         .req            d0.s16
@temp1        .req            d1.s16

@a10          .req            d2.s16
@a11          .req            d3.s16
@a12          .req            d4.s16
@a13          .req            d5.s16              
@a14          .req            d6.s16
@a15          .req            d7.s16 
@a16          .req            d8.s16
@a17          .req            d9.s16 
@a18          .req            d10.s16
@a19          .req            d11.s16 

@L_temp       .req            q10.s32
@L_temp1      .req            q11.s32


_excp_fun:

        stmfd           r13!, {r4 - r12, r14}  
        mov             r4, r0
        mov             r5, r1
        vmov.s16        d0[0], r2
        vmov.s16        d0[1], r3 
        vld1.s16        {d2, d3, d4, d5}, [r4]!
        vld1.s16        {d6, d7, d8, d9}, [r4]! 
        vld1.s16        {d10, d11}, [r4]!
        vmov.s32        q11, #0x8000

        vmull.s16       q10, d2, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d2, q10, #16

        vmull.s16       q10, d3, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d3, q10, #16

        vmull.s16       q10, d4, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d4, q10, #16

        vmull.s16       q10, d5, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d5, q10, #16

        vmull.s16       q10, d6, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d6, q10, #16

        vmull.s16       q10, d7, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d7, q10, #16

        vmull.s16       q10, d8, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d8, q10, #16

        vmull.s16       q10, d9, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d9, q10, #16

        vmull.s16       q10, d10, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d10, q10, #16

        vmull.s16       q10, d11, d0[0]
        vshrn.s32       d1, q10, #15
        vqdmull.s16     q10, d1, d0[1]
        vshr.s32        q10, q10, #1
        vqadd.s32       q10, q10, q11
        vshrn.s32       d11, q10, #16          

        vst1.s16       {d2, d3, d4, d5}, [r1]!
        vst1.s16       {d6, d7, d8, d9}, [r1]!
        vst1.s16       {d10, d11}, [r1]!         
       
excp_fun_end: 
 
        ldmfd           r13!, {r4 - r12, r15}
        @.ENd
