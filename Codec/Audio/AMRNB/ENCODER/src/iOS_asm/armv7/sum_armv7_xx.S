@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@ Word32 sum_armv7_xx(s, s_excf[], s_excf[], Length)
@ {
@   for (j = 0@ j < L_subfr@ j++) {
@	s = L_mac (s, s_excf[j], s_excf[j])@
@ }

        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _sum_armv7_xx


_sum_armv7_xx:
        stmfd          r13!, {r1 - r12, r14}  
        vmov.s32       q10, #0

LOOP:
        vld1.s16       {d0, d1, d2, d3}, [r1]!
        vld1.s16       {d4, d5, d6, d7}, [r1]!
        vld1.s16       {d8, d9}, [r1]!   
        
        vqdmlal.s16    q10, d0, d0
        vqdmlal.s16    q10, d1, d1
        vqdmlal.s16    q10, d2, d2
        vqdmlal.s16    q10, d3, d3
        vqdmlal.s16    q10, d4, d4
        vqdmlal.s16    q10, d5, d5
        vqdmlal.s16    q10, d6, d6
        vqdmlal.s16    q10, d7, d7
        vqdmlal.s16    q10, d8, d8
        vqdmlal.s16    q10, d9, d9

        cmp        r3, #40
        beq        ENd
         
        vld1.s16       {d0, d1, d2, d3}, [r1]!
        vld1.s16       {d4, d5, d6, d7}, [r1]!
        vld1.s16       {d8, d9}, [r1]!   
        
        vqdmlal.s16    q10, d0, d0
        vqdmlal.s16    q10, d1, d1
        vqdmlal.s16    q10, d2, d2
        vqdmlal.s16    q10, d3, d3
        vqdmlal.s16    q10, d4, d4
        vqdmlal.s16    q10, d5, d5
        vqdmlal.s16    q10, d6, d6
        vqdmlal.s16    q10, d7, d7
        vqdmlal.s16    q10, d8, d8
        vqdmlal.s16    q10, d9, d9
        
ENd: 
        vqadd.s32      d21, d21, d20
        vext.32        d20, d21, d21, #1
        vqadd.s32      d21, d21, d20
        vmov.s32       r1, d21[0]
        qadd           r0, r0, r1

sum_armv7_xx_end: 
 
        ldmfd      r13!, {r1 - r12, r15}
        @.ENd
