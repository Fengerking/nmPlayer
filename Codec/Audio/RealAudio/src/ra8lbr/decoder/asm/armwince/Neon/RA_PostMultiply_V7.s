	IMPORT	|postSkip1|
	IMPORT	|cook_cos1sin1tab|
	IMPORT	|cook_nmltTab|
	
	EXPORT	|PostMultiply|

	AREA	|.text|, CODE, READONLY

|PostMultiply| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M2907|
	mov       r5, r1
	mov       r2, r0
	ldr       r0, |$L2910| + 8
	ldr       r3, |$L2910| + 4
	ldr       r4, [r0, +r2, lsl #2]
	ldr       r0, |$L2910|

	VLD1.I32  D12, [r3]
	add       r1, r5, r4, lsl #2
	ldr       r2, [r0, +r2, lsl #2]

	movs      r12, r4, asr #3
	mov       r7, r2, lsl #2
	sub       r4, r1, #8
	add		  r7, r7, #4		
	sub       r9, r10, r11, lsl #1
	add       r6, r3, r7
	beq       |$L2276|	
|$L2274|
	VMOV			D0, D12
	VLD1.I32	  	D4, [r5]!
	VLD1.I32	  	D6, [r5]!
	VLD1.I32		D2, [r6]	
	VLD1.I32    	D5, [r4]
	add		  		r6, r6, r7		
	VMOV			D1, D2
	sub				r5, r5, #16
	sub		  		r4, r4, #8
	VLD1.I32	  	D3, [r6]		
	VLD1.I32    	D7, [r4]	
	VMOV			D12, D3
	add		  		r6, r6, r7
	VTRN.32			Q2, Q3
	VTRN.32			Q0, Q1
	VNEG.S32		D7, D7
	VSHL.S32		Q4, Q1, #1
	VADD.S32		Q5, Q2, Q3					; ar1 + ai1
	VSUB.S32		Q4, Q0, Q4					; cms2 = cps2 - 2*sin2;
	VQDMULH.S32		Q7, Q5, Q1					; t = MULHIGH(sin2, ar1 + ai1);
	VQDMULH.S32		Q8, Q0,	Q3					; MULHIGH(cps2, ai1)
	VQDMULH.S32		Q5, Q4, Q2					; MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q7, Q8					; t - MULHIGH(cps2, ai1);
	VADD.S32		Q0, Q7, Q5					; t + MULHIGH(cms2, ar1)
	VTRN.32			D2, D3
	VTRN.32			D0, D1
	VMOV			D5, D2	
	VMOV			D4, D3
	VST1.I32		{D0, D1}, [r5]!	
	VREV64.I32		Q1, Q2
	sub       		r12, r12, #1	
	VST1.I32		{D2, D3}, [r4]
	cmp       		r12, #0
	sub		  		r4, r4, #8
	bhi       		|$L2274|	
|$L2276|
	ldmia     sp!, {r4 - r11, pc}
|$L2910|
	DCD       |postSkip1|
	DCD       |cook_cos1sin1tab|
	DCD       |cook_nmltTab|
|$M2908|

	ENDP  ; |PostMultiply|

	EXPORT	|PostMultiplyRescale|

	AREA	|.text|, CODE, READONLY

|PostMultiplyRescale| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M3907|
	add				r8, r2, #2
	mov       r5, r1
	mov       r2, r0
	ldr       r0, |$L3910| + 8
	ldr       r3, |$L3910| + 4
	ldr       r4, [r0, +r2, lsl #2]
	ldr       r0, |$L3910|
	VDUP.S32  Q15, r8

	VLD1.I32  D12, [r3]
	add       r1, r5, r4, lsl #2
	ldr       r2, [r0, +r2, lsl #2]

	movs      r12, r4, asr #3
	mov       r7, r2, lsl #2
	sub       r4, r1, #8
	add		  r7, r7, #4		
	sub       r9, r10, r11, lsl #1
	add       r6, r3, r7
	beq       |$L3276|	
|$L3274|
	VMOV			D0, D12
	VLD1.I32	  	D4, [r5]!
	VLD1.I32	  	D6, [r5]!
	VLD1.I32		D2, [r6]	
	VLD1.I32    	D5, [r4]
	add		  		r6, r6, r7		
	VMOV			D1, D2
	sub				r5, r5, #16
	sub		  		r4, r4, #8
	VLD1.I32	  	D3, [r6]		
	VLD1.I32    	D7, [r4]	
	VMOV			D12, D3
	add		  		r6, r6, r7
	VTRN.32			Q2, Q3
	VTRN.32			Q0, Q1
	VNEG.S32		D7, D7
	VSHL.S32		Q4, Q1, #1
	VADD.S32		Q5, Q2, Q3					; ar1 + ai1
	VSUB.S32		Q4, Q0, Q4					; cms2 = cps2 - 2*sin2;
	VQDMULH.S32		Q7, Q5, Q1					; t = MULHIGH(sin2, ar1 + ai1);
	VQDMULH.S32		Q8, Q0,	Q3					; MULHIGH(cps2, ai1)
	VQDMULH.S32		Q5, Q4, Q2					; MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q7, Q8					; t - MULHIGH(cps2, ai1);
	VADD.S32		Q0, Q7, Q5					; t + MULHIGH(cms2, ar1)
	VQRSHL.S32		Q1, Q1, Q15
	VQRSHL.S32		Q0, Q0, Q15
	VSHR.S32		Q1, Q1, #2
	VSHR.S32		Q0, Q0, #2
	VTRN.32			D2, D3
	VTRN.32			D0, D1
	VMOV			D5, D2
	VMOV			D4, D3
	VST1.I32		{D0, D1}, [r5]!	
	VREV64.I32		Q1, Q2
	sub       		r12, r12, #1	
	VST1.I32		{D2, D3}, [r4]
	cmp       		r12, #0
	sub		  		r4, r4, #8
	bhi       		|$L3274|	
|$L3276|
	ldmia     	sp!, {r4 - r11, pc}
|$L3910|
	DCD       |postSkip1|
	DCD       |cook_cos1sin1tab|
	DCD       |cook_nmltTab|
|$M3908|

	ENDP  ; |PostMultiplyRescale|

	END