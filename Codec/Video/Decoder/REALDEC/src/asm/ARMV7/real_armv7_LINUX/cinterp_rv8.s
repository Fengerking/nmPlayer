@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2005		            *
@								 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
	.section .text
	 
	
 .macro	Interpolate_H01_OR_H02 is_H01	@input q1~4,  output q5~8,q9~12
	vmov.s16 d1, #6	
	
	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
			
@first 4 line									    
	vext.u8 d16, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d17, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d28, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d29, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d30, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d31, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]	
								                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
	.if \is_H01>0
	vshll.u8 q5, d16, #1		@(lTemp1<<1)
	vshll.u8 q6, d26, #1		@(lTemp1<<1)	
	vaddw.u8 q5, q5, d17		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q6, q6, d27		@((lTemp1<<1) + lTemp2)	
	.else
	vshll.u8 q5, d17, #1		@(lTemp1<<1)
	vshll.u8 q6, d27, #1		@(lTemp1<<1)	
	vaddw.u8 q5, q5, d16		@((lTemp1<<1) + lTemp2)	
	vaddw.u8 q6, q6, d26		@((lTemp1<<1) + lTemp2)			
	.endif	
	vmul.i16 q5, q5, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddl.u8 q1, d2, d3		@-lTemp0 -lTemp3
	vmul.i16 q6, q6, d1[0]		@6*((lTemp1<<1) + lTemp2)			
	vaddl.u8 q2, d4, d5		@-lTemp0 -lTemp3
	vsub.s16 q5, q5, q1	@result	
	vsub.s16 q6, q6, q2	@result	
	
	.if \is_H01>0
	vshll.u8 q7, d28, #1		@(lTemp1<<1)
	vshll.u8 q8, d30, #1		@(lTemp1<<1)	
	vaddw.u8 q7, q7, d29		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q8, q8, d31		@((lTemp1<<1) + lTemp2)	
	.else
	vshll.u8 q7, d29, #1		@(lTemp1<<1)
	vshll.u8 q8, d31, #1		@(lTemp1<<1)	
	vaddw.u8 q7, q7, d28		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q8, q8, d30		@((lTemp1<<1) + lTemp2)				
	.endif		
	vmul.i16 q7, q7, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddl.u8 q3, d6, d7		@-lTemp0 -lTemp3		
	vmul.i16 q8, q8, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddl.u8 q4, d8, d9		@-lTemp0 -lTemp3
	vsub.s16 q7, q7, q3	@result		
	vsub.s16 q8, q8, q4	@result			
	
	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]		
@last 4 line																				    
	vext.u8 d24, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d25, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d4, d5, #2		@lTemp2= pSrc[1~8]
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]
	             
	vext.u8 d28, d6, d7, #1		@lTemp1= pSrc[0~7]
	vext.u8 d29, d6, d7, #2		@lTemp2= pSrc[1~8]
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]
	
	vext.u8 d30, d8, d9, #1		@lTemp1= pSrc[0~7]
	vext.u8 d31, d8, d9, #2		@lTemp2= pSrc[1~8]
	vext.u8 d9, d8, d9, #3		@lTemp3= pSrc[2~9]	
								                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
	.if \is_H01>0
	vshll.u8 q9, d24, #1		@(lTemp1<<1)
	vshll.u8 q10, d26, #1		@(lTemp1<<1)	
	vaddw.u8 q9, q9, d25		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q10, q10, d27		@((lTemp1<<1) + lTemp2)	
	.else
	vshll.u8 q9, d25, #1		@(lTemp1<<1)
	vshll.u8 q10, d27, #1		@(lTemp1<<1)	
	vaddw.u8 q9, q9, d24		@((lTemp1<<1) + lTemp2)	
	vaddw.u8 q10, q10, d26		@((lTemp1<<1) + lTemp2)			
	.endif	
	vmul.i16 q9, q9, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddl.u8 q1, d2, d3		@-lTemp0 -lTemp3
	vmul.i16 q10, q10, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddl.u8 q2, d4, d5		@-lTemp0 -lTemp3
	vsub.s16 q9, q9, q1	@result	
	vsub.s16 q10, q10, q2	@result	
	
	.if \is_H01>0
	vshll.u8 q11, d28, #1		@(lTemp1<<1)
	vshll.u8 q12, d30, #1		@(lTemp1<<1)	
	vaddw.u8 q11, q11, d29		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q12, q12, d31		@((lTemp1<<1) + lTemp2)	
	.else
	vshll.u8 q11, d29, #1		@(lTemp1<<1)
	vshll.u8 q12, d31, #1		@(lTemp1<<1)	
	vaddw.u8 q11, q11, d28		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q12, q12, d30		@((lTemp1<<1) + lTemp2)			
	.endif	
	vmul.i16 q11, q11, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddl.u8 q3, d6, d7		@-lTemp0 -lTemp3
	vmul.i16 q12, q12, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddl.u8 q4, d8, d9		@-lTemp0 -lTemp3
	vsub.s16 q11, q11, q3	@result		
	vsub.s16 q12, q12, q4	@result	
 .endm	
 
 .macro Interpolate_H01_OR_H02_22	@output q5~8,q9~12,q13~14
	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
		
	vmov.s8 d0, #6	
	vmov.s8 d1, #9
				
@first 4 line									    
	vmull.u8 q5, d2, d0		@6*lTemp1
	vext.u8 d16, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d17, d2, d3, #2		@lTemp2= pSrc[1~8]
	vmlal.u8 q5, d16, d1		@6*lTemp1 + 9*lTemp2
		    
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d4, d5, #2		@lTemp2= pSrc[1~8]								                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
@result = ( 0*lTemp0 + 6*lTemp1 +      9*lTemp2 + lTemp3 + 128) >> 8@


	vmull.u8 q6, d4, d0		@6*lTemp1		
	vaddw.u8 q5, q5, d17		@+ lTemp3  result
	vmlal.u8 q6, d26, d1	@6*lTemp1 + 9*lTemp2	
	
	vext.u8 d28, d6, d7, #1		@lTemp1= pSrc[0~7]
	vaddw.u8 q6, q6, d27		@+ lTemp3  result	
	vmull.u8 q7, d6, d0		@6*lTemp1

	vext.u8 d29, d6, d7, #2		@lTemp2= pSrc[1~8]	
	vmlal.u8 q7, d28, d1	@6*lTemp1 + 9*lTemp2	
	vext.u8 d30, d8, d9, #1		@lTemp1= pSrc[0~7]
	vmull.u8 q8, d8, d0		@6*lTemp1			
	vext.u8 d31, d8, d9, #2		@lTemp2= pSrc[1~8]	
	vmlal.u8 q8, d30, d1	@6*lTemp1 + 9*lTemp2
	vaddw.u8 q7, q7, d29		@+ lTemp3  result			
	vaddw.u8 q8, q8, d31		@+ lTemp3  result			
	
	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q3}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q4}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]	
@last 4 line	
	vmull.u8 q9, d2, d0		@6*lTemp1																			    
	vext.u8 d24, d2, d3, #1		@lTemp1= pSrc[0~7]
	vmull.u8 q10, d4, d0		@6*lTemp1	
	vext.u8 d25, d2, d3, #2		@lTemp2= pSrc[1~8]
	    
	vext.u8 d26, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d27, d4, d5, #2		@lTemp2= pSrc[1~8]
	             
	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0]		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]	
		
	vmlal.u8 q9, d24, d1		@6*lTemp1 + 9*lTemp2		             
	vext.u8 d28, d6, d7, #1		@lTemp1= pSrc[0~7]
	vaddw.u8 q9, q9, d25		@+ lTemp3  result
	vmlal.u8 q10, d26, d1	@6*lTemp1 + 9*lTemp2		
	vext.u8 d29, d6, d7, #2		@lTemp2= pSrc[1~8]
	
	vmull.u8 q11, d6, d0		@6*lTemp1	
	vext.u8 d30, d8, d9, #1		@lTemp1= pSrc[0~7]
	vmlal.u8 q11, d28, d1	@6*lTemp1 + 9*lTemp2	
	vaddw.u8 q10, q10, d27		@+ lTemp3  result
	vmull.u8 q12, d8, d0		@6*lTemp1	
	vaddw.u8 q11, q11, d29		@+ lTemp3  result
	vmlal.u8 q12, d30, d1	@6*lTemp1 + 9*lTemp2			
	vext.u8 d31, d8, d9, #2		@lTemp2= pSrc[1~8]
								                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
