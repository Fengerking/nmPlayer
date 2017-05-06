@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@	written by John							 	                                    *
@***********************************************************************/
	@AREA    |.text|, CODE, READONLY
	
@	#include "../../voASMPort.h"
	#include "voRVDecASMID.h"

	.text

@const T_InterpFnxTableNewPtr gARMV6_ARMV6_InterpolateTable[16] =
@{
@	ARMV6_Interpolate_H00V00, ARMV6_Interpolate_H01V00,
@	ARMV6_Interpolate_H02V00, NULL,
@	ARMV6_Interpolate_H00V01, ARMV6_Interpolate_H01V01,
@	ARMV6_Interpolate_H02V01, NULL,
@	ARMV6_Interpolate_H00V02, ARMV6_Interpolate_H01V02,
@	ARMV6_Interpolate_H02V02, NULL
@}@
@
@const T_InterpFnxTableNewPtr gARMV6_ARMV6_InterpolateChromaTable[16] =
@{
@	ARMV6_MCCopyChroma_H00V00, ARMV6_MCCopyChroma_H01V00,
@	ARMV6_MCCopyChroma_H02V00, NULL,
@	ARMV6_MCCopyChroma_H00V01, ARMV6_MCCopyChroma_H01V01,
@	ARMV6_MCCopyChroma_H02V01, NULL,
@	ARMV6_MCCopyChroma_H00V02, ARMV6_MCCopyChroma_H01V02,
@	ARMV6_MCCopyChroma_H02V02, NULL
@}@
@
@const T_InterpFnxTableNewPtr gARMV6_ARMV6_AddInterpolateTable[16] =
@{
@	ARMV6_AddInterpolate_H00V00, ARMV6_AddInterpolate_H01V00,
@	ARMV6_AddInterpolate_H02V00, NULL,
@	ARMV6_AddInterpolate_H00V01, ARMV6_AddInterpolate_H01V01,
@	ARMV6_AddInterpolate_H02V01, NULL,
@	ARMV6_AddInterpolate_H00V02, ARMV6_AddInterpolate_H01V02,
@	ARMV6_AddInterpolate_H02V02, NULL
@}@
@
@const T_InterpFnxTableNewPtr gARMV6_ARMV6_AddInterpolateChromaTable[16] =
@{
@	ARMV6_AddMCCopyChroma_H00V00, ARMV6_AddMCCopyChroma_H01V00,
@	ARMV6_AddMCCopyChroma_H02V00, NULL,
@	ARMV6_AddMCCopyChroma_H00V01, ARMV6_AddMCCopyChroma_H01V01,
@	ARMV6_AddMCCopyChroma_H02V01, NULL,
@	ARMV6_AddMCCopyChroma_H00V02, ARMV6_AddMCCopyChroma_H01V02,
@	ARMV6_AddMCCopyChroma_H02V02, NULL
@}@
	 
@/******************************************************************/
@/* ARMV6_Interpolate_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/**************************************************************** */
@void RV_FASTCALL  ARMV6_Interpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@
@	/* Do not perform a sequence of U32 copies, since this function  */
@	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
@
@	/*MAP -- Loops are modified as decrementing loops for the purpose*/
@	/*of ARM optimization.											 */
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		memcpy(pDst, pSrc, 8)@ /* Flawfinder: ignore */
@
@		pDst += uDstPitch@
@		pSrc += uSrcPitch@
@	}
@}	/* H00V00 */

	.globl ARMV6_Interpolate_H00V00 
ARMV6_Interpolate_H00V00:  @PROC
        stmfd    sp!,{r4-r11,lr}
        	  
		ands	r4,r0,#3
		mov	r14, #8	
		bne		unalign_H00V00		
