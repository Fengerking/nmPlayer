@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 
@void Syn_filt(
@     Word16 a[],                           /* (i) q12 : a[m+1] prediction coefficients           */
@     Word16 x[],                           /* (i)     : input signal                             */
@     Word16 y[],                           /* (o)     : output signal                            */
@     Word16 mem[],                         /* (i/o)   : memory associated with this filtering.   */
@)
@****************************************************************
@ a[]    ---   r0
@ x[]    ---   r1
@ y[]    ---   r2
@ mem[]  ---   r3
@ m ---  16  lg --- 80  update --- 1
          #include "voAMRWBDecID.h"
          @PRESERVE8
          .text   .align 4
	  .globl   _Syn_filt_asm

_Syn_filt_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          sub           r13, r13, #700                   @ y_buf[L_FRAME16k + M16k]
   
          mov           r4, r3                           @ copy mem[] address
          mov           r5, r13                          @ copy yy = y_buf address

          @ for(i = 0@ i < m@ i++)
          @{
          @    *yy++ = mem[i]@
          @} 
          vld1.S16      {d0, d1, d2, d3}, [r4]!          @load 16 mems
	  vst1.S16      {d0, d1, d2, d3}, [r5]!          @store 16 mem[] to *yy

          ldrsh         r5, [r0], #2                     @ load a[0]
          mov           r8, #0                           @ i = 0
          mov           r5, r5, asr #1                   @ a0 = a[0] >> 1
          vmov.S16      d8[0], r5
          @ load all a[]
          vld1.S16      {d0, d1, d2, d3}, [r0]!          @ load a[1] ~ a[16]
	  vrev64.16     d0, d0
	  vrev64.16     d1, d1
	  vrev64.16     d2, d2
	  vrev64.16     d3, d3 
	  mov           r8, #0                           @ loop times
	  mov           r10, r13                         @ temp = y_buf
	  add           r4, r13, #32                     @ yy[i] address

          vld1.S16      {d4, d5, d6, d7}, [r10]!         @ first 16 temp_p

SYN_LOOP:  
          ldrsh         r6, [r1], #2                     @ load x[i]
	  mul           r12, r6, r5                      @ L_tmp = x[i] * a0
	  add           r10, r4, r8, lsl #1              @ y[i], yy[i] address

	  vdup.S32      q10, r12
	  vmull.S16     q5, d3, d4                    
          vmlal.S16     q5, d2, d5
          vmlal.S16     q5, d1, d6
          vmlal.S16     q5, d0, d7
          vext.8        d4, d4, d5, #2
          vext.8        d5, d5, d6, #2
          vext.8        d6, d6, d7, #2
          vpadd.S32     d12, d10, d11
          add           r8, r8, #1
          vpadd.S32     d10, d12, d12

	  vdup.S32      q7, d10[0]

	  vsub.S32      q9, q10, q7
          vqrshrn.S32   d20, q9, #12   
          vmov.S16      r9, d20[0]
          vext.8        d7, d7, d20, #2
          cmp           r8, #80
          strh          r9, [r10]                        @ yy[i]
          strh          r9, [r2], #2                     @ y[i]          	         
	  
          blt           SYN_LOOP
 
          @ update mem[]
          add           r5, r13, #160                    @ yy[64] address
	  vld1.S16      {d0, d1, d2, d3}, [r5]!
	  vst1.S16      {d0, d1, d2, d3}, [r3]!              

Syn_filt_asm_end:
 
          add           r13, r13, #700		     
          ldmfd   	r13!, {r4 - r12, r15} 
          @ENdFUNC
          @.ENd
 

