        #include "voRADecID.h"	
        .text
        .align  4	
        .globl	_cook_nmltTab
	.globl	_cook_windowOffset
	.globl	_cook_window
	.globl	_cook_DecWindowNoAttacks

_cook_DecWindowNoAttacks:
	stmdb       sp!, {r4 - r11, lr}
	sub         sp, sp, #12
	
	ldr	    r8, DATATABLE
	ldr	    r9, DATATABLE + 4
	ldr	    r10, DATATABLE + 8
	
	ldr         r5, [r8, +r0, lsl #2]						@ nmlt = nmltTab[tabidx];
	add         r7, r1, #1, 20								@ over0 = buf0 + MAXNMLT;
	ldr         r4, [r9, +r0, lsl #2]						@ windowOffset[tabidx]
	sub	    r14, r5, #1	
	add         r12, r10, r4, lsl #2						@ wnd = window + windowOffset[tabidx];
	movs	    r5, r5, asr #1								@ nmltHalf = nmlt >> 1;	
	add	    r8, r7, r14, lsl #2							@ over1 = over0 + nmlt - 1;	
	add	    r0, r1, r5, lsl #2							@ buf0 += nmltHalf;
	mul	    r14, r14, r3								@ (nmlt - 1) * nChans
	sub	    r1, r0, #4									@ buf1  = buf0 - 1;
	mov	    r3, r3, lsl #1
	add	    r4, r2, r14, lsl #1							@ pcm1 = pcm0 + (nmlt - 1) * nChans;
	
	str	    r3, [sp, #4]
	mov         r14, #0x7F, 24	
	orr         r14, r14, #0xFF
	
	beq         ENDLOOP

BEGIN_LOOP:
	ldr         r6, [r1], #-4										@ in = *buf1--;
	
	ldr	    r3, [r12], #4										@ w0 = *wnd++;
	ldr	    r14, [r12], #4							  	@ w1 = *wnd++;
	
	smmul	    r9, r3, r6											@ f0 = MULSHIFT32(w0, in);
	smmul	    r10, r14, r6	  					  		@ f1 = MULSHIFT32(w1, in);
	
	ldr	    r11, [r7, #0]					  				@ oc = *over0;	
	ldr	    r6, [r0], #4										@ in = *buf0++;	
	
	add	    r9, r9, r11											@ f0 + oc	
	ldr	    r11, [r8, #0]										@ oc = *over1;
	
	add	    r9, r9, #0x10		
	add 	    r10, r10, r11										@ f1 + oc		
	
	smmul	    r11, r14, r6										@ MULSHIFT32(w1, in)	
	add	    r10, r10, #0x10
	
	smmul	    r14, r3, r6								 			@ MULSHIFT32(w0, in)	
	str	    r11, [r7], #4										@ *over0++ =  MULSHIFT32(w1, in);
	
	rsb	    r14, r14, #0										@ *over1-- = -MULSHIFT32(w0, in);
	ldr	    r3, [sp, #4]						
					
	ssat  	    r9, #16, r9, asr #5
	str	    r14, [r8], #-4		
	
	ssat  	    r10, #16, r10, asr #5	
	strh        r9, [r2], +r3	
	
	strh	    r10, [r4], -r3	
	subs	    r5, r5, #1		
	
	bne	    BEGIN_LOOP
	
ENDLOOP:
	
	add         sp, sp, #12
	ldmia       sp!, {r4 - r11, pc}	
	
DATATABLE:
	.word         _cook_nmltTab
	.word         _cook_windowOffset
	.word         _cook_window	
	
	@.END
