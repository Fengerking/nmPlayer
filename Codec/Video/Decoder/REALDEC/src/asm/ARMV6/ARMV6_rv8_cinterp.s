;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2005		            *
;	written by John							 	                                    *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY

;const T_InterpFnxTableNewPtr gARMV6_ARMV6_InterpolateTable[16] =
;{
;	ARMV6_Interpolate_H00V00, ARMV6_Interpolate_H01V00,
;	ARMV6_Interpolate_H02V00, NULL,
;	ARMV6_Interpolate_H00V01, ARMV6_Interpolate_H01V01,
;	ARMV6_Interpolate_H02V01, NULL,
;	ARMV6_Interpolate_H00V02, ARMV6_Interpolate_H01V02,
;	ARMV6_Interpolate_H02V02, NULL
;};
;
;const T_InterpFnxTableNewPtr gARMV6_ARMV6_InterpolateChromaTable[16] =
;{
;	ARMV6_MCCopyChroma_H00V00, ARMV6_MCCopyChroma_H01V00,
;	ARMV6_MCCopyChroma_H02V00, NULL,
;	ARMV6_MCCopyChroma_H00V01, ARMV6_MCCopyChroma_H01V01,
;	ARMV6_MCCopyChroma_H02V01, NULL,
;	ARMV6_MCCopyChroma_H00V02, ARMV6_MCCopyChroma_H01V02,
;	ARMV6_MCCopyChroma_H02V02, NULL
;};
;
;const T_InterpFnxTableNewPtr gARMV6_ARMV6_AddInterpolateTable[16] =
;{
;	ARMV6_AddInterpolate_H00V00, ARMV6_AddInterpolate_H01V00,
;	ARMV6_AddInterpolate_H02V00, NULL,
;	ARMV6_AddInterpolate_H00V01, ARMV6_AddInterpolate_H01V01,
;	ARMV6_AddInterpolate_H02V01, NULL,
;	ARMV6_AddInterpolate_H00V02, ARMV6_AddInterpolate_H01V02,
;	ARMV6_AddInterpolate_H02V02, NULL
;};
;
;const T_InterpFnxTableNewPtr gARMV6_ARMV6_AddInterpolateChromaTable[16] =
;{
;	ARMV6_AddMCCopyChroma_H00V00, ARMV6_AddMCCopyChroma_H01V00,
;	ARMV6_AddMCCopyChroma_H02V00, NULL,
;	ARMV6_AddMCCopyChroma_H00V01, ARMV6_AddMCCopyChroma_H01V01,
;	ARMV6_AddMCCopyChroma_H02V01, NULL,
;	ARMV6_AddMCCopyChroma_H00V02, ARMV6_AddMCCopyChroma_H01V02,
;	ARMV6_AddMCCopyChroma_H02V02, NULL
;};
	 
;/******************************************************************/
;/* ARMV6_Interpolate_H00V00 
;/*	 0 horizontal displacement 
;/*	 0 vertical displacement 
;/*	 No interpolation required, simple block copy. 
;/**************************************************************** */
;void   ARMV6_Interpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;
;	/* Do not perform a sequence of U32 copies, since this function  */
;	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
;
;	/*MAP -- Loops are modified as decrementing loops for the purpose*/
;	/*of ARM optimization.											 */
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		memcpy(pDst, pSrc, 8); /* Flawfinder: ignore */
;
;		pDst += uDstPitch;
;		pSrc += uSrcPitch;
;	}
;}	/* H00V00 */

	EXPORT ARMV6_Interpolate_H00V00 
ARMV6_Interpolate_H00V00  PROC
        STMFD    sp!,{r4-r11,lr}
        	  
		ANDS	r4,r0,#3
		MOV	r14, #8	
		BNE		unalign_H00V00		
align_H00V00
align_H00V00_loop	
		LDR		r5,[r0,#4]      ;pTempSrc[1]
		LDR		r4,[r0], r2      ;pTempSrc[0]
		LDR		r7,[r0,#4]      ;pTempSrc[1]
		LDR		r6,[r0], r2      ;pTempSrc[0]		
		LDR		r9,[r0,#4]      ;pTempSrc[1]
		LDR		r8,[r0], r2      ;pTempSrc[0]
		LDR		r11,[r0,#4]      ;pTempSrc[1]
		LDR		r10,[r0], r2      ;pTempSrc[0]				
					

		STRD		r4,[r1],r3      ;pTempDst[0]
		STRD		r6,[r1],r3      ;pTempDst[0]
		STRD		r8,[r1],r3      ;pTempDst[0]
		STRD		r10,[r1],r3      ;pTempDst[0]		
		
;		SUBS	r14,r14,#4

		LDR		r5,[r0,#4]      ;pTempSrc[1]
		LDR		r4,[r0], r2      ;pTempSrc[0]
		LDR		r7,[r0,#4]      ;pTempSrc[1]
		LDR		r6,[r0], r2      ;pTempSrc[0]	
		LDR		r9,[r0,#4]      ;pTempSrc[1]
		LDR		r8,[r0], r2      ;pTempSrc[0]
		LDR		r11,[r0,#4]      ;pTempSrc[1]
		LDR		r10,[r0], r2      ;pTempSrc[0]				
					
		STRD		r4,[r1],r3      ;pTempDst[0]
		STRD		r6,[r1],r3      ;pTempDst[0]
		STRD		r8,[r1],r3      ;pTempDst[0]
		STRD		r10,[r1],r3      ;pTempDst[0]							
;		BGT		align_H00V00_loop						
		B		end_H00V00
unalign_H00V00			
		SUB		r0,r0,r4
		MOV		r4,r4,LSL #3		;i = i<<3;
		RSB		r5,r4,#0x20			;32 - i
unalign_H00V00_loop
		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r8,[r0,#8]		;pTempSrc[2]
		LDR		r6,[r0], r2		;pTempSrc[0]
		
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r9,[r0,#8]		;pTempSrc[2]
		LDR		r10,[r0], r2		;pTempSrc[0]
								
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5
		MOV		r7,r7,LSR r4
		MOV		r11,r11,LSR r4
		ORR		r7,r7,r8,LSL r5 
		ORR		r11,r11,r9,LSL r5 
		
		SUBS	r14,r14,#2
		STRD		r6,[r1],r3
		STRD		r10,[r1],r3					
		BGT		unalign_H00V00_loop
end_H00V00				
	
        LDMFD    sp!,{r4-r11,pc}
	ENDP


;/*******************************************************************/
;/* ARMV6_Interpolate_H01V00 
;/*	1/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/********************************************************************/
;void   ARMV6_Interpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	U32 lTemp;
;	I32 dstRow, dstCol;        
;	I32 lTemp0, lTemp1;
;	I32 lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
;	/*temporary variables lTempX, so that number of loads can be minimized. */
;	/*Decrementing loops are used for the purpose of optimization			*/
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-1]; 
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[1];
;			lTemp3 = pSrc[2];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4;
;			lTemp  = ClampVal(lTemp0);
;
;			lTemp0 = pSrc[3];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp1)) << 8;
;
;			lTemp1 = pSrc[4];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp2)) << 16;
;
;			lTemp2 = pSrc[5];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp3)) << 24;
;
;			*((PU32)pDst)++ = lTemp;
;			pSrc += 4;
;		}
;		pDst += (uDstPitch - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;}
	export ARMV6_Interpolate_H01V00 
ARMV6_Interpolate_H01V00  PROC
        STMFD    sp!,{r4-r11,lr}
        	  	
		MOV	r14, #8
		MOV	r12, #6	
H01V00_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r9, r9, #8		;8 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
    LDRB     r4,[r0,#4]		;lTemp5
    LDRB     r5,[r0,#5]		;lTemp6	
	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r8,r7, lsl #1	;2b+c	
   	rsb		 r6, r6, #8		;8 - (a+d)
	add		 r11,r7,r5		;a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
	usat	 r11,#8,r11,ASR #4
			
;five six
	add		 r9,r8,r6		;a+d   
	add		 r8,r5,r4, lsl #1	;2b+c	
	orr	r10,r10,r11,LSL #24	
   	rsb		 r9, r9, #8		;8 - (a+d)
        STR      r10,[r1]   	
	add		 r10,r4,r7		;a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r7,r6, lsl #1	;2b+c	
   	rsb		 r5, r5, #8		;8 - (a+d)
	add		 r11,r6,r4		;a+d   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,LSL #16	
	usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
	orr	r10,r10,r11,LSL #24		
		add	r0, r0, r2	
        STR      r10,[r1,#4]			
		add	r1, r1, r3					
		BGT		H01V00_loop				
	
        LDMFD    sp!,{r4-r11,pc}
	ENDP                            
                                            
;/******************************************************************/
;/* ARMV6_Interpolate_H02V00                     
;/*	2/3 pel horizontal displacement     
;/*	0 vertical displacement             
;/*	Use horizontal filter (-1,6,12,-1)  
;/******************************************************************/
;void   ARMV6_Interpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{                                           
;	U32 lTemp;                          
;	I32 dstRow, dstCol;                 
;	I32 lTemp0, lTemp1;                 
;	I32 lTemp2, lTemp3;                 
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-1]; 
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[1];
;			lTemp3 = pSrc[2];
;
;			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4;
;			lTemp  = ClampVal(lTemp0);
;
;			lTemp0 = pSrc[3];
;			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp1)) << 8;
;
;			lTemp1 = pSrc[4];
;			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp2)) << 16;
;
;			lTemp2 = pSrc[5];
;			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp3)) << 24;
;
;			*((PU32)pDst)++ = lTemp;
;			pSrc += 4;		
;		}
;		pDst += (uDstPitch - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;}

	EXPORT ARMV6_Interpolate_H02V00 
ARMV6_Interpolate_H02V00  PROC
	STMFD    sp!,{r4-r11,lr}
		
		MOV	r14, #8
		MOV	r12, #6	
H02V00_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r9, r9, #8		;8 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r7,r8, lsl #1	;2b+c	
   	rsb		 r6, r6, #8		;8 - (a+d)
	add		 r11,r7,r5		;a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
	usat	 r11,#8,r11,ASR #4
			
;five six
	add		 r9,r8,r6		;a+d   
	add		 r8,r4,r5, lsl #1	;2b+c	
	orr	r10,r10,r11,LSL #24	
   	rsb		 r9, r9, #8		;8 - (a+d)
        STR      r10,[r1]   	
	add		 r10,r4,r7		;a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r6,r7, lsl #1	;2b+c	
   	rsb		 r5, r5, #8		;8 - (a+d)
	add		 r11,r6,r4		;a+d   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,LSL #16	
	usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
	orr	r10,r10,r11,LSL #24		
		add	r0, r0, r2	
        STR      r10,[r1,#4]			
		add	r1, r1, r3					
		BGT		H02V00_loop
								              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP  
;/*******************************************************************************/
;/* ARMV6_Interpolate_H00V01 
;/*	0 horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use vertical filter (-1,12,6,-1) 
;/******************************************************************************/
;#pragma optimize( "", off)
;void   ARMV6_Interpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
;	/*we process along columns instead of rows so that loads can be minimised*/
;	/*Decrementing loops are used for the purpose of optimization			 */
;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -= 4)
;		{
;			lTemp0 = pSrc[-(I32)(uSrcPitch)];
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[uSrcPitch];
;			lTemp3 = pSrc[uSrcPitch<<1];
;
;			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = pSrc[3*uSrcPitch];
;			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = pSrc[uSrcPitch << 2];
;			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = pSrc[5*uSrcPitch];
;			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4;
;			*pDst = ClampVal(lTemp3);
;			pDst += uDstPitch;
;
;			pSrc += (uSrcPitch << 2);
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		pSrc -= ((uSrcPitch * 8) - 1);
;	}
;}
	EXPORT ARMV6_Interpolate_H00V01 
ARMV6_Interpolate_H00V01  PROC
		STMFD    sp!,{r4-r11,lr}
		MOV      r14, #8						
		SUB		 sp,sp,#8
		ANDS	 r4,r0,#3		     	  	
		STR	     r3,[sp,#0]	
		orr	     r12, r14, r14, lsl #16		
		BNE		 unalign_H00V01		
align_H00V01
align_H00V01_loop
		;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r7,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]	;lTemp3 d = 0, 1, 2, 3
        LDR	r6,[r0],#4			;lTemp1 b = 0, 1, 2, 3         
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		    ;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    ;0, 2	2*b
		mov         r10, r10, lsl #1    ;0, 2	2*b
		;lsl		r11, r11, #1	    ;1, 3	2*b
		mov         r11, r11, lsl #1    ;1, 3	2*b			
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		    r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		    r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		    ;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		    ;1, 3	6*(2*b + c)
		mov		    r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		    r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		    ;0, 2	8 - (a+d)
		usub16		r3, r12, r3		    ;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r7,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]	;lTemp3 d = 0, 1, 2, 3
        LDR	r6,[r0],#-4			;lTemp1 b = 0, 1, 2, 3 
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		    r10, r10, lsl #1	    	;0, 2	2*b
		mov		    r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		    r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		    r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		    r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		    r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	    r14,r14,#1
		add	        r0, r0, r2
		add	        r1, r1, r3		
		BGT		align_H00V01_loop						
		B		end_H00V01
