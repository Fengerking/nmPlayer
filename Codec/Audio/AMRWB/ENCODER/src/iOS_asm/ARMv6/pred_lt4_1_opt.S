@**************************************************************
@* Copyright 2003~2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@***************************************************************
@void Pred_lt4(
@		  Word16 exc[],                         /* in/out: excitation buffer */
@		  Word16 T0,                            /* input : integer pitch lag */
@		  Word16 frac,                          /* input : fraction of lag   */
@		  Word16 L_subfr                        /* input : subframe size     */
@	      )
@******************************
@       ARM Register
@******************************
@ r0  ---  exc[]
@ r1  ---  T0
@ r2  ---  frac
@ r3  ---  L_subfr
          #include "voAMRWBEncID.h"
         .text    .align 4
	 .globl   _pred_lt4_asm
	 .globl   _voAMRWBEncinter4_2

_pred_lt4_asm:

         stmfd     r13!, {r4 - r12, r14}
   
         rsb       r4, r1, #0                         @-T0
         rsb       r2, r2, #0                         @frac = -frac
         add       r5, r0, r4, lsl #1                 @x = exc - T0
         cmp       r2, #0
         addlt     r2, r2, #4                         @frac += UP_SAMP
         sublt     r5, r5, #2                         @x--
         sub       r5, r5, #30                        @x -= 15
         rsb       r4, r2, #3                         @k = 3 - frac
         ldr       r6, Table
	 mov       r8, r4, lsl #6                    
         mov       r1, r5
	 mov       r14, #21
         add       r8, r6, r8                         @ptr2 = &(inter4_2[k][0])

@ used register
         @r0 --- exc[]  r1 --- x  r7 --- j  r8 --- ptr2  r5 --- 0x8000
