;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************


        AREA    |.text|, CODE, READONLY
        EXPORT Vo_Residu_asm

Vo_Residu_asm PROC
        stmdb   r13!, {r4-r12, r14} 
        mov     r3, #40  
L22                          
        mov      r4,r1
        ldrsh    r12,[r4],#-2
        ldrsh    r5,[r0,#0]
	subs     r3,r3,#1

        ldrsh    r7,[r4],#-2
        ldrsh    r6,[r0,#2]
        mul      r5,r12,r5

        ldrsh    r8,[r0,#4]
        ldrsh    r9,[r4],#-2
        mla      r5,r6,r7,r5

        ldrsh    r10,[r0,#6]
        ldrsh    r11,[r4],#-2
        mla      r5,r8,r9,r5

        ldrsh    r6,[r0,#8]
        ldrsh    r7,[r4],#-2
	mla      r5,r10,r11,r5

        ldrsh    r8,[r0,#0xa]
        ldrsh    r9,[r4],#-2
        mla      r5,r6,r7,r5

        ldrsh    r10,[r0,#0xc]
        ldrsh    r11,[r4],#-2
        mla      r5,r8,r9,r5

        ldrsh    r6,[r0,#0xe]
        ldrsh    r7,[r4],#-2

        mla      r5,r10,r11,r5
        ldrsh    r8,[r0,#0x10]
        ldrsh    r9,[r4],#-2
        mla      r5,r6,r7,r5

        ldrsh    r10,[r4],#-2
        ldrsh    r11,[r0,#0x12]
        ldrsh    r4,[r4,#0]
        mla      r5,r8,r9,r5

        ldrsh    r12,[r0,#0x14]
        add      r1,r1,#2
	mla      r5,r10,r11,r5
        mla      r12,r12,r4,r5

        add      r12,r12,#0x800
        mov      r12,r12,ASR #12
        strh     r12,[r2],#2
        bgt      L22                             
L23                          
        ldmfd   r13!, {r4-r12, r15}

        ENDP
        END 