@result = ( 0*lTemp0 + 6*lTemp1 +      9*lTemp2 + lTemp3 + 128) >> 8@
	vmull.u8 q13, d2, d0		@6*lTemp1
	vext.u8 d6, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d7, d2, d3, #2		@lTemp2= pSrc[1~8]
	vmlal.u8 q13, d6, d1		@6*lTemp1 + 9*lTemp2
	vaddw.u8 q13, q13, d7		@+ lTemp3  result	
	vmull.u8 q14, d4, d0		@6*lTemp1		
	vext.u8 d8, d4, d5, #1		@lTemp1= pSrc[0~7]
	vext.u8 d9, d4, d5, #2		@lTemp2= pSrc[1~8]
	vmlal.u8 q14, d8, d1	@6*lTemp1 + 9*lTemp2					
	vaddw.u8 q12, q12, d31		@+ lTemp3  result
	vaddw.u8 q14, q14, d9		@+ lTemp3  result	
	

@last 2 line									                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
@result = ( 0*lTemp0 + 6*lTemp1 +      9*lTemp2 + lTemp3 + 128) >> 8@			
	
 .endm	
 
 .macro Interpolate_H01_OR_H02_HV is_H01	@input q1~3,  output q13~15 @q5~12	

	vld1.8 {q1}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q2}, [r0], r2		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
	vld1.8 {q3}, [r0]		@d0 lTemp0= pSrc[-1~6] d1 = pSrc[7~14]
			
@first 4 line									    
	vext.u8 d28, d2, d3, #1		@lTemp1= pSrc[0~7]
	vext.u8 d29, d2, d3, #2		@lTemp2= pSrc[1~8]
	vext.u8 d3, d2, d3, #3		@lTemp3= pSrc[2~9]
	    							                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
	.if \is_H01>0
	vshll.u8 q13, d28, #1		@(lTemp1<<1)
	vext.u8 d30, d4, d5, #1		@lTemp1= pSrc[0~7]	
	vaddw.u8 q13, q13, d29		@((lTemp1<<1) + lTemp2)
	.else
	vshll.u8 q13, d29, #1		@(lTemp1<<1)
	vext.u8 d30, d4, d5, #1		@lTemp1= pSrc[0~7]	
	vaddw.u8 q13, q13, d28		@((lTemp1<<1) + lTemp2)		
	.endif
	vext.u8 d31, d4, d5, #2		@lTemp2= pSrc[1~8]	
	vmul.i16 q13, q13, d1[0]		@6*((lTemp1<<1) + lTemp2)			
	vaddl.u8 q1, d2, d3		@-lTemp0 -lTemp3
	vext.u8 d5, d4, d5, #3		@lTemp3= pSrc[2~9]	
	vsub.s16 q13, q13, q1	@result
		
	.if \is_H01>0
	vshll.u8 q14, d30, #1		@(lTemp1<<1)
	vext.u8 d8, d6, d7, #1		@lTemp1= pSrc[0~7]	
	vaddw.u8 q14, q14, d31		@((lTemp1<<1) + lTemp2)
	.else
	vshll.u8 q14, d31, #1		@(lTemp1<<1)
	vext.u8 d8, d6, d7, #1		@lTemp1= pSrc[0~7]	
	vaddw.u8 q14, q14, d30		@((lTemp1<<1) + lTemp2)		
	.endif
	vext.u8 d9, d6, d7, #2		@lTemp2= pSrc[1~8]	
	vmul.i16 q14, q14, d1[0]		@6*((lTemp1<<1) + lTemp2)			
	
	.if \is_H01>0
	vshll.u8 q15, d8, #1		@(lTemp1<<1)
	vaddl.u8 q2, d4, d5		@-lTemp0 -lTemp3	
	vaddw.u8 q15, q15, d9		@((lTemp1<<1) + lTemp2)
	.else
	vshll.u8 q15, d9, #1		@(lTemp1<<1)
	vaddl.u8 q2, d4, d5		@-lTemp0 -lTemp3	
	vaddw.u8 q15, q15, d8		@((lTemp1<<1) + lTemp2)		
	.endif	
	vext.u8 d7, d6, d7, #3		@lTemp3= pSrc[2~9]		
	vmul.i16 q15, q15, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddl.u8 q3, d6, d7		@-lTemp0 -lTemp3
	vsub.s16 q14, q14, q2	@result		
	vsub.s16 q15, q15, q3	@result	
 .endm  
 
 .macro Interpolate_V01_OR_V02 is_V01	@input q9,q10,q11,q12,q13~15,q1~4,  output q5~q8,q9~12 		
	
	vld1.8	{d18}, [r0], r2		@pSrc[-uSrcPitch]	
				
	vld1.8	{d20}, [r0], r2		@pSrc[0]
	vld1.8	{d22}, [r0], r2		@pSrc[uSrcPitch] 
	vld1.8	{d24}, [r0], r2		@pSrc[2*uSrcPitch] 
	vld1.8	{d26}, [r0], r2		@pSrc[3*uSrcPitch] 
	vld1.8	{d28}, [r0], r2		@pSrc[4*uSrcPitch] 
	vld1.8	{d30}, [r0], r2		@pSrc[5*uSrcPitch] 
	vld1.8	{d2}, [r0], r2		@pSrc[6*uSrcPitch] 
	vld1.8	{d4}, [r0], r2		@pSrc[7*uSrcPitch]
	 
	vld1.8	{d6}, [r0], r2		@pSrc[8*uSrcPitch] 
	vld1.8	{d8}, [r0], r2		@pSrc[9*uSrcPitch] 	
	vmov.s16 d1, #6
											                                    	                                    
@first 4 line												                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4

@1 line input d18, d20, d22, d24
	.if \is_V01>0
	vshll.u8 q5, d20, #1		@(lTemp1<<1)
	vshll.u8 q6, d22, #1		@(lTemp1<<1)
	vshll.u8 q7, d24, #1		@(lTemp1<<1)
	vshll.u8 q8, d26, #1		@(lTemp1<<1)			
	vaddw.u8 q5, q5, d22		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q6, q6, d24		@((lTemp1<<1) + lTemp2)
	vmul.i16 q5, q5, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddw.u8 q7, q7, d26		@((lTemp1<<1) + lTemp2)
	vmul.i16 q6, q6, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddw.u8 q8, q8, d28		@((lTemp1<<1) + lTemp2)			
	.else
	vshll.u8 q5, d22, #1		@(lTemp1<<1)
	vshll.u8 q6, d24, #1		@(lTemp1<<1)
	vshll.u8 q7, d26, #1		@(lTemp1<<1)
	vshll.u8 q8, d28, #1		@(lTemp1<<1)			
	vaddw.u8 q5, q5, d20		@((lTemp1<<1) + lTemp2)	
	vaddw.u8 q6, q6, d22		@((lTemp1<<1) + lTemp2)
	vmul.i16 q5, q5, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddw.u8 q7, q7, d24		@((lTemp1<<1) + lTemp2)
	vmul.i16 q6, q6, d1[0]		@6*((lTemp1<<1) + lTemp2)		
	vaddw.u8 q8, q8, d26		@((lTemp1<<1) + lTemp2)					
	.endif	
	vmul.i16 q7, q7, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddl.u8 q9, d18, d24		@-lTemp0 -lTemp3
	vmul.i16 q8, q8, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vsub.s16 q5, q5, q9		@result	
@2 line input d20, d22, d24, d26		
	vaddl.u8 q10, d20, d26		@-lTemp0 -lTemp3
	vaddl.u8 q11, d22, d28		@-lTemp0 -lTemp3	
	vsub.s16 q6, q6, q10		@result		
@3 line input d22, d24, d26, d28 	
	vaddl.u8 q12, d24, d30		@-lTemp0 -lTemp3
	vsub.s16 q7, q7, q11		@result		
@4 line input d24, d26, d28, d30 	
	vsub.s16 q8, q8, q12		@result				
	
@last 4 line						
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4

