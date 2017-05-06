@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY

	.section .text
	
	.global IdctBlock1x1_ARMV6
	.global IdctBlock4x4_ARMV6
	.global IdctBlock4x8_ARMV6
	.global IdctBlock8x8_ARMV6		
 
	.align 4
Col8:
	ldrsh	r1, [r0]
	ldrsh	r2, [r0, #16]
	ldrsh	r3, [r0, #32]
	ldrsh	r4, [r0, #48]

	ldrsh	r5, [r0, #64]
	ldrsh	r6, [r0, #80]
	ldrsh	r7, [r0, #96]
	ldrsh	r8, [r0, #112]

	pkhbt	r3, r3, r7, lsl #16	@ r3 = ip6|ip2	
	pkhbt	r4, r6, r4, lsl #16	@ r4 = ip3|ip5
	
	pkhbt	r6, r5, r8, lsl #16	@ r6 = ip7|ip4
	
	orr	r7, r3, r4		@ r7 = ip6|ip2 | ip3|ip5
	orrs	r7, r7, r6		@ r7 = ip6|ip2 | ip3|ip5 | ip7|ip4
	bne	allx1_8_t_lab		@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	cmp	r2, #0				
	bne	only_x1x2t_lab		@ x2!=0
	
	cmp	r1, #0
	bne	only_x1t_lab		@ x1!=0
	mov	pc, lr
	
only_x1t_lab:
@//ip0
@	E = (idct_t)(Blk[0*8]<< 3)@
@	ip[0~7*8] = E@
    	mov	r5, r1, lsl #3
	strh	r5, [r0]
	strh	r5, [r0, #16]
	strh	r5, [r0, #32]
	strh	r5, [r0, #48]
	strh	r5, [r0, #64]
	strh	r5, [r0, #80]
	strh	r5, [r0, #96]
	strh	r5, [r0, #112]
	mov	pc, lr	
only_x1x2t_lab:
@//ip0, 1
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@	E = Blk[0*8] << 11@
@	E += 128@	
@	Add = 181 * ((A - B)>> 8)@		
@	Bdd = 181 * ((A + B)>> 8)@
@	Blk[0*8] = (idct_t)((E + A) >> 8)@
@	Blk[7*8] = (idct_t)((E - A) >> 8)@		
@	Blk[1*8] = (idct_t)((E + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((E - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((E + B) >> 8)@
@	Blk[4*8] = (idct_t)((E - B) >> 8)@				
@	Blk[2*8] = (idct_t)((E + Add) >> 8)@
@	Blk[5*8] = (idct_t)((E - Add) >> 8)@

	smulbt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smulbb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mov	r7, r1, lsl #11	@E = Blk[0*8] << 11
	add	r7, r7, #128	@E += 128@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	smulbb	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	smulbb	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov	r1, r1, asr #8	@
	mov	r2, r2, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r6, r6, asr #8	@
	mov	r9, r9, asr #8	@
	mov	r3, r3, asr #8	@
	
	strh	r1, [r0]
	strh	r8, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr	
allx1_8_t_lab:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7*8] + W1 * Blk[1*8]@
@	B = W7 * Blk[1*8] - W1 * Blk[7*8]@	
@	C = W3 * Blk[3*8] + W5 * Blk[5*8]@
@	D = W3 * Blk[5*8] - W5 * Blk[3*8]@		
@	Ad = A - C@
@	Bd = B - D@
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@		
@	Cd = A + C@
@	Dd = B + D@
@	E = (Blk[0*8] + Blk[4*8]) << 11@
@	F = (Blk[0*8] - Blk[4*8]) << 11@	
@	E += 128@
@	F += 128@			
@	G = W6 * Blk[6*8] + W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8] - W2 * Blk[6*8]@	
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@	
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@			
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@	
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@			
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@

@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
	pkhbt	r2, r2, r6		@ r2 = ip7|ip1
@ r5, r6, r7, r8, r9 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuadx	r5, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r6, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
		
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r2 r4 r9 free now
	sub	r2, r5, r7	@Ad = A - C
	sub	r4, r6, r8	@Bd = B - D@
	sub	r9, r2, r4
	add	r4, r2, r4
	mov	r2, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r4, r4, asr #8	@(Ad + Bd)) >> 8
	smulbb	r2, r12, r2	@Add = (181 * (((Ad - Bd)) >> 8))@
	smulbb	r4, r12, r4	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r5, r5, r7	@Cd = A + C
	add	r7, r6, r8	@Dd = B + D@
@ r6 r8 r9  free now	
	smuadx	r6, r3, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r8, r3, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
	
	sxth	r3, r1		@r3 = Blk[0*8]
	sxth	r1, r1, ror #16	@r1 = Blk[4*8]		
	add	r9, r3, r1	@E = (Blk[0*8] + Blk[4*8]) << 11
	sub	r1, r3, r1	@F = (Blk[0*8] - Blk[4*8]) << 11@
	mov	r9, r9, asl #11
	mov	r1, r1, asl #11
	add	r9, r9, #128
	add	r1, r1, #128			
		
@ r3  free now
	sub	r3, r9, r6	@Ed = E - G@
	add	r9, r9, r6	@Fd = E + G@
	sub	r6, r1, r8	@Gd = F - H@
	add	r8, r1, r8	@Hd = F + H@
	
@ r1 free now
	add	r1, r9, r5	@Fd + Cd
	sub	r9, r9, r5	@Fd - Cd
	add	r5, r8, r4	@Hd + Bdd
	sub	r8, r8, r4	@Hd - Bdd	
	add	r4, r3, r7	@Ed + Dd
	sub	r3, r3, r7	@Ed - Dd	
	add	r7, r6, r2	@Gd + Add
	sub	r6, r6, r2	@Gd - Add
		
	mov	r1, r1, asr #8	@
	mov	r9, r9, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@
	mov	r3, r3, asr #8	@
	mov	r7, r7, asr #8	@
	mov	r6, r6, asr #8	@

	strh	r1, [r0]
	strh	r5, [r0, #16]
	strh	r7, [r0, #32]
	strh	r4, [r0, #48]
	strh	r3, [r0, #64]
	strh	r6, [r0, #80]
	strh	r8, [r0, #96]
	strh	r9, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src:
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	
	orr	r6, r3, r4		@ r6 = ip3|ip2 | ip5|ip4
	orrs	r6, r6, r5		@ r6 = ip3|ip2 | ip5|ip4 | ip7|ip6
	bne	allx1_8_t_lab_Rowsrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rowsrc	@ x2!=0
	ldr	r6, [sp, #4]		@ Src = [sp, #4]
	ldr	r8, [sp, #52]		@ src_stride = [sp, #52]
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r9, r7, r9	
	str	r8, [sp, #4]		@ Src = [sp, #4]
	str	r9, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rowsrc	@ x1!=0
@if(src)
@{
@	if(dst !=src )
@	{
@		dst[0] = src[0]@
@		dst[1] = src[1]@
@		dst[2] = src[2]@
@		dst[3] = src[3]@
@		dst[4] = src[4]@
@		dst[5] = src[5]@
@		dst[6] = src[6]@
@		dst[7] = src[7]@
@	}
@	src += stride@
@}	
	cmp	r6, r7	
	moveq	pc, lr
	ldr	r2, [r6]
	ldr	r3, [r6, #4]								 		
	strd		r2, [r7]		
	mov	pc, lr
	
only_x1t_lab_Rowsrc:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@        dst[0] = SAT(((src[0] + E)))@
@        dst[7] = SAT(((src[7] + E)))@
@
@        dst[1] = SAT(((src[1] + E)))@
@        dst[2] = SAT(((src[2] + E)))@
@
@        dst[3] = SAT(((src[3] + E)))@
@        dst[4] = SAT(((src[4] + E)))@
@
@        dst[5] = SAT(((src[5] + E)))@
@        dst[6] = SAT(((src[6] + E)))@
@	src += stride@
@    }
@}
	mov	r4, #32
	sxtah	r4, r4, r2		@Blk[0] + 32
@	ldrd	r2, [r6]	@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
	movs	r1, r4, asr #6	
	streq	r2, [r7]
	streq	r3, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r8, r3, ror #8		@1, 3	b	
	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r1		@1, 3	a+e
	sadd16		r6, r6, r1		@0, 2	b+e
	sadd16		r8, r8, r1		@1, 3	b+e		
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r8,#8,r8		
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r8,lsl #8
	strd		r2, [r7]						
	mov	pc, lr	
	
only_x1x2t_lab_Rowsrc:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1]@
@	B = W7 * Blk[1]@	
@	E = (Blk[0] + 32) << 11@	
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((Src[0] + ((E + A )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((E - A )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((E + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((E - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((E + B )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((E - B )>>17))))@	
@	Dst[2] = SAT_new(((Src[2] + ((E + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((E - Add )>>17))))@		 				
@}
@ r2 = x1|x0
	mov	r7, #32
	smultt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@
	sxtah	r7, r7, r2		@Blk[0] + 32
	mov	r7, r7, lsl #11	@E = (Blk[0] + 32) << 11
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add

	mov	r1, r1, asr #17	@0
	mov	r2, r2, asr #17	@7
	mov	r8, r8, asr #17	@1
	mov	r4, r4, asr #17	@6
	mov	r5, r5, asr #17	@3
	mov	r3, r3, asr #17	@5	
	mov	r6, r6, asr #17	@4
	mov	r9, r9, asr #17	@2
	
	pkhbt	r7, r3, r2, lsl #16	@1, 3	b				
	ldr	r3, [sp, #4]		@ Src = [sp, #4]	
	ldr	r2, [sp, #52]		@ src_stride = [sp, #52]			
	pkhbt	r1, r1, r9, lsl #16	@0, 2	a
	pkhbt	r9, r8, r5, lsl #16	@1, 3	a
	pkhbt	r8, r6, r4, lsl #16	@0, 2	b
	add	r2, r2, r3
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r3]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r3, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
allx1_8_t_lab_Rowsrc:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7] + W1 * Blk[1]@
@	B = W7 * Blk[1] - W1 * Blk[7]@
@	C = W3 * Blk[3] + W5 * Blk[5]@
@	D = W3 * Blk[5] - W5 * Blk[3]@
@	E = (Blk[0] + 32 + Blk[4]) << 11@
@	F = (Blk[0] + 32 - Blk[4]) << 11@
@	G = W6 * Blk[6] + W2 * Blk[2]@		
@	H = W6 * Blk[2] - W2 * Blk[6]@
@	Ad = A - C@
@	Bd = B - D@
@	Add = 181 * ((Ad - Bd) >> 8)@		
@	Bdd = 181 * ((Ad + Bd) >> 8)@					
@	Cd = A + C@
@	Dd = B + D@
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@
@	Dst[0] = SAT_new(((Src[0] + ((Fd + Cd )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((Fd - Cd )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((Hd + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((Hd - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((Ed + Dd )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((Ed - Dd )>>17))))@
@	Dst[2] = SAT_new(((Src[2] + ((Gd + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((Gd - Add )>>17))))@
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6

@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
@	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
@	pkhbt	r2, r2, r6		@ r2 = ip7|ip1

	pkhbt	r1, r2, r4, lsl #16	@ r1 = ip4|ip0
	pkhtb	r2, r5, r2, asr #16	@ r2 = ip7|ip1
	pkhtb	r4, r3, r4, asr #16	@ r4 = ip3|ip5
	pkhbt	r3, r3, r5, lsl #16	@ r3 = ip6|ip2	

@ r5, r6, r7, r8, r9 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuadx	r5, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r6, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
		
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r2 r4 r9 free now
	sub	r2, r5, r7	@Ad = A - C
	sub	r4, r6, r8	@Bd = B - D@
	sub	r9, r2, r4
	add	r4, r2, r4
	mov	r2, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r4, r4, asr #8	@(Ad + Bd)) >> 8
	mul	r2, r12, r2	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r4, r12, r4	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r5, r5, r7	@Cd = A + C
	add	r7, r6, r8	@Dd = B + D@
@ r6 r8 r9  free now	
	smuadx	r6, r3, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r8, r3, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
	
	sxth	r3, r1		@r3 = Blk[0*8]
	sxth	r1, r1, ror #16	@r1 = Blk[4*8]		
	add	r9, r3, r1	@E = (Blk[0*8] + Blk[4*8]) << 11
	sub	r1, r3, r1	@F = (Blk[0*8] - Blk[4*8]) << 11@
	add	r9, r9, #32
	add	r1, r1, #32	
	mov	r9, r9, asl #11
	mov	r1, r1, asl #11			
		
@ r3  free now
	sub	r3, r9, r6	@Ed = E - G@
	add	r9, r9, r6	@Fd = E + G@
	sub	r6, r1, r8	@Gd = F - H@
	add	r8, r1, r8	@Hd = F + H@
	
@ r1 free now
	add	r1, r9, r5	@Fd + Cd
	sub	r9, r9, r5	@Fd - Cd
	add	r5, r8, r4	@Hd + Bdd
	sub	r8, r8, r4	@Hd - Bdd	
	add	r4, r3, r7	@Ed + Dd
	sub	r3, r3, r7	@Ed - Dd	
	add	r7, r6, r2	@Gd + Add
	sub	r6, r6, r2	@Gd - Add
		
	mov	r1, r1, asr #17	@0
	mov	r9, r9, asr #17	@7
	mov	r5, r5, asr #17	@1
	mov	r8, r8, asr #17	@6
	mov	r4, r4, asr #17	@3
	mov	r3, r3, asr #17	@4
	mov	r7, r7, asr #17	@2
	mov	r6, r6, asr #17	@5
	
	pkhbt	r8, r3, r8, lsl #16	@0, 2	b	
	pkhbt	r6, r6, r9, lsl #16	@1, 3	b				
	ldr	r3, [sp, #4]		@ Src = [sp, #4]	
	ldr	r2, [sp, #52]		@ src_stride = [sp, #52]			
	pkhbt	r1, r1, r7, lsl #16	@0, 2	a
	pkhbt	r9, r5, r4, lsl #16	@1, 3	a

	add	r2, r2, r3
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r3]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r3, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r7, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r7, r7, r8		@0, 2	b+e
	sadd16		r6, r3, r6		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r7,#8,r7
	usat16	 r6,#8,r6	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r7,r6,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP

@#define W1 2841	0B19                // 2048*sqrt(2)*cos(1*pi/16) 
@#define W2 2676	0A74                 // 2048*sqrt(2)*cos(2*pi/16) 
@#define W3 2408	0968                 // 2048*sqrt(2)*cos(3*pi/16) 
@#define W5 1609	0649                 // 2048*sqrt(2)*cos(5*pi/16) 
@#define W6 1108	0454                 // 2048*sqrt(2)*cos(6*pi/16) 
@#define W7 565		0235                  // 2048*sqrt(2)*cos(7*pi/16)
@#define W4 181		00B5                  // 2048*sqrt(2)*cos(7*pi/16)

	.align 8	
WxC1xC7:		.word 0x0B190235			@xC7 = D0.S16[0] xC1 = D0.S16[1]		
WxC3xC5:		.word 0x09680649			@xC5 = D0.S16[2] xC3 = D0.S16[3]			
WxC2xC6:		.word 0x0A740454			@xC6 = D1.S16[0] xC2 = D1.S16[1]


WxC5xC7:		.word 0x06490235			@xC7 = D0.S16[0] xC5 = D0.S16[1]		
WxC3xC1:		.word 0x09680B19			@xC1 = D0.S16[2] xC3 = D0.S16[3]		
Wx00xC4:		.word 0x000000B5			@xC6 = D1.S32[1]/D1.S32[2]


	.align 4
Row8nosrc:
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	
	orr	r6, r3, r4		@ r6 = ip3|ip2 | ip5|ip4
	orrs	r6, r6, r5		@ r6 = ip3|ip2 | ip5|ip4 | ip7|ip6
	bne	allx1_8_t_lab_Rownosrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rownosrc	@ x2!=0
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]
	add	r9, r7, r9	
	str	r9, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rownosrc	@ x1!=0
@@{
@	Dst[0] =
@	Dst[7] =
@	Dst[1] =
@	Dst[6] =
@	Dst[3] =
@	Dst[4] =
@	Dst[2] =
@	Dst[5] = 0@
@@}	
	mov	r2, #0
	mov	r3, #0
	strd		r2, [r7]		
	mov	pc, lr
	
only_x1t_lab_Rownosrc:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@@        dst[0] =
@@        dst[7] =
@@
@@        dst[1] =
@@        dst[2] =
@@
@@        dst[3] =
@@        dst[4] =
@@
@@        dst[5] =
@@        dst[6] = SAT_new(E)@@
@}
	mov	r4, #32
	sxtah	r1, r4, r2		@Blk[0] + 32
	usat	r2,#8, r1, asr #6	
	orr	r2, r2, r2, lsl #8
	orr	r2, r2, r2, lsl #16			
	mov	r3, r2
	strd		r2, [r7]						
	mov	pc, lr	
	
only_x1x2t_lab_Rownosrc:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1]@
@	B = W7 * Blk[1]@	
@	E = (Blk[0] + 32) << 11@	
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((E + A )>>17))@
@	Dst[7] = SAT_new(((E - A )>>17))@	
@	Dst[1] = SAT_new(((E + Bdd )>>17))@
@	Dst[6] = SAT_new(((E - Bdd )>>17))@	
@	Dst[3] = SAT_new(((E + B )>>17))@
@	Dst[4] = SAT_new(((E - B )>>17))@	
@	Dst[2] = SAT_new(((E + Add )>>17))@
@	Dst[5] = SAT_new(((E - Add )>>17))@
@}
@ r2 = x1|x0
	mov	r7, #32
	smultt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@
	sxtah	r7, r7, r2		@Blk[0] + 32
	mov	r7, r7, lsl #11	@E = (Blk[0] + 32) << 11
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add

@1, 8, 9, 5, 6, 3, 4, 2
	usat	r1, #8, r1, asr #17	@0
	usat	r8, #8, r8, asr #17	@1
	usat	r9, #8, r9, asr #17	@2
	usat	r5, #8, r5, asr #17	@3
	usat	r6, #8, r6, asr #17	@4
	usat	r3, #8, r3, asr #17	@5
	usat	r4, #8, r4, asr #17	@6
	usat	r2, #8, r2, asr #17	@7						

	orr	r8, r1, r8, lsl #8
	ldr	r1, [sp, #12]			@ dst = [sp, #12]	
	orr	r8, r8, r9, lsl #16	
	orr	r8, r8, r5, lsl #24		
	orr	r9, r6, r3, lsl #8
	ldr	r5, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r9, r9, r4, lsl #16
	orr	r9, r9, r2, lsl #24
	add	r5, r1, r5
	strd	r8, [r1]	
	str	r5, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
allx1_8_t_lab_Rownosrc:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7] + W1 * Blk[1]@
@	B = W7 * Blk[1] - W1 * Blk[7]@
@	C = W3 * Blk[3] + W5 * Blk[5]@
@	D = W3 * Blk[5] - W5 * Blk[3]@
@	E = (Blk[0] + 32 + Blk[4]) << 11@
@	F = (Blk[0] + 32 - Blk[4]) << 11@
@	G = W6 * Blk[6] + W2 * Blk[2]@		
@	H = W6 * Blk[2] - W2 * Blk[6]@
@	Ad = A - C@
@	Bd = B - D@
@	Add = 181 * ((Ad - Bd) >> 8)@		
@	Bdd = 181 * ((Ad + Bd) >> 8)@					
@	Cd = A + C@
@	Dd = B + D@
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@
@	Dst[0] = SAT_new(((Fd + Cd )>>17))@
@	Dst[7] = SAT_new(((Fd - Cd )>>17))@
@	Dst[1] = SAT_new(((Hd + Bdd )>>17))@
@	Dst[6] = SAT_new(((Hd - Bdd )>>17))@
@	Dst[3] = SAT_new(((Ed + Dd )>>17))@
@	Dst[4] = SAT_new(((Ed - Dd )>>17))@
@	Dst[2] = SAT_new(((Gd + Add )>>17))@
@	Dst[5] = SAT_new(((Gd - Add )>>17))@
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6

@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
@	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
@	pkhbt	r2, r2, r6		@ r2 = ip7|ip1

	pkhbt	r1, r2, r4, lsl #16	@ r1 = ip4|ip0
	pkhtb	r2, r5, r2, asr #16	@ r2 = ip7|ip1
	pkhtb	r4, r3, r4, asr #16	@ r4 = ip3|ip5
	pkhbt	r3, r3, r5, lsl #16	@ r3 = ip6|ip2	

@ r5, r6, r7, r8, r9 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuadx	r5, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r6, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
		
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r2 r4 r9 free now
	sub	r2, r5, r7	@Ad = A - C
	sub	r4, r6, r8	@Bd = B - D@
	sub	r9, r2, r4
	add	r4, r2, r4
	mov	r2, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r4, r4, asr #8	@(Ad + Bd)) >> 8
	mul	r2, r12, r2	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r4, r12, r4	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r5, r5, r7	@Cd = A + C
	add	r7, r6, r8	@Dd = B + D@
@ r6 r8 r9  free now	
	smuadx	r6, r3, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r8, r3, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
	
	sxth	r3, r1		@r3 = Blk[0*8]
	sxth	r1, r1, ror #16	@r1 = Blk[4*8]		
	add	r9, r3, r1	@E = (Blk[0*8] + Blk[4*8]) << 11
	sub	r1, r3, r1	@F = (Blk[0*8] - Blk[4*8]) << 11@
	add	r9, r9, #32
	add	r1, r1, #32	
	mov	r9, r9, asl #11
	mov	r1, r1, asl #11			
		
@ r3  free now
	sub	r3, r9, r6	@Ed = E - G@
	add	r9, r9, r6	@Fd = E + G@
	sub	r6, r1, r8	@Gd = F - H@
	add	r8, r1, r8	@Hd = F + H@
	
@ r1 free now
	add	r1, r9, r5	@Fd + Cd
	sub	r9, r9, r5	@Fd - Cd
	add	r5, r8, r4	@Hd + Bdd
	sub	r8, r8, r4	@Hd - Bdd	
	add	r4, r3, r7	@Ed + Dd
	sub	r3, r3, r7	@Ed - Dd	
	add	r7, r6, r2	@Gd + Add
	sub	r6, r6, r2	@Gd - Add
		
@1, 5, 7, 4, 3, 6, 8, 9	
	usat	r1, #8, r1, asr #17	@0
	usat	r5, #8, r5, asr #17	@1
	usat	r2, #8, r7, asr #17	@2
	usat	r4, #8, r4, asr #17	@3
	usat	r7, #8, r3, asr #17	@4
	usat	r3, #8, r6, asr #17	@5
	usat	r8, #8, r8, asr #17	@6
	usat	r9, #8, r9, asr #17	@7						

	orr	r6, r1, r5, lsl #8
	ldr	r1, [sp, #12]			@ dst = [sp, #12]	
	orr	r6, r6, r2, lsl #16	
	ldr	r2, [sp, #8]		@ dst_stride = [sp, #8]		
	orr	r6, r6, r4, lsl #24		
	orr	r7, r7, r3, lsl #8		
	orr	r7, r7, r8, lsl #16
	orr	r7, r7, r9, lsl #24
	add	r2, r1, r2
	strd	r6, [r1]	
	str	r2, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP
		
	.align 4
Col8_4x4:
	ldrsh	r1, [r0]
	ldrsh	r2, [r0, #16]
	ldrsh	r3, [r0, #32]
	ldrsh	r4, [r0, #48]
	
	orrs	r7, r3, r4		@ r7 = ip2|ip3
	bne	allx1_4_t_lab		@ (ip2|ip3)!=0	
	
 	cmp	r2, #0				
	bne	only_x1x2t_lab_4x4		@ x2!=0
	
	cmp	r1, #0
	bne	only_x1t_lab_4x4		@ x1!=0
	mov	pc, lr
	
only_x1t_lab_4x4:
@//ip0
@	E = (idct_t)(Blk[0*8]<< 3)@
@	ip[0~7*8] = E@    					    																																										    ip[7*8] =
    	mov	r5, r1, lsl #3
	strh	r5, [r0]
	strh	r5, [r0, #16]
	strh	r5, [r0, #32]
	strh	r5, [r0, #48]
	strh	r5, [r0, #64]
	strh	r5, [r0, #80]
	strh	r5, [r0, #96]
	strh	r5, [r0, #112]
	mov	pc, lr	
only_x1x2t_lab_4x4:
@//ip0, 1
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@	E = Blk[0*8] << 11@
@	E += 128@	
@	Add = 181 * ((A - B)>> 8)@		
@	Bdd = 181 * ((A + B)>> 8)@
@	Blk[0*8] = (idct_t)((E + A) >> 8)@
@	Blk[7*8] = (idct_t)((E - A) >> 8)@		
@	Blk[1*8] = (idct_t)((E + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((E - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((E + B) >> 8)@
@	Blk[4*8] = (idct_t)((E - B) >> 8)@				
@	Blk[2*8] = (idct_t)((E + Add) >> 8)@
@	Blk[5*8] = (idct_t)((E - Add) >> 8)@

	smulbb	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	smulbb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mov	r7, r1, lsl #11	@E = Blk[0*8] << 11
	add	r7, r7, #128	@E += 128@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	smulbb	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	smulbb	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov	r1, r1, asr #8	@
	mov	r2, r2, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r6, r6, asr #8	@
	mov	r9, r9, asr #8	@
	mov	r3, r3, asr #8	@
	
	strh	r1, [r0]
	strh	r8, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr	
allx1_4_t_lab:            
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@	E = Blk[0*8] << 11@
@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@
			
	ldr	r9, WxC2xC6	@r10 = WxC2xC6			
	pkhbt	r2, r2, r4, lsl #16	@ r2 = ip3|ip1
					@ r3 = ip2	r1 = ip0		
	smuad	r8, r2, r10	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	
	smusd	r5, r2, r10	@@Ad = A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])
	smuad	r6, r2, r11	@@Bd = B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))	
	
	sub	r7, r5, r6
	add	r6, r5, r6
	mov	r5, r7, asr #8	@(Ad - Bd)) >> 8
	mov	r6, r6, asr #8	@(Ad + Bd)) >> 8
	smulbb	r5, r12, r5	@Add = (181 * (((Ad - Bd)) >> 8))@
	smulbb	r6, r12, r6	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	mov	r1, r1, asl #11
	add	r1, r1, #128	@E
	
	smulbt	r7, r3, r9	@G = W2 * Blk[2*8]
	smulbb	r2, r3, r9	@H = W6 * Blk[2*8]@	
			
@ r3  free now
	sub	r3, r1, r7	@Ed = E - G@
	add	r7, r1, r7	@Fd = E + G@
	sub	r9, r1, r2	@Gd = E - H@
	add	r1, r1, r2	@Hd = E + H@
	
@ r1 free now
	add	r2, r7, r8	@Fd + Cd	0
	sub	r7, r7, r8	@Fd - Cd	7
	add	r8, r1, r6	@Hd + Bdd	1
	sub	r1, r1, r6	@Hd - Bdd	6	
	add	r6, r3, r4	@Ed + Dd	3
	sub	r3, r3, r4	@Ed - Dd	4	
	add	r4, r9, r5	@Gd + Add	2
	sub	r5, r9, r5	@Gd - Add	5
		
	mov	r2, r2, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@	
	mov	r6, r6, asr #8	@
	mov	r3, r3, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r1, r1, asr #8	@
	mov	r7, r7, asr #8	@

	strh	r2, [r0]
	strh	r8, [r0, #16]
	strh	r4, [r0, #32]
	strh	r6, [r0, #48]
	strh	r3, [r0, #64]
	strh	r5, [r0, #80]
	strh	r1, [r0, #96]
	strh	r7, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src_4x4:
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	cmp	r3, #0
	bne	allx1_4_t_lab_Rowsrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rowsrc_4x4	@ x2!=0
	ldr	r6, [sp, #4]		@ Src = [sp, #4]
	ldr	r8, [sp, #52]		@ src_stride = [sp, #52]
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r4, [sp, #8]		@ dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r4, r7, r4	
	str	r8, [sp, #4]		@ Src = [sp, #4]
	str	r4, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rowsrc_4x4	@ x1!=0
@if(src)
@{
@	if(dst !=src )
@	{
@		dst[0] = src[0]@
@		dst[1] = src[1]@
@		dst[2] = src[2]@
@		dst[3] = src[3]@
@		dst[4] = src[4]@
@		dst[5] = src[5]@
@		dst[6] = src[6]@
@		dst[7] = src[7]@
@	}
@	src += stride@
@}	
	cmp	r6, r7	
	moveq	pc, lr
	ldr	r2, [r6]
	ldr	r3, [r6, #4]								 		
	strd		r2, [r7]		
	mov	pc, lr

only_x1t_lab_Rowsrc_4x4:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@        dst[0] = SAT(((src[0] + E)))@
@        dst[7] = SAT(((src[7] + E)))@
@
@        dst[1] = SAT(((src[1] + E)))@
@        dst[2] = SAT(((src[2] + E)))@
@
@        dst[3] = SAT(((src[3] + E)))@
@        dst[4] = SAT(((src[4] + E)))@
@
@        dst[5] = SAT(((src[5] + E)))@
@        dst[6] = SAT(((src[6] + E)))@
@	src += stride@
@    }
@}
	mov	r4, #32
	sxtah	r4, r4, r2		@Blk[0] + 32
@	ldrd	r2, [r6]	@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
	movs	r4, r4, asr #6	
	streq	r2, [r7]
	streq	r3, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
	pkhbt	r1, r4, r4, lsl #16	@ r1 = E|E
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r8, r3, ror #8		@1, 3	b	
	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r1		@1, 3	a+e
	sadd16		r6, r6, r1		@0, 2	b+e
	sadd16		r8, r8, r1		@1, 3	b+e		
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r8,#8,r8		
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r8,lsl #8
	strd		r2, [r7]						
	mov	pc, lr	
	
only_x1x2t_lab_Rowsrc_4x4:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((Src[0] + ((E + A )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((E - A )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((E + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((E - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((E + B )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((E - B )>>17))))@	
@	Dst[2] = SAT_new(((Src[2] + ((E + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((E - Add )>>17))))@		 				
@}
	mov	r7, #32	
	smultb	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	sxtah	r7, r7, r2		@Blk[0] + 32
	mov	r7, r7, asl #11	@E = (Blk[0] + 32) << 11
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add

	mov	r1, r1, asr #17	@0
	mov	r2, r2, asr #17	@7
	mov	r8, r8, asr #17	@1
	mov	r4, r4, asr #17	@6
	mov	r5, r5, asr #17	@3
	mov	r3, r3, asr #17	@5	
	mov	r6, r6, asr #17	@4
	mov	r9, r9, asr #17	@2
	
	pkhbt	r7, r3, r2, lsl #16	@1, 3	b				
	ldr	r3, [sp, #4]		@ Src = [sp, #4]	
	ldr	r2, [sp, #52]		@ src_stride = [sp, #52]			
	pkhbt	r1, r1, r9, lsl #16	@0, 2	a
	pkhbt	r9, r8, r5, lsl #16	@1, 3	a
	pkhbt	r8, r6, r4, lsl #16	@0, 2	b
	add	r2, r2, r3
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r3]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r3, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
allx1_4_t_lab_Rowsrc:
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@
@ r2 = x1|x0 r3 = x3|x2

	mov	r1, #32			
	ldr	r9, WxC2xC6	@r10 = WxC2xC6			
	sxtah	r1, r1, r2		@Blk[0] + 32
	mov	r1, r1, asl #11	@E = (Blk[0] + 32) << 11				
	pkhtb	r2, r3, r2, asr #16	@ r2 = ip3|ip1
					@ r3 = ip2	r1 = ip0		
	smuad	r8, r2, r10	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	
	smusd	r5, r2, r10	@@Ad = A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])
	smuad	r6, r2, r11	@@Bd = B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))	
	
	sub	r7, r5, r6
	add	r6, r5, r6
	mov	r5, r7, asr #8	@(Ad - Bd)) >> 8
	mov	r6, r6, asr #8	@(Ad + Bd)) >> 8
	mul	r5, r12, r5	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r6, r12, r6	@Bdd = (181 * (((Ad + Bd)) >> 8))@			
	
	smulbt	r7, r3, r9	@G = W2 * Blk[2*8]
	smulbb	r2, r3, r9	@H = W6 * Blk[2*8]@	
			
@ r3  free now
	sub	r3, r1, r7	@Ed = E - G@
	add	r7, r1, r7	@Fd = E + G@
	sub	r9, r1, r2	@Gd = E - H@
	add	r1, r1, r2	@Hd = E + H@
	
@ r1 free now
	add	r2, r7, r8	@Fd + Cd	0
	sub	r7, r7, r8	@Fd - Cd	7
	add	r8, r1, r6	@Hd + Bdd	1
	sub	r1, r1, r6	@Hd - Bdd	6	
	add	r6, r3, r4	@Ed + Dd	3
	sub	r3, r3, r4	@Ed - Dd	4	
	add	r4, r9, r5	@Gd + Add	2
	sub	r5, r9, r5	@Gd - Add	5

	mov	r2, r2, asr #17	@0
	mov	r7, r7, asr #17	@7
	mov	r8, r8, asr #17	@1
	mov	r1, r1, asr #17	@6
	mov	r6, r6, asr #17	@3	
	mov	r3, r3, asr #17	@4
	mov	r4, r4, asr #17	@2
	mov	r5, r5, asr #17	@5	
	
	pkhbt	r9, r8, r6, lsl #16	@1, 3	a
	pkhbt	r8, r3, r1, lsl #16	@0, 2	b				
	pkhbt	r1, r2, r4, lsl #16	@0, 2	a	
	ldr	r3, [sp, #4]		@ Src = [sp, #4]	
	ldr	r2, [sp, #52]		@ src_stride = [sp, #52]			
	pkhbt	r7, r5, r7, lsl #16	@1, 3	b

	add	r2, r2, r3
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r3]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r3, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr	
	@ENDP

	.align 4
Row8nosrc_4x4:
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	cmp	r3, #0
	bne	allx1_4_t_lab_Rownosrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rownosrc_4x4	@ x2!=0
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r4, [sp, #8]		@ dst_stride = [sp, #8]
	add	r4, r7, r4	
	str	r4, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rownosrc_4x4	@ x1!=0
@@{
@	Dst[0] =
@	Dst[7] =
@	Dst[1] =
@	Dst[6] =
@	Dst[3] =
@	Dst[4] =
@	Dst[2] =
@	Dst[5] = 0@
@@}	
	mov	r2, #0
	mov	r3, #0
	strd		r2, [r7]		
	mov	pc, lr
	
only_x1t_lab_Rownosrc_4x4:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@@        dst[0] =
@@        dst[7] =
@@
@@        dst[1] =
@@        dst[2] =
@@
@@        dst[3] =
@@        dst[4] =
@@
@@        dst[5] =
@@        dst[6] = SAT_new(E)@@
@}
	mov	r4, #32
	sxtah	r1, r4, r2		@Blk[0] + 32
	usat	r2,#8, r1, asr #6	
	orr	r2, r2, r2, lsl #8
	orr	r2, r2, r2, lsl #16			
	mov	r3, r2
	strd		r2, [r7]						
	mov	pc, lr	
	
only_x1x2t_lab_Rownosrc_4x4:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((E + A )>>17))@
@	Dst[7] = SAT_new(((E - A )>>17))@	
@	Dst[1] = SAT_new(((E + Bdd )>>17))@
@	Dst[6] = SAT_new(((E - Bdd )>>17))@	
@	Dst[3] = SAT_new(((E + B )>>17))@
@	Dst[4] = SAT_new(((E - B )>>17))@	
@	Dst[2] = SAT_new(((E + Add )>>17))@
@	Dst[5] = SAT_new(((E - Add )>>17))@
@}
@ r2 = x1|x0
	mov	r7, #32
	smultb	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	sxtah	r7, r7, r2		@Blk[0] + 32
	mov	r7, r7, asl #11	@E = (Blk[0] + 32) << 11	
	
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	usat	r1, #8, r1, asr #17	@0
	usat	r2, #8, r2, asr #17	@7
	usat	r8, #8, r8, asr #17	@1
	usat	r4, #8, r4, asr #17	@6
	usat	r5, #8, r5, asr #17	@3
	usat	r3, #8, r3, asr #17	@5
	usat	r6, #8, r6, asr #17	@4
	usat	r9, #8, r9, asr #17	@2					

	orr	r8, r1, r8, lsl #8
	ldr	r1, [sp, #12]			@ dst = [sp, #12]	
	orr	r8, r8, r9, lsl #16
	orr	r9, r6, r3, lsl #8
	ldr	r3, [sp, #8]		@ dst_stride = [sp, #8]		
	orr	r9, r9, r4, lsl #16			
	orr	r4, r8, r5, lsl #24
	orr	r5, r9, r2, lsl #24
	add	r3, r1, r3
	strd	r4, [r1]	
	str	r3, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr

allx1_4_t_lab_Rownosrc:
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@@	E = Blk[0*8] << 11@	E = (Blk[0] + 32) << 11@
@@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@
@ r2 = x1|x0 r3 = x3|x2
	mov	r1, #32			
	ldr	r9, WxC2xC6	@r10 = WxC2xC6			
	sxtah	r1, r1, r2		@Blk[0] + 32
	mov	r1, r1, asl #11	@E = (Blk[0] + 32) << 11				
	pkhtb	r2, r3, r2, asr #16	@ r2 = ip3|ip1
					@ r3 = ip2	r1 = ip0		
	smuad	r8, r2, r10	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	
	smusd	r5, r2, r10	@@Ad = A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])
	smuad	r6, r2, r11	@@Bd = B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))	
	
	sub	r7, r5, r6
	add	r6, r5, r6
	mov	r5, r7, asr #8	@(Ad - Bd)) >> 8
	mov	r6, r6, asr #8	@(Ad + Bd)) >> 8
	mul	r5, r12, r5	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r6, r12, r6	@Bdd = (181 * (((Ad + Bd)) >> 8))@
	
	smulbt	r7, r3, r9	@G = W2 * Blk[2*8]
	smulbb	r2, r3, r9	@H = W6 * Blk[2*8]@	
			
@ r3  free now
	sub	r3, r1, r7	@Ed = E - G@
	add	r7, r1, r7	@Fd = E + G@
	sub	r9, r1, r2	@Gd = E - H@
	add	r1, r1, r2	@Hd = E + H@
	
@ r1 free now
	add	r2, r7, r8	@Fd + Cd	0
	sub	r7, r7, r8	@Fd - Cd	7
	add	r8, r1, r6	@Hd + Bdd	1
	sub	r1, r1, r6	@Hd - Bdd	6	
	add	r6, r3, r4	@Ed + Dd	3
	sub	r3, r3, r4	@Ed - Dd	4	
	add	r4, r9, r5	@Gd + Add	2
	sub	r5, r9, r5	@Gd - Add	5

	usat	r2, #8, r2, asr #17	@0
	usat	r8, #8, r8, asr #17	@1
	usat	r4, #8, r4, asr #17	@2
	usat	r6, #8, r6, asr #17	@3
	usat	r3, #8, r3, asr #17	@4
	usat	r5, #8, r5, asr #17	@5
	usat	r1, #8, r1, asr #17	@6
	usat	r7, #8, r7, asr #17	@7							

	orr	r2, r2, r8, lsl #8
	ldr	r8, [sp, #12]			@ dst = [sp, #12]	
	orr	r2, r2, r4, lsl #16
	orr	r3, r3, r5, lsl #8
	ldr	r5, [sp, #8]		@ dst_stride = [sp, #8]		
	orr	r3, r3, r1, lsl #16			
	orr	r2, r2, r6, lsl #24
	orr	r3, r3, r7, lsl #24
	add	r5, r8, r5
	strd	r2, [r8]	
	str	r5, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP
			
	.align 4
IdctBlock4x4_ARMV6:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]

	ldr	r10, WxC3xC1	@r10 = WxC3xC1
	ldr	r11, WxC5xC7	@r11 = WxC5xC7
	ldr	r12, Wx00xC4	@r12 = Wx00xC4
	
	bl      Col8_4x4  
	add     r0, r0, #2
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  	
	sub     r0, r0, #6
	
	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_lab_4x4		@ Src=0	

	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]                                           		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
only_noSrc_lab_4x4:

	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]

                                         		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        	
	@ENDP

	.align 4
IdctBlock8x8_ARMV6:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]	

	ldr	r10, WxC2xC6	@r10 = WxC2xC6
	ldr	r11, WxC1xC7	@r11 = WxC1xC7
	ldr	r12, Wx00xC4	@r12 = Wx00xC4	
	bl      Col8  
	add     r0, r0, #2
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8 
	sub     r0, r0, #14
	
	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_lab		@ Src=0	

	bl		Row8src
	add		r0, r0, #16			@Block += 16		
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0]                                              		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
        
only_noSrc_lab:
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0]                                             		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
	.align 4
IdctBlock4x8_ARMV6:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]	

	ldr	r10, WxC2xC6	@r10 = WxC2xC6
	ldr	r11, WxC1xC7	@r11 = WxC1xC7
	ldr	r12, Wx00xC4	@r12 = Wx00xC4	
	bl      Col8  
	add     r0, r0, #2
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8
	sub     r0, r0, #6
		
	ldr	r10, WxC3xC1	@r10 = WxC3xC1
	ldr	r11, WxC5xC7	@r11 = WxC5xC7
	ldr	r12, Wx00xC4	@r12 = Wx00xC4		
	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_labD		@ Src=0	

	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]                                           		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
        
only_noSrc_labD:
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	sub     r0, r0, #112	
	mov	r10, #0
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]                                           		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}

	.align 4

IdctBlock1x1_ARMV6:	
        STMFD    sp!,{r4-r11,lr}
		@ r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src
	ldrsh	r12, [r0]
	mov	r5, #0
	cmp	r3, #0	
	add	r12, r12, #4	
	strh	r5, [r0]	
	mov	r0, r12, asr #3
@	int32_t OutD = (Block[0]+4)>>3@		
	beq	IDCT_CPY_NOSRC_ARMv6	
		
	@const uint8_t* SrcEnd = Src + 8*SrcPitch (int SrcPitch = 8@)
		ldr		r14,[sp,#36]	@SrcPitch
        cmp     r0, #0    @if (v==0)                       
        bne     outCopyBlock8x8_asm_ARMv6     
		             
CopyBlock8x8_asm_ARMv6:
 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14 
 		ldrd     r8, [r3], r14        
 		ldrd     r10, [r3], r14	
        strd     r4, [r1], r2 
        strd     r6, [r1], r2 
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14 
 		ldrd     r8, [r3], r14        
 		ldrd     r10, [r3], r14	
        strd     r4, [r1], r2 
        strd     r6, [r1], r2 
        strd     r8, [r1], r2 
        strd     r10, [r1], r2
        LDMFD    sp!,{r4-r11,pc}

outCopyBlock8x8_asm_ARMv6:

		blt     little_begin_ARMv6 

big_begin_ARMv6_ARMv6:	
                                       
        orr     r12, r0, r0, lsl #8            
        orr     r0, r12, r12, lsl #16    
		        
 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	 r8, r4, r0
		uqadd8	 r9, r5, r0
		uqadd8	 r10, r6, r0
		uqadd8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	 r8, r4, r0
		uqadd8	 r9, r5, r0
		uqadd8	 r10, r6, r0
		uqadd8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	 r8, r4, r0
		uqadd8	 r9, r5, r0
		uqadd8	 r10, r6, r0
		uqadd8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqadd8	 r8, r4, r0
		uqadd8	 r9, r5, r0
		uqadd8	 r10, r6, r0
		uqadd8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 													                                             
        LDMFD    sp!,{r4-r11,pc}      
	
little_begin_ARMv6:
                      
        rsb     r12, r0, #0                       
        orr     r12, r12, r12, lsl #8            
        orr     r0, r12, r12, lsl #16            


 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	 r8, r4, r0
		uqsub8	 r9, r5, r0
		uqsub8	 r10, r6, r0
		uqsub8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	 r8, r4, r0
		uqsub8	 r9, r5, r0
		uqsub8	 r10, r6, r0
		uqsub8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	 r8, r4, r0
		uqsub8	 r9, r5, r0
		uqsub8	 r10, r6, r0
		uqsub8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 

 		ldrd     r4, [r3], r14        
 		ldrd     r6, [r3], r14   			             
		uqsub8	 r8, r4, r0
		uqsub8	 r9, r5, r0
		uqsub8	 r10, r6, r0
		uqsub8	 r11, r7, r0
        strd     r8, [r1], r2 
        strd     r10, [r1], r2 
        LDMFD    sp!,{r4-r11,pc}  
        
IDCT_CPY_NOSRC_ARMv6:	
@	OutD = SAT_new(OutD)@
	usat	r12, #8, r0
	orr     r10, r12, r12, lsl #8
	orr     r10, r10, r10, lsl #16
	mov	r11, r10	
		
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1]  
              		
        LDMFD    sp!,{r4-r11,pc}		        
	@ENDP                

 
	.align 4	
Arm11IdctC1:	
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	ldrsh	r12, [r0]
	mov	r5, #0
	add	r12, r12, #4	
	strh	r5, [r0]	
	mov	r12, r12, asr #3						
	cmp			r3, #0
@	int32_t OutD = (Block[0]+4)>>3@		
	beq			IDCT_CPY_NOSRC	
	ldr	r11,[sp, #36]	@src_stride
	pkhbt	r12, r12, r12, lsl #16			
	mov	r10, #8
CPYSRCADD_armv6_loop:	
		pld		[r3, r11, lsl #1]	
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
	uxtb16		r6, r4		    	@0, 2
	uxtb16		r7, r4, ror #8		@1, 3
	uxtb16		r8, r5		    	@0, 2
	uxtb16		r9, r5, ror #8		@1, 3
	        
	sadd16		r6, r12, r6		@0, 2
	sadd16		r7, r12, r7		@1, 3
	sadd16		r8, r12, r8		@0, 2
	sadd16		r9, r12, r9		@1, 3		
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7
	usat16	 r8,#8,r8
	usat16	 r9,#8,r9	
	orr	r6,r6,r7,lsl #8	
	orr	r7,r8,r9,lsl #8	
        subs	r10, r10, #1	
        strd     r6, [r1], r2
        bne	CPYSRCADD_armv6_loop 

        LDMFD    sp!,{r4-r11,pc}	
IDCT_CPY_NOSRC:	
@	OutD = SAT_new(OutD)@
	usat	r12, #8, r12
	orr     r10, r12, r12, lsl #8
	orr     r10, r10, r10, lsl #16
	mov	r11, r10	
		
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1]  
              		
        LDMFD    sp!,{r4-r11,pc}		        
	@ENDP

	@ENDP
	@END