# 1 "/mnt/hgfs/yurenjie/Developer/Numen/trunk/Codec/Video/Encoder/h264/src/arm_mac/armv7/voH264HPel_ARMV7_gnu.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/mnt/hgfs/yurenjie/Developer/Numen/trunk/Codec/Video/Encoder/h264/src/arm_mac/armv7/voH264HPel_ARMV7_gnu.S"
@*****************************************************************************
@* *
@* VisualOn, Inc. Confidential and Proprietary, 2010 *
@* *
@*****************************************************************************

#include "../../voH264EncID.h"
@.include "..\\..\\voH264EncID.h"
    @AREA |.text|, CODE

.text

.globl _HpelFilter16X16_V_ARMV7
.globl _HpelFilter16X16_H_ARMV7
.globl _HpelFilter16X16_C_ARMV7
.globl _Hpel16x16SearchFilter_HV_ARMV7

 .align 4
# 74 "/mnt/hgfs/yurenjie/Developer/Numen/trunk/Codec/Video/Encoder/h264/src/arm_mac/armv7/voH264HPel_ARMV7_gnu.S"
 .macro HALF16X16_H_ROW @r, sd
  vld1.64 {q0}, [r2], r3
  vld1.64 {q13}, [r12], r3
  vext.8 q1, q0, q13, #1
  vext.8 q2, q0, q13, #2
  vext.8 q3, q0, q13, #3
  vext.8 q4, q0, q13, #4
  vext.8 q5, q0, q13, #5
  vaddl.u8 q10, d4, d6
  vaddl.u8 q11, d5, d7
  vaddl.u8 q6, d0, d10
  vaddl.u8 q7, d1, d11
  vaddl.u8 q8, d2, d8
  vaddl.u8 q9, d3, d9
  vmla.s16 q6, q10, q15
  vmla.s16 q7, q11, q15
  vmls.s16 q6, q8, q14
  vmls.s16 q7, q9, q14
  vqrshrun.s16 d24, q6, #5
  vqrshrun.s16 d25, q7, #5
  vst1.64 {q12}, [$0], $1
 .endmacro

 .macro HALF16X16_HV_EXTRAH @r, sd
  vext.8 d4, d26, d27, #1
  vext.8 d6, d26, d27, #2
  vext.8 d8, d26, d27, #3
  vext.8 d10, d26, d27, #4
  vext.8 d12, d26, d27, #5
  vaddl.u8 q10, d6, d8
  vaddl.u8 q7, d26, d12
  vaddl.u8 q8, d4, d10
  vmla.s16 q7, q10, q15
  vmls.s16 q7, q8, q14
  vqrshrun.s16 d24, q7, #5
  vst1.64 {d24}, [$0], $1
 .endmacro


_HpelFilter16X16_V_ARMV7:

 sub r2, r2, r3, LSL #1
 vld1.64 {q0}, [r2], r3
 add r12, r2, r3, LSL #2
 vld1.64 {q1}, [r2], r3
 vmov.i16 q14, #5
 vld1.64 {q5}, [r12], r3
 vaddl.u8 q6, d0, d10
 vld1.64 {q2}, [r2], r3
 vaddl.u8 q7, d1, d11
 vld1.64 {q3}, [r2], r3
 vmov.i16 q15, #20
 vld1.64 {q4}, [r2], r3
 vaddl.u8 q8, d2, d8
 vaddl.u8 q9, d3, d9
 vaddl.u8 q10, d4, d6
 vaddl.u8 q11, d5, d7
 vmla.s16 q6, q10, q15
 vmla.s16 q7, q11, q15
 vmls.s16 q6, q8, q14
 vmls.s16 q7, q9, q14
 vqrshrun d24.u8, q6.s16, #5
 vqrshrun d25.u8, q7.s16, #5
 vst1.64 {q12}, [r0], r1

vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;

 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r0], r1 