THREE_LOOP:

         @mov       r1, r5                             @ptr1 = x
	 mov       r2, r8                             @ptr = ptr2
	 ldrsh     r4, [r1], #2                       @x[0]
	 mov       r10, #0
	 ldrsh     r6, [r1], #2                       @x[1]
         ldr       r3, [r2], #4                       @h[0], h[1]
	 ldrsh     r9, [r1], #2                       @x[2]

	 mov       r11, #0
	 mov       r12, #0

	 pkhbt     r5, r4, r6, lsl #16
         ldrsh     r4, [r1], #2                       @x[3]
	 pkhbt     r7, r6, r9, lsl #16

	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r6, [r1], #2                       @x[4]
	 smlad     r12, r5, r3, r12

	 ldr       r3, [r2], #4                       @h[2], h[3]
	 pkhbt     r5, r9, r4, lsl #16
         ldrsh     r9, [r1], #2                       @x[5] 
	 pkhbt     r7, r4, r6, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r6, r9, lsl #16
         ldrsh     r4, [r1], #2                       @x[6]
	 smlad     r12, r5, r3, r12

         ldr       r3, [r2], #4                       @h[4], h[5]
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[7]
	 pkhbt     r7, r9, r4, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r9, [r1], #2                       @x[8]
	 smlad     r12, r5, r3, r12


         ldr       r3, [r2], #4                       @h[6], h[7]
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[9]
	 pkhbt     r7, r6, r9, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r6, [r1], #2                       @x[10]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[8], h[9]
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[11]
	 pkhbt     r7, r4, r6, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r4, [r1], #2                       @x[12]
	 smlad     r12, r5, r3, r12


         ldr       r3, [r2], #4                       @h[10], h[11]
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[13]
	 pkhbt     r7, r9, r4, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r9, [r1], #2                       @x[14]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[12], h[13]
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[15]
	 pkhbt     r7, r6, r9, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r6, [r1], #2                       @x[16]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[14], h[15]
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[17]
	 pkhbt     r7, r4, r6, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r4, [r1], #2                       @x[18]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[16], h[17]
	 pkhbt     r5, r6, r9, lsl #16
         ldrsh     r6, [r1], #2                       @x[19]
	 pkhbt     r7, r9, r4, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r4, r6, lsl #16
         ldrsh     r9, [r1], #2                       @x[20]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[18], h[19]
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[21]
	 pkhbt     r7, r6, r9, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r6, [r1], #2                       @x[22]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[20], h[21]
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[23]
	 pkhbt     r7, r4, r6, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r4, [r1], #2                       @x[24]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[22], h[23]
	 pkhbt     r5, r6, r9, lsl #16
         ldrsh     r6, [r1], #2                       @x[25]
	 pkhbt     r7, r9, r4, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r4, r6, lsl #16
         ldrsh     r9, [r1], #2                       @x[26]	 
	 smlad     r12, r5, r3, r12



	 ldr       r3, [r2], #4                       @h[24], h[25]
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[27]
	 pkhbt     r7, r6, r9, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r6, [r1], #2                       @x[28]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[26], h[27]
	 pkhbt     r5, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[29]
	 pkhbt     r7, r4, r6, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r6, r9, lsl #16
	 ldrsh     r4, [r1], #2                       @x[30]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[28], h[29]
	 pkhbt     r5, r6, r9, lsl #16
         ldrsh     r6, [r1], #2                       @x[31]
	 pkhbt     r7, r9, r4, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r5, r4, r6, lsl #16
         ldrsh     r9, [r1], #2                       @x[32]
	 smlad     r12, r5, r3, r12


	 ldr       r3, [r2], #4                       @h[30], h[31]
	 pkhbt     r5, r4, r6, lsl #16
	 ldrsh     r4, [r1], #-60                     @x[33]
	 pkhbt     r7, r6, r9, lsl #16
	 smlad     r10, r5, r3, r10
	 smlad     r11, r7, r3, r11
	 pkhbt     r9, r9, r4, lsl #16
	 mov       r10, r10, lsl #1

	 smlad     r12, r9, r3, r12

	 @SSAT      r10, #32, r10, lsl #2
	 @SSAT      r11, #32, r11, lsl #2
	 @SSAT      r12, #32, r12, lsl #2

	 mov       r11, r11, lsl #1
	 mov       r12, r12, lsl #1
	 qadd      r10, r10, r10
	 mov       r5, #0x8000
	 qadd      r11, r11, r11
	 subs      r14, r14, #1
	 qadd      r12, r12, r12

	 qadd      r10, r10, r5
	 qadd      r11, r11, r5
	 mov       r10, r10, asr #16
	 qadd      r12, r12, r5


	 mov       r11, r11, asr #16
	 mov       r12, r12, asr #16

	 strh      r10, [r0], #2
	 strh      r11, [r0], #2
	 strh      r12, [r0], #2
	 bne       THREE_LOOP

	 mov       r2, r8                             @ptr = ptr2

