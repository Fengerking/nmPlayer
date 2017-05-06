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
	.section  .text
	.global   Sub_VQ_asm
	.global   Sub_VQ1_asm

Sub_VQ_asm:

        STMFD           r13!, {r0 - r12, r14}
	MOV             r5, #0                                  @index = 0
        MOV             r4, #0x3FFFFFFF	                        @dist_min = MAX_32
	MOV             r6, r1                                  @p_dico = dico

	LDRSH           r7,  [r0]                               @x[0]
	LDRSH           r8,  [r0, #2]                           @x[1]
	LDRSH           r9,  [r0, #4]                           @x[2]
	LDRSH           r10, [r0, #6]                           @x[3]
	PKHBT           r7, r7, r8, LSL #16                     @ x[1], x[0]
	PKHBT           r8, r9, r10, LSL #16                    @ x[3], x[2]

	@ r0 --- *x
	@ r1 --- *dico
	@ r3 --- dico_size
	@ r4 --- 0x7FFFFFFF
	@ r5 --- index
	@ r6 --- *p_dico
	@ r7 --- x[1], x[0]
	@ r8 --- x[3], x[2]
      
	MOV             r9, #0                                  @ i = 0

FOR_LOOP:

        MOV             r10, #0                                 @ dist = 0
	LDRSH           r2,  [r6], #2                           @ *p_dico++
	LDRSH           r14, [r6], #2                           @ *p_dico++
	LDRSH           r11, [r6], #2                           @ *p_dico++
	LDRSH           r12, [r6], #2                           @ *p_dico++
	PKHBT           r2, r2, r14, LSL #16
	PKHBT           r14, r11, r12, LSL #16
	SSUB16          r11, r7, r2
	SSUB16          r12, r8, r14
        SMLAD           r10, r11, r11, r10
	SMLAD           r10, r12, r12, r10
	CMP             r10, r4
        MOVLT           r4, r10
        MOVLT           r5, r9
        ADD             r9, r9, #1
        CMP             r9, r3
        BLT             FOR_LOOP

	MOV             r10, r5, LSL #2                         
	LDR             r2, [r13, #56]                          
        MOV             r4, r4, LSL #1                          
        ADD             r9, r1, r10, LSL #1                     
	STR             r5, [r13]
	STR             r4, [r2]

@ Reading the selected vector
	LDRSH		r6, [r9], #2					
	LDRSH		r7, [r9], #2
        LDRSH           r10, [r9], #2
        LDRSH           r11, [r9], #2	
	STRH		r6, [r0], #2					
	STRH		r7, [r0], #2
        STRH            r10, [r0], #2
        STRH            r11, [r0], #2
	LDMFD		r13!, {r0 -r12,pc}	
        @ENDP         
 

Sub_VQ1_asm:
			
	STMFD	        r13!, {r0 -r12,r14}
	MOV             r5, #0  					
	MOV		r3, r3, ASR #2					
	STR		r5, [r13]					
	MOV             r4, #0x3FFFFFFF					
	STR		r3, [r13,#12]					
	MOV		r3, r5						
	STR		r3, [r13,#8]					

FOR_LOOP1:

	MOV             r10, #0 					
	SUB		r2, r2, #1					@dim-=1
	MOV		r11, #0
	MOV		r12, #0
	MOV		r14, #0
	MOV		r2, r2, LSL #1

	LDRSH	        r6, [r0], #2					@*x++				
	LDRSH	        r7, [r0], #2					@*x++	
	LDRSH          	r3, [r1], #2					@*p_dico++	
	LDRSH		r8, [r1], r2					@*p_dico++	
	PKHBT		r6, r6, r7, LSL #16		

	LDRSH		r7,[r1],#2					@*p_dico++	
	PKHBT		r9,r3,r8,LSL #16			
	LDRSH		r8,[r1],r2					@*p_dico++	
	SSUB16		r9,r6,r9				

	LDRSH		r3,[r1],#2					@*p_dico++	
	SMLAD		r10,r9,r9,r10				
	PKHBT		r9,r7,r8,LSL #16			
	LDRSH		r8,[r1],r2					@*p_dico++	
	SSUB16		r9,r6,r9				

	LDRSH		r7,[r1],#2					@*p_dico++	
	SMLAD		r11,r9,r9,r11				
	PKHBT		r9,r3,r8,LSL #16 			
	LDRSH		r8,[r1],r2					@*p_dico++	
	SSUB16		r9,r6,r9				
	
	SUB		r1,r1,r2,LSL #2
	SMLAD		r12,r9,r9,r12				
	PKHBT		r9,r7,r8,LSL #16			
	SUB		r1,r1,#4
	SSUB16		r9,r6,r9				
	SMLAD		r14,r9,r9,r14			           
	
	ADD		r2,r2,#2					
	LDRSH		r6,[r0],#2					
	LDRSH		r3,[r1],r2					
	LDRSH		r7,[r1],r2					
	LDRSH		r8,[r1],r2					
	LDRSH		r9,[r1],#2					
	SUB		r3,r6,r3					
	SUB		r7,r6,r7					
	SMLABB		r10,r3,r3,r10				
	SMLABB		r11,r7,r7,r11				
	LDR		r3,[r13,#8]					
	SUB		r8,r6,r8					
	SUB		r9,r6,r9					
	LDR		r6,[r13]					
	SMLABB		r12,r8,r8,r12					
	SMLABB		r14,r9,r9,r14					

	
	MOV		r2,r2,ASR #1
	CMP		r10,r4	  					
	SUB		r0,r0,r2,LSL #1					
	MOVLT		r4,r10						
	ADDLT		r3,r6,#0					
	CMP		r11,r4						
	MOVLT		r4,r11						
	ADDLT		r3,r6,#1					
	CMP		r12,r4						
	LDR		r5,[r13,#12]@					
	MOVLT		r4,r12						
	ADDLT		r3,r6,#2					
	CMP		r14,r4						
	MOVLT		r4,r14						
	ADDLT		r3,r6,#3					
	ADD		r6,r6,#4					
	SUBS		r5,r5,#1					
	STR		r6,[r13]					
	STR		r3,[r13,#8]					
	STR		r5,[r13,#12]					
	BNE		FOR_LOOP1

	LDR		r9, [r13, #4]					
	SMULBB		r7, r3, r2					
	LDR		r8, [r13, #56]					
	MOV		r4, r4, LSL #1					
	ADD		r9, r9, r7, LSL #1				
	STR		r4, [r8]					
	STR		r3, [r13]					
	
@ Reading the selected vector

	LDRSH		r6, [r9], #2					
	LDRSH		r7, [r9], #2					
        LDRSH           r5, [r9], #2				
	STRH		r6, [r0], #2				
	STRH		r7, [r0], #2
        STRH            r5, [r0], #2	

	LDMFD		r13!, {r0 -r12, pc}
        @ENDP	

	.END