@5 line input d26, d28, d30, d2
	.if \is_V01>0
	vshll.u8 q9, d28, #1		@(lTemp1<<1)
	vshll.u8 q10, d30, #1		@(lTemp1<<1)
	vshll.u8 q11, d2, #1		@(lTemp1<<1)		
	vshll.u8 q12, d4, #1		@(lTemp1<<1)	
	vaddw.u8 q9, q9, d30		@((lTemp1<<1) + lTemp2)
	vaddw.u8 q10, q10, d2		@((lTemp1<<1) + lTemp2)
	vmul.i16 q9, q9, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddw.u8 q11, q11, d4		@((lTemp1<<1) + lTemp2)
	vmul.i16 q10, q10, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddw.u8 q12, q12, d6		@((lTemp1<<1) + lTemp2)			
	.else
	vshll.u8 q9, d30, #1		@(lTemp1<<1)
	vshll.u8 q10, d2, #1		@(lTemp1<<1)	
	vshll.u8 q11, d4, #1		@(lTemp1<<1)	
	vshll.u8 q12, d6, #1		@(lTemp1<<1)	
	vaddw.u8 q9, q9, d28		@((lTemp1<<1) + lTemp2)	
	vaddw.u8 q10, q10, d30		@((lTemp1<<1) + lTemp2)
	vmul.i16 q9, q9, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddw.u8 q11, q11, d2		@((lTemp1<<1) + lTemp2)	
	vmul.i16 q10, q10, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddw.u8 q12, q12, d4		@((lTemp1<<1) + lTemp2)						
	.endif	
	vmul.i16 q11, q11, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vaddl.u8 q13, d26, d2		@-lTemp0 -lTemp3
	vmul.i16 q12, q12, d1[0]		@6*((lTemp1<<1) + lTemp2)	
	vsub.s16 q9, q9, q13		@result	
@6 line input d28, d30, d2, d4	
	
	vaddl.u8 q14, d28, d4		@-lTemp0 -lTemp3
	vaddl.u8 q15, d30, d6		@-lTemp0 -lTemp3	
	vsub.s16 q10, q10, q14		@result		
@7 line input d30, d2, d4, d6 	
	vaddl.u8 q1, d2, d8		@-lTemp0 -lTemp3
	vsub.s16 q11, q11, q15		@result		
@8 line input d2, d4, d6, d8	
	vsub.s16 q12, q12, q1		@result	
 .endm
		  
 .macro  Interpolate_V01_OR_V02_HV is_V01, is_ADD, round, shif	@input q5~15,  output q1~8			
	.if \is_ADD > 0	
	mov	r12, r1	
	.endif	
											                                    	                                    
@first 4 line												                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4

@1 line input q5, q6, q7, q8	out q1
	.if \is_V01>0
	vshl.s16 q4, q6, #1		@(lTemp1<<1)
	vshl.s16 q2, q7, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q7		@((lTemp1<<1) + lTemp2)
	vmov.s16 d0, #6		
	vadd.s16 q2, q2, q8		@((lTemp1<<1) + lTemp2)		
	.else
	vshl.s16 q4, q7, #1		@(lTemp1<<1)
	vshl.s16 q2, q8, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q6		@((lTemp1<<1) + lTemp2)
	vmov.s16 d0, #6			
	vadd.s16 q2, q2, q7		@((lTemp1<<1) + lTemp2)			
	.endif	
	vmull.s16 q3, d8, d0[0]		@6*((lTemp1<<1) + lTemp2)	
	vmov.s32 q1, #\round		
	vmull.s16 q4, d9, d0[0]		@6*((lTemp1<<1) + lTemp2)	
	vsubw.s16 q3, q3, d10		@-lTemp0
	vsubw.s16 q5, q4, d11		@-lTemp3			
