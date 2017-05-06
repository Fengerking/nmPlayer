	INCLUDE LDRDATA.H

	IMPORT	|cook_cos4sin4tabOffset|
	IMPORT	|cook_cos4sin4tab|
	IMPORT	|cook_nmltTab|
	
	EXPORT	|PreMultiply|

	AREA	|.text|, CODE, READONLY

|PreMultiply| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M2882|
	mov       r2, r0
	mov       lr, r1
	ldr       r0, |$L2885| + 8
	ldr       r3, [r0, +r2, lsl #2]
	ldr       r0, |$L2885| + 4
	add       r1, lr, r3, lsl #2
	sub       r5, r1, #8
	ldr       r2, [r0, +r2, lsl #2]
	ldr       r1, |$L2885|

	movs      r12, r3, asr #2
	add       r4, r1, r2, lsl #2
	beq       |$L2236|
|$L2881|
	LOAD_DATA	r0, r1, r4, #0
	LOAD_DATA	r2, r3, lr, #0
	LOAD_DATA	r6, r7, r5, #0
	add		  r8, r2, r7
	
	IF	CPU_TYPE > CARMV5E
		sub		  r10, r0, r1, lsl #1
		smmul	  r11, r1, r8		
		smmls	  r9, r0, r7, r11
		smmla	  r8, r10, r2, r11
		LOAD_DATA	r0, r1, r4, #8
		rsb		  r9, r9, #0	
		add		  r10, r6, r3
		STR_DATA	r8, r9, lr, #0
		smmul 	  r11, r1, r10
		sub		  r2, r0, r1, lsl #1
		smmls   r9, r0, r3, r11
		add		  lr, lr, #8	
		smmla	  r8, r2, r6, r11
		rsb		  r9, r9, #0		
	ELSE
		sub		  r10, r0, r1, lsl #1
		smull	  r8, r11, r1, r8							; t  = MULHIGH(sin2a, ar1 + ai1);
		smull	  r1, r9, r0, r7							; MULHIGH(cps2a, ai1)
		smull	  r2, r8, r10, r2							; MULSHIFT32(cms2, ar1)
		LOAD_DATA	r0, r1, r4, #8
		sub		  r9, r9, r11	
		add			r8, r8, r11
		add		  r10, r6, r3									; ar2 + ai2
		STR_DATA	r8, r9, lr, #0
		smull 	r10, r11, r1, r10					   ; t  = MULHIGH(sin2a, ar1 + ai1);
		sub		  r2, r0, r1, lsl #1
		smull   r0, r9, r0, r3
		add		  lr, lr, #8	
		smull	  r1, r8, r2, r6
		sub		  r9, r9, r11
		add			r8, r8, r11	
	ENDIF

	add		  r4, r4, #16	
	STR_DATA	r8, r9, r5, #0
	sub       r12, r12, #1
	sub		  r5, r5, #8	
	cmp       r12, #0
	bhi       |$L2881|
|$L2236|
	ldmia     sp!, {r4 - r11, pc}
|$L2885|
	DCD       |cook_cos4sin4tab|
	DCD       |cook_cos4sin4tabOffset|
	DCD       |cook_nmltTab|
|$M2883|

	ENDP  ; |PreMultiply|

	EXPORT	|PreMultiplyRescale|

	AREA	|.text|, CODE, READONLY

|PreMultiplyRescale| PROC
	stmdb     sp!, {r4 - r11, lr}
	sub       sp, sp, #8 
|$M3882|
	str       r2, [sp, #0]
	mov       lr, r1
	mov       r2, r0
	ldr       r0, |$L3885| + 8
	ldr       r3, [r0, +r2, lsl #2]
	ldr       r0, |$L3885| + 4
	add       r1, lr, r3, lsl #2
	sub       r5, r1, #8
	ldr       r2, [r0, +r2, lsl #2]
	ldr       r1, |$L3885|

	movs      r12, r3, asr #2
	add       r4, r1, r2, lsl #2
	beq       |$L3236|
|$L3881|
	ldr       r8, [sp, #0]
	LOAD_DATA	r0, r1, r4, #0
	LOAD_DATA	r2, r3, lr, #0
	LOAD_DATA	r6, r7, r5, #0
	mov       r2, r2, asr r8
	mov       r7, r7, asr r8
	mov       r3, r3, asr r8
	mov       r6, r6, asr r8
	add		    r8, r2, r7

	
	IF	CPU_TYPE > CARMV5E
		sub		  r10, r0, r1, lsl #1
		smmul	  r11, r1, r8
		smmls	  r9, r0, r7, r11
		smmla	  r8, r10, r2, r11
		LOAD_DATA	r0, r1, r4, #8
		rsb		  r9, r9, #0	
		add		  r10, r6, r3
		STR_DATA	r8, r9, lr, #0
		smmul 	  r11, r1, r10
		sub		  r2, r0, r1, lsl #1
		smmls     r9, r0, r3, r11
		add		  lr, lr, #8	
		smmla	  r8, r2, r6, r11
		rsb		  r9, r9, #0
	ELSE
		sub		  r10, r0, r1, lsl #1
		smull	  r8, r11, r1, r8							; t  = MULHIGH(sin2a, ar1 + ai1);		
		smull	  r1, r9, r0, r7							; MULHIGH(cps2a, ai1)
		smull	  r2, r8, r10, r2							; MULSHIFT32(cms2, ar1)
		LOAD_DATA	r0, r1, r4, #8
		sub		  r9, r9, r11	
		add			r8, r8, r11
		add		  r10, r6, r3									; ar2 + ai2
		STR_DATA	r8, r9, lr, #0
		smull 	r10, r11, r1, r10					  ; t  = MULHIGH(sin2a, ar1 + ai1);
		sub		  r2, r0, r1, lsl #1
		smull   r0, r9, r0, r3
		add		  lr, lr, #8	
		smull	  r1, r8, r2, r6
		sub		  r9, r9, r11
		add			r8, r8, r11		
	ENDIF
	
	add		  r4, r4, #16	
	STR_DATA	r8, r9, r5, #0
	sub       r12, r12, #1
	sub		  r5, r5, #8	
	cmp       r12, #0
	bhi       |$L3881|
|$L3236|
	add       sp, sp, #8 
	ldmia     sp!, {r4 - r11, pc}
|$L3885|
	DCD       |cook_cos4sin4tab|
	DCD       |cook_cos4sin4tabOffset|
	DCD       |cook_nmltTab|
|$M3883|

	ENDP  ; |PreMultiplyRescale|
	END