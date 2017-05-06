@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@* File Name: 
@*            amrnb_residu_neon.s
@*
@* description: 
@*            This module implements the Residu().
@*
@* Functions Included:
@*             void Residu()
@*
@***************************** Change History**************************
@* 
@*    dd/MMM/YYYY     Code Ver     description             Author
@*    -----------     --------     -----------             ------
@*    08-12-2008        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@********************************************************************** 
           #include "voAMRNBDecID.h"
           .text  
           .align 4
           .globl _voAMRNBDec_Residu_asm

_voAMRNBDec_Residu_asm:

           stmfd              sp!, {r0-r11,lr}
           vld1.s16           {d0, d1}, [r0]!                      @ load all prediction coefficient
           sub                r1, r1, #14                          @ get  x[-7] address
           vmov.s32           q13, #0x8000
       
Residu_loop:
           vld1.s16           {d4, d5}, [r1]!                      @ load x[-7], x[-6],..., x[0]
           mov                r8, r1
           vext.16            d6, d4, d5, #1                       @ dx1 --- x[-6], x[-5], x[-4], x[-3]

           vqdmull.s16        q10, d4, d1[3] 		           @ a[7].x[-7], a[7].x[-6], a[7].x[-5], a[7].x[-4]
           vext.16            d7, d4, d5, #2                       @ dx2 --- x[-5], x[-4], x[-3], x[-2]
           vqdmull.s16        q11, d5, d1[3]       		   @ a[7].x[-3], a[7].x[-2], a[7].x[-1]. a[7].x[0]
           vext.16            d8, d4, d5, #3                       @ dx3 --- x[-4], x[-3], x[-2], x[-1] 

           vld1.s16           d4, [r8]!                            @ x[1], x[2], x[3], x[4]
           vext.16            d9, d5, d4, #1                       @ dx5 --- x[-2], x[-1], x[0], x[1]

           vqdmlal.s16        q10, d6, d1[2]                       @ a[6].x[-6], a[6].x[-5], a[6].x[-4], a[6].x[-3]
           vext.16            d10, d5, d4, #2                      @ dx6 --- x[-1], x[0], x[1], x[2]
           vqdmlal.s16        q11, d9, d1[2]                       @ a[6].x[-2], a[6].x[-1], a[6].x[0], a[6].x[1]
              
           vqdmlal.s16        q10, d7, d1[1]                       @ a[5].x[-5], a[5].x[-4], a[5].x[-3], a[5].x[-2]
           vext.16            d11, d5, d4, #3                      @ dx7 --- x[0], x[1], x[2], x[3]
           vqdmlal.s16        q11, d10, d1[1]                      @ a[5].x[-1], a[5].x[0], a[5].x[1], a[5].x[2]

           vqdmlal.s16        q10, d8, d1[0]                       @ a[4].x[-4], a[4].x[-3], a[4].x[-2], a[4].x[-1]
           vqdmlal.s16        q11, d11, d1[0]                      @ a[4].x[0], a[4].x[1], a[4].x[2], a[4].x[3]

           vqdmlal.s16        q10, d5, d0[3]                       @ a[3].x[-3], a[3].x[-2], a[3].x[-1], a[3].x[0]
           vqdmlal.s16        q11, d4, d0[3]                       @ a[3].x[1], a[3].x[2], a[3].x[3], a[3].x[4]

           vld1.s16           d5, [r8]!                            @ x[5], x[6], x[7], x[8]
           vext.16            d6, d4, d5, #1                       @ x[2], x[3], x[4], x[5]
 
           vqdmlal.s16        q10, d9, d0[2]                       @ a[2].x[-2], a[2].x[-1], a[2].x[0], a[2].x[1]
           vext.16            d7, d4, d5, #2                       @ x[3], x[4], x[5], x[6]
           vqdmlal.s16        q11, d6, d0[2]                       @ a[2].x[2], a[2].x[3], a[2].x[4], a[2].x[5]

           vqdmlal.s16        q10, d10, d0[1]                      @ a[1].x[-1], a[1].x[0], a[1].x[1], a[1].x[2]
           vext.16            d8, d4, d5, #3                       @ x[4], x[5], x[6], x[7]
           vqdmlal.s16        q11, d7, d0[1]                       @ a[1].x[3], a[1].x[4], a[1].x[5], a[1].x[6]

           vqdmlal.s16        q10, d11, d0[0]                      @ a[0].x[0], a[0].x[1], a[0].x[2], a[0].x[3]
           vqdmlal.s16        q11, d8, d0[0]                       @ a[0].x[4], a[0].x[5], a[0].x[6], a[0].x[7]

           vqshl.s32          q10, q10, #3
           vqshl.s32          q11, q11, #3
  
           vqadd.s32          q10, q10, q13
           vqadd.s32          q11, q11, q13

           vshrn.s32          d24, q10, #16
           vshrn.s32          d25, q11, #16
          
           vst1.s16           {d24, d25}, [r2]!
           subs               r3, r3, #8
           bgt                Residu_loop

Vo_Residu_asm_end:

           ldmfd              sp!, {r0 - r11,pc}      
           
           @.END