;
 bx lr

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_HpelFilter16X16_H_ARMV7:

 sub r2, r2, #2
 add r12, r2, #16
 vmov.i16 q14, #5
 vmov.i16 q15, #20
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 HALF16X16_H_ROW r0, r1
 bx lr
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_HpelFilter16X16_C_ARMV7:

 push {r4 - r10, r14}
 sub r2, r2, r3, LSL #1
 sub r2, r2, #2
 add r5, r2, #8
 add r7, r0, #8
 vld1.64 {q0}, [r2], r3
 add r12, r2, r3, LSL #2
 vld1.64 {q1}, [r2], r3
 vmov.i16 q14, #5
 vld1.64 {q5}, [r12], r3
 vaddl.u8 q6, d0, d10
 vld1.64 {q2}, [r2], r3
 vaddl.u8 q7, d1, d11
 vld1.64 {q3}, [r2], r3
 vmov.i16 q15, #20
 vld1.64 {q4}, [r2], r3
 vaddl.u8 q8, d2, d8
 vaddl.u8 q9, d3, d9
 vaddl.u8 q10, d4, d6
 vaddl.u8 q11, d5, d7
 vmla.s16 q6, q10, q15
 vmla.s16 q7, q11, q15
 vmls.s16 q6, q8, q14
 vmls.s16 q7, q9, q14
 vext.8 q8, q6, q7, #2
 vext.8 q9, q6, q7, #4
 vext.8 q10, q6, q7, #6
 vext.8 q11, q6, q7, #8
 vext.8 q12, q6, q7, #10
 vaddl.s16 q7, d12, d24
 vaddl.s16 q0, d13, d25
 vaddl.s16 q6, d16, d22
 vaddl.s16 q12, d17, d23
 vaddl.s16 q8, d18, d20
 vaddl.s16 q11, d19, d21
 vmov.i32 q9, #5
 vmov.i32 q10, #20
 vmla.s32 q7, q8, q10
 vmla.s32 q0, q11, q10
 vmls.s32 q7, q6, q9
 vmls.s32 q0, q12, q9
 vqrshrun d20.u16, q7.s32, #10
 vqrshrun d21.u16, q0.s32, #10
 vqmovn d24.u8, q10.u16
 vst1.64 {d24}, [r0], r1
@row 2 0-7
    vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 3 0-7
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 4 0-7
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 5 0-7
 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q4, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q4, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q4, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q4, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1
;
@row 6 0-7
 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q5, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q5, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q5, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q5, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 7 0-7
 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q0, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q0, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q0, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q0, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 8 0-7
    vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 9 0-7
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 10 0-7
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 11 0-7
 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q4, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q4, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q4, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q4, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 12 0-7
 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q5, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q5, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q5, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q5, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 13 0-7
 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q0, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q0, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q0, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q0, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 14 0-7
    vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 15 0-7
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
@row 16 0-7
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r0], r1 
;
 vld1.64 {q0}, [r5], r3
 add r6, r5, r3, LSL #2
 vld1.64 {q1}, [r5], r3
 vmov.i16 q14, #5
 vld1.64 {q5}, [r6], r3
 vaddl.u8 q6, d0, d10
 vld1.64 {q2}, [r5], r3
 vaddl.u8 q7, d1, d11
 vld1.64 {q3}, [r5], r3
 vmov.i16 q15, #20
 vld1.64 {q4}, [r5], r3
 vaddl.u8 q8, d2, d8
 vaddl.u8 q9, d3, d9
 vaddl.u8 q10, d4, d6
 vaddl.u8 q11, d5, d7
 vmla.s16 q6, q10, q15
 vmla.s16 q7, q11, q15
 vmls.s16 q6, q8, q14
 vmls.s16 q7, q9, q14
 vext.8 q8, q6, q7, #2
 vext.8 q9, q6, q7, #4
 vext.8 q10, q6, q7, #6
 vext.8 q11, q6, q7, #8
 vext.8 q12, q6, q7, #10
 vaddl.s16 q7, d12, d24
 vaddl.s16 q0, d13, d25
 vaddl.s16 q6, d16, d22
 vaddl.s16 q12, d17, d23
 vaddl.s16 q8, d18, d20
 vaddl.s16 q11, d19, d21
 vmov.i32 q9, #5
 vmov.i32 q10, #20
 vmla.s32 q7, q8, q10
 vmla.s32 q0, q11, q10
 vmls.s32 q7, q6, q9
 vmls.s32 q0, q12, q9
 vqrshrun d20.u16, q7.s32, #10
 vqrshrun d21.u16, q0.s32, #10
 vqmovn d24.u8, q10.u16
 vst1.64 {d24}, [r7], r1
@row 2 8-15
    vld1.64 {q0}, [r6], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 3 8-15
 vld1.64 {q1}, [r6], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 4 8-15
 vld1.64 {q2}, [r6], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 5 8-15
 vld1.64 {q3}, [r6], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q4, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q4, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q4, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q4, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 6 8-15
 vld1.64 {q4}, [r6], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q5, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q5, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q5, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q5, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 7 8-15
 vld1.64 {q5}, [r6], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q0, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q0, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q0, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q0, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 8 8-15
    vld1.64 {q0}, [r6], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 9 8-15
 vld1.64 {q1}, [r6], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 10 8-15
 vld1.64 {q2}, [r6], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
@row 11 8-15
 vld1.64 {q3}, [r6], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q4, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q4, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q4, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q4, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 12 8-15
 vld1.64 {q4}, [r6], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q5, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q5, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q5, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q5, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 13 8-15
 vld1.64 {q5}, [r6], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q0, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q0, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q0, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q0, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 14 8-15
    vld1.64 {q0}, [r6], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q1, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q1, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q1, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q1, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1
;
@row 15 8-15
 vld1.64 {q1}, [r6], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q2, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q2, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q2, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q2, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1 
