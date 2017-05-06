	.section .text
	
	.extern	cos1sin1tab1024
	.extern	cos1sin1tabOffset
	.extern	nmdctTab

	.global	PreMultiply

PreMultiply:

	stmdb     sp!, {r4 - r11, lr}
L1230:	
	ldr       r8, LPREDATA + 8								@ nmdctTab
	ldr       r9, LPREDATA + 4								@ cos1sin1tabOffset	
	ldr       r10, LPREDATA									@ cos1sin1tab1024
	
	ldr       r5, [r8, +r0, lsl #2]							@ nmdct = nmdctTab[tabidx];	
	ldr       r6, [r9, +r0, lsl #2]							@ cos1sin1tabOffset[tabidx]
	add       r7, r1, r5, lsl #2							@ zbuf3 = zbuf1 + nmdct;
	add       r4, r10, r6, lsl #2							@ csptr = cos1sin1tab1024 + cos1sin1tabOffset[tabidx];
	sub       r2, r7, #32									@ zbuf3 = zbuf1 + nmdct - 8;

	movs      r0, r5, asr #2
	beq       L1233
	
L1231:
	VLD4.I32		{d0, d2, d4, d6}, [r4]!					@ cps2a = *csptr++;sin2a = *csptr++;
	VLD4.I32		{d1, d3, d5, d7}, [r4]!					@ cps2b = *csptr++;sin2b = *csptr++;
	VLD2.I32		{d8, d9, d10, d11}, [r1]				@ ar1 = *(zbuf1 + 0);ai2 = *(zbuf1 + 1);
	VLD2.I32		{d13, d15}, [r2]!					@ ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);
	VLD2.I32		{d12, d14}, [r2]!					@ ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);	
	VSHL.S32		Q9, Q1, #1
	VREV64.32		Q10, Q7
	sub				r2, r2, #32
	VSUB.S32		Q9, Q0, Q9							@ cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q4, Q10							@ ar1 + ai1
	VQDMULH.S32		Q12, Q0, Q10							@ MULHIGH(cps2a, ai1)
	VQDMULH.S32		Q13, Q1, Q11							@ MULHIGH(sin2a, ar1 + ai1)
	VQDMULH.S32		Q14, Q9, Q4								@ MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q12, Q13							@ z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							@ z1 = MULHIGH(cms2, ar1) + t;
	
	VST2.I32		{d0, d1, d2, d3}, [r1]!
	
	VSHL.S32		Q9, Q3, #1
	VREV64.32		Q10, Q6
	VSUB.S32		Q9, Q2, Q9								@ cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q5, Q10							@ ar2 + ai2
	VQDMULH.S32		Q12, Q2, Q5								@ MULHIGH(cps2a, ai2)
	VQDMULH.S32		Q13, Q3, Q11							@ MULHIGH(sin2a, ar2 + ai2)
	VQDMULH.S32		Q14, Q9, Q10							@ MULHIGH(cms2, ar2)
	VSUB.S32		Q1, Q12, Q13							@ z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							@ z1 = MULHIGH(cms2, ar1) + t;

	VREV64.32		Q3, Q1
	VREV64.32		Q2, Q0
		
	VST2.I32		{d5, d7}, [r2]!	
	VST2.I32		{d4, d6}, [r2]! 
	
	subs     		r0, r0, #4
	sub		  		r2, r2, #64	
	bne       		L1231
	
L1233:
	ldmia     sp!, {r4 - r11, pc}
LPREDATA:
	.word       cos1sin1tab1024
	.word       cos1sin1tabOffset
	.word       nmdctTab

	.global	PostMultiply

PostMultiply:

	stmdb     sp!, {r4 - r11, lr}
L1602:
	ldr       r8, LPOSTDATA + 8								@ nmdctTab
	ldr       r9, LPOSTDATA + 4								@ cos1sin1tabOffset
	ldr       r10, LPOSTDATA								@ cos1sin1tab1024
	
	ldr       r5, [r8, +r0, lsl #2]							@ nmdct = nmdctTab[tabidx];	
	ldr       r6, [r9, +r0, lsl #2]							@ cos1sin1tabOffset[tabidx]
	add       r7, r1, r5, lsl #2							@ zbuf3 = zbuf1 + nmdct;
	add       r4, r10, r6, lsl #2							@ csptr = cos1sin1tab1024 + cos1sin1tabOffset[tabidx];
	sub       r2, r7, #32									@ zbuf3 = zbuf1 + nmdct - 8;

	movs      r0, r5, asr #2
	beq       L1276
L1274:
	VLD4.I32		{d0, d2, d4, d6}, [r4]!					@ cps2a = *csptr++;sin2a = *csptr++;
	VLD4.I32		{d1, d3, d5, d7}, [r4]!					@ cps2b = *csptr++;sin2b = *csptr++;
	VLD2.I32		{d8, d9, d10, d11}, [r1]				@ ar1 = *(zbuf1 + 0);ai2 = *(zbuf1 + 1);
	VLD2.I32		{d13, d15}, [r2]!					@ ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);
	VLD2.I32		{d12, d14}, [r2]!					@ ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);	
	
	VSHL.S32		Q9, Q1, #1
	sub				r2, r2, #32
	VSUB.S32		Q9, Q0, Q9								@ cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q4, Q5								@ ar1 + ai1
	VQDMULH.S32		Q12, Q0, Q5								@ MULHIGH(cps2a, ai1)
	VQDMULH.S32		Q13, Q1, Q11							@ MULHIGH(sin2a, ar1 + ai1)
	VQDMULH.S32		Q14, Q9, Q4								@ MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q13, Q12							@ z2 = t - MULHIGH(cps2a, ai1);
	VADD.S32		Q0, Q13, Q14							@ z1 = t + MULHIGH(cms2, ar1) ;
	
	VREV64.32		Q5, Q7		
	VREV64.32		Q4, Q6	
	VSHL.S32		Q9, Q3, #1
	VREV64.32		Q7,  Q1	
	VSUB.S32		Q9, Q2, Q9								@ cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q4, Q5								@ ar2 + ai2
	VQDMULH.S32		Q12, Q2, Q5								@ MULHIGH(cps2a, ai2)
	VQDMULH.S32		Q13, Q3, Q11							@ MULHIGH(sin2a, ar2 + ai2)
	VQDMULH.S32		Q14, Q9, Q4								@ MULHIGH(cms2, ar2)
	VADD.S32		Q3, Q13, Q14							@ z1 = t + MULHIGH(cms2, ar1);
	VSUB.S32		Q1, Q13, Q12							@ z2 = t - MULHIGH(cps2a, ai1);
	VREV64.32		Q6, Q3
		
	VST2.I32		{d0, d1, d2, d3}, [r1]!
	
	VST2.I32		{d13, d15}, [r2]!	
	VST2.I32		{d12, d14}, [r2]! 
	
	subs     		r0, r0, #4
	sub		  		r2, r2, #64		

	bne       		L1274
L1276:

	ldmia     sp!, {r4 - r11, pc}
LPOSTDATA:
	.word       cos1sin1tab1024
	.word       cos1sin1tabOffset
	.word       nmdctTab

	@ENDP  													; |PostMultiply|
	.end
