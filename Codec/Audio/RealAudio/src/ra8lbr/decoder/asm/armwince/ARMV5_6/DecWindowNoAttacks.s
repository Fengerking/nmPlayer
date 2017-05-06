	INCLUDE LDRDATA.H
	
	IMPORT	|cook_nmltTab|
	IMPORT	|cook_windowOffset|
	IMPORT	|cook_window|
	
	EXPORT	|cook_DecWindowNoAttacks|
	
	AREA	|.text|, CODE, READONLY
	
|cook_DecWindowNoAttacks| PROC
	stmdb       sp!, {r4 - r11, lr}
	sub         sp, sp, #16
	
	ldr			r8, DATATABLE
	ldr			r9, DATATABLE + 4
	ldr			r10, DATATABLE + 8
	
	ldr         r5, [r8, +r0, lsl #2]						; nmlt = nmltTab[tabidx];
	add         r7, r1, #1, 20								; over0 = buf0 + MAXNMLT;
	ldr         r4, [r9, +r0, lsl #2]						; windowOffset[tabidx]
	sub			r14, r5, #1	
	add         r12, r10, r4, lsl #2						; wnd = window + windowOffset[tabidx];
	movs		r5, r5, asr #1								; nmltHalf = nmlt >> 1;	
	add			r8, r7, r14, lsl #2							; over1 = over0 + nmlt - 1;	
	add			r0, r1, r5, lsl #2							; buf0 += nmltHalf;
	mul			r14, r14, r3								; (nmlt - 1) * nChans
	sub			r1, r0, #4									; buf1  = buf0 - 1;
	mov			r3, r3, lsl #1
	add			r4, r2, r14, lsl #1							; pcm1 = pcm0 + (nmlt - 1) * nChans;
	
	str			r3, [sp, #4]
	mov         r14, #0x7F, 24	
	orr         r14, r14, #0xFF
	
	beq         ENDLOOP

BEGIN_LOOP	
	IF	CPU_TYPE <= CARMV5E
		str			r5, [sp, #8]	
		ldr         r6, [r1], #-4								; in = *buf1--;
	
		ldr			r3, [r12], #4								; w0 = *wnd++;
		ldr			r5, [r12], #4							  	; w1 = *wnd++;

	
		smull		r9, r11, r3, r6								; f0 = MULSHIFT32(w0, in);
		smull		r10, r6, r5, r6						  		; f1 = MULSHIFT32(w1, in);
	
		ldr			r9, [r7, #0]								; oc = *over0;
	
		ldr			r10, [r0], #4								; in = *buf0++;	
		add			r9, r9, r11									; f0 + oc	
	
		smull		r11, r5, r5, r10							; MULSHIFT32(w1, in)	
		add			r9, r9, #0x10

		smull		r11, r3, r3, r10							; MULSHIFT32(w0, in)	
		mov			r9, r9, asr #5	
	
		ldr			r10, [r8, #0]								; oc = *over1;		
		str			r5, [r7], #4								; *over0++ =  MULSHIFT32(w1, in);
	
		add			r11, r10, r6								; f1 + oc
		rsb			r3, r3, #0									; *over1-- = -MULSHIFT32(w0, in);
	
		add			r11, r11, #0x10	
		mov         r6, r9, asr #31			
	
		mov			r11, r11, asr #5	
		str			r3, [r8], #-4						
					
		mov         r10, r11, asr #31			
	
		cmp         r6, r9, asr #15	
		eorne       r9, r6, r14	
	
		ldr			r3, [sp, #4]
	
		cmp         r10, r11, asr #15
		eorne       r11, r10, r14
	
		ldr			r5, [sp, #8]
	
		strh        r9, [r2], +r3	
		strh		r11, [r4], -r3
	ELSE	
		ldr     r6, [r1], #-4										; in = *buf1--;
	
		ldr			r3, [r12], #4										; w0 = *wnd++;
		ldr			r14, [r12], #4							  	; w1 = *wnd++;
	
		smmul		r9, r3, r6											; f0 = MULSHIFT32(w0, in);
		smmul		r10, r14, r6	  					  		; f1 = MULSHIFT32(w1, in);
	
		ldr			r11, [r7, #0]					  				; oc = *over0;	
		ldr			r6, [r0], #4										; in = *buf0++;	
	
		add			r9, r9, r11											; f0 + oc	
		ldr			r11, [r8, #0]										; oc = *over1;
	
		add			r9, r9, #0x10		
		add 		r10, r10, r11										; f1 + oc		
	
		smmul		r11, r14, r6										; MULSHIFT32(w1, in)	
		add			r10, r10, #0x10
	
		smmul		r14, r3, r6								 			; MULSHIFT32(w0, in)	
		str			r11, [r7], #4										; *over0++ =  MULSHIFT32(w1, in);
	
		rsb			r14, r14, #0										; *over1-- = -MULSHIFT32(w0, in);
		ldr			r3, [sp, #4]						
					
		ssat  	r9, #16, r9, asr #5
		str			r14, [r8], #-4		
	
		ssat  	r10, #16, r10, asr #5	
		strh    r9, [r2], +r3	
	
		strh		r10, [r4], -r3		
	ENDIF
	
	subs		r5, r5, #1		
	bne			BEGIN_LOOP
	
ENDLOOP
	
	add         sp, sp, #16
	ldmia       sp!, {r4 - r11, pc}	
	
DATATABLE
	DCD         |cook_nmltTab|
	DCD         |cook_windowOffset|
	DCD         |cook_window|	
	
	END