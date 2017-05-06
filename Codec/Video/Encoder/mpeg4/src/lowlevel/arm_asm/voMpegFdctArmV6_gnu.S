
	.global 	FastDct_ARMV6
	.global 	ExpandByteToShort_ARMV6

	.section .text
	
	
			
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@void Cpy8To16Armv6(signed short * const dst,
@					  const unsigned char * const src,
@					   const unsigned int stride)	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@r0 :dst
@r1 :src
@r2 :stride ->r14
@r12:0xff

ExpandByteToShort_ARMV6:

	stmdb	sp!, {r4 - r11, lr}
	mov		r14, r2

	pld		[r1]
	pld		[r1, r2]
	add		r2, r2, r14	
	pld		[r1, r2]
	add		r2, r2, r14 
	pld		[r1, r2]
	add		r2, r2, r14
	pld		[r1, r2]
	add		r2, r2, r14
	pld		[r1, r2]
	add		r2, r2, r14
	pld		[r1, r2]
	add		r2, r2, r14
	pld		[r1, r2]     
	
	ldrd	r2, [r1], r14  @|d|c|b|a|
	mov		r12, #0xff
	ldrd	r4, [r1], r14
	ldrd	r6, [r1], r14
@row 0	
	and		r10, r12, r2, lsr #8
	and		r8, r12, r2	
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r2, lsr #24
	and		r9, r12, r2, lsr #16	
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r3, lsr #8
	and		r8, r12, r3
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r3, lsr #24
	and		r9, r12, r3, lsr #16	
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
@row 1	
	and		r10, r12, r4, lsr #8
	and		r8, r12, r4
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r4, lsr #24	
	and		r9, r12, r4, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r5, lsr #8
	and		r8, r12, r5
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r5, lsr #24	
	and		r9, r12, r5, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
@row 2	
	and		r10, r12, r6, lsr #8
	and		r8, r12, r6
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r6, lsr #24	
	and		r9, r12, r6, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r7, lsr #8
	and		r8, r12, r7
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r7, lsr #24	
	and		r9, r12, r7, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
		
	ldrd	r2, [r1], r14  @|d|c|b|a|
	ldrd	r4, [r1], r14
	ldrd	r6, [r1], r14
@row 3	
	and		r10, r12, r2, lsr #8
	and		r8, r12, r2
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r2, lsr #24	
	and		r9, r12, r2, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r3, lsr #8
	and		r8, r12, r3
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r3, lsr #24	
	and		r9, r12, r3, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
@row 4	
	and		r10, r12, r4, lsr #8
	and		r8, r12, r4
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r4, lsr #24	
	and		r9, r12, r4, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r5, lsr #8
	and		r8, r12, r5
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r5, lsr #24	
	and		r9, r12, r5, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
@row 5	
	and		r10, r12, r6, lsr #8
	and		r8, r12, r6
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r6, lsr #24	
	and		r9, r12, r6, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r7, lsr #8
	and		r8, r12, r7
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r7, lsr #24	
	and		r9, r12, r7, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
		
	ldrd	r2, [r1], r14  @|d|c|b|a|
	ldrd	r4, [r1], r14
@row 6	
	and		r10, r12, r2, lsr #8
	and		r8, r12, r2
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r2, lsr #24	
	and		r9, r12, r2, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8	
	and		r10, r12, r3, lsr #8
	and		r8, r12, r3
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r3, lsr #24	
	and		r9, r12, r3, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
@row 7	
	and		r10, r12, r4, lsr #8
	and		r8, r12, r4
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r4, lsr #24	
	and		r9, r12, r4, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8
	and		r10, r12, r5, lsr #8
	and		r8, r12, r5
	orr		r8, r8, r10, lsl #16  @ |0|b|0|a|
	and		r10, r12, r5, lsr #24	
	and		r9, r12, r5, lsr #16
	orr		r9, r9, r10, lsl #16  @ |0|d|0|c|
	strd	r8, [r0], #8			
	ldmia	sp!, {r4 - r11, pc}

	
FastDct_ARMV6:

	stmdb   	sp!, {r4 - r11, lr}
	
	mov			r1, r0	 
	ldrd		r2, [r0], #8			@ r2 = [1][0] r3 = [3][2]
	mov			r12, #8		
	mov			lr, #1, 16
	ldrd		r4, [r0], #8			@ r4 = [5][4] r5 = [7][6]
	orr			lr, lr, #1					@ lr = 0x10001
	ldr			r11, FR11
	ldr			r10, FR10
	ldr			r9, FR9
	ldr			r8, FR8
		
