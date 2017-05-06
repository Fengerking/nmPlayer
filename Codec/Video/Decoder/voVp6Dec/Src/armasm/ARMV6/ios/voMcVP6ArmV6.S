@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
	#include "../../../../Inc/voVP6DecID.h"
	.text
	.align 4
	.globl _FilterBlock1dBil_wRecon_Armv6
	.globl _FilterBlock2dBil_wRecon_Armv6
	.globl _FilterBlock1d_wRecon_Armv6
	.globl _FilterBlock2d_wRecon_Armv6			

_FilterBlock1dBil_wRecon_Armv6: @PROC
@void FilterBlock1dBil_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	Srcstride,	r3
@	INT32	LineStep,	r4
@	FILTER_DATA  *Filter 	r5
@)
@{
@	UINT32 i, j@
@	// accomodate incrementing SrcPtr++ each time.
@	Srcstride -= 8@
@	for (i = 0@ i < 8@ i++)
@	{
@		for (j = 0@ j < 8@ j++)
@		{
@			INT16 Temp@
@			// Apply filter 
@			// NOTE: Rounding doesn't improve accuracy but is 
@			//       easier to implement on certain platforms.
@			Temp = (INT16)((((INT32)SrcPtr[0]	 * Filter[0])
@				 +	((INT32)SrcPtr[PixelStep]*Filter[1])
@				 +	(FILTER_WEIGHT/2) ) >> FILTER_SHIFT )@
@			dstPtr[j] = (UINT8)Temp@
@			SrcPtr++@
@		}
@		// Next row...
@		SrcPtr   += Srcstride@
@		//diffPtr += BLOCK_HEIGHT_WIDTH@
@		dstPtr += LineStep@
@	}
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@LineStep
	ldr	r5,[sp,#40]	@Filter
	cmp	r2, #1
	
	mov	r14, #8
	mov	r12, #64		
	ldr	r2, [R5]
	ldr	r5, [R5, #4]	
			
	bne	FilterBlock1dBil_wRecon_Armv6_V
						
H01V00_1dBil_loop:
@first 0~3		
        ldrb     r6,[r0,#0]		@lTemp0
        ldrb     r7,[r0,#1]		@lTemp1   
        ldrb     r8,[r0,#2]		@lTemp2            
        ldrb     r9,[r0,#3]		@lTemp3 

		smlabb	 r10,r2,r6,r12
		smlabb	 r11,r2,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7

        	ldrb     r6,[r0,#4]		@lTemp4					
	orr		 r10,r10,r11,LSL #8
				
		smlabb	 r7,r2,r8,r12
		smlabb	 r11,r2,r9,r12
		
		smlabb	 r7,r5,r9,r7
		smlabb	 r11,r5,r6,r11
					
		usat	 r7,#8,r7,ASR #7						
		usat	 r11,#8,r11,ASR #7
	orr		 r10,r10,r7,LSL #16
	orr		 r10,r10,r11,LSL #24
@second 0~3		
        ldrb     r7,[r0,#5]		@lTemp0
        ldrb     r8,[r0,#6]		@lTemp1   
        ldrb     r9,[r0,#7]		@lTemp2
		str      r10,[r1]        

		smlabb	 r10,r2,r6,r12
		smlabb	 r11,r2,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7

        	ldrb     r6,[r0,#8]		@lTemp4					
	orr		 r10,r10,r11,LSL #8
				
		smlabb	 r7,r2,r8,r12
		smlabb	 r11,r2,r9,r12
		
		smlabb	 r7,r5,r9,r7
		smlabb	 r11,r5,r6,r11
					
		usat	 r7,#8,r7,ASR #7						
		usat	 r11,#8,r11,ASR #7
	orr		 r10,r10,r7,LSL #16
	orr		 r10,r10,r11,LSL #24							

		subs	r14,r14,#1
		str      r10,[r1, #4]			
		add	r0, r0, r3
		add	r1, r1, r4		
		bgt		H01V00_1dBil_loop	
	
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1dBil_wRecon_Armv6_V:
					
H00V01_1dBil_loop:
@first 0~3		
        ldrb     r6,[r0], r3		@lTemp0
        ldrb     r7,[r0], r3		@lTemp1   
        ldrb     r8,[r0], r3		@lTemp2            
        ldrb     r9,[r0], r3		@lTemp3 

		smlabb	 r10,r2,r6,r12
		smlabb	 r11,r2,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7

        	ldrb     r6,[r0], r3		@lTemp4
        	strb	 r10, [r1], r4
        	strb	 r11, [r1], r4
				
		smlabb	 r10,r2,r8,r12
		smlabb	 r11,r2,r9,r12
		
		smlabb	 r10,r5,r9,r10
		smlabb	 r11,r5,r6,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7
        	strb	 r10, [r1], r4
        	strb	 r11, [r1], r4
@second 0~3		
        ldrb     r7,[r0], r3		@lTemp0
        ldrb     r8,[r0], r3		@lTemp1   
        ldrb     r9,[r0], r3		@lTemp2

		smlabb	 r10,r2,r6,r12
		smlabb	 r11,r2,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7

        	ldrb     r6,[r0]		@lTemp4					
        	strb	 r10, [r1], r4
        	strb	 r11, [r1], r4
				
		smlabb	 r10,r2,r8,r12
		smlabb	 r11,r2,r9,r12
		
		smlabb	 r10,r5,r9,r10
		smlabb	 r11,r5,r6,r11
					
		usat	 r10,#8,r10,ASR #7						
		usat	 r11,#8,r11,ASR #7
        	strb	 r10, [r1], r4
        	strb	 r11, [r1], r4							

		sub	r0, r0, r3, lsl #3			
		sub	r1, r1, r4, lsl #3
		subs	r14,r14,#1
		add	r0, r0, #1
		add	r1, r1, #1
		bgt		H00V01_1dBil_loop
		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP     
	
	
_FilterBlock2dBil_wRecon_Armv6: @PROC
@void FilterBlock2dBil_wRecon_C 
@(
@	UINT8 *SrcPtr, 		r0
@	UINT8 *dstPtr, 		r1
@	UINT32 SrcPixelsPerLine,r2
@	UINT32 LineStep, 	r3
@	FILTER_DATA * HFilter, 	r4
@	FILTER_DATA * VFilter 	r5 
@)
@{
@	INT32 FData[BLOCK_HEIGHT_WIDTH*11]@	// Temp data bufffer used in filtering	
@	// First filter 1-D horizontally...
@	FilterBlock2dBilFirstPass_wRecon_C ( SrcPtr, FData, SrcPixelsPerLine, HFilter )@	
@	// then 1-D vertically...
@	FilterBlock2dBilSecondPass_wRecon_C ( (UINT32 *)FData, dstPtr, LineStep, VFilter )@
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
        sub      sp,sp,#176	@(8+3)*8*2 = 176

        str	r1, [sp]
        add	r1, sp, #8        

	ldr	r5,[sp,#212]	@HFilter
@	ldr	r5,[sp,#216]	@VFilter
@H
	mov	r14, #9
	mov	r12, #64		
	ldr	r4, [R5]
	ldr	r5, [R5, #4]
						
H01V00_2dBil_loop:
@first 0~3		
        ldrb     r6,[r0,#0]		@lTemp0
        ldrb     r7,[r0,#1]		@lTemp1   
        ldrb     r8,[r0,#2]		@lTemp2            
        ldrb     r9,[r0,#3]		@lTemp3 

		smlabb	 r10,r4,r6,r12
		smlabb	 r11,r4,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		mov	 r10,r10,ASR #7						
		mov	 r11,r11,ASR #7

        	ldrb     r6,[r0,#4]		@lTemp4					
	orr		 r10,r10,r11,LSL #16			
		smlabb	 r7,r4,r8,r12
		smlabb	 r11,r4,r9,r12
		str	R10, [r1], #4		
		
		smlabb	 r7,r5,r9,r7
		smlabb	 r11,r5,r6,r11
					
		mov	 r7, r7,ASR #7						
		mov	 r11,r11,ASR #7
@second 0~3		

        ldrb     r8,[r0,#6]		@lTemp1 
        ldrb     r9,[r0,#7]		@lTemp2          
	orr		 r10,r7,r11,LSL #16
        ldrb     r7,[r0,#5]		@lTemp0	        

		str	R10, [r1], #4        

		smlabb	 r10,r4,r6,r12
		smlabb	 r11,r4,r7,r12
		
		smlabb	 r10,r5,r7,r10
		smlabb	 r11,r5,r8,r11
					
		mov	 r10,r10,ASR #7						
		mov	 r11,r11,ASR #7

        	ldrb     r6,[r0,#8]		@lTemp4					
	orr		 r10,r10,r11,LSL #16
				
		smlabb	 r7,r4,r8,r12
		smlabb	 r11,r4,r9,r12
		str	R10, [r1], #4
				
		smlabb	 r7,r5,r9,r7
		smlabb	 r11,r5,r6,r11
					
		mov	 r7,r7,ASR #7						
		mov	 r11,r11,ASR #7							
		subs	r14,r14,#1
	orr		 r10,r7,r11,LSL #16					
		add	r0, r0, r2		
		str	R10, [r1], #4		
		bgt		H01V00_2dBil_loop	
	
@V
        ldr	r1, [sp]
        add	r4, sp, #8
        
	ldr	r5,[sp,#216]	@VFilter	

	mov	r14, #8
	mov	r12, #64		
	ldr	r2, [R5]
	ldr	r5, [R5, #4]
H00V01_2dBil_loop:
@first 0~3		
        ldr	r6,[r4], #16			@lTemp0 a = 0, 1
        ldr	r7,[r4], #16			@lTemp1 b = 0, 1                 
        ldr	r8,[r4], #16			@lTemp2 c = 0, 1  
        ldr	r9,[r4], #16			@lTemp3 d = 0, 1

		smlabb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		usat	 r0,#8,r10,ASR #7
		smlabt	 r10,r2,r6,r12		
		smlabt	 r10,r5,r7,r10
		usat	 r10,#8,r10,ASR #7
	orr		 r10,r0,r10,LSL #8
        						
		smlabb	 r11,r2,r7,r12		
		smlabb	 r11,r5,r8,r11				
		usat	 r0,#8,r11,ASR #7					
		smlabt	 r11,r2,r7,r12		
		smlabt	 r11,r5,r8,r11				
		usat	 r11,#8,r11,ASR #7
	orr		 r11,r0,r11,LSL #8
        	strh	 r10, [r1], r3						
        	strh	 r11, [r1], r3
        	
        	ldr	r6,[r4], #16			@lTemp0 a = 0, 1
				
		smlabb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		usat	 r0,#8,r10,ASR #7		
		smlabt	 r10,r2,r8,r12
		smlabt	 r10,r5,r9,r10
		usat	 r10,#8,r10,ASR #7
	orr		 r10,r0,r10,LSL #8		
        	        											
		smlabb	 r11,r2,r9,r12
		smlabb	 r11,r5,r6,r11				
		usat	 r0,#8,r11,ASR #7
		smlabt	 r11,r2,r9,r12
		smlabt	 r11,r5,r6,r11				
		usat	 r11,#8,r11,ASR #7
	orr		 r11,r0,r11,LSL #8
        	strh	 r10, [r1], r3						
        	strh	 r11, [r1], r3
@second 0~3		
        ldr	r7,[r4], #16			@lTemp1 b = 0, 1                 
        ldr	r8,[r4], #16			@lTemp2 c = 0, 1  
        ldr	r9,[r4], #16			@lTemp3 d = 0, 1

		smlabb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		usat	 r0,#8,r10,ASR #7		
		smlabt	 r10,r2,r6,r12
		smlabt	 r10,r5,r7,r10
		usat	 r10,#8,r10,ASR #7
	orr		 r10,r0,r10,LSL #8
        												
		smlabb	 r11,r2,r7,r12
		smlabb	 r11,r5,r8,r11				
		usat	 r0,#8,r11,ASR #7
		smlabt	 r11,r2,r7,r12
		smlabt	 r11,r5,r8,r11			
		usat	 r11,#8,r11,ASR #7
	orr		 r11,r0,r11,LSL #8
        	strh	 r10, [r1], r3						
        	strh	 r11, [r1], r3
        	
        	ldr	r6,[r4], #16			@lTemp0 a = 0, 1
				
		smlabb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		usat	 r0,#8,r10,ASR #7
		smlabt	 r10,r2,r8,r12
		smlabt	 r10,r5,r9,r10
		usat	 r10,#8,r10,ASR #7
	orr		 r10,r0,r10,LSL #8
		smlabb	 r11,r2,r9,r12				
											
		smlabb	 r11,r5,r6,r11
		usat	 r0,#8,r11,ASR #7
		smlabt	 r11,r2,r9,r12
		smlabt	 r11,r5,r6,r11				
		usat	 r11,#8,r11,ASR #7
	orr		 r11,r0,r11,LSL #8					
        	strh	 r10, [r1], r3
        	strh	 r11, [r1], r3					

        sub      r4,r4,#140	@144 - 4			
		sub	r1, r1, r3, lsl #3
		subs	r14,r14,#2
		add	r1, r1, #2
		bgt		H00V01_2dBil_loop
	
       add      sp,sp,#176	@(8+3)*8*2 = 176		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	
	
_FilterBlock1d_wRecon_Armv6: @PROC
@void FilterBlock1d_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	Srcstride,	r3
@	INT32	LineStep,	r4
@	FILTER_DATA  *Filter 	r5
@)
@{
@	UINT32 i, j@
@	INT32  Temp@ 
@
@	// SrcPtr is increment each time in the inner loop, 8 in all.
@	SrcPixelsPerLine -= 8@
@	for (i = 0@ i < 8@ i++)
@	{
@		for (j = 0@ j < 8@ j++)
@		{
@			// Apply filter...
@			Temp =  -((INT32)SrcPtr[-(INT32)PixelStep]* Filter[0]) +
@					((INT32)SrcPtr[0]* Filter[1]) +
@					((INT32)SrcPtr[PixelStep]* Filter[2]) +
@					-((INT32)SrcPtr[2*PixelStep]* Filter[3]) + 
@					(FILTER_WEIGHT >> 1)@       // Rounding
@			// Normalize back to 0-255
@			Temp = Temp >> FILTER_SHIFT@
@			if ( Temp < 0 ) Temp = 0@
@			else if ( Temp > 255 ) Temp = 255@
@			
@			dstPtr[j] = (UINT8)Temp@
@			SrcPtr++@
@		}	
@		// Next row...
@		SrcPtr  += SrcPixelsPerLine@
@		//diffPtr += 8@
@		dstPtr  += LineStep@
@	}
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@LineStep
	ldr	r5,[sp,#40]	@Filter
	cmp	r2, #1
	mov	r14, #8		
	ldr	r6, [R5]
	ldr	r7, [R5, #4]
	ldr	r8, [R5, #8]
	ldr	r9, [R5, #12]
	orr	r2,r6,r7,LSL #16
	orr	r5,r8,r9,LSL #16				
	bne	FilterBlock1d_wRecon_Armv6_V
	sub	r0, r0, #1										
H01V00_1d_loop:		
        ldrb     r6,[r0,#0]		@lTemp0
        ldrb     r7,[r0,#1]		@lTemp1   
        ldrb     r8,[r0,#2]		@lTemp2            
        ldrb     r9,[r0,#3]		@lTemp3       
@6, 7, 8, 9
		smultb	 r10,r2,r7
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	 r12,#8,r10,ASR #7						

        	ldrb     r6,[r0,#4]		@lTemp4
@7, 8, 9, 6		
		smultb	 r10,r2,r8
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7
		smlatb	 r11,r5,r6,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #8
		
        	ldrb     r7,[r0,#5]		@lTemp5
@8, 9, 6, 7		
		smultb	 r10,r2,r9
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #16
		
        	ldrb     r8,[r0,#6]		@lTemp6
@9, 6, 7, 8		
		smultb	 r10,r2,r6
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #24
		
        	ldrb     r9,[r0,#7]		@lTemp7
		str	R12, [r1]        	
@6, 7, 8, 9		
		smultb	 r10,r2,r7
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	 r12,#8,r10,ASR #7							
	
        	ldrb     r6,[r0,#8]		@lTemp8
@7, 8, 9, 6		
		smultb	 r10,r2,r8
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7
		smlatb	 r11,r5,r6,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #8
		
        	ldrb     r7,[r0,#9]		@lTemp9
@8, 9, 6, 7		
		smultb	 r10,r2,r9
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #16
		
        	ldrb     r8,[r0,#10]		@lTemp10
@9, 6, 7, 8		
		smultb	 r10,r2,r6
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11
		add	R10, R10, #64
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r12,r12,r10,LSL #24						

		subs	r14,r14,#1	
		str	R12, [r1, #4]		
		add	r0, r0, r3
		add	r1, r1, r4		
		bgt		H01V00_1d_loop	
	
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1d_wRecon_Armv6_V:
	sub	r0, r0, r3
	mov	r12, #64					
H00V01_1d_loop:
        ldrb     r6,[r0], r3		@lTemp0
        ldrb     r7,[r0], r3		@lTemp1   
        ldrb     r8,[r0], r3		@lTemp2            
        ldrb     r9,[r0], r3		@lTemp3       
@6, 7, 8, 9
		smlatb	 r10,r2,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11
		sub	R10, R10, R11			
		usat	 r10,#8,r10,ASR #7						
        	ldrb     r6,[r0], r3		@lTemp4
        	strb	 r10, [r1], r4        	
@7, 8, 9, 6		
		smlatb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7
		smlatb	 r11,r5,r6,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
        	ldrb     r7,[r0], r3		@lTemp5
        	strb	 r10, [r1], r4        	
@8, 9, 6, 7		
		smlatb	 r10,r2,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
        	ldrb     r8,[r0], r3		@lTemp6
        	strb	 r10, [r1], r4        	
@9, 6, 7, 8		
		smlatb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
        	ldrb     r9,[r0], r3		@lTemp7     	
       		strb	 r10, [r1], r4        	
@6, 7, 8, 9		
		smlatb	 r10,r2,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11
		sub	R10, R10, R11			
		usat	 r10,#8,r10,ASR #7
        	ldrb     r6,[r0], r3		@lTemp8
        	strb	 r10, [r1], r4        	
@7, 8, 9, 6		
		smlatb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7
		smlatb	 r11,r5,r6,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
        	ldrb     r7,[r0]		@lTemp9
        	strb	 r10, [r1], r4        	
@8, 9, 6, 7		
		smlatb	 r10,r2,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
        	ldrb     r8,[r0, r3]	@lTemp10
        	strb	 r10, [r1], r4        	
@9, 6, 7, 8		
		smlatb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		subs	r14,r14,#1
        	strb	 r10, [r1], r4
 
		add	R10, R3, R3, LSL #3			
		sub	r1, r1, r4, lsl #3
		sub	r0, r0, r10
		add	r0, r0, #1
		add	r1, r1, #1
		bgt		H00V01_1d_loop
		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	
_FilterBlock2d_wRecon_Armv6: @PROC
@void FilterBlock2d_wRecon_C 
@(
@	UINT8 *SrcPtr, 		r0
@	UINT8 *dstPtr, 		r1
@	UINT32 SrcPixelsPerLine,r2
@	UINT32 LineStep, 	r3
@	FILTER_DATA * HFilter, 	r4
@	FILTER_DATA * VFilter 	r5 
@)
@{
@	INT32 FData[BLOCK_HEIGHT_WIDTH*11]@	// Temp data bufffer used in filtering
@
@	// First filter 1-D horizontally...
@	FilterBlock2dFirstPass_wRecon_C ( SrcPtr-SrcPixelsPerLine, FData, SrcPixelsPerLine, HFilter )@
@	
@	// then filter verticaly...
@	FilterBlock2dSecondPass_wRecon_C ( FData+BLOCK_HEIGHT_WIDTH, dstPtr, LineStep, VFilter )@
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
        sub      sp,sp,#184	@(8+3)*8*2 = 176 + 4 + 4
        str	r1, [sp]
        str	r3, [sp, #4]
        add	r1, sp, #8        
	ldr	r5,[sp,#220]	@HFilter
@	ldr	r5,[sp,#224]	@VFilter
@H
	sub	r0, r0, r2
	sub	r0, r0, #1
	mov	r14, #11
	mov	r12, #64
	ldr	r6, [R5]
	ldr	r7, [R5, #4]
	ldr	r8, [R5, #8]
	ldr	r9, [R5, #12]
	orr	r4,r6,r7,LSL #16
	orr	r5,r8,r9,LSL #16
						
H01V00_2d_loop:
        ldrb     r6,[r0,#0]		@lTemp0
        ldrb     r7,[r0,#1]		@lTemp1   
        ldrb     r8,[r0,#2]		@lTemp2            
        ldrb     r9,[r0,#3]		@lTemp3       
@6, 7, 8, 9
		smlatb	 r10,r4,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r4,r6
		smlatb	 r11,r5,r9,r11
		sub	R10, R10, R11			
		usat	 r3,#8,r10,ASR #7						
        	ldrb     r6,[r0,#4]		@lTemp4
@7, 8, 9, 6		
		smlatb	 r10,r4,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r4,r7
		smlatb	 r11,r5,r6,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r3,r3,r10,LSL #16
		str	R3, [R1], #4		
        	ldrb     r7,[r0,#5]		@lTemp5
@8, 9, 6, 7		
		smlatb	 r10,r4,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r4,r8
		smlatb	 r11,r5,r7,r11
		sub	R10, R10, R11			
		usat	r3,#8,r10,ASR #7
        	ldrb     r8,[r0,#6]		@lTemp6
@9, 6, 7, 8		
		smlatb	 r10,r4,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r4,r9
		smlatb	 r11,r5,r8,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r3,r3,r10,LSL #16
		str	R3, [R1], #4	
        	ldrb     r9,[r0,#7]		@lTemp7      	
@6, 7, 8, 9		
		smlatb	 r10,r4,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r4,r6
		smlatb	 r11,r5,r9,r11
		sub	R10, R10, R11			
		usat	 r3,#8,r10,ASR #7
        	ldrb     r6,[r0,#8]		@lTemp8
@7, 8, 9, 6		
		smlatb	 r10,r4,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r4,r7
		smlatb	 r11,r5,r6,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r3,r3,r10,LSL #16
		str	R3, [R1], #4
        	ldrb     r7,[r0,#9]		@lTemp9
@8, 9, 6, 7		
		smlatb	 r10,r4,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r4,r8
		smlatb	 r11,r5,r7,r11
		sub	R10, R10, R11			
		usat	r3,#8,r10,ASR #7
        	ldrb     r8,[r0,#10]		@lTemp10
@9, 6, 7, 8		
		smlatb	 r10,r4,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r4,r9
		smlatb	 r11,r5,r8,r11
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
		orr	r3,r3,r10,LSL #16
		str	R3, [R1], #4
		subs	r14,r14,#1		
		add	r0, r0, r2		
		bgt		H01V00_2d_loop	
	
@V
         ldr	r1, [sp]
        ldr	r3, [sp, #4]
        add	r4, sp, #8
@	ldr	r5,[sp,#220]	@HFilter
	ldr	r5,[sp,#224]	@VFilter
	
	mov	r14, #8
	mov	r12, #64	
	ldr	r6, [R5]
	ldr	r7, [R5, #4]
	ldr	r8, [R5, #8]
	ldr	r9, [R5, #12]
	orr	r2,r6,r7,LSL #16
	orr	r5,r8,r9,LSL #16
H00V01_2d_loop:
        ldr	r6,[r4], #16			@lTemp0 a = 0, 1
        ldr	r7,[r4], #16			@lTemp1 b = 0, 1                 
        ldr	r8,[r4], #16			@lTemp2 c = 0, 1  
        ldr	r9,[r4], #16			@lTemp3 d = 0, 1 
@6, 7, 8, 9
		smlatb	 r10,r2,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11
		sub	R10, R10, R11			
		usat	 r0,#8,r10,ASR #7
	
		smlatt	 r10,r2,r7,r12
		smlabt	 r10,r5,r8,r10
		smulbt	 r11,r2,r6
		smlatt	 r11,r5,r9,r11		
		sub	R10, R10, R11			
		usat	 r10,#8,r10,ASR #7
	orr		 r0,r0,r10,LSL #8						
        	ldr	r6,[r4], #16			@lTemp0 a = 0, 1
        	strh	 r0, [r1], r3	        	       	
@7, 8, 9, 6		
		smlatb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7 
		smlatb	 r11,r5,r6,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7

		smlatt	 r10,r2,r8,r12
		smlabt	 r10,r5,r9,r10
		smulbt	 r11,r2,r7
		smlatt	 r11,r5,r6,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7			
	        ldr	r7,[r4], #16			@lTemp1 b = 0, 1                 
	orr		 r0,r0,r10,LSL #8	        
        	strh	 r0, [r1], r3    	      	
@8, 9, 6, 7		
		smlatb	 r10,r2,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7
			
		smlatt	 r10,r2,r9,r12
		smlabt	 r10,r5,r6,r10
		smulbt	 r11,r2,r8
		smlatt	 r11,r5,r7,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7		
        	ldr	r8,[r4], #16			@lTemp2 c = 0, 1
	orr		 r0,r0,r10,LSL #8
        	strh	 r0, [r1], r3	        	        	         	
@9, 6, 7, 8		
		smlatb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7
		
		smlatt	 r10,r2,r6,r12
		smlabt	 r10,r5,r7,r10
		smulbt	 r11,r2,r9
		smlatt	 r11,r5,r8,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7		
	orr		 r0,r0,r10,LSL #8		 
        	ldr	r9,[r4], #16			@lTemp3 d = 0, 1    	
          	strh	 r0, [r1], r3      	
@6, 7, 8, 9		
		smlatb	 r10,r2,r7,r12
		smlabb	 r10,r5,r8,r10
		smulbb	 r11,r2,r6
		smlatb	 r11,r5,r9,r11		
		sub	R10, R10, R11			
		usat	 r0,#8,r10,ASR #7

		smlatt	 r10,r2,r7,r12
		smlabt	 r10,r5,r8,r10
		smulbt	 r11,r2,r6
		smlatt	 r11,r5,r9,r11		
		sub	R10, R10, R11			
		usat	 r10,#8,r10,ASR #7
	        ldr	r6,[r4], #16			@lTemp0 a = 0, 1
	orr		 r0,r0,r10,LSL #8
        	strh	 r0, [r1], r3		              	
@7, 8, 9, 6		
		smlatb	 r10,r2,r8,r12
		smlabb	 r10,r5,r9,r10
		smulbb	 r11,r2,r7
		smlatb	 r11,r5,r6,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7
	
		smlatt	 r10,r2,r8,r12
		smlabt	 r10,r5,r9,r10
		smulbt	 r11,r2,r7
		smlatt	 r11,r5,r6,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
	orr		 r0,r0,r10,LSL #8		
	        ldr	r7,[r4], #16			@lTemp1 b = 0, 1                  
          	strh	 r0, [r1], r3    	
@8, 9, 6, 7		
		smlatb	 r10,r2,r9,r12
		smlabb	 r10,r5,r6,r10
		smulbb	 r11,r2,r8
		smlatb	 r11,r5,r7,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7
		
		smlatt	 r10,r2,r9,r12
		smlabt	 r10,r5,r6,r10
		smulbt	 r11,r2,r8
		smlatt	 r11,r5,r7,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7		
	        ldr	r8,[r4], #16			@lTemp2 c = 0, 1
	orr		 r0,r0,r10,LSL #8
        	strh	 r0, [r1], r3		              	
@9, 6, 7, 8		
		smlatb	 r10,r2,r6,r12
		smlabb	 r10,r5,r7,r10
		smulbb	 r11,r2,r9
		smlatb	 r11,r5,r8,r11		
		sub	R10, R10, R11			
		usat	r0,#8,r10,ASR #7
	
		smlatt	 r10,r2,r6,r12
		smlabt	 r10,r5,r7,r10
		smulbt	 r11,r2,r9
		smlatt	 r11,r5,r8,r11		
		sub	R10, R10, R11			
		usat	r10,#8,r10,ASR #7
	orr		 r0,r0,r10,LSL #8
					
		subs	r14,r14,#2
          	strh	 r0, [r1], r3
 
        sub      r4,r4,#172	@(8+3)*8*2 = 176 - 4
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2
		bgt		H00V01_2d_loop

        add      sp,sp,#184	@(8+3)*8*2 = 176 + 4 + 4	
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
       	@.END