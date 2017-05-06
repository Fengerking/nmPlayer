@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void ex_ltp(Word16 exc_enhanced[],
@            Word16 code[], 
@            Word16 *ptr,
@            Word16 pitch_fac, 
@            Word16 gain_code,
@            Word16 tmp_shift)
        #include "voAMRNBDecID.h"
        .text   
        .align 4
        .globl  _ex_ltp 

@******************************
@ ARM register 
@******************************
@ exc_enhanced[]        RN         0
@ code[]                RN         1
@ *ptr[]                RN         2
@ pitch_fac             RN         3
@ gain_code             RN         4
@ tmp_shift             RN         5
        
@******************************
@ Neon register 
@******************************
@a10          .req            d2.S16
@a11          .req            d3.S16              
@a12          .req            d4.S16
@a13          .req            d5.S16 
@a14          .req            d6.S16
@a15          .req            d7.S16 
@a16          .req            d8.S16
@a17          .req            d9.S16 
@a18          .req            d10.S16
@a19          .req            d11.S16 

@b10          .req            d12.S16
@b11          .req            d13.S16              
@b12          .req            d14.S16
@b13          .req            d15.S16 
@b14          .req            d16.S16
@b15          .req            d17.S16 
@b16          .req            d18.S16
@b17          .req            d19.S16 
@b18          .req            d20.S16
@b19          .req            d21.S16
 
@sum          .req            q11.S32
@sum_hi       .req            d22.S16

@temp2        .req            d24.S16
@L_temp       .req            q13.S32
@L_temp1      .req            q14.S32


_ex_ltp:
        stmfd             r13!, {r4 - r12, r14}  
        ldr               r4, [r13, #40]                          @ get gain_code
        ldr               r5, [r13, #44]                          @ get tmp_shift
        mov               r8, #40
        mov               r6, r2                                  @ copy *ptr
        @load all *(ptr), code[]
        
        vld1.S16          {d2, d3, d4, d5}, [r6]!
        vld1.S16          {d6, d7, d8, d9}, [r6]!
        vld1.S16          {d10, d11}, [r6]!

        vld1.S16          {d12, d13, d14, d15}, [r1]!
        vld1.S16          {d16, d17, d18, d19}, [r1]!
        vld1.S16          {d20, d21}, [r1]!
   
        vst1.S16          {d2, d3, d4, d5}, [r0]! 
        vst1.S16          {d6, d7, d8, d9}, [r0]!
        vst1.S16          {d10, d11}, [r0]!

        
        vmov.S16           d0[0], r3                          @ pitch_fac
        vmov.S16           d0[1], r4                          @ gain_code
        vdup.S32           q14, r5                            @ tmp_shift
        vmov.S32           q13, #0x8000


        vqdmull.S16        q11, d2, d0[0]                      
        vqdmlal.S16        q11, d12, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 
 
        vqdmull.S16        q11, d3, d0[0]                      
        vqdmlal.S16        q11, d13, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d4, d0[0]                      
        vqdmlal.S16        q11, d14, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d5, d0[0]                      
        vqdmlal.S16        q11, d15, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d6, d0[0]                      
        vqdmlal.S16        q11, d16, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d7, d0[0]                      
        vqdmlal.S16        q11, d17, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d8, d0[0]                      
        vqdmlal.S16        q11, d18, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]!  

        vqdmull.S16        q11, d9, d0[0]                      
        vqdmlal.S16        q11, d19, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

        vqdmull.S16        q11, d10, d0[0]                      
        vqdmlal.S16        q11, d20, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]!    

        vqdmull.S16        q11, d11, d0[0]                      
        vqdmlal.S16        q11, d21, d0[1]
        vqshl.S32          q11, q11, q14
        vqadd.S32          q11, q11, q13
        vshrn.S32          d24, q11, #16        
        vst1.S16           d24, [r2]! 

ex_ltp_end: 
 
        ldmfd              r13!, {r4 - r12, r15}
    
        @.ENd
