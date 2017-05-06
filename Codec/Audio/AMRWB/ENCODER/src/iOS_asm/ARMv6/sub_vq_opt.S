@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************
@Word16 Sub_VQ(                                      /* output: return quantization index     */
@	      Word16 * x,                           /* input : ISF residual vector           */
@	      Word16 * dico,                        /* input : quantization codebook         */
@	      Word16 dim,                           /* input : dimention of vector           */
@	      Word16 dico_size,                     /* input : size of quantization codebook */
@	      Word32 * distance                     /* output: error of quantization         */
@	      )
@
@****************
@ ARM Registers
@****************
@ r0 --- *x
@ r1 --- *dico
@ r2 --- dim
@ r3 --- dico_size
@ r4 --- *distance
        #include "voAMRWBEncID.h"
	.text   .align 4
	.globl   _Sub_VQ_asm
	.globl   _Sub_VQ1_asm

_Sub_VQ_asm:

        stmfd           r13!, {r0 - r12, r14}
	mov             r5, #0                                  @index = 0
        mov             r4, #0x3FFFFFFF	                        @dist_min = MAX_32
	mov             r6, r1                                  @p_dico = dico

	ldrsh           r7,  [r0]                               @x[0]
	ldrsh           r8,  [r0, #2]                           @x[1]
	ldrsh           r9,  [r0, #4]                           @x[2]
	ldrsh           r10, [r0, #6]                           @x[3]
	pkhbt           r7, r7, r8, lsl #16                     @ x[1], x[0]
	pkhbt           r8, r9, r10, lsl #16                    @ x[3], x[2]

	@ r0 --- *x
	@ r1 --- *dico
	@ r3 --- dico_size
	@ r4 --- 0x7FFFFFFF
	@ r5 --- index
	@ r6 --- *p_dico
	@ r7 --- x[1], x[0]
	@ r8 --- x[3], x[2]
      
	mov             r9, #0                                  @ i = 0
FOR_LOOP:

        mov             r10, #0                                 @ dist = 0
	ldrsh           r2,  [r6], #2                           @ *p_dico++
	ldrsh           r14, [r6], #2                           @ *p_dico++
	ldrsh           r11, [r6], #2                           @ *p_dico++
	ldrsh           r12, [r6], #2                           @ *p_dico++
	pkhbt           r2, r2, r14, lsl #16
	pkhbt           r14, r11, r12, lsl #16
	ssub16          r11, r7, r2
	ssub16          r12, r8, r14
        smlad           r10, r11, r11, r10
	smlad           r10, r12, r12, r10
	cmp             r10, r4
        movlt           r4, r10
        movlt           r5, r9
        add             r9, r9, #1
        cmp             r9, r3
        blt             FOR_LOOP

	mov             r10, r5, lsl #2                         @index * dim
	ldr             r2, [r13, #56]                          @load *distance
        mov             r4, r4, lsl #1                          @get dist_min
        add             r9, r1, r10, lsl #1                     @&dico[index * dim]
	str             r5, [r13]
	str             r4, [r2]

@ Reading the selected vector
	ldrsh		r6, [r9], #2					
	ldrsh		r7, [r9], #2
        ldrsh           r10, [r9], #2
        ldrsh           r11, [r9], #2	
	strh		r6, [r0], #2					
	strh		r7, [r0], #2
        strh            r10, [r0], #2
        strh            r11, [r0], #2
	ldmfd		r13!, {r0 -r12,pc}	

 

_Sub_VQ1_asm:
			
	stmfd	        r13!, {r0 -r12,r14}
	mov             r5, #0  						
	mov		r3, r3, asr #2					
	str		r5, [r13]					
	mov             r4, #0x3FFFFFFF				
	str		r3, [r13,#12]					
	mov		r3, r5					
	str		r3, [r13,#8]				

FOR_LOOP1:

	mov             r10, #0 				
	sub		r2, r2, #1				
	mov		r11, #0
	mov		r12, #0
	mov		r14, #0
	mov		r2, r2, lsl #1

	ldrsh	        r6, [r0], #2					@*x++				
	ldrsh	        r7, [r0], #2					@*x++	
	ldrsh          	r3, [r1], #2					@*p_dico++	
	ldrsh		r8, [r1], r2					@*p_dico++	
	pkhbt		r6, r6, r7, lsl #16		

	ldrsh		r7,[r1],#2					@*p_dico++	
	pkhbt		r9,r3,r8,lsl #16			
	ldrsh		r8,[r1],r2					@*p_dico++	
	ssub16		r9,r6,r9				

	ldrsh		r3,[r1],#2					@*p_dico++	
	smlad		r10,r9,r9,r10				
	pkhbt		r9,r7,r8,lsl #16			
	ldrsh		r8,[r1],r2					@*p_dico++	
	ssub16		r9,r6,r9				

	ldrsh		r7,[r1],#2					@*p_dico++	
	smlad		r11,r9,r9,r11				
	pkhbt		r9,r3,r8,lsl #16 			
	ldrsh		r8,[r1],r2					@*p_dico++	
	ssub16		r9,r6,r9				
	
	sub		r1,r1,r2,lsl #2
	smlad		r12,r9,r9,r12				
	pkhbt		r9,r7,r8,lsl #16			
	sub		r1,r1,#4
	ssub16		r9,r6,r9				
	smlad		r14,r9,r9,r14			           
	
	add		r2,r2,#2					
	ldrsh		r6,[r0],#2					
	ldrsh		r3,[r1],r2					
	ldrsh		r7,[r1],r2					
	ldrsh		r8,[r1],r2					
	ldrsh		r9,[r1],#2					
	sub		r3,r6,r3					
	sub		r7,r6,r7					
	smlabb		r10,r3,r3,r10				
	smlabb		r11,r7,r7,r11				
	ldr		r3,[r13,#8]					
	sub		r8,r6,r8					
	sub		r9,r6,r9					
	ldr		r6,[r13]					
	smlabb		r12,r8,r8,r12					
	smlabb		r14,r9,r9,r14					

	
	mov		r2,r2,asr #1
	cmp		r10,r4	  					
	sub		r0,r0,r2,lsl #1					
	movlt		r4,r10						
	addlt		r3,r6,#0					
	cmp		r11,r4						
	movlt		r4,r11						
	addlt		r3,r6,#1					
	cmp		r12,r4						
	ldr		r5,[r13,#12]@					
	movlt		r4,r12						
	addlt		r3,r6,#2					
	cmp		r14,r4						
	movlt		r4,r14						
	addlt		r3,r6,#3					
	add		r6,r6,#4					
	subs		r5,r5,#1					
	str		r6,[r13]					
	str		r3,[r13,#8]					
	str		r5,[r13,#12]					
	bne		FOR_LOOP1

	ldr		r9, [r13, #4]					
	smulbb		r7, r3, r2					
	ldr		r8, [r13, #56]					
	mov		r4, r4, lsl #1					
	add		r9, r9, r7, lsl #1				
	str		r4, [r8]					
	str		r3, [r13]					
	
@ Reading the selected vector

	ldrsh		r6, [r9], #2					
	ldrsh		r7, [r9], #2					
        ldrsh           r5, [r9], #2				
	strh		r6, [r0], #2				
	strh		r7, [r0], #2
        strh            r5, [r0], #2	

	ldmfd		r13!, {r0 -r12, pc}

	@.END



