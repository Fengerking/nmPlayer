@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@static Word16 Interpol(                    /* return result of interpolation */
@     Word16 * x,                           /* input vector                   */
@     Word16 * fir,                         /* filter coefficient             */
@     Word16 frac,                          /* fraction (0..resol)            */
@     Word16 resol,                         /* resolution                     */
@     Word16 nb_coef                        /* number of coefficients         */
@)
@****************************************************************
@ *x      ---   r0
@ *fir    ---   r1
@ frac    ---   r2
@ resol   ---   r3
@ nb_coef ---   r4            @nb_coef = 12
           #include "voAMRWBDecID.h"
          .text    .align 4
          .globl   _Interpol_asm

_Interpol_asm:

          stmfd    	r13!, {r4 - r12, r14} 
          sub           r4, r0, #22                    @ x -= nb_coef
          add           r5, r2, #1                     @ 1 + frac
          sub           r6, r3, r5                     @ resol - 1 - frac
          mov           r7, #24 
	  mul           r12, r6, r7                
          add           r5, r1, r12, LSL #1             @ get fir[k] address
          
          @ r4 ---- x  r5 ---- fir[]
          ldrsh         r7,  [r4], #2                  @ get x[0]
          ldrsh         r8,  [r5], #2                  @ fir[k]
          ldrsh         r9,  [r4], #2                  @ get x[1]
          ldrsh         r10, [r5], #2                  @ fir[k]
          mul           r14, r7, r8       
          ldrsh         r7,  [r4], #2                  @ load x[2]
          ldrsh         r8,  [r5], #2                 
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[3]
          ldrsh         r10, [r5], #2 
          mla           r14, r7, r8, r14
          ldrsh         r7,  [r4], #2                  @ load x[4]
          ldrsh         r8,  [r5], #2    
          mla           r14, r9, r10, r14      
          ldrsh         r9,  [r4], #2                  @ load x[5]
          ldrsh         r10, [r5], #2             
          mla           r14, r7, r8, r14
          ldrsh         r7,  [r4], #2                  @ load x[6]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[7]
          ldrsh         r10, [r5], #2
          mla           r14, r7, r8, r14             
          ldrsh         r7,  [r4], #2                  @ load x[8]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[9]
          ldrsh         r10, [r5],#2
          mla           r14, r7, r8, r14 
          ldrsh         r7,  [r4], #2                  @ load x[10]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[11]
          ldrsh         r10, [r5], #2 
          mla           r14, r7, r8, r14
          ldrsh         r7,  [r4], #2                  @ get x[0]
          mla           r14, r9, r10, r14

          ldrsh         r8,  [r5], #2                  @ fir[k]
          ldrsh         r9,  [r4], #2                  @ get x[1]
          ldrsh         r10, [r5], #2                  @ fir[k]
          mla           r14, r7, r8, r14       
          ldrsh         r7,  [r4], #2                  @ load x[2]
          ldrsh         r8,  [r5], #2                 
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[3]
          ldrsh         r10, [r5], #2 
          mla           r14, r7, r8, r14
          ldrsh         r7,  [r4], #2                  @ load x[4]
          ldrsh         r8,  [r5], #2    
          mla           r14, r9, r10, r14      
          ldrsh         r9,  [r4], #2                  @ load x[5]
          ldrsh         r10, [r5], #2             
          mla           r14, r7, r8, r14
          ldrsh         r7,  [r4], #2                  @ load x[6]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[7]
          ldrsh         r10, [r5], #2
          mla           r14, r7, r8, r14             
          ldrsh         r7,  [r4], #2                  @ load x[8]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[9]
          ldrsh         r10, [r5],#2
          mla           r14, r7, r8, r14 
          ldrsh         r7,  [r4], #2                  @ load x[10]
          ldrsh         r8,  [r5], #2
          mla           r14, r9, r10, r14
          ldrsh         r9,  [r4], #2                  @ load x[11]
          ldrsh         r10, [r5], #2 
          mla           r14, r7, r8, r14
          mov           r8, #0x7fffffff
          mla           r14, r9, r10, r14
          mov           r9, #0x00008000
          
          @ L_sum --- r14
          mov           r7, r14
          mov           r14, r7, lsl #2
          teq           r7, r14, asr #2
          eorne         r14, r8, r7, asr #31
          qadd          r0, r14, r9
          mov           r0, r0, asr #16 

Interpol_asm_end:
		     
          ldmfd   	r13!, {r4 - r12, r15} 
          @ENDFUNC
          @.END



