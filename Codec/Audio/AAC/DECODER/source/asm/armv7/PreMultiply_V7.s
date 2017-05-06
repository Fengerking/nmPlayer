	IMPORT	|AAD_srtdata66|
	IMPORT	|AAD_srtdata20|
	IMPORT	|AAD_srtidata81|
	
	EXPORT	|AAD_srtidata516|

	AREA	|.text|, CODE, READONLY

|AAD_srtidata516| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M2882|
	mov       r3, r0
	mov       r10, r1
	ldr       r6, |$L2885| + 4
	ldr       r7, |$L2885| + 8
	ldr       r8, [r6, +r3, lsl #2]	
	ldr       r2, [r7, +r3, lsl #2]
	
	ldr       r1, |$L2885|
	
	add       r9, r10, r2, lsl #2					; zbuf2 = zbuf1 + nmdct - 1;
	movs      r12, r2, asr #2
	sub       r5, r9, #32	
	add       r4, r1, r8, lsl #2					; csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];
	beq       |$L2236|
|$L2881|
	VLD4.I32		{d0, d2,  d4,  d6 }, [r4]!				; cps2a = *csptr++;sin2a = *csptr++;
	VLD4.I32		{d1, d3,  d5,  d7 }, [r4]!				; cps2b = *csptr++;sin2b = *csptr++;
	VLD2.I32		{d8, d9, d10, d11}, [r10]!				; ar1 = *(zbuf1 + 0);ai2 = *(zbuf1 + 1);
	VLD2.I32		{d13, d15}, [r5]!						; ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);
	VLD2.I32		{d12, d14}, [r5]!						; ar2 = *(zbuf2 - 1);ai1 = *(zbuf2 + 0);
	VSHL.S32		Q9, Q1, #1
	VREV64.32		Q10, Q7
	sub				r5, r5, #32
	VSUB.S32		Q9, Q0, Q9								; cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q4, Q10							; ar1 + ai1
	sub				r10, r10, #32
	VQDMULH.S32		Q12, Q0, Q10							; MULHIGH(cps2a, ai1)
	VQDMULH.S32		Q13, Q1, Q11							; MULHIGH(sin2a, ar1 + ai1)
	VQDMULH.S32		Q14, Q9, Q4								; MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q12, Q13							; z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							; z1 = MULHIGH(cms2, ar1) + t;
		
	VST2.I32		{d0, d1, d2, d3}, [r10]!
		
	VSHL.S32		Q9, Q3, #1
	VREV64.32		Q10, Q6
	VSUB.S32		Q9, Q2, Q9								; cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q5, Q10							; ar2 + ai2
	VQDMULH.S32		Q12, Q2, Q5								; MULHIGH(cps2a, ai2)
	VQDMULH.S32		Q13, Q3, Q11							; MULHIGH(sin2a, ar2 + ai2)
	VQDMULH.S32		Q14, Q9, Q10							; MULHIGH(cms2, ar2)
	VSUB.S32		Q1, Q12, Q13							; z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							; z1 = MULHIGH(cms2, ar1) + t;
	VREV64.32		Q2, Q0
	VREV64.32		Q3, Q1
		
	VST2.I32		{d5, d7}, [r5]!	
	VST2.I32		{d4, d6}, [r5]! 
	
	sub     		r12, r12, #4
	sub		  		r5, r5, #64	
	cmp     		r12, #0
	bhi     		|$L2881|
|$L2236|
	ldmia     sp!, {r4 - r11, pc}
|$L2885|
	DCD       |AAD_srtdata20|
	DCD       |AAD_srtdata66|
	DCD       |AAD_srtidata81|
|$M2883|

	ENDP  ; |AAD_srtidata516|

	EXPORT	|AAD_srtidata517|

	AREA	|.text|, CODE, READONLY

|AAD_srtidata517| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M3882|
	rsb		  r8, r2, #0
	mov       r2, r0
	mov       r10, r1
	ldr       r6, |$L3885| + 4
	ldr       r7, |$L3885| + 8
	ldr       r11, [r6, +r2, lsl #2]	
	ldr       r3, [r7, +r2, lsl #2]
	ldr       r1, |$L3885|	
	add       r9, r10, r3, lsl #2					; zbuf2 = zbuf1 + nmdct - 1;
	
	VDUP.S32  Q15, r8
	sub       r5, r9, #32	

	movs      r12, r3, asr #2
	add       r4, r1, r11, lsl #2					; csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];
	beq       |$L3236|
|$L3881|
	VLD4.I32		{d0, d2,  d4,  d6 }, [r4]!
	VLD4.I32		{d1, d3,  d5,  d7 }, [r4]!
	VLD2.I32		{d8, d9, d10, d11}, [r10]!
	VLD2.I32		{d13, d15}, [r5]!
	VLD2.I32		{d12, d14}, [r5]!
	VSHL.S32		Q4, Q4, Q15
	VSHL.S32		Q5, Q5, Q15
	VSHL.S32		Q6, Q6, Q15
	VSHL.S32		Q7, Q7, Q15
	VSHL.S32		Q9, Q1, #1
	VREV64.32		Q10, Q7
	sub				r5, r5, #32
	VSUB.S32		Q9, Q0, Q9								; cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q4, Q10							; ar1 + ai1
	sub				r10, r10, #32
	VQDMULH.S32		Q12, Q0, Q10							; MULHIGH(cps2a, ai1)
	VQDMULH.S32		Q13, Q1, Q11							; MULHIGH(sin2a, ar1 + ai1)
	VQDMULH.S32		Q14, Q9, Q4								; MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q12, Q13							; z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							; z1 = MULHIGH(cms2, ar1) + t;
		
	VST2.I32		{d0, d1, d2, d3}, [r10]!
		
	VSHL.S32		Q9, Q3, #1
	VREV64.32		Q10, Q6
	VSUB.S32		Q9, Q2, Q9								; cms2 = cps2a - 2*sin2a;
	VADD.S32		Q11, Q5, Q10							; ar1 + ai1
	VQDMULH.S32		Q12, Q2, Q5								; MULHIGH(cps2a, ai1)
	VQDMULH.S32		Q13, Q3, Q11							; MULHIGH(sin2a, ar1 + ai1)
	VQDMULH.S32		Q14, Q9, Q10							; MULHIGH(cms2, ar1)
	VSUB.S32		Q1, Q12, Q13							; z2 = MULHIGH(cps2a, ai1) - t;
	VADD.S32		Q0, Q14, Q13							; z1 = MULHIGH(cms2, ar1) + t;
	VREV64.I32		Q2, Q0
	VREV64.I32		Q3, Q1
		
	VST2.I32		{d5, d7}, [r5]!	
	VST2.I32		{d4, d6}, [r5]! 
	
	sub     		r12, r12, #4
	sub		  		r5, r5, #64	
	cmp     		r12, #0
	bhi     		|$L3881|
|$L3236|
	ldmia     sp!, {r4 - r11, pc}
|$L3885|
	DCD       |AAD_srtdata20|
	DCD       |AAD_srtdata66|
	DCD       |AAD_srtidata81|
|$M3883|

	ENDP  ; |AAD_srtidata517|
	END
