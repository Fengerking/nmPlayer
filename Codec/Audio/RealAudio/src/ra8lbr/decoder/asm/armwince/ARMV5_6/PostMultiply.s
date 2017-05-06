	INCLUDE LDRDATA.H

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

	LOAD_DATA	r10, r11, r3, #0
	add       r1, r5, r4, lsl #2			; fft2 = fft1 + nmlt;
	ldr       r7, [r0, +r2, lsl #2]

	movs      r12, r4, asr #2
	mov       r7, r7, lsl #2
	sub       r4, r1, #8					; fft2 =- 2;
	add		  r7, r7, #4		
	sub       r9, r10, r11, lsl #1
	add       r6, r3, r7
	beq       |$L2276|	
|$L2274|
	LOAD_DATA	r0, r1, r5, #0				; ar1 = *(fft1 + 0); ai1 = *(fft1 + 1);
	LOAD_DATA	r2, r3, r4, #0				; ar2 = *(fft2 - 1); ai2 = *(fft2 + 0);
	add		    lr, r0, r1					; ar1 + ai1
	
	IF	CPU_TYPE > CARMV5E
		rsb		  r3, r3, #0
		smmul	  r8, lr, r11		
		smmls	  r1, r10, r1, r8
		LOAD_DATA	r10, r11, r6, #0
		smmla	  r0, r9, r0, r8
		add		  lr, r3, r2
		sub		  r9, r10, r11, lsl #1 	
		smmul	  r8, lr, r11
		add		  r6, r6, r7
		mov		  lr, r1
		smmla     r1, r2, r9, r8
		smmls	  r2, r3, r10, r8
    ELSE
		rsb		  r3, r3, #0					; ai2 = -ai2;
		smull	  r11, r8, lr, r11				; t = MULSHIFT32(sin2, ar1 + ai1);
		smull	  r10, r1, r1, r10				; MULSHIFT32(cps2, ai1)
		smull	  lr, r0, r9, r0				; MULSHIFT32(cms2, ar1)	
		LOAD_DATA	r10, r11, r6, #0
		sub		  r1, r8, r1					; *fft2-- = t - MULSHIFT32(cps2, ai1);
		add		  r0, r8, r0					; *fft1++ = t + MULSHIFT32(cms2, ar1); 
		
		add		  lr, r3, r2					; ar2 + ai2
		sub		  r9, r10, r11, lsl #1 			; cms2 = cps2 - 2*sin2;
		smull	  lr, r8, lr, r11				; t = MULSHIFT32(sin2, ar1 + ai1);
		add		  r6, r6, r7
		mov		  lr, r1
		smull     r2, r1, r2, r9				; *fft1++ = t + MULSHIFT32(cms2, ar2);
		smull	  r3, r2, r3, r10				; *fft2-- = t - MULSHIFT32(cps2, ai2);
		add		  r1, r1, r8
		sub		  r2, r8, r2		
    ENDIF	
  
	mov		  r3, lr
	STR_DATA  r0, r1, r5, #0
	STR_DATA  r2, r3, r4, #0
	add		  r5, r5, #8
	subs	  r12, r12, #1	
	sub		  r4, r4, #8
	bne       |$L2274|	
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
	sub       sp, sp, #12 
|$M3907|
	str       r2, [sp, #0]
	mov       r5, r1
	mov       r2, r0
	ldr       r0, |$L3910| + 8
	ldr       r3, |$L3910| + 4
	ldr       r4, [r0, +r2, lsl #2]
	ldr       r0, |$L3910|

	LOAD_DATA r10, r11, r3, #0
	add       r1, r5, r4, lsl #2
	ldr       r2, [r0, +r2, lsl #2]

	movs      r12, r4, asr #2
	mov       r7, r2, lsl #2
	sub       r4, r1, #8
	add		  r7, r7, #4		
	add       r6, r3, r7
	sub       r9, r10, r11, lsl #1
	str		  r12, [sp, #4]
	str		  r7, [sp, #8]
	beq       |$L3276|	
|$L3274|
	LOAD_DATA	r0, r1, r5, #0
	LOAD_DATA	r2, r3, r4, #0
	add		  lr, r0, r1
	
	IF	CPU_TYPE > CARMV5E
		rsb		  r3, r3, #0		
		smmul	  r8, lr, r11
		smmls	  r1, r10, r1, r8
		LOAD_DATA r10, r11, r6, #0
		smmla	  r0, r9, r0, r8
		add		  lr, r3, r2
		sub		  r9, r10, r11, lsl #1 	
		smmul	  r8, lr, r11
		add		  r6, r6, r7
		mov		  lr, r1
		smmla     r1, r2, r9, r8
		smmls	  r2, r3, r10, r8
		mov		  r3, lr
		ldr       r8, [sp, #0]
		mov       r0, r0, lsl r8
		mov       r1, r1, lsl r8
		mov       r2, r2, lsl r8
		mov       r3, r3, lsl r8
		
		ssat	  r0, #30, r0
		ssat	  r1, #30, r1
		ssat	  r2, #30, r2
		ssat	  r3, #30, r3
	ELSE
		rsb		  r3, r3, #0
		smull	  r11, r8, lr, r11				; t = MULSHIFT32(sin2, ar1 + ai1);
		smull	  r10, r1, r1, r10				; MULSHIFT32(cps2, ai1)
		smull	  lr, r0, r9, r0				; MULSHIFT32(cms2, ar1)	
		LOAD_DATA	r10, r11, r6, #0
		sub		  r1, r8, r1					; *fft2-- = t - MULSHIFT32(cps2, ai1);
		add		  r0, r8, r0					; *fft1++ = t + MULSHIFT32(cms2, ar1); 
		
		ldr		  r7, [sp, #8]
		add		  lr, r3, r2					; ar2 + ai2
		sub		  r9, r10, r11, lsl #1 			; cms2 = cps2 - 2*sin2;
		smull	  lr, r8, lr, r11				; t = MULSHIFT32(sin2, ar1 + ai1);
		add		  r6, r6, r7
		mov		  lr, r1
		smull     r2, r1, r2, r9				; *fft1++ = t + MULSHIFT32(cms2, ar2);
		smull	  r3, r2, r3, r10				; *fft2-- = t - MULSHIFT32(cps2, ai2);
		add		  r1, r1, r8
		sub		  r2, r8, r2
		
		mov		  r3, lr
		
		ldr       r8, [sp, #0]		
		mvn       r12, #3, 2		
		rsb       r7, r8, #0x1E
		
		mov       lr, r0, asr #31
		cmp       lr, r0, asr r7
		eorne     r0, lr, r12
		moveq     r0, r0, lsl r8

		mov       lr, r1, asr #31
		cmp       lr, r1, asr r7
		eorne     r1, lr, r12
		moveq     r1, r1, lsl r8
		
		mov       lr, r2, asr #31
		cmp       lr, r2, asr r7
		eorne     r2, lr, r12
		moveq     r2, r2, lsl r8	
		
		mov       lr, r3, asr #31
		cmp       lr, r3, asr r7
		eorne     r3, lr, r12
		moveq     r3, r3, lsl r8		
	ENDIF
	
	STR_DATA	r0, r1, r5, #0
	STR_DATA	r2, r3, r4, #0	
	ldr		  r12, [sp, #4]
	add		  r5, r5, #8
	subs      r12, r12, #1
	sub		  r4, r4, #8	
	str		  r12, [sp, #4]
	bne       |$L3274|	
|$L3276|
	add       sp, sp, #12 
	ldmia     sp!, {r4 - r11, pc}
|$L3910|
	DCD       |postSkip1|
	DCD       |cook_cos1sin1tab|
	DCD       |cook_nmltTab|
|$M3908|

	ENDP  ; |PostMultiplyRescale|

	END