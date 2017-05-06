        #include "voRADecID.h"
	.text
        .align  4
	@.globl	_cook_cos4sin4tabOffset
	@.globl	_cook_cos4sin4tab
	@.globl	_cook_nmltTab
	.globl	_PreMultiply

_PreMultiply: @PROC
	stmdb     sp!, {r4 - r11, lr}
	mov       r14, r0
	mov       r10, r1
        ldr       r6, [sp, #36]                          @get r4
	@ldr       r6, L2885 + 4                          @get cos3sin4tabOffset
	@ldr       r7, L2885 + 8                          @nmdctTab
	ldr       r8, [r6, +r14, lsl #2]	
	ldr       r11, [r2, +r14, lsl #2]
	
	@ldr       r1, L2885                              @cos4sin4tab
	add       r9, r10, r11, lsl #2					@ zbuf2 = zbuf1 + nmdct - 1@
	movs      r12, r11, asr #2
	sub       r5, r9, #32	
	add       r4, r3, r8, lsl #2					@ csptr = cos4sin4tab + cos4sin4tabOffset[tabidx]@
	beq       L2236
L2881:	
	vld4.i32		{d0, d2,  d4,  d6 }, [r4]!
	vld4.i32		{d1, d3,  d5,  d7 }, [r4]!				@ cps2b = *csptr++@sin2b = *csptr++@
	vld2.i32		{d8, d9, d10, d11}, [r10]!				@ ar1 = *(zbuf1 + 0)@ai2 = *(zbuf1 + 1)@
	vld2.i32		{d13, d15}, [r5]!						@ ar2 = *(zbuf2 - 1)@ai1 = *(zbuf2 + 0)@
	vld2.i32		{d12, d14}, [r5]!						@ ar2 = *(zbuf2 - 1)@ai1 = *(zbuf2 + 0)@
	vshl.s32		q9, q1, #1
	vrev64.32		q10, q7
	sub				r5, r5, #32
	vsub.s32		q9, q0, q9								@ cms2 = cps2a - 2*sin2a@
	vadd.s32		q11, q4, q10							@ ar1 + ai1
	sub				r10, r10, #32
	vqdmulh.s32		q12, q0, q10							@ MULHIGH(cps2a, ai1)
	vqdmulh.s32		q13, q1, q11							@ MULHIGH(sin2a, ar1 + ai1)
	vqdmulh.s32		q14, q9, q4								@ MULHIGH(cms2, ar1)
  vsub.s32		q1, q12, q13							@ z2 = MULHIGH(cps2a, ai1) - t@
	vadd.s32		q0, q14, q13							@ z1 = MULHIGH(cms2, ar1) + t@
		
	vst2.i32		{d0, d1, d2, d3}, [r10]!
		
	vshl.s32		q9, q3, #1
	vrev64.32		q10, q6
	vsub.s32		q9, q2, q9								@ cms2 = cps2a - 2*sin2a@
	vadd.s32		q11, q5, q10							@ ar2 + ai2
	vqdmulh.s32		q12, q2, q5								@ MULHIGH(cps2a, ai2)
	vqdmulh.s32		q13, q3, q11							@ MULHIGH(sin2a, ar2 + ai2)
	vqdmulh.s32		q14, q9, q10							@ MULHIGH(cms2, ar2)
	vsub.s32		q1, q12, q13							@ z2 = MULHIGH(cps2a, ai1) - t@
	vadd.s32		q0, q14, q13							@ z1 = MULHIGH(cms2, ar1) + t@
	vrev64.32		q2, q0
	vrev64.32		q3, q1
		
	vst2.i32		{d5, d7}, [r5]!	
	vst2.i32		{d4, d6}, [r5]! 
	
	sub     		r12, r12, #4
	sub		  		r5, r5, #64	
	cmp     		r12, #0
	bhi     		L2881
L2236:
	ldmia     sp!, {r4 - r11, pc}
L2885:
	.word       _cook_cos4sin4tab
	.word       _cook_cos4sin4tabOffset
	.word       _cook_nmltTab

	.globl	_PreMultiplyRescale
	@.align 5

	@AREA	.text, CODE, READONLY

_PreMultiplyRescale: @PROC
	stmdb     sp!, {r4 - r11, lr}
	rsb		  r8, r2, #0
	mov       r2, r0
	mov       r10, r1
        ldr       r6, [sp, #40]
	@ldr       r6, L3885 + 4                                        @get con4sin4tabOffset
	@ldr       r7, L3885 + 8                                        @get nmdctTab
	ldr       r11, [r6, +r2, lsl #2]	
	ldr       r7, [r3, +r2, lsl #2]
        ldr        r1, [sp, #36]
	@ldr       r1, L3885	                                       @cos4sin4tab
	add       r9, r10, r7, lsl #2					@ zbuf2 = zbuf1 + nmdct - 1@
	
	vdup.s32  Q15, r8
	sub       r5, r9, #32	

	movs      r12, r7, asr #2
	add       r4, r1, r11, lsl #2					@ csptr = cos4sin4tab + cos4sin4tabOffset[tabidx]@
	beq       L3236
L3881:
	vld4.i32		{d0, d2,  d4,  d6 }, [r4]!
	vld4.i32		{d1, d3,  d5,  d7 }, [r4]!
	vld2.i32		{d8, d9, d10, d11}, [r10]!
	vld2.i32		{d13, d15}, [r5]!
	vld2.i32		{d12, d14}, [r5]!
	vshl.s32		q4, q4, q15
	vshl.s32		q5, q5, q15
	vshl.s32		q6, q6, q15
	vshl.s32		q7, q7, q15
	vshl.s32		q9, q1, #1
	vrev64.32		q10, q7
	sub				r5, r5, #32
	vsub.s32		q9, q0, q9								@ cms2 = cps2a - 2*sin2a@
	vadd.s32		q11, q4, q10							@ ar1 + ai1
	sub				r10, r10, #32
	vqdmulh.s32		q12, q0, q10							@ MULHIGH(cps2a, ai1)
	vqdmulh.s32		q13, q1, q11							@ MULHIGH(sin2a, ar1 + ai1)
	vqdmulh.s32		q14, q9, q4								@ MULHIGH(cms2, ar1)
	vsub.s32		q1, q12, q13							@ z2 = MULHIGH(cps2a, ai1) - t@
	vadd.s32		q0, q14, q13							@ z1 = MULHIGH(cms2, ar1) + t@
		
	vst2.i32		{d0, d1, d2, d3}, [r10]!
		
	vshl.s32		q9, q3, #1
	vrev64.32		q10, q6
	vsub.s32		q9, q2, q9								@ cms2 = cps2a - 2*sin2a@
	vadd.s32		q11, q5, q10							@ ar1 + ai1
	vqdmulh.s32		q12, q2, q5								@ MULHIGH(cps2a, ai1)
	vqdmulh.s32		q13, q3, q11							@ MULHIGH(sin2a, ar1 + ai1)
	vqdmulh.s32		q14, q9, q10							@ MULHIGH(cms2, ar1)
	vsub.s32		q1, q12, q13							@ z2 = MULHIGH(cps2a, ai1) - t@
	vadd.s32		q0, q14, q13							@ z1 = MULHIGH(cms2, ar1) + t@
	vrev64.i32		q2, q0
	vrev64.i32		q3, q1
		
	vst2.i32		{d5, d7}, [r5]!	
	vst2.i32		{d4, d6}, [r5]! 
	
	sub     		r12, r12, #4
	sub		  		r5, r5, #64	
	cmp     		r12, #0
	bhi     		L3881
L3236:
	ldmia     sp!, {r4 - r11, pc}
@L3885:
@	.word       _cook_cos4sin4tab
@	.word       _cook_cos4sin4tabOffset
@	.word       _cook_nmltTab

	@ENDP  @ PreMultiplyRescale
	@.END

