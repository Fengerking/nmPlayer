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
          .section  .text
          .global   Interpol_asm

Interpol_asm:

          STMFD   	    r13!, {r4 - r12, r14} 
          SUB           r4, r0, #22                    @ x -= nb_coef
          ADD           r5, r2, #1                     @ 1 + frac
          SUB           r6, r3, r5                     @ resol - 1 - frac
          MOV           r7, #24 
	  MUL           r12, r6, r7                
          ADD           r5, r1, r12, LSL #1             @ get fir[k] address
          
          @ r4 ---- x  r5 ---- fir[]
          LDRSH         r7,  [r4], #2                  @ get x[0]
          LDRSH         r8,  [r5], #2                  @ fir[k]
          LDRSH         r9,  [r4], #2                  @ get x[1]
          LDRSH         r10, [r5], #2                  @ fir[k]
          MUL           r14, r7, r8       
          LDRSH         r7,  [r4], #2                  @ load x[2]
          LDRSH         r8,  [r5], #2                 
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[3]
          LDRSH         r10, [r5], #2 
          MLA           r14, r7, r8, r14
          LDRSH         r7,  [r4], #2                  @ load x[4]
          LDRSH         r8,  [r5], #2    
          MLA           r14, r9, r10, r14      
          LDRSH         r9,  [r4], #2                  @ load x[5]
          LDRSH         r10, [r5], #2             
          MLA           r14, r7, r8, r14
          LDRSH         r7,  [r4], #2                  @ load x[6]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[7]
          LDRSH         r10, [r5], #2
          MLA           r14, r7, r8, r14             
          LDRSH         r7,  [r4], #2                  @ load x[8]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[9]
          LDRSH         r10, [r5],#2
          MLA           r14, r7, r8, r14 
          LDRSH         r7,  [r4], #2                  @ load x[10]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[11]
          LDRSH         r10, [r5], #2 
          MLA           r14, r7, r8, r14
          LDRSH         r7,  [r4], #2                  @ get x[0]
          MLA           r14, r9, r10, r14

          LDRSH         r8,  [r5], #2                  @ fir[k]
          LDRSH         r9,  [r4], #2                  @ get x[1]
          LDRSH         r10, [r5], #2                  @ fir[k]
          MLA           r14, r7, r8, r14       
          LDRSH         r7,  [r4], #2                  @ load x[2]
          LDRSH         r8,  [r5], #2                 
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[3]
          LDRSH         r10, [r5], #2 
          MLA           r14, r7, r8, r14
          LDRSH         r7,  [r4], #2                  @ load x[4]
          LDRSH         r8,  [r5], #2    
          MLA           r14, r9, r10, r14      
          LDRSH         r9,  [r4], #2                  @ load x[5]
          LDRSH         r10, [r5], #2             
          MLA           r14, r7, r8, r14
          LDRSH         r7,  [r4], #2                  @ load x[6]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[7]
          LDRSH         r10, [r5], #2
          MLA           r14, r7, r8, r14             
          LDRSH         r7,  [r4], #2                  @ load x[8]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[9]
          LDRSH         r10, [r5],#2
          MLA           r14, r7, r8, r14 
          LDRSH         r7,  [r4], #2                  @ load x[10]
          LDRSH         r8,  [r5], #2
          MLA           r14, r9, r10, r14
          LDRSH         r9,  [r4], #2                  @ load x[11]
          LDRSH         r10, [r5], #2 
          MLA           r14, r7, r8, r14
          MOV           r8, #0x7fffffff
          MLA           r14, r9, r10, r14
          MOV           r9, #0x00008000
          
          @ L_sum --- r14
          MOV           r7, r14
          MOV           r14, r7, LSL #2
          TEQ           r7, r14, ASR #2
          EORNE         r14, r8, r7, ASR #31
          QADD          r0, r14, r9
          MOV           r0, r0, ASR #16 

Interpol_asm_end:
		     
          LDMFD   	    r13!, {r4 - r12, r15} 
          @ENDFUNC
          .END



