@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/
	@AREA	 .text , CODE, READONLY
	#include "../../../../Inc/voVP6DecID.h"
	.section .text

	.global	 Copy16x12_Armv7
	
@	EXPORT VP6_FilteringHoriz_12_Armv7	
@	EXPORT VP6_FilteringVert_12_Armv7		

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
Copy16x12_Armv7: @PROC	
@ r0 = src, r1 = dst, r2 = srcstride
	vld1.32	{q0}, [r0], r2		@	
	vld1.32	{q1}, [r0], r2		@	
	vld1.32	{q2}, [r0], r2		@	
	vld1.32	{q3}, [r0], r2		@
	vld1.32	{q4}, [r0], r2		@	
	vld1.32	{q5}, [r0], r2		@	
	vld1.32	{q6}, [r0], r2		@	
	vld1.32	{q7}, [r0], r2		@	
	vld1.32	{q8}, [r0], r2		@	
	vld1.32	{q9}, [r0], r2		@	
	vld1.32	{q10}, [r0], r2		@	
	vld1.32	{q11}, [r0]		@
	
	vst1.64	{q0}, [r1]!		@	
	vst1.64	{q1}, [r1]!		@	
	vst1.64	{q2}, [r1]!		@	
	vst1.64	{q3}, [r1]!		@
	vst1.64	{q4}, [r1]!		@	
	vst1.64	{q5}, [r1]!		@	
	vst1.64	{q6}, [r1]!		@	
	vst1.64	{q7}, [r1]!		@	
	vst1.64	{q8}, [r1]!		@	
	vst1.64	{q9}, [r1]!		@	
	vst1.64	{q10}, [r1]!		@	
	vst1.64	{q11}, [r1]		@
		
	mov		pc,lr	        
	@ENDP

	.END