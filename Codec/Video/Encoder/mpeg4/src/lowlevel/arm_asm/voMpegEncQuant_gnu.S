
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	************************************************************************/

	.global		QuantInter_ARMV4
	
	@AREA	DATA
	.section	.data	
	.extern	__voMPEG4E0220  @quant_multi     @extern const unsigned int quant_multi[32]
				
	@AREA	|.text|, CODE
	.section	.text
			

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@QuantInterMPEG4Armv6(signed short * coeff,
@				   const signed short * data,
@				   const unsigned int quant)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@r0: coeff
@r1: data		
@r2: quant
@r3: mult
@r4: quant_m_2		
@r5: quant_d_2, temp
@r6: sum
@r7: abs(acLevel)
@r8: coeff[i]
@r9: i
@r10: tmp
@r11: acLevel
@r14: temp

.equ	SCALE_BITS	,16

QuantInter_ARMV4:

	stmdb	sp!, {r4 - r11, lr}

	@pld		[r1]
	add		r1, r1, #32
	@pld		[r1]
	add		r1, r1, #32
	@pld		[r1]
	add		r1, r1, #32
	@pld		[r1]
	sub		r1, r1, #96
	
	ldr		r3, =__voMPEG4E0220@__voH263E0220@@quant_multi
	mov		r4, r2, lsl #1				@quant_m_2 = quant << 1@
	mov		r5, r2, lsr #1				@quant_d_2 = quant >> 1@
	ldr		r3, [r3, r2, lsl #2]		@mult = quant_multi[quant]@	
	mov		r6, #0
	mov		r9, #32						@loop num

INTER_QUANT_LOOP:
	ldr		r7, [r1], #4				@acLevel = data[i]@
	mov		r8, #0						
	mov		r11, r7, asr #16
	mov		r7, r7, lsl #16
	mov		r7, r7, asr #16
	mov		r10, r7		
	rsbs	r14, r10, #0				@if(acLevel < 0)
	movgt	r10, r14					@   abs(acLevel)		
	sub		r10, r10, r5				@tmp = abs(acLevel) - quant_d_2@
	subs	r14, r4, r10				@if(tmp < quant_m_2)
	bgt		NEXT_DATA					@continue@
	mul		r10, r10, r3				@tmp = (tmp * mult) >> SCALEBITS@
	mov		r8, r10, lsr #SCALE_BITS
	add		r6, r6, r10				    @sum += tmp@
	movs	r7, r7						@if(acLevel < 0)
	rsbmi	r8, r8, #0					@tmp = -tmp@
NEXT_DATA:
	strh	r8, [r0], #2				@coeff[i]@	
	mov		r10, r11				
	rsbs	r14, r10, #0				@if(acLevel < 0)
	movgt	r10, r14					@   abs(acLevel)		
	sub		r10, r10, r5				@tmp = abs(acLevel) - quant_d_2@
	subs	r14, r4, r10				@if(tmp < quant_m_2)
	movgt	r8, #0						@coeff[i] = 0@		
	bgt		NEXT_LOOP					@continue@
	mul		r8, r10, r3		        	@tmp = (tmp * mult) >> SCALEBITS@
	mov		r8, r8, lsr #SCALE_BITS
	add		r6, r6, r8				    @sum += tmp@		
	movs	r11, r11					@if(acLevel < 0)
	rsbmi	r8, r8, #0					@tmp = -tmp@
NEXT_LOOP:
	subs	r9, r9, #1
	strh	r8, [r0], #2				@coeff[i] = (signed short)tmp@
	bne		INTER_QUANT_LOOP

	mov		r0, r6
	ldmia	sp!, {r4 - r11, pc}
	
	
	.END


