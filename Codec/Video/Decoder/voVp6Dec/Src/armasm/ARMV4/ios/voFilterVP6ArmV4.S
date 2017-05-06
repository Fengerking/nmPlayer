@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
	.text
	.globl _Copy16x12_Armv4
@	.globl VP6_FilteringHoriz_12_Armv4	
@	.globl VP6_FilteringVert_12_Armv4	
	.align 4		

@void Copy16x12_C(const UINT8 *src, UINT8 *dest, UINT32 srcstride)
@{
@	const unsigned int *s = (const unsigned int *)src@
@	unsigned int *d = (unsigned int *)dest@
@	srcstride = srcstride >> 2@
@	d[0]  = s[0]@
@	d[1]  = s[1]@
@	d[2]  = s[2]@
@	d[3]  = s[3]@
@	s += srcstride@
@	//d[0] ~d[47]
@	//total 12 times
@}
	.align 4	
_Copy16x12_Armv4: @PROC
        STMFD    sp!,{r4-r11,lr}	
@ r0 = src, r1 = dst, r2 = srcstride
	mov	r12, #6
loop_cpy:
@1
	@pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2
	subs 	r12, r12, #1		
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2	
	str	r4, [r1], #4
	str	r5, [r1], #4
	str	r6, [r1], #4
	str	r7, [r1], #4
	str	r8, [r1], #4
	str	r9, [r1], #4
	str	r10, [r1], #4
	str	r11, [r1], #4
	bne	loop_cpy
        
        LDMFD    sp!,{r4-r11,pc}	        
	@ENDP
	@.END	