unalign_H00V01			
		SUB		r0,r0,r4
		sub		r0, r0, r2
		MOV		r4,r4,LSL #3		;i = i<<3;
unalign_H00V01_loop
		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
		tst		r10, #0x08000
		mov		r10, r10, asr #4
		orrne	r10, r10, #0x0000f000
		andeq	r10, r10, #0xffff0fff
		tst		r11, #0x08000			
		;asr		r10, r10, #4
		;asr		r11, r11, #4
		
		mov		r11, r11, asr #4				
		orrne	r11, r11, #0x0000f000					
		andeq	r11, r11, #0xffff0fff			
		usat16	 r10,#8,r10
		usat16	 r11,#8,r11	
		orr	r10,r10,r11,lsl #8
		
		str	r10, [r1] 

		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		BGT		unalign_H00V01_loop
end_H00V01				                                   
	
		ADD		 sp,sp,#8								              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP  


;/****************************************************************************/
;/* ARMV6_Interpolate_H01V01 
;/*	1/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/*	Use vertical filter (-1,12,6,-1) 
;/***************************************************************************/
;void   ARMV6_Interpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. First do horizantal interpolation   */
;	/*followed by vertical interpolation. Decrementing loops are used for the*/
;	/*purpose of ARM optimization											 */
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
;			*pDst = ClampVal(lTemp3);
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;
;}
;huwei 20090819 stack_bug
	EXPORT ARMV6_Interpolate_H01V01 
ARMV6_Interpolate_H01V01  PROC
	   STMFD    sp!,{r4-r11,lr}
       ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
       sub       sp,sp,#180	;(8+3)*8*2 + 4 = 180
       str       r3,[sp,#0]
       add       r3,sp,#4	
;Horizontal			
		MOV	r14, #11
		sub	r0, r0, r2
		MOV	r12, #6	
H01V01_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r9, r9, #0		;0 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #0		;0 - (a+d)		
;	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
;	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d
	PKHBT	r10, r9, r10, lsl #16	
	add		r9,r8,r7, lsl #1	;2b+c	
   	rsb		 r6, r6, #0		;8 - (a+d)
	add		 r11,r7,r5		;a+d 
        STR      r10,[r3], #4 	  		   
	smlabb	 r10, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #0		;8 - (a+d)   		
;	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
;	orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
;	usat	 r11,#8,r11,ASR #4
			
;five six
	add		 r9,r8,r6		;a+d   
	add		 r8,r5,r4, lsl #1	;2b+c
	PKHBT	r10, r10, r11, lsl #16			
   	rsb		 r9, r9, #0		;8 - (a+d)
        STR      r10,[r3], #4  	
	add		 r10,r4,r7		;a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r10, r10, #0		;8 - (a+d)		
;	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
;	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d
	PKHBT	r10, r9, r10, lsl #16			 	
	add		r9,r7,r6, lsl #1	;2b+c	
   	rsb		 r5, r5, #0		;8 - (a+d)
        STR      r10,[r3], #4    	
	add		 r11,r6,r4		;a+d   		   
	smlabb	 r10, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	;2b+c	
   	rsb		 r11, r11, #0		;8 - (a+d)	
;	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
;	orr	r10,r10,r5,LSL #16	
;	usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
	PKHBT	r10, r10, r11, lsl #16		
		add	r0, r0, r2
;		add	sp, sp, #16			
        STR      r10,[r3], #4			
;		add	r1, r1, r3					
		BGT		H01V01_loop				
;Vertical
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        ldr       r3,[sp,#0]
        add       r4,sp,#4
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		MOV	r14, #8
		orr	r12, r12, r12, lsl #16	

H01V01_Vloop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r4], #16			;lTemp0 a = 0, 1
        LDR	r6,[r4], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r4], #16			;lTemp2 c = 0, 1  
        LDR	r8,[r4], #16			;lTemp3 d = 0, 1       
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl  #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r8,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;5
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r4], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3										

        ;SUB      sp,sp,#172	;(8+3)*8*2 = 176 - 4
         SUB      r4,r4,#172	;(8+3)*8*2 = 176 - 4
        				
		SUBS	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		BGT		H01V01_Vloop						
	
        ;add      sp,sp,#160	;(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180
        LDMFD    sp!,{r4-r11,pc}                     
	ENDP  
	            

;/************************************************************************/
;/* ARMV6_Interpolate_H02V01 
;/*	2/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (-1,6,12,-1)
;/*	Use vertical filter (-1,12,6,-1) 
;/************************************************************************/
;void   ARMV6_Interpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
;			*pDst = ClampVal(lTemp3);
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}	
;}
;huwei 20090819 stack_bug
	EXPORT ARMV6_Interpolate_H02V01 
