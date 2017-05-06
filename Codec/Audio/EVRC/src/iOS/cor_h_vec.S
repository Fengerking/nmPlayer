@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: 
@*            cor_h_vec.s
@* Description: 
@*            This module implements the cor_h_vec().
@* Functions Included:
@*            1. void cor_h_vec
@*
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    04-07-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@static void cor_h_vec123(
@		         Shortword h[],		/* (i) scaled impulse response */
@		         Shortword vec[],	        /* (i) vector to correlate with h[] */
@		         Shortword track,	        /* (i) track to use */
@		         Shortword sign[],    	/* (i) sign vector */
@		         Shortword rrixix[][NB_POS],/* (i) correlation of h[x] with h[x] */
@		         Shortword corx[],           /* (o) result of correlation (NB_POS elements) */
@                        Shortword cory[]
@)
@********************
@ ARM Register
@********************
@r0   ---  h[]
@r1   ---  vec[]
@r2   ---  track
@r3   ---  sign[]
@r4   ---  rrixix[][NB_POS]
@r5   ---  corx[]
@r6   ---  cory[]
#include "voEvrcDecID.h"
        .text
        .align 4
	.globl   _cor_h_vec123

_cor_h_vec123:
        
        stmfd       sp!, {r4-r12,lr}
        mov         r5, #22
        mov         r10, #0                     @i = 0
        ldr         r4, [sp, #40]               @get rrixix[][NB_POS]
        mul         r8, r2, r5                 
        add         r7, r4, r8                  @p0 = rrixix[track]
        add         r8, r7, #22                 @p3 = rrixix[track + 1]

LOOP1:
        mov         r4, #0                      @s  = 0
        mov         r9, #0                      @s1 = 0
        mov         r11, r0                     @p1 = h
        add         r12, r1, r2, LSL #1         @p2 = &vec[pos]
        rsb         r14, r2, #54                @j = L_SUBFR - pos - 1

LOOP2:
        ldrsh       r5, [r11], #2               @*p1
        ldrsh       r6, [r12], #2               @*p2++
        mla         r4, r5, r6, r4              @s += (*p1) * (*p2++)
        ldrsh       r6, [r12]                   @*p2
        subs        r14, r14, #1
        mla         r9, r5, r6, r9              @s1 += (*p1++) * (*p2)
        bgt         LOOP2


        ldrsh       r5, [r11]                   @*p1
        ldrsh       r6, [r12]                   @*p2
        mov         r9, r9, LSL #1              @s1 <<=1
        mla         r4, r5, r6, r4              @s += (*p1) * (*p2)
        add         r9, r9, #0x8000
        add         r14, r3, r2, LSL #1         @sign[pos]
        mov         r4, r4, LSL #1              @s <<=1
        add         r4, r4, #0x8000
        mov         r9, r9, ASR #16             @s1_temp = (s1 + 0x8000) >> 16

        ldrsh       r11, [r14], #2              @sign[pos]
        mov         r4, r4, ASR #16             @s_temp = (s + 0x8000) >> 16
        ldrsh       r12, [r14]                  @sign[pos + 1]
        ldrsh       r5, [r7], #2                @*p0++
        mul         r14, r4, r11
        mul         r11, r9, r12
        ldrsh       r6, [r8], #2                @*p3++
        mov         r14, r14, ASR #15
        add         r14, r14, r5
        mov         r11, r11, ASR #15
        ldr         r5, [sp, #44]               @get corx[] 
        add         r11, r11, r6
        ldr         r6, [sp, #48]               @get cory[]    
        add         r4, r5, r10, LSL #1
        add         r2, r2, #5
        add         r9, r6, r10, LSL #1
        strh        r14, [r4]
        strh        r11, [r9]
        add         r10, r10, #1
        cmp         r10, #11
        blt         LOOP1  
        
        
        ldmfd       sp!, {r4 - r12,pc} 
        @.END

