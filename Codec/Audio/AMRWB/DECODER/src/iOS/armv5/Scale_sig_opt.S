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
          .text   .align 4
	  .globl   _Scale_sig_asm

_Scale_sig_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          @mov           r8, #0                          @ i =0
          cmp           r2, #0
          mov           r14, r0                          @ copy x[] address 
          bgt           LOOP1
          rsb           r3, r2, #0                       @ -exp

LOOP:                
          ldrsh         r4, [r14], #2                    
          ldrsh         r5, [r14], #2
          ldrsh         r6, [r14], #2
          ldrsh         r7, [r14], #2         
          mov           r9, r4, lsl #16
          mov           r10, r5, lsl #16
          mov           r11, r6, lsl #16
          mov           r12, r7, lsl #16
          mov           r4, r9, asr r3
          mov           r5, r10, asr r3
          mov           r6, r11, asr r3
          mov           r7, r12, asr r3
          add           r9, r4, #0x8000
          add           r10, r5, #0x8000
          add           r11, r6, #0x8000
          add           r12, r7, #0x8000
          mov           r4, r9, asr #16
          mov           r5, r10, asr #16
          mov           r6, r11, asr #16
          mov           r7, r12, asr #16
          strh          r4, [r0], #2
          strh          r5, [r0], #2
          strh          r6, [r0], #2
          strh          r7, [r0], #2
          subs          r1, r1, #4
          bgt           LOOP
          b             Scale_sig_asm_end

LOOP1:
          ldrsh         r4, [r14], #2                    
          ldrsh         r5, [r14], #2
          ldrsh         r6, [r14], #2
          ldrsh         r7, [r14], #2         
          mov           r9, r4, lsl #16
          mov           r10, r5, lsl #16
          mov           r11, r6, lsl #16
          mov           r12, r7, lsl #16
          mov           r4, r9, lsl r2
          mov           r5, r10, lsl r2
          mov           r6, r11, lsl r2
          mov           r7, r12, lsl r2
          add           r9, r4, #0x8000
          add           r10, r5, #0x8000
          add           r11, r6, #0x8000
          add           r12, r7, #0x8000
          mov           r4, r9, asr #16
          mov           r5, r10, asr #16
          mov           r6, r11, asr #16
          mov           r7, r12, asr #16
          strh          r4, [r0], #2
          strh          r5, [r0], #2
          strh          r6, [r0], #2
          strh          r7, [r0], #2
          subs          r1, r1, #4
          bgt           LOOP1      
                          
Scale_sig_asm_end:

          ldmfd   	r13!, {r4 - r12, r15} 
          @.END
 