ARMV6_Interpolate_H02V01  PROC
     STMFD    sp!,{r4-r11,lr}
     ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
     sub      sp,sp,#180	;(8+3)*8*2 + 4 = 180
     str      r3,[sp,#0]
     add      r3,sp,#4	
;Horizontal			
     MOV	r14, #11
     sub	r0, r0, r2
     MOV	r12, #6	
H02V01_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
     LDRB     r4,[r0,#-1]		;lTemp0
     LDRB     r5,[r0,#0]		;lTemp1   
     LDRB     r6,[r0,#1]		;lTemp2            
     LDRB     r7,[r0,#2]		;lTemp3    
     LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r9, r9, #0		;0 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	;2b+c	
   	rsb		 r10, r10, #0		;0 - (a+d)		
;	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
    LDRB     r4,[r0,#4]		;lTemp5
    LDRB     r5,[r0,#5]		;lTemp6	
;	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d
	PKHBT	r10, r9, r10, lsl #16	
	add		r9,r7,r8, lsl #1	;2b+c	
   	rsb		 r6, r6, #0		;8 - (a+d)
	add		 r11,r7,r5		;a+d 
    STR      r10,[r3], #4 	  		   
	smlabb	 r10, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	;2b+c	
   	rsb		 r11, r11, #0		;8 - (a+d)   		
;	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
;	orr	r10,r10,r6,LSL #16	
    LDRB     r6,[r0,#6]		;lTemp5
    LDRB     r7,[r0,#7]		;lTemp6		
;	usat	 r11,#8,r11,ASR #4
			
;five six
	add		 r9,r8,r6		;a+d   
	add		 r8,r4,r5, lsl #1	;2b+c
	PKHBT	r10, r10, r11, lsl #16			
   	rsb		 r9, r9, #0		;8 - (a+d)
    STR      r10,[r3], #4  	
	add		 r10,r4,r7		;a+d    		   
	smlabb	 r9, r8, r12,r9
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #0		;8 - (a+d)		
;	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
    LDRB     r8,[r0,#8]		;lTemp7
    LDRB     r4,[r0,#9]		;lTemp8	
;	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d
	PKHBT	r10, r9, r10, lsl #16			 	
	add		r9,r6,r7, lsl #1	;2b+c	
   	rsb		 r5, r5, #0		;8 - (a+d)
    STR      r10,[r3], #4    	
	add		 r11,r6,r4		;a+d   		   
	smlabb	 r10, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #0		;8 - (a+d)	
;	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
;	orr	r10,r10,r5,LSL #16	
;	usat	 r11,#8,r11,ASR #4				
	SUBS	 r14,r14,#1
	PKHBT	 r10, r10, r11, lsl #16		
	add	     r0, r0, r2			
    STR      r10,[r3], #4			
;	add	r1, r1, r3					
	BGT		H02V01_loop				
;Vertical
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp, #4
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		mov	r14, #8
		orr	r12, r12, r12, lsl #16	

H02V01_Vloop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r11], #16			;lTemp0 a = 0, 1
        LDR	r6,[r11], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r11], #16			;lTemp2 c = 0, 1  
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1       
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;5
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8		
		orr	r9,r9,r10,lsl #8
		strh	r9, [r1], r3										

        ;SUB      sp,sp,#172	;(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	;(8+3)*8*2 = 176 - 4
        				
		SUBS	r14,r14,#2
		sub	    r1, r1, r3, lsl #3
		add	    r1, r1, #2		
		BGT		H02V01_Vloop						
	
        ;add      sp,sp,#160	;(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 

;/******************************************************************************/
;/* ARMV6_Interpolate_H00V02 
;/*	0 horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use vertical filter (-1,6,12,-1) 
;/*****************************************************************************/
;#pragma optimize( "", off)
;void   ARMV6_Interpolate_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;
;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-(I32)(uSrcPitch)];
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[uSrcPitch];
;			lTemp3 = pSrc[uSrcPitch<<1];
;
;			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = pSrc[3*uSrcPitch];
;			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = pSrc[4*uSrcPitch];
;			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = pSrc[5*uSrcPitch];
;			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4;
;			*pDst = ClampVal(lTemp3);
;			pDst += uDstPitch;
;
;			pSrc += (uSrcPitch << 2);
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		pSrc -= ((uSrcPitch * 8) - 1);
;	}
;}
	EXPORT ARMV6_Interpolate_H00V02 
ARMV6_Interpolate_H00V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #8						
		SUB		 sp,sp,#8
		ANDS	r4,r0,#3		     	  	
		STR	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		BNE		unalign_H00V02		
align_H00V02
align_H00V02_loop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r6,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r7,[r0],#4			;lTemp1 b = 0, 1, 2, 3         
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b					
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r6,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r7,[r0],#-4			;lTemp1 b = 0, 1, 2, 3 
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b	
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b			
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		BGT		align_H00V02_loop						
		B		end_H00V02
unalign_H00V02			
		SUB		r0,r0,r4
		sub		r0, r0, r2
		MOV		r4,r4,LSL #3		;i = i<<3;
unalign_H00V02_loop
		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b	
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b			
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)		
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		BGT		unalign_H00V02_loop
end_H00V02				                                   
	
		ADD		 sp,sp,#8							              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP  

;/*******************************************************************/
;/* ARMV6_Interpolate_H01V02 
;/*	1/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/*	Use vertical filter (-1,6,12,-1) 
;/********************************************************************/
;void   ARMV6_Interpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8;
;			*pDst = ClampVal(lTemp3);
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;}
	EXPORT ARMV6_Interpolate_H01V02 
ARMV6_Interpolate_H01V02  PROC
    STMFD  sp!,{r4-r11,lr}
    SUB    sp,sp,#0x4c0
	MOV		 r4,sp
	SUB		 r0,r0,r2         ;p = pSrc - (I32)(uSrcPitch);
	SUB		 r2,r2,#8         ;uSrcPitch - 8
	MOV    r6,#8			
	RSB		 r12,r6,#16       ;r12 = 16 - 8
	MOV		 r12,r12,LSL #2   ;r12 = 8 * 4
	MOV		 lr,#6            ;lr = 6
	ADD    r5,r6,#3         ;r5 = 8 + 3
    ;B     Ln2180
Ln2020
    MOV    r6,#8            ;r6 = 8
    ;B     Ln2160
Ln2028
    LDRB     r9,[r0,#1]		;lTemp2
    LDRB     r8,[r0,#0]		;lTemp1        
    LDRB     r10,[r0,#2]	;lTemp3
    LDRB     r7,[r0,#-1]	;lTemp0
		
	ADD		 r11,r9,r8,LSL #1
	SMULBB r11,lr,r11
	ADD		 r7,r7,r10
	SUB		 r7,r11,r7
	STR		 r7,[r4],#4

	LDRB	 r7,[r0,#3]		
	ADD		 r11,r10,r9,LSL #1
	SMULBB r11,lr,r11
	ADD		 r8,r8,r7
	SUB		 r8,r11,r8
	STR		 r8,[r4],#4

	LDRB	 r8,[r0,#4]				
	ADD		 r11,r7,r10,LSL #1
	SMULBB r11,lr,r11
	ADD		 r9,r9,r8
	SUB		 r9,r11,r9
	STR		 r9,[r4],#4

	LDRB	 r9,[r0,#5]				
	ADD		 r11,r8,r7,LSL #1
	SMULBB r11,lr,r11
	ADD		 r10,r10,r9
	SUB		 r10,r11,r10
	STR		 r10,[r4],#4

	SUBS	 r6,r6,#4  ;8 - 4	
	ADD		 r0,r0,#4  ;p + 4             
Ln2160
    BGT    Ln2028
	ADD		 r4,r4,r12 ;b += (16 - 8)
	ADD		 r0,r0,r2  ;p += (uSrcPitch - 8)		
    SUBS   r5,r5,#1  ;dstRow--
Ln2180
    BGT      Ln2020

    MOV    r2,#8
	MOV	   r2,r2,LSL #4 
	SUB	   r2,r2,#1  ;r2 = (8 << 4) - 1
	MOV    r4,r3,LSL #3
	SUB    r4,r4,#1  ;r4 = (uDstPitch * 8) - 1	
	MOV	   r0,sp
	MOV	   r12,#0x80 ;128
	MOV	   r5,#8     ;8
    ;B     Ln2428
Ln2220
    MOV    r6,#8		 ;8
    ;B     Ln2408
Ln2228
    LDR      r9,[r0,#0x80]		;lTemp2
    LDR      r8,[r0,#0x40]		;lTemp1        
    LDR      r10,[r0,#0xc0]		;lTemp3
    LDR      r7,[r0,#0]			  ;lTemp0
		
	ADD		 r11,r8,r9,LSL #1
	SMLABB r11,lr,r11,r12
	ADD		 r7,r7,r10
	SUB		 r7,r11,r7
	USAT	 r7,#8,r7,ASR #8
	
	ADD		 r11,r9,r10,LSL #1
	STRB	 r7,[r1]
	ADD    r1,r1,r3          ;pDst += uDstPitch
	LDR		 r7,[r0,#0x100]
	SMLABB r11,lr,r11,r12
	SUB		 r8,r11,r8
	SUB		 r8,r8,r7
	USAT	 r8,#8,r8,ASR #8

	ADD		 r11,r10,r7,LSL #1		
	STRB	 r8,[r1]
	ADD    r1,r1,r3         ;pDst += uDstPitch
	LDR		 r8,[r0,#0x140]
	SMLABB r11,lr,r11,r12
	SUB		 r9,r11,r9
	SUB		 r9,r9,r8
	USAT	 r9,#8,r9,ASR #8

	ADD		 r11,r7,r8,LSL #1		
	STRB	 r9,[r1]
	ADD    r1,r1,r3         ;pDst += uDstPitch
	LDR		 r9,[r0,#0x180]
	SMLABB r11,lr,r11,r12
	SUB		 r10,r11,r10
	SUB		 r10,r10,r9
	USAT	 r10,#8,r10,ASR #8

	SUBS	 r6,r6,#4	
	ADD		 r0,r0,#0x100   ; b += 64
    STRB   r10,[r1]
    ADD    r1,r1,r3       ;pDst += uDstPitch
Ln2408
    BGT    Ln2228
	SUB		 r0,r0,r2,LSL #2 ;b -= ((8 << 4) - 1)
    SUB		 r1,r1,r4        ;pDst -= ((uDstPitch*8) - 1);       
	SUBS   r5,r5,#1       
Ln2428
    BGT      Ln2220
    ADD      sp,sp,#0x4c0
    LDMFD    sp!,{r4-r11,pc}                
	ENDP  
	

;/****************************************************************/
;/* ARMV6_Interpolate_H02V02 
;/*	2/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (6,9,1) 
;/*	Use vertical filter (6,9,1) 
;/****************************************************************/
;void   ARMV6_Interpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*18];
;	I32 *b;	
;	I32   lTemp0, lTemp1, lTemp2;    
;
;	b = buff;
;	for (dstRow = 8+2; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = pSrc[0];
;			lTemp1 = pSrc[1];
;			lTemp2 = pSrc[2];
;
;			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);
;			b++;
;
;			lTemp0 = pSrc[3];
;			*b = (6*lTemp1 + 9*lTemp2 + lTemp0);
;			b++;
;
;			lTemp1 = pSrc[4];
;			*b = (6*lTemp2 + 9*lTemp0 + lTemp1);
;			b++;
;
;			lTemp2 = pSrc[5];
;			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);			
;			b++;
;
;			pSrc +=4;			
;		}
;		b += (16 - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;
;			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			lTemp0 = b[48];
;			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8;
;			*pDst = ClampVal(lTemp1);
;			pDst += uDstPitch;
;
;			lTemp1 = b[64];
;			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8;
;			*pDst = ClampVal(lTemp2);
;			pDst += uDstPitch;
;
;			lTemp2 = b[80];
;			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
;			*pDst = ClampVal(lTemp0);
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;}
LAB_0X00090006 dcd 0x00090006

;huwei 20090819 stack_bug
	EXPORT ARMV6_Interpolate_H02V02 
ARMV6_Interpolate_H02V02  PROC
		STMFD    sp!,{r4-r11,lr}
        ;SUB      sp,sp,#160	;(8+2)*8*2 = 160
        sub      sp,sp,#164	;(8+2)*8*2 + 4 = 164
        str      r3,[sp,#0]
        add      r3,sp,#4	
;Horizontal			
		MOV	r14, #10	
H02V02_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		

        LDRB     r5,[r0,#0]		;lTemp0   
        LDRB     r6,[r0,#1]		;lTemp1            
        LDRB     r7,[r0,#2]		;lTemp2    
        LDRB     r8,[r0,#3]		;lTemp3

;input  r4~r8
;one ;tow	
		ADD		 r9,r5,r5,LSL #1
		ADD		 r10,r6,r6,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r7		
		
		ADD		 r11,r6,r6,LSL #1
		ADD		 r12,r7,r7,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r8
		
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r5,[r0,#4]		;lTemp4
        LDRB     r6,[r0,#5]		;lTemp5			
;three; four      	
		ADD		 r9,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r5		
		
		ADD		 r11,r8,r8,LSL #1
		ADD		 r12,r5,r5,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r6
		
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r7,[r0,#6]		;lTemp4
        LDRB     r8,[r0,#7]		;lTemp5			
;five six
		ADD		 r9,r5,r5,LSL #1
		ADD		 r10,r6,r6,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r7		
		
		ADD		 r11,r6,r6,LSL #1
		ADD		 r12,r7,r7,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r8
		
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r5,[r0,#8]		;lTemp4
        LDRB     r6,[r0,#9]		;lTemp5	
;seven; eight  
		ADD		 r9,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r5		
		
		ADD		 r11,r8,r8,LSL #1
		ADD		 r12,r5,r5,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r6
		
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
    				
		SUBS	r14,r14,#1	
		add	r0, r0, r2					
		BGT		H02V02_loop				
;Vertical
        ;SUB      sp,sp,#160	;(8+2)*8*2 = 160
        add     r2,sp,#4
        ldr     r3,[sp,#0]
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		MOV	r14, #8
		ldr	r4, LAB_0X00090006	

H02V02_Vloop
;r4~r12		;(6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8
        LDR	r5,[r2], #16			;lTemp0 a = 0, 1
        LDR	r6,[r2], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r2], #16			;lTemp2 c = 0, 1        
;one	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;two		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;three		
		PKHBT	r9, r7, r5, lsl #16
		PKHTB	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		;lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;four	
;one2	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;two2		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;three2		
		PKHBT	r9, r7, r5, lsl #16
		PKHTB	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		;lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
		
;one3	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r2], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3
;two3		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
		orr	r8,r8,r10,lsl #8
		strh	r8, [r1], r3		

        ;SUB      sp,sp,#156	;(8+2)*8*2 = 160 - 4
        sub      r2,r2,#156	;(8+2)*8*2 = 160 - 4
        				
		SUBS	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		BGT		H02V02_Vloop						
	
        ;add      sp,sp,#144	;(8+2)*8*2 = 160 - 4*4
        add      sp,sp,#164
        LDMFD    sp!,{r4-r11,pc}                     
	ENDP  

;/******************************************************************/
;/* ARMV6_AddInterpolate_H00V00 
;/*	 0 horizontal displacement 
;/*	 0 vertical displacement 
;/*	 No interpolation required, simple block copy. 
;/**************************************************************** */
;void   ARMV6_AddInterpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	U32 a,b,c,d;
;	U32 q,w;
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		a=pSrc[0]|(pSrc[1]<<8)|((pSrc[2]|(pSrc[3]<<8))<<16);
;		b=pSrc[4]|(pSrc[5]<<8)|((pSrc[6]|(pSrc[7]<<8))<<16);
;		c=((U32 *)pDst)[0];
;		d=((U32 *)pDst)[1];
;		q=(a|c) & 0x01010101;
;		w=(b|d) & 0x01010101;
;		q+=(a>>1) & 0x7F7F7F7F;
;		w+=(b>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;		w+=(d>>1) & 0x7F7F7F7F;
;		((U32 *)pDst)[0]=q;
;		((U32 *)pDst)[1]=w;
;		pDst += uDstPitch;
;		pSrc += uSrcPitch;
;	}
;}

LAB_0X01010101 dcd 0x01010101
	EXPORT ARMV6_AddInterpolate_H00V00 
ARMV6_AddInterpolate_H00V00  PROC
        STMFD    sp!,{r4-r11,lr}
        	  
		ANDS	r4,r0,#3
		ldr	r12, LAB_0X01010101		
		MOV	r14, #8			
		BNE		unalign_AddH00V00		
align_AddH00V00
align_AddH00V00_loop	
		LDR		r5,[r0,#4]      ;pTempSrc[1]
		LDR		r4,[r0], r2      ;pTempSrc[0]
		LDR		r7,[r0,#4]      ;pTempSrc[1]
		LDR		r6,[r0], r2      ;pTempSrc[0]		
		LDRD		r8,[r1]      	 ;pTempDst[0]
		LDRD		r10,[r1,r3]      ;pTempDst[0]
							
		uqadd8  r4, r4, r12
		uqadd8  r5, r5, r12
		uqadd8  r6, r6, r12
		uqadd8  r7, r7, r12
				  
		uhadd8	r4, r4, r8
		uhadd8	r5, r5, r9
		uhadd8	r6, r6, r10
		uhadd8	r7, r7, r11	
						
		SUBS	r14,r14,#2
		STRD		r4,[r1],r3      ;pTempDst[0]
		STRD		r6,[r1],r3      ;pTempDst[0]					
		BGT		align_AddH00V00_loop						
		B		end_AddH00V00
unalign_AddH00V00			
		SUB		r0,r0,r4
		MOV		r4,r4,LSL #3		;i = i<<3;
		RSB		r5,r4,#0x20			;32 - i
unalign_AddH00V00_loop
		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r8,[r0,#8]		;pTempSrc[2]
		LDR		r6,[r0], r2		;pTempSrc[0]
		
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r9,[r0,#8]		;pTempSrc[2]
		LDR		r10,[r0], r2		;pTempSrc[0]
								
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5
		MOV		r7,r7,LSR r4
		MOV		r11,r11,LSR r4
		ORR		r7,r7,r8,LSL r5 
		ORR		r11,r11,r9,LSL r5 
		
		
		LDRD		r8,[r1]      	 ;pTempDst[0]					
		uqadd8  r6, r6, r12
		uqadd8  r7, r7, r12
		uhadd8	r6, r6, r8
		uhadd8	r7, r7, r9
		STRD		r6,[r1],r3
				
		LDRD		r8,[r1]      ;pTempDst[0]				
		uqadd8  r10, r10, r12
		uqadd8  r11, r11, r12
		uhadd8	r10, r10, r8
		uhadd8	r11, r11, r9		
		SUBS	r14,r14,#2

		STRD		r10,[r1],r3					
		BGT		unalign_AddH00V00_loop
end_AddH00V00				
	
        LDMFD    sp!,{r4-r11,pc}
	ENDP 	
;/*******************************************************************/
;/* ARMV6_AddInterpolate_H01V00 
;/*	1/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/********************************************************************/
;void   ARMV6_AddInterpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	U32 lTemp;
;	U32 c,q;
;	I32 dstRow, dstCol;        
;	I32 lTemp0, lTemp1;
;	I32 lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
;	/*temporary variables lTempX, so that number of loads can be minimized. */
;	/*Decrementing loops are used for the purpose of optimization			*/
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-1]; 
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[1];
;			lTemp3 = pSrc[2];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4;
;			lTemp  = ClampVal(lTemp0);
;
;			lTemp0 = pSrc[3];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp1)) << 8;
;
;			lTemp1 = pSrc[4];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp2)) << 16;
;
;			lTemp2 = pSrc[5];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp3)) << 24;
;
;			c = ((U32 *)pDst)[0];
;			q = (lTemp|c) & 0x01010101;
;			q += (lTemp>>1) & 0x7F7F7F7F;
;			q += (c>>1) & 0x7F7F7F7F;
;
;			*((PU32)pDst)++ = q;
;			pSrc += 4;
;		}
;		pDst += (uDstPitch - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;}
	EXPORT ARMV6_AddInterpolate_H01V00 
ARMV6_AddInterpolate_H01V00  PROC
	STMFD    sp!,{r4-r11,lr}	     	  	
		MOV	r14, #8
		SUB		 sp,sp,#8		
		MOV	r12, #6	
		STR	     r2,[sp,#0]		
		ldr	r2, LAB_0X01010101		
AddH01V00_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r9, r9, #8		;8 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r7,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r8,r7, lsl #1	;2b+c	
   	rsb		 r6, r6, #8		;8 - (a+d)
	add		 r11,r7,r5		;a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r4,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
	usat	 r11,#8,r11,ASR #4	
;five six
	add		 r9,r8,r6		;a+d  
	orr	r10,r10,r11,LSL #24
	ldr	r11, [r1]	 
	add		 r8,r5,r4, lsl #1	;2b+c	
		uqadd8  r11, r11, r2
   	rsb		 r9, r9, #8		;8 - (a+d)				 
		uhadd8	r10, r11, r10  
	smlabb	 r9, r8, r12,r9		 	
        STR      r10,[r1]   	
	add		 r10,r4,r7		;a+d    		   
	add		 r4,r6,r5, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r7,r6, lsl #1	;2b+c	
   	rsb		 r5, r5, #8		;8 - (a+d)
	add		 r11,r6,r4		;a+d
	ldr	r4, [r1,#4]	   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r8,r7, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,LSL #16	
	usat	 r11,#8,r11,ASR #4
		ldr	     r5,[sp,#0]	
		uqadd8  r4, r4, r2						
	orr	r10,r10,r11,LSL #24		
		SUBS	r14,r14,#1	
		uhadd8	r10, r4, r10 	
		add	r0, r0, r5 									
        STR      r10,[r1,#4]		       	
		add	r1, r1, r3					
		BGT		AddH01V00_loop
				
		ADD		 sp,sp,#8								              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 
;/******************************************************************/
;/* ARMV6_AddInterpolate_H02V00 
;/*	2/3 pel horizontal displacement
;/*	0 vertical displacement 
;/*	Use horizontal filter (-1,6,12,-1) 
;/******************************************************************/
;void   ARMV6_AddInterpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	U32 lTemp;
;	U32 c,q;
;	I32 dstRow, dstCol;
;	I32 lTemp0, lTemp1;
;	I32 lTemp2, lTemp3;
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-1]; 
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[1];
;			lTemp3 = pSrc[2];
;
;			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4;
;			lTemp  = ClampVal(lTemp0);
;
;			lTemp0 = pSrc[3];
;			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp1)) << 8;
;
;			lTemp1 = pSrc[4];
;			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp2)) << 16;
;
;			lTemp2 = pSrc[5];
;			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4;
;			lTemp  |= (ClampVal(lTemp3)) << 24;
;
;			c=((U32 *)pDst)[0];
;			q=(lTemp|c) & 0x01010101;
;			q+=(lTemp>>1) & 0x7F7F7F7F;
;			q+=(c>>1) & 0x7F7F7F7F;
;
;			*((PU32)pDst)++ = q;
;			pSrc += 4;		
;		}
;		pDst += (uDstPitch - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;}

	EXPORT ARMV6_AddInterpolate_H02V00 
ARMV6_AddInterpolate_H02V00  PROC
	STMFD    sp!,{r4-r11,lr}	     	  	
		MOV	r14, #8
		SUB		 sp,sp,#8		
		MOV	r12, #6	
		STR	     r2,[sp,#0]		
		ldr	r2, LAB_0X01010101		
AddH02V00_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
	add		 r9,r4,r7		;a+d   
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r9, r9, #8		;8 - (a+d)
	add		 r10,r5,r8		;a+d    		   
	smlabb	 r9, r4, r12,r9
	add		 r5,r6,r7, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
	usat	 r10,#8,r10,ASR #4		
;three; four      	
	add	r6,r6,r4		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r7,r8, lsl #1	;2b+c	
   	rsb		 r6, r6, #8		;8 - (a+d)
	add		 r11,r7,r5		;a+d   		   
	smlabb	 r6, r9, r12,r6   
	add		 r7,r8,r4, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r6,#8,r6,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
	usat	 r11,#8,r11,ASR #4	
;five six
	add		 r9,r8,r6		;a+d  
	orr	r10,r10,r11,LSL #24
	ldr	r11, [r1]	 
	add		 r8,r4,r5, lsl #1	;2b+c	
		uqadd8  r11, r11, r2
   	rsb		 r9, r9, #8		;8 - (a+d)				 
		uhadd8	r10, r11, r10  
	smlabb	 r9, r8, r12,r9		 	
        STR      r10,[r1]   	
	add		 r10,r4,r7		;a+d    		   
	add		 r4,r5,r6, lsl #1	;2b+c	
   	rsb		 r10, r10, #8		;8 - (a+d)		
	usat	 r9,#8,r9,ASR #4
	smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
	usat	 r10,#8,r10,ASR #4
;seven; eight      	
	add	r5,r5,r8		;a+d		 
	orr	r10,r9,r10,LSL #8 	
	add		r9,r6,r7, lsl #1	;2b+c	
   	rsb		 r5, r5, #8		;8 - (a+d)
	add		 r11,r6,r4		;a+d
	ldr	r4, [r1,#4]	   		   
	smlabb	 r5, r9, r12,r5   
	add		 r7,r7,r8, lsl #1	;2b+c	
   	rsb		 r11, r11, #8		;8 - (a+d)	
	usat	 r5,#8,r5,ASR #4   
	smlabb	 r11, r7, r12,r11
	orr	r10,r10,r5,LSL #16	
	usat	 r11,#8,r11,ASR #4
		ldr	     r5,[sp,#0]	
		uqadd8  r4, r4, r2						
	orr	r10,r10,r11,LSL #24		
		SUBS	r14,r14,#1	
		uhadd8	r10, r4, r10 	
		add	r0, r0, r5 									
        STR      r10,[r1,#4]		       	
		add	r1, r1, r3					
		BGT		AddH02V00_loop
				
		ADD		 sp,sp,#8								              	 		
						              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP
;/*******************************************************************************/
;/* ARMV6_AddInterpolate_H00V01 
;/*	0 horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use vertical filter (-1,12,6,-1) 
;/******************************************************************************/
;#pragma optimize( "", off)
;void   ARMV6_AddInterpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
;	/*we process along columns instead of rows so that loads can be minimised*/
;	/*Decrementing loops are used for the purpose of optimization			 */
;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -= 4)
;		{
;			lTemp0 = pSrc[-(I32)(uSrcPitch)];
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[uSrcPitch];
;			lTemp3 = pSrc[uSrcPitch<<1];
;
;			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = pSrc[3*uSrcPitch];
;			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = pSrc[uSrcPitch << 2];
;			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = pSrc[5*uSrcPitch];
;			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4;
;			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			pSrc += (uSrcPitch << 2);
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		pSrc -= ((uSrcPitch * 8) - 1);
;	}
;}

	EXPORT ARMV6_AddInterpolate_H00V01 
ARMV6_AddInterpolate_H00V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #8						
		SUB		 sp,sp,#8
		ANDS	r4,r0,#3		     	  	
		STR	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		BNE		unalign_AddH00V01		
align_AddH00V01
align_AddH00V01_loop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r7,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r6,[r0],#4			;lTemp1 b = 0, 1, 2, 3         
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b					
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r7,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r6,[r0],#-4			;lTemp1 b = 0, 1, 2, 3 
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b	
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b			
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		BGT		align_AddH00V01_loop						
		B		end_AddH00V01
unalign_AddH00V01			
		SUB		r0,r0,r4
		sub		r0, r0, r2
		MOV		r4,r4,LSL #3		;i = i<<3;
unalign_AddH00V01_loop
		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		BGT		unalign_AddH00V01_loop
end_AddH00V01				                                   
	
		ADD		 sp,sp,#8								              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP  
;/****************************************************************************/
;/* ARMV6_AddInterpolate_H01V01 
;/*	1/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/*	Use vertical filter (-1,12,6,-1) 
;/***************************************************************************/
;void   ARMV6_AddInterpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	/*MAP -- Process 4 pixels at a time. First do horizantal interpolation   */
;	/*followed by vertical interpolation. Decrementing loops are used for the*/
;	/*purpose of ARM optimization											 */
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
;			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;
;} 
;huwei 20090819 stack_bug
	EXPORT ARMV6_AddInterpolate_H01V01 
ARMV6_AddInterpolate_H01V01  PROC
		STMFD    sp!,{r4-r11,lr}
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        sub      sp,sp,#180	;(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0]
        add      r3,sp,#4	
;Horizontal			
		MOV	r14, #11
		sub	r0, r0, r2
		MOV	r12, #6	
AddH01V01_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
		add		 r9,r4,r7		;a+d   
		add		 r4,r6,r5, lsl #1	;2b+c	
		rsb		 r9, r9, #0		;0 - (a+d)
		add		 r10,r5,r8		;a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r7,r6, lsl #1	;2b+c	
		rsb		 r10, r10, #0		;0 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
;		usat	 r10,#8,r10,ASR #4		
;three; four      	
		add	r6,r6,r4		;a+d
		PKHBT	r10, r9, r10, lsl #16	
		add		r9,r8,r7, lsl #1	;2b+c	
   		rsb		 r6, r6, #0		;8 - (a+d)
		add		 r11,r7,r5		;a+d 
        STR      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r4,r8, lsl #1	;2b+c	
		rsb		 r11, r11, #0		;8 - (a+d)   		
;		usat	 r6,#8,r6,ASR #4   
		smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
;		usat	 r11,#8,r11,ASR #4
			
;five six
		add		 r9,r8,r6		;a+d   
		add		 r8,r5,r4, lsl #1	;2b+c
		PKHBT	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		;8 - (a+d)
        STR      r10,[r3], #4  	
		add		 r10,r4,r7		;a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r6,r5, lsl #1	;2b+c	
   		rsb		 r10, r10, #0		;8 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
;		usat	 r10,#8,r10,ASR #4
;seven; eight      	
		add	r5,r5,r8		;a+d
		PKHBT	r10, r9, r10, lsl #16			 	
		add		r9,r7,r6, lsl #1	;2b+c	
   		rsb		 r5, r5, #0		;8 - (a+d)
        STR      r10,[r3], #4    	
		add		 r11,r6,r4		;a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r8,r7, lsl #1	;2b+c	
   		rsb		 r11, r11, #0		;8 - (a+d)	
;		usat	 r5,#8,r5,ASR #4   
		smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r5,LSL #16	
;		usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
		PKHBT	r10, r10, r11, lsl #16		
		add	r0, r0, r2
;		add	sp, sp, #16			
        STR      r10,[r3], #4			
;		add	r1, r1, r3					
		BGT		AddH01V01_loop				
;Vertical
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp,#4
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		MOV	r14, #8
		ldr	r4, LAB_0X01010101
		orr	r12, r12, r12, lsl #16		

AddH01V01_Vloop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r11], #16			;lTemp0 a = 0, 1
        LDR	r6,[r11], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r11], #16			;lTemp2 c = 0, 1  
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1       
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;5
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        ;SUB      sp,sp,#172	;(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	;(8+3)*8*2 = 176 - 4
        				
		SUBS	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		BGT		AddH01V01_Vloop						
	
        ;add      sp,sp,#160	;(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        LDMFD    sp!,{r4-r11,pc}                     
	ENDP  	
;/************************************************************************/
;/* ARMV6_AddInterpolate_H02V01 
;/*	2/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (-1,6,12,-1)
;/*	Use vertical filter (-1,12,6,-1) 
;/************************************************************************/
;void   ARMV6_AddInterpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
;			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}	
;}
;huwei 20090819 stack_bug
	EXPORT ARMV6_AddInterpolate_H02V01 
ARMV6_AddInterpolate_H02V01  PROC
		STMFD    sp!,{r4-r11,lr}
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        sub      sp,sp,#180	;(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0]
        add      r3,sp,#4        	
;Horizontal			
		MOV	r14, #11
		sub	r0, r0, r2
		MOV	r12, #6	
AddH02V01_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
		add		 r9,r4,r7		;a+d   
		add		 r4,r5,r6, lsl #1	;2b+c	
		rsb		 r9, r9, #0		;0 - (a+d)
		add		 r10,r5,r8		;a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r6,r7, lsl #1	;2b+c	
		rsb		 r10, r10, #0		;0 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
;		usat	 r10,#8,r10,ASR #4		
;three; four      	
		add	r6,r6,r4		;a+d
		PKHBT	r10, r9, r10, lsl #16	
		add		r9,r7,r8, lsl #1	;2b+c	
		rsb		 r6, r6, #0		;8 - (a+d)
		add		 r11,r7,r5		;a+d 
        STR      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r8,r4, lsl #1	;2b+c	
   		rsb		 r11, r11, #0		;8 - (a+d)   		
;	usat	 r6,#8,r6,ASR #4   
			smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
;		usat	 r11,#8,r11,ASR #4
			
;five six
		add		 r9,r8,r6		;a+d   
		add		 r8,r4,r5, lsl #1	;2b+c
		PKHBT	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		;8 - (a+d)
        STR      r10,[r3], #4  	
		add		 r10,r4,r7		;a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r5,r6, lsl #1	;2b+c	
		rsb		 r10, r10, #0		;8 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
;		usat	 r10,#8,r10,ASR #4
;seven; eight      	
		add	r5,r5,r8		;a+d
		PKHBT	r10, r9, r10, lsl #16			 	
		add		r9,r6,r7, lsl #1	;2b+c	
		rsb		 r5, r5, #0		;8 - (a+d)
        STR      r10,[r3], #4    	
		add		 r11,r6,r4		;a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r7,r8, lsl #1	;2b+c	
   		rsb		 r11, r11, #0		;8 - (a+d)	
;		usat	 r5,#8,r5,ASR #4   
		smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r5,LSL #16	
;		usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
		PKHBT	r10, r10, r11, lsl #16		
		add	r0, r0, r2
;		add	sp, sp, #16			
        STR      r10,[r3], #4			
;		add	r1, r1, r3					
		BGT		AddH02V01_loop				
;Vertical
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        ldr       r3,[sp,#0]
        add       r11,sp,#4
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		MOV	r14, #8
		ldr	r4, LAB_0X01010101
		orr	r12, r12, r12, lsl #16	

AddH02V01_Vloop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r11], #16			;lTemp0 a = 0, 1
        LDR	r6,[r11], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r11], #16			;lTemp2 c = 0, 1  
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1       
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;5
;one	
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        ;SUB      sp,sp,#172	;(8+3)*8*2 = 176 - 4
        sub      r11,r11,#172	;(8+3)*8*2 = 176 - 4
        				
		SUBS	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		BGT		AddH02V01_Vloop						
	
        ;add      sp,sp,#160	;(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/******************************************************************************/
;/* ARMV6_AddInterpolate_H00V02 
;/*	0 horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use vertical filter (-1,6,12,-1) 
;/*****************************************************************************/
;#pragma optimize( "", off)ARMV6_AddInterpolate_H00V02
;void   (const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;
;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	for (dstRow = 8; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -= 4)
;		{
;			lTemp0 = pSrc[-(I32)(uSrcPitch)];
;			lTemp1 = pSrc[0];
;			lTemp2 = pSrc[uSrcPitch];
;			lTemp3 = pSrc[uSrcPitch<<1];
;
;			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = pSrc[3*uSrcPitch];
;			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = pSrc[4*uSrcPitch];
;			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = pSrc[5*uSrcPitch];
;			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4;
;			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			pSrc += (uSrcPitch << 2);
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		pSrc -= ((uSrcPitch * 8) - 1);
;	}
;}

LAB2_0X01010101 dcd 0x01010101
	EXPORT ARMV6_AddInterpolate_H00V02 
ARMV6_AddInterpolate_H00V02  PROC	
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #8						
		SUB		 sp,sp,#8
		ANDS	r4,r0,#3		     	  	
		STR	     r3,[sp,#0]	
		orr	r12, r14, r14, lsl #16		
		BNE		unalign_AddH00V02		
align_AddH00V02
align_AddH00V02_loop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r6,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r7,[r0],#4			;lTemp1 b = 0, 1, 2, 3         
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

        LDR	r5,[r0,-r2]			;lTemp0 a = 0, 1, 2, 3        
        LDR	r6,[r0,r2]			;lTemp2 c = 0, 1, 2, 3  
        LDR	r8,[r0,r2,LSL #1]		;lTemp3 d = 0, 1, 2, 3
        LDR	r7,[r0],#-4			;lTemp1 b = 0, 1, 2, 3 
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)	
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)		
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3		
		BGT		align_AddH00V02_loop						
		B		end_AddH00V02
unalign_AddH00V02			
		SUB		r0,r0,r4
		sub		r0, r0, r2
		MOV		r4,r4,LSL #3		;i = i<<3;
unalign_AddH00V02_loop
		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	add	r0, r0, #4 
;one	
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b			
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b	
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)				
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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

		RSB		r10,r4,#0x20			;32 - i
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r5,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r5,r5,LSR r4
	ORR		r5,r5,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r7,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r7,r7,LSR r4
	ORR		r7,r7,r11,LSL r10
	
        LDR	r9,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r6,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r6,r6,LSR r4
	ORR		r6,r6,r9,LSL r10
	
        LDR	r11,[r0,#4]			;lTemp0 a = 4, 5, 6, 7
        LDR	r8,[r0], r2			;lTemp0 a = 0, 1, 2, 3        
	MOV		r8,r8,LSR r4
	ORR		r8,r8,r11,LSL r10			
		          
	sub	r0, r0, r2, lsl #2
	sub	r0, r0, #4        
;tow
		uxtb16		r9, r5		    	;0, 2	a
		uxtb16		r3, r5, ror #8		;1, 3	a		
		uxtab16		r9, r9, r8		;0, 2	a+d
		uxtab16		r3, r3, r8, ror #8	;1, 3	a+d		
		uxtb16		r10, r6		    	;0, 2	b
		uxtb16		r11, r6, ror #8		;1, 3	b		
		;lsl		r10, r10, #1	    	;0, 2	2*b
		;lsl		r11, r11, #1	    	;1, 3	2*b	
		mov		r10, r10, lsl #1	    	;0, 2	2*b
		mov		r11, r11, lsl #1	    	;1, 3	2*b				
		uxtab16		r10, r10, r7		;0, 2	2*b + c
		uxtab16		r11, r11, r7, ror #8	;1, 3	2*b + c		
		add		r10, r10, r10, lsl #1  	;0, 2	3*(2*b + c)
		add		r11, r11, r11, lsl #1  	;1, 3	3*(2*b + c)		
		;lsl		r10, r10, #1		;0, 2	6*(2*b + c)
		;lsl		r11, r11, #1		;1, 3	6*(2*b + c)
		mov		r10, r10, lsl #1		;0, 2	6*(2*b + c)
		mov		r11, r11, lsl #1		;1, 3	6*(2*b + c)			
		usub16		r9, r12, r9		;0, 2	8 - (a+d)
		usub16		r3, r12, r3		;1, 3	8 - (a+d)				
		sadd16		r10, r10, r9		;0, 2	6*(2*b + c) +(8 - (a+d))
		sadd16		r11, r11, r3		;1, 3	6*(2*b + c) +(8 - (a+d))
		
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
		SUBS	r14,r14,#1
		add	r0, r0, r2
		add	r1, r1, r3					
		BGT		unalign_AddH00V02_loop
end_AddH00V02				                                   
	
		ADD		 sp,sp,#8							              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 

;/*******************************************************************/
;/* ARMV6_AddInterpolate_H01V02 
;/*	1/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (-1,12,6,-1) 
;/*	Use vertical filter (-1,6,12,-1) 
;/********************************************************************/
;void   ARMV6_AddInterpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*19];
;	I32 *b;
;	const U8 *p;
;	I32   lTemp0, lTemp1;
;	I32   lTemp2, lTemp3;
;
;	b = buff;
;	p = pSrc - (I32)(uSrcPitch);
;
;	for (dstRow = 8+3; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = p[-1];
;			lTemp1 = p[0];
;			lTemp2 = p[1];
;			lTemp3 = p[2];
;
;			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
;			b++;
;
;			lTemp0 = p[3];
;			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
;			b++;
;
;			lTemp1 = p[4];
;			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
;			b++;
;
;			lTemp2 = p[5];
;			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
;			b++;
;
;			p +=4;			
;		}
;		b += (16 - 8);
;		p += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;			lTemp3 = b[48];
;
;			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = b[64];
;			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = b[80];
;			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = b[96];
;			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8;
;			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;}
;huwei 20090819 stack_bug
	EXPORT ARMV6_AddInterpolate_H01V02 
ARMV6_AddInterpolate_H01V02  PROC
		STMFD    sp!,{r4-r11,lr}
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        sub      sp,sp,#180	;(8+3)*8*2 + 4 = 180
        str      r3,[sp,#0]
        add      r3,sp,#4	
;Horizontal			
		MOV	r14, #11
		sub	r0, r0, r2
		MOV	r12, #6	
AddH01V02_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#-1]		;lTemp0
        LDRB     r5,[r0,#0]		;lTemp1   
        LDRB     r6,[r0,#1]		;lTemp2            
        LDRB     r7,[r0,#2]		;lTemp3    
        LDRB     r8,[r0,#3]		;lTemp4

;input  r4~r8
;one ;tow   
		add		 r9,r4,r7		;a+d   
		add		 r4,r6,r5, lsl #1	;2b+c	
   		rsb		 r9, r9, #0		;0 - (a+d)
		add		 r10,r5,r8		;a+d    		   
		smlabb	 r9, r4, r12,r9
		add		 r5,r7,r6, lsl #1	;2b+c	
   		rsb		 r10, r10, #0		;0 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r5, r12,r10
        LDRB     r4,[r0,#4]		;lTemp5
        LDRB     r5,[r0,#5]		;lTemp6	
;		usat	 r10,#8,r10,ASR #4		
;three; four      	
		add	r6,r6,r4		;a+d
		PKHBT	r10, r9, r10, lsl #16	
		add		r9,r8,r7, lsl #1	;2b+c	
   		rsb		 r6, r6, #0		;8 - (a+d)
		add		 r11,r7,r5		;a+d 
        STR      r10,[r3], #4 	  		   
		smlabb	 r10, r9, r12,r6   
		add		 r7,r4,r8, lsl #1	;2b+c	
   		rsb		 r11, r11, #0		;8 - (a+d)   		
;		usat	 r6,#8,r6,ASR #4   
		smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r6,LSL #16	
        LDRB     r6,[r0,#6]		;lTemp5
        LDRB     r7,[r0,#7]		;lTemp6		
;		usat	 r11,#8,r11,ASR #4
			
;five six
		add		 r9,r8,r6		;a+d   
		add		 r8,r5,r4, lsl #1	;2b+c
		PKHBT	r10, r10, r11, lsl #16			
   		rsb		 r9, r9, #0		;8 - (a+d)
        STR      r10,[r3], #4  	
		add		 r10,r4,r7		;a+d    		   
		smlabb	 r9, r8, r12,r9
		add		 r4,r6,r5, lsl #1	;2b+c	
   		rsb		 r10, r10, #0		;8 - (a+d)		
;		usat	 r9,#8,r9,ASR #4
		smlabb	 r10, r4, r12,r10
        LDRB     r8,[r0,#8]		;lTemp7
        LDRB     r4,[r0,#9]		;lTemp8	
;		usat	 r10,#8,r10,ASR #4
;seven; eight      	
		add	r5,r5,r8		;a+d
		PKHBT	r10, r9, r10, lsl #16			 	
		add		r9,r7,r6, lsl #1	;2b+c	
   		rsb		 r5, r5, #0		;8 - (a+d)
        STR      r10,[r3], #4    	
		add		 r11,r6,r4		;a+d   		   
		smlabb	 r10, r9, r12,r5   
		add		 r7,r8,r7, lsl #1	;2b+c	
   		rsb		 r11, r11, #0		;8 - (a+d)	
;		usat	 r5,#8,r5,ASR #4   
		smlabb	 r11, r7, r12,r11
;		orr	r10,r10,r5,LSL #16	
;		usat	 r11,#8,r11,ASR #4				
		SUBS	r14,r14,#1
		PKHBT	r10, r10, r11, lsl #16		
		add	r0, r0, r2
;		add	sp, sp, #16			
        STR      r10,[r3], #4			
;		add	r1, r1, r3					
		BGT		AddH01V02_loop				
;Vertical
        ;SUB      sp,sp,#176	;(8+3)*8*2 = 176
        ldr     r3,[sp,#0]
        add     r11,sp,#4
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		MOV	r14, #8
		ldr	r4, LAB2_0X01010101
		orr	r12, r12, r12, lsl #16	

AddH01V02_Vloop
;r4~r12		; -a + 6*(2*b + c) -d
        LDR	r5,[r11], #16			;lTemp0 a = 0, 1
        LDR	r6,[r11], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[r11], #16			;lTemp2 c = 0, 1  
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1       
;one	
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8
		
		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r8,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;5
;one	
		sadd16		r0, r7, r7	    	;2*b
		sadd16		r0, r0, r6	    	;2*b + c
		sadd16		r2, r5, r8	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;two		
		sadd16		r0, r8, r8	    	;2*b
		sadd16		r0, r0, r7	    	;2*b + c
		sadd16		r2, r6, r5	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;three		
		sadd16		r0, r5, r5	    	;2*b
		sadd16		r0, r0, r8	    	;2*b + c
		sadd16		r2, r7, r6	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[r11], #16			;lTemp3 d = 0, 1		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3
;four		
		sadd16		r0, r6, r6	    	;2*b
		sadd16		r0, r0, r5	    	;2*b + c
		sadd16		r2, r8, r7	    	;a+d
		ssub16		r2, r12, r2	    	;128 -(a+d)
		sxth		r9, r0		    	;0; 2*b + c
		sxth		r10, r0, ror #16	;1; 2*b + c	
		add		r9, r9, r9, lsl #1  	;3*(2*b + c)
		add		r10, r10, r10, lsl #1  	;3*(2*b + c)		
		;lsl		r9, r9, #1		;6*(2*b + c)
		;lsl		r10, r10, #1		;6*(2*b + c)
		mov		r9, r9, lsl #1		;6*(2*b + c)
		mov		r10, r10, lsl #1		;6*(2*b + c)
		sxtah		r9, r9, r2		;0; 6*(2*b + c) +(8 - (a+d))
		sxtah		r10, r10, r2, ror #16	;1; 6*(2*b + c) +(8 - (a+d))			
		usat	 r9,#8,r9,ASR #8
		usat	 r10,#8,r10,ASR #8		
		orr	r9,r9,r10,lsl #8

		ldrh	r10, [r1]
		uqadd8  r9, r9, r4
		uhadd8	r9, r9, r10			
		strh	r9, [r1], r3										

        ;SUB      sp,sp,#172	;(8+3)*8*2 = 176 - 4
        sub     r11,r11,#172	;(8+3)*8*2 = 176 - 4
        				
		SUBS	r14,r14,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2		
		BGT		AddH01V02_Vloop						
	
        ;add      sp,sp,#160	;(8+3)*8*2 = 176 - 4*4
        add      sp,sp,#180
        LDMFD    sp!,{r4-r11,pc}                     
	ENDP  
;/****************************************************************/
;/* ARMV6_AddInterpolate_H02V02 
;/*	2/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (6,9,1) 
;/*	Use vertical filter (6,9,1) 
;/****************************************************************/
;void   ARMV6_AddInterpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;	I32 dstCol;    
;	I32 buff[16*18];
;	I32 *b;	
;	I32   lTemp0, lTemp1, lTemp2;    
;
;	b = buff;
;	for (dstRow = 8+2; dstRow > 0; dstRow--)
;	{
;		for (dstCol = 8; dstCol > 0; dstCol -=4)
;		{
;			lTemp0 = pSrc[0];
;			lTemp1 = pSrc[1];
;			lTemp2 = pSrc[2];
;
;			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);
;			b++;
;
;			lTemp0 = pSrc[3];
;			*b = (6*lTemp1 + 9*lTemp2 + lTemp0);
;			b++;
;
;			lTemp1 = pSrc[4];
;			*b = (6*lTemp2 + 9*lTemp0 + lTemp1);
;			b++;
;
;			lTemp2 = pSrc[5];
;			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);			
;			b++;
;
;			pSrc +=4;			
;		}
;		b += (16 - 8);
;		pSrc += (uSrcPitch - 8);
;	}
;
;	b = buff;
;	for (dstCol = 8; dstCol > 0; dstCol--)
;	{
;		for (dstRow = 8; dstRow > 0; dstRow -=4)
;		{
;			lTemp0 = b[0];
;			lTemp1 = b[16];
;			lTemp2 = b[32];
;
;			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp0 = b[48];
;			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8;
;			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp1 = b[64];
;			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8;
;			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			lTemp2 = b[80];
;			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
;			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
;			pDst += uDstPitch;
;
;			b += 64;
;		}
;		pDst -= ((uDstPitch * 8) - 1);
;		b -= ((8 << 4) - 1);
;	}
;}
;huwei 20090819 stack_bug
LAB2_0X00090006 dcd 0x00090006
	EXPORT ARMV6_AddInterpolate_H02V02 
ARMV6_AddInterpolate_H02V02  PROC
		STMFD    sp!,{r4-r11,lr}
        ;SUB      sp,sp,#160	;(8+2)*8*2 = 160
        sub      sp,sp,#168	;(8+2)*8*2 + 4 + 4 = 168
        mov      r4, #8
        str      r4,[sp,#4]
        str      r3,[sp,#0]
        add      r3,sp,#8	
;Horizontal			
		MOV	r14, #10	
AddH02V02_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		

        LDRB     r5,[r0,#0]		;lTemp0   
        LDRB     r6,[r0,#1]		;lTemp1            
        LDRB     r7,[r0,#2]		;lTemp2    
        LDRB     r8,[r0,#3]		;lTemp3

;input  r4~r8
;one ;tow	
		ADD		 r9,r5,r5,LSL #1
		ADD		 r10,r6,r6,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r7		
		
		ADD		 r11,r6,r6,LSL #1
		ADD		 r12,r7,r7,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r8
	
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r5,[r0,#4]		;lTemp4
        LDRB     r6,[r0,#5]		;lTemp5			
;three; four      	
		ADD		 r9,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r5		
		
		ADD		 r11,r8,r8,LSL #1
		ADD		 r12,r5,r5,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r6
	
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r7,[r0,#6]		;lTemp4
        LDRB     r8,[r0,#7]		;lTemp5			
;five six
		ADD		 r9,r5,r5,LSL #1
		ADD		 r10,r6,r6,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r7		
		
		ADD		 r11,r6,r6,LSL #1
		ADD		 r12,r7,r7,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r8
	
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
				
        LDRB     r5,[r0,#8]		;lTemp4
        LDRB     r6,[r0,#9]		;lTemp5	
;seven; eight  
		ADD		 r9,r7,r7,LSL #1
		ADD		 r10,r8,r8,LSL #3
		ADD		 r10,r10,r9,LSL #1
		ADD		 r10,r10,r5		
		
		ADD		 r11,r8,r8,LSL #1
		ADD		 r12,r5,r5,LSL #3
		ADD		 r12,r12,r11,LSL #1
		ADD		 r12,r12,r6
	
		PKHBT	r10, r10, r12, lsl #16			
        STR      r10,[r3], #4
    				
		SUBS	r14,r14,#1	
		add	r0, r0, r2					
		BGT		AddH02V02_loop				
;Vertical
        ;SUB      sp,sp,#160	;(8+2)*8*2 = 160
        ldr       r3,[sp,#0]
        add       lr,sp,#8      ;huwei bugfixed 20110527
;r0 r2 is free now;  used r1, r3, sp, free: r0, r2, r4~11	
		mov	r12, #128
		;mov	r14, #8                 ;huwei bugfixed 20110527
		ldr	r4, LAB2_0X00090006	
		ldr	r0, LAB2_0X01010101

AddH02V02_Vloop
;r4~r12		;(6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8
        LDR	r5,[lr], #16			;lTemp0 a = 0, 1
        LDR	r6,[lr], #16			;lTemp1 b = 0, 1                 
        LDR	r7,[lr], #16			;lTemp2 c = 0, 1        
;one	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
;two		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
;three		
		PKHBT	r9, r7, r5, lsl #16
		PKHTB	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		;lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
;four	
;one2	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
;two2		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r6,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
;three2		
		PKHBT	r9, r7, r5, lsl #16
		PKHTB	r11, r5, r7, asr #16	
		sxtah		r8, r12, r6		;lTemp2 + 128
		sxtah		r10, r12, r6, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r7,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3
		
;one3	
		PKHBT	r9, r5, r6, lsl #16
		PKHTB	r11, r6, r5, asr #16	
		sxtah		r8, r12, r7		;lTemp2 + 128
		sxtah		r10, r12, r7, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
        LDR	r5,[lr], #16			;lTemp3 d = 0, 1		
		orr	r8,r8,r10,lsl #8

		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3

;two3		
		PKHBT	r9, r6, r7, lsl #16
		PKHTB	r11, r7, r6, asr #16	
		sxtah		r8, r12, r5		;lTemp2 + 128
		sxtah		r10, r12, r5, ror #16	;lTemp2 + 128		
		SMLAD		r8, r9, r4, r8
		SMLAD		r10, r11, r4, r10	
		usat	 r8,#8,r8,ASR #8
		usat	 r10,#8,r10,ASR #8
		orr	r8,r8,r10,lsl #8
		ldrh	r2, [r1]
		uqadd8  r8, r8, r0
		uhadd8	r8, r8, r2			
		strh	r8, [r1], r3


        ;SUB      sp,sp,#156	;(8+2)*8*2 = 160 - 4
        sub      lr,lr,#156	;(8+2)*8*2 = 160 - 4
       
        ldr 	r2, [sp, #4]   ;huwei 20110527	bug fixed			
		SUBS	r2, r2,#2
		sub	r1, r1, r3, lsl #3
		add	r1, r1, #2
		str     r2, [sp, #4]		
		BGT		AddH02V02_Vloop						
	
        ;add      sp,sp,#144	;(8+2)*8*2 = 160 - 4*4
        add      sp,sp,#168
        LDMFD    sp!,{r4-r11,pc}                     
	ENDP

;/* chroma functions */
;/* Block size is 4x4 for all. */
;
;/******************************************************************/
;/* ARMV6_MCCopyChroma_H00V00 
;/*	 0 horizontal displacement 
;/*	 0 vertical displacement 
;/*	 No interpolation required, simple block copy. 
;/*******************************************************************/
;void   ARMV6_MCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;
;	/* Do not perform a sequence of U32 copies, since this function */
;	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
;
;	for (dstRow = 4; dstRow > 0; dstRow--)
;	{
;		pDst[0] = pSrc[0];
;		pDst[1] = pSrc[1];
;		pDst[2] = pSrc[2];
;		pDst[3] = pSrc[3];
;
;		pDst += uDstPitch;
;		pSrc += uSrcPitch;
;	}
;}

	EXPORT ARMV6_MCCopyChroma_H00V00 
ARMV6_MCCopyChroma_H00V00  PROC
	STMFD    sp!,{r4-r11,lr}

		ANDS	r4,r0,#3
		BNE		unalign_ChromaH00V00		
align_ChromaH00V00
align_ChromaH00V00_Chroma_loop	
		LDR		r4,[r0], r2      ;pTempSrc[0]
		LDR		r6,[r0], r2      ;pTempSrc[0]		
		LDR		r8,[r0], r2      ;pTempSrc[0]
		LDR		r10,[r0], r2      ;pTempSrc[0]				
					
		STR		r4,[r1],r3      ;pTempDst[0]
		STR		r6,[r1],r3      ;pTempDst[0]
		STR		r8,[r1],r3      ;pTempDst[0]
		STR		r10,[r1],r3      ;pTempDst[0]					
		B		end_ChromaH00V00
unalign_ChromaH00V00			
		SUB		r0,r0,r4
		MOV		r4,r4,LSL #3		;i = i<<3;
		RSB		r5,r4,#0x20			;32 - i
unalign_ChromaH00V00_Chroma_loop
		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r6,[r0], r2		;pTempSrc[0]	
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r10,[r0], r2		;pTempSrc[0]							
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5	
		STR		r6,[r1],r3
		STR		r10,[r1],r3
		
		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r6,[r0], r2		;pTempSrc[0]	
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r10,[r0], r2		;pTempSrc[0]							
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5	
		STR		r6,[r1],r3
		STR		r10,[r1],r3
end_ChromaH00V00					
		
        LDMFD    sp!,{r4-r11,pc}
	ENDP
;/*********************************************************************/
;/* ARMV6_MCCopyChroma_H01V00 
;/*	Motion compensated 4x4 chroma block copy.
;/*	1/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/**********************************************************************/
;void   ARMV6_MCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp, lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;
;		lTemp0 = pSrc[2];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];	
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pSrc += uSrcPitch;
;		pDst += uDstPitch;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H01V00 
ARMV6_MCCopyChroma_H01V00  PROC
	STMFD    sp!,{r4-r11,lr}		  	
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4			
H01V00_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1   
        LDRB     r6,[r0,#2]		;lTemp2            
        LDRB     r7,[r0,#3]		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r4,r11
		SMLABB	 r8,r12,r5,r11	
					
		ADD		 r4,r5,r5,LSL #1
		ADD		 r5,r6,r6,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0,#4]		;lTemp4					
	ORR		 r9,r9,r8,LSL #8
				
		SMLABB	 r5,r12,r6,r11
		SMLABB	 r8,r12,r7,r11
								
		ADD		 r6,r7,r7,LSL #1
		ADD		 r7,r4,r4,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
													
	ORR		 r9,r9,r5,LSL #16	
	ORR		 r9,r9,r8,LSL #24	
		STR      r9,[r1],r3					

		SUBS	r14,r14,#1	
		add	r0, r0, r2
		BGT		H01V00_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP     	

;/******************************************************************/
;/* ARMV6_MCCopyChroma_H02V00 
;/*	Motion compensated 4x4 chroma block copy.
;/*	2/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/********************************************************************/
;void   ARMV6_MCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp, lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;
;		lTemp0 = pSrc[2];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];	
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pSrc += uSrcPitch;
;		pDst += uDstPitch;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H02V00 
ARMV6_MCCopyChroma_H02V00  PROC
	STMFD    sp!,{r4-r11,lr}		  	
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4			
H02V00_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1   
        LDRB     r6,[r0,#2]		;lTemp2            
        LDRB     r7,[r0,#3]		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r5,r11
		SMLABB	 r8,r12,r6,r11	
					
		ADD		 r4,r4,r4,LSL #1
		ADD		 r5,r5,r5,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0,#4]		;lTemp4					
	ORR		 r9,r9,r8,LSL #8
				
		SMLABB	 r5,r12,r7,r11
		SMLABB	 r8,r12,r4,r11
								
		ADD		 r6,r6,r6,LSL #1
		ADD		 r7,r7,r7,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
													
	ORR		 r9,r9,r5,LSL #16	
	ORR		 r9,r9,r8,LSL #24	
		SUBS	r14,r14,#1	
		add	r0, r0, r2	
		STR      r9,[r1],r3					
		BGT		H02V00_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP     	 
;/******************************************************************/
;/* ARMV6_MCCopyChroma_H00V01 
;/*	Motion compensated 4x4 chroma block copy.
;/*	0 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/******************************************************************/
;void   ARMV6_MCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;	
;	U32 lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[uSrcPitch];
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		pDst[0] = (U8)lTemp0;
;
;		lTemp0 = pSrc[uSrcPitch << 1];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		pDst[uDstPitch] = (U8)lTemp1;
;
;		lTemp1 = pSrc[3*uSrcPitch];
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		pDst[uDstPitch << 1] = (U8)lTemp0;
;
;		lTemp0 = pSrc[uSrcPitch << 2];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		pDst[3*uDstPitch] = (U8)lTemp1;
;
;		pDst++;
;		pSrc++;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H00V01 
ARMV6_MCCopyChroma_H00V01  PROC
	STMFD    sp!,{r4-r11,lr}		  	
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4			
H00V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0], r2		;lTemp0
        LDRB     r5,[r0], r2		;lTemp1   
        LDRB     r6,[r0], r2		;lTemp2            
        LDRB     r7,[r0], r2		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r4,r11
		SMLABB	 r8,r12,r5,r11	
					
		ADD		 r4,r5,r5,LSL #1
		ADD		 r5,r6,r6,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0]		;lTemp4	
        	
        STRB     r9,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1   
				
		SMLABB	 r5,r12,r6,r11
		SMLABB	 r8,r12,r7,r11
								
		ADD		 r6,r7,r7,LSL #1
		ADD		 r7,r4,r4,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
	
		SUBS	r14,r14,#1	
		sub	r0, r0, r2, lsl #2													
        STRB     r5,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1					

		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		BGT		H00V01_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/*****************************************************************/
;/* ARMV6_MCCopyChroma_H00V02 
;/*	Motion compensated 4x4 chroma block copy.
;/*	0 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/*****************************************************************/
;void   ARMV6_MCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[uSrcPitch];
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		pDst[0] = (U8)lTemp0;
;
;		lTemp0 = pSrc[uSrcPitch << 1];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		pDst[uDstPitch] = (U8)lTemp1;
;
;		lTemp1 = pSrc[3*uSrcPitch];
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		pDst[uDstPitch << 1] = (U8)lTemp0;
;
;		lTemp0 = pSrc[uSrcPitch << 2];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		pDst[3*uDstPitch] = (U8)lTemp1;
;
;		pDst++;
;		pSrc++;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H00V02 
ARMV6_MCCopyChroma_H00V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4			
H00V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0], r2		;lTemp0
        LDRB     r5,[r0], r2		;lTemp1   
        LDRB     r6,[r0], r2		;lTemp2            
        LDRB     r7,[r0], r2		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r5,r11
		SMLABB	 r8,r12,r6,r11	
					
		ADD		 r4,r4,r4,LSL #1
		ADD		 r5,r5,r5,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0]		;lTemp4	
        	
        STRB     r9,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1   
				
		SMLABB	 r5,r12,r7,r11
		SMLABB	 r8,r12,r4,r11
								
		ADD		 r6,r6,r6,LSL #1
		ADD		 r7,r7,r7,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		

		SUBS	r14,r14,#1	
		sub	r0, r0, r2, lsl #2													
        STRB     r5,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1					
        
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		BGT		H00V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/******************************************************************/
;/* ARMV6_MCCopyChroma_H01V01 
;/*	Motion compensated chroma 4x4 block copy.
;/*	1/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/*******************************************************************/
;void   ARMV6_MCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H01V01 
ARMV6_MCCopyChroma_H01V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
H01V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r9,r9,LSL #3	;9*lTemp3
		add	r10, r5, r8		;lTemp1 + lTemp2				
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r8,r8,LSL #3	;9*lTemp3
		add	r10, r4, r9		;lTemp1 + lTemp2				
		SMLABB	 r11,r12,r5,r11		;25*lTemp0 + 9*lTemp3
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r8,r8,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r5,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
        	sub	r0, r0, #4		
		ORR		 r14,r14,r10,LSL #24
        	add	r0, r0, r2 			
        STR     r14,[r1], r3		;lTemp1					

		SUBS	r7,r7,#1				
		BGT		H01V01_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/*****************************************************************/
;/* ARMV6_MCCopyChroma_H02V01 
;/*	Motion compensated 4x4 chroma block copy.
;/*	2/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/******************************************************************/
;void   ARMV6_MCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;
;}
	EXPORT ARMV6_MCCopyChroma_H02V01 
ARMV6_MCCopyChroma_H02V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
H02V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r8,r8,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r5,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r8,r8,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r5,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
		ORR		 r14,r14,r10,LSL #24
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        STR     r14,[r1], r3		;lTemp1
      					

		SUBS	r7,r7,#1				
		BGT		H02V01_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/**********************************************************************/
;/* ARMV6_MCCopyChroma_H01V02 
;/*	Motion compensated 4x4 chroma block copy.
;/*	1/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/**********************************************************************/
;void   ARMV6_MCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32	  lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;}
	EXPORT ARMV6_MCCopyChroma_H01V02 
ARMV6_MCCopyChroma_H01V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
H01V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r5,r5,LSL #3	;9*lTemp1		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r8,r11		;25*lTemp2 + 9*lTemp1	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r5,r5,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r8,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
		ORR		 r14,r14,r10,LSL #24
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        STR     r14,[r1], r3		;lTemp1
      					

		SUBS	r7,r7,#1				
		BGT		H01V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/*******************************************************************/
;/* ARMV6_MCCopyChroma_H02V02 
;/*	Motion compensated 4x4 chroma block copy. 
;/*	2/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/********************************************************************/
;void   ARMV6_MCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	const U8 *pSrc2 = pSrc + uPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		*((PU32)pDst) = lTemp;
;		pDst  += uDstPitch;
;		pSrc  += uPitch;
;		pSrc2 += uPitch;
;	}
;
;}
	EXPORT ARMV6_MCCopyChroma_H02V02 
ARMV6_MCCopyChroma_H02V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
H02V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r5,r5,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r8,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r5,r5,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r8,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #24	
        	sub	r0, r0, #4
        	add	r0, r0, r2 		
        STR     r14,[r1], r3		;lTemp1      					

		SUBS	r7,r7,#1				
		BGT		H02V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/******************************************************************/
;/* ARMV6_AddMCCopyChroma_H00V00 
;/*	 0 horizontal displacement 
;/*	 0 vertical displacement 
;/*	 No interpolation required, simple block copy. 
;/*******************************************************************/
;void   ARMV6_AddMCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 dstRow;
;
;	/* Do not perform a sequence of U32 copies, since this function */
;	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */
;
;	for (dstRow = 4; dstRow > 0; dstRow--)
;	{
;		pDst[0] = (pSrc[0] + pDst[0] + 1)>>1;
;		pDst[1] = (pSrc[1] + pDst[1] + 1)>>1;
;		pDst[2] = (pSrc[2] + pDst[2] + 1)>>1;
;		pDst[3] = (pSrc[3] + pDst[3] + 1)>>1;
;
;		pDst += uDstPitch;
;		pSrc += uSrcPitch;
;	}
;}
cLAB_0X01010101 dcd 0x01010101
	EXPORT ARMV6_AddMCCopyChroma_H00V00 
ARMV6_AddMCCopyChroma_H00V00  PROC
        STMFD    sp!,{r4-r11,lr}
        	  
		ANDS	r4,r0,#3
		ldr	r12, cLAB_0X01010101					
		BNE		unalign_Chroma_AddH00V00		
align_Chroma_AddH00V00
		mov	r14, r1
align_Chroma_AddH00V00_Chroma_loop
		LDR		r4,[r0], r2      ;pTempSrc[0]
		LDR		r6,[r0], r2      ;pTempSrc[0]		
		LDR		r8,[r0], r2      ;pTempSrc[0]
		LDR		r10,[r0], r2      ;pTempSrc[0]
		
		LDR		r5,[r14], r3      ;pTempSrc[0]
		LDR		r7,[r14], r3      ;pTempSrc[0]		
		LDR		r9,[r14], r3      ;pTempSrc[0]
		LDR		r11,[r14], r3      ;pTempSrc[0]						
			
		uqadd8  r4, r4, r12
		uqadd8  r6, r6, r12
		uqadd8  r8, r8, r12
		uqadd8  r10, r10, r12		
				  
		uhadd8	r4, r4, r5
		uhadd8	r6, r6, r7
		uhadd8	r8, r8, r9		
		uhadd8	r10, r10, r11
							
		STR		r4,[r1],r3      ;pTempDst[0]
		STR		r6,[r1],r3      ;pTempDst[0]
		STR		r8,[r1],r3      ;pTempDst[0]
		STR		r10,[r1],r3      ;pTempDst[0]
						
		B		end_Chroma_AddH00V00
unalign_Chroma_AddH00V00		
		SUB		r0,r0,r4
		MOV		r4,r4,LSL #3		;i = i<<3;
		RSB		r5,r4,#0x20			;32 - i
unalign_Chroma_AddH00V00_Chroma_loop

		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r6,[r0], r2		;pTempSrc[0]	
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r10,[r0], r2		;pTempSrc[0]							
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5
		
		LDR		r8,[r1]      ;pTempSrc[0]
		LDR		r9,[r1, r3]      ;pTempSrc[0]		
		uqadd8  r6, r6, r12
		uqadd8  r10, r10, r12
		uhadd8	r6, r6, r8
		uhadd8	r10, r10, r9		
					
		STR		r6,[r1],r3
		STR		r10,[r1],r3

		LDR		r7,[r0,#4]		;pTempSrc[1]
		LDR		r6,[r0], r2		;pTempSrc[0]	
		LDR		r11,[r0,#4]		;pTempSrc[1]
		LDR		r10,[r0], r2		;pTempSrc[0]							
		MOV		r6,r6,LSR r4
		MOV		r10,r10,LSR r4			
		ORR		r6,r6,r7,LSL r5
		ORR		r10,r10,r11,LSL r5
		
		LDR		r8,[r1]      ;pTempSrc[0]
		LDR		r9,[r1, r3]      ;pTempSrc[0]		
		uqadd8  r6, r6, r12
		uqadd8  r10, r10, r12
		uhadd8	r6, r6, r8
		uhadd8	r10, r10, r9		
					
		STR		r6,[r1],r3
		STR		r10,[r1],r3
end_Chroma_AddH00V00				

        LDMFD    sp!,{r4-r11,pc}
	ENDP	
;/*********************************************************************/
;/* ARMV6_AddMCCopyChroma_H01V00 
;/*	Motion compensated 4x4 chroma block copy.
;/*	1/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/**********************************************************************/
;void   ARMV6_AddMCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp, lTemp0, lTemp1;
;	U32 c,q;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;
;		lTemp0 = pSrc[2];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];	
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pSrc += uSrcPitch;
;		pDst += uDstPitch;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H01V00 
ARMV6_AddMCCopyChroma_H01V00  PROC
	STMFD    sp!,{r4-r11,lr}
	
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4
		ldr	r10, cLAB_0X01010101					
Chroma_AddH01V00_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1   
        LDRB     r6,[r0,#2]		;lTemp2            
        LDRB     r7,[r0,#3]		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r4,r11
		SMLABB	 r8,r12,r5,r11	
					
		ADD		 r4,r5,r5,LSL #1
		ADD		 r5,r6,r6,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0,#4]		;lTemp4					
	ORR		 r9,r9,r8,LSL #8
				
		SMLABB	 r5,r12,r6,r11
		SMLABB	 r8,r12,r7,r11
								
		ADD		 r6,r7,r7,LSL #1
		ADD		 r7,r4,r4,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
													
	ORR		 r9,r9,r5,LSL #16	
	ORR		 r9,r9,r8,LSL #24
	
        LDR     r4,[r1,#0]		;lTemp0				
		uqadd8  r9, r9, r10
		uhadd8	r9, r9, r4
		SUBS	r14,r14,#1	
		add	r0, r0, r2			
		STR      r9,[r1],r3					

					
		BGT		Chroma_AddH01V00_Chroma_loop				

        LDMFD    sp!,{r4-r11,pc}                      
	ENDP     

;/******************************************************************/
;/* ARMV6_AddMCCopyChroma_H02V00 
;/*	Motion compensated 4x4 chroma block copy.
;/*	2/3 pel horizontal displacement 
;/*	0 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/********************************************************************/
;void   ARMV6_AddMCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp, lTemp0, lTemp1;
;	U32 c,q;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;
;		lTemp0 = pSrc[2];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];	
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pSrc += uSrcPitch;
;		pDst += uDstPitch;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H02V00 
ARMV6_AddMCCopyChroma_H02V00  PROC
	STMFD    sp!,{r4-r11,lr}		  	
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4	
		ldr	r10, cLAB_0X01010101				
Chroma_AddH02V00_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0,#0]		;lTemp0
        LDRB     r5,[r0,#1]		;lTemp1   
        LDRB     r6,[r0,#2]		;lTemp2            
        LDRB     r7,[r0,#3]		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r5,r11
		SMLABB	 r8,r12,r6,r11	
					
		ADD		 r4,r4,r4,LSL #1
		ADD		 r5,r5,r5,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
        	LDRB     r4,[r0,#4]		;lTemp4					
	ORR		 r9,r9,r8,LSL #8
				
		SMLABB	 r5,r12,r7,r11
		SMLABB	 r8,r12,r4,r11
								
		ADD		 r6,r6,r6,LSL #1
		ADD		 r7,r7,r7,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
													
	ORR		 r9,r9,r5,LSL #16	
	ORR		 r9,r9,r8,LSL #24
	
        LDR     r4,[r1,#0]		;lTemp0				
		uqadd8  r9, r9, r10
		uhadd8	r9, r9, r4
		SUBS	r14,r14,#1	
		add	r0, r0, r2			
		STR      r9,[r1],r3					


		BGT		Chroma_AddH02V00_Chroma_loop				
								              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP   
;/******************************************************************/
;/* ARMV6_AddMCCopyChroma_H00V01 
;/*	Motion compensated 4x4 chroma block copy.
;/*	0 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/******************************************************************/
;void   ARMV6_AddMCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;	
;	U32 lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[uSrcPitch];
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1;
;
;		lTemp0 = pSrc[uSrcPitch << 1];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1;
;
;		lTemp1 = pSrc[3*uSrcPitch];
;		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
;		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1;
;
;		lTemp0 = pSrc[uSrcPitch << 2];
;		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
;		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1;
;
;		pDst++;
;		pSrc++;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H00V01 
ARMV6_AddMCCopyChroma_H00V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4
		mov	r10, #1			
Chroma_AddH00V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0], r2		;lTemp0
        LDRB     r5,[r0], r2		;lTemp1   
        LDRB     r6,[r0], r2		;lTemp2            
        LDRB     r7,[r0], r2		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r4,r11
		SMLABB	 r8,r12,r5,r11	
					
		ADD		 r4,r5,r5,LSL #1
		ADD		 r5,r6,r6,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3
		
		ldrb	r4, [r1]
		ldrb	r5, [r1, r3]
		uqadd8  r9, r9, r10
		uqadd8  r8, r8, r10
		uhadd8	r9, r9, r4
		uhadd8	r8, r8, r5							
        	LDRB     r4,[r0]		;lTemp4        	
        STRB     r9,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1   

				
		SMLABB	 r5,r12,r6,r11
		SMLABB	 r8,r12,r7,r11
								
		ADD		 r6,r7,r7,LSL #1
		ADD		 r7,r4,r4,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		

		ldrb	r4, [r1]
		ldrb	r6, [r1, r3]
		uqadd8  r5, r5, r10
		uqadd8  r8, r8, r10
		uhadd8	r5, r5, r4
		uhadd8	r8, r8, r6
															
        STRB     r5,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1					

		SUBS	r14,r14,#1	
		sub	r0, r0, r2, lsl #2
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		BGT		Chroma_AddH00V01_Chroma_loop										              	 		
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	
;/*****************************************************************/
;/* ARMV6_AddMCCopyChroma_H00V02 
;/*	Motion compensated 4x4 chroma block copy.
;/*	0 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/*****************************************************************/
;void   ARMV6_AddMCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32 j;
;	U32 lTemp0, lTemp1;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[uSrcPitch];
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1;
;
;		lTemp0 = pSrc[uSrcPitch << 1];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1;
;
;		lTemp1 = pSrc[3*uSrcPitch];
;		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
;		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1;
;
;		lTemp0 = pSrc[uSrcPitch << 2];
;		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
;		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1;
;		pDst++;
;		pSrc++;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H00V02 
ARMV6_AddMCCopyChroma_H00V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r14, #4
		MOV	r12, #5
		MOV	r11,#4	
		mov	r10, #1					
Chroma_AddH00V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d		
        LDRB     r4,[r0], r2		;lTemp0
        LDRB     r5,[r0], r2		;lTemp1   
        LDRB     r6,[r0], r2		;lTemp2            
        LDRB     r7,[r0], r2		;lTemp3    

;input  r4~r8
;one ;tow  
		SMLABB	 r9,r12,r5,r11
		SMLABB	 r8,r12,r6,r11	
					
		ADD		 r4,r4,r4,LSL #1
		ADD		 r5,r5,r5,LSL #1
					
		ADD		 r9,r9,r4
		ADD		 r8,r8,r5
						
		MOV		 r9,r9,ASR #3
		MOV		 r8,r8,ASR #3	
        	
		ldrb	r4, [r1]
		ldrb	r5, [r1, r3]
		uqadd8  r9, r9, r10
		uqadd8  r8, r8, r10
		uhadd8	r9, r9, r4
		uhadd8	r8, r8, r5
									
        	LDRB     r4,[r0]		;lTemp4        	
        STRB     r9,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1    
				
		SMLABB	 r5,r12,r7,r11
		SMLABB	 r8,r12,r4,r11
								
		ADD		 r6,r6,r6,LSL #1
		ADD		 r7,r7,r7,LSL #1
					
		ADD		 r5,r5,r6
		ADD		 r8,r8,r7
						
		MOV		 r5,r5,ASR #3
		MOV		 r8,r8,ASR #3		
													
		ldrb	r4, [r1]
		ldrb	r6, [r1, r3]
		uqadd8  r5, r5, r10
		uqadd8  r8, r8, r10
		uhadd8	r5, r5, r4
		uhadd8	r8, r8, r6
															
        STRB     r5,[r1], r3		;lTemp0
        STRB     r8,[r1], r3		;lTemp1					

		SUBS	r14,r14,#1	
		sub	r0, r0, r2, lsl #2
		sub	r1, r1, r3, lsl #2		
		add	r0, r0, #1
		add	r1, r1, #1				
		BGT		Chroma_AddH00V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 
;/******************************************************************/
;/* ARMV6_AddMCCopyChroma_H01V01 
;/*	Motion compensated chroma 4x4 block copy.
;/*	1/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/*******************************************************************/
;void   ARMV6_AddMCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32   j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	U32   c,q;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H01V01 
ARMV6_AddMCCopyChroma_H01V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
Chroma_AddH01V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r8,r8,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r5,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r8,r8,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r5,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
	ldr	r5, cLAB_0X01010101		
		ORR		 r14,r14,r10,LSL #24
	
        LDR     r4,[r1]			;lTemp1		
		uqadd8  r14, r14, r5
       	sub	r0, r0, #4		
		uhadd8	r14, r14, r4
        	add	r0, r0, r2  				
        STR     r14,[r1], r3		;lTemp1
     					

		SUBS	r7,r7,#1				
		BGT		Chroma_AddH01V01_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	


;/*****************************************************************/
;/* ARMV6_AddMCCopyChroma_H02V01 
;/*	Motion compensated 4x4 chroma block copy.
;/*	2/3 pel horizontal displacement 
;/*	1/3 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Use vertical filter (5,3) 
;/*	Dst pitch is uDstPitch. 
;/******************************************************************/
;void   ARMV6_AddMCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32   j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	U32   c,q;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;
;}
	EXPORT ARMV6_AddMCCopyChroma_H02V01 
ARMV6_AddMCCopyChroma_H02V01  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
Chroma_AddH02V01_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r8,r8,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r5,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r9,r9,LSL #3	;9*lTemp3
		add	r10, r5, r8		;lTemp1 + lTemp2				
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r8,r8,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r5,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r9,r9,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r4,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
	ldr	r5, cLAB_0X01010101		
		ORR		 r14,r14,r10,LSL #24
	
        LDR     r4,[r1]			;lTemp1
		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4	

        	add	r0, r0, r2 				
        STR     r14,[r1], r3		;lTemp1
      					

		SUBS	r7,r7,#1				
		BGT		Chroma_AddH02V01_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/**********************************************************************/
;/* ARMV6_AddMCCopyChroma_H01V02 
;/*	Motion compensated 4x4 chroma block copy.
;/*	1/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (5,3) 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/**********************************************************************/
;void   ARMV6_AddMCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
;{
;	I32   j;
;	U32	  lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	U32   c,q;
;	const U8 *pSrc2 = pSrc + uSrcPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pDst  += uDstPitch;
;		pSrc  += uSrcPitch;
;		pSrc2 += uSrcPitch;
;	}
;}
	EXPORT ARMV6_AddMCCopyChroma_H01V02 
ARMV6_AddMCCopyChroma_H01V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
Chroma_AddH01V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r5,r5,LSL #3	;9*lTemp1		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r8,r11		;25*lTemp2 + 9*lTemp1	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r5,r5,LSL #3	;9*lTemp2		
		add	r10, r4, r9		;lTemp0 + lTemp3		
		SMLABB	 r11,r12,r8,r11		;25*lTemp1 + 9*lTemp2	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp0 + lTemp3)
		add	r10, r10, #32		;15*(lTemp0 + lTemp3) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
	ldr	r5, cLAB_0X01010101		
		ORR		 r14,r14,r10,LSL #24
	
        LDR     r4,[r1]			;lTemp1		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4		
        	add	r0, r0, r2			
        STR     r14,[r1], r3		;lTemp1       					

		SUBS	r7,r7,#1				
		BGT		Chroma_AddH01V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP 	

;/*******************************************************************/
;/* ARMV6_AddMCCopyChroma_H02V02 
;/*	Motion compensated 4x4 chroma block copy. 
;/*	2/3 pel horizontal displacement 
;/*	2/3 vertical displacement 
;/*	Use horizontal filter (3,5) 
;/*	Use vertical filter (3,5) 
;/*	Dst pitch is uDstPitch. 
;/********************************************************************/
;void   ARMV6_AddMCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
;{
;	I32   j;
;	U32   lTemp;
;	U32   lTemp0, lTemp1;
;	U32   lTemp2, lTemp3;
;	U32   c,q;
;	const U8 *pSrc2 = pSrc + uPitch;
;
;	for (j = 4; j > 0; j--)
;	{
;		lTemp0 = pSrc[0];
;		lTemp1 = pSrc[1];
;		lTemp2 = pSrc2[0];
;		lTemp3 = pSrc2[1];
;		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
;
;		lTemp0 = pSrc[2];
;		lTemp2 = pSrc2[2];
;		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 8);
;
;		lTemp1 = pSrc[3];
;		lTemp3 = pSrc2[3];
;		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
;		lTemp |= (lTemp0 << 16);
;
;		lTemp0 = pSrc[4];
;		lTemp2 = pSrc2[4];
;		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
;		lTemp |= (lTemp1 << 24);
;
;		c=((U32 *)pDst)[0];
;		q=(lTemp|c) & 0x01010101;
;		q+=(lTemp>>1) & 0x7F7F7F7F;
;		q+=(c>>1) & 0x7F7F7F7F;
;
;		*((PU32)pDst) = q;
;		pDst  += uDstPitch;
;		pSrc  += uPitch;
;		pSrc2 += uPitch;
;	}
;
;}
	EXPORT ARMV6_AddMCCopyChroma_H02V02 
ARMV6_AddMCCopyChroma_H02V02  PROC
	STMFD    sp!,{r4-r11,lr}
		MOV	r12, #25
		mov	r7, #4			
Chroma_AddH02V02_Chroma_loop
;r4~r9, r10~12		; -a + 6*(2*b + c) -d
        LDRB     r8,[r0, r2]		;lTemp00
        LDRB     r4,[r0], #1		;lTemp0	
        LDRB     r9,[r0, r2]		;lTemp11                       	
        LDRB     r5,[r0], #1		;lTemp1
;input  r4,r5,r8,r9 
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r14,r10,ASR #6
						
        LDRB     r8,[r0, r2]		;lTemp33						
        LDRB     r4,[r0], #1		;lTemp22        
;input  r5,r4,r9,r8  
		add	r11,r5,r5,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r8,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
		ORR		 r14,r14,r10,LSL #8			
		
        LDRB     r9,[r0, r2]		;lTemp33		
        LDRB     r5,[r0], #1		;lTemp22
;input  r4,r5,r8,r9 
		add	r11,r4,r4,LSL #3	;9*lTemp3		
		add	r10, r5, r8		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r9,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6
				
        LDRB     r8,[r0, r2]		;lTemp33				
        LDRB     r4,[r0]		;lTemp22
		ORR		 r14,r14,r10,LSL #16
;input  r5,r4,r9,r8  
		add	r11,r5,r5,LSL #3	;9*lTemp3		
		add	r10, r4, r9		;lTemp1 + lTemp2		
		SMLABB	 r11,r12,r8,r11		;25*lTemp0 + 9*lTemp3	
		RSB	r10,r10,r10,LSL #4	;15*(lTemp1 + lTemp2)
		add	r10, r10, #32		;15*(lTemp1 + lTemp2) + 32
		add	r10, r10, r11
		MOV		 r10,r10,ASR #6	
	ldr	r5, cLAB_0X01010101		
		ORR		 r14,r14,r10,LSL #24
	
        LDR     r4,[r1]			;lTemp1		
		uqadd8  r14, r14, r5
        	sub	r0, r0, #4		
		uhadd8	r14, r14, r4		
        	add	r0, r0, r2  			
        STR     r14,[r1], r3		;lTemp1     					

		SUBS	r7,r7,#1				
		BGT		Chroma_AddH02V02_Chroma_loop				
	
        LDMFD    sp!,{r4-r11,pc}                      
	ENDP
	
	END