;
@row 16 8-15
 vld1.64 {q2}, [r6], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vext.8 q8, q6, q7, #2 
 vext.8 q9, q6, q7, #4 
 vext.8 q10, q6, q7, #6 
 vext.8 q11, q6, q7, #8 
 vext.8 q12, q6, q7, #10 
 vaddl.s16 q7, d12, d24 
 vaddl.s16 q3, d13, d25 
 vaddl.s16 q6, d16, d22 
 vaddl.s16 q12, d17, d23 
 vaddl.s16 q8, d18, d20 
 vaddl.s16 q11, d19, d21 
 vmov.i32 q9, #5 
 vmov.i32 q10, #20 
 vmla.s32 q7, q8, q10 
 vmla.s32 q3, q11, q10 
 vmls.s32 q7, q6, q9 
 vmls.s32 q3, q12, q9 
 vqrshrun.s32 d20, q7, #10 
 vqrshrun.s32 d21, q3, #10 
 vqmovn d24.u8, q10.u16 
 vst1.64 {d24}, [r7], r1
;
 pop {r4 - r10, pc}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_Hpel16x16SearchFilter_HV_ARMV7:
@r0 dsth
@r1 dstv
@r2 src
@r3 src_stride
 push {r4 - r10, r14}
 mov r5, r2
 mov r6, #32
 sub r2, r2, r3, LSL #1
 sub r2, r2, r3
 vld1.64 {q0}, [r2], r3
 add r12, r2, r3, LSL #2
 vld1.64 {q1}, [r2], r3
 vmov.i16 q14, #5
 vld1.64 {q5}, [r12], r3
 vaddl.u8 q6, d0, d10
 vld1.64 {q2}, [r2], r3
 vaddl.u8 q7, d1, d11
 vld1.64 {q3}, [r2], r3
 vmov.i16 q15, #20
 vld1.64 {q4}, [r2], r3
 vaddl.u8 q8, d2, d8
 vaddl.u8 q9, d3, d9
 vaddl.u8 q10, d4, d6
 vaddl.u8 q11, d5, d7
 vmla.s16 q6, q10, q15
 vmla.s16 q7, q11, q15
 vmls.s16 q6, q8, q14
 vmls.s16 q7, q9, q14
 vqrshrun d24.u8, q6.s16, #5
 vqrshrun d25.u8, q7.s16, #5
 vst1.64 {q12}, [r1], r6
@row 2
 vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 3
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 4
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 5
 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 6
 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 7
 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 8
 vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 9
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 10
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 11
 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 12
 vld1.64 {q4}, [r12], r3 
 vaddl.u8 q10, d2, d4 
 vaddl.u8 q11, d3, d5 
 vaddl.u8 q6, d10, d8 
 vaddl.u8 q7, d11, d9 
 vaddl.u8 q8, d0, d6 
 vaddl.u8 q9, d1, d7 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 13
 vld1.64 {q5}, [r12], r3 
 vaddl.u8 q10, d4, d6 
 vaddl.u8 q11, d5, d7 
 vaddl.u8 q6, d0, d10 
 vaddl.u8 q7, d1, d11 
 vaddl.u8 q8, d2, d8 
 vaddl.u8 q9, d3, d9 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 14
 vld1.64 {q0}, [r12], r3 
 vaddl.u8 q10, d6, d8 
 vaddl.u8 q11, d7, d9 
 vaddl.u8 q6, d2, d0 
 vaddl.u8 q7, d3, d1 
 vaddl.u8 q8, d4, d10 
 vaddl.u8 q9, d5, d11 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 15
 vld1.64 {q1}, [r12], r3 
 vaddl.u8 q10, d8, d10 
 vaddl.u8 q11, d9, d11 
 vaddl.u8 q6, d4, d2 
 vaddl.u8 q7, d5, d3 
 vaddl.u8 q8, d6, d0 
 vaddl.u8 q9, d7, d1 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 16
 vld1.64 {q2}, [r12], r3 
 vaddl.u8 q10, d10, d0 
 vaddl.u8 q11, d11, d1 
 vaddl.u8 q6, d6, d4 
 vaddl.u8 q7, d7, d5 
 vaddl.u8 q8, d8, d2 
 vaddl.u8 q9, d9, d3 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
@row 17
 vld1.64 {q3}, [r12], r3 
 vaddl.u8 q10, d0, d2 
 vaddl.u8 q11, d1, d3 
 vaddl.u8 q6, d8, d6 
 vaddl.u8 q7, d9, d7 
 vaddl.u8 q8, d10, d4 
 vaddl.u8 q9, d11, d5 
 vmla.s16 q6, q10, q15 
 vmla.s16 q7, q11, q15 
 vmls.s16 q6, q8, q14 
 vmls.s16 q7, q9, q14 
 vqrshrun.s16 d24, q6, #5 
 vqrshrun.s16 d25, q7, #5 
 vst1.64 {q12}, [r1], r6 
;
 sub r2, r5, #3
 add r5, r0, #16
 add r12, r2, #16
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 HALF16X16_H_ROW r0, r6
 HALF16X16_HV_EXTRAH r5, r6
 pop {r4 - r10, pc}


 @.end