@2 line input q6, q7, q8, q9	out q2
	
	vmull.s16 q4, d4, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d16		@-lTemp0
	vsubw.s16 q5, q5, d17		@-lTemp3	
	vmull.s16 q2, d5, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q4, q4, d12		@-lTemp0
	vsubw.s16 q6, q2, d13		@-lTemp3
	vsubw.s16 q4, q4, d18		@-lTemp0
	vsubw.s16 q6, q6, d19		@-lTemp3
	
	vadd.s32 q3, q3, q1		@+8
	vadd.s32 q5, q5, q1		@+8				
	vadd.s32 q4, q4, q1		@+8
	vadd.s32 q6, q6, q1		@+8
	
	vqshrun.s32 d6, q3, #\shif
	vqshrun.s32 d7, q5, #\shif
	vqshrun.s32 d10, q4, #\shif
	vqshrun.s32 d11, q6, #\shif		
	.if \is_ADD>0	
	vld1.64 {d8 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vqmovn.u16 d4,q3	
	vqmovn.u16 d5,q5	
	vrhadd.u8 d12, d4, d8	
	vrhadd.u8 d13, d5, d9	
	.else
	vqmovn.u16 d12,q3	
	vqmovn.u16 d13,q5	
	.endif	
@3 line input q7, q8, q9, q10	out q1
	.if \is_V01>0
	vshl.s16 q4, q8, #1		@(lTemp1<<1)
	vshl.s16 q2, q9, #1		@(lTemp1<<1)		
	vadd.s16 q4, q4, q9		@((lTemp1<<1) + lTemp2)
	vadd.s16 q2, q2, q10		@((lTemp1<<1) + lTemp2)	
	.else
	vshl.s16 q4, q9, #1		@(lTemp1<<1)
	vshl.s16 q2, q10, #1		@(lTemp1<<1)		
	vadd.s16 q4, q4, q8		@((lTemp1<<1) + lTemp2)	
	vadd.s16 q2, q2, q9		@((lTemp1<<1) + lTemp2)		
	.endif	
	vmull.s16 q3, d8, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vst1.64	{d12}, [r1], r3			
	vst1.64	{d13}, [r1], r3		
	vmull.s16 q5, d9, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d14		@-lTemp0
	vsubw.s16 q5, q5, d15		@-lTemp3			
	
@4 line input q8, q9, q10, q11	out q2
	
	vmull.s16 q4, d4, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d20		@-lTemp0
	vsubw.s16 q5, q5, d21		@-lTemp3	
	vmull.s16 q6, d5, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q4, q4, d16		@-lTemp0
	vsubw.s16 q6, q6, d17		@-lTemp3
	vsubw.s16 q4, q4, d22		@-lTemp0
	vsubw.s16 q6, q6, d23		@-lTemp3			

	vadd.s32 q3, q3, q1		@+8
	vadd.s32 q5, q5, q1		@+8				
	vadd.s32 q4, q4, q1		@+8
	vadd.s32 q6, q6, q1		@+8
	
	vqshrun.s32 d6, q3, #\shif
	vqshrun.s32 d7, q5, #\shif	
	
	vqshrun.s32 d10, q4, #\shif
	vqshrun.s32 d11, q6, #\shif	
	.if \is_ADD>0	
	vld1.64 {d8 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vqmovn.u16 d4,q3	
	vqmovn.u16 d5,q5	
	vrhadd.u8 d12, d4, d8	
	vrhadd.u8 d13, d5, d9	
	.else
	vqmovn.u16 d12,q3	
	vqmovn.u16 d13,q5	
	.endif		
	
@last 4 line						
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4

@5 line input q9, q10, q11, q12	out q1 
	.if \is_V01>0
	vshl.s16 q4, q10, #1		@(lTemp1<<1)
	vshl.s16 q2, q11, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q11		@((lTemp1<<1) + lTemp2)
	vadd.s16 q2, q2, q12		@((lTemp1<<1) + lTemp2)	
	.else
	vshl.s16 q4, q11, #1		@(lTemp1<<1)
	vshl.s16 q2, q12, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q10		@((lTemp1<<1) + lTemp2)	
	vadd.s16 q2, q2, q11		@((lTemp1<<1) + lTemp2)	
	.endif	
	vmull.s16 q3, d8, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vst1.64	{d12}, [r1], r3			
	vst1.64	{d13}, [r1], r3		
	vmull.s16 q5, d9, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d18		@-lTemp0
	vsubw.s16 q5, q5, d19		@-lTemp3			

@6 line input q10, q11, q12, q13 out q2
	
	vmull.s16 q4, d4, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d24		@-lTemp0
	vsubw.s16 q5, q5, d25		@-lTemp3	
	vmull.s16 q6, d5, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q4, q4, d20		@-lTemp0
	vsubw.s16 q6, q6, d21		@-lTemp3
	vsubw.s16 q4, q4, d26		@-lTemp0
	vsubw.s16 q6, q6, d27		@-lTemp3			

	vadd.s32 q3, q3, q1		@+8
	vadd.s32 q5, q5, q1		@+8				
	vadd.s32 q4, q4, q1		@+8
	vadd.s32 q6, q6, q1		@+8
	
	vqshrun.s32 d6, q3, #\shif
	vqshrun.s32 d7, q5, #\shif
	vqshrun.s32 d10, q4, #\shif
	vqshrun.s32 d11, q6, #\shif
	.if \is_ADD>0	
	vld1.64 {d8 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vqmovn.u16 d4,q3	
	vqmovn.u16 d5,q5	
	vrhadd.u8 d12, d4, d8	
	vrhadd.u8 d13, d5, d9	
	.else
	vqmovn.u16 d12,q3	
	vqmovn.u16 d13,q5	
	.endif
	
@7 line input q11, q12, q13, q14 out q1
	.if \is_V01>0
	vshl.s16 q4, q12, #1		@(lTemp1<<1)
	vshl.s16 q2, q13, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q13		@((lTemp1<<1) + lTemp2)
	vadd.s16 q2, q2, q14		@((lTemp1<<1) + lTemp2)	
	.else
	vshl.s16 q4, q13, #1		@(lTemp1<<1)
	vshl.s16 q2, q14, #1		@(lTemp1<<1)	
	vadd.s16 q4, q4, q12		@((lTemp1<<1) + lTemp2)	
	vadd.s16 q2, q2, q13		@((lTemp1<<1) + lTemp2)		
	.endif	
	vmull.s16 q3, d8, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vst1.64	{d12}, [r1], r3			
	vst1.64	{d13}, [r1], r3		
	vmull.s16 q5, d9, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d22		@-lTemp0
	vsubw.s16 q5, q5, d23		@-lTemp3			
		
@8 line input q12, q13, q14, q15 out q2 
	
	vmull.s16 q4, d4, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q3, q3, d28		@-lTemp0
	vsubw.s16 q5, q5, d29		@-lTemp3	
	vmull.s16 q6, d5, d0[0]		@6*((lTemp1<<1) + lTemp2)
	vsubw.s16 q4, q4, d24		@-lTemp0
	vsubw.s16 q6, q6, d25		@-lTemp3
	vsubw.s16 q4, q4, d30		@-lTemp0
	vsubw.s16 q6, q6, d31		@-lTemp3			

	vadd.s32 q3, q3, q1		@+8
	vadd.s32 q5, q5, q1		@+8				
	vadd.s32 q4, q4, q1		@+8
	vadd.s32 q6, q6, q1		@+8
	
	vqshrun.s32 d6, q3, #\shif
	vqshrun.s32 d7, q5, #\shif
	vqshrun.s32 d10, q4, #\shif
	vqshrun.s32 d11, q6, #\shif
	.if \is_ADD>0	
	vld1.64 {d8 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d9 }, [r12]		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vqmovn.u16 d4,q3	
	vqmovn.u16 d5,q5	
	vrhadd.u8 d12, d4, d8	
	vrhadd.u8 d13, d5, d9	
	.else
	vqmovn.u16 d12,q3	
	vqmovn.u16 d13,q5	
	.endif	
	vst1.64	{d12}, [r1], r3			
	vst1.64	{d13}, [r1]		
 .endm
	
 .macro Interpolate_V01_OR_V02_HV_22 is_ADD, round, shif	@input q5~8,q9~12,q13~14  output q1~8 	
@first 4 line												                                    	                                    
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4
@result = lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2)

	vmov.s16 q0, #6	
	
@1 line input q5, q6, q7, q8	out q1
	vmul.i16 q1, q5, q0		@6*lTemp1
	vmov.s16 q5, #\round
	vmul.i16 q2, q6, q0		@6*lTemp1
	vmov.s16 q15, #9					
	vmla.i16 q1, q6, q15		@6*lTemp1 + 9*lTemp2
	.if \is_ADD>0	
	mov	r12, r1
	.endif
	vmla.i16 q2, q7, q15		@6*lTemp1 + 9*lTemp2				
@2 line input q6, q7, q8, q9	out q2		
@3 line input q7, q8, q9, q10	out q3
	vadd.u16 q1, q1, q7		@+ lTemp3  result
	vadd.u16 q2, q2, q8		@+ lTemp3  result
	vmul.i16 q3, q7, q0		@6*lTemp1
	vadd.u16 q1, q1, q5		@+8	
	vmla.i16 q3, q8, q15		@6*lTemp1 + 9*lTemp2		
	vqshrn.u16 d2, q1, #\shif	
	vadd.u16 q3, q3, q9		@+ lTemp3  result	
@4 line input q8, q9, q10, q11	out q4 
	vmul.i16 q4, q8, q0		@6*lTemp1
	vadd.u16 q2, q2, q5		@+8	
	vmla.i16 q4, q9, q15		@6*lTemp1 + 9*lTemp2
	vqshrn.u16 d4, q2, #\shif			
	vadd.u16 q4, q4, q10		@+ lTemp3  result			
	
	vadd.u16 q3, q3, q5		@+8
	vadd.u16 q4, q4, q5		@+8	
	vqshrn.u16 d6, q3, #\shif							
	vqshrn.u16 d8, q4, #\shif	
	
	.if \is_ADD>0	
	vld1.64 {d3 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d5 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d7 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]			
	vrhadd.u8 d2 , d2 , d3 
	vrhadd.u8 d4 , d4 , d5 
	vrhadd.u8 d6 , d6 , d7 
	vrhadd.u8 d8 , d8 , d9 
	.endif	
		
@last 4 line						
@result = (-1*lTemp0 + 6*((lTemp1<<1) + lTemp2) + -1*lTemp3 + 8)>>4

@5 line input q9, q10, q11, q12	out q6
	vmul.i16 q6, q9, q0		@6*lTemp1
	vst1.64	{d2 }, [r1], r3	
	vmul.i16 q7, q10, q0		@6*lTemp1	
	vst1.64	{d4 }, [r1], r3 	
	vmla.i16 q6, q10, q15		@6*lTemp1 + 9*lTemp2
	vst1.64	{d6 }, [r1], r3 	
	vmla.i16 q7, q11, q15		@6*lTemp1 + 9*lTemp2	
	vst1.64	{d8 }, [r1], r3 	
	vmul.i16 q8, q11, q0		@6*lTemp1			
	vadd.u16 q6, q6, q11		@+ lTemp3  result
	vmla.i16 q8, q12, q15		@6*lTemp1 + 9*lTemp2	
	vadd.u16 q7, q7, q12		@+ lTemp3  result
@6 line input q10, q11, q12, q13 out q7
	vadd.u16 q6, q6, q5		@+8	
@7 line input q11, q12, q13, q14 out q8

	vmul.i16 q9, q12, q0		@6*lTemp1		
	vadd.u16 q8, q8, q13		@+ lTemp3  result
	vadd.u16 q7, q7, q5		@+8	
@8 line input q12, q13, q14, q15 out q9 

	vmla.i16 q9, q13, q15		@6*lTemp1 + 9*lTemp2	
	vadd.u16 q8, q8, q5		@+8		
	vadd.u16 q9, q9, q14		@+ lTemp3  result
	vqshrn.u16 d12, q6, #\shif
	vqshrn.u16 d14, q7, #\shif	
	vadd.u16 q9, q9, q5		@+8	
	vqshrn.u16 d16, q8, #\shif
	vqshrn.u16 d18, q9, #\shif	
	
	.if \is_ADD>0	
	vld1.64 {d13 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d15 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d17 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d19 }, [r12]		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]			
	vrhadd.u8 d12 , d12 , d13 
	vrhadd.u8 d14 , d14 , d15 
	vrhadd.u8 d16 , d16 , d17 
	vrhadd.u8 d18 , d18 , d19 
	.endif	
							
	vst1.64	{d12 }, [r1], r3
	vst1.64	{d14 }, [r1], r3 
	vst1.64	{d16 }, [r1], r3 
	vst1.64	{d18 }, [r1] 		
	
 .endm
 .macro	Interpolate_Chroma_H01V00andH02V00 is_ADD, round, shif			
	vld1.8 {d0}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d4}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vld1.8 {d6}, [r0], r2		@d6 lTemp0= pSrc[0~7]	
	
	vmull.u8 q5, d0, d28			@5*lTemp0	
	vext.u8 d20, d0, d1, #1		@lTemp1= pSrc[1~8]
	vmull.u8 q6, d2, d28			@5*lTemp0			    
	vext.u8 d21, d2, d3, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d20, d29		@3*lTemp1
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q6, d21, d29		@3*lTemp1	             
	vext.u8 d23, d6, d7, #1		@lTemp1= pSrc[1~8]	