Last2LOOP:

	 ldrsh     r4, [r1], #2                       @x[0]
         ldr       r3, [r2], #4                       @h[0], h[1]
	 ldrsh     r6, [r1], #2                       @x[1]
	 mov       r10, #0
	 ldrsh     r9, [r1], #2                       @x[2]

	 mov       r11, #0

	 pkhbt     r7, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[3]
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r6, r3, r11

         ldrsh     r6, [r1], #2                       @x[4]
	 ldr       r3, [r2], #4                       @h[2], h[3]
	 pkhbt     r7, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[5]
	 pkhbt     r4, r4, r6, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r4, r3, r11 

	 ldrsh     r4, [r1], #2                       @x[6]
	 ldr       r3, [r2], #4                       @h[4], h[5]
	 pkhbt     r7, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[7]
	 pkhbt     r9, r9, r4, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r9, r3, r11

	 ldrsh     r9, [r1], #2                       @x[8]
	 ldr       r3, [r2], #4                       @h[6], h[7]
	 pkhbt     r7, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[9]
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r6, r3, r11

	 ldrsh     r6, [r1], #2                       @x[10]
	 ldr       r3, [r2], #4                       @h[8], h[9]
	 pkhbt     r7, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[11]
	 pkhbt     r4, r4, r6, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r4, r3, r11

	 ldrsh     r4, [r1], #2                       @x[12]
	 ldr       r3, [r2], #4                       @h[10], h[11]
	 pkhbt     r7, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[13]
	 pkhbt     r9, r9, r4, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r9, r3, r11

	 ldrsh     r9, [r1], #2                       @x[14]
	 ldr       r3, [r2], #4                       @h[12], h[13]
	 pkhbt     r7, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[15]
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r6, r3, r11

	 ldrsh     r6, [r1], #2                       @x[16]
	 ldr       r3, [r2], #4                       @h[14], h[15]
	 pkhbt     r7, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[17]
	 pkhbt     r4, r4, r6, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r4, r3, r11

	 ldrsh     r4, [r1], #2                       @x[18]
	 ldr       r3, [r2], #4                       @h[16], h[17]
	 pkhbt     r7, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[19]
	 pkhbt     r9, r9, r4, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r9, r3, r11

	 ldrsh     r9, [r1], #2                       @x[20]
	 ldr       r3, [r2], #4                       @h[18], h[19]
	 pkhbt     r7, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[21]
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r6, r3, r11

	 ldrsh     r6, [r1], #2                       @x[22]
	 ldr       r3, [r2], #4                       @h[20], h[21]
	 pkhbt     r7, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[23]
	 pkhbt     r4, r4, r6, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r4, r3, r11

	 ldrsh     r4, [r1], #2                       @x[24]
	 ldr       r3, [r2], #4                       @h[22], h[23]
	 pkhbt     r7, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[25]
	 pkhbt     r9, r9, r4, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r9, r3, r11

	 ldrsh     r9, [r1], #2                       @x[26]
	 ldr       r3, [r2], #4                       @h[24], h[25]
	 pkhbt     r7, r4, r6, lsl #16
	 ldrsh     r4, [r1], #2                       @x[27]
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r6, r3, r11

	 ldrsh     r6, [r1], #2                       @x[28]
	 ldr       r3, [r2], #4                       @h[26], h[27]
	 pkhbt     r7, r9, r4, lsl #16
	 ldrsh     r9, [r1], #2                       @x[29]
	 pkhbt     r4, r4, r6, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r4, r3, r11

	 ldrsh     r4, [r1], #2                       @x[30]
	 ldr       r3, [r2], #4                       @h[28], h[29]
	 pkhbt     r7, r6, r9, lsl #16
	 ldrsh     r6, [r1], #2                       @x[31]
	 pkhbt     r9, r9, r4, lsl #16
	 smlad     r10, r7, r3, r10
	 smlad     r11, r9, r3, r11

	 ldrsh     r9, [r1], #2                       @x[32]
	 ldr       r3, [r2], #4                       @h[30], h[31]
	 pkhbt     r4, r4, r6, lsl #16
	 mov       r5, #0x8000
	 pkhbt     r6, r6, r9, lsl #16
	 smlad     r10, r4, r3, r10
	 smlad     r11, r6, r3, r11


	 @SSAT      r10, #32, r10, lsl #2
	 @SSAT      r11, #32, r11, lsl #2
	 mov       r10, r10, lsl #1
	 mov       r11, r11, lsl #1

	 qadd      r10, r10, r10
	 qadd      r11, r11, r11 

	 qadd      r10, r10, r5
	 qadd      r11, r11, r5

	 mov       r10, r10, asr #16
	 mov       r11, r11, asr #16

	 strh      r10, [r0], #2
	 strh      r11, [r0], #2


pred_lt4_end:

         ldmfd     r13!, {r4 - r12, r15}

Table:
         .word       _voAMRWBEncinter4_2
	 @.END



