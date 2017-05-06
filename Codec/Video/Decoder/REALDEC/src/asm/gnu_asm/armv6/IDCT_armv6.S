@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"

	@AREA ITRFRM, CODE, READONLY
	.section  .text
ARMV6_Transform4x4_Add:		@PROC
       
        stmfd    sp!,{r4-r12,lr}
		mov		 lr,#13
        mov      r4,r0      	 @pin = in
        mov      r9,#4      	 @r9 loop var
Ln16:
		ldrsh    r5,[r4,#0x00]     @r5 = a0
		ldrsh    r7,[r4,#0x20]     @r7 = a2
		ldrsh    r6,[r4,#0x10]     @r6 = a1
		ldrsh    r8,[r4,#0x30]     @r8 = a3
		
		pkhbt	 r5,r5,r7,lsl #16
		pkhbt    r6,r6,r8,lsl #16
		
		ldr      r7,W13_13
		ldr      r8,W7_17
		
		smuad    r10,r5,r7
		smuad    r11,r6,r8
		add      r12,r10,r11
		str		 r12,[r4],#4 
		
		sub      r10,r10,r11
		str      r10,[r4,#44] 
		
		smusd    r10,r5,r7
		smusdx   r11,r6,r8
		add      r12,r10,r11
		str      r12,[r4,#12]
		
		sub      r10,r10,r11
		str      r10,[r4,#28]      
        
        subs    r9,r9,#1	 @decrement loop var  
        bne      Ln16
        
        mov      r9,#4
        mov      r4,r0      	   @pin = in
Ln116:
		ldmia	 r4!,{r5-r8}	   @a0,a1,a2,a3
		@add		 r4,r4,#16     @pin += 4			

        add		 r10,r5,r7 	       @b0 = a0 + a2		
		mul      r10,lr,r10
        sub		 r11,r5,r7 	       @b1 = a0 - a2		
		mul      r11,lr,r11
		
        rsb      r7,r6,r6,lsl #3    @b2 = a1 * 7
        add      r12,r8,r8,lsl #4   @Temp(r12) = a3*17
        sub		 r7,r7,r12			@b2(r9) = b2 - Temp
        add      r5,r6,r6,lsl #4    @b3 = a1*17
        rsb      r6,r8,r8,lsl #3    @Temp= a3 * 7
        add		 r12,r5,r6			@b3(r12) = b3 + Temp		
        
        add		 r5,r10,r12	        @pin[0] = (b0 + b3 + 0x200) >> 10
        ldrb	 r8,[r3]
        add      r5,r5,#0x200
        @mov      r5,r5,asr #10
        add		 r5,r8,r5,asr #10      
              
		add		 r6,r11,r7			 @pin[1] = (b1 + b2 + 0x200) >> 10
		ldrb	 r8,[r3, #1]
        add      r6,r6,#0x200
        @mov     r6,r6,asr #10 
        add		 r6,r8,r6,asr #10
                               
        sub		 r7,r11,r7			 @pin[2] = (b1 - b2 + 0x200) >> 10
        ldrb	 r8,[r3, #2]
        add      r7,r7,#0x200
        @mov      r7,r7,asr #10
        add		 r7,r8,r7,asr #10
        
        sub      r8,r10,r12		     @pin[3] = (b0 - b3 + 0x200) >> 10  
        ldrb	 r10,[r3, #3] 	        
        add      r8,r8,#0x200		
        @mov     r8,r8,asr #10
        add		 r8,r10,r8,asr #10
        ldr      r10,[sp,#40]
        add		 r3,r3,r10           @pPredSrc += uPredPitch
        
        usat     r5, #8, r5
        usat     r6, #8, r6
        usat     r7, #8, r7
        usat     r8, #8, r8
              
        orr		  r5, r5, r8, lsl #24
        orr		  r5, r5, r6, lsl #8
        orr		  r5, r5, r7, lsl #16
        str		  r5, [r1]
        add       r1, r1, r2	    @pDst += uDestStride
        subs      r9,r9,#1	        @decrement loop var    
        bne       Ln116

        ldmfd    sp!,{r4-r12,pc}
		@ENDP
		.global	ARMV6_Transform4x4_Add		
		
	@AREA ITRFRM, CODE, READONLY
	.section  .text
ARMV6_ITransform4x4_DCOnly_Add:		@PROC
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14}
        
		ldr	   r4, [r0]
		mov	   lr, #13
		mul	   r4, lr, r4
		mul	   r4, lr, r4
		add	   r4, r4, #0x200
		mov	   r4, r4,asr #10

		ldr	   r9, [sp,#36]	@uPredPitch
        cmp    r4, #0       @if (v==0)                       
        bne    outCopyBlock4x4_asm_ARMv6     
		             
CopyBlock4x4_asm_ARMv6:
		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9 
 		ldr     r8, [r3], r9        
 		ldr     r10,[r3]	
        str     r4, [r1], r2 
        str     r6, [r1], r2 
        str     r8, [r1], r2 
        str     r10,[r1] 
		    		  				                                       
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}

outCopyBlock4x4_asm_ARMv6:

		blt     little_begin_ARMv6 

big_begin_ARMv6_ARMv6:
        usat    r4, #8, r4 @real_TCK	                                       
        orr     r12, r4, r4, lsl #8            
        orr     r5, r12, r12, lsl #16    
		        
 		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9   			             
		uqadd8	r8, r4, r5
		uqadd8	r10,r6, r5
        str     r8, [r1], r2 
        str     r10,[r1], r2 

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3]   			             
		uqadd8	r8, r4, r5
		uqadd8	r10,r6, r5
        str     r8, [r1], r2 
        str     r10,[r1]											                                             
   
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15}   
	
little_begin_ARMv6:
                      
        rsb     r12, r4, #0                       
        usat    r12, #8, r12 @real_TCK                               
        orr     r12, r12, r12, lsl #8            
        orr     r5, r12, r12, lsl #16            

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3], r9   			             
		uqsub8	 r8, r4, r5
		uqsub8	 r10, r6, r5
        str     r8, [r1], r2 
        str     r10, [r1], r2 

 		ldr     r4, [r3], r9        
 		ldr     r6, [r3]   			             
		uqsub8	 r8, r4, r5
		uqsub8	 r10, r6, r5
        str     r8, [r1], r2 
        str     r10, [r1] 

        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @C_ITransform4x4_DCOnly_Add
        @ENDP
		.global	ARMV6_ITransform4x4_DCOnly_Add 
		
ARMV6_Intra16x16ITransform4x4:	@PROC     
        stmfd    sp!,{r4-r12,lr}
		mov		 r8,#13
        mov      r1,r0      	   @pin = in
        mov      r2,#4      	   @r2 loop var
Ln272:
		ldr      r5,[r1, #0]       @r5 = a0
		ldr      r6,[r1, #8]       @r6 = a2

        subs     r2,r2,#1		   @decrement loop var        			

        add		 r3,r5,r6 	       @b0 = a0 + a2
		mul      r3,r8,r3
        sub		 r4,r5,r6 	       @b1 = a0 - a2
		mul      r4,r8,r4
		
		ldr      r5,[r1, #4]       @r5 = a1
		ldr      r6,[r1, #12]      @r6 = a3
		
        rsb      r7,r5,r5,lsl #3   @b2 = a1 * 7
        add      r12,r6,r6,lsl #4  @Temp(r12) = a3*17
        sub		 r7,r7,r12         @b2(r7) = b2 - Temp
        add      r5,r5,r5,lsl #4   @b3 = a1*17
        rsb      r6,r6,r6,lsl #3   @Temp= a3 * 7
        add		 r12,r5,r6	       @b3(r12) = b3 + Temp		
        
        add		 r5,r3,r12	       @pin[0] = b0 + b3
        add		 r6,r4,r7		   @pin[1] = b1 + b2
        str		 r5,[r1], #4
        str		 r6,[r1], #4
        sub		 r5,r4,r7	       @pin[2] = b1 - b2
        sub      r6,r3,r12		   @pin[3] = b0 - b3	        
        str		 r5,[r1], #4
        str		 r6,[r1], #4        

        bne      Ln272
        
        mov      r2,#4
        mov      r1,r0      	   @pin = in
Ln368:
		ldr      r5,[r1,#0x00]     @r5 = a0
		ldr      r6,[r1,#0x20]     @r6 = a2

        subs     r2,r2,#1		   @decrement loop var        			

        add		 r3,r5,r6 	       @b0 = a0 + a2
		mul      r3,r8,r3
        sub		 r4,r5,r6 	       @b1 = a0 - a2
		mul      r4,r8,r4
		
		ldr      r5,[r1, #0x10]    @r5 = a1
		ldr      r6,[r1, #0x30]    @r6 = a3
		
        rsb      r7,r5,r5,lsl #3   @b2 = a1 * 7
        add      r12,r6,r6,lsl #4  @Temp(r12) = a3*17
        sub		 r7,r7,r12         @b2(r7) = b2 - Temp
        add      r5,r5,r5,lsl #4   @b3 = a1*17
        rsb      r6,r6,r6,lsl #3   @Temp = a3 * 7
        add		 r12,r5,r6	       @b3(r12) = b3 + Temp		
        
        add		 r5,r3,r12	       @pin[0]  = ((b0 + b3) * 3) >> 11
        rsb      r5,r5,r5,lsl #2
        mov      r5,r5,asr #11
        add		 r6,r4,r7		   @pin[4]  = ((b1 + b2) * 3) >> 11
        rsb      r6,r6,r6,lsl #2
        mov      r6,r6,asr #11
        str		 r5,[r1], #4
        str		 r6,[r1, #12]
        sub		 r5,r4,r7	       @pin[8]  = ((b1 - b2) * 3) >> 11
        rsb      r5,r5,r5,lsl #2
        mov      r5,r5,asr #11
        sub      r6,r3,r12		   @pin[12] = ((b0 - b3) * 3) >> 11        
        rsb      r6,r6,r6,lsl #2
        mov      r6,r6,asr #11
        str		 r5,[r1, #28]
        str		 r6,[r1, #44]

        bne      Ln368

        ldmfd    sp!,{r4-r12,pc}

		@ENDP
		.global	ARMV6_Intra16x16ITransform4x4		

.align 
W13_13:		.word	0x000D000D
W7_17:		.word	0x00070011	@huwei 20080917 16bit idct
		
	@END