@result = (5*lTemp0 + 3*lTemp1 + 4)>>3@
						

	vmov.s16 q0, #\round	
	vmull.u8 q7, d4, d28		@5*lTemp0
	vadd.s16 q5, q5, q0		@+8	
	vmull.u8 q8, d6, d28		@5*lTemp0
	vadd.s16 q6, q6, q0		@+8		
	vmlal.u8 q7, d22, d29		@3*lTemp1
	vqshrun.s16 d10, q5, #\shif	
	vmlal.u8 q8, d23, d29		@3*lTemp1
	vqshrun.s16 d11, q6, #\shif
	vadd.s16 q7, q7, q0		@+8
	vadd.s16 q8, q8, q0		@+8					
	vqshrun.s16 d12, q7, #\shif
	vqshrun.s16 d13, q8, #\shif

	.if \is_ADD>0	
	mov	r12, r1	
	vld1.32 {d0[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d1[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d2[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d3[0]}, [r12]		
	vrhadd.u8 d10, d0, d10
	vrhadd.u8 d11, d1, d11
	vrhadd.u8 d12, d2, d12
	vrhadd.u8 d13, d3, d13						
	.endif
			
	vst1.32	{d10[0]}, [r1], r3
	vst1.32	{d11[0]}, [r1], r3 
	vst1.32	{d12[0]}, [r1], r3 
	vst1.32	{d13[0]}, [r1]
				
 .endm
 
 .macro	Interpolate_Chroma_H00V01andH00V02 is_ADD, round, shif			
	vld1.8 {d0}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d4}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vmull.u8 q5, d0, d28			@5*lTemp0	
	vld1.8 {d6}, [r0], r2		@d6 lTemp0= pSrc[0~7]
	vmull.u8 q6, d2, d28			@5*lTemp0		
	vld1.8 {d8}, [r0]		@d6 lTemp0= pSrc[0~7]	

@result = (5*lTemp0 + 3*lTemp1 + 4)>>3@	
	vmlal.u8 q5, d2, d29			@3*lTemp1
	vmov.s16 q0, #\round		
	vmlal.u8 q6, d4, d29			@3*lTemp1
	vadd.s16 q5, q5, q0		@+8
		
	vmull.u8 q7, d4, d28		@5*lTemp0
	vadd.s16 q6, q6, q0		@+8	
	vmull.u8 q8, d6, d28		@5*lTemp0
	vqshrun.s16 d10, q5, #\shif
				
	vmlal.u8 q7, d6, d29		@3*lTemp1
	vqshrun.s16 d11, q6, #\shif	
	vmlal.u8 q8, d8, d29		@3*lTemp1
	
	vadd.s16 q7, q7, q0		@+8
	vadd.s16 q8, q8, q0		@+8					
	
	vqshrun.s16 d12, q7, #\shif
	vqshrun.s16 d13, q8, #\shif

	.if \is_ADD>0	
	mov	r12, r1
	vld1.32 {d0[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d1[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d2[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d3[0]}, [r12]	
		
	vrhadd.u8 d10, d0, d10
	vrhadd.u8 d11, d1, d11
	vrhadd.u8 d12, d2, d12
	vrhadd.u8 d13, d3, d13						
	.endif
			
	vst1.32	{d10[0]}, [r1], r3
	vst1.32	{d11[0]}, [r1], r3 
	vst1.32	{d12[0]}, [r1], r3 
	vst1.32	{d13[0]}, [r1]			
	
 .endm 
	
 .macro	Interpolate_Chroma_HV is_ADD, is_1122, round, shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 25(9) d29 = 15 d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	
	.if \is_1122>0
	vld1.8 {d0}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d4}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vmull.u8 q5, d0, d28	
	vld1.8 {d6}, [r0], r2		@d6 lTemp0= pSrc[0~7]
	vmull.u8 q6, d2, d28	
	vld1.8 {d8}, [r0]		@d6 lTemp0= pSrc[0~7]		
	vmull.u8 q7, d4, d28		
	vext.u8 d20, d0, d1, #1		@lTemp1= pSrc[1~8]
	vmull.u8 q8, d6, d28	
	vext.u8 d21, d2, d3, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d20, d29	
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q6, d21, d29		             
	vext.u8 d23, d6, d7, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d2, d29	
	vext.u8 d24, d8, d9, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d21, d30
	vmov.s16 q0, #\round
	vmlal.u8 q6, d4, d29					
@ 4 line
@1 line input d0, d20, d2, d21
	vadd.s16 q5, q5, q0		@+8	
	vmlal.u8 q6, d22, d30		
	vqshrun.s16 d10, q5, #\shif	
@2 line input d2, d21, d4, d22
	vadd.s16 q6, q6, q0		@+8
	vmlal.u8 q7, d22, d29
	vqshrun.s16 d11, q6, #\shif		
	vmlal.u8 q7, d6, d29	
@3 line input d4, d22, d6, d23	
	vmlal.u8 q7, d23, d30	
	vmlal.u8 q8, d23, d29	
	vadd.s16 q7, q7, q0		@+8
	vmlal.u8 q8, d8, d29
	vqshrun.s16 d12, q7, #\shif			
	vmlal.u8 q8, d24, d30	
	
@4 line input d6, d23, d8, d24
	vadd.s16 q8, q8, q0		@+8					
	vqshrun.s16 d13, q8, #\shif	
	
	.else
	vld1.8 {d0}, [r0], r2		@d0 lTemp0= pSrc[0~7]
	vld1.8 {d2}, [r0], r2		@d2 lTemp0= pSrc[0~7]
	vld1.8 {d4}, [r0], r2		@d4 lTemp0= pSrc[0~7]
	vmull.u8 q5, d0, d29	
	vld1.8 {d6}, [r0], r2		@d6 lTemp0= pSrc[0~7]
	vmull.u8 q6, d2, d29	
	vld1.8 {d8}, [r0]		@d6 lTemp0= pSrc[0~7]
	vmull.u8 q7, d4, d29						
	vext.u8 d20, d0, d1, #1		@lTemp1= pSrc[1~8]
	vmull.u8 q8, d6, d29	
	vext.u8 d21, d2, d3, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d20, d28		    
	vext.u8 d22, d4, d5, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q6, d21, d28		             
	vext.u8 d23, d6, d7, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d2, d30	
	vext.u8 d24, d8, d9, #1		@lTemp1= pSrc[1~8]
	vmlal.u8 q5, d21, d29		
@ 4 line
@1 line input d0, d20, d2, d21		
@2 line input d2, d21, d4, d22
	vmov.s16 q0, #\round
	vmlal.u8 q7, d22, d28			
	vadd.s16 q5, q5, q0		@+8
	vmlal.u8 q6, d4, d30
	vmlal.u8 q7, d6, d30
	vqshrun.s16 d10, q5, #\shif				
	vmlal.u8 q6, d22, d29			
	
@3 line input d4, d22, d6, d23	
	vmlal.u8 q7, d23, d29
	vadd.s16 q6, q6, q0		@+8	
@4 line input d6, d23, d8, d24		
	vmlal.u8 q8, d23, d28
	vadd.s16 q7, q7, q0		@+8	
	vmlal.u8 q8, d8, d30
	vqshrun.s16 d11, q6, #\shif			
	vmlal.u8 q8, d24, d29	
	vqshrun.s16 d12, q7, #\shif	
	vadd.s16 q8, q8, q0		@+8
	vqshrun.s16 d13, q8, #\shif		
	.endif		

	.if \is_ADD>0		
	mov	r12, r1	
	vld1.32 {d0[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d1[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d2[0]}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.32 {d3[0]}, [r12]
		
	vrhadd.u8 d10, d0, d10
	vrhadd.u8 d11, d1, d11
	vrhadd.u8 d12, d2, d12
	vrhadd.u8 d13, d3, d13									
	.endif
					
	vst1.32	{d10[0]}, [r1], r3
	vst1.32	{d11[0]}, [r1], r3 
	vst1.32	{d12[0]}, [r1], r3 
	vst1.32	{d13[0]}, [r1]		
	
 .endm

 .macro Interpolate_ROUNDS round, shif	@input q5~12  output d2, d4, d6~d16
	vmov.s16 q15, #\round		
	vadd.s16 q5, q5, q15		@+8
	vadd.s16 q6, q6, q15		@+8	
	vadd.s16 q7, q7, q15		@+8
	vadd.s16 q8, q8, q15		@+8
	vadd.s16 q9, q9, q15		@+8
	vadd.s16 q10, q10, q15		@+8
	vadd.s16 q11, q11, q15		@+8
	vadd.s16 q12, q12, q15		@+8						
	
	vqshrun.s16 d2, q5, #\shif
	vqshrun.s16 d4, q6, #\shif
	vqshrun.s16 d6, q7, #\shif
	vqshrun.s16 d8, q8, #\shif
	vqshrun.s16 d10, q9, #\shif
	vqshrun.s16 d12, q10, #\shif
	vqshrun.s16 d14, q11, #\shif
	vqshrun.s16 d16, q12, #\shif											
 .endm
		 
@#ifndef ARM_ASM
@/******************************************************************/
@/* ARMV7_Interpolate_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/**************************************************************** */
@void RV_FASTCALL  C_Interpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

	.global ARMV7_Interpolate_H00V00 
ARMV7_Interpolate_H00V00:  @PROC

	VLD1.8  {d0},[r0],r2  
	VLD1.8  {d1},[r0],r2  
	VLD1.8  {d2},[r0],r2  
	VLD1.8  {d3},[r0],r2  
	VLD1.8  {d4},[r0],r2  
	VLD1.8  {d5},[r0],r2  
	VLD1.8  {d6},[r0],r2  
	VLD1.8  {d7},[r0] 
		 
	VST1.64   {d0},[r1],r3 
	VST1.64   {d1},[r1],r3 
	VST1.64   {d2},[r1],r3 
	VST1.64   {d3},[r1],r3	
	VST1.64   {d4},[r1],r3 
	VST1.64   {d5},[r1],r3 
	VST1.64   {d6},[r1],r3 
	VST1.64   {d7},[r1]	 		
	
	mov	pc, lr
	@ENDP


@/*******************************************************************/
@/* C_Interpolate_H01V00 
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/********************************************************************/
@void RV_FASTCALL  C_Interpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H01V00 
ARMV7_Interpolate_H01V00:  @PROC
		
	sub	r0, r0, #1		
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                    
	
	vst1.64	{d2}, [r1], r3
	vst1.64	{d4}, [r1], r3 
	vst1.64	{d6}, [r1], r3 
	vst1.64	{d8}, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1] 
								              	 		
	mov	pc, lr                      
	@ENDP                                
                                            
@/******************************************************************/
@/* C_Interpolate_H02V00                     
@/*	2/3 pel horizontal displacement     
@/*	0 vertical displacement             
@/*	Use horizontal filter (-1,6,12,-1)  
@/******************************************************************/
@void RV_FASTCALL  C_Interpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

	.global ARMV7_Interpolate_H02V00 
ARMV7_Interpolate_H02V00:  @PROC
		
	sub	r0, r0, #1		
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 0

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                    
	
	vst1.64	{d2}, [r1], r3
	vst1.64	{d4}, [r1], r3 
	vst1.64	{d6}, [r1], r3 
	vst1.64	{d8}, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1] 
								              	 		
	mov	pc, lr                      
	@ENDP  
@/*******************************************************************************/
@/* C_Interpolate_H00V01 
@/*	0 horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (-1,12,6,-1) 
@/******************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  C_Interpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H00V01 
ARMV7_Interpolate_H00V01:  @PROC
		
	sub	r0, r0, r2
@ 	Interpolate_V01_OR_V02 $is_V01	@input q9,q10,q11,q12,q13~15,q1~4,  output q5~q8,q9~12	
	Interpolate_V01_OR_V02 1

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                    
	
	vst1.64	{d2}, [r1], r3
	vst1.64	{d4}, [r1], r3 
	vst1.64	{d6}, [r1], r3 
	vst1.64	{d8}, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1] 
								              	 		
	mov	pc, lr                      
	@ENDP  


@/****************************************************************************/
@/* C_Interpolate_H01V01 
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,12,6,-1) 
@/***************************************************************************/
@void RV_FASTCALL  C_Interpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H01V01 
ARMV7_Interpolate_H01V01:  @PROC
@Horizontal					
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 1	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 1, 0, 128, 8							              	 		
	mov	pc, lr                      
	@ENDP  
	            

@/************************************************************************/
@/* C_Interpolate_H02V01 
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1)
@/*	Use vertical filter (-1,12,6,-1) 
@/************************************************************************/
@void RV_FASTCALL  C_Interpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H02V01 
ARMV7_Interpolate_H02V01:  @PROC
@Horizontal					  
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 0
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 0	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 1, 0, 128, 8							              	 		
	mov	pc, lr                      
	@ENDP 	

@/******************************************************************************/
@/* C_Interpolate_H00V02 
@/*	0 horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (-1,6,12,-1) 
@/*****************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  C_Interpolate_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H00V02 
ARMV7_Interpolate_H00V02:  @PROC
		
	sub	r0, r0, r2
@ 	Interpolate_V01_OR_V02 $is_V01	@input q9,q10,q11,q12,q13~15,q1~4,  output q5~q8,q9~12	
	Interpolate_V01_OR_V02 0

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                      
	
	vst1.64	{d2}, [r1], r3
	vst1.64	{d4}, [r1], r3 
	vst1.64	{d6}, [r1], r3 
	vst1.64	{d8}, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1] 
								              	 		
	mov	pc, lr                      
	@ENDP  

@/*******************************************************************/
@/* C_Interpolate_H01V02 
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,6,12,-1) 
@/********************************************************************/
@void RV_FASTCALL  C_Interpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H01V02 
ARMV7_Interpolate_H01V02:  @PROC
@Horizontal												              	 		
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 1	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 0, 0, 128, 8							              	 		
	mov	pc, lr                      
	@ENDP 	

@/****************************************************************/
@/* C_Interpolate_H02V02 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (6,9,1) 
@/*	Use vertical filter (6,9,1) 
@/****************************************************************/
@void RV_FASTCALL  C_Interpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_Interpolate_H02V02 
ARMV7_Interpolate_H02V02:  @PROC
@Horizontal					
@	sub	r0, r0, #1
@	sub	r0, r0, r2			
	
  @Interpolate_H01_OR_H02_22	@output q5~8,q9~12,q13~14
	Interpolate_H01_OR_H02_22
				
  @Vertical	input q5~8,q9~12,q13~14	10 lines
		
  @Interpolate_V01_OR_V02_HV_22 $is_ADD, $round, $shif	@input q5~8,q9~12,q13~14  output q1~8
	Interpolate_V01_OR_V02_HV_22 0, 128, 8
								              	 		
	mov	pc, lr                      
	@ENDP  


@/******************************************************************/
@/* C_AddInterpolate_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/**************************************************************** */
@void RV_FASTCALL  C_AddInterpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H00V00 
ARMV7_AddInterpolate_H00V00:  @PROC
	mov	r12, r1	  
	VLD1.8  {d0},[r0],r2
	VLD1.8  {d1},[r0],r2
	VLD1.8  {d2},[r0],r2
	VLD1.8  {d3},[r0],r2
	VLD1.8  {d8},[r0],r2
	VLD1.8  {d9},[r0],r2
	VLD1.8  {d10},[r0],r2
	VLD1.8  {d11},[r0]	

	VLD1.64  {d4},[r1],r3
	VLD1.64  {d5},[r1],r3
	VLD1.64  {d6},[r1],r3
	VLD1.64  {d7},[r1],r3 
	VLD1.64  {d12},[r1],r3
	VLD1.64  {d13},[r1],r3
	VLD1.64  {d14},[r1],r3
	VLD1.64  {d15},[r1]	
	  
	VRHADD.U8 d0, d0, d4
	VRHADD.U8 d1, d1, d5
	VRHADD.U8 d2, d2, d6
	VRHADD.U8 d3, d3, d7	
	VRHADD.U8 d8, d8, d12
	VRHADD.U8 d9, d9, d13
	VRHADD.U8 d10, d10, d14
	VRHADD.U8 d11, d11, d15	    
	
	VST1.64   {d0},[r12],r3
	VST1.64   {d1},[r12],r3
	VST1.64   {d2},[r12],r3
	VST1.64   {d3},[r12],r3 
	VST1.64   {d8},[r12],r3
	VST1.64   {d9},[r12],r3
	VST1.64   {d10},[r12],r3
	VST1.64   {d11},[r12] 	 	
	mov	pc, lr
	@ENDP	
@/*******************************************************************/
@/* C_AddInterpolate_H01V00 
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/********************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H01V00 
ARMV7_AddInterpolate_H01V00:  @PROC
	
	sub	r0, r0, #1		
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                    

	mov	r12, r1
	
	vld1.8 {d3 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.8 {d5 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d7 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d11}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d13}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d15}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.8 {d17}, [r12]			@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
		
		
	vrhadd.u8 d2 , d2 , d3 
	vrhadd.u8 d4 , d4 , d5 
	vrhadd.u8 d6 , d6 , d7 
	vrhadd.u8 d8 , d8 , d9 
	vrhadd.u8 d10, d10, d11
	vrhadd.u8 d12, d12, d13
	vrhadd.u8 d14, d14, d15
	vrhadd.u8 d16, d16, d17							
		
	vst1.64	{d2 }, [r1], r3
	vst1.64	{d4 }, [r1], r3 
	vst1.64	{d6 }, [r1], r3 
	vst1.64	{d8 }, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1]   
								              	 		
	mov	pc, lr                      
	@ENDP 
@/******************************************************************/
@/* C_AddInterpolate_H02V00 
@/*	2/3 pel horizontal displacement
@/*	0 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1) 
@/******************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

	.global ARMV7_AddInterpolate_H02V00 
ARMV7_AddInterpolate_H02V00:  @PROC
	
	sub	r0, r0, #1		
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 0

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                    

	mov	r12, r1
	
	vld1.64 {d3 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d5 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d7 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d11}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d13}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d15}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d17}, [r12]			@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
		
		
	vrhadd.u8 d2 , d2 , d3 
	vrhadd.u8 d4 , d4 , d5 
	vrhadd.u8 d6 , d6 , d7 
	vrhadd.u8 d8 , d8 , d9 
	vrhadd.u8 d10, d10, d11
	vrhadd.u8 d12, d12, d13
	vrhadd.u8 d14, d14, d15
	vrhadd.u8 d16, d16, d17							
		
	vst1.64	{d2 }, [r1], r3
	vst1.64	{d4 }, [r1], r3 
	vst1.64	{d6 }, [r1], r3 
	vst1.64	{d8 }, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1]   
								              	 		
	mov	pc, lr                      
	@ENDP
@/*******************************************************************************/
@/* C_AddInterpolate_H00V01 
@/*	0 horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (-1,12,6,-1) 
@/******************************************************************************/
@#pragma optimize( "", off)
@void RV_FASTCALL  C_AddInterpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

	.global ARMV7_AddInterpolate_H00V01 
ARMV7_AddInterpolate_H00V01:  @PROC
		
	sub	r0, r0, r2
@ 	Interpolate_V01_OR_V02 $is_V01	@input q9,q10,q11,q12,q13~15,q1~4,  output q5~q8,q9~12	
	Interpolate_V01_OR_V02 1

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                      

	mov	r12, r1
	
	vld1.64 {d3 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d5 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d7 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d11}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d13}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d15}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d17}, [r12]			@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
		
		
	vrhadd.u8 d2 , d2 , d3 
	vrhadd.u8 d4 , d4 , d5 
	vrhadd.u8 d6 , d6 , d7 
	vrhadd.u8 d8 , d8 , d9 
	vrhadd.u8 d10, d10, d11
	vrhadd.u8 d12, d12, d13
	vrhadd.u8 d14, d14, d15
	vrhadd.u8 d16, d16, d17							
		
	vst1.64	{d2 }, [r1], r3
	vst1.64	{d4 }, [r1], r3 
	vst1.64	{d6 }, [r1], r3 
	vst1.64	{d8 }, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1]   
								              	 		
	mov	pc, lr                      
	@ENDP