align_H00V00:
align_H00V00_loop:	
		ldr		r5,[r0,#4]      @pTempSrc[1]
		ldr		r4,[r0], r2      @pTempSrc[0]
		ldr		r7,[r0,#4]      @pTempSrc[1]
		ldr		r6,[r0], r2      @pTempSrc[0]		
		ldr		r9,[r0,#4]      @pTempSrc[1]
		ldr		r8,[r0], r2      @pTempSrc[0]
		ldr		r11,[r0,#4]      @pTempSrc[1]
		ldr		r10,[r0], r2      @pTempSrc[0]				
					

		strd		r4,[r1],r3      @pTempDst[0]
		strd		r6,[r1],r3      @pTempDst[0]
		strd		r8,[r1],r3      @pTempDst[0]
		strd		r10,[r1],r3      @pTempDst[0]		
		
@		subs	r14,r14,#4

		ldr		r5,[r0,#4]      @pTempSrc[1]
		ldr		r4,[r0], r2      @pTempSrc[0]
		ldr		r7,[r0,#4]      @pTempSrc[1]
		ldr		r6,[r0], r2      @pTempSrc[0]	
		ldr		r9,[r0,#4]      @pTempSrc[1]
		ldr		r8,[r0], r2      @pTempSrc[0]
		ldr		r11,[r0,#4]      @pTempSrc[1]
		ldr		r10,[r0], r2      @pTempSrc[0]				
					
		strd		r4,[r1],r3      @pTempDst[0]
		strd		r6,[r1],r3      @pTempDst[0]
		strd		r8,[r1],r3      @pTempDst[0]
		strd		r10,[r1],r3      @pTempDst[0]							
@		bgt		align_H00V00_loop						
		b		end_H00V00
unalign_H00V00:			
		sub		r0,r0,r4
		mov		r4,r4,lsl #3		@i = i<<3@
		rsb		r5,r4,#0x20			@32 - i
unalign_H00V00_loop:
		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r8,[r0,#8]		@pTempSrc[2]
		ldr		r6,[r0], r2		@pTempSrc[0]
		
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r9,[r0,#8]		@pTempSrc[2]
		ldr		r10,[r0], r2		@pTempSrc[0]
								
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5
		mov		r7,r7,LSR r4
		mov		r11,r11,LSR r4
		orr		r7,r7,r8,lsl r5 
		orr		r11,r11,r9,lsl r5 
		
		subs	r14,r14,#2
		strd		r6,[r1],r3
		strd		r10,[r1],r3					
		bgt		unalign_H00V00_loop
end_H00V00:				
	
        ldmfd    sp!,{r4-r11,pc}
	@ENDP


@/*******************************************************************/
@/* ARMV6_Interpolate_H01V00 
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	U32 lTemp@
@	I32 dstRow, dstCol@        
@	I32 lTemp0, lTemp1@
@	I32 lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. Pixel values pSrc[x] are taken into*/
@	/*temporary variables lTempX, so that number of loads can be minimized. */
@	/*Decrementing loops are used for the purpose of optimization			*/
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-1]@ 
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[1]@
@			lTemp3 = pSrc[2]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4@
@			lTemp  = ClampVal(lTemp0)@
@
@			lTemp0 = pSrc[3]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp1)) << 8@
@
@			lTemp1 = pSrc[4]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp2)) << 16@
@
@			lTemp2 = pSrc[5]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp3)) << 24@
@
@			*((PU32)pDst)++ = lTemp@
@			pSrc += 4@
@		}
@		pDst += (uDstPitch - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@}
	.globl ARMV6_Interpolate_H01V00 
ARMV6_Interpolate_H01V00:  @PROC
        stmfd    sp!,{r4-r11,lr}
        	  	
		mov	r14, #8
		mov	r12, #6	
H01V00_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r9, r9, #8		@8 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
    ldrb     r4,[r0,#4]		@lTemp5
    ldrb     r5,[r0,#5]		@lTemp6	
	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r8,r7, lsl #1	@2b+c	
   	rsb		 r6, r6, #8		@8 - (a+d)
	add		 r11,r7,r5		@a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
	usat	 r11,#8,r11,asr #4
			
@five six
	add		 r9,r8,r6		@a+d   
	add		 r8,r5,r4, lsl #1	@2b+c	
	orr	r10,r10,r11,lsl #24	
   	rsb		 r9, r9, #8		@8 - (a+d)
        str      r10,[r1]   	
	add		 r10,r4,r7		@a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r7,r6, lsl #1	@2b+c	
   	rsb		 r5, r5, #8		@8 - (a+d)
	add		 r11,r6,r4		@a+d   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,lsl #16	
	usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
	orr	r10,r10,r11,lsl #24		
		add	r0, r0, r2	
        str      r10,[r1,#4]			
		add	r1, r1, r3					
		bgt		H01V00_loop				
	
        ldmfd    sp!,{r4-r11,pc}
	@ENDP                            
                                            
@/******************************************************************/
@/* ARMV6_Interpolate_H02V00                     
@/*	2/3 pel horizontal displacement     
@/*	0 vertical displacement             
@/*	Use horizontal filter (-1,6,12,-1)  
@/******************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{                                           
@	U32 lTemp@                          
@	I32 dstRow, dstCol@                 
@	I32 lTemp0, lTemp1@                 
@	I32 lTemp2, lTemp3@                 
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-1]@ 
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[1]@
@			lTemp3 = pSrc[2]@
@
@			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4@
@			lTemp  = ClampVal(lTemp0)@
@
@			lTemp0 = pSrc[3]@
@			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp1)) << 8@
@
@			lTemp1 = pSrc[4]@
@			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp2)) << 16@
@
@			lTemp2 = pSrc[5]@
@			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp3)) << 24@
@
@			*((PU32)pDst)++ = lTemp@
@			pSrc += 4@		
@		}
@		pDst += (uDstPitch - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@}

	.globl ARMV6_Interpolate_H02V00 
ARMV6_Interpolate_H02V00:  @PROC
	stmfd    sp!,{r4-r11,lr}
		
		mov	r14, #8
		mov	r12, #6	
H02V00_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r9, r9, #8		@8 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r7,r8, lsl #1	@2b+c	
   	rsb		 r6, r6, #8		@8 - (a+d)
	add		 r11,r7,r5		@a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
	usat	 r11,#8,r11,asr #4
			
@five six
	add		 r9,r8,r6		@a+d   
	add		 r8,r4,r5, lsl #1	@2b+c	
	orr	r10,r10,r11,lsl #24	
   	rsb		 r9, r9, #8		@8 - (a+d)
        str      r10,[r1]   	
	add		 r10,r4,r7		@a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r6,r7, lsl #1	@2b+c	
   	rsb		 r5, r5, #8		@8 - (a+d)
	add		 r11,r6,r4		@a+d   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,lsl #16	
	usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
	orr	r10,r10,r11,lsl #24		
		add	r0, r0, r2	
        str      r10,[r1,#4]			
		add	r1, r1, r3					
		bgt		H02V00_loop
								              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP  
@/*******************************************************************************/
@/* ARMV6_Interpolate_H00V01 
@/*	0 horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (-1,12,6,-1) 
@/******************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  ARMV6_Interpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. While doing vertical interploation  */
@	/*we @PROCess along columns instead of rows so that loads can be minimised*/
@	/*Decrementing loops are used for the purpose of optimization			 */
@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -= 4)
@		{
@			lTemp0 = pSrc[-(I32)(uSrcPitch)]@
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[uSrcPitch]@
@			lTemp3 = pSrc[uSrcPitch<<1]@
@
@			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = pSrc[3*uSrcPitch]@
@			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = pSrc[uSrcPitch << 2]@
@			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = pSrc[5*uSrcPitch]@
@			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4@
@			*pDst = ClampVal(lTemp3)@
@			pDst += uDstPitch@
@
@			pSrc += (uSrcPitch << 2)@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		pSrc -= ((uSrcPitch * 8) - 1)@
@	}
@}
	.globl ARMV6_Interpolate_H00V01 
ARMV6_Interpolate_H00V01:  @PROC
		stmfd    sp!,{r4-r11,lr}
		mov      r14, #8						
		sub		 sp,sp,#8
		ands	 r4,r0,#3		     	  	
		str	     r3,[sp,#0]	
		orr	     r12, r14, r14, lsl #16		
		bne		 unalign_H00V01		
align_H00V01:
align_H00V01_loop:
		@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r7,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]	@lTemp3 d = 0, 1, 2, 3
        ldr	r6,[r0],#4			@lTemp1 b = 0, 1, 2, 3         
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		    @0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    @0, 2	2*b
		mov         r10, r10, lsl #1    @0, 2	2*b
		@lsl		r11, r11, #1	    @1, 3	2*b
		mov         r11, r11, lsl #1    @1, 3	2*b			
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		    r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		    r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		    @0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		    @1, 3	6*(2*b + c)
		mov		    r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		    r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		    @0, 2	8 - (a+d)
		usub16		r3, r12, r3		    @1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		    r10, #0x08000
		mov		    r10, r10, asr #4
		orrne		r10, r10, #0x0000f000
		andeq		r10, r10, #0xffff0fff
		tst		    r11, #0x08000					
		mov		    r11, r11, asr #4				
		orrne		r11, r11, #0x0000f000					
		andeq		r11, r11, #0xffff0fff			
		usat16	    r10,#8,r10
		usat16	    r11,#8,r11	
		orr	        r10,r10,r11,lsl #8
		
		str	r10, [r1] 

        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r7,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]	@lTemp3 d = 0, 1, 2, 3
        ldr	r6,[r0],#-4			@lTemp1 b = 0, 1, 2, 3 
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		    r10, r10, lsl #1	    	@0, 2	2*b
		mov		    r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		    r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		    r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		    r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		    r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		    r10, #0x08000
		mov		    r10, r10, asr #4
		orrne		r10, r10, #0x0000f000
		andeq		r10, r10, #0xffff0fff
		tst		    r11, #0x08000					
		mov		    r11, r11, asr #4					
		orrne		r11, r11, #0x0000f000				
		andeq		r11, r11, #0xffff0fff			
		usat16	    r10,#8,r10
		usat16	    r11,#8,r11	
		orr 	    r10,r10,r11,lsl #8

		str	        r10, [r1, #4] 

		ldr	        r3,[sp,#0]					
		subs	    r14,r14,#1
		add	        r0, r0, r2
		add	        r1, r1, r3		
		bgt		align_H00V01_loop						
		b		end_H00V01
unalign_H00V01:			
		sub		r0,r0,r4
		sub		r0, r0, r2
		mov		r4,r4,lsl #3		@i = i<<3@
unalign_H00V01_loop:
		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000			
		@asr		r10, r10, #4
		@asr		r11, r11, #4
		
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1] 

		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff	
		tst		r11, #0x08000			
		mov		r11, r11, asr #4			
		orrne	r11, r11, #0x0000f000				
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		bgt		unalign_H00V01_loop
end_H00V01:				                                   
	
		add		 sp,sp,#8								              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP  


@/****************************************************************************/
@/* ARMV6_Interpolate_H01V01 
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,12,6,-1) 
@/***************************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. First do horizantal interpolation   */
@	/*followed by vertical interpolation. Decrementing loops are used for the*/
@	/*purpose of ARM optimization											 */
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8@
@			*pDst = ClampVal(lTemp3)@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@
@}
@huwei 20090819 stack_bug
	.globl ARMV6_Interpolate_H01V01 
ARMV6_Interpolate_H01V01:  @PROC
	   stmfd    sp!,{r4-r11,lr}
       @sub      sp,sp,#176	@(8+3)*8*2 = 176
       sub       sp,sp,#180	@(8+3)*8*2 + 4 = 180
       str       r3,[sp,#0]
       add       r3,sp,#4	
@Horizontal			
		mov	r14, #11
		sub	r0, r0, r2
		mov	r12, #6	
H01V01_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r9, r9, #0		@0 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #0		@0 - (a+d)		
@	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
@	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d
	pkhbt	r10, r9, r10, lsl #16	
	add		r9,r8,r7, lsl #1	@2b+c	
   	rsb		 r6, r6, #0		@8 - (a+d)
	add		 r11,r7,r5		@a+d 
        str      r10,[r3], #4 	  		   
	smlabb	 r10, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #0		@8 - (a+d)   		
@	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
@	orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
@	usat	 r11,#8,r11,asr #4
			
@five six
	add		 r9,r8,r6		@a+d   
	add		 r8,r5,r4, lsl #1	@2b+c
	pkhbt	r10, r10, r11, lsl #16			
   	rsb		 r9, r9, #0		@8 - (a+d)
        str      r10,[r3], #4  	
	add		 r10,r4,r7		@a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r10, r10, #0		@8 - (a+d)		
@	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
@	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d
	pkhbt	r10, r9, r10, lsl #16			 	
	add		r9,r7,r6, lsl #1	@2b+c	
   	rsb		 r5, r5, #0		@8 - (a+d)
        str      r10,[r3], #4    	
	add		 r11,r6,r4		@a+d   		   
	smlabb	 r10, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	@2b+c	
   	rsb		 r11, r11, #0		@8 - (a+d)	
@	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
@	orr	r10,r10,r5,lsl #16	
@	usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
	pkhbt	r10, r10, r11, lsl #16		
		add	r0, r0, r2
@		add	sp, sp, #16			
        str      r10,[r3], #4			
@		add	r1, r1, r3					
		bgt		H01V01_loop				
@Vertical
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        ldr       r3,[sp,#0]
        add       r4,sp,#4
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		orr	r12, r12, r12, lsl #16	

H01V01_Vloop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r4], #16			@lTemp0 a = 0, 1
        ldr	r6,[r4], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r4], #16			@lTemp2 c = 0, 1  
        ldr	r8,[r4], #16			@lTemp3 d = 0, 1       
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl  #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r8,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@5
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r4], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3										

        @sub      sp,sp,#172	@(8+3)*8*2 = 176 - 4
         sub      r4,r4,#172	@(8+3)*8*2 = 176 - 4
        				
		subs	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		bgt		H01V01_Vloop						
	
        @add      sp,sp,#160	@(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180
        ldmfd    sp!,{r4-r11,pc}                     
	@ENDP  
	            

@/************************************************************************/
@/* ARMV6_Interpolate_H02V01 
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1)
@/*	Use vertical filter (-1,12,6,-1) 
@/************************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8@
@			*pDst = ClampVal(lTemp3)@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}	
@}
@huwei 20090819 stack_bug
	.globl ARMV6_Interpolate_H02V01 
ARMV6_Interpolate_H02V01:  @PROC
     stmfd    sp!,{r4-r11,lr}
     @sub      sp,sp,#176	@(8+3)*8*2 = 176
     sub      sp,sp,#180	@(8+3)*8*2 + 4 = 180
     str      r3,[sp,#0]
     add      r3,sp,#4	
@Horizontal			
     mov	r14, #11
     sub	r0, r0, r2
     mov	r12, #6	
H02V01_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
     ldrb     r4,[r0,#-1]		@lTemp0
     ldrb     r5,[r0,#0]		@lTemp1   
     ldrb     r6,[r0,#1]		@lTemp2            
     ldrb     r7,[r0,#2]		@lTemp3    
     ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r9, r9, #0		@0 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	@2b+c	
   	rsb		 r10, r10, #0		@0 - (a+d)		
@	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
    ldrb     r4,[r0,#4]		@lTemp5
    ldrb     r5,[r0,#5]		@lTemp6	
@	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d
	pkhbt	r10, r9, r10, lsl #16	
	add		r9,r7,r8, lsl #1	@2b+c	
   	rsb		 r6, r6, #0		@8 - (a+d)
	add		 r11,r7,r5		@a+d 
    str      r10,[r3], #4 	  		   
	smlabb	 r10, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	@2b+c	
   	rsb		 r11, r11, #0		@8 - (a+d)   		
@	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
@	orr	r10,r10,r6,lsl #16	
    ldrb     r6,[r0,#6]		@lTemp5
    ldrb     r7,[r0,#7]		@lTemp6		
@	usat	 r11,#8,r11,asr #4
			
@five six
	add		 r9,r8,r6		@a+d   
	add		 r8,r4,r5, lsl #1	@2b+c
	pkhbt	r10, r10, r11, lsl #16			
   	rsb		 r9, r9, #0		@8 - (a+d)
    str      r10,[r3], #4  	
	add		 r10,r4,r7		@a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #0		@8 - (a+d)		
@	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
    ldrb     r8,[r0,#8]		@lTemp7
    ldrb     r4,[r0,#9]		@lTemp8	
@	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d
	pkhbt	r10, r9, r10, lsl #16			 	
	add		r9,r6,r7, lsl #1	@2b+c	
   	rsb		 r5, r5, #0		@8 - (a+d)
    str      r10,[r3], #4    	
	add		 r11,r6,r4		@a+d   		   
	smlabb	 r10, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #0		@8 - (a+d)	
@	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
@	orr	r10,r10,r5,lsl #16	
@	usat	 r11,#8,r11,asr #4				
	subs	 r14,r14,#1
	pkhbt	 r10, r10, r11, lsl #16		
	add	     r0, r0, r2			
    str      r10,[r3], #4			
@	add	r1, r1, r3					
	bgt		H02V01_loop				
@Vertical
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp, #4
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		orr	r12, r12, r12, lsl #16	

H02V01_Vloop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r11], #16			@lTemp0 a = 0, 1
        ldr	r6,[r11], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r11], #16			@lTemp2 c = 0, 1  
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1       
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@5
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3										

        @sub      sp,sp,#172	@(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	@(8+3)*8*2 = 176 - 4
        				
		subs	r14,r14,#2
		sub	    r1, r1, r3, lsl #3
		add	    r1, r1, #2		
		bgt		H02V01_Vloop						
	
        @add      sp,sp,#160	@(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 

@/******************************************************************************/
@/* ARMV6_Interpolate_H00V02 
@/*	0 horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (-1,6,12,-1) 
@/*****************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  ARMV6_Interpolate_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@
@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-(I32)(uSrcPitch)]@
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[uSrcPitch]@
@			lTemp3 = pSrc[uSrcPitch<<1]@
@
@			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = pSrc[3*uSrcPitch]@
@			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = pSrc[4*uSrcPitch]@
@			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = pSrc[5*uSrcPitch]@
@			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4@
@			*pDst = ClampVal(lTemp3)@
@			pDst += uDstPitch@
@
@			pSrc += (uSrcPitch << 2)@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		pSrc -= ((uSrcPitch * 8) - 1)@
@	}
@}
	.globl ARMV6_Interpolate_H00V02 
ARMV6_Interpolate_H00V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #8						
		sub		 sp,sp,#8
		ands	r4,r0,#3		     	  	
		str	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		bne		unalign_H00V02		
align_H00V02:
align_H00V02_loop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r6,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r7,[r0],#4			@lTemp1 b = 0, 1, 2, 3         
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b					
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1] 

        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r6,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r7,[r0],#-4			@lTemp1 b = 0, 1, 2, 3 
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b	
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b			
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff	
		tst		r11, #0x08000					
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000				
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8

		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		bgt		align_H00V02_loop						
		b		end_H00V02
unalign_H00V02:			
		sub		r0,r0,r4
		sub		r0, r0, r2
		mov		r4,r4,lsl #3		@i = i<<3@
unalign_H00V02_loop:
		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b	
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b			
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)		
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1] 

		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		bgt		unalign_H00V02_loop
end_H00V02:				                                   
	
		add		 sp,sp,#8							              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP  

@/*******************************************************************/
@/* ARMV6_Interpolate_H01V02 
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,6,12,-1) 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8@
@			*pDst = ClampVal(lTemp3)@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@}
	.globl ARMV6_Interpolate_H01V02 
ARMV6_Interpolate_H01V02:  @PROC
    stmfd  sp!,{r4-r11,lr}
    sub    sp,sp,#0x4c0
	mov		 r4,sp
	sub		 r0,r0,r2         @p = pSrc - (I32)(uSrcPitch)@
	sub		 r2,r2,#8         @uSrcPitch - 8
	mov    r6,#8			
	rsb		 r12,r6,#16       @r12 = 16 - 8
	mov		 r12,r12,lsl #2   @r12 = 8 * 4
	mov		 lr,#6            @lr = 6
	add    r5,r6,#3         @r5 = 8 + 3
    @b     Ln2180
Ln2020:
    mov    r6,#8            @r6 = 8
    @b     Ln2160
Ln2028:
    ldrb     r9,[r0,#1]		@lTemp2
    ldrb     r8,[r0,#0]		@lTemp1        
    ldrb     r10,[r0,#2]	@lTemp3
    ldrb     r7,[r0,#-1]	@lTemp0
		
	add		 r11,r9,r8,lsl #1
	smulbb r11,lr,r11
	add		 r7,r7,r10
	sub		 r7,r11,r7
	str		 r7,[r4],#4

	ldrb	 r7,[r0,#3]		
	add		 r11,r10,r9,lsl #1
	smulbb r11,lr,r11
	add		 r8,r8,r7
	sub		 r8,r11,r8
	str		 r8,[r4],#4

	ldrb	 r8,[r0,#4]				
	add		 r11,r7,r10,lsl #1
	smulbb r11,lr,r11
	add		 r9,r9,r8
	sub		 r9,r11,r9
	str		 r9,[r4],#4

	ldrb	 r9,[r0,#5]				
	add		 r11,r8,r7,lsl #1
	smulbb r11,lr,r11
	add		 r10,r10,r9
	sub		 r10,r11,r10
	str		 r10,[r4],#4

	subs	 r6,r6,#4  @8 - 4	
	add		 r0,r0,#4  @p + 4             
Ln2160:
    bgt    Ln2028
	add		 r4,r4,r12 @b += (16 - 8)
	add		 r0,r0,r2  @p += (uSrcPitch - 8)		
    subs   r5,r5,#1  @dstRow--
Ln2180:
    bgt      Ln2020

    mov    r2,#8
	mov	   r2,r2,lsl #4 
	sub	   r2,r2,#1  @r2 = (8 << 4) - 1
	mov    r4,r3,lsl #3
	sub    r4,r4,#1  @r4 = (uDstPitch * 8) - 1	
	mov	   r0,sp
	mov	   r12,#0x80 @128
	mov	   r5,#8     @8
    @b     Ln2428
Ln2220:
    mov    r6,#8		 @8
    @b     Ln2408
Ln2228:
    ldr      r9,[r0,#0x80]		@lTemp2
    ldr      r8,[r0,#0x40]		@lTemp1        
    ldr      r10,[r0,#0xc0]		@lTemp3
    ldr      r7,[r0,#0]			  @lTemp0
		
	add		 r11,r8,r9,lsl #1
	smlabb r11,lr,r11,r12
	add		 r7,r7,r10
	sub		 r7,r11,r7
	usat	 r7,#8,r7,asr #8
	
	add		 r11,r9,r10,lsl #1
	strb	 r7,[r1]
	add    r1,r1,r3          @pDst += uDstPitch
	ldr		 r7,[r0,#0x100]
	smlabb r11,lr,r11,r12
	sub		 r8,r11,r8
	sub		 r8,r8,r7
	usat	 r8,#8,r8,asr #8

	add		 r11,r10,r7,lsl #1		
	strb	 r8,[r1]
	add    r1,r1,r3         @pDst += uDstPitch
	ldr		 r8,[r0,#0x140]
	smlabb r11,lr,r11,r12
	sub		 r9,r11,r9
	sub		 r9,r9,r8
	usat	 r9,#8,r9,asr #8

	add		 r11,r7,r8,lsl #1		
	strb	 r9,[r1]
	add    r1,r1,r3         @pDst += uDstPitch
	ldr		 r9,[r0,#0x180]
	smlabb r11,lr,r11,r12
	sub		 r10,r11,r10
	sub		 r10,r10,r9
	usat	 r10,#8,r10,asr #8

	subs	 r6,r6,#4	
	add		 r0,r0,#0x100   @ b += 64
    strb   r10,[r1]
    add    r1,r1,r3       @pDst += uDstPitch
Ln2408:
    bgt    Ln2228
	sub		 r0,r0,r2,lsl #2 @b -= ((8 << 4) - 1)
    sub		 r1,r1,r4        @pDst -= ((uDstPitch*8) - 1)@       
	subs   r5,r5,#1       
Ln2428:
    bgt      Ln2220
    add      sp,sp,#0x4c0
    ldmfd    sp!,{r4-r11,pc}                
	@ENDP  
	

@/****************************************************************/
@/* ARMV6_Interpolate_H02V02 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (6,9,1) 
@/*	Use vertical filter (6,9,1) 
@/****************************************************************/
@void RV_FASTCALL  ARMV6_Interpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*18]@
@	I32 *b@	
@	I32   lTemp0, lTemp1, lTemp2@    
@
@	b = buff@
@	for (dstRow = 8+2@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = pSrc[0]@
@			lTemp1 = pSrc[1]@
@			lTemp2 = pSrc[2]@
@
@			*b = (6*lTemp0 + 9*lTemp1 + lTemp2)@
@			b++@
@
@			lTemp0 = pSrc[3]@
@			*b = (6*lTemp1 + 9*lTemp2 + lTemp0)@
@			b++@
@
@			lTemp1 = pSrc[4]@
@			*b = (6*lTemp2 + 9*lTemp0 + lTemp1)@
@			b++@
@
@			lTemp2 = pSrc[5]@
@			*b = (6*lTemp0 + 9*lTemp1 + lTemp2)@			
@			b++@
@
@			pSrc +=4@			
@		}
@		b += (16 - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@
@			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			lTemp0 = b[48]@
@			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8@
@			*pDst = ClampVal(lTemp1)@
@			pDst += uDstPitch@
@
@			lTemp1 = b[64]@
@			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8@
@			*pDst = ClampVal(lTemp2)@
@			pDst += uDstPitch@
@
@			lTemp2 = b[80]@
@			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8@
@			*pDst = ClampVal(lTemp0)@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@}
LAB_0X00090006: .word 0x00090006

