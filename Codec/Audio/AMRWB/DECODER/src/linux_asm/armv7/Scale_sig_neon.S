@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@void Scale_sig(
@               Word16 x[],                           /* (i/o) : signal to scale               */
@               Word16 lg,                            /* (i)   : size of x[]                   */
@               Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
@)
@****************************************************************
@  x[]   ---  r0
@  lg    ---  r1
@  exp   ---  r2
          #include "voAMRWBDecID.h"
          .section  .text
          .global   Scale_sig_asm

Scale_sig_asm:

          STMFD   	r13!, {r4 - r12, r14} 
          MOV           r4, #4
          VMOV.S32      Q15, #0x8000       
          VDUP.S32      Q14, r2  
          MOV           r5, r0                          @ copy x[] address    
          CMP           r1, #64
          MOVEQ         r4, #1
          BEQ           LOOP
          CMP           r1, #256
          BEQ           LOOP
          @312-256 = 56
          VLD1.S16      {Q0, Q1}, [r5]!                 @load 16 Word16 x[]
          VLD1.S16      {Q2, Q3}, [r5]!                 @load 16 Word16 x[] 
          VLD1.S16      {Q4, Q5}, [r5]!                 @load 16 Word16 x[]
          VLD1.S16      {Q6}, [r5]!

          VSHLL.S16     Q10, D0, #16
          VSHLL.S16     Q11, D1, #16
          VSHLL.S16     Q12, D2, #16
          VSHLL.S16     Q13, D3, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VADDHN.S32    D18, Q12, Q15
          VADDHN.S32    D19, Q13, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]
          VSHLL.S16     Q10, D4, #16
          VSHLL.S16     Q11, D5, #16
          VSHLL.S16     Q12, D6, #16
          VSHLL.S16     Q13, D7, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VADDHN.S32    D18, Q12, Q15
          VADDHN.S32    D19, Q13, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]  
          VSHLL.S16     Q10, D8, #16
          VSHLL.S16     Q11, D9, #16
          VSHLL.S16     Q12, D10, #16
          VSHLL.S16     Q13, D11, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VADDHN.S32    D18, Q12, Q15
          VADDHN.S32    D19, Q13, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]   
          VSHLL.S16     Q10, D12, #16
          VSHLL.S16     Q11, D13, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VST1.S16      {Q8}, [r0]!                     @store 8 Word16 x[]                        


LOOP:

          VLD1.S16      {Q0, Q1}, [r5]!                 @load 16 Word16 x[]
          VLD1.S16      {Q2, Q3}, [r5]!                 @load 16 Word16 x[]
          VLD1.S16      {Q4, Q5}, [r5]!                 @load 16 Word16 x[]
          VLD1.S16      {Q6, Q7}, [r5]!                 @load 16 Word16 x[]

          VSHLL.S16     Q8, D0, #16
          VSHLL.S16     Q9, D1, #16
          VSHLL.S16     Q10, D2, #16
          VSHLL.S16     Q11, D3, #16     
          VSHL.S32      Q8, Q8, Q14
          VSHL.S32      Q9, Q9, Q14
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VADDHN.S32    D16, Q8, Q15
          VADDHN.S32    D17, Q9, Q15
          VADDHN.S32    D18, Q10, Q15
          VADDHN.S32    D19, Q11, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]

   
          VSHLL.S16     Q12, D4, #16
          VSHLL.S16     Q13, D5, #16
          VSHLL.S16     Q10, D6, #16
          VSHLL.S16     Q11, D7, #16
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VADDHN.S32    D16, Q12, Q15
          VADDHN.S32    D17, Q13, Q15
          VADDHN.S32    D18, Q10, Q15
          VADDHN.S32    D19, Q11, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]

          VSHLL.S16     Q10, D8, #16
          VSHLL.S16     Q11, D9, #16
          VSHLL.S16     Q12, D10, #16
          VSHLL.S16     Q13, D11, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VADDHN.S32    D18, Q12, Q15
          VADDHN.S32    D19, Q13, Q15
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]

          VSHLL.S16     Q10, D12, #16   
          VSHLL.S16     Q11, D13, #16
          VSHLL.S16     Q12, D14, #16
          VSHLL.S16     Q13, D15, #16
          VSHL.S32      Q10, Q10, Q14
          VSHL.S32      Q11, Q11, Q14
          VSHL.S32      Q12, Q12, Q14
          VSHL.S32      Q13, Q13, Q14
          VADDHN.S32    D16, Q10, Q15
          VADDHN.S32    D17, Q11, Q15
          VADDHN.S32    D18, Q12, Q15
          VADDHN.S32    D19, Q13, Q15 
          VST1.S16      {Q8, Q9}, [r0]!                 @store 16 Word16 x[]  
          SUBS          r4, r4, #1
          BGT           LOOP     
                                          
Scale_sig_asm_end:

          LDMFD   	r13!, {r4 - r12, r15} 
          @ENDFUNC
          .END
 

