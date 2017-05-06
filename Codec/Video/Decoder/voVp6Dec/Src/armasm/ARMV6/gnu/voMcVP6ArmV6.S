@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
#include "../../../../Inc/voVP6DecID.h"
	.section .text
	.global FilterBlock1dBil_wRecon_Armv6
	.global FilterBlock2dBil_wRecon_Armv6
	.global FilterBlock1d_wRecon_Armv6
	.global FilterBlock2d_wRecon_Armv6			

FilterBlock1dBil_wRecon_Armv6: @PROC
@void FilterBlock1dBil_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	SrcStride,	r3
@	INT32	LineStep,	r4
@	FILTER_DATA  *Filter 	r5
@)
@{
@	UINT32 i, j@
@	// accomodate incrementing SrcPtr++ each time.
@	SrcStride -= 8@
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
@		SrcPtr   += SrcStride@
@		//diffPtr += BLOCK_HEIGHT_WIDTH@
@		dstPtr += LineStep@
@	}
@}
@[sp, #36] = LineStep, [sp, #40] = Filter
	stmdb	sp!, {r4 - r11, lr}
	ldr	r4,[sp,#36]	@LineStep
	ldr	r5,[sp,#40]	@Filter
	cmp	r2, #1
	
	MOV	r14, #8
	MOV	r12, #64		
	LDR	r2, [R5]
	LDR	r5, [R5, #4]	
			
	bne	FilterBlock1dBil_wRecon_Armv6_V
						
H01V00_1dBil_loop:
@first 0~3		
        LDRB     r6,[r0,#0]		@lTemp0
        LDRB     r7,[r0,#1]		@lTemp1   
        LDRB     r8,[r0,#2]		@lTemp2            
        LDRB     r9,[r0,#3]		@lTemp3 

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r11,r2,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7

        	LDRB     r6,[r0,#4]		@lTemp4					
	ORR		 r10,r10,r11,LSL #8
				
		SMLABB	 r7,r2,r8,r12
		SMLABB	 r11,r2,r9,r12
		
		SMLABB	 r7,r5,r9,r7
		SMLABB	 r11,r5,r6,r11
					
		USAT	 r7,#8,r7,ASR #7						
		USAT	 r11,#8,r11,ASR #7
	ORR		 r10,r10,r7,LSL #16
	ORR		 r10,r10,r11,LSL #24
@second 0~3		
        LDRB     r7,[r0,#5]		@lTemp0
        LDRB     r8,[r0,#6]		@lTemp1   
        LDRB     r9,[r0,#7]		@lTemp2
		STR      r10,[r1]        

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r11,r2,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7

        	LDRB     r6,[r0,#8]		@lTemp4					
	ORR		 r10,r10,r11,LSL #8
				
		SMLABB	 r7,r2,r8,r12
		SMLABB	 r11,r2,r9,r12
		
		SMLABB	 r7,r5,r9,r7
		SMLABB	 r11,r5,r6,r11
					
		USAT	 r7,#8,r7,ASR #7						
		USAT	 r11,#8,r11,ASR #7
	ORR		 r10,r10,r7,LSL #16
	ORR		 r10,r10,r11,LSL #24							

		SUBS	r14,r14,#1
		STR      r10,[r1, #4]			
		add	r0, r0, r3
		add	r1, r1, r4		
		BGT		H01V00_1dBil_loop	
	
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1dBil_wRecon_Armv6_V:
					
H00V01_1dBil_loop:
@first 0~3		
        LDRB     r6,[r0], r3		@lTemp0
        LDRB     r7,[r0], r3		@lTemp1   
        LDRB     r8,[r0], r3		@lTemp2            
        LDRB     r9,[r0], r3		@lTemp3 

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r11,r2,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7

        	LDRB     r6,[r0], r3		@lTemp4
        	STRB	 r10, [r1], r4
        	STRB	 r11, [r1], r4
				
		SMLABB	 r10,r2,r8,r12
		SMLABB	 r11,r2,r9,r12
		
		SMLABB	 r10,r5,r9,r10
		SMLABB	 r11,r5,r6,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7
        	STRB	 r10, [r1], r4
        	STRB	 r11, [r1], r4
@second 0~3		
        LDRB     r7,[r0], r3		@lTemp0
        LDRB     r8,[r0], r3		@lTemp1   
        LDRB     r9,[r0], r3		@lTemp2

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r11,r2,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7

        	LDRB     r6,[r0]		@lTemp4					
        	STRB	 r10, [r1], r4
        	STRB	 r11, [r1], r4
				
		SMLABB	 r10,r2,r8,r12
		SMLABB	 r11,r2,r9,r12
		
		SMLABB	 r10,r5,r9,r10
		SMLABB	 r11,r5,r6,r11
					
		USAT	 r10,#8,r10,ASR #7						
		USAT	 r11,#8,r11,ASR #7
        	STRB	 r10, [r1], r4
        	STRB	 r11, [r1], r4							

		SUB	r0, r0, r3, lsl #3			
		SUB	r1, r1, r4, lsl #3
		SUBS	r14,r14,#1
		ADD	r0, r0, #1
		ADD	r1, r1, #1
		BGT		H00V01_1dBil_loop
		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP     
	
	
FilterBlock2dBil_wRecon_Armv6: @PROC
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
        SUB      sp,sp,#176	@(8+3)*8*2 = 176

        str	r1, [sp]
        add	r1, sp, #8        

	ldr	r5,[sp,#212]	@HFilter
@	ldr	r5,[sp,#216]	@VFilter
@H
	MOV	r14, #9
	MOV	r12, #64		
	LDR	r4, [R5]
	LDR	r5, [R5, #4]
						
H01V00_2dBil_loop:
@first 0~3		
        LDRB     r6,[r0,#0]		@lTemp0
        LDRB     r7,[r0,#1]		@lTemp1   
        LDRB     r8,[r0,#2]		@lTemp2            
        LDRB     r9,[r0,#3]		@lTemp3 

		SMLABB	 r10,r4,r6,r12
		SMLABB	 r11,r4,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		MOV	 r10,r10,ASR #7						
		MOV	 r11,r11,ASR #7

        	LDRB     r6,[r0,#4]		@lTemp4					
	ORR		 r10,r10,r11,LSL #16			
		SMLABB	 r7,r4,r8,r12
		SMLABB	 r11,r4,r9,r12
		STR	R10, [r1], #4		
		
		SMLABB	 r7,r5,r9,r7
		SMLABB	 r11,r5,r6,r11
					
		MOV	 r7, r7,ASR #7						
		MOV	 r11,r11,ASR #7
@second 0~3		

        LDRB     r8,[r0,#6]		@lTemp1 
        LDRB     r9,[r0,#7]		@lTemp2          
	ORR		 r10,r7,r11,LSL #16
        LDRB     r7,[r0,#5]		@lTemp0	        

		STR	R10, [r1], #4        

		SMLABB	 r10,r4,r6,r12
		SMLABB	 r11,r4,r7,r12
		
		SMLABB	 r10,r5,r7,r10
		SMLABB	 r11,r5,r8,r11
					
		MOV	 r10,r10,ASR #7						
		MOV	 r11,r11,ASR #7

        	LDRB     r6,[r0,#8]		@lTemp4					
	ORR		 r10,r10,r11,LSL #16
				
		SMLABB	 r7,r4,r8,r12
		SMLABB	 r11,r4,r9,r12
		STR	R10, [r1], #4
				
		SMLABB	 r7,r5,r9,r7
		SMLABB	 r11,r5,r6,r11
					
		MOV	 r7,r7,ASR #7						
		MOV	 r11,r11,ASR #7							
		SUBS	r14,r14,#1
	ORR		 r10,r7,r11,LSL #16					
		add	r0, r0, r2		
		STR	R10, [r1], #4		
		BGT		H01V00_2dBil_loop	
	
@V
        ldr	r1, [sp]
        add	r4, sp, #8
        
	ldr	r5,[sp,#216]	@VFilter	

	MOV	r14, #8
	MOV	r12, #64		
	LDR	r2, [R5]
	LDR	r5, [R5, #4]
H00V01_2dBil_loop:
@first 0~3		
        LDR	r6,[r4], #16			@lTemp0 a = 0, 1
        LDR	r7,[r4], #16			@lTemp1 b = 0, 1                 
        LDR	r8,[r4], #16			@lTemp2 c = 0, 1  
        LDR	r9,[r4], #16			@lTemp3 d = 0, 1

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		USAT	 r0,#8,r10,ASR #7
		SMLABT	 r10,r2,r6,r12		
		SMLABT	 r10,r5,r7,r10
		USAT	 r10,#8,r10,ASR #7
	ORR		 r10,r0,r10,LSL #8
        						
		SMLABB	 r11,r2,r7,r12		
		SMLABB	 r11,r5,r8,r11				
		USAT	 r0,#8,r11,ASR #7					
		SMLABT	 r11,r2,r7,r12		
		SMLABT	 r11,r5,r8,r11				
		USAT	 r11,#8,r11,ASR #7
	ORR		 r11,r0,r11,LSL #8
        	STRH	 r10, [r1], r3						
        	STRH	 r11, [r1], r3
        	
        	LDR	r6,[r4], #16			@lTemp0 a = 0, 1
				
		SMLABB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		USAT	 r0,#8,r10,ASR #7		
		SMLABT	 r10,r2,r8,r12
		SMLABT	 r10,r5,r9,r10
		USAT	 r10,#8,r10,ASR #7
	ORR		 r10,r0,r10,LSL #8		
        	        											
		SMLABB	 r11,r2,r9,r12
		SMLABB	 r11,r5,r6,r11				
		USAT	 r0,#8,r11,ASR #7
		SMLABT	 r11,r2,r9,r12
		SMLABT	 r11,r5,r6,r11				
		USAT	 r11,#8,r11,ASR #7
	ORR		 r11,r0,r11,LSL #8
        	STRH	 r10, [r1], r3						
        	STRH	 r11, [r1], r3
@second 0~3		
        LDR	r7,[r4], #16			@lTemp1 b = 0, 1                 
        LDR	r8,[r4], #16			@lTemp2 c = 0, 1  
        LDR	r9,[r4], #16			@lTemp3 d = 0, 1

		SMLABB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		USAT	 r0,#8,r10,ASR #7		
		SMLABT	 r10,r2,r6,r12
		SMLABT	 r10,r5,r7,r10
		USAT	 r10,#8,r10,ASR #7
	ORR		 r10,r0,r10,LSL #8
        												
		SMLABB	 r11,r2,r7,r12
		SMLABB	 r11,r5,r8,r11				
		USAT	 r0,#8,r11,ASR #7
		SMLABT	 r11,r2,r7,r12
		SMLABT	 r11,r5,r8,r11			
		USAT	 r11,#8,r11,ASR #7
	ORR		 r11,r0,r11,LSL #8
        	STRH	 r10, [r1], r3						
        	STRH	 r11, [r1], r3
        	
        	LDR	r6,[r4], #16			@lTemp0 a = 0, 1
				
		SMLABB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		USAT	 r0,#8,r10,ASR #7
		SMLABT	 r10,r2,r8,r12
		SMLABT	 r10,r5,r9,r10
		USAT	 r10,#8,r10,ASR #7
	ORR		 r10,r0,r10,LSL #8
		SMLABB	 r11,r2,r9,r12				
											
		SMLABB	 r11,r5,r6,r11
		USAT	 r0,#8,r11,ASR #7
		SMLABT	 r11,r2,r9,r12
		SMLABT	 r11,r5,r6,r11				
		USAT	 r11,#8,r11,ASR #7
	ORR		 r11,r0,r11,LSL #8					
        	STRH	 r10, [r1], r3
        	STRH	 r11, [r1], r3					

        SUB      r4,r4,#140	@144 - 4			
		SUB	r1, r1, r3, lsl #3
		SUBS	r14,r14,#2
		ADD	r1, r1, #2
		BGT		H00V01_2dBil_loop
	
       ADD      sp,sp,#176	@(8+3)*8*2 = 176		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	
	
FilterBlock1d_wRecon_Armv6: @PROC
@void FilterBlock1d_wRecon_C
@(
@	UINT8  *SrcPtr, 	r0
@	UINT8  *dstPtr,		r1
@	INT32	PixelStep,	r2
@	INT32	SrcStride,	r3
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
	MOV	r14, #8		
	LDR	r6, [R5]
	LDR	r7, [R5, #4]
	LDR	r8, [R5, #8]
	LDR	r9, [R5, #12]
	ORR	r2,r6,r7,LSL #16
	ORR	r5,r8,r9,LSL #16				
	bne	FilterBlock1d_wRecon_Armv6_V
	sub	r0, r0, #1										
H01V00_1d_loop:		
        LDRB     r6,[r0,#0]		@lTemp0
        LDRB     r7,[r0,#1]		@lTemp1   
        LDRB     r8,[r0,#2]		@lTemp2            
        LDRB     r9,[r0,#3]		@lTemp3       
@6, 7, 8, 9
		SMULTB	 r10,r2,r7
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	 r12,#8,r10,ASR #7						

        	LDRB     r6,[r0,#4]		@lTemp4
@7, 8, 9, 6		
		SMULTB	 r10,r2,r8
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7
		SMLATB	 r11,r5,r6,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #8
		
        	LDRB     r7,[r0,#5]		@lTemp5
@8, 9, 6, 7		
		SMULTB	 r10,r2,r9
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #16
		
        	LDRB     r8,[r0,#6]		@lTemp6
@9, 6, 7, 8		
		SMULTB	 r10,r2,r6
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #24
		
        	LDRB     r9,[r0,#7]		@lTemp7
		STR	R12, [r1]        	
@6, 7, 8, 9		
		SMULTB	 r10,r2,r7
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	 r12,#8,r10,ASR #7							
	
        	LDRB     r6,[r0,#8]		@lTemp8
@7, 8, 9, 6		
		SMULTB	 r10,r2,r8
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7
		SMLATB	 r11,r5,r6,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #8
		
        	LDRB     r7,[r0,#9]		@lTemp9
@8, 9, 6, 7		
		SMULTB	 r10,r2,r9
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #16
		
        	LDRB     r8,[r0,#10]		@lTemp10
@9, 6, 7, 8		
		SMULTB	 r10,r2,r6
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11
		ADD	R10, R10, #64
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r12,r12,r10,LSL #24						

		SUBS	r14,r14,#1	
		STR	R12, [r1, #4]		
		add	r0, r0, r3
		add	r1, r1, r4		
		BGT		H01V00_1d_loop	
	
	ldmia	sp!, {r4 - r11, pc} 
							
FilterBlock1d_wRecon_Armv6_V:
	sub	r0, r0, r3
	mov	r12, #64					
H00V01_1d_loop:
        LDRB     r6,[r0], r3		@lTemp0
        LDRB     r7,[r0], r3		@lTemp1   
        LDRB     r8,[r0], r3		@lTemp2            
        LDRB     r9,[r0], r3		@lTemp3       
@6, 7, 8, 9
		SMLATB	 r10,r2,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11
		SUB	R10, R10, R11			
		USAT	 r10,#8,r10,ASR #7						
        	LDRB     r6,[r0], r3		@lTemp4
        	STRB	 r10, [r1], r4        	
@7, 8, 9, 6		
		SMLATB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7
		SMLATB	 r11,r5,r6,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
        	LDRB     r7,[r0], r3		@lTemp5
        	STRB	 r10, [r1], r4        	
@8, 9, 6, 7		
		SMLATB	 r10,r2,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
        	LDRB     r8,[r0], r3		@lTemp6
        	STRB	 r10, [r1], r4        	
@9, 6, 7, 8		
		SMLATB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
        	LDRB     r9,[r0], r3		@lTemp7     	
       		STRB	 r10, [r1], r4        	
@6, 7, 8, 9		
		SMLATB	 r10,r2,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11
		SUB	R10, R10, R11			
		USAT	 r10,#8,r10,ASR #7
        	LDRB     r6,[r0], r3		@lTemp8
        	STRB	 r10, [r1], r4        	
@7, 8, 9, 6		
		SMLATB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7
		SMLATB	 r11,r5,r6,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
        	LDRB     r7,[r0]		@lTemp9
        	STRB	 r10, [r1], r4        	
@8, 9, 6, 7		
		SMLATB	 r10,r2,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
        	LDRB     r8,[r0, r3]	@lTemp10
        	STRB	 r10, [r1], r4        	
@9, 6, 7, 8		
		SMLATB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		SUBS	r14,r14,#1
        	STRB	 r10, [r1], r4
 
		ADD	R10, R3, R3, LSL #3			
		SUB	r1, r1, r4, lsl #3
		SUB	r0, r0, r10
		ADD	r0, r0, #1
		ADD	r1, r1, #1
		BGT		H00V01_1d_loop
		
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	
FilterBlock2d_wRecon_Armv6: @PROC
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
        SUB      sp,sp,#184	@(8+3)*8*2 = 176 + 4 + 4
        str	r1, [sp]
        str	r3, [sp, #4]
        add	r1, sp, #8        
	ldr	r5,[sp,#220]	@HFilter
@	ldr	r5,[sp,#224]	@VFilter
@H
	sub	r0, r0, r2
	sub	r0, r0, #1
	MOV	r14, #11
	MOV	r12, #64
	LDR	r6, [R5]
	LDR	r7, [R5, #4]
	LDR	r8, [R5, #8]
	LDR	r9, [R5, #12]
	ORR	r4,r6,r7,LSL #16
	ORR	r5,r8,r9,LSL #16
						
H01V00_2d_loop:
        LDRB     r6,[r0,#0]		@lTemp0
        LDRB     r7,[r0,#1]		@lTemp1   
        LDRB     r8,[r0,#2]		@lTemp2            
        LDRB     r9,[r0,#3]		@lTemp3       
@6, 7, 8, 9
		SMLATB	 r10,r4,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r4,r6
		SMLATB	 r11,r5,r9,r11
		SUB	R10, R10, R11			
		USAT	 r3,#8,r10,ASR #7						
        	LDRB     r6,[r0,#4]		@lTemp4
@7, 8, 9, 6		
		SMLATB	 r10,r4,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r4,r7
		SMLATB	 r11,r5,r6,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r3,r3,r10,LSL #16
		STR	R3, [R1], #4		
        	LDRB     r7,[r0,#5]		@lTemp5
@8, 9, 6, 7		
		SMLATB	 r10,r4,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r4,r8
		SMLATB	 r11,r5,r7,r11
		SUB	R10, R10, R11			
		USAT	r3,#8,r10,ASR #7
        	LDRB     r8,[r0,#6]		@lTemp6
@9, 6, 7, 8		
		SMLATB	 r10,r4,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r4,r9
		SMLATB	 r11,r5,r8,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r3,r3,r10,LSL #16
		STR	R3, [R1], #4	
        	LDRB     r9,[r0,#7]		@lTemp7      	
@6, 7, 8, 9		
		SMLATB	 r10,r4,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r4,r6
		SMLATB	 r11,r5,r9,r11
		SUB	R10, R10, R11			
		USAT	 r3,#8,r10,ASR #7
        	LDRB     r6,[r0,#8]		@lTemp8
@7, 8, 9, 6		
		SMLATB	 r10,r4,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r4,r7
		SMLATB	 r11,r5,r6,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r3,r3,r10,LSL #16
		STR	R3, [R1], #4
        	LDRB     r7,[r0,#9]		@lTemp9
@8, 9, 6, 7		
		SMLATB	 r10,r4,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r4,r8
		SMLATB	 r11,r5,r7,r11
		SUB	R10, R10, R11			
		USAT	r3,#8,r10,ASR #7
        	LDRB     r8,[r0,#10]		@lTemp10
@9, 6, 7, 8		
		SMLATB	 r10,r4,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r4,r9
		SMLATB	 r11,r5,r8,r11
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
		ORR	r3,r3,r10,LSL #16
		STR	R3, [R1], #4
		SUBS	r14,r14,#1		
		add	r0, r0, r2		
		BGT		H01V00_2d_loop	
	
@V
         ldr	r1, [sp]
        ldr	r3, [sp, #4]
        add	r4, sp, #8
@	ldr	r5,[sp,#220]	@HFilter
	ldr	r5,[sp,#224]	@VFilter
	
	MOV	r14, #8
	MOV	r12, #64	
	LDR	r6, [R5]
	LDR	r7, [R5, #4]
	LDR	r8, [R5, #8]
	LDR	r9, [R5, #12]
	ORR	r2,r6,r7,LSL #16
	ORR	r5,r8,r9,LSL #16
H00V01_2d_loop:
        LDR	r6,[r4], #16			@lTemp0 a = 0, 1
        LDR	r7,[r4], #16			@lTemp1 b = 0, 1                 
        LDR	r8,[r4], #16			@lTemp2 c = 0, 1  
        LDR	r9,[r4], #16			@lTemp3 d = 0, 1 
@6, 7, 8, 9
		SMLATB	 r10,r2,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11
		SUB	R10, R10, R11			
		USAT	 r0,#8,r10,ASR #7
	
		SMLATT	 r10,r2,r7,r12
		SMLABT	 r10,r5,r8,r10
		SMULBT	 r11,r2,r6
		SMLATT	 r11,r5,r9,r11		
		SUB	R10, R10, R11			
		USAT	 r10,#8,r10,ASR #7
	ORR		 r0,r0,r10,LSL #8						
        	LDR	r6,[r4], #16			@lTemp0 a = 0, 1
        	STRH	 r0, [r1], r3	        	       	
@7, 8, 9, 6		
		SMLATB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7 
		SMLATB	 r11,r5,r6,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7

		SMLATT	 r10,r2,r8,r12
		SMLABT	 r10,r5,r9,r10
		SMULBT	 r11,r2,r7
		SMLATT	 r11,r5,r6,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7			
	        LDR	r7,[r4], #16			@lTemp1 b = 0, 1                 
	ORR		 r0,r0,r10,LSL #8	        
        	STRH	 r0, [r1], r3    	      	
@8, 9, 6, 7		
		SMLATB	 r10,r2,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7
			
		SMLATT	 r10,r2,r9,r12
		SMLABT	 r10,r5,r6,r10
		SMULBT	 r11,r2,r8
		SMLATT	 r11,r5,r7,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7		
        	LDR	r8,[r4], #16			@lTemp2 c = 0, 1
	ORR		 r0,r0,r10,LSL #8
        	STRH	 r0, [r1], r3	        	        	         	
@9, 6, 7, 8		
		SMLATB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7
		
		SMLATT	 r10,r2,r6,r12
		SMLABT	 r10,r5,r7,r10
		SMULBT	 r11,r2,r9
		SMLATT	 r11,r5,r8,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7		
	ORR		 r0,r0,r10,LSL #8		 
        	LDR	r9,[r4], #16			@lTemp3 d = 0, 1    	
          	STRH	 r0, [r1], r3      	
@6, 7, 8, 9		
		SMLATB	 r10,r2,r7,r12
		SMLABB	 r10,r5,r8,r10
		SMULBB	 r11,r2,r6
		SMLATB	 r11,r5,r9,r11		
		SUB	R10, R10, R11			
		USAT	 r0,#8,r10,ASR #7

		SMLATT	 r10,r2,r7,r12
		SMLABT	 r10,r5,r8,r10
		SMULBT	 r11,r2,r6
		SMLATT	 r11,r5,r9,r11		
		SUB	R10, R10, R11			
		USAT	 r10,#8,r10,ASR #7
	        LDR	r6,[r4], #16			@lTemp0 a = 0, 1
	ORR		 r0,r0,r10,LSL #8
        	STRH	 r0, [r1], r3		              	
@7, 8, 9, 6		
		SMLATB	 r10,r2,r8,r12
		SMLABB	 r10,r5,r9,r10
		SMULBB	 r11,r2,r7
		SMLATB	 r11,r5,r6,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7
	
		SMLATT	 r10,r2,r8,r12
		SMLABT	 r10,r5,r9,r10
		SMULBT	 r11,r2,r7
		SMLATT	 r11,r5,r6,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
	ORR		 r0,r0,r10,LSL #8		
	        LDR	r7,[r4], #16			@lTemp1 b = 0, 1                  
          	STRH	 r0, [r1], r3    	
@8, 9, 6, 7		
		SMLATB	 r10,r2,r9,r12
		SMLABB	 r10,r5,r6,r10
		SMULBB	 r11,r2,r8
		SMLATB	 r11,r5,r7,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7
		
		SMLATT	 r10,r2,r9,r12
		SMLABT	 r10,r5,r6,r10
		SMULBT	 r11,r2,r8
		SMLATT	 r11,r5,r7,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7		
	        LDR	r8,[r4], #16			@lTemp2 c = 0, 1
	ORR		 r0,r0,r10,LSL #8
        	STRH	 r0, [r1], r3		              	
@9, 6, 7, 8		
		SMLATB	 r10,r2,r6,r12
		SMLABB	 r10,r5,r7,r10
		SMULBB	 r11,r2,r9
		SMLATB	 r11,r5,r8,r11		
		SUB	R10, R10, R11			
		USAT	r0,#8,r10,ASR #7
	
		SMLATT	 r10,r2,r6,r12
		SMLABT	 r10,r5,r7,r10
		SMULBT	 r11,r2,r9
		SMLATT	 r11,r5,r8,r11		
		SUB	R10, R10, R11			
		USAT	r10,#8,r10,ASR #7
	ORR		 r0,r0,r10,LSL #8
					
		SUBS	r14,r14,#2
          	STRH	 r0, [r1], r3
 
        SUB      r4,r4,#172	@(8+3)*8*2 = 176 - 4
		SUB	r1, r1, r3, lsl #3
		ADD	r1, r1, #2
		BGT		H00V01_2d_loop

        add      sp,sp,#184	@(8+3)*8*2 = 176 + 4 + 4	
	ldmia	sp!, {r4 - r11, pc}
	@ENDP
	.END