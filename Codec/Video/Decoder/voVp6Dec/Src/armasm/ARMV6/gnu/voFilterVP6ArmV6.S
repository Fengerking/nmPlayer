@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
#include "../../../../Inc/voVP6DecID.h"
	.section .text
	.global Copy16x12_Armv6
@	.global VP6_FilteringHoriz_12_Armv6	
@	.global VP6_FilteringVert_12_Armv6	
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
Copy16x12_Armv6: @PROC
        STMFD    sp!,{r4-r11,lr}	
@ r0 = src, r1 = dst, r2 = srcstride
@1
	pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2	
        strd	r4, [r1]
        strd	r6, [r1, #8]
        strd	r8, [r1, #16]
        strd	r10, [r1, #24] 
@2
	pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2
	pld	[r0, r2, lsl #1]	
        strd	r4, [r1, #32]
        strd	r6, [r1, #40]
        strd	r8, [r1, #48]
        strd	r10, [r1, #56] 
@3
	pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2	
        strd	r4, [r1, #64]
        strd	r6, [r1, #72]
        strd	r8, [r1, #80]
        strd	r10, [r1, #88] 
@4
	pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2	
        strd	r4, [r1, #96]
        strd	r6, [r1, #104]
        strd	r8, [r1, #112]
        strd	r10, [r1, #120]
@5
	pld	[r0, r2, lsl #1]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		
	ldr	r8, [r0], r2		
        strd	r4, [r1, #128]
        strd	r6, [r1, #136]
        strd	r8, [r1, #144]
        strd	r10, [r1, #152] 
@6
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]		
	ldr	r4, [r0], r2
	ldr	r8, [r0]	
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]		

        strd	r4, [r1, #160]
        strd	r6, [r1, #168]
        strd	r8, [r1, #176]
        strd	r10, [r1, #184]
        
        LDMFD    sp!,{r4-r11,pc}	        
	@ENDP
	.END	
