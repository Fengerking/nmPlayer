@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@ Word32 voAMRNBDecsum_armv7_xx(s, s_excf[], s_excf[], Length)
@ {
@   for (j = 0@ j < L_subfr@ j++) {
@	s = L_mac (s, s_excf[j], s_excf[j])@
@ }
        #include "voAMRNBDecID.h"
        .text   
        .align 4
        .globl  _voAMRNBDecsum_armv7_xx

@******************************
@ constant
@******************************

@******************************
@ ARM register 
@******************************
@ r0 --- s
@ r1 --- s_excf[]
@ r2 --- s_excf[]
@ r3 --- length         
@******************************
@ Neon register 
@******************************
@a10          .req            d0.s16
@a11          .req            d1.s16              
@a12          .req            d2.s16
@a13          .req            d3.s16 
@a14          .req            d4.s16
@a15          .req            d5.s16 
@a16          .req            d6.s16
@a17          .req            d7.s16 
@a18          .req            d8.s16
@a19          .req            d9.s16 

@sum          .req            q10.s32
@sum_hi       .req            d20.s32
@sum_lo       .req            d21.s32


_voAMRNBDecsum_armv7_xx:

        stmfd          r13!, {r1 - r12, r14} 
        vld4.s16       {d0, d1, d2, d3}, [r1]!
        vmov.s32       q10, #0
        vld4.s16       {d4, d5, d6, d7}, [r1]!
        vld2.s16       {d8, d9}, [r1]!   
        
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

        cmp            r3, #40
        beq            END
         
        vld4.s16       {d0, d1, d2, d3}, [r1]!
        vld4.s16       {d4, d5, d6, d7}, [r1]!
        vld2.s16       {d8, d9}, [r1]!   
        
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
        
END: 
        vqadd.s32      d21, d21, d20
        vext.32        d20, d21, d21, #1
        vqadd.s32      d21, d21, d20
        vmov.s32       r0, d21[0]

sum_armv7_xx_end: 
 
        ldmfd          r13!, {r1 - r12, r15}
        
        @.ENd