@huwei 20090819 stack_bug
	.globl ARMV6_Interpolate_H02V02 
ARMV6_Interpolate_H02V02:  @PROC
		stmfd    sp!,{r4-r11,lr}
        @sub      sp,sp,#160	@(8+2)*8*2 = 160
        sub      sp,sp,#164	@(8+2)*8*2 + 4 = 164
        str      r3,[sp,#0]
        add      r3,sp,#4	
@Horizontal			
		mov	r14, #10	
H02V02_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		

        ldrb     r5,[r0,#0]		@lTemp0   
        ldrb     r6,[r0,#1]		@lTemp1            
        ldrb     r7,[r0,#2]		@lTemp2    
        ldrb     r8,[r0,#3]		@lTemp3

@input  r4~r8
@one @tow	
		add		 r9,r5,r5,lsl #1
		add		 r10,r6,r6,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r7		
		
		add		 r11,r6,r6,lsl #1
		add		 r12,r7,r7,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r8
		
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r5,[r0,#4]		@lTemp4
        ldrb     r6,[r0,#5]		@lTemp5			
@three@ four      	
		add		 r9,r7,r7,lsl #1
		add		 r10,r8,r8,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r5		
		
		add		 r11,r8,r8,lsl #1
		add		 r12,r5,r5,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r6
		
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r7,[r0,#6]		@lTemp4
        ldrb     r8,[r0,#7]		@lTemp5			
@five six
		add		 r9,r5,r5,lsl #1
		add		 r10,r6,r6,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r7		
		
		add		 r11,r6,r6,lsl #1
		add		 r12,r7,r7,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r8
		
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r5,[r0,#8]		@lTemp4
        ldrb     r6,[r0,#9]		@lTemp5	
@seven@ eight  
		add		 r9,r7,r7,lsl #1
		add		 r10,r8,r8,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r5		
		
		add		 r11,r8,r8,lsl #1
		add		 r12,r5,r5,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r6
		
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
    				
		subs	r14,r14,#1	
		add	r0, r0, r2					
		bgt		H02V02_loop				
@Vertical
        @sub      sp,sp,#160	@(8+2)*8*2 = 160
        add     r2,sp,#4
        ldr     r3,[sp,#0]
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		ldr	r4, LAB_0X00090006	

H02V02_Vloop:
@r4~r12		@(6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8
        ldr	r5,[r2], #16			@lTemp0 a = 0, 1
        ldr	r6,[r2], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r2], #16			@lTemp2 c = 0, 1        
@one	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@two		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@three		
		pkhbt	r9, r7, r5, lsl #16
		pkhtb	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		@lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@four	
@one2	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@two2		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@three2		
		pkhbt	r9, r7, r5, lsl #16
		pkhtb	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		@lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
		
@one3	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r2], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
@two3		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3		

        @sub      sp,sp,#156	@(8+2)*8*2 = 160 - 4
        sub      r2,r2,#156	@(8+2)*8*2 = 160 - 4
        				
		subs	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		bgt		H02V02_Vloop						
	
        @add      sp,sp,#144	@(8+2)*8*2 = 160 - 4*4
        add      sp,sp,#164
        ldmfd    sp!,{r4-r11,pc}                     
	@ENDP  

@/******************************************************************/
@/* ARMV6_AddInterpolate_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/**************************************************************** */
@void RV_FASTCALL  ARMV6_AddInterpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	U32 a,b,c,d@
@	U32 q,w@
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		a=pSrc[0]|(pSrc[1]<<8)|((pSrc[2]|(pSrc[3]<<8))<<16)@
@		b=pSrc[4]|(pSrc[5]<<8)|((pSrc[6]|(pSrc[7]<<8))<<16)@
@		c=((U32 *)pDst)[0]@
@		d=((U32 *)pDst)[1]@
@		q=(a|c) & 0x01010101@
@		w=(b|d) & 0x01010101@
@		q+=(a>>1) & 0x7F7F7F7F@
@		w+=(b>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@		w+=(d>>1) & 0x7F7F7F7F@
@		((U32 *)pDst)[0]=q@
@		((U32 *)pDst)[1]=w@
@		pDst += uDstPitch@
@		pSrc += uSrcPitch@
@	}
@}

LAB_0X01010101: .word 0x01010101
	.globl ARMV6_AddInterpolate_H00V00 
ARMV6_AddInterpolate_H00V00:  @PROC
        stmfd    sp!,{r4-r11,lr}
        	  
		ands	r4,r0,#3
		ldr	r12, LAB_0X01010101		
		mov	r14, #8			
		bne		unalign_AddH00V00		
align_AddH00V00:
align_AddH00V00_loop:	
		ldr		r5,[r0,#4]      @pTempSrc[1]
		ldr		r4,[r0], r2      @pTempSrc[0]
		ldr		r7,[r0,#4]      @pTempSrc[1]
		ldr		r6,[r0], r2      @pTempSrc[0]		
		ldrd		r8,[r1]      	 @pTempDst[0]
		ldrd		r10,[r1,r3]      @pTempDst[0]
							
		uqadd8  r4, r4, r12
		uqadd8  r5, r5, r12
		uqadd8  r6, r6, r12
		uqadd8  r7, r7, r12
				  
		uhadd8	r4, r4, r8
		uhadd8	r5, r5, r9
		uhadd8	r6, r6, r10
		uhadd8	r7, r7, r11	
						
		subs	r14,r14,#2
		strd		r4,[r1],r3      @pTempDst[0]
		strd		r6,[r1],r3      @pTempDst[0]					
		bgt		align_AddH00V00_loop						
		b		end_AddH00V00
unalign_AddH00V00:			
		sub		r0,r0,r4
		mov		r4,r4,lsl #3		@i = i<<3@
		rsb		r5,r4,#0x20			@32 - i
unalign_AddH00V00_loop:
		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r8,[r0,#8]		@pTempSrc[2]
		ldr		r6,[r0], r2		@pTempSrc[0]
		
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r9,[r0,#8]		@pTempSrc[2]
		ldr		r10,[r0], r2		@pTempSrc[0]
								
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5
		mov		r7,r7,LSR r4
		mov		r11,r11,LSR r4
		orr		r7,r7,r8,lsl r5 
		orr		r11,r11,r9,lsl r5 
		
		
		ldrd		r8,[r1]      	 @pTempDst[0]					
		uqadd8  r6, r6, r12
		uqadd8  r7, r7, r12
		uhadd8	r6, r6, r8
		uhadd8	r7, r7, r9
		strd		r6,[r1],r3
				
		ldrd		r8,[r1]      @pTempDst[0]				
		uqadd8  r10, r10, r12
		uqadd8  r11, r11, r12
		uhadd8	r10, r10, r8
		uhadd8	r11, r11, r9		
		subs	r14,r14,#2

		strd		r10,[r1],r3					
		bgt		unalign_AddH00V00_loop
end_AddH00V00:				
	
        ldmfd    sp!,{r4-r11,pc}
	@ENDP 	
@/*******************************************************************/
@/* ARMV6_AddInterpolate_H01V00 
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	U32 lTemp@
@	U32 c,q@
@	I32 dstRow, dstCol@        
@	I32 lTemp0, lTemp1@
@	I32 lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. Pixel values pSrc[x] are taken into*/
@	/*temporary variables lTempX, so that number of loads can be minimized. */
@	/*Decrementing loops are used for the purpose of optimization			*/
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-1]@ 
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[1]@
@			lTemp3 = pSrc[2]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4@
@			lTemp  = ClampVal(lTemp0)@
@
@			lTemp0 = pSrc[3]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp1)) << 8@
@
@			lTemp1 = pSrc[4]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp2)) << 16@
@
@			lTemp2 = pSrc[5]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp3)) << 24@
@
@			c = ((U32 *)pDst)[0]@
@			q = (lTemp|c) & 0x01010101@
@			q += (lTemp>>1) & 0x7F7F7F7F@
@			q += (c>>1) & 0x7F7F7F7F@
@
@			*((PU32)pDst)++ = q@
@			pSrc += 4@
@		}
@		pDst += (uDstPitch - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@}
	.globl ARMV6_AddInterpolate_H01V00 
ARMV6_AddInterpolate_H01V00:  @PROC
	stmfd    sp!,{r4-r11,lr}	     	  	
		mov	r14, #8
		sub		 sp,sp,#8		
		mov	r12, #6	
		str	     r2,[sp,#0]		
		ldr	r2, LAB_0X01010101		
AddH01V00_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r9, r9, #8		@8 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r8,r7, lsl #1	@2b+c	
   	rsb		 r6, r6, #8		@8 - (a+d)
	add		 r11,r7,r5		@a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
	usat	 r11,#8,r11,asr #4	
@five six
	add		 r9,r8,r6		@a+d  
	orr	r10,r10,r11,lsl #24
	ldr	r11, [r1]	 
	add		 r8,r5,r4, lsl #1	@2b+c	
		uqadd8  r11, r11, r2
   	rsb		 r9, r9, #8		@8 - (a+d)				 
		uhadd8	r10, r11, r10  
	smlabb	 r9, r8, r12,r9		 	
        str      r10,[r1]   	
	add		 r10,r4,r7		@a+d    		   
	add		 r4,r6,r5, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r7,r6, lsl #1	@2b+c	
   	rsb		 r5, r5, #8		@8 - (a+d)
	add		 r11,r6,r4		@a+d
	ldr	r4, [r1,#4]	   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,lsl #16	
	usat	 r11,#8,r11,asr #4
		ldr	     r5,[sp,#0]	
		uqadd8  r4, r4, r2						
	orr	r10,r10,r11,lsl #24		
		subs	r14,r14,#1	
		uhadd8	r10, r4, r10 	
		add	r0, r0, r5 									
        str      r10,[r1,#4]		       	
		add	r1, r1, r3					
		bgt		AddH01V00_loop
				
		add		 sp,sp,#8								              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 
@/******************************************************************/
@/* ARMV6_AddInterpolate_H02V00 
@/*	2/3 pel horizontal displacement
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1) 
@/******************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	U32 lTemp@
@	U32 c,q@
@	I32 dstRow, dstCol@
@	I32 lTemp0, lTemp1@
@	I32 lTemp2, lTemp3@
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-1]@ 
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[1]@
@			lTemp3 = pSrc[2]@
@
@			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4@
@			lTemp  = ClampVal(lTemp0)@
@
@			lTemp0 = pSrc[3]@
@			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp1)) << 8@
@
@			lTemp1 = pSrc[4]@
@			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp2)) << 16@
@
@			lTemp2 = pSrc[5]@
@			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4@
@			lTemp  |= (ClampVal(lTemp3)) << 24@
@
@			c=((U32 *)pDst)[0]@
@			q=(lTemp|c) & 0x01010101@
@			q+=(lTemp>>1) & 0x7F7F7F7F@
@			q+=(c>>1) & 0x7F7F7F7F@
@
@			*((PU32)pDst)++ = q@
@			pSrc += 4@		
@		}
@		pDst += (uDstPitch - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@}

	.globl ARMV6_AddInterpolate_H02V00 
ARMV6_AddInterpolate_H02V00:  @PROC
	stmfd    sp!,{r4-r11,lr}	     	  	
		mov	r14, #8
		sub		 sp,sp,#8		
		mov	r12, #6	
		str	     r2,[sp,#0]		
		ldr	r2, LAB_0X01010101		
AddH02V00_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
	add		 r9,r4,r7		@a+d   
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r9, r9, #8		@8 - (a+d)
	add		 r10,r5,r8		@a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
	usat	 r10,#8,r10,asr #4		
@three@ four      	
	add	r6,r6,r4		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r7,r8, lsl #1	@2b+c	
   	rsb		 r6, r6, #8		@8 - (a+d)
	add		 r11,r7,r5		@a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r6,#8,r6,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
	usat	 r11,#8,r11,asr #4	
@five six
	add		 r9,r8,r6		@a+d  
	orr	r10,r10,r11,lsl #24
	ldr	r11, [r1]	 
	add		 r8,r4,r5, lsl #1	@2b+c	
		uqadd8  r11, r11, r2
   	rsb		 r9, r9, #8		@8 - (a+d)				 
		uhadd8	r10, r11, r10  
	smlabb	 r9, r8, r12,r9		 	
        str      r10,[r1]   	
	add		 r10,r4,r7		@a+d    		   
	add		 r4,r5,r6, lsl #1	@2b+c	
   	rsb		 r10, r10, #8		@8 - (a+d)		
	usat	 r9,#8,r9,asr #4
	smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
	usat	 r10,#8,r10,asr #4
@seven@ eight      	
	add	r5,r5,r8		@a+d		 
	orr	r10,r9,r10,lsl #8 	
	add		r9,r6,r7, lsl #1	@2b+c	
   	rsb		 r5, r5, #8		@8 - (a+d)
	add		 r11,r6,r4		@a+d
	ldr	r4, [r1,#4]	   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	@2b+c	
   	rsb		 r11, r11, #8		@8 - (a+d)	
	usat	 r5,#8,r5,asr #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,lsl #16	
	usat	 r11,#8,r11,asr #4
		ldr	     r5,[sp,#0]	
		uqadd8  r4, r4, r2						
	orr	r10,r10,r11,lsl #24		
		subs	r14,r14,#1	
		uhadd8	r10, r4, r10 	
		add	r0, r0, r5 									
        str      r10,[r1,#4]		       	
		add	r1, r1, r3					
		bgt		AddH02V00_loop
				
		add		 sp,sp,#8								              	 		
						              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP
@/*******************************************************************************/
@/* ARMV6_AddInterpolate_H00V01 
@/*	0 horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (-1,12,6,-1) 
@/******************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  ARMV6_AddInterpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. While doing vertical interploation  */
@	/*we @PROCess along columns instead of rows so that loads can be minimised*/
@	/*Decrementing loops are used for the purpose of optimization			 */
@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -= 4)
@		{
@			lTemp0 = pSrc[-(I32)(uSrcPitch)]@
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[uSrcPitch]@
@			lTemp3 = pSrc[uSrcPitch<<1]@
@
@			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = pSrc[3*uSrcPitch]@
@			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = pSrc[uSrcPitch << 2]@
@			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = pSrc[5*uSrcPitch]@
@			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4@
@			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			pSrc += (uSrcPitch << 2)@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		pSrc -= ((uSrcPitch * 8) - 1)@
@	}
@}

	.globl ARMV6_AddInterpolate_H00V01 
ARMV6_AddInterpolate_H00V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #8						
		sub		 sp,sp,#8
		ands	r4,r0,#3		     	  	
		str	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		bne		unalign_AddH00V01		
align_AddH00V01:
align_AddH00V01_loop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r7,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r6,[r0],#4			@lTemp1 b = 0, 1, 2, 3         
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b					
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11
		ldr	r7, LAB_0X01010101			
		orr	r10,r10,r11,lsl #8
				
		ldr	r8, [r1]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
				
		str	r10, [r1] 

        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r7,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r6,[r0],#-4			@lTemp1 b = 0, 1, 2, 3 
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b	
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b			
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000				
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB_0X01010101		
		orr	r10,r10,r11,lsl #8
		
		ldr	r8, [r1, #4]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
		
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		bgt		align_AddH00V01_loop						
		b		end_AddH00V01
unalign_AddH00V01:			
		sub		r0,r0,r4
		sub		r0, r0, r2
		mov		r4,r4,lsl #3		@i = i<<3@
unalign_AddH00V01_loop:
		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB_0X01010101		
		orr	r10,r10,r11,lsl #8
			
		ldr	r8, [r1]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
				
		str	r10, [r1] 

		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB_0X01010101		
		orr	r10,r10,r11,lsl #8
				
		ldr	r8, [r1, #4]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10		
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		bgt		unalign_AddH00V01_loop
end_AddH00V01:				                                   
	
		add		 sp,sp,#8								              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP  
@/****************************************************************************/
@/* ARMV6_AddInterpolate_H01V01 
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,12,6,-1) 
@/***************************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	/*MAP -- @PROCess 4 pixels at a time. First do horizantal interpolation   */
@	/*followed by vertical interpolation. Decrementing loops are used for the*/
@	/*purpose of ARM optimization											 */
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8@
@			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@
@} 
@huwei 20090819 stack_bug
	.globl ARMV6_AddInterpolate_H01V01 
ARMV6_AddInterpolate_H01V01:  @PROC
		stmfd    sp!,{r4-r11,lr}
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        sub      sp,sp,#180	@(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0]
        add      r3,sp,#4	
@Horizontal			
		mov	r14, #11
		sub	r0, r0, r2
		mov	r12, #6	
AddH01V01_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
		add		 r9,r4,r7		@a+d   
		add		 r4,r6,r5, lsl #1	@2b+c	
		rsb		 r9, r9, #0		@0 - (a+d)
		add		 r10,r5,r8		@a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r7,r6, lsl #1	@2b+c	
		rsb		 r10, r10, #0		@0 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
@		usat	 r10,#8,r10,asr #4		
@three@ four      	
		add	r6,r6,r4		@a+d
		pkhbt	r10, r9, r10, lsl #16	
		add		r9,r8,r7, lsl #1	@2b+c	
   		rsb		 r6, r6, #0		@8 - (a+d)
		add		 r11,r7,r5		@a+d 
        str      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r4,r8, lsl #1	@2b+c	
		rsb		 r11, r11, #0		@8 - (a+d)   		
@		usat	 r6,#8,r6,asr #4   
		smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
@		usat	 r11,#8,r11,asr #4
			
@five six
		add		 r9,r8,r6		@a+d   
		add		 r8,r5,r4, lsl #1	@2b+c
		pkhbt	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		@8 - (a+d)
        str      r10,[r3], #4  	
		add		 r10,r4,r7		@a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r6,r5, lsl #1	@2b+c	
   		rsb		 r10, r10, #0		@8 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
@		usat	 r10,#8,r10,asr #4
@seven@ eight      	
		add	r5,r5,r8		@a+d
		pkhbt	r10, r9, r10, lsl #16			 	
		add		r9,r7,r6, lsl #1	@2b+c	
   		rsb		 r5, r5, #0		@8 - (a+d)
        str      r10,[r3], #4    	
		add		 r11,r6,r4		@a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r8,r7, lsl #1	@2b+c	
   		rsb		 r11, r11, #0		@8 - (a+d)	
@		usat	 r5,#8,r5,asr #4   
		smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r5,lsl #16	
@		usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
		pkhbt	r10, r10, r11, lsl #16		
		add	r0, r0, r2
@		add	sp, sp, #16			
        str      r10,[r3], #4			
@		add	r1, r1, r3					
		bgt		AddH01V01_loop				
@Vertical
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp,#4
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		ldr	r4, LAB_0X01010101
		orr	r12, r12, r12, lsl #16		

AddH01V01_Vloop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r11], #16			@lTemp0 a = 0, 1
        ldr	r6,[r11], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r11], #16			@lTemp2 c = 0, 1  
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1       
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@5
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        @sub      sp,sp,#172	@(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	@(8+3)*8*2 = 176 - 4
        				
		subs	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		bgt		AddH01V01_Vloop						
	
        @add      sp,sp,#160	@(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        ldmfd    sp!,{r4-r11,pc}                     
	@ENDP  	
@/************************************************************************/
@/* ARMV6_AddInterpolate_H02V01 
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1)
@/*	Use vertical filter (-1,12,6,-1) 
@/************************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8@
@			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}	
@}
@huwei 20090819 stack_bug
	.globl ARMV6_AddInterpolate_H02V01 
ARMV6_AddInterpolate_H02V01:  @PROC
		stmfd    sp!,{r4-r11,lr}
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        sub      sp,sp,#180	@(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0] 
        add      r3,sp,#4        	
@Horizontal			
		mov	r14, #11
		sub	r0, r0, r2
		mov	r12, #6	
AddH02V01_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
		add		 r9,r4,r7		@a+d   
		add		 r4,r5,r6, lsl #1	@2b+c	
		rsb		 r9, r9, #0		@0 - (a+d)
		add		 r10,r5,r8		@a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r6,r7, lsl #1	@2b+c	
		rsb		 r10, r10, #0		@0 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
@		usat	 r10,#8,r10,asr #4		
@three@ four      	
		add	r6,r6,r4		@a+d
		pkhbt	r10, r9, r10, lsl #16	
		add		r9,r7,r8, lsl #1	@2b+c	
		rsb		 r6, r6, #0		@8 - (a+d)
		add		 r11,r7,r5		@a+d 
        str      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r8,r4, lsl #1	@2b+c	
   		rsb		 r11, r11, #0		@8 - (a+d)   		
@	usat	 r6,#8,r6,asr #4   
			smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
@		usat	 r11,#8,r11,asr #4
			
@five six
		add		 r9,r8,r6		@a+d   
		add		 r8,r4,r5, lsl #1	@2b+c
		pkhbt	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		@8 - (a+d)
        str      r10,[r3], #4  	
		add		 r10,r4,r7		@a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r5,r6, lsl #1	@2b+c	
		rsb		 r10, r10, #0		@8 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
@		usat	 r10,#8,r10,asr #4
@seven@ eight      	
		add	r5,r5,r8		@a+d
		pkhbt	r10, r9, r10, lsl #16			 	
		add		r9,r6,r7, lsl #1	@2b+c	
		rsb		 r5, r5, #0		@8 - (a+d)
        str      r10,[r3], #4    	
		add		 r11,r6,r4		@a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r7,r8, lsl #1	@2b+c	
   		rsb		 r11, r11, #0		@8 - (a+d)	
@		usat	 r5,#8,r5,asr #4   
		smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r5,lsl #16	
@		usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
		pkhbt	r10, r10, r11, lsl #16		
		add	r0, r0, r2
@		add	sp, sp, #16			
        str      r10,[r3], #4			
@		add	r1, r1, r3					
		bgt		AddH02V01_loop				
@Vertical
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        ldr       r3,[sp,#0]
        add       r11,sp,#4
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		ldr	r4, LAB_0X01010101
		orr	r12, r12, r12, lsl #16	

AddH02V01_Vloop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r11], #16			@lTemp0 a = 0, 1
        ldr	r6,[r11], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r11], #16			@lTemp2 c = 0, 1  
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1       
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@5
@one	
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        @sub      sp,sp,#172	@(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	@(8+3)*8*2 = 176 - 4
        				
		subs	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		bgt		AddH02V01_Vloop						
	
        @add      sp,sp,#160	@(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/******************************************************************************/
@/* ARMV6_AddInterpolate_H00V02 
@/*	0 horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (-1,6,12,-1) 
@/*****************************************************************************/
@#pragma optimize( "", off)ARMV6_AddInterpolate_H00V02
@void RV_FASTCALL  (const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@
@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	for (dstRow = 8@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -= 4)
@		{
@			lTemp0 = pSrc[-(I32)(uSrcPitch)]@
@			lTemp1 = pSrc[0]@
@			lTemp2 = pSrc[uSrcPitch]@
@			lTemp3 = pSrc[uSrcPitch<<1]@
@
@			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = pSrc[3*uSrcPitch]@
@			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = pSrc[4*uSrcPitch]@
@			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = pSrc[5*uSrcPitch]@
@			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4@
@			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			pSrc += (uSrcPitch << 2)@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		pSrc -= ((uSrcPitch * 8) - 1)@
@	}
@}

LAB2_0X01010101: .word 0x01010101
	.globl ARMV6_AddInterpolate_H00V02 
ARMV6_AddInterpolate_H00V02:  @PROC	
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #8						
		sub		 sp,sp,#8
		ands	r4,r0,#3		     	  	
		str	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		bne		unalign_AddH00V02		
align_AddH00V02:
align_AddH00V02_loop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r6,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r7,[r0],#4			@lTemp1 b = 0, 1, 2, 3         
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB_0X01010101		
		orr	r10,r10,r11,lsl #8
		
		ldr	r8, [r1]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
				
		str	r10, [r1] 

        ldr	r5,[r0,-r2]			@lTemp0 a = 0, 1, 2, 3        
        ldr	r6,[r0,r2]			@lTemp2 c = 0, 1, 2, 3  
        ldr	r8,[r0,r2,lsl #1]		@lTemp3 d = 0, 1, 2, 3
        ldr	r7,[r0],#-4			@lTemp1 b = 0, 1, 2, 3 
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)	
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)		
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000				
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB_0X01010101		
		orr	r10,r10,r11,lsl #8
		
		ldr	r8, [r1, #4]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
		
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		bgt		align_AddH00V02_loop						
		b		end_AddH00V02
unalign_AddH00V02:			
		sub		r0,r0,r4
		sub		r0, r0, r2
		mov		r4,r4,lsl #3		@i = i<<3@
unalign_AddH00V02_loop:
		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
@one	
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b			
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b	
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB2_0X01010101		
		orr	r10,r10,r11,lsl #8
		
		ldr	r8, [r1]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
				
		str	r10, [r1] 

		rsb		r10,r4,#0x20			@32 - i
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r5,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r5,r5,LSR r4
	orr		r5,r5,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r7,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r7,r7,LSR r4
	orr		r7,r7,r11,lsl r10
	
        ldr	r9,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r6,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r6,r6,LSR r4
	orr		r6,r6,r9,lsl r10
	
        ldr	r11,[r0,#4]			@lTemp0 a = 4, 5, 6, 7
        ldr	r8,[r0], r2			@lTemp0 a = 0, 1, 2, 3        
	mov		r8,r8,LSR r4
	orr		r8,r8,r11,lsl r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
@tow
		uxtb16		r9, r5		    	@0, 2	a
		uxtb16		r3, r5, ror #8		@1, 3	a		
		uxtab16		r9, r9, r8		@0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	@1, 3	a+d		
		uxtb16		r10, r6		    	@0, 2	b
		uxtb16		r11, r6, ror #8		@1, 3	b		
		@lsl		r10, r10, #1	    	@0, 2	2*b
		@lsl		r11, r11, #1	    	@1, 3	2*b	
		mov		r10, r10, lsl #1	    	@0, 2	2*b
		mov		r11, r11, lsl #1	    	@1, 3	2*b				
		uxtab16		r10, r10, r7		@0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	@1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	@0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	@1, 3	3*(2*b + c)		
		@lsl		r10, r10, #1		@0, 2	6*(2*b + c)
		@lsl		r11, r11, #1		@1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		@0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		@1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		@0, 2	8 - (a+d)
		usub16		r3, r12, r3		@1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		@0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		@1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000					
		mov		r11, r11, asr #4					
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		ldr	r7, LAB2_0X01010101		
		orr	r10,r10,r11,lsl #8
				
		ldr	r8, [r1, #4]
		uqadd8  r10, r10, r7
		uhadd8	r10, r8, r10
				
		str	r10, [r1, #4] 

		ldr	     r3,[sp,#0]					
		subs	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		bgt		unalign_AddH00V02_loop
end_AddH00V02:				                                   
	
		add		 sp,sp,#8							              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 

@/*******************************************************************/
@/* ARMV6_AddInterpolate_H01V02 
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,6,12,-1) 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*19]@
@	I32 *b@
@	const U8 *p@
@	I32   lTemp0, lTemp1@
@	I32   lTemp2, lTemp3@
@
@	b = buff@
@	p = pSrc - (I32)(uSrcPitch)@
@
@	for (dstRow = 8+3@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = p[-1]@
@			lTemp1 = p[0]@
@			lTemp2 = p[1]@
@			lTemp3 = p[2]@
@
@			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3)@
@			b++@
@
@			lTemp0 = p[3]@
@			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0)@
@			b++@
@
@			lTemp1 = p[4]@
@			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1)@
@			b++@
@
@			lTemp2 = p[5]@
@			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2)@			
@			b++@
@
@			p +=4@			
@		}
@		b += (16 - 8)@
@		p += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@			lTemp3 = b[48]@
@
@			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = b[64]@
@			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = b[80]@
@			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = b[96]@
@			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8@
@			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@}
@huwei 20090819 stack_bug
	.globl ARMV6_AddInterpolate_H01V02 
ARMV6_AddInterpolate_H01V02:  @PROC
		stmfd    sp!,{r4-r11,lr}
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        sub      sp,sp,#180	@(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0]
        add      r3,sp,#4	
@Horizontal			
		mov	r14, #11
		sub	r0, r0, r2
		mov	r12, #6	
AddH01V02_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#-1]		@lTemp0
        ldrb     r5,[r0,#0]		@lTemp1   
        ldrb     r6,[r0,#1]		@lTemp2            
        ldrb     r7,[r0,#2]		@lTemp3    
        ldrb     r8,[r0,#3]		@lTemp4

@input  r4~r8
@one @tow   
		add		 r9,r4,r7		@a+d   
		add		 r4,r6,r5, lsl #1	@2b+c	
   		rsb		 r9, r9, #0		@0 - (a+d)
		add		 r10,r5,r8		@a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r7,r6, lsl #1	@2b+c	
   		rsb		 r10, r10, #0		@0 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r5, r12,r10
        ldrb     r4,[r0,#4]		@lTemp5
        ldrb     r5,[r0,#5]		@lTemp6	
@		usat	 r10,#8,r10,asr #4		
@three@ four      	
		add	r6,r6,r4		@a+d
		pkhbt	r10, r9, r10, lsl #16	
		add		r9,r8,r7, lsl #1	@2b+c	
   		rsb		 r6, r6, #0		@8 - (a+d)
		add		 r11,r7,r5		@a+d 
        str      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r4,r8, lsl #1	@2b+c	
   		rsb		 r11, r11, #0		@8 - (a+d)   		
@		usat	 r6,#8,r6,asr #4   
		smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r6,lsl #16	
        ldrb     r6,[r0,#6]		@lTemp5
        ldrb     r7,[r0,#7]		@lTemp6		
@		usat	 r11,#8,r11,asr #4
			
@five six
		add		 r9,r8,r6		@a+d   
		add		 r8,r5,r4, lsl #1	@2b+c
		pkhbt	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		@8 - (a+d)
        str      r10,[r3], #4  	
		add		 r10,r4,r7		@a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r6,r5, lsl #1	@2b+c	
   		rsb		 r10, r10, #0		@8 - (a+d)		
@		usat	 r9,#8,r9,asr #4
		smlabb	 r10, r4, r12,r10
        ldrb     r8,[r0,#8]		@lTemp7
        ldrb     r4,[r0,#9]		@lTemp8	
@		usat	 r10,#8,r10,asr #4
@seven@ eight      	
		add	r5,r5,r8		@a+d
		pkhbt	r10, r9, r10, lsl #16			 	
		add		r9,r7,r6, lsl #1	@2b+c	
   		rsb		 r5, r5, #0		@8 - (a+d)
        str      r10,[r3], #4    	
		add		 r11,r6,r4		@a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r8,r7, lsl #1	@2b+c	
   		rsb		 r11, r11, #0		@8 - (a+d)	
@		usat	 r5,#8,r5,asr #4   
		smlabb	 r11, r7, r12,r11
@		orr	r10,r10,r5,lsl #16	
@		usat	 r11,#8,r11,asr #4				
		subs	r14,r14,#1
		pkhbt	r10, r10, r11, lsl #16		
		add	r0, r0, r2
@		add	sp, sp, #16			
        str      r10,[r3], #4			
@		add	r1, r1, r3					
		bgt		AddH01V02_loop				
@Vertical
        @sub      sp,sp,#176	@(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp,#4
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		ldr	r4, LAB2_0X01010101
		orr	r12, r12, r12, lsl #16	

AddH01V02_Vloop:
@r4~r12		@ -a + 6*(2*b + c) -d
        ldr	r5,[r11], #16			@lTemp0 a = 0, 1
        ldr	r6,[r11], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[r11], #16			@lTemp2 c = 0, 1  
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1       
@one	
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r8,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@5
@one	
		sadd16		r0, r7, r7	    	@2*b
		sadd16		r0, r0, r6	    	@2*b + c
		sadd16		r2, r5, r8	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@two		
		sadd16		r0, r8, r8	    	@2*b
		sadd16		r0, r0, r7	    	@2*b + c
		sadd16		r2, r6, r5	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@three		
		sadd16		r0, r5, r5	    	@2*b
		sadd16		r0, r0, r8	    	@2*b + c
		sadd16		r2, r7, r6	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[r11], #16			@lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
@four		
		sadd16		r0, r6, r6	    	@2*b
		sadd16		r0, r0, r5	    	@2*b + c
		sadd16		r2, r8, r7	    	@a+d
		ssub16		r2, r12, r2	    	@128 -(a+d)
		sxth		r9, r0		    	@0@ 2*b + c
		sxth		r10, r0, ror #16	@1@ 2*b + c	
		add		r9, r9, r9, lsl #1  	@3*(2*b + c)
		add		r10, r10, r10, lsl #1  	@3*(2*b + c)		
		@lsl		r9, r9, #1		@6*(2*b + c)
		@lsl		r10, r10, #1		@6*(2*b + c)
		mov		r9, r9, lsl #1		@6*(2*b + c)
		mov		r10, r10, lsl #1		@6*(2*b + c)
		sxtah		r9, r9, r2		@0@ 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	@1@ 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,asr #8
		usat	 r10,#8,r10,asr #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        @sub      sp,sp,#172	@(8+3)*8*2 = 176 - 4
        sub     r11,r11,#172	@(8+3)*8*2 = 176 - 4
        				
		subs	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		bgt		AddH01V02_Vloop						
	
        @add      sp,sp,#160	@(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180
        ldmfd    sp!,{r4-r11,pc}                     
	@ENDP  
@/****************************************************************/
@/* ARMV6_AddInterpolate_H02V02 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (6,9,1) 
@/*	Use vertical filter (6,9,1) 
@/****************************************************************/
@void RV_FASTCALL  ARMV6_AddInterpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@	I32 dstCol@    
@	I32 buff[16*18]@
@	I32 *b@	
@	I32   lTemp0, lTemp1, lTemp2@    
@
@	b = buff@
@	for (dstRow = 8+2@ dstRow > 0@ dstRow--)
@	{
@		for (dstCol = 8@ dstCol > 0@ dstCol -=4)
@		{
@			lTemp0 = pSrc[0]@
@			lTemp1 = pSrc[1]@
@			lTemp2 = pSrc[2]@
@
@			*b = (6*lTemp0 + 9*lTemp1 + lTemp2)@
@			b++@
@
@			lTemp0 = pSrc[3]@
@			*b = (6*lTemp1 + 9*lTemp2 + lTemp0)@
@			b++@
@
@			lTemp1 = pSrc[4]@
@			*b = (6*lTemp2 + 9*lTemp0 + lTemp1)@
@			b++@
@
@			lTemp2 = pSrc[5]@
@			*b = (6*lTemp0 + 9*lTemp1 + lTemp2)@			
@			b++@
@
@			pSrc +=4@			
@		}
@		b += (16 - 8)@
@		pSrc += (uSrcPitch - 8)@
@	}
@
@	b = buff@
@	for (dstCol = 8@ dstCol > 0@ dstCol--)
@	{
@		for (dstRow = 8@ dstRow > 0@ dstRow -=4)
@		{
@			lTemp0 = b[0]@
@			lTemp1 = b[16]@
@			lTemp2 = b[32]@
@
@			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp0 = b[48]@
@			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8@
@			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp1 = b[64]@
@			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8@
@			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			lTemp2 = b[80]@
@			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8@
@			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1@
@			pDst += uDstPitch@
@
@			b += 64@
@		}
@		pDst -= ((uDstPitch * 8) - 1)@
@		b -= ((8 << 4) - 1)@
@	}
@}
@huwei 20090819 stack_bug
LAB2_0X00090006: .word 0x00090006
	.globl ARMV6_AddInterpolate_H02V02 
ARMV6_AddInterpolate_H02V02:  @PROC
		stmfd    sp!,{r4-r11,lr}
        @sub      sp,sp,#160	@(8+2)*8*2 = 160
        sub      sp,sp,#168	@(8+2)*8*2 + 4 = 164
        mov      r4, #8
        str      r4,[sp,#4]
        str      r3,[sp,#0]
        add      r3,sp,#8	
@Horizontal			
		mov	r14, #10	
AddH02V02_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		

        ldrb     r5,[r0,#0]		@lTemp0   
        ldrb     r6,[r0,#1]		@lTemp1            
        ldrb     r7,[r0,#2]		@lTemp2    
        ldrb     r8,[r0,#3]		@lTemp3

@input  r4~r8
@one @tow	
		add		 r9,r5,r5,lsl #1
		add		 r10,r6,r6,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r7		
		
		add		 r11,r6,r6,lsl #1
		add		 r12,r7,r7,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r8
	
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r5,[r0,#4]		@lTemp4
        ldrb     r6,[r0,#5]		@lTemp5			
@three@ four      	
		add		 r9,r7,r7,lsl #1
		add		 r10,r8,r8,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r5		
		
		add		 r11,r8,r8,lsl #1
		add		 r12,r5,r5,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r6
	
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r7,[r0,#6]		@lTemp4
        ldrb     r8,[r0,#7]		@lTemp5			
@five six
		add		 r9,r5,r5,lsl #1
		add		 r10,r6,r6,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r7		
		
		add		 r11,r6,r6,lsl #1
		add		 r12,r7,r7,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r8
	
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
				
        ldrb     r5,[r0,#8]		@lTemp4
        ldrb     r6,[r0,#9]		@lTemp5	
@seven@ eight  
		add		 r9,r7,r7,lsl #1
		add		 r10,r8,r8,lsl #3
		add		 r10,r10,r9,lsl #1
		add		 r10,r10,r5		
		
		add		 r11,r8,r8,lsl #1
		add		 r12,r5,r5,lsl #3
		add		 r12,r12,r11,lsl #1
		add		 r12,r12,r6
	
		pkhbt	r10, r10, r12, lsl #16			
        str      r10,[r3], #4
    				
		subs	r14,r14,#1	
		add	r0, r0, r2					
		bgt		AddH02V02_loop				
@Vertical
        @sub      sp,sp,#160	@(8+2)*8*2 = 160
        ldr       r3,[sp,#0]
        add       lr,sp,#8
@r0 r2 is free now@  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		@mov	r14, #8
		ldr	r4, LAB2_0X00090006	
		ldr	r0, LAB2_0X01010101

AddH02V02_Vloop:
@r4~r12		@(6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8
        ldr	r5,[lr], #16			@lTemp0 a = 0, 1
        ldr	r6,[lr], #16			@lTemp1 b = 0, 1                 
        ldr	r7,[lr], #16			@lTemp2 c = 0, 1        
@one	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
@two		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
@three		
		pkhbt	r9, r7, r5, lsl #16
		pkhtb	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		@lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
@four	
@one2	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
@two2		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r6,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
@three2		
		pkhbt	r9, r7, r5, lsl #16
		pkhtb	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		@lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r7,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
		
@one3	
		pkhbt	r9, r5, r6, lsl #16
		pkhtb	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		@lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
        ldr	r5,[lr], #16			@lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8

		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3

@two3		
		pkhbt	r9, r6, r7, lsl #16
		pkhtb	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		@lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	@lTemp2 + 128		
		smlad		r8, r9, r4, r8
		smlad		r10, r11, r4, r10	
		usat	 r8,#8,r8,asr #8
		usat	 r10,#8,r10,asr #8
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3


        @sub      sp,sp,#156	@(8+2)*8*2 = 160 - 4
        sub      lr,lr,#156	@(8+2)*8*2 = 160 - 4
        				
		@subs	r14,r14,#2
		ldr 	r2, [sp, #4]   @huwei 20110527	bug fixed			
		subs	r2, r2,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2	
		str     r2, [sp, #4]	
		bgt		AddH02V02_Vloop						
	
        @add      sp,sp,#144	@(8+2)*8*2 = 160 - 4*4
        add      sp,sp,#168
        ldmfd    sp!,{r4-r11,pc}                     
	@ENDP

@/* chroma functions */
@/* Block size is 4x4 for all. */
@
@/******************************************************************/
@/* ARMV6_MCCopyChroma_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/*******************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@
@	/* Do not perform a sequence of U32 copies, since this function */
@	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
@
@	for (dstRow = 4@ dstRow > 0@ dstRow--)
@	{
@		pDst[0] = pSrc[0]@
@		pDst[1] = pSrc[1]@
@		pDst[2] = pSrc[2]@
@		pDst[3] = pSrc[3]@
@
@		pDst += uDstPitch@
@		pSrc += uSrcPitch@
@	}
@}

	.globl ARMV6_MCCopyChroma_H00V00 
ARMV6_MCCopyChroma_H00V00:  @PROC
	stmfd    sp!,{r4-r11,lr}

		ands	r4,r0,#3
		bne		unalign_ChromaH00V00		
align_ChromaH00V00:
align_ChromaH00V00_Chroma_loop:	
		ldr		r4,[r0], r2      @pTempSrc[0]
		ldr		r6,[r0], r2      @pTempSrc[0]		
		ldr		r8,[r0], r2      @pTempSrc[0]
		ldr		r10,[r0], r2      @pTempSrc[0]				
					
		str		r4,[r1],r3      @pTempDst[0]
		str		r6,[r1],r3      @pTempDst[0]
		str		r8,[r1],r3      @pTempDst[0]
		str		r10,[r1],r3      @pTempDst[0]					
		b		end_ChromaH00V00
unalign_ChromaH00V00:			
		sub		r0,r0,r4
		mov		r4,r4,lsl #3		@i = i<<3@
		rsb		r5,r4,#0x20			@32 - i
unalign_ChromaH00V00_Chroma_loop:
		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r6,[r0], r2		@pTempSrc[0]	
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r10,[r0], r2		@pTempSrc[0]							
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5	
		str		r6,[r1],r3
		str		r10,[r1],r3
		
		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r6,[r0], r2		@pTempSrc[0]	
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r10,[r0], r2		@pTempSrc[0]							
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5	
		str		r6,[r1],r3
		str		r10,[r1],r3
end_ChromaH00V00:					
		
        ldmfd    sp!,{r4-r11,pc}
	@ENDP
@/*********************************************************************/
@/* ARMV6_MCCopyChroma_H01V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp, lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@
@		lTemp0 = pSrc[2]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@	
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pSrc += uSrcPitch@
@		pDst += uDstPitch@
@	}
@}
	.globl ARMV6_MCCopyChroma_H01V00 
ARMV6_MCCopyChroma_H01V00:  @PROC
	stmfd    sp!,{r4-r11,lr}		  	
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4			
H01V00_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#0]		@lTemp0
        ldrb     r5,[r0,#1]		@lTemp1   
        ldrb     r6,[r0,#2]		@lTemp2            
        ldrb     r7,[r0,#3]		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r4,r11
		smlabb	 r8,r12,r5,r11	
					
		add		 r4,r5,r5,lsl #1
		add		 r5,r6,r6,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0,#4]		@lTemp4					
	orr		 r9,r9,r8,lsl #8
				
		smlabb	 r5,r12,r6,r11
		smlabb	 r8,r12,r7,r11
								
		add		 r6,r7,r7,lsl #1
		add		 r7,r4,r4,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
													
	orr		 r9,r9,r5,lsl #16	
	orr		 r9,r9,r8,lsl #24	
		str      r9,[r1],r3					

		subs	r14,r14,#1	
		add	r0, r0, r2
		bgt		H01V00_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP     	

@/******************************************************************/
@/* ARMV6_MCCopyChroma_H02V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp, lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@
@		lTemp0 = pSrc[2]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@	
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pSrc += uSrcPitch@
@		pDst += uDstPitch@
@	}
@}
	.globl ARMV6_MCCopyChroma_H02V00 
ARMV6_MCCopyChroma_H02V00:  @PROC
	stmfd    sp!,{r4-r11,lr}		  	
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4			
H02V00_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#0]		@lTemp0
        ldrb     r5,[r0,#1]		@lTemp1   
        ldrb     r6,[r0,#2]		@lTemp2            
        ldrb     r7,[r0,#3]		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r5,r11
		smlabb	 r8,r12,r6,r11	
					
		add		 r4,r4,r4,lsl #1
		add		 r5,r5,r5,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0,#4]		@lTemp4					
	orr		 r9,r9,r8,lsl #8
				
		smlabb	 r5,r12,r7,r11
		smlabb	 r8,r12,r4,r11
								
		add		 r6,r6,r6,lsl #1
		add		 r7,r7,r7,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
													
	orr		 r9,r9,r5,lsl #16	
	orr		 r9,r9,r8,lsl #24	
		subs	r14,r14,#1	
		add	r0, r0, r2	
		str      r9,[r1],r3					
		bgt		H02V00_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP     	 
@/******************************************************************/
@/* ARMV6_MCCopyChroma_H00V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@	
@	U32 lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[uSrcPitch]@
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		pDst[0] = (U8)lTemp0@
@
@		lTemp0 = pSrc[uSrcPitch << 1]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		pDst[uDstPitch] = (U8)lTemp1@
@
@		lTemp1 = pSrc[3*uSrcPitch]@
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		pDst[uDstPitch << 1] = (U8)lTemp0@
@
@		lTemp0 = pSrc[uSrcPitch << 2]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		pDst[3*uDstPitch] = (U8)lTemp1@
@
@		pDst++@
@		pSrc++@
@	}
@}
	.globl ARMV6_MCCopyChroma_H00V01 
ARMV6_MCCopyChroma_H00V01:  @PROC
	stmfd    sp!,{r4-r11,lr}		  	
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4			
H00V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0], r2		@lTemp0
        ldrb     r5,[r0], r2		@lTemp1   
        ldrb     r6,[r0], r2		@lTemp2            
        ldrb     r7,[r0], r2		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r4,r11
		smlabb	 r8,r12,r5,r11	
					
		add		 r4,r5,r5,lsl #1
		add		 r5,r6,r6,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0]		@lTemp4	
        	
        strb     r9,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1   
				
		smlabb	 r5,r12,r6,r11
		smlabb	 r8,r12,r7,r11
								
		add		 r6,r7,r7,lsl #1
		add		 r7,r4,r4,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
	
		subs	r14,r14,#1	
		sub	r0, r0, r2, lsl #2													
        strb     r5,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1					

		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		bgt		H00V01_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/*****************************************************************/
@/* ARMV6_MCCopyChroma_H00V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/*****************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[uSrcPitch]@
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		pDst[0] = (U8)lTemp0@
@
@		lTemp0 = pSrc[uSrcPitch << 1]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		pDst[uDstPitch] = (U8)lTemp1@
@
@		lTemp1 = pSrc[3*uSrcPitch]@
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		pDst[uDstPitch << 1] = (U8)lTemp0@
@
@		lTemp0 = pSrc[uSrcPitch << 2]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		pDst[3*uDstPitch] = (U8)lTemp1@
@
@		pDst++@
@		pSrc++@
@	}
@}
	.globl ARMV6_MCCopyChroma_H00V02 
ARMV6_MCCopyChroma_H00V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4			
H00V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0], r2		@lTemp0
        ldrb     r5,[r0], r2		@lTemp1   
        ldrb     r6,[r0], r2		@lTemp2            
        ldrb     r7,[r0], r2		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r5,r11
		smlabb	 r8,r12,r6,r11	
					
		add		 r4,r4,r4,lsl #1
		add		 r5,r5,r5,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0]		@lTemp4	
        	
        strb     r9,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1   
				
		smlabb	 r5,r12,r7,r11
		smlabb	 r8,r12,r4,r11
								
		add		 r6,r6,r6,lsl #1
		add		 r7,r7,r7,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		

		subs	r14,r14,#1	
		sub	r0, r0, r2, lsl #2													
        strb     r5,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1					
        
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		bgt		H00V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/******************************************************************/
@/* ARMV6_MCCopyChroma_H01V01 
@/*	Motion compensated chroma 4x4 block copy.
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/*******************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@}
	.globl ARMV6_MCCopyChroma_H01V01 
ARMV6_MCCopyChroma_H01V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
H01V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r9,r9,lsl #3	@9*lTemp3
		add	r10, r5, r8		@lTemp1 + lTemp2				
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r8,r8,lsl #3	@9*lTemp3
		add	r10, r4, r9		@lTemp1 + lTemp2				
		smlabb	 r11,r12,r5,r11		@25*lTemp0 + 9*lTemp3
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r8,r8,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r5,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
        	sub	r0, r0, #4		
		orr		 r14,r14,r10,lsl #24
        	add	r0, r0, r2 			
        str     r14,[r1], r3		@lTemp1					

		subs	r7,r7,#1				
		bgt		H01V01_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/*****************************************************************/
@/* ARMV6_MCCopyChroma_H02V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@
@}
	.globl ARMV6_MCCopyChroma_H02V01 
ARMV6_MCCopyChroma_H02V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
H02V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r8,r8,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r5,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r8,r8,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r5,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
		orr		 r14,r14,r10,lsl #24
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        str     r14,[r1], r3		@lTemp1
      					

		subs	r7,r7,#1				
		bgt		H02V01_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/**********************************************************************/
@/* ARMV6_MCCopyChroma_H01V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32	  lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@}
	.globl ARMV6_MCCopyChroma_H01V02 
ARMV6_MCCopyChroma_H01V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
H01V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r5,r5,lsl #3	@9*lTemp1		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r8,r11		@25*lTemp2 + 9*lTemp1	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r5,r5,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r8,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
		orr		 r14,r14,r10,lsl #24
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        str     r14,[r1], r3		@lTemp1
      					

		subs	r7,r7,#1				
		bgt		H01V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/*******************************************************************/
@/* ARMV6_MCCopyChroma_H02V02 
@/*	Motion compensated 4x4 chroma block copy. 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_MCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	const U8 *pSrc2 = pSrc + uPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		*((PU32)pDst) = lTemp@
@		pDst  += uDstPitch@
@		pSrc  += uPitch@
@		pSrc2 += uPitch@
@	}
@
@}
	.globl ARMV6_MCCopyChroma_H02V02 
ARMV6_MCCopyChroma_H02V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
H02V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r5,r5,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r8,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r5,r5,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r8,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #24	
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        str     r14,[r1], r3		@lTemp1      					

		subs	r7,r7,#1				
		bgt		H02V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/******************************************************************/
@/* ARMV6_AddMCCopyChroma_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/*******************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 dstRow@
@
@	/* Do not perform a sequence of U32 copies, since this function */
@	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
@
@	for (dstRow = 4@ dstRow > 0@ dstRow--)
@	{
@		pDst[0] = (pSrc[0] + pDst[0] + 1)>>1@
@		pDst[1] = (pSrc[1] + pDst[1] + 1)>>1@
@		pDst[2] = (pSrc[2] + pDst[2] + 1)>>1@
@		pDst[3] = (pSrc[3] + pDst[3] + 1)>>1@
@
@		pDst += uDstPitch@
@		pSrc += uSrcPitch@
@	}
@}
cLAB_0X01010101: .word 0x01010101
	.globl ARMV6_AddMCCopyChroma_H00V00 
ARMV6_AddMCCopyChroma_H00V00:  @PROC
        stmfd    sp!,{r4-r11,lr}
        	  
		ands	r4,r0,#3
		ldr	r12, cLAB_0X01010101					
		bne		unalign_Chroma_AddH00V00		
align_Chroma_AddH00V00:
		mov	r14, r1
align_Chroma_AddH00V00_Chroma_loop:
		ldr		r4,[r0], r2      @pTempSrc[0]
		ldr		r6,[r0], r2      @pTempSrc[0]		
		ldr		r8,[r0], r2      @pTempSrc[0]
		ldr		r10,[r0], r2      @pTempSrc[0]
		
		ldr		r5,[r14], r3      @pTempSrc[0]
		ldr		r7,[r14], r3      @pTempSrc[0]		
		ldr		r9,[r14], r3      @pTempSrc[0]
		ldr		r11,[r14], r3      @pTempSrc[0]						
			
		uqadd8  r4, r4, r12
		uqadd8  r6, r6, r12
		uqadd8  r8, r8, r12
		uqadd8  r10, r10, r12		
				  
		uhadd8	r4, r4, r5
		uhadd8	r6, r6, r7
		uhadd8	r8, r8, r9		
		uhadd8	r10, r10, r11
							
		str		r4,[r1],r3      @pTempDst[0]
		str		r6,[r1],r3      @pTempDst[0]
		str		r8,[r1],r3      @pTempDst[0]
		str		r10,[r1],r3      @pTempDst[0]
						
		b		end_Chroma_AddH00V00
unalign_Chroma_AddH00V00:		
		sub		r0,r0,r4
		mov		r4,r4,lsl #3		@i = i<<3@
		rsb		r5,r4,#0x20			@32 - i
unalign_Chroma_AddH00V00_Chroma_loop:

		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r6,[r0], r2		@pTempSrc[0]	
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r10,[r0], r2		@pTempSrc[0]							
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5
		
		ldr		r8,[r1]      @pTempSrc[0]
		ldr		r9,[r1, r3]      @pTempSrc[0]		
		uqadd8  r6, r6, r12
		uqadd8  r10, r10, r12
		uhadd8	r6, r6, r8
		uhadd8	r10, r10, r9		
					
		str		r6,[r1],r3
		str		r10,[r1],r3

		ldr		r7,[r0,#4]		@pTempSrc[1]
		ldr		r6,[r0], r2		@pTempSrc[0]	
		ldr		r11,[r0,#4]		@pTempSrc[1]
		ldr		r10,[r0], r2		@pTempSrc[0]							
		mov		r6,r6,LSR r4
		mov		r10,r10,LSR r4			
		orr		r6,r6,r7,lsl r5
		orr		r10,r10,r11,lsl r5
		
		ldr		r8,[r1]      @pTempSrc[0]
		ldr		r9,[r1, r3]      @pTempSrc[0]		
		uqadd8  r6, r6, r12
		uqadd8  r10, r10, r12
		uhadd8	r6, r6, r8
		uhadd8	r10, r10, r9		
					
		str		r6,[r1],r3
		str		r10,[r1],r3
end_Chroma_AddH00V00:				

        ldmfd    sp!,{r4-r11,pc}
	@ENDP	
@/*********************************************************************/
@/* ARMV6_AddMCCopyChroma_H01V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp, lTemp0, lTemp1@
@	U32 c,q@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@
@		lTemp0 = pSrc[2]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@	
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pSrc += uSrcPitch@
@		pDst += uDstPitch@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H01V00 
ARMV6_AddMCCopyChroma_H01V00:  @PROC
	stmfd    sp!,{r4-r11,lr}
	
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4
		ldr	r10, cLAB_0X01010101					
Chroma_AddH01V00_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#0]		@lTemp0
        ldrb     r5,[r0,#1]		@lTemp1   
        ldrb     r6,[r0,#2]		@lTemp2            
        ldrb     r7,[r0,#3]		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r4,r11
		smlabb	 r8,r12,r5,r11	
					
		add		 r4,r5,r5,lsl #1
		add		 r5,r6,r6,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0,#4]		@lTemp4					
	orr		 r9,r9,r8,lsl #8
				
		smlabb	 r5,r12,r6,r11
		smlabb	 r8,r12,r7,r11
								
		add		 r6,r7,r7,lsl #1
		add		 r7,r4,r4,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
													
	orr		 r9,r9,r5,lsl #16	
	orr		 r9,r9,r8,lsl #24
	
        ldr     r4,[r1,#0]		@lTemp0				
		uqadd8  r9, r9, r10
		uhadd8	r9, r9, r4
		subs	r14,r14,#1	
		add	r0, r0, r2			
		str      r9,[r1],r3					

					
		bgt		Chroma_AddH01V00_Chroma_loop				

        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP     

@/******************************************************************/
@/* ARMV6_AddMCCopyChroma_H02V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp, lTemp0, lTemp1@
@	U32 c,q@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@
@		lTemp0 = pSrc[2]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@	
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pSrc += uSrcPitch@
@		pDst += uDstPitch@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H02V00 
ARMV6_AddMCCopyChroma_H02V00:  @PROC
	stmfd    sp!,{r4-r11,lr}		  	
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4	
		ldr	r10, cLAB_0X01010101				
Chroma_AddH02V00_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0,#0]		@lTemp0
        ldrb     r5,[r0,#1]		@lTemp1   
        ldrb     r6,[r0,#2]		@lTemp2            
        ldrb     r7,[r0,#3]		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r5,r11
		smlabb	 r8,r12,r6,r11	
					
		add		 r4,r4,r4,lsl #1
		add		 r5,r5,r5,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
        	ldrb     r4,[r0,#4]		@lTemp4					
	orr		 r9,r9,r8,lsl #8
				
		smlabb	 r5,r12,r7,r11
		smlabb	 r8,r12,r4,r11
								
		add		 r6,r6,r6,lsl #1
		add		 r7,r7,r7,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
													
	orr		 r9,r9,r5,lsl #16	
	orr		 r9,r9,r8,lsl #24
	
        ldr     r4,[r1,#0]		@lTemp0				
		uqadd8  r9, r9, r10
		uhadd8	r9, r9, r4
		subs	r14,r14,#1	
		add	r0, r0, r2			
		str      r9,[r1],r3					


		bgt		Chroma_AddH02V00_Chroma_loop				
								              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP   
@/******************************************************************/
@/* ARMV6_AddMCCopyChroma_H00V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@	
@	U32 lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[uSrcPitch]@
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1@
@
@		lTemp0 = pSrc[uSrcPitch << 1]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1@
@
@		lTemp1 = pSrc[3*uSrcPitch]@
@		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3@
@		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1@
@
@		lTemp0 = pSrc[uSrcPitch << 2]@
@		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3@
@		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1@
@
@		pDst++@
@		pSrc++@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H00V01 
ARMV6_AddMCCopyChroma_H00V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4
		mov	r10, #1			
Chroma_AddH00V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0], r2		@lTemp0
        ldrb     r5,[r0], r2		@lTemp1   
        ldrb     r6,[r0], r2		@lTemp2            
        ldrb     r7,[r0], r2		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r4,r11
		smlabb	 r8,r12,r5,r11	
					
		add		 r4,r5,r5,lsl #1
		add		 r5,r6,r6,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3
		
		ldrb	r4, [r1]
		ldrb	r5, [r1, r3]
		uqadd8  r9, r9, r10
		uqadd8  r8, r8, r10
		uhadd8	r9, r9, r4
		uhadd8	r8, r8, r5							
        	ldrb     r4,[r0]		@lTemp4        	
        strb     r9,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1   

				
		smlabb	 r5,r12,r6,r11
		smlabb	 r8,r12,r7,r11
								
		add		 r6,r7,r7,lsl #1
		add		 r7,r4,r4,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		

		ldrb	r4, [r1]
		ldrb	r6, [r1, r3]
		uqadd8  r5, r5, r10
		uqadd8  r8, r8, r10
		uhadd8	r5, r5, r4
		uhadd8	r8, r8, r6
															
        strb     r5,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1					

		subs	r14,r14,#1	
		sub	r0, r0, r2, lsl #2
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		bgt		Chroma_AddH00V01_Chroma_loop										              	 		
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	
@/*****************************************************************/
@/* ARMV6_AddMCCopyChroma_H00V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/*****************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32 j@
@	U32 lTemp0, lTemp1@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[uSrcPitch]@
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1@
@
@		lTemp0 = pSrc[uSrcPitch << 1]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1@
@
@		lTemp1 = pSrc[3*uSrcPitch]@
@		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3@
@		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1@
@
@		lTemp0 = pSrc[uSrcPitch << 2]@
@		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3@
@		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1@
@		pDst++@
@		pSrc++@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H00V02 
ARMV6_AddMCCopyChroma_H00V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r14, #4
		mov	r12, #5
		mov	r11,#4	
		mov	r10, #1					
Chroma_AddH00V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d		
        ldrb     r4,[r0], r2		@lTemp0
        ldrb     r5,[r0], r2		@lTemp1   
        ldrb     r6,[r0], r2		@lTemp2            
        ldrb     r7,[r0], r2		@lTemp3    

@input  r4~r8
@one @tow  
		smlabb	 r9,r12,r5,r11
		smlabb	 r8,r12,r6,r11	
					
		add		 r4,r4,r4,lsl #1
		add		 r5,r5,r5,lsl #1
					
		add		 r9,r9,r4
		add		 r8,r8,r5
						
		mov		 r9,r9,asr #3
		mov		 r8,r8,asr #3	
        	
		ldrb	r4, [r1]
		ldrb	r5, [r1, r3]
		uqadd8  r9, r9, r10
		uqadd8  r8, r8, r10
		uhadd8	r9, r9, r4
		uhadd8	r8, r8, r5
									
        	ldrb     r4,[r0]		@lTemp4        	
        strb     r9,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1    
				
		smlabb	 r5,r12,r7,r11
		smlabb	 r8,r12,r4,r11
								
		add		 r6,r6,r6,lsl #1
		add		 r7,r7,r7,lsl #1
					
		add		 r5,r5,r6
		add		 r8,r8,r7
						
		mov		 r5,r5,asr #3
		mov		 r8,r8,asr #3		
													
		ldrb	r4, [r1]
		ldrb	r6, [r1, r3]
		uqadd8  r5, r5, r10
		uqadd8  r8, r8, r10
		uhadd8	r5, r5, r4
		uhadd8	r8, r8, r6
															
        strb     r5,[r1], r3		@lTemp0
        strb     r8,[r1], r3		@lTemp1					

		subs	r14,r14,#1	
		sub	r0, r0, r2, lsl #2
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		bgt		Chroma_AddH00V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 
@/******************************************************************/
@/* ARMV6_AddMCCopyChroma_H01V01 
@/*	Motion compensated chroma 4x4 block copy.
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/*******************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32   j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	U32   c,q@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H01V01 
ARMV6_AddMCCopyChroma_H01V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
Chroma_AddH01V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r8,r8,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r5,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r8,r8,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r5,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
	ldr	r5, cLAB_0X01010101		
		orr		 r14,r14,r10,lsl #24
	
        ldr     r4,[r1]			@lTemp1		
		uqadd8  r14, r14, r5
       	sub	r0, r0, #4		
		uhadd8	r14, r14, r4
        	add	r0, r0, r2  				
        str     r14,[r1], r3		@lTemp1
     					

		subs	r7,r7,#1				
		bgt		Chroma_AddH01V01_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	


@/*****************************************************************/
@/* ARMV6_AddMCCopyChroma_H02V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32   j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	U32   c,q@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@
@}
	.globl ARMV6_AddMCCopyChroma_H02V01 
ARMV6_AddMCCopyChroma_H02V01:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
Chroma_AddH02V01_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r8,r8,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r5,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r9,r9,lsl #3	@9*lTemp3
		add	r10, r5, r8		@lTemp1 + lTemp2				
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r8,r8,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r5,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r9,r9,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r4,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
	ldr	r5, cLAB_0X01010101		
		orr		 r14,r14,r10,lsl #24
	
        ldr     r4,[r1]			@lTemp1
		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4	

        	add	r0, r0, r2 				
        str     r14,[r1], r3		@lTemp1
      					

		subs	r7,r7,#1				
		bgt		Chroma_AddH02V01_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/**********************************************************************/
@/* ARMV6_AddMCCopyChroma_H01V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
@{
@	I32   j@
@	U32	  lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	U32   c,q@
@	const U8 *pSrc2 = pSrc + uSrcPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pDst  += uDstPitch@
@		pSrc  += uSrcPitch@
@		pSrc2 += uSrcPitch@
@	}
@}
	.globl ARMV6_AddMCCopyChroma_H01V02 
ARMV6_AddMCCopyChroma_H01V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
Chroma_AddH01V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r5,r5,lsl #3	@9*lTemp1		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r8,r11		@25*lTemp2 + 9*lTemp1	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r5,r5,lsl #3	@9*lTemp2		
		add	r10, r4, r9		@lTemp0 + lTemp3		
		smlabb	 r11,r12,r8,r11		@25*lTemp1 + 9*lTemp2	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp0 + lTemp3)
		add	r10, r10, #32		@15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
	ldr	r5, cLAB_0X01010101		
		orr		 r14,r14,r10,lsl #24
	
        ldr     r4,[r1]			@lTemp1		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4		
        	add	r0, r0, r2			
        str     r14,[r1], r3		@lTemp1       					

		subs	r7,r7,#1				
		bgt		Chroma_AddH01V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP 	

@/*******************************************************************/
@/* ARMV6_AddMCCopyChroma_H02V02 
@/*	Motion compensated 4x4 chroma block copy. 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  ARMV6_AddMCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
@{
@	I32   j@
@	U32   lTemp@
@	U32   lTemp0, lTemp1@
@	U32   lTemp2, lTemp3@
@	U32   c,q@
@	const U8 *pSrc2 = pSrc + uPitch@
@
@	for (j = 4@ j > 0@ j--)
@	{
@		lTemp0 = pSrc[0]@
@		lTemp1 = pSrc[1]@
@		lTemp2 = pSrc2[0]@
@		lTemp3 = pSrc2[1]@
@		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6@
@
@		lTemp0 = pSrc[2]@
@		lTemp2 = pSrc2[2]@
@		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 8)@
@
@		lTemp1 = pSrc[3]@
@		lTemp3 = pSrc2[3]@
@		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6@
@		lTemp |= (lTemp0 << 16)@
@
@		lTemp0 = pSrc[4]@
@		lTemp2 = pSrc2[4]@
@		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6@
@		lTemp |= (lTemp1 << 24)@
@
@		c=((U32 *)pDst)[0]@
@		q=(lTemp|c) & 0x01010101@
@		q+=(lTemp>>1) & 0x7F7F7F7F@
@		q+=(c>>1) & 0x7F7F7F7F@
@
@		*((PU32)pDst) = q@
@		pDst  += uDstPitch@
@		pSrc  += uPitch@
@		pSrc2 += uPitch@
@	}
@
@}
	.globl ARMV6_AddMCCopyChroma_H02V02 
ARMV6_AddMCCopyChroma_H02V02:  @PROC
	stmfd    sp!,{r4-r11,lr}
		mov	r12, #25
		mov	r7, #4			
Chroma_AddH02V02_Chroma_loop:
@r4~r9, r10~12		@ -a + 6*(2*b + c) -d
        ldrb     r8,[r0, r2]		@lTemp00
        ldrb     r4,[r0], #1		@lTemp0	
        ldrb     r9,[r0, r2]		@lTemp11                       	
        ldrb     r5,[r0], #1		@lTemp1
@input  r4,r5,r8,r9 
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r14,r10,asr #6
						
        ldrb     r8,[r0, r2]		@lTemp33						
        ldrb     r4,[r0], #1		@lTemp22        
@input  r5,r4,r9,r8  
		add	r11,r5,r5,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r8,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
		orr		 r14,r14,r10,lsl #8			
		
        ldrb     r9,[r0, r2]		@lTemp33		
        ldrb     r5,[r0], #1		@lTemp22
@input  r4,r5,r8,r9 
		add	r11,r4,r4,lsl #3	@9*lTemp3		
		add	r10, r5, r8		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r9,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6
				
        ldrb     r8,[r0, r2]		@lTemp33				
        ldrb     r4,[r0]		@lTemp22
		orr		 r14,r14,r10,lsl #16
@input  r5,r4,r9,r8  
		add	r11,r5,r5,lsl #3	@9*lTemp3		
		add	r10, r4, r9		@lTemp1 + lTemp2		
		smlabb	 r11,r12,r8,r11		@25*lTemp0 + 9*lTemp3	
		rsb	r10,r10,r10,lsl #4	@15*(lTemp1 + lTemp2)
		add	r10, r10, #32		@15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		mov		 r10,r10,asr #6	
	ldr	r5, cLAB_0X01010101		
		orr		 r14,r14,r10,lsl #24
	
        ldr     r4,[r1]			@lTemp1		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4		
        	add	r0, r0, r2  			
        str     r14,[r1], r3		@lTemp1     					

		subs	r7,r7,#1				
		bgt		Chroma_AddH02V02_Chroma_loop				
	
        ldmfd    sp!,{r4-r11,pc}                      
	@ENDP
	
	@END




