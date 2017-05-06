@/************************************************************************
@VisualOn Proprietary
@Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
@
@VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
@
@All data and information contained in or disclosed by this document are
@confidential and proprietary information of VisualOn, and all rights
@therein are expressly reserved. By accepting this material, the
@recipient agrees that this material and the information contained
@therein are held in confidence and in trust. The material may only be
@used and/or disclosed as authorized in a license agreement controlling
@such use and disclosure.
@************************************************************************/
@
@/************************************************************************
@* @file h265_deblock_neon.asm 
@*
@* H.265 decoder deblock filter functions neon optimization 
@*
@* @author  Huaping Liu
@* @date    2012-12-28
@************************************************************************/
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
	@AREA     |.text|, CODE, READONLY, ALIGN=2
        .section  .text
        .align    4
        .if DEBLOCK_ASM_ENABLED==1   
        .global   voLumaFilterHor_ASM
        .global   voLumaFilterVer_ASM
        .global   voChromaFilterHor_ASM
        .global   voChromaFilterVer_ASM
        .global   voChromaFilterCross_ASM
        .global   voLumaFilterCross_ASM

voLumaFilterCross_ASM:
@ r0 src
@ r1 stride
@ r2 beta
@ r3 tc
    stmfd           r13!, {r4 - r10, r14}
    vdup.u32        d30, r3
    vdup.u32        d31, r2
    vmov.u8         r4, d30[0]              @tc0
    vmov.u8         r5, d30[1]              @tc1
    vmov.u8         r6, d30[2]              @tc2
    vmov.u8         r7, d30[3]              @tc3
    adds            r8, r4, r5
    cmp             r8, #0
    beq             LumaCross_Load
LumaCross_Ver:
    sub             r9, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r9, #4              @ piSrc[-4 * iStride-4] 
    vld4.8		    {d0[0],d1[0],d2[0],d3[0]}, [r8], r1
    vld4.8		    {d4[0],d5[0],d6[0],d7[0]}, [r9], r1
    vld4.8		    {d0[1],d1[1],d2[1],d3[1]}, [r8], r1
    vld4.8		    {d4[1],d5[1],d6[1],d7[1]}, [r9], r1
    vld4.8		    {d0[2],d1[2],d2[2],d3[2]}, [r8], r1
    vld4.8		    {d4[2],d5[2],d6[2],d7[2]}, [r9], r1
    vld4.8		    {d0[3],d1[3],d2[3],d3[3]}, [r8], r1
    vld4.8		    {d4[3],d5[3],d6[3],d7[3]}, [r9], r1
    vld4.8		    {d0[4],d1[4],d2[4],d3[4]}, [r8], r1
    vld4.8		    {d4[4],d5[4],d6[4],d7[4]}, [r9], r1
    vld4.8		    {d0[5],d1[5],d2[5],d3[5]}, [r8], r1
    vld4.8		    {d4[5],d5[5],d6[5],d7[5]}, [r9], r1
    vld4.8		    {d0[6],d1[6],d2[6],d3[6]}, [r8], r1
    vld4.8		    {d4[6],d5[6],d6[6],d7[6]}, [r9], r1
    vld4.8		    {d0[7],d1[7],d2[7],d3[7]}, [r8], r1
    vld4.8		    {d4[7],d5[7],d6[7],d7[7]}, [r9], r1
    
    vshll.u8	    q4, d2, #1              @ 2*p1
    vshll.u8	    q5, d5, #1              @ 2*q1
    vaddl.u8        q6, d1, d3              @ p2+p0
    vaddl.u8        q7, d6, d4              @ q2+q0
    vabd.u16		q4, q6, q4              @ dp0-3
    vabd.u16		q5, q5, q7              @ dq0-3
    vadd.u16        q6, q5, q4              @ d0-3
    vmov.u16        r8, d12[0]              @ d0
    vmov.u16        r9, d12[3]              @ d3
    add             r9, r9, r8              @ d0+d3
    vmov.u8         r10, d31[0]             @ beta0
    subs            r9, r9, r10             @ (d0+d3)<beta
    movge           r4, #0                  @ set tc0 to 0
    vmov.u16        r8, d13[0]              @ d0
    vmov.u16        r10, d13[3]             @ d3
    add             r8, r10, r8             @ d0+d3
    vmov.u8         r10, d31[1]             @ beta1
    subs            r8, r8, r10             @ (d0+d3)<beta
    movge           r5, #0                  @ set tc1 to 0
    adds            r8, r4, r5
    cmp             r8, #0
    beq             LumaCross_Hor
    vdup.u8         d28, r4                 @ tc0
    vdup.u8         d29, r5                 @ tc1
    vtrn.32         d28, d29                @ tc0 tc1
    vmov.u8         r8, d31[0]              @ beta0
    vmov.u8         r9, d31[1]              @ beta1
    vdup.u8         d29, r8                 @ beta0
    vdup.u8         d27, r9                 @ beta1
    vtrn.32         d29, d27                @ beta0 beta1
    vmovl.u8        q13, d28                @ tc
    vmovl.u8        q12, d29                @ beta
    vshl.u16        q11, q13, #2            @ 4*tc
    vadd.u16        q11, q11, q13           @ 5*tc
    vrshr.u16       q11, q11, #1            @ (5*tc+1)>>1
    vshr.u16        q10, q12 , #2           @ beta>>2
    vshr.u16        q9,  q12 , #3           @ beta>>3 
    vshl.u16        q6, q6 , #1             @ d0-3<<1
    vclt.u16		q6, q6, q10			    @ (d0-3<<1)<(beta>>2)    !1
    vabdl.u8		q8, d0, d3              @ abs(p3-p0)
    vabdl.u8		q7, d7, d4              @ abs(q3-q0)
    vabdl.u8		q10, d3, d4             @ abs(p0-q0)
    vadd.u16		q8, q8, q7			    @ abs(p3-p0)+abs(q3-q0) 
    vclt.u16		q8, q8, q9			    @ abs(p3-p0)+abs(q3-q0)<(beta>>3) !2
    vclt.u16		q10, q10, q11			@ abs(p0-q0)<tc_s !3
    vand.u16        q6, q6, q8              @ !1&&!2
    vand.u16        q6, q6, q10             @ !1&&!2&&!3
    vtrn.u16        d12, d13
    vmovl.s16       q7, d12
    vmovl.s16       q8, d13
    vand.u32        d26, d14, d17            @ mask for strong
    vmvn            d27, d26                 @ mask for weak
    vmov.s32        r8, d26[0]
    vmov.s32        r9, d26[1]
    cmp             r4, #0
    moveq           r8, r9
    cmp             r5, #0
    moveq           r9, r8
    add             r10, r8, r9
    cmp             r10, #-2
    beq             v_strong_all
    cmp             r10, #0
    beq             v_weak_all
    
    @ weak
    vtrn.u16        d8, d9                  @ d8:00xx d9:xx33 dp
    vtrn.u16        d10, d11                @ d10:00xx d11:xx33 dq
    vtrn.u32        d8, d10                 @ d8: 0000 dp dq
    vtrn.u32        d9, d11                 @ d1: 3333 dp dq
    vaddl.s16       q4, d8, d11             @ dp0+3 dp0+3 dq0+3 dq0+3 
    vtrn.u16        d24, d25                @ d24:b0b1b0b1
    vshr.u16        d25, d24, #1            @ b>>1
    vaddl.u16       q12, d24, d25           @ b+(b>>1)
    vshr.u32        q12, q12, #3            @ (b+(b>>1))>>3
    vclt.s32        q12, q4, q12            @ d24 p1 d25 q1
    @ delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4@