@/****************************************************************************/
@/* C_AddInterpolate_H01V01 
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,12,6,-1) 
@/***************************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H01V01 
ARMV7_AddInterpolate_H01V01:  @PROC
@Horizontal	
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 1	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 1, 1, 128, 8		  							              	 		
	mov	pc, lr                      
	@ENDP	

@/************************************************************************/
@/* C_AddInterpolate_H02V01 
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (-1,6,12,-1)
@/*	Use vertical filter (-1,12,6,-1) 
@/************************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H02V01 
ARMV7_AddInterpolate_H02V01:  @PROC
@Horizontal
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 0
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 0	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 1, 1, 128, 8  								              	 		
	mov	pc, lr                      
	@ENDP	


@/******************************************************************************/
@/* C_AddInterpolate_H00V02 
@/*	0 horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (-1,6,12,-1) 
@/*****************************************************************************/
@#pragma optimize( "", off)C_AddInterpolate_H00V02
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
	.global ARMV7_AddInterpolate_H00V02 
ARMV7_AddInterpolate_H00V02:  @PROC
		
	sub	r0, r0, r2
@ 	Interpolate_V01_OR_V02 $is_V01	@input q9,q10,q11,q12,q13~15,q1~4,  output q5~q8,q9~12	
	Interpolate_V01_OR_V02 0

