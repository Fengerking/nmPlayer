@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            bl_intrp.s
@* Description: 
@*            This module implements the bl_intrp().
@* Functions Included:
@*            1. void bl_intrp
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-14-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@void bl_intrp(
@	        short *output,   	/* int ptr */
@	        short *input,	        /* int ptr */
@		long delay,	        /* sign bit + 7bits int  + 24bits frac */
@		short factor,	        /* fractional value */
@		short fl		/* int value */
@)
@********************
@ ARM Register
@********************
@r0   ---  *output
@r1   ---  *input
@r2   ---  delay
@r3   ---  factor
@r4   ---  f1
#include "voEvrcDecID.h"

        .section   .text
	.global bl_intrp_asm
	@.extern Table1
	@.extern Table

bl_intrp_asm:

        STMFD       sp!, {r4-r12,lr}
        LDR         r4, [sp, #40]               @get f1
        MOV         r7, r2, ASR #23             @delay >> 23
        MOV         r6, #0x4000                 @loutput = 0x4000
        ADD         r8, r7, #0x01               @(delay>>23) + 0x01
        LDR         r5, =0x7333                 @factor2 = 29491
        MOV         r9, r8, ASR #1              @offset
        MOV         r7, r9, LSL #24             @offset << 24
        SUB         r8, r7, r2                  @(offset << 24) - delay
        MOV         r7, r8, ASR #15             @((offset << 24) - delay)>>15
        ADD         r8, r7, #288                
        MOV         r7, r8, ASR #6              @t = ((((offset << 24) - delay) >> 15) + 288) >> 6

        CMP         r7, #8
        MOVEQ       r7, #0                      @t = 0
        SUBEQ       r9, r9, #1                  @offset--
        ADD         r8, r9, r4                  @offset + f1
        ADD         r10, r4, r4
        ADD         r10, r10, #1
        MUL         r11, r7, r10                @t = (t * ((f1 << 1) + 1))
        SUB         r10, r1, r8, LSL #1         @f = input - offset - f1
        ADR         r7, Table1Copy                    @get Table1 address
        ADR         r12, TableCopy                    @get Table address
        CMP         r3, r5
        ADDEQ       r12, r7, r11, LSL #1        @coef_ptr = Table1 + t
        ADDNE       r12, r12, r11, LSL #1       @coef_ptr = Table + t
        LDRSH       r5, [r12], #2               
        LDRSH       r7, [r10], #2
        MLA         r6, r5, r7, r6

LOOP:        
        LDRSH       r8, [r12], #2
        LDRSH       r9, [r10], #2

        LDRSH       r5, [r12], #2
        LDRSH       r7, [r10], #2
        MLA         r6, r8, r9, r6
        MLA         r6, r5, r7, r6
        SUBS        r4, r4, #1
        BGT         LOOP
        MOV         r8, r6, ASR #15
        STRH        r8, [r0]
        
        LDMFD       sp!, {r4 - r12,pc} 

Table1Copy:
	.hword	109,-146,99,180,-901,2411,-5769,20443,20443,-5769,2411,-901,180,99,-146,109,-87
	.hword	59,-34,-114,525,-1363,2887,-5861,16168,24161,-4850,1533,-249,-241,336,-258,153,-100
	.hword	10,62,-282,761,-1605,2957,-5257,11650,27043,-3039,309,529,-692,565,-355,184,-101
	.hword	-33,135,-390,873,-1623,2667,-4135,7204,28867,-350,-1153,1346,-1114,752,-419,195,-90
	.hword	-67,179,-434,864,-1444,2096,-2697,3123,29491,3123,-2697,2096,-1444,864,-434,179,-67
	.hword	-90,195,-419,752,-1114,1346,-1153,-350,28867,7204,-4135,2667,-1623,873,-390,135,-33
	.hword	-101,184,-355,565,-692,529,309,-3039,27043,11650,-5257,2957,-1605,761,-282,62,10
	.hword	-100,153,-258,336,-241,-249,1533,-4850,24161,16168,-5861,2887,-1363,525,-114,-34,59
TableCopy:
	.hword	-747,3158,14079,14079,3158,-747,-169,-709
	.hword	2109,12898,15054,4364,-720,-213,-632,1232
	.hword	11559,15782,5702,-604,-271,-536,530,10116
	.hword	16232,7136,-372,-346,-437,0,8624,16384
	.hword	8624,0,-437,-346,-372,7136,16232,10116
	.hword	530,-536,-271,-604,5702,15782,11559,1232
	.hword	-632,-213,-720,4364,15054,12898,2109,-709
		
        .END

