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
        .section  .text
        .global   Pred_lt_3or6_asm 
	.extern   inter_61

Pred_lt_3or6_asm:

        STMFD          r13!, {r4 - r12, r14} 
	RSB            r1, r1, #0                 @ -T0
	ADD            r3, r0, r1, LSL #1         @ x0 = &exc[-T0]
        MOV            r4, #0                     @ j = 0
	LDR            r5, =0x7303                @ 29443
	LDR            r2, Table                  @ c2 = &inter_61[0]

LOOP:
        ADD            r6, r3, #2                 @ x2 = x0 + 1
        LDRSH          r7, [r3]                   @ x1[0]
        LDRSH          r9, [r6]                   @ x2[0]
        LDRSH          r8, [r3, #-2]              @ x1[-1]
        LDRSH          r10, [r6, #2]              @ x2[1]
        LDRSH          r11, [r3, #-4]             @ x1[-2]
        ADD            r9, r9, r8                 @ x2[0] + x1[-1]
        ADD            r10, r10, r11              @ x2[1] + x1[-2]
        MUL            r12, r7, r5                @ s = x1[0] * 29443
        LDRSH          r7, [r2]                   @ c2[0]
        LDRSH          r8, [r6, #4]               @ x2[2]
        LDRSH          r11, [r3, #-6]             @ x1[-3]
        MLA            r12, r9, r7, r12           @ s += (x2[0] + x1[-1]) * c2[0]
	LDRSH          r7, [r2, #2]               @ c2[1]
        ADD            r8, r8, r11                @ x2[2] + x1[-3]
        MLA            r12, r10, r7, r12          @ s += (x2[1] + x1[-2]) * c2[1]
        LDRSH          r7, [r2, #4]               @ c2[2]
        LDRSH          r9, [r6, #6]               @ x2[3]
        LDRSH          r10,[r3, #-8]              @ x1[-4]
        MLA            r12, r8, r7, r12           @ s += (x2[2] + x1[-3]) * c2[2]
	LDRSH          r7, [r2, #6]               @ c2[3]
	ADD            r9, r9, r10                @ x2[3] + x1[-4]
	LDRSH          r8, [r6, #8]               @ x2[4]
	LDRSH          r11,[r3, #-10]             @ x1[-5]
	MLA            r12, r9, r7, r12           @ s += (x2[3] + x1[-4]) * c2[3]
	LDRSH          r7, [r2, #8]               @ c2[4]
	ADD            r8, r8, r11                @ x2[4] + x1[-5]
	LDRSH          r9, [r6, #10]              @ x2[5]
	LDRSH          r10,[r3, #-12]             @ x1[-6]
        MLA            r12, r7, r8, r12           @ s += (x2[4] + x1[-5]) * c2[4]
	LDRSH          r7, [r2, #10]              @ c2[5]
	ADD            r9, r9, r10                @ x2[5] + x1[-6]
	LDRSH          r8, [r6, #12]              @ x2[6]
	LDRSH          r11,[r3, #-14]             @ x1[-7]
	MLA            r12, r7, r9, r12           @ s += (x2[5] + x1[-6]) * c2[5]
	LDRSH          r7, [r2, #12]              @ c2[6]
	ADD            r8, r8, r11                @ x2[6] + x1[-7]
	LDRSH          r9, [r6, #14]              @ x2[7]
	LDRSH          r10, [r3, #-16]            @ x1[-8]
	MLA            r12, r7, r8, r12           @ s += (x2[6] + x1[-7]) * c2[6]
	LDRSH          r7, [r2, #14]              @ c2[7]
	ADD            r9, r9, r10                @ x2[7] + x1[-8]
	LDRSH          r8, [r6, #16]              @ x2[8]
	LDRSH          r11,[r3, #-18]             @ x1[-9]
	MLA            r12, r7, r9, r12           @ s += (x2[7] + x1[-8]) * c2[7]
	ADD            r8, r8, r11                @ x2[8] + x1[-9]
	LDRSH          r7, [r2, #16]              @ c2[8]
	ADD            r3, r3, #2                 @ x0++
	MOV            r9, #0x8000
	MLA            r12, r7, r8, r12           @ s += (x2[8] + x1[-9]) * c2[8]
        ADD            r4, r4, #1
	ADD            r10, r9, r12, LSL #1
	MOV            r12, r10, ASR #16
	CMP            r4, #40
	STRH           r12, [r0], #2
	BLT            LOOP

Pred_lt_3or6_asm_end: 
 
        LDMFD          r13!, {r4 - r12, r15}  

Table:
        .word          inter_61     
        .END