@	Interpolate_ROUNDS $round, $shif	@input q5~12  output d2, d4, d6~d16 
	Interpolate_ROUNDS 8, 4	                                      

	mov	r12, r1
	
	vld1.64 {d3 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
	vld1.64 {d5 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d7 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d9 }, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d11}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d13}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d15}, [r12], r3		@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]
	vld1.64 {d17}, [r12]			@d0 lTemp0= pSrc[0~7] d1 = pSrc[8~15]	
		
		
	vrhadd.u8 d2 , d2 , d3 
	vrhadd.u8 d4 , d4 , d5 
	vrhadd.u8 d6 , d6 , d7 
	vrhadd.u8 d8 , d8 , d9 
	vrhadd.u8 d10, d10, d11
	vrhadd.u8 d12, d12, d13
	vrhadd.u8 d14, d14, d15
	vrhadd.u8 d16, d16, d17							
		
	vst1.64	{d2 }, [r1], r3
	vst1.64	{d4 }, [r1], r3 
	vst1.64	{d6 }, [r1], r3 
	vst1.64	{d8 }, [r1], r3 
	vst1.64	{d10}, [r1], r3 
	vst1.64	{d12}, [r1], r3 
	vst1.64	{d14}, [r1], r3 
	vst1.64	{d16}, [r1]   
								              	 		
	mov	pc, lr                      
	@ENDP

@/*******************************************************************/
@/* C_AddInterpolate_H01V02 
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (-1,12,6,-1) 
@/*	Use vertical filter (-1,6,12,-1) 
@/********************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H01V02 
ARMV7_AddInterpolate_H01V02:  @PROC
@Horizontal					
	sub	r0, r0, #1
	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02 $is_H01	@input q1~4,  output q5~8,q9~12
	Interpolate_H01_OR_H02 1
	
@ 	Interpolate_H01_OR_H02_HV $is_H01	@input q1~3,  output q13~15
	Interpolate_H01_OR_H02_HV 1	
				
@Vertical	input q5~8,q9~12, q13, q14, q15,	11 lines
		
@ 	Interpolate_V01_OR_V02_HV $is_V01, $is_ADD, $round, $shif @input q5~15,  output q1~8
	Interpolate_V01_OR_V02_HV 0, 1, 128, 8
								              	 		
	mov	pc, lr                      
	@ENDP

@/****************************************************************/
@/* C_AddInterpolate_H02V02 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (6,9,1) 
@/*	Use vertical filter (6,9,1) 
@/****************************************************************/
@void RV_FASTCALL  C_AddInterpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddInterpolate_H02V02 
ARMV7_AddInterpolate_H02V02:  @PROC
@Horizontal					
@	sub	r0, r0, #1
@	sub	r0, r0, r2			
@ 	Interpolate_H01_OR_H02_22	@output q5~8,q9~12,q13~14
	Interpolate_H01_OR_H02_22
	
				
@Vertical	input q5~8,q9~12,q13~14	10 lines
		
@ 	Interpolate_V01_OR_V02_HV_22 $is_ADD, $round, $shif	@input q5~8,q9~12,q13~14  output q1~8
	Interpolate_V01_OR_V02_HV_22 1, 128, 8  
								              	 		
	mov	pc, lr                      
	@ENDP


