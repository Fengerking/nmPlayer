@**************************************************************
@* Copyright 2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void Pred_lt_3or6 (
@		Word16 exc[],     /* in/out: excitation buffer                         */
@		Word16 T0,        /* input : integer pitch lag                         */
@		Word16 frac,      /* input : fraction of lag                           */
@		Word16 L_subfr,   /* input : subframe size                             */
@		Word16 flag3      /* input : if set, upsampling rate = 3 (6 otherwise) */
@	        )
@****************************************
@            ARM  Register 
@****************************************
@ r0 --- exc[]
@ r1 --- T0
@ r2 --- frac
@ r3 --- L_subfr
@ r4 --- flag3
        #include "voAMRNBEncID.h"
        .text   .align 4
        .globl   _Pred_lt_3or6_asm 
	.globl   _inter_61

_Pred_lt_3or6_asm:

        stmfd          r13!, {r4 - r12, r14} 
	rsb            r1, r1, #0                 @ -T0
	add            r3, r0, r1, lsl #1         @ x0 = &exc[-T0]
        mov            r4, #0                     @ j = 0
	ldr            r5, Cvalue3                @ 29443
	ldr            r2, Table                  @ c2 = &inter_61[0]

LOOP:
        add            r6, r3, #2                 @ x2 = x0 + 1
        ldrsh          r7, [r3]                   @ x1[0]
        ldrsh          r9, [r6]                   @ x2[0]
        ldrsh          r8, [r3, #-2]              @ x1[-1]
        ldrsh          r10, [r6, #2]              @ x2[1]
        ldrsh          r11, [r3, #-4]             @ x1[-2]
        add            r9, r9, r8                 @ x2[0] + x1[-1]
        add            r10, r10, r11              @ x2[1] + x1[-2]
        mul            r12, r7, r5                @ s = x1[0] * 29443
        ldrsh          r7, [r2]                   @ c2[0]
        ldrsh          r8, [r6, #4]               @ x2[2]
        ldrsh          r11, [r3, #-6]             @ x1[-3]
        mla            r12, r9, r7, r12           @ s += (x2[0] + x1[-1]) * c2[0]
	ldrsh          r7, [r2, #2]               @ c2[1]
        add            r8, r8, r11                @ x2[2] + x1[-3]
        mla            r12, r10, r7, r12          @ s += (x2[1] + x1[-2]) * c2[1]
        ldrsh          r7, [r2, #4]               @ c2[2]
        ldrsh          r9, [r6, #6]               @ x2[3]
        ldrsh          r10,[r3, #-8]              @ x1[-4]
        mla            r12, r8, r7, r12           @ s += (x2[2] + x1[-3]) * c2[2]
	ldrsh          r7, [r2, #6]               @ c2[3]
	add            r9, r9, r10                @ x2[3] + x1[-4]
	ldrsh          r8, [r6, #8]               @ x2[4]
	ldrsh          r11,[r3, #-10]             @ x1[-5]
	mla            r12, r9, r7, r12           @ s += (x2[3] + x1[-4]) * c2[3]
	ldrsh          r7, [r2, #8]               @ c2[4]
	add            r8, r8, r11                @ x2[4] + x1[-5]
	ldrsh          r9, [r6, #10]              @ x2[5]
	ldrsh          r10,[r3, #-12]             @ x1[-6]
        mla            r12, r7, r8, r12           @ s += (x2[4] + x1[-5]) * c2[4]
	ldrsh          r7, [r2, #10]              @ c2[5]
	add            r9, r9, r10                @ x2[5] + x1[-6]
	ldrsh          r8, [r6, #12]              @ x2[6]
	ldrsh          r11,[r3, #-14]             @ x1[-7]
	mla            r12, r7, r9, r12           @ s += (x2[5] + x1[-6]) * c2[5]
	ldrsh          r7, [r2, #12]              @ c2[6]
	add            r8, r8, r11                @ x2[6] + x1[-7]
	ldrsh          r9, [r6, #14]              @ x2[7]
	ldrsh          r10, [r3, #-16]            @ x1[-8]
	mla            r12, r7, r8, r12           @ s += (x2[6] + x1[-7]) * c2[6]
	ldrsh          r7, [r2, #14]              @ c2[7]
	add            r9, r9, r10                @ x2[7] + x1[-8]
	ldrsh          r8, [r6, #16]              @ x2[8]
	ldrsh          r11,[r3, #-18]             @ x1[-9]
	mla            r12, r7, r9, r12           @ s += (x2[7] + x1[-8]) * c2[7]
	add            r8, r8, r11                @ x2[8] + x1[-9]
	ldrsh          r7, [r2, #16]              @ c2[8]
	add            r3, r3, #2                 @ x0++
	mov            r9, #0x8000
	mla            r12, r7, r8, r12           @ s += (x2[8] + x1[-9]) * c2[8]
        add            r4, r4, #1
	add            r10, r9, r12, lsl #1
	mov            r12, r10, asr #16
	cmp            r4, #40
	strh           r12, [r0], #2
	blt            LOOP

Pred_lt_3or6_asm_end: 
 
        ldmfd          r13!, {r4 - r12, r15}  

Table:
        .word          _inter_61  

Cvalue3:
        .word          0x7303
     
        @.END
