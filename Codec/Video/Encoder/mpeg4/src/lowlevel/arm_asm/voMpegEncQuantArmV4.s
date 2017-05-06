
;	/************************************************************************
;	*																		*
;	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
;	************************************************************************/

	EXPORT	QuantInter_ARMV4
	
	AREA	DATA	
	IMPORT	__voMPEG4E0220;__voH263E0220 ;quant_multi_table     ;extern const unsigned int quant_multi_table[32]

				
	AREA	|.text|, CODE
			
			
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;QuantInterMPEG4Armv4(signed short * coeff,
;				   const signed short * data,
;				   const unsigned int quant)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;r0: coeff
;r1: data		
;r2: quant
;r3: mult
;r4: quant_m_2		
;r5: quant_d_2, temp
;r6: sum
;r7: abs(acLevel)
;r8: coeff[i]
;r9: tmp
;r10: tmp
;r12: next data
;r14: i

SCALE_BITS	EQU		16

QuantInter_ARMV4

	stmdb	sp!, {r4 - r10, lr}

	pld		[r1]
	ldr		r3, =__voMPEG4E0220;__voH263E0220 ;quant_multi_table
	mov		r4, r2, lsl #1				;quant_m_2 = quant << 1;
	mov		r5, r2, lsr #1				;quant_d_2 = quant >> 1;
	mov		r6, #0
	mov		lr, #32						;loop num
	ldr		r3, [r3, r2, lsl #2]		;mult = quant_multi_table[quant];	

INTER_QUANT_LOOP
	ldr		r7, [r1], #4				;acLevel = data[i];
	mov		r8, #0						
	mov		r12, r7, asr #16			;next data
	mov		r7, r7, lsl #16
	movs	r7, r7, asr #16
	rsblt	r9, r7, #0					;abs(acLevel)
	movge	r9, r7
	sub		r10, r9, r5				;tmp = abs(acLevel) - quant_d_2;
	subs	r9, r4, r10				;if(tmp < quant_m_2)
	bgt		NEXT_DATA					;continue;
	mul		r10, r10, r3				;tmp = (tmp * mult) >> VOSCALEBITS;
	movs	r7, r7						;if(acLevel < 0)
	mov		r8, r10, lsr #SCALE_BITS
	add		r6, r6, r8				    ;sum += tmp;
	rsblt	r8, r8, #0					;tmp = -tmp;
NEXT_DATA
	strh	r8, [r0], #2				;coeff[i];	
	movs	r10, r12				
	rsblt	r10, r10, #0				;abs(acLevel)
	sub		r10, r10, r5				;tmp = abs(acLevel) - quant_d_2;
	subs	r9, r4, r10					;if(tmp < quant_m_2)
	movgt	r8, #0						;coeff[i] = 0;		
	bgt		NEXT_LOOP					;continue;
	mul		r8, r10, r3		        	;tmp = (tmp * mult) >> VOSCALEBITS;
	movs	r12, r12					;if(acLevel < 0)
	mov		r8, r8, lsr #SCALE_BITS
	add		r6, r6, r8				    ;sum += tmp;		
	rsblt	r8, r8, #0					;tmp = -tmp;
NEXT_LOOP
	subs	lr, lr, #1
	strh	r8, [r0], #2				;coeff[i] = (signed short)tmp;
	bne		INTER_QUANT_LOOP

	mov		r0, r6
	ldmia	sp!, {r4 - r10, pc}
	
	
	END


