#include "voMP3DecID.h"
	.section .text
	.global	voMP3DecSynthMono

voMP3DecSynthMono: @PROC
	stmdb   sp!, {r4 - r11, lr}
	
	mov				 r10, #4096
	add				 r3, r1, #32
	VDUP.32		 	 Q11, r10
	   
	VLD2.16    	 	 {D16, D17, D18, D19}, [r2] 		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@	
	VLD1.16				 {D0, D1}, [r1]					@ vLo = *(vb1+(x))@
	VLD1.16				 {D2, D3}, [r3]					@ vHi = *(vb1+(23-(x)))@
	VREV64.16  	 	 Q1, Q1
		
	VMULL.S16	 	   Q15, D0, D16						@ sum1L += vLo * c1@
	VMLSL.S16  	 	 Q15, D3, D18						@ sum1L -= vHi * c2@
	VMLAL.S16	 	   Q15, D1, D17						@ sum1L += vLo * c1@
	VMLSL.S16  	 	 Q15, D2, D19						@ sum1L -= vHi * c2@
	
  VADD.S32		   D0, D30, D31
 
	add				     r6, r2, #512 
	VMOV			     r10, r11, D0	
 	add				     r7, r1, #2048
  
  VLD1.I16	 	  {D16, D17}, [r6]
	add				    r12, r10, r11	
	
	VLD1.16			  {D0, D1}, [r7]
	add						r12, r12, #4096
	
	VMULL.S16		  Q15, D0, D16							@sum1L += vLo * c1@
	ssat				  r8, #16, r12, asr #13 
		
	VMLAL.S16		  Q15, D1, D17							@sum1L += vLo * c1@
	strh          r8, [r0, #0] 
	
 	VADD.S32		  D0, D30, D31

 	add				    r2, r2, #32
	VMOV			    r10, r11, D0	
  mov     	    r6, #15

	add				    r1, r1, #128		
	add				    r12, r10, r11
	add				    r7, r0, #2

	add				    r12, r12, #4096
	add				    r3, r3, #128
	ssat			    r8, #16, r12, asr #13
	strh    	    r8, [r0, #32]

M493:
	VLD2.I16		 {D16, D17, D18, D19}, [r2]! 			@c1 = *coef@		coef++@		c2 = *coef@		coef++@
	VLD1.I16		 {D0, D1}, [r1]
	VLD1.I16		 {D2, D3}, [r3]
	VMULL.S16		 Q14, D0, D16							@sum1L += vLo * c1@
	VREV64.I16	 Q1, Q1
	
	VMULL.S16		 Q15, D0, D18							@sum2L += vLo * c2@
	VMLSL.S16		 Q14, D3, D18		  				@sum1L -= vHi * c2
	VMLAL.S16		 Q15, D3, D16							@sum2L += vHi * c1@
	VMLAL.S16		 Q14, D1, D17							@sum1L += vLo * c1@   	
	VMLAL.S16		 Q15, D1, D19							@sum2L += vLo * c2@
  VMLSL.S16		 Q14, D2, D19							@sum1L -= vHi * c2
  VMLAL.S16		 Q15, D2, D17							@sum2L += vHi * c1 
  
  VADD.S32		 D28, D28, D29
  VADD.S32		 D29, D30, D31
  	  
  VPADD.S32		 D0, D28, D29
  
  add				 r3, r3, #128
  VADD.S32		 D2, D0, D22
  mov     		 r8, r6, lsl #2 
  VQSHRN.S32		 D0, Q1, #13
  
  add				 r8, r7, r8
  add				 r1, r1, #128

	VST1.I16     	 {D0[0]}, [r7]    
			   
	subs     		 r6, r6, #1 
	add     		 r7, r7, #2 
	VST1.I16     	 {D0[1]}, [r8]
	bne     		 M493

M494:
	ldmia   sp!, {r4 - r11, pc}

	@ENDP


	.global	voMP3DecSynthStereo
	@AREA	.text, CODE, READONLY
voMP3DecSynthStereo: @PROC
	stmdb   sp!, {r4 - r11, lr}
	
	mov				 r10, #4096
	add				 r3, r1, #32
	add				 r4, r1, #64
	add				 r5, r1, #96
	VDUP.32		 	 Q11, r10
	   
	VLD2.16    	 	 {D16, D17, D18, D19}, [r2] 			@ c1 = *coef@		coef++@		c2 = *coef@		coef++@	
	VLD1.16		 	 {D0, D1}, [r1]							@ vLo = *(vb1+(x))@
	VLD1.16		 	 {D2, D3}, [r3]							@ vHi = *(vb1+(23-(x)))@
	VLD1.16		 	 {D4, D5}, [r4]							@ vLo = *(vb1+32+(x))@
	VLD1.16		 	 {D6, D7}, [r5]							@ vHi = *(vb1+32+(23-(x)))@	
	VREV64.16  	 	 Q1, Q1
	VREV64.16  		 Q3, Q3
		
	VMULL.S16	 	 Q14, D0, D16							@sum1L += vLo * c1@
	VMULL.S16  	 	 Q15, D4, D16							@sum1R += vLo * c1@
	VMLSL.S16  	 	 Q14, D3, D18							@sum1L -= vHi * c2@
	VMLSL.S16  	 	 Q15, D7, D18							@sum1R -= vHi * c2@
	VMLAL.S16	 	 Q14, D1, D17							@sum1L += vLo * c1@
	VMLAL.S16  	 	 Q15, D5, D17							@sum1R += vLo * c1@
	VMLSL.S16  	 	 Q14, D2, D19							@sum1L -= vHi * c2@
	VMLSL.S16	 	 Q15, D6, D19							@sum1R -= vHi * c2@
	
  	VADD.S32		 D28, D28, D29
  	VADD.S32		 D29, D30, D31
 
	add				 r6, r2, #512 
	VPADD.S32		 D28, D28, D29	
  	add				 r7, r1, #2048
  
  	VLD1.16			 {D16, D17}, [r6]
	VADD.S32		 D28, D28, D22
	add				 r8, r4, #2048
	
	VLD1.16			 {D0, D1}, [r7]
	VQSHRN.S32	 	 D4, Q14, #13		
	
	VLD1.16			 {D2, D3}, [r8]
	VST1.I32   		 {D4[0]}, [r0] 

	VMULL.S16		 Q14, D0, D16							@sum1L += vLo * c1@
	VMULL.S16		 Q15, D2, D16							@sum1R += vLo * c1@
	VMLAL.S16		 Q14, D1, D17							@sum1L += vLo * c1@
	VMLAL.S16		 Q15, D3, D17							@sum1R += vLo * c1@
	
	VADD.S32		 D28, D28, D29
  	VADD.S32		 D29, D30, D31
  
  	add				 r2, r2, #32
  	VPADD.S32		 D28, D28, D29
  	mov     		 r6, #15
  mov						r9, #128

	VADD.S32		 D4, D28, D22	
	add				 r1, r1, r9
	add				 r7, r0, #4
		
	VQSHRN.S32	 	 D4, Q2, #13	
	add				 r4, r4, r9
	add				 r0, r0, #64
	add				 r5, r5, r9
	add				 r3, r3, r9
	
	VST1.I32   		 {D4[0]}, [r0]
M1493:
	VLD2.I16		 {D16, D17, D18, D19}, [r2]! 	@c1 = *coef@		coef++@		c2 = *coef@		coef++@
	VLD1.I16		 {D0, D1}, [r1], r9
	VLD1.I16		 {D2, D3}, [r3], r9
	VLD1.I16		 {D4, D5}, [r4], r9
	VLD1.I16		 {D6, D7}, [r5], r9

	VMULL.S16		 Q12, D0, D16					@sum1L += vLo * c1@
	VREV64.I16	 	 Q1, Q1
	
  	VMULL.S16		 Q13, D4, D16					@sum1R += vLo * c1@	
    VREV64.I16	 	 Q3, Q3  	
    
	VMULL.S16		 Q14, D0, D18					@sum2L += vLo * c2@
  	VMULL.S16		 Q15, D4, D18	  				@sum2R += vLo * c2@  
  		
	VMLSL.S16		 Q12, D3, D18		  			@sum1L -= vHi * c2	
  	VMLSL.S16		 Q13, D7, D18					@sum1R -= vHi * c2  
  		
	VMLAL.S16		 Q14, D3, D16					@sum2L += vHi * c1@  
  	VMLAL.S16		 Q15, D7, D16					@sum2R += vHi * c1@	
  	
  	VMLAL.S16		 Q12, D1, D17					@sum1L += vLo * c1@  
  	VMLAL.S16		 Q13, D5, D17					@sum1R += vLo * c1@ 
  	  	
  	VMLAL.S16		 Q14, D1, D19					@sum2L += vLo * c2@  
  	VMLAL.S16		 Q15, D5, D19	  				@sum2R += vLo * c2@
  	  	
  	VMLSL.S16		 Q12, D2, D19					@sum1L -= vHi * c2
  	VMLSL.S16		 Q13, D6, D19					@sum1R -= vHi * c2  
  		
  	VMLAL.S16		 Q14, D2, D17					@sum2L += vHi * c1  
  	VMLAL.S16		 Q15, D6, D17					@sum2R += vHi * c1@
  
  	VADD.S32		 D24, D24, D25  
  	VADD.S32		 D25, D26, D27  	
  	VADD.S32		 D28, D28, D29
  	VADD.S32		 D29, D30, D31
  
  	VPADD.S32		 D0, D24, D25
  	VPADD.S32		 D1, D28, D29
  
  	mov     		 r8, r6, lsl #3 
  	@add				 r3, r3, #128
  	VADD.S32		 Q1, Q0, Q11
  	add				 r8, r7, r8
  	VQSHRN.S32	 	 D0, Q1, #13
  
  	@add				 r1, r1, #128

	@add				 r4, r4, #128
	@add				 r5, r5, #128
	
	subs     		 r6, r6, #1 
	VST1.I32     	 {D0[0]}, [r7]!            
	VST1.I32       	 {D0[1]}, [r8] 
	@add     		 r7, r7, #4 
	bne     		 M1493

	ldmia   sp!, {r4 - r11, pc}

	@ENDP
	.END
        