@/* chroma functions */
@/* Block size is 4x4 for all. */
@
@/******************************************************************/
@/* C_MCCopyChroma_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/*******************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

	.global ARMV7_MCCopyChroma_H00V00 
ARMV7_MCCopyChroma_H00V00:  @PROC
	VLD1.8  {d0},[r0],r2  
	VLD1.8  {d1},[r0],r2  
	VLD1.8  {d2},[r0],r2  
	VLD1.8  {d3},[r0]  
  
	VST1.32   {d0[0]},[r1],r3 
	VST1.32   {d1[0]},[r1],r3 
	VST1.32   {d2[0]},[r1],r3 
	VST1.32   {d3[0]},[r1]		 		
	mov	pc, lr
	@ENDP
@/*********************************************************************/
@/* C_MCCopyChroma_H01V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H01V00 
ARMV7_MCCopyChroma_H01V00:  @PROC
	vmov.s8 d28, #5	
	vmov.s8 d29, #3	
	
@	Interpolate_Chroma_H01V00andH02V00 $is_ADD, $round, $shif

	Interpolate_Chroma_H01V00andH02V00 0, 4, 3	
								              	 		
	mov	pc, lr                      
	@ENDP     	

@/******************************************************************/
@/* C_MCCopyChroma_H02V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H02V00 
ARMV7_MCCopyChroma_H02V00:  @PROC
	vmov.s8 d28, #3	
	vmov.s8 d29, #5
	
@	Interpolate_Chroma_H01V00andH02V00 $is_ADD, $round, $shif

	Interpolate_Chroma_H01V00andH02V00 0, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP 
@/******************************************************************/
@/* C_MCCopyChroma_H00V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H00V01 
ARMV7_MCCopyChroma_H00V01:  @PROC
	vmov.s8 d28, #5	
	vmov.s8 d29, #3	
	
@	Interpolate_Chroma_H00V01andH00V02 $is_ADD, $round, $shif
	Interpolate_Chroma_H00V01andH00V02 0, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP
	

@/*****************************************************************/
@/* C_MCCopyChroma_H00V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/*****************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H00V02 
ARMV7_MCCopyChroma_H00V02:  @PROC
	vmov.s8 d28, #3	
	vmov.s8 d29, #5	
	
@	Interpolate_Chroma_H00V01andH00V02 $is_ADD, $round, $shif
	Interpolate_Chroma_H00V01andH00V02 0, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP 	

@/******************************************************************/
@/* C_MCCopyChroma_H01V01 
@/*	Motion compensated chroma 4x4 block copy.
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/*******************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H01V01 
ARMV7_MCCopyChroma_H01V01:  @PROC
	vmov.s8 d28, #25	
	vmov.s8 d29, #15
	vmov.s8 d30, #9		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 0, 1, 32, 6	
								              	 		
	mov	pc, lr                      
	@ENDP 
@/*****************************************************************/
@/* C_MCCopyChroma_H02V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H02V01 
ARMV7_MCCopyChroma_H02V01:  @PROC
	vmov.s8 d28, #25	
	vmov.s8 d29, #15
	vmov.s8 d30, #9		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 0, 0, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP 

@/**********************************************************************/
@/* C_MCCopyChroma_H01V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H01V02 
ARMV7_MCCopyChroma_H01V02:  @PROC
	vmov.s8 d28, #9	
	vmov.s8 d29, #15
	vmov.s8 d30, #25		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 0, 0, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP 

@/*******************************************************************/
@/* C_MCCopyChroma_H02V02 
@/*	Motion compensated 4x4 chroma block copy. 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  C_MCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
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
	.global ARMV7_MCCopyChroma_H02V02 
ARMV7_MCCopyChroma_H02V02:  @PROC
	vmov.s8 d28, #9	
	vmov.s8 d29, #15
	vmov.s8 d30, #25		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 0, 1, 32, 6 
								              	 		
	mov	pc, lr                      
	@ENDP 

@/******************************************************************/
@/* C_AddMCCopyChroma_H00V00 
@/*	 0 horizontal displacement 
@/*	 0 vertical displacement 
@/*	 No interpolation required, simple block copy. 
@/*******************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H00V00 
ARMV7_AddMCCopyChroma_H00V00:  @PROC
	mov	r12, r1	  
	VLD1.8  {d0},[r0],r2
	VLD1.8  {d1},[r0],r2
	VLD1.8  {d2},[r0],r2
	VLD1.8  {d3},[r0]	

	VLD1.64  {d4},[r1],r3
	VLD1.64  {d5},[r1],r3
	VLD1.64  {d6},[r1],r3
	VLD1.64  {d7},[r1]	
	  
	VRHADD.U8 d0, d0, d4
	VRHADD.U8 d1, d1, d5
	VRHADD.U8 d2, d2, d6
	VRHADD.U8 d3, d3, d7    
	
	VST1.32   {d0[0]},[r12],r3 
	VST1.32   {d1[0]},[r12],r3 
	VST1.32   {d2[0]},[r12],r3 
	VST1.32   {d3[0]},[r12]	 	
	mov	pc, lr
	@ENDP	
@/*********************************************************************/
@/* C_AddMCCopyChroma_H01V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H01V00 
ARMV7_AddMCCopyChroma_H01V00:  @PROC
	vmov.s8 d28, #5	
	vmov.s8 d29, #3	
	
@	Interpolate_Chroma_H01V00andH02V00 $is_ADD, $round, $shif

	Interpolate_Chroma_H01V00andH02V00 1, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP     

@/******************************************************************/
@/* C_AddMCCopyChroma_H02V00 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	0 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H02V00 
ARMV7_AddMCCopyChroma_H02V00:  @PROC
	vmov.s8 d28, #3	
	vmov.s8 d29, #5	
	
@	Interpolate_Chroma_H01V00andH02V00 $is_ADD, $round, $shif

	Interpolate_Chroma_H01V00andH02V00 1, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP   
@/******************************************************************/
@/* C_AddMCCopyChroma_H00V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H00V01 
ARMV7_AddMCCopyChroma_H00V01:  @PROC
	vmov.s8 d28, #5	
	vmov.s8 d29, #3	
	
@	Interpolate_Chroma_H00V01andH00V02 $is_ADD, $round, $shif
	Interpolate_Chroma_H00V01andH00V02 1, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP 	
@/*****************************************************************/
@/* C_AddMCCopyChroma_H00V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	0 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/*****************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H00V02 
ARMV7_AddMCCopyChroma_H00V02:  @PROC
	vmov.s8 d28, #3	
	vmov.s8 d29, #5	
	
@	Interpolate_Chroma_H00V01andH00V02 $is_ADD, $round, $shif
	Interpolate_Chroma_H00V01andH00V02 1, 4, 3
								              	 		
	mov	pc, lr                      
	@ENDP  	

@/******************************************************************/
@/* C_AddMCCopyChroma_H01V01 
@/*	Motion compensated chroma 4x4 block copy.
@/*	1/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/*******************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H01V01 
ARMV7_AddMCCopyChroma_H01V01:  @PROC
	vmov.s8 d28, #25	
	vmov.s8 d29, #15
	vmov.s8 d30, #9		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 1, 1, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP

@/*****************************************************************/
@/* C_AddMCCopyChroma_H02V01 
@/*	Motion compensated 4x4 chroma block copy.
@/*	2/3 pel horizontal displacement 
@/*	1/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (5,3) 
@/*	Dst pitch is uDstPitch. 
@/******************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H02V01 
ARMV7_AddMCCopyChroma_H02V01:  @PROC
	vmov.s8 d28, #25	
	vmov.s8 d29, #15
	vmov.s8 d30, #9			
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 1, 0, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP

@/**********************************************************************/
@/* C_AddMCCopyChroma_H01V02 
@/*	Motion compensated 4x4 chroma block copy.
@/*	1/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (5,3) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/**********************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H01V02 
ARMV7_AddMCCopyChroma_H01V02:  @PROC
	vmov.s8 d28, #9	
	vmov.s8 d29, #15
	vmov.s8 d30, #25	
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 1, 0, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP

@/*******************************************************************/
@/* C_AddMCCopyChroma_H02V02 
@/*	Motion compensated 4x4 chroma block copy. 
@/*	2/3 pel horizontal displacement 
@/*	2/3 vertical displacement 
@/*	Use horizontal filter (3,5) 
@/*	Use vertical filter (3,5) 
@/*	Dst pitch is uDstPitch. 
@/********************************************************************/
@void RV_FASTCALL  C_AddMCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
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
	.global ARMV7_AddMCCopyChroma_H02V02 
ARMV7_AddMCCopyChroma_H02V02:  @PROC
	vmov.s8 d28, #9	
	vmov.s8 d29, #15
	vmov.s8 d30, #25		
	
@	Interpolate_Chroma_HV $is_ADD, $is_1122, $round, $shif 
@d28 = 25(9) d29 = 15 d30 = 9(25)	
@(1,1) fi,j = (25pi,j + 15pi+1,j + 15pi,j+1 + 9pi+1,j+1 + 32) >> 6 	25, 15, 15, 9
@(2,2) fi,j = (9pi,j + 15pi+1,j + 15pi,j+1 + 25pi+1,j+1 + 32) >> 6 	9, 15, 15, 25

@d28 = 15 d29 = 25(9) d30 = 9(25)
@(1,2) fi,j = (15pi,j + 9pi+1,j + 25pi,j+1 + 15pi+1,j+1 + 32) >> 6 	15, 25, 9, 15
@(2,1) fi,j = (15pi,j + 25pi+1,j + 9i, j+1 + 15pi+1,j+1 + 32) >> 6 	15, 9, 25, 15
	Interpolate_Chroma_HV 1, 1, 32, 6
								              	 		
	mov	pc, lr                      
	@ENDP