@ d0 p3   @ d7 q3
@ d1 p2   @ d6 q2
@ d2 p1   @ d5 q1
@ d3 p0   @ d4 q0 
    vsubl.u8        q4, d4, d3              @ q0 - p0
    vsubl.u8        q5, d5, d2              @ q1 - p1
    vshl.s16        q6, q4, #3              @ 8*(q0 - p0)
    vadd.s16        q6, q6, q4              @ 9*(q0 - p0)
    vshl.s16        q7, q5, #1              @ 2*(q1 - p1)
    vadd.s16        q7, q7, q5              @ 3*(q1 - p1)
    vsub.s16        q5, q6, q7              @ 9*(q0 - p0) - 3*(q1 - p1)
    vshl.u8         d22, d28, #3            @ tc*8
    vadd.u8         d22, d22, d28
    vadd.u8         d22, d22, d28           @ 10*tc
    vrshrn.s16      d10, q5, #4             @ delta
    vneg.s8         d29, d28                @ -tc
    vabs.s8         d12, d10                @ ABS(delta)
    vmax.s8         d11, d10, d29           @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d12, d22           @ ABS(delta) < 10*tc
    vmin.s8         d11, d11, d28           @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d11, d11, d13             @ mask for p0 q0
    vshr.u8         d30, d28, #1             @ tc_2
    vneg.s8         d29, d30                 @ -tc_2
    vand.s8         d24, d24, d13           @ mask for p1
    vand.s8         d25, d25, d13           @ mask for q1
    vrhadd.u8       d17, d1, d3             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d4, d6            @(q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d2            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d5           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d11            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d11            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d29            @ min
    vmax.s8         d18, d18, d29            @ min 
    vmin.s8         d17, d17, d30           @ deltap
    vmin.s8         d18, d18, d30           @ deltaq
    vand.s8         d17, d17, d24
    vand.s8         d18, d18, d25
    vmovl.s8	    q10, d11					@ delta
    vaddw.u8	    q11, q10, d3		    @ p0+delta 
    vqmovun.s16	    d8, q11
    vand.u8         d3, d3, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d3, d3, d8
    vmovl.u8	    q10, d4					@ q0	
    vsubw.s8	    q11, q10, d11		    @ q0 - delta
    vqmovun.s16	    d8, q11
    vand.u8         d4, d4, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d4, d4, d8
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d2		    @ p1 + deltap 
    vqmovun.s16	    d8, q11
    vand.u8         d2, d2, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d2, d2, d8
    vmovl.s8	    q10, d18				@deltaq
    vaddw.u8	    q11, q10, d5		    @ q1 + deltaq 
    vqmovun.s16	    d8, q11
    vand.u8         d5, d5, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d5, d5, d8
    @ strong
    vshl.u8         d8, d28, #1            @ tc2   
    vaddl.u8        q5, d1, d2             @ p2 + p1 
    vaddl.u8        q6, d3, d4             @ p0 + q0
    vaddl.u8        q7, d0, d1             @ p3 + p2
    vadd.u16        q12, q5, q6            @ p2 + p1 + p0 + q0 
    vadd.u16        q15, q12, q7           @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        q15, q15, q7           @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q5, d2, d3             @ p1 + p0
    vaddl.u8        q7, d4, d5             @ q0 + q1
    vadd.u16        q14, q12, q5           @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        q14, q14, q7           @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q8, d6, d5             @ q2 + q1
    vaddl.u8        q10, d7, d6            @ q3 + q2
    vadd.u16        q11, q8, q6            @ q2 + q1 + p0 + q0
    vadd.u16        q8, q11, q10           @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        q8, q8, q10            @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        q6, q11, q7            @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        q6, q6, q5             @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d20, d2, d8            @ p1 - tc2
    vqadd.u8        d21, d2, d8            @ p1 + tc2
    vrshrn.u16      d24, q12, #2           @ ( p2 + p1 + p0 + q0 + 2 ) >> 2   
    vmax.u8         d10, d24, d20           @ 
    vmin.u8         d10, d10, d21            @ p1
    vand.u8         d2, d2, d27             @ weak
    vand.u8         d10, d10, d26             @ strong
    vorr.u8         d2, d2, d10
    vqsub.u8        d20, d1, d8            @ p2 - tc2
    vqadd.u8        d21, d1, d8            @ p2 + tc2
    vrshrn.u16      d30, q15, #3           @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d10, d30, d20          @ 
    vmin.u8         d10, d10, d21          @ p2
    vand.u8         d1, d1, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d1, d1, d10
    vqsub.u8        d20, d3, d8            @ p0 - tc2
    vqadd.u8        d21, d3, d8            @ p0 + tc2
    vrshrn.u16      d28, q14, #3           @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d10, d28, d20          @ 
    vmin.u8         d10, d10, d21          @ p0
    vand.u8         d3, d3, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d3, d3, d10
    vqsub.u8        d20, d5, d8            @ q1 - tc2
    vqadd.u8        d21, d5, d8            @ q1 + tc2
    vrshrn.u16      d22, q11, #2           @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d10, d22, d20          @ 
    vmin.u8         d10, d10, d21          @ q1
    vand.u8         d5, d5, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d5, d5, d10
    vqsub.u8        d20, d6, d8            @ q2 - tc2
    vqadd.u8        d21, d6, d8            @ q2 + tc2
    vrshrn.u16      d22, q8, #3            @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d10, d22, d20          @ 
    vmin.u8         d10, d10, d21          @ q2
    vand.u8         d6, d6, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d6, d6, d10
    vqsub.u8        d20, d4, d8            @ q0 - tc2
    vqadd.u8        d21, d4, d8            @ q0 + tc2
    vrshrn.u16      d12, q6, #3            @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d10, d12, d20          @ 
    vmin.u8         d10, d10, d21          @ q0
    vand.u8         d4, d4, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d4, d4, d10
    b               LumaCross_Hor
v_strong_all:
    vshl.u8         d8, d28, #1            @ tc2   
    vaddl.u8        q5, d1, d2             @ p2 + p1 
    vaddl.u8        q6, d3, d4             @ p0 + q0
    vaddl.u8        q7, d0, d1             @ p3 + p2
    vadd.u16        q12, q5, q6            @ p2 + p1 + p0 + q0 
    vadd.u16        q15, q12, q7           @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        q15, q15, q7           @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q5, d2, d3             @ p1 + p0
    vaddl.u8        q7, d4, d5             @ q0 + q1
    vadd.u16        q14, q12, q5           @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        q14, q14, q7           @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q8, d6, d5             @ q2 + q1
    vaddl.u8        q10, d7, d6            @ q3 + q2
    vadd.u16        q11, q8, q6            @ q2 + q1 + p0 + q0
    vadd.u16        q8, q11, q10           @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        q8, q8, q10            @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        q6, q11, q7            @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        q6, q6, q5             @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d20, d2, d8            @ p1 - tc2
    vqadd.u8        d21, d2, d8            @ p1 + tc2
    vrshrn.u16      d2, q12, #2            @ ( p2 + p1 + p0 + q0 + 2 ) >> 2   
    vmax.u8         d2, d2, d20            @ 
    vmin.u8         d2, d2, d21            @ p1
    vqsub.u8        d20, d1, d8            @ p2 - tc2
    vqadd.u8        d21, d1, d8            @ p2 + tc2
    vrshrn.u16      d1, q15, #3            @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d1, d1, d20            @ 
    vmin.u8         d1, d1, d21            @ p2
    vqsub.u8        d20, d3, d8            @ p0 - tc2
    vqadd.u8        d21, d3, d8            @ p0 + tc2
    vrshrn.u16      d3, q14, #3            @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d3, d3, d20            @ 
    vmin.u8         d3, d3, d21            @ p0
    vqsub.u8        d20, d5, d8            @ q1 - tc2
    vqadd.u8        d21, d5, d8            @ q1 + tc2
    vrshrn.u16      d5, q11, #2            @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d5, d5, d20            @ 
    vmin.u8         d5, d5, d21            @ q1
    vqsub.u8        d20, d6, d8            @ q2 - tc2
    vqadd.u8        d21, d6, d8            @ q2 + tc2
    vrshrn.u16      d6, q8, #3             @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d6, d6, d20            @ 
    vmin.u8         d6, d6, d21            @ q2
    vqsub.u8        d20, d4, d8            @ q0 - tc2
    vqadd.u8        d21, d4, d8            @ q0 + tc2
    vrshrn.u16      d4, q6, #3             @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d4, d4, d20            @ 
    vmin.u8         d4, d4, d21            @ q0
    b               LumaCross_Hor
v_weak_all:
    vtrn.u16        d8, d9                  @ d8:00xx d9:xx33 dp
    vtrn.u16        d10, d11                @ d10:00xx d11:xx33 dq
    vtrn.u32        d8, d10                 @ d8: 0000 dp dq
    vtrn.u32        d9, d11                 @ d1: 3333 dp dq
    vaddl.s16       q4, d8, d11             @ dp0+3 dp0+3 dq0+3 dq0+3 
    vtrn.u16        d24, d25                @ d24:b0b1b0b1
    vshr.u16        d25, d24, #1            @ b>>1
    vaddl.u16       q12, d24, d25           @ b+(b>>1)
    vshr.u32        q12, q12, #3            @ (b+(b>>1))>>3
    vclt.s32        q12, q4, q12            @ d24 p1 d25 q1 
    vsubl.u8        q4, d4, d3              @ q0 - p0
    vsubl.u8        q5, d5, d2              @ q1 - p1
    vshl.s16        q6, q4, #3              @ 8*(q0 - p0)
    vadd.s16        q6, q6, q4              @ 9*(q0 - p0)
    vshl.s16        q7, q5, #1              @ 2*(q1 - p1)
    vadd.s16        q7, q7, q5              @ 3*(q1 - p1)
    vsub.s16        q5, q6, q7              @ 9*(q0 - p0) - 3*(q1 - p1)
    vshl.u8         d22, d28, #3            @ tc*8
    vadd.u8         d22, d22, d28
    vadd.u8         d22, d22, d28           @ 10*tc
    vrshrn.s16      d10, q5, #4             @ delta
    vneg.s8         d29, d28                @ -tc
    vabs.s8         d12, d10                @ ABS(delta)
    vmax.s8         d11, d10, d29           @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d12, d22           @ ABS(delta) < 10*tc
    vmin.s8         d11, d11, d28           @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d11, d11, d13             @ mask for p0 q0
    vshr.u8         d30, d28, #1             @ tc_2
    vneg.s8         d29, d30                 @ -tc_2
    vand.s8         d24, d24, d13           @ mask for p1
    vand.s8         d25, d25, d13           @ mask for q1
    vrhadd.u8       d17, d1, d3             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d4, d6            @(q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d2            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d5           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d11            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d11            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d29            @ min
    vmax.s8         d18, d18, d29            @ min 
    vmin.s8         d17, d17, d30           @ deltap
    vmin.s8         d18, d18, d30           @ deltaq
    vand.s8         d17, d17, d24
    vand.s8         d18, d18, d25
    vmovl.s8	    q10, d11					@ delta
    vaddw.u8	    q11, q10, d3		    @ p0+delta 
    vqmovun.s16	    d3, q11
    vmovl.u8	    q10, d4					@ q0	
    vsubw.s8	    q11, q10, d11		    @ q0 - delta
    vqmovun.s16	    d4, q11
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d2		    @ p1 + deltap 
    vqmovun.s16	    d2, q11
    vmovl.s8	    q10, d18				@deltaq
    vaddw.u8	    q11, q10, d5		    @ q1 + deltaq 
    vqmovun.s16	    d5, q11
    b               LumaCross_Hor
LumaCross_Load:
    sub             r4, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r4, #4              @ piSrc[-4 * iStride-4]
    vld1.64         {d0}, [r8], r1          @ p3
    vld1.64         {d1}, [r8], r1          @ p2
    vld1.64         {d2}, [r8], r1          @ p1
    vld1.64         {d3}, [r8], r1          @ p0
    vld1.64         {d4}, [r8], r1          @ q3
    vld1.64         {d5}, [r8], r1          @ q2
    vld1.64         {d6}, [r8], r1          @ q1
    vld1.64         {d7}, [r8], r1          @ q0
    b               LumaCross_Hor_Calc
LumaCross_Hor:
    vtrn.8          d0, d1      @t00 t10 t02 t12 t04 t14 t06 t16     t01 t11 t03 t13 t05 t15 t07 t17
    vtrn.8          d2, d3      @t20 t30 t22 t32 t24 t34 t26 t36     t21 t31 t23 t33 t25 t35 t27 t37
    vtrn.16         d0, d2      @t00 t10 t20 t30 t04 t14 t24 t34     t02 t12 t22 t32 t06 t16 t26 t36
    vtrn.8          d4, d5      @t40 t50 t42 t52 t44 t54 t46 t56     t41 t51 t43 t53 t45 t55 t47 t57
    vtrn.8          d6, d7      @t60 t70 t62 t72 t64 t74 t66 t76     t61 t71 t63 t73 t65 t75 t67 t77 
    vtrn.16         d1, d3      @t01 t11 t21 t31 t05 t15 t25 t35     t03 t13 t23 t33 t05 t15 t25 t35
    vtrn.16         d4, d6      @t40 t50 t60 t70 t44 t54 t64 t74     t42 t52 t62 t72 t46 t56 t66 t76
    vtrn.16         d5, d7      @t41 t51 t61 t71 t45 t55 t65 t75     t43 t53 t63 t73 t47 t57 t67 t77
        
    vtrn.32         d0, d4      @ t00 t10 t20 t30 t40 t50 t60 t70    t04 t14 t24 t34 t44 t54 t64 t74
    vtrn.32         d1, d5      @ t01 t11 t21 t31 t41 t51 t61 t71    t05 t15 t25 t35 t45 t55 t65 t75  
    vtrn.32         d2, d6      @ t02 t12 t22 t32 t42 t52 t62 t72    t06 t16 t26 t36 t46 t56 t66 t76 
    vtrn.32         d3, d7      @ t03 t13 t23 t33 t43 t53 t63 t73    t07 t17 t27 t37 t47 t57 t67 t77
    adds            r8, r6, r7
    cmp             r8, #0
    beq             LumaCross_Store
LumaCross_Hor_Calc:
    vdup.u32        d31, r2
    vshll.u8	    q4, d2, #1              @ 2*p1
    vshll.u8	    q5, d5, #1              @ 2*q1
    vaddl.u8        q6, d1, d3              @ p2+p0
    vaddl.u8        q7, d6, d4              @ q2+q0
    vabd.u16		q4, q6, q4              @ dp0-3
    vabd.u16		q5, q5, q7              @ dq0-3
    vadd.u16        q6, q5, q4              @ d0-3
    vmov.u16        r8, d12[0]              @ d0
    vmov.u16        r9, d12[3]              @ d3
    add             r9, r9, r8              @ d0+d3
    vmov.u8         r10, d31[2]             @ beta0
    subs            r9, r9, r10             @ (d0+d3)<beta
    movge           r6, #0                  @ set tc0 to 0
    vmov.u16        r8, d13[0]              @ d0
    vmov.u16        r10, d13[3]             @ d3
    add             r8, r10, r8             @ d0+d3
    vmov.u8         r10, d31[3]             @ beta1
    subs            r8, r8, r10             @ (d0+d3)<beta
    movge           r7, #0                  @ set tc1 to 0
    adds            r8, r6, r7
    cmp             r8, #0
    beq             LumaCross_Store
    vdup.u8         d28, r6                 @ tc0
    vdup.u8         d29, r7                 @ tc1
    vtrn.32         d28, d29                @ tc0 tc1
    vmov.u8         r8, d31[2]              @ beta0
    vmov.u8         r9, d31[3]              @ beta1
    vdup.u8         d29, r8                 @ beta0
    vdup.u8         d27, r9                 @ beta1
    vtrn.32         d29, d27                @ beta0 beta1
    vmovl.u8        q13, d28                @ tc
    vmovl.u8        q12, d29                @ beta
    vshl.u16        q11, q13, #2            @ 4*tc
    vadd.u16        q11, q11, q13           @ 5*tc
    vrshr.u16       q11, q11, #1            @ (5*tc+1)>>1
    vshr.u16        q10, q12 , #2           @ beta>>2
    vshr.u16        q9,  q12 , #3           @ beta>>3 
    vshl.u16        q6, q6 , #1             @ d0-3<<1
    vclt.u16		q6, q6, q10			    @ (d0-3<<1)<(beta>>2)    !1
    vabdl.u8		q8, d0, d3              @ abs(p3-p0)
    vabdl.u8		q7, d7, d4              @ abs(q3-q0)
    vabdl.u8		q10, d3, d4             @ abs(p0-q0)
    vadd.u16		q8, q8, q7			    @ abs(p3-p0)+abs(q3-q0) 
    vclt.u16		q8, q8, q9			    @ abs(p3-p0)+abs(q3-q0)<(beta>>3) !2
    vclt.u16		q10, q10, q11			@ abs(p0-q0)<tc_s !3
    vand.u16        q6, q6, q8              @ !1&&!2
    vand.u16        q6, q6, q10             @ !1&&!2&&!3
    vtrn.u16        d12, d13
    vmovl.s16       q7, d12
    vmovl.s16       q8, d13
    vand.u32        d26, d14, d17            @ mask for strong
    vmvn            d27, d26                 @ mask for weak
    vmov.s32        r8, d26[0]
    vmov.s32        r9, d26[1]
    cmp             r6, #0
    moveq           r8, r9
    cmp             r7, #0
    moveq           r9, r8
    add             r10, r8, r9
    cmp             r10, #-2
    beq             h_strong_all
    cmp             r10, #0
    beq             h_weak_all
    @ weak
    vtrn.u16        d8, d9                  @ d8:00xx d9:xx33 dp
    vtrn.u16        d10, d11                @ d10:00xx d11:xx33 dq
    vtrn.u32        d8, d10                 @ d8: 0000 dp dq
    vtrn.u32        d9, d11                 @ d1: 3333 dp dq
    vaddl.s16       q4, d8, d11             @ dp0+3 dp0+3 dq0+3 dq0+3 
    vtrn.u16        d24, d25                @ d24:b0b1b0b1
    vshr.u16        d25, d24, #1            @ b>>1
    vaddl.u16       q12, d24, d25           @ b+(b>>1)
    vshr.u32        q12, q12, #3            @ (b+(b>>1))>>3
    vclt.s32        q12, q4, q12            @ d24 p1 d25 q1
    @ delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4@
@ d0 p3   @ d7 q3
@ d1 p2   @ d6 q2
@ d2 p1   @ d5 q1
@ d3 p0   @ d4 q0 
    vsubl.u8        q4, d4, d3              @ q0 - p0
    vsubl.u8        q5, d5, d2              @ q1 - p1
    vshl.s16        q6, q4, #3              @ 8*(q0 - p0)
    vadd.s16        q6, q6, q4              @ 9*(q0 - p0)
    vshl.s16        q7, q5, #1              @ 2*(q1 - p1)
    vadd.s16        q7, q7, q5              @ 3*(q1 - p1)
    vsub.s16        q5, q6, q7              @ 9*(q0 - p0) - 3*(q1 - p1)
    vshl.u8         d22, d28, #3            @ tc*8
    vadd.u8         d22, d22, d28
    vadd.u8         d22, d22, d28           @ 10*tc
    vrshrn.s16      d10, q5, #4             @ delta
    vneg.s8         d29, d28                @ -tc
    vabs.s8         d12, d10                @ ABS(delta)
    vmax.s8         d11, d10, d29           @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d12, d22           @ ABS(delta) < 10*tc
    vmin.s8         d11, d11, d28           @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d11, d11, d13             @ mask for p0 q0
    vshr.u8         d30, d28, #1             @ tc_2
    vneg.s8         d29, d30                 @ -tc_2
    vand.s8         d24, d24, d13           @ mask for p1
    vand.s8         d25, d25, d13           @ mask for q1
    vrhadd.u8       d17, d1, d3             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d4, d6            @(q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d2            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d5           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d11            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d11            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d29            @ min
    vmax.s8         d18, d18, d29            @ min 
    vmin.s8         d17, d17, d30           @ deltap
    vmin.s8         d18, d18, d30           @ deltaq
    vand.s8         d17, d17, d24
    vand.s8         d18, d18, d25
    vmovl.s8	    q10, d11					@ delta
    vaddw.u8	    q11, q10, d3		    @ p0+delta 
    vqmovun.s16	    d8, q11
    vand.u8         d3, d3, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d3, d3, d8
    vmovl.u8	    q10, d4					@ q0	
    vsubw.s8	    q11, q10, d11		    @ q0 - delta
    vqmovun.s16	    d8, q11
    vand.u8         d4, d4, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d4, d4, d8
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d2		    @ p1 + deltap 
    vqmovun.s16	    d8, q11
    vand.u8         d2, d2, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d2, d2, d8
    vmovl.s8	    q10, d18				@deltaq
    vaddw.u8	    q11, q10, d5		    @ q1 + deltaq 
    vqmovun.s16	    d8, q11
    vand.u8         d5, d5, d26             @ strong
    vand.u8         d8, d8, d27             @ weak
    vorr.u8         d5, d5, d8
    @ strong
    vshl.u8         d8, d28, #1            @ tc2   
    vaddl.u8        q5, d1, d2             @ p2 + p1 
    vaddl.u8        q6, d3, d4             @ p0 + q0
    vaddl.u8        q7, d0, d1             @ p3 + p2
    vadd.u16        q12, q5, q6            @ p2 + p1 + p0 + q0 
    vadd.u16        q15, q12, q7           @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        q15, q15, q7           @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q5, d2, d3             @ p1 + p0
    vaddl.u8        q7, d4, d5             @ q0 + q1
    vadd.u16        q14, q12, q5           @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        q14, q14, q7           @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q8, d6, d5             @ q2 + q1
    vaddl.u8        q10, d7, d6            @ q3 + q2
    vadd.u16        q11, q8, q6            @ q2 + q1 + p0 + q0
    vadd.u16        q8, q11, q10           @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        q8, q8, q10            @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        q6, q11, q7            @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        q6, q6, q5             @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d20, d2, d8            @ p1 - tc2
    vqadd.u8        d21, d2, d8            @ p1 + tc2
    vrshrn.u16      d24, q12, #2           @ ( p2 + p1 + p0 + q0 + 2 ) >> 2   
    vmax.u8         d10, d24, d20           @ 
    vmin.u8         d10, d10, d21            @ p1
    vand.u8         d2, d2, d27             @ weak
    vand.u8         d10, d10, d26             @ strong
    vorr.u8         d2, d2, d10
    vqsub.u8        d20, d1, d8            @ p2 - tc2
    vqadd.u8        d21, d1, d8            @ p2 + tc2
    vrshrn.u16      d30, q15, #3           @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d10, d30, d20          @ 
    vmin.u8         d10, d10, d21          @ p2
    vand.u8         d1, d1, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d1, d1, d10
    vqsub.u8        d20, d3, d8            @ p0 - tc2
    vqadd.u8        d21, d3, d8            @ p0 + tc2
    vrshrn.u16      d28, q14, #3           @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d10, d28, d20          @ 
    vmin.u8         d10, d10, d21          @ p0
    vand.u8         d3, d3, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d3, d3, d10
    vqsub.u8        d20, d5, d8            @ q1 - tc2
    vqadd.u8        d21, d5, d8            @ q1 + tc2
    vrshrn.u16      d22, q11, #2           @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d10, d22, d20          @ 
    vmin.u8         d10, d10, d21          @ q1
    vand.u8         d5, d5, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d5, d5, d10
    vqsub.u8        d20, d6, d8            @ q2 - tc2
    vqadd.u8        d21, d6, d8            @ q2 + tc2
    vrshrn.u16      d22, q8, #3            @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d10, d22, d20          @ 
    vmin.u8         d10, d10, d21          @ q2
    vand.u8         d6, d6, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d6, d6, d10
    vqsub.u8        d20, d4, d8            @ q0 - tc2
    vqadd.u8        d21, d4, d8            @ q0 + tc2
    vrshrn.u16      d12, q6, #3            @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d10, d12, d20          @ 
    vmin.u8         d10, d10, d21          @ q0
    vand.u8         d4, d4, d27            @ weak
    vand.u8         d10, d10, d26          @ strong
    vorr.u8         d4, d4, d10
    b               LumaCross_Store
h_strong_all:
    vshl.u8         d8, d28, #1            @ tc2   
    vaddl.u8        q5, d1, d2             @ p2 + p1 
    vaddl.u8        q6, d3, d4             @ p0 + q0
    vaddl.u8        q7, d0, d1             @ p3 + p2
    vadd.u16        q12, q5, q6            @ p2 + p1 + p0 + q0 
    vadd.u16        q15, q12, q7           @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        q15, q15, q7           @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q5, d2, d3             @ p1 + p0
    vaddl.u8        q7, d4, d5             @ q0 + q1
    vadd.u16        q14, q12, q5           @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        q14, q14, q7           @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q8, d6, d5             @ q2 + q1
    vaddl.u8        q10, d7, d6            @ q3 + q2
    vadd.u16        q11, q8, q6            @ q2 + q1 + p0 + q0
    vadd.u16        q8, q11, q10           @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        q8, q8, q10            @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        q6, q11, q7            @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        q6, q6, q5             @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d20, d2, d8            @ p1 - tc2
    vqadd.u8        d21, d2, d8            @ p1 + tc2
    vrshrn.u16      d2, q12, #2            @ ( p2 + p1 + p0 + q0 + 2 ) >> 2   
    vmax.u8         d2, d2, d20            @ 
    vmin.u8         d2, d2, d21            @ p1
    vqsub.u8        d20, d1, d8            @ p2 - tc2
    vqadd.u8        d21, d1, d8            @ p2 + tc2
    vrshrn.u16      d1, q15, #3            @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d1, d1, d20            @ 
    vmin.u8         d1, d1, d21            @ p2
    vqsub.u8        d20, d3, d8            @ p0 - tc2
    vqadd.u8        d21, d3, d8            @ p0 + tc2
    vrshrn.u16      d3, q14, #3            @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d3, d3, d20            @ 
    vmin.u8         d3, d3, d21            @ p0
    vqsub.u8        d20, d5, d8            @ q1 - tc2
    vqadd.u8        d21, d5, d8            @ q1 + tc2
    vrshrn.u16      d5, q11, #2            @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d5, d5, d20            @ 
    vmin.u8         d5, d5, d21            @ q1
    vqsub.u8        d20, d6, d8            @ q2 - tc2
    vqadd.u8        d21, d6, d8            @ q2 + tc2
    vrshrn.u16      d6, q8, #3             @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d6, d6, d20            @ 
    vmin.u8         d6, d6, d21            @ q2
    vqsub.u8        d20, d4, d8            @ q0 - tc2
    vqadd.u8        d21, d4, d8            @ q0 + tc2
    vrshrn.u16      d4, q6, #3             @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d4, d4, d20            @ 
    vmin.u8         d4, d4, d21            @ q0
    b               LumaCross_Store
h_weak_all:
    vtrn.u16        d8, d9                  @ d8:00xx d9:xx33 dp
    vtrn.u16        d10, d11                @ d10:00xx d11:xx33 dq
    vtrn.u32        d8, d10                 @ d8: 0000 dp dq
    vtrn.u32        d9, d11                 @ d1: 3333 dp dq
    vaddl.s16       q4, d8, d11             @ dp0+3 dp0+3 dq0+3 dq0+3 
    vtrn.u16        d24, d25                @ d24:b0b1b0b1
    vshr.u16        d25, d24, #1            @ b>>1
    vaddl.u16       q12, d24, d25           @ b+(b>>1)
    vshr.u32        q12, q12, #3            @ (b+(b>>1))>>3
    vclt.s32        q12, q4, q12            @ d24 p1 d25 q1 
    vsubl.u8        q4, d4, d3              @ q0 - p0
    vsubl.u8        q5, d5, d2              @ q1 - p1
    vshl.s16        q6, q4, #3              @ 8*(q0 - p0)
    vadd.s16        q6, q6, q4              @ 9*(q0 - p0)
    vshl.s16        q7, q5, #1              @ 2*(q1 - p1)
    vadd.s16        q7, q7, q5              @ 3*(q1 - p1)
    vsub.s16        q5, q6, q7              @ 9*(q0 - p0) - 3*(q1 - p1)
    vshl.u8         d22, d28, #3            @ tc*8
    vadd.u8         d22, d22, d28
    vadd.u8         d22, d22, d28           @ 10*tc
    vrshrn.s16      d10, q5, #4             @ delta
    vneg.s8         d29, d28                @ -tc
    vabs.s8         d12, d10                @ ABS(delta)
    vmax.s8         d11, d10, d29           @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d12, d22           @ ABS(delta) < 10*tc
    vmin.s8         d11, d11, d28           @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d11, d11, d13             @ mask for p0 q0
    vshr.u8         d30, d28, #1             @ tc_2
    vneg.s8         d29, d30                 @ -tc_2
    vand.s8         d24, d24, d13           @ mask for p1
    vand.s8         d25, d25, d13           @ mask for q1
    vrhadd.u8       d17, d1, d3             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d4, d6            @ (q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d2            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d5           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d11            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d11            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d29            @ min
    vmax.s8         d18, d18, d29            @ min 
    vmin.s8         d17, d17, d30           @ deltap
    vmin.s8         d18, d18, d30           @ deltaq
    vand.s8         d17, d17, d24
    vand.s8         d18, d18, d25
    vmovl.s8	    q10, d11					@ delta
    vaddw.u8	    q11, q10, d3		    @ p0+delta 
    vqmovun.s16	    d3, q11
    vmovl.u8	    q10, d4					@ q0	
    vsubw.s8	    q11, q10, d11		    @ q0 - delta
    vqmovun.s16	    d4, q11
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d2		    @ p1 + deltap 
    vqmovun.s16	    d2, q11
    vmovl.s8	    q10, d18				@ deltaq
    vaddw.u8	    q11, q10, d5		    @ q1 + deltaq 
    vqmovun.s16	    d5, q11
    b               LumaCross_Store
LumaCross_Store:
    sub             r7, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r7, #4              @ piSrc[-4 * iStride-4]
    vst1.64         {d0}, [r8], r1          @ p3
    vst1.64         {d1}, [r8], r1          @ p2
    vst1.64         {d2}, [r8], r1          @ p1
    vst1.64         {d3}, [r8], r1          @ p0
    vst1.64         {d4}, [r8], r1          @ q3
    vst1.64         {d5}, [r8], r1          @ q2
    vst1.64         {d6}, [r8], r1          @ q1
    vst1.64         {d7}, [r8], r1          @ q0
    ldmfd           r13!, {r4 - r10, r15}

voChromaFilterCross_ASM:
@r0 src
@r1 stride
@r2 ptc
    stmfd           r13!, {r4 - r9, r14}
    vdup.u32        d0, r2
    vmov.u8         r3, d0[0]               @tc0
    vmov.u8         r4, d0[1]               @tc1
    vmov.u8         r5, d0[2]               @tc2
    vmov.u8         r6, d0[3]               @tc3
    adds            r7, r3, r4
    cmp             r7, #0
    beq             ChromaCross_Load
ChromaCross_Ver:
    sub             r7, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r7, #4              @ piSrc[-4 * iStride-4] 
    vld4.8		    {d0[0],d1[0],d2[0],d3[0]}, [r8], r1
    vld4.8		    {d4[0],d5[0],d6[0],d7[0]}, [r7], r1
    vld4.8		    {d0[1],d1[1],d2[1],d3[1]}, [r8], r1
    vld4.8		    {d4[1],d5[1],d6[1],d7[1]}, [r7], r1
    vld4.8		    {d0[2],d1[2],d2[2],d3[2]}, [r8], r1
    vld4.8		    {d4[2],d5[2],d6[2],d7[2]}, [r7], r1
    vld4.8		    {d0[3],d1[3],d2[3],d3[3]}, [r8], r1
    vld4.8		    {d4[3],d5[3],d6[3],d7[3]}, [r7], r1
    vld4.8		    {d0[4],d1[4],d2[4],d3[4]}, [r8], r1
    vld4.8		    {d4[4],d5[4],d6[4],d7[4]}, [r7], r1
    vld4.8		    {d0[5],d1[5],d2[5],d3[5]}, [r8], r1
    vld4.8		    {d4[5],d5[5],d6[5],d7[5]}, [r7], r1
    vld4.8		    {d0[6],d1[6],d2[6],d3[6]}, [r8], r1
    vld4.8		    {d4[6],d5[6],d6[6],d7[6]}, [r7], r1
    vld4.8		    {d0[7],d1[7],d2[7],d3[7]}, [r8], r1
    vld4.8		    {d4[7],d5[7],d6[7],d7[7]}, [r7], r1

    vdup.u8         d10, r3                 @ tc0
    vdup.u8         d11, r4                 @ tc1
    vtrn.32         d10, d11
    vneg.s8         d11, d10                @ -tc0 -tc1
    vsubl.u8        q6, d4, d3              @ q0-p0
    vsubl.u8        q7, d2, d5              @ p1-q1
    vshl.s16        q6, q6, #2              @ (q0-p0)<<2
    vadd.s16        q6, q7, q6              @ (q0-p0)<<2+p1-q1
    vrshrn.s16      d12, q6, #3             @ ((q0-p0)<<2+p1-q1+4)>>3
    vmax.s8         d12, d12, d11 
    vmin.s8         d12, d12, d10           @ delta
    vmovl.s8	    q7, d12					@ delta
    vaddw.u8	    q7, q7, d3		        @ p0+delta 
    vqmovun.s16	    d3, q7                 @ Clip(p0+delta)
    vmovl.u8	    q8, d4					@ q0	
    vsubw.s8	    q8, q8, d12		        @ q0-delta
    vqmovun.s16	    d4, q8                  @ Clip(q0-delta)   
    b               ChromaCross_Hor
ChromaCross_Load:
    sub             r7, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r7, #4              @ piSrc[-4 * iStride-4]
    vld1.64         {d0}, [r8], r1          @ p3
    vld1.64         {d1}, [r8], r1          @ p2
    vld1.64         {d2}, [r8], r1          @ p1
    vld1.64         {d3}, [r8], r1          @ p0
    vld1.64         {d4}, [r8], r1          @ q3
    vld1.64         {d5}, [r8], r1          @ q2
    vld1.64         {d6}, [r8], r1          @ q1
    vld1.64         {d7}, [r8], r1          @ q0
    b               ChromaCross_Hor_Calc
ChromaCross_Hor:
    vtrn.8          d0, d1      @t00 t10 t02 t12 t04 t14 t06 t16     t01 t11 t03 t13 t05 t15 t07 t17
    vtrn.8          d2, d3      @t20 t30 t22 t32 t24 t34 t26 t36     t21 t31 t23 t33 t25 t35 t27 t37
    vtrn.16         d0, d2      @t00 t10 t20 t30 t04 t14 t24 t34     t02 t12 t22 t32 t06 t16 t26 t36
    vtrn.8          d4, d5      @t40 t50 t42 t52 t44 t54 t46 t56     t41 t51 t43 t53 t45 t55 t47 t57
    vtrn.8          d6, d7      @t60 t70 t62 t72 t64 t74 t66 t76     t61 t71 t63 t73 t65 t75 t67 t77 
    vtrn.16         d1, d3      @t01 t11 t21 t31 t05 t15 t25 t35     t03 t13 t23 t33 t05 t15 t25 t35
    vtrn.16         d4, d6      @t40 t50 t60 t70 t44 t54 t64 t74     t42 t52 t62 t72 t46 t56 t66 t76
    vtrn.16         d5, d7      @t41 t51 t61 t71 t45 t55 t65 t75     t43 t53 t63 t73 t47 t57 t67 t77
        
    vtrn.32         d0, d4      @ t00 t10 t20 t30 t40 t50 t60 t70    t04 t14 t24 t34 t44 t54 t64 t74
    vtrn.32         d1, d5      @ t01 t11 t21 t31 t41 t51 t61 t71    t05 t15 t25 t35 t45 t55 t65 t75  
    vtrn.32         d2, d6      @ t02 t12 t22 t32 t42 t52 t62 t72    t06 t16 t26 t36 t46 t56 t66 t76 
    vtrn.32         d3, d7      @ t03 t13 t23 t33 t43 t53 t63 t73    t07 t17 t27 t37 t47 t57 t67 t77
    adds            r7, r5, r6
    cmp             r7, #0
    beq             ChromaCross_Store
ChromaCross_Hor_Calc:
    vdup.u8         d10, r5                 @ tc0
    vdup.u8         d11, r6                 @ tc1
    vtrn.32         d10, d11
    vneg.s8         d11, d10                @ -tc0 -tc1
    vsubl.u8        q6, d4, d3              @ q0-p0
    vsubl.u8        q7, d2, d5              @ p1-q1
    vshl.s16        q6, q6, #2              @ (q0-p0)<<2
    vadd.s16        q6, q7, q6              @ (q0-p0)<<2+p1-q1
    vrshrn.s16      d12, q6, #3             @ ((q0-p0)<<2+p1-q1+4)>>3
    vmax.s8         d12, d12, d11 
    vmin.s8         d12, d12, d10           @ delta
    vmovl.s8	    q7, d12					@ delta
    vaddw.u8	    q7, q7, d3		        @ p0+delta 
    vqmovun.s16	    d3, q7                  @ Clip(p0+delta)
    vmovl.u8	    q8, d4					@ q0	
    vsubw.s8	    q8, q8, d12		        @ q0-delta
    vqmovun.s16	    d4, q8                  @ Clip(q0-delta) 
ChromaCross_Store:
    sub             r7, r0, r1, lsl #2      @ piSrc[-4 * iStride]  
    sub             r8, r7, #4              @ piSrc[-4 * iStride-4]
    vst1.64         {d0}, [r8], r1          @ p3
    vst1.64         {d1}, [r8], r1          @ p2
    vst1.64         {d2}, [r8], r1          @ p1
    vst1.64         {d3}, [r8], r1          @ p0
    vst1.64         {d4}, [r8], r1          @ q3
    vst1.64         {d5}, [r8], r1          @ q2
    vst1.64         {d6}, [r8], r1          @ q1
    vst1.64         {d7}, [r8], r1          @ q0
    ldmfd           r13!, {r4 - r9, r15}
  

        
voChromaFilterVer_ASM: 
@ r0 src
@ r1 stride
@ r2 tc
    stmfd           r13!, {r4 - r9, r14}
    sub             r4, r0, #2               @ piSrc[-2]
    vld4.8		    {d0[0],d1[0],d2[0],d3[0]}, [r4], r1   @ p1
    vld4.8		    {d0[1],d1[1],d2[1],d3[1]}, [r4], r1   @ p0
    vld4.8		    {d0[2],d1[2],d2[2],d3[2]}, [r4], r1   @ q0
    vld4.8		    {d0[3],d1[3],d2[3],d3[3]}, [r4], r1   @ q1
    @ delta = Clip3(-iTc,iTc, (((( q0 - p0 ) << 2 ) + p1 - q1 + 4 ) >> 3) )@
    vdup.u8         d8, r2                  @ tc
    vneg.s8         d9, d8                  @ -tc
    vsubl.u8        q2, d2, d1              @ q0-p0
    vsubl.u8        q3, d0, d3              @ p1-q1
    vshl.s16        d4, d4, #2              @ (q0-p0)<<2
    vadd.s16        d4, d6, d4              @ (q0-p0)<<2+p1-q1
    vrshrn.s16      d4, q2, #3              @ ((q0-p0)<<2+p1-q1+4)>>3
    vmax.s8         d4, d4, d9 
    vmin.s8         d4, d4, d8              @ delta
    vmovl.s8	    q3, d4					@ delta
    vaddw.u8	    q4, q3, d1		        @ p0+delta 
    vqmovun.s16	    d1, q4                 @ Clip(p0+delta)
    vmovl.u8	    q4, d2					@ q0	
    vsubw.s8	    q4, q4, d4		        @ q0-delta
    vqmovun.s16	    d2, q4                  @ Clip(q0-delta)
    sub             r4, r0, #1              @ piSrc[-1]
    vst2.8		    {d1[0],d2[0]}, [r4], r1   @ p0
    vst2.8		    {d1[1],d2[1]}, [r4], r1   @ q0
    vst2.8		    {d1[2],d2[2]}, [r4], r1   @ p0
    vst2.8		    {d1[3],d2[3]}, [r4], r1   @ q0
    ldmfd           r13!, {r4 - r9, r15}

        
voChromaFilterHor_ASM: 
@ r0 src
@ r1 stride
@ r2 tc
    stmfd           r13!, {r4 - r9, r14}
    sub             r4, r0, r1, lsl #1      @ piSrc[-2 * iStride]
    vld1.32         {d0[0]}, [r4], r1       @ p1
    vld1.32         {d1[0]}, [r4], r1       @ p0
    vld1.32         {d2[0]}, [r4], r1       @ q0
    vld1.32         {d3[0]}, [r4], r1       @ q1
    @ delta = Clip3(-iTc,iTc, (((( q0 - p0 ) << 2 ) + p1 - q1 + 4 ) >> 3) )@
    vdup.u8         d8, r2                  @ tc
    vneg.s8         d9, d8                  @ -tc
    vsubl.u8        q2, d2, d1              @ q0-p0
    vsubl.u8        q3, d0, d3              @ p1-q1
    vshl.s16        d4, d4, #2              @ (q0-p0)<<2
    vadd.s16        d4, d6, d4              @ (q0-p0)<<2+p1-q1
    vrshrn.s16      d4, q2, #3              @ ((q0-p0)<<2+p1-q1+4)>>3
    vmax.s8         d4, d4, d9 
    vmin.s8         d4, d4, d8              @ delta
    vmovl.s8	    q3, d4					@ delta
    vaddw.u8	    q4, q3, d1		        @ p0+delta 
    vqmovun.s16	    d1, q4                 @ Clip(p0+delta)
    vmovl.u8	    q4, d2					@ q0	
    vsubw.s8	    q4, q4, d4		        @ q0-delta
    vqmovun.s16	    d2, q4                  @ Clip(q0-delta)
    sub             r4, r0, r1              @ piSrc[-iStride]
    vst1.32         {d1[0]}, [r4], r1       @ p0
    vst1.32         {d2[0]}, [r4], r1       @ q0
    ldmfd           r13!, {r4 - r9, r15}

    
voLumaFilterVer_ASM:
@r0 src
@r1 stride
@r2 beta
@r3 tc
    stmfd           r13!, {r4 - r9, r14}
    sub             r4, r0, #4                @ src[-4]
    mov             r5, r0                    @ src[0]
    vld4.8		    {d0[0],d1[0],d2[0],d3[0]}, [r4], r1
    vld4.8		    {d4[0],d5[0],d6[0],d7[0]}, [r5], r1
    vld4.8		    {d0[1],d1[1],d2[1],d3[1]}, [r4], r1
    vld4.8		    {d4[1],d5[1],d6[1],d7[1]}, [r5], r1
    vld4.8		    {d0[2],d1[2],d2[2],d3[2]}, [r4], r1
    vld4.8		    {d4[2],d5[2],d6[2],d7[2]}, [r5], r1
    vld4.8		    {d0[3],d1[3],d2[3],d3[3]}, [r4], r1
    vld4.8		    {d4[3],d5[3],d6[3],d7[3]}, [r5], r1

    vshll.u8	    q4, d2, #1              @ 2*p1
    vshll.u8	    q5, d5, #1              @ 2*q1
    vaddl.u8        q6, d1, d3              @ p2+p0
    vaddl.u8        q7, d6, d4              @ q2+q0
    vabd.u16		d8, d12, d8             @ dp0-3
    vabd.u16		d9, d10, d14            @ dq0-3
    vadd.u16        d10, d8, d9             @ d0-3
    vmov.u16        r5, d10[0]              @ d0
    vmov.u16        r6, d10[3]              @ d3
    add             r6, r6, r5              @ d0+d3
    subs            r6, r6, r2              @ (d0+d3)<beta
@    bge             LumaVer_End
    ldmgefd           r13!, {r4 - r9, r15}
@   if(ABS(src[-4] -  src[-1]) + ABS( src[3] -  src[0]) < (iBeta>>3) && 
@	   ABS(src[-1] -  src[0]) < tc_s &&
@      ABS(src[stride*3-4] - src[stride*3-1]) + ABS(src[stride*3+3] - src[stride*3]) < (iBeta>>3) && 
@      ABS(src[stride*3-1] - src[stride*3]) < tc_s &&
@      (d0 << 1) < (iBeta>>2) && (d3 << 1) < (iBeta>>2)) 
    add             r7,  r3,  r3,  lsl #2   @ 5*tc
    add             r7,  #1                 @ 5*tc+1 tc_s
    asr             r7,  r7,  #1            @ (5*tc+1)>>1
    vdup.16		    d11, r2				    @ beta
    vshr.u16        d12, d11 , #2           @ beta>>2
    vshr.u16        d13, d11 , #3           @ beta>>3
    vdup.16		    d14, r7				    @ tc_s  
    vshl.u16        d10, d10 , #1           @ d0-3<<1
    vclt.u16		d10, d10, d12			@ (d0-3<<1)<(beta>>2)    !1
    vabdl.u8		q8, d0, d3              @ abs(p3-p0)
    vabdl.u8		q9, d7, d4              @ abs(q3-q0)
    vabdl.u8		q10, d3, d4             @ abs(p0-q0)
    vadd.u16		d15, d16, d18			@ abs(p3-p0)+abs(q3-q0) 
    vclt.u16		d15, d15, d13			@ abs(p3-p0)+abs(q3-q0)<(beta>>3) !2
    vclt.u16		d20, d20, d14			@ abs(p0-q0)<tc_s !3
    vand.u16        d10, d10, d15           @ !1&&!2
    vand.u16        d10, d10, d20           @ !1&&!2&&!3
    vmov.s16        r5, d10[0]              @ d0
    vmov.s16        r6, d10[3]              @ d3
    add             r6, r6, r5              @ d0+d3
    cmp             r6, #-1
    blt             LumaVer_Strong
LumaVer_Weak:
    vmov.u16        r5, d8[0]               @ dp0
    vmov.u16        r6, d8[3]               @ dp3
    add             r6, r6, r5              @ dp0+dp3
    vmov.u16        r5, d9[0]               @ dq0
    vmov.u16        r7, d9[3]               @ dq3
    add             r7, r7, r5              @ dq0+dq3
    add             r8, r2, r2,   lsr#1     @ beta+(beta>>1)
    lsr             r8, r8, #3              @(beta+(beta>>1))>>3
    cmp             r6, r8
    movlt           r6, #-1
    movge           r6, #0
    cmp             r7, r8
    movlt           r7, #-1
    movge           r7, #0 
    @delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4@
@d0 p3   @d7 q3
@d1 p2   @d6 q2
@d2 p1   @d5 q1
@d3 p0   @d4 q0 
    vsubl.u8        q4, d4, d3              @ q0 - p0
    vsubl.u8        q5, d5, d2              @ q1 - p1
    vshl.s16        q6, q4, #3              @ 8*(q0 - p0)
    vadd.s16        q6, q6, q4              @ 9*(q0 - p0)
    vshl.s16        q7, q5, #1              @ 2*(q1 - p1)
    vadd.s16        q7, q7, q5              @ 3*(q1 - p1)
    vsub.s16        q5, q6, q7              @ 9*(q0 - p0) - 3*(q1 - p1)
    vdup.u8         d8, r3                  @ tc
    vshl.u8         d9, d8, #3              @ tc*8
    vadd.u8         d9, d9, d8
    vadd.u8         d9, d9, d8              @ 10*tc
    vrshrn.s16      d10, q5, #4             @ delta
    vneg.s8         d11, d8                 @ -tc
    vabs.s8         d12, d10                @ ABS(delta)
    vmax.s8         d11, d10, d11           @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d12, d9            @ ABS(delta) < 10*tc
    vmin.s8         d11, d11,  d8           @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d11, d11, d13             @ mask for p0 q0
    vshr.u8         d24, d8, #1             @ tc_2
    vneg.s8         d25, d24                 @ -tc_2
    vdup.u8         d16, r6                 @ dp0 + dp3 < ((iBeta+(iBeta>>1))>>3)
    vdup.u8         d15, r7                 @ dq0 + dq3 < ((iBeta+(iBeta>>1))>>3)
    vand.s8         d16, d16, d13           @ mask for p1
    vand.s8         d15, d15, d13           @ mask for q1
    vrhadd.u8       d17, d1, d3             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d4, d6            @(q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d2            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d5           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d11            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d11            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d25            @ min
    vmax.s8         d18, d18, d25            @ min 
    vmin.s8         d17, d17, d24           @ deltap
    vmin.s8         d18, d18, d24           @ deltaq
    vand.s8         d17, d17, d16
    vand.s8         d18, d18, d15
    vmovl.s8	    q10, d11					@ delta
    vaddw.u8	    q11, q10, d3		    @ p0+delta 
    vqmovun.s16	    d3, q11
    vmovl.u8	    q10, d4					@ q0	
    vsubw.s8	    q11, q10, d11		    @ q0 - delta
    vqmovun.s16	    d4, q11
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d2		    @ p1 + deltap 
    vqmovun.s16	    d2, q11
    vmovl.s8	    q10, d18				@deltaq
    vaddw.u8	    q11, q10, d5		    @ q1 + deltaq 
    vqmovun.s16	    d5, q11
    sub             r4, r0, #2
    vst4.8          {d2[0], d3[0], d4[0], d5[0]}, [r4], r1
    vst4.8          {d2[1], d3[1], d4[1], d5[1]}, [r4], r1
    vst4.8          {d2[2], d3[2], d4[2], d5[2]}, [r4], r1
    vst4.8          {d2[3], d3[3], d4[3], d5[3]}, [r4], r1
@    b               LumaVer_End
    ldmfd           r13!, {r4 - r9, r15}
LumaVer_Strong:
@d0 p3   @d7 q3
@d1 p2   @d6 q2
@d2 p1   @d5 q1
@d3 p0   @d4 q0 
    vdup.u8         d8, r3                 @ tc
    vshl.u8         d8, d8, #1             @ tc2   
    vaddl.u8        q5, d1, d2             @ p2 + p1 
    vaddl.u8        q6, d3, d4             @ p0 + q0
    vaddl.u8        q7, d0, d1             @ p3 + p2
    vadd.u16        d24, d10, d12          @ p2 + p1 + p0 + q0 
    vadd.u16        d26, d24, d14          @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        d26, d26, d14          @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q5, d2, d3             @ p1 + p0
    vaddl.u8        q7, d4, d5             @ q0 + q1
    vadd.u16        d28, d24, d10          @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        d28, d28, d14          @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q8, d6, d5             @ q2 + q1
    vaddl.u8        q10, d7, d6            @ q3 + q2
    vadd.u16        d30, d16, d12          @ q2 + q1 + p0 + q0
    vadd.u16        d16, d30, d20          @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        d16, d16, d20          @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        d12, d30, d14          @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        d12, d12, d10          @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d20, d2, d8            @ p1 - tc2
    vqadd.u8        d21, d2, d8            @ p1 + tc2
    vrshrn.u16      d24, q12, #2           @ ( p2 + p1 + p0 + q0 + 2 ) >> 2
    vmax.u8         d2, d24, d20           @ 
    vmin.u8         d2, d2, d21            @ p1
    vqsub.u8        d20, d1, d8            @ p2 - tc2
    vqadd.u8        d21, d1, d8            @ p2 + tc2
    vrshrn.u16      d26, q13, #3           @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d1, d26, d20           @ 
    vmin.u8         d1, d1, d21            @ p2
    vqsub.u8        d20, d3, d8            @ p0 - tc2
    vqadd.u8        d21, d3, d8            @ p0 + tc2
    vrshrn.u16      d28, q14, #3           @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d3, d28, d20           @ 
    vmin.u8         d3, d3, d21            @ p0
    vqsub.u8        d20, d5, d8            @ q1 - tc2
    vqadd.u8        d21, d5, d8            @ q1 + tc2
    vrshrn.u16      d30, q15, #2           @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d5, d30, d20           @ 
    vmin.u8         d5, d5, d21            @ q1
    vqsub.u8        d20, d6, d8            @ q2 - tc2
    vqadd.u8        d21, d6, d8            @ q2 + tc2
    vrshrn.u16      d10, q8, #3            @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d6, d10, d20           @ 
    vmin.u8         d6, d6, d21            @ q2
    vqsub.u8        d20, d4, d8            @ q0 - tc2
    vqadd.u8        d21, d4, d8            @ q0 + tc2
    vrshrn.u16      d12, q6, #3            @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d4, d12, d20           @ 
    vmin.u8         d4, d4, d21            @ q0
    sub             r4, r0, #4             @ piSrc[-2 * iStride]
    mov             r5, r0                 @ src[0]
    vst4.8		    {d0[0],d1[0],d2[0],d3[0]}, [r4], r1
    vst4.8		    {d4[0],d5[0],d6[0],d7[0]}, [r5], r1
    vst4.8		    {d0[1],d1[1],d2[1],d3[1]}, [r4], r1
    vst4.8		    {d4[1],d5[1],d6[1],d7[1]}, [r5], r1
    vst4.8		    {d0[2],d1[2],d2[2],d3[2]}, [r4], r1
    vst4.8		    {d4[2],d5[2],d6[2],d7[2]}, [r5], r1
    vst4.8		    {d0[3],d1[3],d2[3],d3[3]}, [r4], r1
    vst4.8		    {d4[3],d5[3],d6[3],d7[3]}, [r5], r1
LumaVer_End:    
    ldmfd           r13!, {r4 - r9, r15}

voLumaFilterHor_ASM:
@r0 src
@r1 stride
@r2 beta
@r3 tc
    stmfd           r13!, {r4 - r9, r14}
    sub             r4, r0, r1, lsl #1      @ piSrc[-2 * iStride]
    sub             r4, r4, r1              @ piSrc[-3 * iStride]
    sub             r7, r4, r1              @ piSrc[-4 * iStride]
    vld1.32         {d2[0]}, [r4], r1       @ p2
    vld1.32         {d4[0]}, [r4], r1       @ p1
    vld1.32         {d6[0]}, [r4], r1       @ p0
    vld1.32         {d8[0]}, [r4], r1       @ q0
    vld1.32         {d10[0]}, [r4], r1      @ q1
    vld1.32         {d12[0]}, [r4], r1      @ q2
    
    vshll.u8	    q7, d4, #1              @ 2*p1
    vshll.u8	    q8, d10, #1             @ 2*q1
    vaddl.u8        q9, d2, d6              @ p2+p0
    vaddl.u8        q10, d12, d8            @ q2+q0
    vabd.u16		d1, d18, d14            @ dp0-3
    vabd.u16		d15, d20, d16           @ dq0-3
    vadd.u16        d17, d1, d15            @ d0-3
    vmov.u16        r5, d17[0]              @ d0
    vmov.u16        r6, d17[3]              @ d3
    add             r6, r6, r5              @ d0+d3
    subs            r6, r6, r2              @ (d0+d3)<beta
@    bge             LumaHor_End
    ldmgefd           r13!, {r4 - r9, r15}
    
    vld1.32         {d0[0]}, [r7], r1       @ p3
    vld1.32         {d14[0]}, [r4], r1      @ q3
@    if(ABS(src[-4*stride] -  src[-stride]) + ABS( src[3*stride] -  src[0]) < (iBeta>>3) && 
@		ABS(src[-stride] -  src[0]) < tc_s &&
@       ABS(src[3-4*stride] - src[3-stride]) + ABS(src[3+3*stride] - src[3]) < (iBeta>>3) && 
@       ABS(src[3-stride] - src[3]) < tc_s &&
@       (d0 << 1) < (iBeta>>2) && (d3 << 1) < (iBeta>>2)) //strong
    add             r7,  r3,  r3,  lsl #2   @ 5*tc
    add             r7,  #1                 @ 5*tc+1 tc_s
    asr             r7,  r7,  #1            @ (5*tc+1)>>1
    vdup.16		    d3, r2				    @ beta
    vshr.u16        d23, d3 , #2            @ beta>>2
    vshr.u16        d30, d3 , #3            @ beta>>3
    vdup.16		    d16, r7				    @ tc_s
    
    vshl.u16        d17, d17 , #1           @ d0-3<<1
    vclt.u16		d17, d17, d23			@ (d0-3<<1)<(beta>>2)    !1
    vabdl.u8		q12, d0, d6             @ abs(p3-p0)
    vabdl.u8		q13, d14, d8            @ abs(q3-q0)
    vabdl.u8		q14, d6, d8             @ abs(p0-q0)
    vadd.u16		d27, d24, d26			@ abs(p3-p0)+abs(q3-q0) 
    vclt.u16		d27, d27, d30			@ abs(p3-p0)+abs(q3-q0)<(beta>>3) !2
    vclt.u16		d28, d28, d16			@ abs(p0-q0)<tc_s !3
    vand.u16        d17, d17, d27           @ !1&&!2
    vand.u16        d17, d17, d28           @ !1&&!2&&!3
    vmov.s16        r5, d17[0]              @ d0
    vmov.s16        r6, d17[3]              @ d3
    add             r6, r6, r5              @ d0+d3
    cmp             r6, #-1
    blt             LumaHor_Strong
LumaHor_Weak:
    vmov.u16        r5, d1[0]               @ dp0
    vmov.u16        r6, d1[3]               @ dp3
    add             r6, r6, r5              @ dp0+dp3
    vmov.u16        r5, d15[0]              @ dq0
    vmov.u16        r7, d15[3]              @ dq3
    add             r7, r7, r5              @ dq0+dq3
    add             r8, r2, r2,   lsr#1     @ beta+(beta>>1)
    lsr             r8, r8, #3              @(beta+(beta>>1))>>3
    cmp             r6, r8
    movlt           r6, #-1
    movge           r6, #0
    cmp             r7, r8
    movlt           r7, #-1
    movge           r7, #0
    @delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4@
    vsubl.u8        q8, d8, d6              @ q0 - p0
    vsubl.u8        q9, d10, d4             @ q1 - p1
    vshl.s16        q10, q8, #3             @ 8*(q0 - p0)
    vadd.s16        q10, q10, q8            @ 9*(q0 - p0)
    vshl.s16        q11, q9, #1             @ 2*(q1 - p1)
    vadd.s16        q11, q11, q9            @ 3*(q1 - p1)
    vsub.s16        q10, q10, q11           @ 9*(q0 - p0) - 3*(q1 - p1)
    vdup.u8         d7, r3                  @ tc
    vshl.u8         d16, d7, #3             @ tc*8
    vadd.u8         d16, d16, d7
    vadd.u8         d16, d16, d7            @ 10*tc
    vrshrn.s16      d5, q10, #4             @ delta
    vneg.s8         d9, d7                  @ -tc
    vabs.s8         d11, d5                 @ ABS(delta)
    vmax.s8         d9, d5, d9              @ delta = Clip3(-tc, tc, delta)@  
    vclt.u8         d13, d11, d16           @ ABS(delta) < 10*tc
    vmin.s8         d9, d9,  d7             @ delta = Clip3(-tc, tc, delta)       
    vand.s8         d9, d9, d13             @ mask for p0 q0
    vshr.u8         d11, d7, #1             @ tc_2
    vneg.s8         d5, d11                 @ -tc_2
    vdup.u8         d1, r6                  @ dp0 + dp3 < ((iBeta+(iBeta>>1))>>3)
    vdup.u8         d15, r7                 @ dq0 + dq3 < ((iBeta+(iBeta>>1))>>3)
    vand.s8         d1, d1, d13             @ mask for p1
    vand.s8         d15, d15, d13           @ mask for q1
    vrhadd.u8       d17, d2, d6             @ (p2 + p0 + 1) >> 1)
    vrhadd.u8       d18, d8, d12            @(q2 + q0 + 1) >> 1)
    vsubl.u8        q10, d17, d4            @ (p2 + p0 + 1) >> 1) - p1
    vsubl.u8        q11, d18, d10           @ (q2 + q0 + 1) >> 1) - q1
    vaddw.s8        q10, q10, d9            @ (((p2 + p0 + 1) >> 1) - p1 + delta)
    vshrn.s16       d17, q10, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vsubw.s8        q11, q11, d9            @ (((q2 + q0 + 1) >> 1) - q1 - delta) 
    vshrn.s16       d18, q11, #1            @ (((p2 + p0 + 1) >> 1) - p1 + delta) >> 1
    vmax.s8         d17, d17, d5            @ min
    vmax.s8         d18, d18, d5            @ min 
    vmin.s8         d17, d17, d11           @ deltap
    vmin.s8         d18, d18, d11           @ deltaq
    vand.s8         d17, d17, d1
    vand.s8         d18, d18, d15
    vmovl.s8	    q10, d9					@ delta
    vaddw.u8	    q11, q10, d6		    @ p0+delta 
    vqmovun.s16	    d6, q11
    vmovl.u8	    q10, d8					@ q0	
    vsubw.s8	    q11, q10, d9		    @ q0 - delta
    vqmovun.s16	    d8, q11
    vmovl.s8	    q10, d17				@ deltap
    vaddw.u8	    q11, q10, d4		    @ p1 + deltap 
    vqmovun.s16	    d4, q11
    vmovl.s8	    q10, d18				@deltaq
    vaddw.u8	    q11, q10, d10		    @ q1 + deltaq 
    vqmovun.s16	    d10, q11
    sub             r4, r0, r1, lsl #1
    vst1.32         {d4[0]}, [r4], r1
    vst1.32         {d6[0]}, [r4], r1
    vst1.32         {d8[0]}, [r4], r1
    vst1.32         {d10[0]}, [r4], r1
@    b               LumaHor_End
    ldmfd           r13!, {r4 - r9, r15}
LumaHor_Strong:
@src[-1] = Clip3( p0-tc2, p0+tc2,( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3)@
@src[-2] = Clip3( p1-tc2, p1+tc2,( p2 + p1 + p0 + q0 + 2 ) >> 2)@
@src[-3] = Clip3( p2-tc2, p2+tc2,( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3)@
@src[0] = Clip3( q0-tc2, q0+tc2,( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3)@
@src[1] = Clip3( q1-tc2, q1+tc2,( p0 + q0 + q1 + q2 + 2 ) >> 2)@
@src[2] = Clip3( q2-tc2, q2+tc2,( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3)@
    vdup.u8         d13, r3                @ tc
    vshl.u8         d13, d13, #1           @ tc2   
    vaddl.u8        q8, d2, d4             @ p2 + p1 
    vaddl.u8        q9, d6, d8             @ p0 + q0
    vaddl.u8        q10, d0, d2            @ p3 + p2
    vadd.u16        d22, d16, d18          @ p2 + p1 + p0 + q0 
    vadd.u16        d24, d22, d20          @ p3 + 2*p2 + p1 + p0 + q0
    vadd.u16        d24, d24, d20           @ 2*p3 + 3*p2 + p1 + p0 + q0
    vaddl.u8        q8, d4, d6            @ p1 + p0
    vaddl.u8        q13, d8, d10           @ q0 + q1
    vadd.u16        d28, d22, d16            @ p2 + 2*p1 + 2*p0 + q0
    vadd.u16        d28, d28, d26            @ p2 + 2*p1 + 2*p0 + 2*q0 + q1
    vaddl.u8        q15, d12, d10           @ q2 + q1
    vaddl.u8        q10, d14, d12          @ q3 + q2
    vadd.u16        d30, d30, d18          @ q2 + q1 + p0 + q0
    vadd.u16        d18, d30, d20           @ q3 + 2*q2 + q1 + p0 + q0
    vadd.u16        d18, d18, d20            @ 2*q3 + 3*q2 + q1 + p0 + q0
    vadd.u16        d20, d30, d26           @ q2 + 2*q1+ 2*q0 + p0
    vadd.u16        d20, d20, d16          @ q2 + 2*q1+ 2*q0 + 2*p0 + p1
    vqsub.u8        d1, d4, d13           @ p1 - tc2
    vqadd.u8        d3, d4, d13           @ p1 + tc2
    vrshrn.u16      d5, q11, #2             @ ( p2 + p1 + p0 + q0 + 2 ) >> 2
    vmax.u8         d4, d5, d1            @ 
    vmin.u8         d4, d4, d3            @ p1
    vqsub.u8        d1, d2, d13           @ p2 - tc2
    vqadd.u8        d3, d2, d13           @ p2 + tc2
    vrshrn.u16      d5, q12, #3             @ ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3
    vmax.u8         d2, d5, d1            @ 
    vmin.u8         d2, d2, d3            @ p2
    vqsub.u8        d1, d6, d13           @ p0 - tc2
    vqadd.u8        d3, d6, d13           @ p0 + tc2
    vrshrn.u16      d5, q14, #3             @ ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3
    vmax.u8         d6, d5, d1            @ 
    vmin.u8         d6, d6, d3            @ p0
    vqsub.u8        d1, d10, d13          @ q1 - tc2
    vqadd.u8        d3, d10, d13          @ q1 + tc2
    vrshrn.u16      d7, q15, #2             @ ( p0 + q0 + q1 + q2 + 2 ) >> 2
    vmax.u8         d10, d7, d1           @ 
    vmin.u8         d10, d10, d3          @ q1
    vqsub.u8        d1, d12, d13          @ q2 - tc2
    vqadd.u8        d3, d12, d13          @ q2 + tc2
    vrshrn.u16      d9, q9, #3             @ ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3
    vmax.u8         d12, d9, d1           @ 
    vmin.u8         d12, d12, d3          @ q2
    vqsub.u8        d1, d8, d13           @ q0 - tc2
    vqadd.u8        d3, d8, d13           @ q0 + tc2
    vrshrn.u16      d11, q10, #3            @ ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3
    vmax.u8         d8, d11, d1           @ 
    vmin.u8         d8, d8, d3            @ q0
    sub             r4, r0, r1, lsl #1     @ piSrc[-2 * iStride]
    sub             r4, r4, r1             @ piSrc[-3 * iStride]
    vst1.32         {d2[0]}, [r4], r1
    vst1.32         {d4[0]}, [r4], r1
    vst1.32         {d6[0]}, [r4], r1
    vst1.32         {d8[0]}, [r4], r1
    vst1.32         {d10[0]}, [r4], r1
    vst1.32         {d12[0]}, [r4], r1
LumaHor_End:    
    ldmfd           r13!, {r4 - r9, r15}

        .endif          @.if DEBLOCK_ASM_ENABLED==1
        .end
        @endfunc 
        @end


