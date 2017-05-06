#include "voMP3DecID.h"
	.text
	.align 4
	.globl	_voMP3DecSynthMono

_voMP3DecSynthMono: @PROC
	stmdb                    sp!, {r4 - r11, lr}
	
	mov			 r10, #4096
	add			 r3, r1, #32
	vdup.32		 	 q11, r10
	   
	vld2.16    	 	 {d16, d17, d18, d19}, [r2] 		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@	
	vld1.16			 {d0, d1}, [r1]					@ vLo = *(vb1+(x))@
	vld1.16			 {d2, d3}, [r3]					@ vHi = *(vb1+(23-(x)))@
	vrev64.16  	 	 q1, q1
		
	vmull.s16	 	 q15, d0, d16						@ sum1L += vLo * c1@
	vmlsl.s16  	 	 q15, d3, d18						@ sum1L -= vHi * c2@
	vmlal.s16	 	 q15, d1, d17						@ sum1L += vLo * c1@
	vmlsl.s16  	 	 q15, d2, d19						@ sum1L -= vHi * c2@
	
  	vadd.s32		 d0, d30, d31
 
	add			 r6, r2, #512 
	vmov			 r10, r11, d0	
 	add			 r7, r1, #2048
  
  	vld1.i16	 	 {d16, d17}, [r6]
	add			 r12, r10, r11	
	
	vld1.16			 {d0, d1}, [r7]
	add			 r12, r12, #4096
	
	vmull.s16		 q15, d0, d16							@sum1L += vLo * c1@
	ssat			 r8, #16, r12, asr #13 
		
	vmlal.s16		 q15, d1, d17							@sum1L += vLo * c1@
	strh                     r8, [r0, #0] 
	
 	vadd.s32		 d0, d30, d31

 	add			 r2, r2, #32
	vmov			 r10, r11, d0	
  	mov     	         r6, #15

	add			 r1, r1, #128		
	add			 r12, r10, r11
	add			 r7, r0, #2

	add			 r12, r12, #4096
	add			 r3, r3, #128
	ssat			 r8, #16, r12, asr #13
	strh    	    	 r8, [r0, #32]

M493:
	vld2.i16		 {d16, d17, d18, d19}, [r2]! 			@c1 = *coef@		coef++@		c2 = *coef@		coef++@
	vld1.i16		 {d0, d1}, [r1]
	vld1.i16		 {d2, d3}, [r3]
	vmull.s16		 q14, d0, d16							@sum1L += vLo * c1@
	vrev64.i16	 	 q1, q1
	
	vmull.s16		 q15, d0, d18							@sum2L += vLo * c2@
	vmlsl.s16		 q14, d3, d18		  				@sum1L -= vHi * c2
	vmlal.s16		 q15, d3, d16							@sum2L += vHi * c1@
	vmlal.s16		 q14, d1, d17							@sum1L += vLo * c1@   	
	vmlal.s16		 q15, d1, d19							@sum2L += vLo * c2@
  	vmlsl.s16		 q14, d2, d19							@sum1L -= vHi * c2
  	vmlal.s16		 q15, d2, d17							@sum2L += vHi * c1 
  
  	vadd.s32		 d28, d28, d29
  	vadd.s32		 d29, d30, d31
  	  
  	vpadd.s32		 d0, d28, d29
  
  	add			 r3, r3, #128
  	vadd.s32		 d2, d0, d22
  	mov     		 r8, r6, lsl #2 
  	vqshrn.s32		 d0, q1, #13
  
  	add			 r8, r7, r8
  	add			 r1, r1, #128

	vst1.i16     	 	 {d0[0]}, [r7]    
			   
	subs     		 r6, r6, #1 
	add     		 r7, r7, #2 
	vst1.i16     	 	 {d0[1]}, [r8]
	bne     		 M493

M494:
	ldmia                    sp!, {r4 - r11, pc}

	@ENdP


	.globl	_voMP3DecSynthStereo
	@AREA	.text, COdE, REAdONLY

_voMP3DecSynthStereo: @PROC
	stmdb   sp!, {r4 - r11, lr}
	
	mov			 r10, #4096
	add			 r3, r1, #32
	add			 r4, r1, #64
	add			 r5, r1, #96
	vdup.32		 	 q11, r10
	   
	vld2.16    	 	 {d16, d17, d18, d19}, [r2] 			                @ c1 = *coef@		coef++@		c2 = *coef@		coef++@	
	vld1.16		 	 {d0, d1}, [r1]							@ vLo = *(vb1+(x))@
	vld1.16		 	 {d2, d3}, [r3]							@ vHi = *(vb1+(23-(x)))@
	vld1.16		 	 {d4, d5}, [r4]							@ vLo = *(vb1+32+(x))@
	vld1.16		 	 {d6, d7}, [r5]							@ vHi = *(vb1+32+(23-(x)))@	
	vrev64.16  	 	 q1, q1
	vrev64.16  		 q3, q3
		
	vmull.s16	 	 q14, d0, d16							@sum1L += vLo * c1@
	vmull.s16  	 	 q15, d4, d16							@sum1R += vLo * c1@
	vmlsl.s16  	 	 q14, d3, d18							@sum1L -= vHi * c2@
	vmlsl.s16  	 	 q15, d7, d18							@sum1R -= vHi * c2@
	vmlal.s16	 	 q14, d1, d17							@sum1L += vLo * c1@
	vmlal.s16  	 	 q15, d5, d17							@sum1R += vLo * c1@
	vmlsl.s16  	 	 q14, d2, d19							@sum1L -= vHi * c2@
	vmlsl.s16	 	 q15, d6, d19							@sum1R -= vHi * c2@
	
  	vadd.s32		 d28, d28, d29
  	vadd.s32		 d29, d30, d31
 
	add			 r6, r2, #512 
	vpadd.s32		 d28, d28, d29	
  	add			 r7, r1, #2048
  
  	vld1.16			 {d16, d17}, [r6]
	vadd.s32		 d28, d28, d22
	add			 r8, r4, #2048
	
	vld1.16			 {d0, d1}, [r7]
	vqshrn.s32	 	 d4, q14, #13		
	
	vld1.16			 {d2, d3}, [r8]
	vst1.I32   		 {d4[0]}, [r0] 

	vmull.s16		 q14, d0, d16							@sum1L += vLo * c1@
	vmull.s16		 q15, d2, d16							@sum1R += vLo * c1@
	vmlal.s16		 q14, d1, d17							@sum1L += vLo * c1@
	vmlal.s16		 q15, d3, d17							@sum1R += vLo * c1@
	
	vadd.s32		 d28, d28, d29
  	vadd.s32		 d29, d30, d31
  
  	add			 r2, r2, #32
  	vpadd.s32		 d28, d28, d29
  	mov     		 r6, #15
        mov			 r9, #128

	vadd.s32		 d4, d28, d22	
	add			 r1, r1, r9
	add			 r7, r0, #4
		
	vqshrn.s32	 	 d4, q2, #13	
	add			 r4, r4, r9
	add			 r0, r0, #64
	add			 r5, r5, r9
	add			 r3, r3, r9
	
	vst1.I32   		 {d4[0]}, [r0]
M1493:
	vld2.i16		 {d16, d17, d18, d19}, [r2]! 	@c1 = *coef@		coef++@		c2 = *coef@		coef++@
	vld1.i16		 {d0, d1}, [r1], r9
	vld1.i16		 {d2, d3}, [r3], r9
	vld1.i16		 {d4, d5}, [r4], r9
	vld1.i16		 {d6, d7}, [r5], r9

	vmull.s16		 q12, d0, d16					@sum1L += vLo * c1@
	vrev64.i16	 	 q1, q1
	
  	vmull.s16		 q13, d4, d16					@sum1R += vLo * c1@	
    	vrev64.i16	 	 q3, q3  	
    
	vmull.s16		 q14, d0, d18					@sum2L += vLo * c2@
  	vmull.s16		 q15, d4, d18	  				@sum2R += vLo * c2@  
  		
	vmlsl.s16		 q12, d3, d18		  			@sum1L -= vHi * c2	
  	vmlsl.s16		 q13, d7, d18					@sum1R -= vHi * c2  
  		
	vmlal.s16		 q14, d3, d16					@sum2L += vHi * c1@  
  	vmlal.s16		 q15, d7, d16					@sum2R += vHi * c1@	
  	
  	vmlal.s16		 q12, d1, d17					@sum1L += vLo * c1@  
  	vmlal.s16		 q13, d5, d17					@sum1R += vLo * c1@ 
  	  	
  	vmlal.s16		 q14, d1, d19					@sum2L += vLo * c2@  
  	vmlal.s16		 q15, d5, d19	  				@sum2R += vLo * c2@
  	  	
  	vmlsl.s16		 q12, d2, d19					@sum1L -= vHi * c2
  	vmlsl.s16		 q13, d6, d19					@sum1R -= vHi * c2  
  		
  	vmlal.s16		 q14, d2, d17					@sum2L += vHi * c1  
  	vmlal.s16		 q15, d6, d17					@sum2R += vHi * c1@
  
  	vadd.s32		 d24, d24, d25  
  	vadd.s32		 d25, d26, d27  	
  	vadd.s32		 d28, d28, d29
  	vadd.s32		 d29, d30, d31
  
  	vpadd.s32		 d0, d24, d25
  	vpadd.s32		 d1, d28, d29
  
  	mov     		 r8, r6, lsl #3 
  	@add			 r3, r3, #128
  	vadd.s32		 q1, q0, q11
  	add			 r8, r7, r8
  	vqshrn.s32	 	 d0, q1, #13
  
  	@add			 r1, r1, #128

	@add			 r4, r4, #128
	@add			 r5, r5, #128
	
	subs     		 r6, r6, #1 
	vst1.I32     		 {d0[0]}, [r7]!            
	vst1.I32       	 	 {d0[1]}, [r8] 
	@add     		 r7, r7, #4 
	bne     		 M1493

	ldmia   sp!, {r4 - r11, pc}

	@ENdP
	@.ENd
        
