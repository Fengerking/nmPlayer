@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
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
          .text   .align 4
          .globl   _Interpol_asm

_Interpol_asm:

          stmfd         r13!, {r4 - r12, r14} 
          sub           r4, r0, #22                    @ x -= nb_coef
          add           r5, r2, #1                     @ 1 + frac
	  mov           r7, #24
          sub           r6, r3, r5                     @ resol - 1 - frac
          mul           r12, r6, r7                    @ (resol - 1 - frac) * 24

	  cmp           r12, #96
	  beq           Lable
          add           r5, r1, r12, lsl #1             @ get fir[k] address
          @ r4 ---- x  r5 ---- fir[]
          
          vld1.s16      {d0, d1, d2, d3}, [r4]!        @get x[0] ~ x[15]
	  vld1.s16      {d4, d5}, [r4]!                @get x[16] ~ x[23]
	  
	  vld1.s16      {d6, d7, d8, d9}, [r5]!        @get 16 fir[]
	  vld1.s16      {d10, d11}, [r5]!              @get 8 fir[]


	  vqdmull.s16   q10, d0, d6
	  vqdmlal.s16   q10, d1, d7
          vqdmlal.s16   q10, d2, d8
	  vqdmlal.s16   q10, d3, d9
	  vqdmlal.s16   q10, d4, d10
	  vqdmlal.s16   q10, d5, d11

	  vqadd.S32     d20, d20, d21
          vpadd.S32     d20, d20, d20
          mov           r9, #0x00008000  
          vmov.S32      r12, d20[0]
          qadd          r8, r12, r12
	  b             Lable1

Lable: 
          add           r8, r12, #11
	  add           r4, r4, #22                   @x[11] address
          add           r5, r1, r8, lsl #1            @get fir[96 + 11] address
	  ldrsh         r10, [r4]          
	  ldrsh         r11, [r5]
	  mul           r8, r10, r11                  @L_sum = x[11] * fir[k+11]
	  qadd          r10, r8, r8
	  mov           r9, #0x00008000
	  qadd          r8, r10, r10

Lable1:
          qadd          r0, r8, r9
          mov           r0, r0, asr #16 

Interpol_asm_end:
		     
          ldmfd         r13!, {r4 - r12, r15} 
          @ENdFUNC
          @.ENd

