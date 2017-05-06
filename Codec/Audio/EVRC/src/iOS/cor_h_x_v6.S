@**************************************************************
@* Copyright 2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@void cor_h_x (
@	      Word16 h[],    /* (i): impulse response of weighted synthesis filter */
@	      Word16 x[],    /* (i): target                                        */
@	      Word16 dn[],   /* (o): correlation between target and h[]            */
@	      )
@******************
@   ARM Register
@******************
@ r0 --- h[]
@ r1 --- x[]
@ r2 --- dn[]
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl   _cor_h_x2_asm

_cor_h_x2_asm:

        stmfd          r13!, {r4 - r12, r14} 
        mov            r4, #5                             @ tot = 5
        sub            r13, r13, #280                     @ y32[L_CODE]
	mov            r5, #4                             @ k = NB_TRACK - 1
	str            r2, [r13, #4]                      @ push dn[]
	add            r2, r13, #12                       @ y32[] addr

LOOPK:
	mov            r7, r5                             @ i = k
        mov            r6, #0                             @ max = 0

LOOPi:
        mov            r9, r7                             @ j = i
	add            r3, r2, r7, LSL #2                 @ y32[i] addr
        mov            r10, r0
        add            r11, r1, r9, LSL #1                @ x[j] addr
        mov            r8, #0                             @ s = 0

LOOPj:
        ldrsh          r12, [r11], #2                     @ load x[j]
        ldrsh          r14, [r10], #2                     @ load h[j - i]
	cmp            r12, #0
	beq            Lable
	mla            r8, r12, r14, r8

Lable:
	add            r9, r9, #1
	cmp            r9, #55
	blt            LOOPj
        add            r12, r8, r8                        @ s <<= 1@
	str            r12, [r3]
        cmp            r12, #0
	rsblt          r12, r12, #0                        @ s = (s < 0) ? -s: s
	cmp            r12, r6
	movgt          r6, r12                             @ if(s > max) max = s
	add            r7, r7, #5
	cmp            r7, #55
	blt            LOOPi

        add            r4, r4, r6, ASR #1                 @ tot += (max >> 1)
        
        subs           r5, r5, #1
	bge            LOOPK
        @ r4 --- tot

	ldr            r5, [r13, #4]                      @ pull dn[]
        clz            r6, r4
        mov            r7, #0                             @ i = 0
        sub            r6, r6, #3                         @ j = norm_l(tot) - sf

	@ r2 --- y32[], r7 --- i, r6 --- j, r5 --- dn[]
	mov            r12, #0x8000
        cmp            r6, #0
        bge            LOOPL2
        rsb            r6, r6, #0    

LOOPL1:	
        ldr            r8,  [r2], #4
        ldr            r9,  [r2], #4
        ldr            r10, [r2], #4
        ldr            r11, [r2], #4
	ldr            r3,  [r2], #4

        add            r8, r12, r8, ASR r6
        add            r9, r12, r9, ASR r6	
        add            r10, r12, r10, ASR r6
	add            r11, r12, r11, ASR r6
	add            r3,  r12, r3,  ASR r6

	mov            r8, r8, ASR #16
	mov            r9, r9, ASR #16
	mov            r10, r10, ASR #16
	mov            r11, r11, ASR #16
	mov            r3, r3, ASR #16

	strh           r8,  [r5], #2
	strh           r9,  [r5], #2
	strh           r10, [r5], #2
        strh           r11, [r5], #2
	strh           r3,  [r5], #2

	add            r7, r7, #5
	cmp            r7, #55
	blt            LOOPL1
	bl             cor_h_x2_asm_end 

LOOPL2:
        ldr            r8,  [r2], #4
        ldr            r9,  [r2], #4
        ldr            r10, [r2], #4
        ldr            r11, [r2], #4
	ldr            r3,  [r2], #4

        add            r8, r12, r8, LSL r6
        add            r9, r12, r9, LSL r6	
        add            r10, r12, r10, LSL r6
	add            r11, r12, r11, LSL r6
	add            r3,  r12, r3, LSL r6

	mov            r8, r8, ASR #16
	mov            r9, r9, ASR #16
	mov            r10, r10, ASR #16
	mov            r11, r11, ASR #16
	mov            r3, r3, ASR #16

	strh           r8,  [r5], #2
	strh           r9,  [r5], #2
	strh           r10, [r5], #2
        strh           r11, [r5], #2
	strh           r3, [r5], #2

	add            r7, r7, #5
	cmp            r7, #55
	blt            LOOPL2
	
cor_h_x2_asm_end:

        add            r13, r13, #280    
        ldmfd          r13!, {r4 - r12, r15}  

	@ENDFUNC
        @.END
