@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void ex_ltp(Word16 exc_enhanced[],
@            Word16 code[], 
@            Word16 *ptr,
@            Word16 pitch_fac, 
@            Word16 gain_code,
@            Word16 tmp_shift)
        #include "voAMRNBDecID.h"
        .section .text
        .global  ex_ltp 

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
@a10          .req            D2.S16
@a11          .req            D3.S16              
@a12          .req            D4.S16
@a13          .req            D5.S16 
@a14          .req            D6.S16
@a15          .req            D7.S16 
@a16          .req            D8.S16
@a17          .req            D9.S16 
@a18          .req            D10.S16
@a19          .req            D11.S16 

@b10          .req            D12.S16
@b11          .req            D13.S16              
@b12          .req            D14.S16
@b13          .req            D15.S16 
@b14          .req            D16.S16
@b15          .req            D17.S16 
@b16          .req            D18.S16
@b17          .req            D19.S16 
@b18          .req            D20.S16
@b19          .req            D21.S16
 
@sum          .req            Q11.S32
@sum_hi       .req            D22.S16

@temp2        .req            D24.S16
@L_temp       .req            Q13.S32
@L_temp1      .req            Q14.S32


ex_ltp:
        STMFD             r13!, {r4 - r12, r14}  
        LDR               r4, [r13, #40]                          @ get gain_code
        LDR               r5, [r13, #44]                          @ get tmp_shift
        MOV               r8, #40
        MOV               r6, r2                                  @ copy *ptr
        @load all *(ptr), code[]
        
        VLD1.S16          {D2, D3, D4, D5}, [r6]!
        VLD1.S16          {D6, D7, D8, D9}, [r6]!
        VLD1.S16          {D10, D11}, [r6]!

        VLD1.S16          {D12, D13, D14, D15}, [r1]!
        VLD1.S16          {D16, D17, D18, D19}, [r1]!
        VLD1.S16          {D20, D21}, [r1]!
   
        VST1.S16          {D2, D3, D4, D5}, [r0]! 
        VST1.S16          {D6, D7, D8, D9}, [r0]!
        VST1.S16          {D10, D11}, [r0]!

        
        VMOV.S16           D0[0], r3                          @ pitch_fac
        VMOV.S16           D0[1], r4                          @ gain_code
        VDUP.S32           Q14, r5                            @ tmp_shift
        VMOV.S32           Q13, #0x8000


        VQDMULL.S16        Q11, D2, D0[0]                      
        VQDMLAL.S16        Q11, D12, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 
 
        VQDMULL.S16        Q11, D3, D0[0]                      
        VQDMLAL.S16        Q11, D13, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D4, D0[0]                      
        VQDMLAL.S16        Q11, D14, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D5, D0[0]                      
        VQDMLAL.S16        Q11, D15, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D6, D0[0]                      
        VQDMLAL.S16        Q11, D16, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D7, D0[0]                      
        VQDMLAL.S16        Q11, D17, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D8, D0[0]                      
        VQDMLAL.S16        Q11, D18, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]!  

        VQDMULL.S16        Q11, D9, D0[0]                      
        VQDMLAL.S16        Q11, D19, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D10, D0[0]                      
        VQDMLAL.S16        Q11, D20, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]!    

        VQDMULL.S16        Q11, D11, D0[0]                      
        VQDMLAL.S16        Q11, D21, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

ex_ltp_end: 
 
        LDMFD              r13!, {r4 - r12, r15}
    
        .END
