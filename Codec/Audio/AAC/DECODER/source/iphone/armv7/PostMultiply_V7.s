
	@AREA	.text, CODE, READONLY
	.text
	.align 4

	@.globl	_AAD_srtidata81
	@.globl	_AAD_srtdata67
	@.globl	_AAD_srtidata82

@PostMultiply(tabidx, coef, &nmdctTab[0], &cos1sin1tab[0], &postSkip[0]);

	.globl	_AAD_srtidata514

_AAD_srtidata514: @PROC
	stmdb     sp!, {r4 - r11, lr}
	mov       r5, r1
	mov       r14, r0
	@ldr       r0, L2910 + 8                   @get nmdctTab
	@ldr       r3, L2910 + 4                   @cos1sin1tab
	ldr       r4, [r2, +r14, lsl #2]
        ldr       r0, [sp, #36]
	@ldr       r0, L2910                       @postSkip

	vld1.i32  d12, [r3]
	add       r1, r5, r4, lsl #2
	ldr       r2, [r0, +r14, lsl #2]

	movs      r12, r4, asr #3
	mov       r7, r2, lsl #2
	sub       r4, r1, #8
	add		  	r7, r7, #4		
	sub       r9, r10, r11, lsl #1
	add       r6, r3, r7
	beq       L2276	
L2274:
	vshl.s32			d0, d12, #0
	vld1.i32	  	d4, [r5]!
	vld1.i32	  	d6, [r5]!
	vld1.i32		d2, [r6]	
	vld1.i32    	d5, [r4]
	add		  		r6, r6, r7		
	vshl.s32	d1, d2, #0
	sub				r5, r5, #16
	sub		  		r4, r4, #8
	vld1.i32	  	d3, [r6]		
	vld1.i32    	d7, [r4]	
	vshl.s32		d12, d3, #0
	add		  		r6, r6, r7
	vtrn.32			q2, q3
	vtrn.32			q0, q1
	vneg.s32		d7, d7
	vshl.s32		q4, q1, #1
	vadd.s32		q5, q2, q3					@ ar1 + ai1
	vsub.s32		q4, q0, q4					@ cms2 = cps2 - 2*sin2@
	vqdmulh.s32		q7, q5, q1					@ t = MULHIGH(sin2, ar1 + ai1)@
	vqdmulh.s32		q8, q0,	q3					@ MULHIGH(cps2, ai1)
	vqdmulh.s32		q5, q4, q2					@ MULHIGH(cms2, ar1)
	vsub.s32		q1, q7, q8					@ t - MULHIGH(cps2, ai1)@
	vadd.s32		q0, q7, q5					@ t + MULHIGH(cms2, ar1)
	vtrn.32			d2, d3
	vtrn.32			d0, d1
	vrev64.i32	q1, q1
	vst1.i32		{d0, d1}, [r5]!	
	vswp.i32		d2, d3
	sub       		r12, r12, #1	
	vst1.i32		{d2, d3}, [r4]
	cmp       		r12, #0
	sub		  		r4, r4, #8
	bhi       		L2274	
L2276:
	ldmia     sp!, {r4 - r11, pc}
@L2910:
@	.word       _AAD_srtidata82
@	.word       _AAD_srtdata67
@	.word       _AAD_srtidata81

	@ENDP  @ AAD_srtidata514

@PostMultiplyRescale(tabidx, coef, es, &nmdctTab[0], &cos1sin1tab[0], &postSkip[0]);

	.globl	_AAD_srtidata515

	@AREA	.text, CODE, READONLY

_AAD_srtidata515: @PROC
	stmdb     sp!, {r4 - r11, lr}
	add				r8, r2, #2
	mov       r5, r1
	mov       r2, r0
	@ldr       r0, L3910 + 8                        @get nmdctTab
        ldr       r14, [sp, #36]
	@ldr       r3, L3910 + 4                        @cos1sin1tab
	ldr       r4, [r3, +r2, lsl #2]

        ldr       r0, [sp, #40]
	@ldr       r0, L3910                            @postSkip
	vdup.s32  q15, r8

	vld1.i32  d12, [r14]
	add       r1, r5, r4, lsl #2
	ldr       r2, [r0, +r2, lsl #2]

	movs      r12, r4, asr #3
	mov       r7, r2, lsl #2
	sub       r4, r1, #8
	add		  	r7, r7, #4		
	sub       r9, r10, r11, lsl #1
	add       r6, r14, r7
	beq       L3276	
L3274:
	vshl.s32			d0, d12, #0
	vld1.i32	  	d4, [r5]!
	vld1.i32	  	d6, [r5]!
	vld1.i32		d2, [r6]	
	vld1.i32    	d5, [r4]
	add		  		r6, r6, r7		
	vshl.s32		d1, d2, #0
	sub				r5, r5, #16
	sub		  		r4, r4, #8
	vld1.i32	  	d3, [r6]		
	vld1.i32    	d7, [r4]	
	vshl.s32	d12, d3, #0
	add		  		r6, r6, r7
	vtrn.32			q2, q3
	vtrn.32			q0, q1
	vneg.s32		d7, d7
	vshl.s32		q4, q1, #1
	vadd.s32		q5, q2, q3					@ ar1 + ai1
	vsub.s32		q4, q0, q4					@ cms2 = cps2 - 2*sin2@
	vqdmulh.s32		q7, q5, q1					@ t = MULHIGH(sin2, ar1 + ai1)@
	vqdmulh.s32		q8, q0,	q3					@ MULHIGH(cps2, ai1)
	vqdmulh.s32		q5, q4, q2					@ MULHIGH(cms2, ar1)
	vsub.s32		q1, q7, q8					@ t - MULHIGH(cps2, ai1)@
	vadd.s32		q0, q7, q5					@ t + MULHIGH(cms2, ar1)
	vqrshl.s32		q1, q1, q15
	vqrshl.s32		q0, q0, q15
	vshr.s32		q1, q1, #2
	vshr.s32		q0, q0, #2
	vtrn.32			d2, d3
	vtrn.32			d0, d1
	vrev64.32		q1, q1
	vst1.i32		{d0, d1}, [r5]!	
	vswp					d2, d3
	sub       		r12, r12, #1	
	vst1.i32		{d2, d3}, [r4]
	cmp       		r12, #0
	sub		  		r4, r4, #8
	bhi       		L3274	
L3276:
	ldmia     	sp!, {r4 - r11, pc}
@L3910:
@	.word			_AAD_srtidata82
@	.word			_AAD_srtdata67
@	.word			_AAD_srtidata81

	@ENDP  @ AAD_srtidata515

	@.END

