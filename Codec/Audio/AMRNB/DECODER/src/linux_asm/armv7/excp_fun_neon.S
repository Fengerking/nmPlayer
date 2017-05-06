@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void excp_fun(Word16 *ptr, 
@	       Word16 excp[],
@	       Word16 pit_sharp,
@	       Word16 gain_pit)
        #include "voAMRNBDecID.h"
        .section .text
        .global    excp_fun 

@******************************
@ Neon register 
@******************************
@temp         .req            D0.S16
@temp1        .req            D1.S16

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

@L_temp       .req            Q10.S32
@L_temp1      .req            Q11.S32


excp_fun:
        STMFD           r13!, {r4 - r12, r14}  
        MOV             r4, r0
        MOV             r5, r1
        VMOV.S16        D0[0], r2
        VMOV.S16        D0[1], r3 
        VLD1.S16        {D2, D3, D4, D5}, [r4]!
        VLD1.S16        {D6, D7, D8, D9}, [r4]! 
        VLD1.S16        {D10, D11}, [r4]!
        VMOV.S32        Q11, #0x8000

        VMULL.S16       Q10, D2, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D2, Q10, #16

        VMULL.S16       Q10, D3, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D3, Q10, #16

        VMULL.S16       Q10, D4, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D4, Q10, #16

        VMULL.S16       Q10, D5, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D5, Q10, #16

        VMULL.S16       Q10, D6, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D6, Q10, #16

        VMULL.S16       Q10, D7, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D7, Q10, #16

        VMULL.S16       Q10, D8, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D8, Q10, #16

        VMULL.S16       Q10, D9, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D9, Q10, #16

        VMULL.S16       Q10, D10, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D10, Q10, #16

        VMULL.S16       Q10, D11, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D11, Q10, #16          

        VST1.S16       {D2, D3, D4, D5}, [r1]!
        VST1.S16       {D6, D7, D8, D9}, [r1]!
        VST1.S16       {D10, D11}, [r1]!         
       
excp_fun_end: 
 
        LDMFD      r13!, {r4 - r12, r15}
        .END