ROW:		
	saddsubx	r6, r2, r5				@ r6 = [1+6][0-7] [tmp1][tmp7]
	ssubaddx	r2, r2, r5				@ r2 = [1-6][0+7] [tmp6][tmp0]
	
	saddsubx	r5, r3, r4				@ r5 = [3+4][2-5] [tmp3][tmp5]
	ssubaddx	r3, r3, r4				@ r3 = [3-4][2+5] [tmp4][tmp2]
	
	subs		r12, r12, #1
	pkhtb		r4, r5, r6, asr #16		@ r4 = [tmp3][tmp1]
	pkhbt		r7, r3, r2, lsl #16 	@ r7 = [tmp0][tmp2]
	
	pkhtb		r3, r3, r6			 	@ r3 = [tmp4][tmp7]
	pkhbt		r2, r5, r2				@ r2 = [tmp6][tmp5]
	
	sadd16		r5, r4, r7 				@ r5 = [tmp3+tmp0][tmp1+tmp2] = [tmp10][tmp11]
	ssub16		r6 ,r4, r7				@ r6 = [tmp3-tmp0][tmp1-tmp2] = [-tmp13][tmp12]

	smusdx	  	r4, lr, r5				@ r4 = tmp10 - tmp11
	smuad	  	r5, lr, r5				@ r5 = tmp10 + tmp11
	mov			r4, r4, lsl #2
	mov			r5, r5, lsl #2
			
	strh		r4, [r1, #8]			@ r4 = dst[4]
	strh		r5, [r1]				@ r5 = dst[0]
	
	smusdx	  	r4, r11, r6				@ r4 = 4433*tmp13-10703*tmp12 @ r4 = 10703*tmp13+4433*tmp12
	smuad	  	r5, r11, r6				@ r5 = 4433*tmp12+10703*tmp13 @ r5 = -10703*tmp12+4433*tmp13	
	
	add			r4, r4, #1<<10			@ + RND
	add			r5, r5, #1<<10
	mov			r4, r4, asr #11
	mov			r5, r5, asr #11
	sadd16		r6, r2, r3				@ r6 = [tmp4+tmp6][tmp5+tmp7]=[z3][z4]
	strh		r5, [r1, #12]			@ r5 = dst[6]	
	strh		r4, [r1, #4]			@ r4 = dst[2]
	
	
	smusd	  	r4, r10, r6				@ r4 = z3 = 9633*z4-6436*z3
	smuadx	  	r5, r10, r6				@ r5 = z4 = 9633*z3+6436*z4	
	
	smusd	  	r6, r9, r3				@ r6 = z1 + tmp4 = (-7373)*tmp7-4926*tmp4
	smuadx	  	r3, r9, r3				@ r3 = z1 + tmp7 = (-7373)*tmp4+4926*tmp7	
	
	
	smusdx	  	r7, r8, r2				@ r7 = z2 + tmp5 = (-20995)*tmp6-4176*tmp5
	smuad	  	r2, r8, r2				@ r2 = z2 + tmp6 = (-20995)*tmp5+4176*tmp6	
	
		
	add			r6, r6, r4				@ r6 = tmp4 + z1 + z3	dst[7]
	add			r3, r3, r5				@ r3 = tmp7 + z1 + z4	dst[1]
	add			r7, r7, r5				@ r7 = tmp5 + z2 + z4	dst[5]
	add			r2, r2, r4				@ r2 = tmp6 + z2 + z3	dst[3]
	add			r6, r6, #1<<10			@ + RND
	add			r3, r3, #1<<10
	add			r7, r7, #1<<10
	add			r2, r2, #1<<10
	mov			r3, r3, asr #11
	mov			r2, r2, asr #11
	strh		r3, [r1, #2]
	strh		r2, [r1, #6]
	mov			r7, r7, asr #11
	ldrd		r2, [r0], #8			@ r2 = [1][0] r3 = [3][2]
	mov			r6, r6, asr #11
	ldrd		r4, [r0], #8			@ r2 = [1][0] r3 = [3][2]		
	strh		r7, [r1, #10]
	strh		r6, [r1, #14]
	addne		r1, r1, #16
	
	bne			ROW
	
	sub			r0, r0, #128+16
	mov			r12, #8
	
	
	@ r2 = [1][0] r3 = [3][2]
	@ r4 = [5][4] r5 = [7][6]
COL:	
	ldrh		r2, [r0]				@ r2 = [0]
	ldrh		r5, [r0, #16]			@ r5 = [1]
	ldrh		r3, [r0, #32]			@ r3 = [2]
	ldrh		r6, [r0, #48]			@ r6 = [3]
	ldrh		r4, [r0, #64]			@ r4 = [4]
	ldrh		r7, [r0, #80]			@ r7 = [5]
	
	pkhbt		r2, r2, r5, lsl #16		@ r2 = [1][0]
	pkhbt		r3, r3, r6, lsl #16		@ r3 = [3][2]
	ldrh		r5, [r0, #96]			@ r5 = [6]
	ldrh		r6, [r0, #112]			@ r6 = [7]
	pkhbt		r4, r4, r7, lsl #16		@ r4 = [5][4]
	subs		r12, r12, #1
	pkhbt		r5, r5, r6, lsl	#16		@ r5 = [7][6]
	
	saddsubx	r6, r2, r5				@ r6 = [1+6][0-7] [tmp1][tmp7]
	ssubaddx	r2, r2, r5				@ r2 = [1-6][0+7] [tmp6][tmp0]
	
	saddsubx	r5, r3, r4				@ r5 = [3+4][2-5] [tmp3][tmp5]
	ssubaddx	r3, r3, r4				@ r3 = [3-4][2+5] [tmp4][tmp2]
	
	pkhtb		r4, r5, r6, asr #16		@ r4 = [tmp3][tmp1]
	pkhbt		r7, r3, r2, lsl #16 	@ r7 = [tmp0][tmp2]
	
	pkhtb		r3, r3, r6			 	@ r3 = [tmp4][tmp7]
	pkhbt		r2, r5, r2				@ r2 = [tmp6][tmp5]
	
	sadd16		r5, r4, r7 				@ r5 = [tmp3+tmp0][tmp1+tmp2] = [tmp10][tmp11]
	ssub16		r6 ,r4, r7				@ r6 = [tmp3-tmp0][tmp1-tmp2] = [-tmp13][tmp12]

	smusdx	  	r4, lr, r5				@ r4 = tmp10 - tmp11
	smuad	  	r5, lr, r5				@ r5 = tmp10 + tmp11
	
	add			r4, r4, #1<<4			@ + RND
	add			r5, r5, #1<<4
	mov			r4, r4, asr #5
	mov			r5, r5, asr #5
			
	strh		r4, [r0, #64]			@ r4 = dst[4]
	strh		r5, [r0]				@ r5 = dst[0]
	
	smusdx	  	r4, r11, r6				@ r4 = 4433*tmp13-10703*tmp12 @ r4 = 10703*tmp13+4433*tmp12
	smuad	  	r5, r11, r6				@ r5 = 4433*tmp12+10703*tmp13 @ r5 = -10703*tmp12+4433*tmp13
	add			r4, r4, #1<<17			@ + RND
	add			r5, r5, #1<<17
	mov			r4, r4, asr #18
	mov			r5, r5, asr #18
	
	sadd16		r6, r2, r3				@ r6 = [tmp4+tmp6][tmp5+tmp7]=[z3][z4]
	strh		r5, [r0, #96]			@ r5 = dst[6]	
	strh		r4, [r0, #32]			@ r4 = dst[2]
	
	smusd	  	r4, r10, r6				@ r4 = z3 = 9633*z4-6436*z3
	smuadx	  	r5, r10, r6				@ r5 = z4 = 9633*z3+6436*z4	
	
	smusd	  	r6, r9, r3				@ r6 = z1 + tmp4 = (-7373)*tmp7-4926*tmp4
	smuadx	  	r3, r9, r3				@ r3 = z1 + tmp7 = (-7373)*tmp4+4926*tmp7	
	
	
	smusdx	  	r7, r8, r2				@ r7 = z2 + tmp5 = (-20995)*tmp6-4176*tmp5
	smuad	  	r2, r8, r2				@ r2 = z2 + tmp6 = (-20995)*tmp5+4176*tmp6	
	
		
	add			r6, r6, r4				@ r6 = tmp4 + z1 + z3 		dst[7]
	add			r3, r3, r5				@ r3 = tmp7 + z1 + z4		dst[1]
	add			r7, r7, r5				@ r7 = tmp5 + z2 + z4		dst[5]
	add			r2, r2, r4				@ r2 = tmp6 + z2 + z3		dst[3]
	add			r6, r6, #1<<17			@ + RND
	add			r3, r3, #1<<17
	add			r7, r7, #1<<17
	add			r2, r2, #1<<17
	mov			r6, r6, asr #18
	mov			r3, r3, asr #18
	mov			r7, r7, asr #18
	mov			r2, r2, asr #18
		
	strh		r6, [r0, #112]
	strh		r3, [r0, #16]
	strh		r7, [r0, #80]
	strh		r2, [r0, #48]
	addne		r0, r0, #2
	bne			COL	
				
	ldmia   	sp!, {r4 - r11, pc} 

	
.ALIGN 8
FR11:
			.word   0xEEAFD631				@ r11 = -4433|-10703
FR10:
			.word	0x192425A1				@ r10 = 6436|9633
FR9:
				.word	0x133EE333				@ r9 = 4926|-7373
FR8:
				.word	0x1050ADFD				@ r8 = 4176|-20995

	.END