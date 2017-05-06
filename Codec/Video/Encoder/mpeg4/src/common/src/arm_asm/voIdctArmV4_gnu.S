@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
	.section .text
	
	.global IdctBlock1x1_ARMV4
	.global IdctBlock4x4_ARMV4
	.global IdctBlock4x8_ARMV4
	.global IdctBlock8x8_ARMV4	
 
	.align 4
MaskCarry:	
		.word 0x80808080

IdctBlock1x1_ARMV4:	
        STMFD    sp!,{r4-r11,lr}
		@ r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src

	ldrsh	r12, [r0]
	mov	r5, #0
	cmp	r3, #0	
	add	r12, r12, #4	
	strh	r5, [r0]	
	mov	r0, r12, asr #3
@	int32_t OutD = (Block[0]+4)>>3@		
	beq	IDCT_CPY_NOSRC_ARMv4	
	
	@const uint8_t* SrcEnd = Src + 8*SrcPitch (int SrcPitch = 8@)
		ldr		r12,[sp,#36]	@SrcPitch
	@if (vv==0)
        cmp     r0, #0
		sub     r12, r12, #4
		sub		r2, r2, #4                        
        bne     armv4_outCopyBlock8x8_asm     
		             
armv4_CopyBlock8x8_asm:   

        ldr     r0, [r3], #4   
        ldr     r4, [r3], r12                      
        ldr     r5, [r3], #4                      
        ldr     r6, [r3], r12                    
        ldr     r7, [r3], #4    
        ldr     r8, [r3], r12                    
        ldr     r9, [r3], #4   
        ldr     r14, [r3], r12                    
        str     r0, [r1], #4   
        str     r4, [r1], r2
        str     r5, [r1], #4 
        str     r6, [r1], r2
        str     r7, [r1], #4    
        str     r8, [r1], r2 
        str     r9, [r1], #4 
        str     r14, [r1], r2


        ldr     r0, [r3], #4   
        ldr     r4, [r3], r12                      
        ldr     r5, [r3], #4                      
        ldr     r6, [r3], r12                    
        ldr     r7, [r3], #4    
        ldr     r8, [r3], r12                    
        ldr     r9, [r3], #4   
        ldr     r14, [r3], r12                    
        str     r0, [r1], #4   
        str     r4, [r1], r2
        str     r5, [r1], #4 
        str     r6, [r1], r2
        str     r7, [r1], #4    
        str     r8, [r1], r2 
        str     r9, [r1], #4 
        str     r14, [r1], r2		  		    		  				                         
             
        LDMFD    sp!,{r4-r11,pc}

armv4_outCopyBlock8x8_asm:

		mov		r6, r12    
		mov		r8, #8  
        ldr     r10, MaskCarry
		str		r8,[sp,#36]	@count		  
		blt     armv4_little_begin 

armv4_big_begin:	                                     
	@v |= v << 8@
        orr     r12, r0, r0, lsl #8            
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16    	        
armv4_big_loop_do:
	@a = ((uint32_t*)Src)[0]@                
        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
        ldr     r4, [r3], r6                                       
	@b = a + v@
        add     r14, r5, r8
	@c = a & v@
        and     r7, r5, r8
	@a ^= v@	
        eor     r9, r8, r5
	@a &= ~b@
        mvn     r8, r14
        and     r8, r9, r8
	@a |= c@	
        orr     r7, r8, r7
	@a &= MaskCarry@
        and     r12, r7, r10
	@c = a << 1@	b -= c@	
        sub     r14, r14, r12, lsl #1
	@b |= c - (a >> 7)@
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1
        orr     r11, r14, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r7, r5, r4
	@d ^= v@	
        eor     r8, r4, r5
	@d &= ~b@
        mvn     r4, r12
        and     r4, r8, r4
	@d |= c@
        orr     r4, r7, r4
	@d &= MaskCarry@
        and     r4, r4, r10
	@c = d << 1@	b -= c@	
        sub     r14, r12, r4, lsl #1
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1
        orr     r12, r14, r12                     
		ldr		r8,[sp,#36]	@count    		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r1], #4
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r1], r2
	@Dst += DstPitch@
@        add     r1, r1, r2
	@Src += SrcPitch@
@        add     r3, r3, #8
	@while (Src != SrcEnd)@	
@        cmp     r3, r6

		subs	r8, r8, #1     
		str		r8,[sp,#36]	@count                 
        bne     armv4_big_loop_do                             
        LDMFD    sp!,{r4-r11,pc} 
		
armv4_little_begin:
                      
        rsb   r12, r0, #0                       

	@v |= v << 8@
        orr     r12, r12, r12, lsl #8
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16           
armv4_little_loop_do:
	@a = ((uint32_t*)Src)[0]@                
        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
        ldr     r4, [r3], r6                     
	@a = ~a@	
        mvn   r8, r8 
       	mvn   r4, r4                                      
	@b = a + v@
        add     r14, r5, r8                      
	@c = a & v@
        and     r7, r5, r8                        
	@a ^= v@	
        eor     r9, r8, r5                        
	@a &= ~b@
        mvn     r8, r14                           
        and     r8, r9, r8                        
	@a |= c@	
        orr     r7, r8, r7                        
	@a &= MaskCarry@
        and     r12, r7, r10                      
	@c = a << 1@	b -= c@	
        sub     r14, r14, r12, lsl #1             
	@b |= c - (a >> 7)@
        mov     r7, r12, lsr #7                   
        rsb     r12, r7, r12, lsl #1              
        orr     r11, r14, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r7, r5, r4                        
	@d ^= v@	
        eor     r8, r4, r5                        
	@d &= ~b@
        mvn     r4, r12                           
        and     r4, r8, r4                        
	@d |= c@
        orr     r4, r7, r4                        
	@d &= MaskCarry@
        and     r4, r4, r10                       
	@c = d << 1@	b -= c@	
        sub     r14, r12, r4, lsl #1              
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              
        orr     r12, r14, r12                  
	@b = ~b@	
        mvn   r11, r11  
	@b = ~b@
        mvn   r12, r12   
	ldr		r8,[sp,#36]	@count 		      		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r1], #4
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r1], r2                     
	@Dst += DstPitch@
@        add     r1, r1, r2                        
	@Src += SrcPitch@
@        add     r3, r3, #8                        
	@while (Src != SrcEnd)@	
@        cmp     r3, r6
		subs	r8, r8, #1     
		str		r8,[sp,#36]	@count                                
        bne     armv4_little_loop_do                             
        LDMFD    sp!,{r4-r11,pc} 

IDCT_CPY_NOSRC_ARMv4:	
@	OutD = SAT_new(OutD)@
@@	usat	r12, #8, r0
	mov	r10, #0xFFFFFF00
	tst     r0, r10
	movne	r0, #0xFF
	movmi	r0, #0x00
	orr     r10, r0, r0, lsl #8
	orr     r10, r10, r10, lsl #16
        str     r10, [r1, #4] 
        str     r10, [r1], r2 
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1], r2
        str     r10, [r1, #4] 
        str     r10, [r1]   		
        LDMFD    sp!,{r4-r11,pc}		        		        
	@ENDP

	.align 4
Col8:
	ldrsh	r1, [r0]
	ldrsh	r2, [r0, #16]
	ldrsh	r3, [r0, #32]
	ldrsh	r4, [r0, #48]

	ldrsh	r5, [r0, #64]
	ldrsh	r6, [r0, #80]
	ldrsh	r7, [r0, #96]
	ldrsh	r8, [r0, #112]

	orr       r9, r3, r4
	orr       r10, r5, r6
	orr       r9, r9, r7
	orr       r10, r10, r8
	orrs      r9, r9, r10

	bne	allx1_8_t_lab		@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	cmp	r2, #0				
	bne	only_x1x2t_lab		@ x2!=0
	
	cmp	r1, #0
	bne	only_x1t_lab		@ x1!=0
	mov	pc, lr
	
only_x1t_lab:
@//ip0
@	E = (idct_t)(Blk[0*8]<< 3)@
@	ip[0~7*8] = E@
    	mov	r5, r1, lsl #3
	strh	r5, [r0]
	strh	r5, [r0, #16]
	strh	r5, [r0, #32]
	strh	r5, [r0, #48]
	strh	r5, [r0, #64]
	strh	r5, [r0, #80]
	strh	r5, [r0, #96]
	strh	r5, [r0, #112]
	mov	pc, lr	
only_x1x2t_lab:
@//ip0, 1
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@	E = Blk[0*8] << 11@
@	E += 128@	
@	Add = 181 * ((A - B)>> 8)@		
@	Bdd = 181 * ((A + B)>> 8)@
@	Blk[0*8] = (idct_t)((E + A) >> 8)@
@	Blk[7*8] = (idct_t)((E - A) >> 8)@		
@	Blk[1*8] = (idct_t)((E + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((E - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((E + B) >> 8)@
@	Blk[4*8] = (idct_t)((E - B) >> 8)@				
@	Blk[2*8] = (idct_t)((E + Add) >> 8)@
@	Blk[5*8] = (idct_t)((E - Add) >> 8)@

	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mov	r7, r1, lsl #11	@E = Blk[0*8] << 11
	add	r7, r7, #128	@E += 128@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov	r1, r1, asr #8	@
	mov	r2, r2, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r6, r6, asr #8	@
	mov	r9, r9, asr #8	@
	mov	r3, r3, asr #8	@
	
	strh	r1, [r0]
	strh	r8, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr	
allx1_8_t_lab:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7*8] + W1 * Blk[1*8]@
@	B = W7 * Blk[1*8] - W1 * Blk[7*8]@	
@	C = W3 * Blk[3*8] + W5 * Blk[5*8]@
@	D = W3 * Blk[5*8] - W5 * Blk[3*8]@		
@	Ad = A - C@
@	Bd = B - D@
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@		
@	Cd = A + C@
@	Dd = B + D@
@	E = (Blk[0*8] + Blk[4*8]) << 11@
@	F = (Blk[0*8] - Blk[4*8]) << 11@	
@	E += 128@
@	F += 128@			
@	G = W6 * Blk[6*8] + W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8] - W2 * Blk[6*8]@	
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@	
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@			
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@	
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@			
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@	ip0~ip7 = r1~8

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r11, r9,  r2		@M(xC1S7, ip[1*8])
	mla     r12, r10, r8, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
	mul     r2,  r10, r2		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r2, r2, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r11, r9,  r4		@M(xC3S5, ip[3*8])
	mla     r8, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r6,  r9, r6		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r4, r6, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	sub	r6, r2, r4	@Bd = B - D@
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	add	r4, r2, r4	@Dd = B + D@
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r2, r9,  r3		@M(xC2S6, ip[2*8])
	mla     r12, r10, r7, r2	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r3,  r10, r3		@M(xC6S2, ip[2*8])
	mul     r7,  r9,  r7		@M(xC2S6, ip[6*8])
	sub	r2, r3, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
		
@ r9, r10, r3, r7 free now	G = r12, H = r2
	add	r3, r1, r5	@E = (Blk[0*8] + Blk[4*8]) << 11
	sub	r7, r1, r5	@F = (Blk[0*8] - Blk[4*8]) << 11@
	mov	r3, r3, asl #11
	mov	r7, r7, asl #11
	add	r3, r3, #128
	add	r7, r7, #128
				
@ r9, r10 free now		E = r3, F = r7
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	sub	r3, r7, r2	@Gd = F - H@
	add	r12, r7, r2	@Hd = F + H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r2, r10, r8	@Fd + Cd
	sub	r7, r10, r8	@Fd - Cd
	add	r8, r12, r11	@Hd + Bdd
	sub	r10, r12, r11	@Hd - Bdd	
	add	r11, r9, r4	@Ed + Dd
	sub	r12, r9, r4	@Ed - Dd	
	add	r4, r3, r6	@Gd + Add
	sub	r9, r3, r6	@Gd - Add

@ r3, r6  free now
		
	mov	r2, r2, asr #8		@
	mov	r7, r7, asr #8		@
	mov	r8, r8, asr #8		@
	mov	r10, r10, asr #8	@
	mov	r11, r11, asr #8	@
	mov	r12, r12, asr #8	@
	mov	r4, r4, asr #8		@
	mov	r9, r9, asr #8		@

	strh	r2, [r0]
	strh	r8, [r0, #16]
	strh	r4, [r0, #32]
	strh	r11, [r0, #48]
	strh	r12, [r0, #64]
	strh	r9, [r0, #80]
	strh	r10, [r0, #96]
	strh	r7, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src:
@	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
@	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]			 
	
	orr	r6, r3, r4		@ r6 = ip3|ip2 | ip5|ip4
	orrs	r6, r6, r5		@ r6 = ip3|ip2 | ip5|ip4 | ip7|ip6
	bne	allx1_8_t_lab_Rowsrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rowsrc	@ x2!=0
	ldr	r6, [sp, #4]		@ Src = [sp, #4]
	ldr	r8, [sp, #52]		@ src_stride = [sp, #52]
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r9, r7, r9	
	str	r8, [sp, #4]		@ Src = [sp, #4]
	str	r9, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rowsrc	@ x1!=0
@if(src)
@{
@	if(dst !=src )
@	{
@		dst[0] = src[0]@
@		dst[1] = src[1]@
@		dst[2] = src[2]@
@		dst[3] = src[3]@
@		dst[4] = src[4]@
@		dst[5] = src[5]@
@		dst[6] = src[6]@
@		dst[7] = src[7]@
@	}
@	src += stride@
@}	
	cmp	r6, r7	
	moveq	pc, lr
	ldr	r2, [r6]
	ldr	r3, [r6, #4]								 		
@	strd		r2, [r7]
	str	r2, [r7]
	str	r3, [r7, #4]		
	mov	pc, lr
	
only_x1t_lab_Rowsrc:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@        dst[0] = SAT(((src[0] + E)))@
@        dst[7] = SAT(((src[7] + E)))@
@
@        dst[1] = SAT(((src[1] + E)))@
@        dst[2] = SAT(((src[2] + E)))@
@
@        dst[3] = SAT(((src[3] + E)))@
@        dst[4] = SAT(((src[4] + E)))@
@
@        dst[5] = SAT(((src[5] + E)))@
@        dst[6] = SAT(((src[6] + E)))@
@	src += stride@
@    }
@}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	add	r1, r2, #32

	ldr	r8, [r6]
	ldr	r4, [r6, #4]
	movs	r1, r1, asr #6	
	streq	r8, [r7]
	streq	r4, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
@	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
@	uxtb16		r4, r2			@0, 2	a
@	uxtb16		r5, r2, ror #8		@1, 3	a
@	uxtb16		r6, r3			@0, 2	b
@	uxtb16		r8, r3, ror #8		@1, 3	b		
@	sadd16		r4, r4, r1		@0, 2	a+e
@	sadd16		r5, r5, r1		@1, 3	a+e
@	sadd16		r6, r6, r1		@0, 2	b+e
@	sadd16		r8, r8, r1		@1, 3	b+e		
@	usat16	 r4,#8,r4
@	usat16	 r5,#8,r5
@	usat16	 r6,#8,r6
@	usat16	 r8,#8,r8		
@	orr	r2,r4,r5,lsl #8
@	orr	r3,r6,r8,lsl #8
@	strd		r2, [r7]						
@	mov	pc, lr	

        ldr     r10, MaskCarry 
	blt     armv4_little_begin_8x8 
armv4_big_begin_8x8:	
                                      
	@v |= v << 8@
        orr     r12, r1, r1, lsl #8            
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16    	        

	@a = ((uint32_t*)Src)[0]@                
@        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
@        ldr     r4, [r3], r6                                       
	@b = a + v@
        add     r3, r5, r8
	@c = a & v@
        and     r1, r5, r8
	@a ^= v@	
        eor     r9, r8, r5
	@a &= ~b@
        mvn     r8, r3
        and     r8, r9, r8
	@a |= c@	
        orr     r1, r8, r1
	@a &= MaskCarry@
        and     r12, r1, r10
	@c = a << 1@	b -= c@	
        sub     r3, r3, r12, lsl #1
	@b |= c - (a >> 7)@
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1
        orr     r11, r3, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r1, r5, r4
	@d ^= v@	
        eor     r8, r4, r5
	@d &= ~b@
        mvn     r4, r12
        and     r4, r8, r4
	@d |= c@
        orr     r4, r1, r4
	@d &= MaskCarry@
        and     r4, r4, r10
	@c = d << 1@	b -= c@	
        sub     r3, r12, r4, lsl #1
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1
        orr     r12, r3, r12 		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r7]
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r7, #4]
	mov	pc, lr      
		
armv4_little_begin_8x8:
                      
        rsb   r12, r1, #0
	@v |= v << 8@
        orr     r12, r12, r12, lsl #8
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16           

	@a = ((uint32_t*)Src)[0]@                
@        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
@        ldr     r4, [r3], r6                     
	@a = ~a@	
        mvn   r8, r8 
       	mvn   r4, r4                                      
	@b = a + v@
        add     r3, r5, r8                      
	@c = a & v@
        and     r1, r5, r8                        
	@a ^= v@	
        eor     r9, r8, r5                        
	@a &= ~b@
        mvn     r8, r3                           
        and     r8, r9, r8                        
	@a |= c@	
        orr     r1, r8, r1                        
	@a &= MaskCarry@
        and     r12, r1, r10                      
	@c = a << 1@	b -= c@	
        sub     r3, r3, r12, lsl #1             
	@b |= c - (a >> 7)@
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1              
        orr     r11, r3, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r1, r5, r4                        
	@d ^= v@	
        eor     r8, r4, r5                        
	@d &= ~b@
        mvn     r4, r12                           
        and     r4, r8, r4                        
	@d |= c@
        orr     r4, r1, r4                        
	@d &= MaskCarry@
        and     r4, r4, r10                       
	@c = d << 1@	b -= c@	
        sub     r3, r12, r4, lsl #1              
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              
        orr     r12, r3, r12                  
	@b = ~b@	
        mvn   r11, r11  
	@b = ~b@
        mvn   r12, r12	      		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r7]
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r7, #4]
	mov	pc, lr
			
only_x1x2t_lab_Rowsrc:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1]@
@	B = W7 * Blk[1]@	
@	E = (Blk[0] + 32) << 11@	
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((Src[0] + ((E + A )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((E - A )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((E + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((E - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((E + B )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((E - B )>>17))))@	
@	Dst[2] = SAT_new(((Src[2] + ((E + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((E - Add )>>17))))@		 				
@}
@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	@E = (Blk[0] + 32) << 11@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
allx1_8_t_lab_Rowsrc:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7] + W1 * Blk[1]@
@	B = W7 * Blk[1] - W1 * Blk[7]@
@	C = W3 * Blk[3] + W5 * Blk[5]@
@	D = W3 * Blk[5] - W5 * Blk[3]@
@	E = (Blk[0] + 32 + Blk[4]) << 11@
@	F = (Blk[0] + 32 - Blk[4]) << 11@
@	G = W6 * Blk[6] + W2 * Blk[2]@		
@	H = W6 * Blk[2] - W2 * Blk[6]@
@	Ad = A - C@
@	Bd = B - D@
@	Add = 181 * ((Ad - Bd) >> 8)@		
@	Bdd = 181 * ((Ad + Bd) >> 8)@					
@	Cd = A + C@
@	Dd = B + D@
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@
@	Dst[0] = SAT_new(((Src[0] + ((Fd + Cd )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((Fd - Cd )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((Hd + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((Hd - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((Ed + Dd )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((Ed - Dd )>>17))))@
@	Dst[2] = SAT_new(((Src[2] + ((Gd + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((Gd - Add )>>17))))@
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6
	@	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
		
	mov	r7, r5, lsl #16
	mov	r7, r7, asr #16
	mov	r8, r5, asr #16
	
	mov	r5, r4, lsl #16
	mov	r5, r5, asr #16
	mov	r6, r4, asr #16	
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r11, r9,  r2		@M(xC1S7, ip[1*8])
	mla     r12, r10, r8, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
	mul     r2,  r10, r2		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r2, r2, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r11, r9,  r4		@M(xC3S5, ip[3*8])
	mla     r8, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r6,  r9, r6		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r4, r6, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	sub	r6, r2, r4	@Bd = B - D@
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	add	r4, r2, r4	@Dd = B + D@
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r2, r9,  r3		@M(xC2S6, ip[2*8])
	mla     r12, r10, r7, r2	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r3,  r10, r3		@M(xC6S2, ip[2*8])
	mul     r7,  r9,  r7		@M(xC2S6, ip[6*8])
	sub	r2, r3, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
		
@ r9, r10, r3, r7 free now	G = r12, H = r2
	add	r1, r1, #32
	add	r3, r1, r5	@E = (Blk[0*8] + 32 + Blk[4*8]) << 11
	sub	r7, r1, r5	@F = (Blk[0*8] + 32 - Blk[4*8]) << 11@
	mov	r3, r3, asl #11
	mov	r7, r7, asl #11
				
@ r9, r10 free now		E = r3, F = r7
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	sub	r3, r7, r2	@Gd = F - H@
	add	r12, r7, r2	@Hd = F + H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	@Fd + Cd
	sub	r2, r10, r8	@Fd - Cd
	add	r5, r9, r4	@Ed + Dd
	sub	r7, r9, r4	@Ed - Dd	r6	

	add	r8, r12, r11	@Hd + Bdd
	sub	r4, r12, r11	@Hd - Bdd	

	add	r9, r3, r6	@Gd + Add
	sub	r3, r3, r6	@Gd - Add
	mov	r6, r7
	
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00						
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP

@#define W1 2841	0B19                // 2048*sqrt(2)*cos(1*pi/16) 
@#define W2 2676	0A74                 // 2048*sqrt(2)*cos(2*pi/16) 
@#define W3 2408	0968                 // 2048*sqrt(2)*cos(3*pi/16) 
@#define W5 1609	0649                 // 2048*sqrt(2)*cos(5*pi/16) 
@#define W6 1108	0454                 // 2048*sqrt(2)*cos(6*pi/16) 
@#define W7 565		0235                  // 2048*sqrt(2)*cos(7*pi/16)
@#define W4 181		00B5                  // 2048*sqrt(2)*cos(7*pi/16)

	.align 8
WxW1:		.word 0x0B19
WxW2:		.word 0x0A74
WxW3:		.word 0x0968
WxW4:		.word 0x00B5
WxW5:		.word 0x0649
WxW6:		.word 0x0454
WxW7:		.word 0x0235	

	.align 4
Row8nosrc:
@	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
@	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]	
	
	orr	r6, r3, r4		@ r6 = ip3|ip2 | ip5|ip4
	orrs	r6, r6, r5		@ r6 = ip3|ip2 | ip5|ip4 | ip7|ip6
	bne	allx1_8_t_lab_Rownosrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rownosrc	@ x2!=0
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]
	add	r9, r7, r9	
	str	r9, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rownosrc	@ x1!=0
@@{
@	Dst[0] =
@	Dst[7] =
@	Dst[1] =
@	Dst[6] =
@	Dst[3] =
@	Dst[4] =
@	Dst[2] =
@	Dst[5] = 0@
@@}	
	mov	r2, #0
@	strd		r2, [r7]
	str	r2, [r7]
	str	r2, [r7, #4]		
	mov	pc, lr
	
only_x1t_lab_Rownosrc:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@@        dst[0] =
@@        dst[7] =
@@
@@        dst[1] =
@@        dst[2] =
@@
@@        dst[3] =
@@        dst[4] =
@@
@@        dst[5] =
@@        dst[6] = SAT_new(E)@@
@}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	add	r1, r2, #32
	mov	r2, r1, asr #6
	mov	r11, #0xFFFFFF00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00	
	orr     r2, r2, r2, lsl #8
	orr     r2, r2, r2, lsl #16
	str	r2, [r7]						
	str	r2, [r7, #4]						
	mov	pc, lr	
	
only_x1x2t_lab_Rownosrc:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1]@
@	B = W7 * Blk[1]@	
@	E = (Blk[0] + 32) << 11@	
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((E + A )>>17))@
@	Dst[7] = SAT_new(((E - A )>>17))@	
@	Dst[1] = SAT_new(((E + Bdd )>>17))@
@	Dst[6] = SAT_new(((E - Bdd )>>17))@	
@	Dst[3] = SAT_new(((E + B )>>17))@
@	Dst[4] = SAT_new(((E - B )>>17))@	
@	Dst[2] = SAT_new(((E + Add )>>17))@
@	Dst[5] = SAT_new(((E - Add )>>17))@
@}
@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	@E = (Blk[0] + 32) << 11@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00								
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
allx1_8_t_lab_Rownosrc:
@//ip0,1,2,3 4,5,6,7
@	A = W7 * Blk[7] + W1 * Blk[1]@
@	B = W7 * Blk[1] - W1 * Blk[7]@
@	C = W3 * Blk[3] + W5 * Blk[5]@
@	D = W3 * Blk[5] - W5 * Blk[3]@
@	E = (Blk[0] + 32 + Blk[4]) << 11@
@	F = (Blk[0] + 32 - Blk[4]) << 11@
@	G = W6 * Blk[6] + W2 * Blk[2]@		
@	H = W6 * Blk[2] - W2 * Blk[6]@
@	Ad = A - C@
@	Bd = B - D@
@	Add = 181 * ((Ad - Bd) >> 8)@		
@	Bdd = 181 * ((Ad + Bd) >> 8)@					
@	Cd = A + C@
@	Dd = B + D@
@	Ed = E - G@
@	Fd = E + G@
@	Gd = F - H@		
@	Hd = F + H@
@	Dst[0] = SAT_new(((Fd + Cd )>>17))@
@	Dst[7] = SAT_new(((Fd - Cd )>>17))@
@	Dst[1] = SAT_new(((Hd + Bdd )>>17))@
@	Dst[6] = SAT_new(((Hd - Bdd )>>17))@
@	Dst[3] = SAT_new(((Ed + Dd )>>17))@
@	Dst[4] = SAT_new(((Ed - Dd )>>17))@
@	Dst[2] = SAT_new(((Gd + Add )>>17))@
@	Dst[5] = SAT_new(((Gd - Add )>>17))@
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6
	@	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
		
	mov	r7, r5, lsl #16
	mov	r7, r7, asr #16
	mov	r8, r5, asr #16
	
	mov	r5, r4, lsl #16
	mov	r5, r5, asr #16
	mov	r6, r4, asr #16	
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r11, r9,  r2		@M(xC1S7, ip[1*8])
	mla     r12, r10, r8, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
	mul     r2,  r10, r2		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r2, r2, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r11, r9,  r4		@M(xC3S5, ip[3*8])
	mla     r8, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r6,  r9, r6		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r4, r6, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	sub	r6, r2, r4	@Bd = B - D@
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	add	r4, r2, r4	@Dd = B + D@
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r2, r9,  r3		@M(xC2S6, ip[2*8])
	mla     r12, r10, r7, r2	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r3,  r10, r3		@M(xC6S2, ip[2*8])
	mul     r7,  r9,  r7		@M(xC2S6, ip[6*8])
	sub	r2, r3, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
		
@ r9, r10, r3, r7 free now	G = r12, H = r2
	add	r1, r1, #32
	add	r3, r1, r5	@E = (Blk[0*8] + 32 + Blk[4*8]) << 11
	sub	r7, r1, r5	@F = (Blk[0*8] + 32 - Blk[4*8]) << 11@
	mov	r3, r3, asl #11
	mov	r7, r7, asl #11
				
@ r9, r10 free now		E = r3, F = r7
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	sub	r3, r7, r2	@Gd = F - H@
	add	r12, r7, r2	@Hd = F + H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	@Fd + Cd
	sub	r2, r10, r8	@Fd - Cd
	add	r5, r9, r4	@Ed + Dd
	sub	r7, r9, r4	@Ed - Dd	r6
		
	add	r8, r12, r11	@Hd + Bdd
	sub	r4, r12, r11	@Hd - Bdd
		
	add	r9, r3, r6	@Gd + Add
	sub	r3, r3, r6	@Gd - Add
	mov	r6, r7
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17	
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now

	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP
		
	.align 4
Col8_4x4:
	ldrsh	r1, [r0]
	ldrsh	r2, [r0, #16]
	ldrsh	r3, [r0, #32]
	ldrsh	r4, [r0, #48]
	
	orrs	r7, r3, r4		@ r7 = ip2|ip3
	bne	allx1_4_t_lab		@ (ip2|ip3)!=0	
	
 	cmp	r2, #0				
	bne	only_x1x2t_lab_4x4		@ x2!=0
	
	cmp	r1, #0
	bne	only_x1t_lab_4x4		@ x1!=0
	mov	pc, lr
	
only_x1t_lab_4x4:
@//ip0
@	E = (idct_t)(Blk[0*8]<< 3)@
@	ip[0~7*8] = E@    					    																																										    ip[7*8] =
    	mov	r5, r1, lsl #3
	strh	r5, [r0]
	strh	r5, [r0, #16]
	strh	r5, [r0, #32]
	strh	r5, [r0, #48]
	strh	r5, [r0, #64]
	strh	r5, [r0, #80]
	strh	r5, [r0, #96]
	strh	r5, [r0, #112]
	mov	pc, lr
only_x1x2t_lab_4x4:
@//ip0, 1
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@	E = Blk[0*8] << 11@
@	E += 128@	
@	Add = 181 * ((A - B)>> 8)@		
@	Bdd = 181 * ((A + B)>> 8)@
@	Blk[0*8] = (idct_t)((E + A) >> 8)@
@	Blk[7*8] = (idct_t)((E - A) >> 8)@		
@	Blk[1*8] = (idct_t)((E + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((E - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((E + B) >> 8)@
@	Blk[4*8] = (idct_t)((E - B) >> 8)@				
@	Blk[2*8] = (idct_t)((E + Add) >> 8)@
@	Blk[5*8] = (idct_t)((E - Add) >> 8)@

	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mov	r7, r1, lsl #11	@E = Blk[0*8] << 11
	add	r7, r7, #128	@E += 128@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov	r1, r1, asr #8	@
	mov	r2, r2, asr #8	@
	mov	r8, r8, asr #8	@
	mov	r4, r4, asr #8	@
	mov	r5, r5, asr #8	@
	mov	r6, r6, asr #8	@
	mov	r9, r9, asr #8	@
	mov	r3, r3, asr #8	@
	
	strh	r1, [r0]
	strh	r8, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr
	
allx1_4_t_lab:            
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@	E = Blk[0*8] << 11@
@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@(-D) = M(xC5S3, ip[3*8])

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	add	r6, r2, r4	@Bd = B - D@	B + (-D)
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	sub	r4, r2, r4	@Dd = B + D@	B - (-D)
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		@G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		@H = M(xC6S2, ip[2*8])		
@ r9, r10, r3, r7 free now	G = r12, H = r2
	mov	r1, r1, asl #11
	add	r3, r1, #128
				
@ r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	add	r12, r3, r2	@Hd = F + H@	
	sub	r3, r3, r2	@Gd = F - H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r2, r10, r8	@Fd + Cd
	sub	r7, r10, r8	@Fd - Cd
	add	r8, r12, r11	@Hd + Bdd
	sub	r10, r12, r11	@Hd - Bdd	
	add	r11, r9, r4	@Ed + Dd
	sub	r12, r9, r4	@Ed - Dd	
	add	r4, r3, r6	@Gd + Add
	sub	r9, r3, r6	@Gd - Add

@ r3, r6  free now
		
	mov	r2, r2, asr #8		@
	mov	r7, r7, asr #8		@
	mov	r8, r8, asr #8		@
	mov	r10, r10, asr #8	@
	mov	r11, r11, asr #8	@
	mov	r12, r12, asr #8	@
	mov	r4, r4, asr #8		@
	mov	r9, r9, asr #8		@

	strh	r2, [r0]
	strh	r8, [r0, #16]
	strh	r4, [r0, #32]
	strh	r11, [r0, #48]
	strh	r12, [r0, #64]
	strh	r9, [r0, #80]
	strh	r10, [r0, #96]
	strh	r7, [r0, #112]	
	mov		pc,lr
	@ENDP


	.align 4
Row8src_4x4:
@	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	cmp	r3, #0
	bne	allx1_4_t_lab_Rowsrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rowsrc_4x4	@ x2!=0
	ldr	r6, [sp, #4]		@ Src = [sp, #4]
	ldr	r8, [sp, #52]		@ src_stride = [sp, #52]
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r4, [sp, #8]		@ dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r4, r7, r4	
	str	r8, [sp, #4]		@ Src = [sp, #4]
	str	r4, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rowsrc_4x4	@ x1!=0
@if(src)
@{
@	if(dst !=src )
@	{
@		dst[0] = src[0]@
@		dst[1] = src[1]@
@		dst[2] = src[2]@
@		dst[3] = src[3]@
@		dst[4] = src[4]@
@		dst[5] = src[5]@
@		dst[6] = src[6]@
@		dst[7] = src[7]@
@	}
@	src += stride@
@}	
	cmp	r6, r7	
	moveq	pc, lr
	ldr	r2, [r6]
	ldr	r3, [r6, #4]								 		
@	strd		r2, [r7]
	str	r2, [r7]
	str	r3, [r7, #4]		
	mov	pc, lr

only_x1t_lab_Rowsrc_4x4:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@        dst[0] = SAT(((src[0] + E)))@
@        dst[7] = SAT(((src[7] + E)))@
@
@        dst[1] = SAT(((src[1] + E)))@
@        dst[2] = SAT(((src[2] + E)))@
@
@        dst[3] = SAT(((src[3] + E)))@
@        dst[4] = SAT(((src[4] + E)))@
@
@        dst[5] = SAT(((src[5] + E)))@
@        dst[6] = SAT(((src[6] + E)))@
@	src += stride@
@    }
@}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	add	r1, r2, #32

	ldr	r8, [r6]
	ldr	r4, [r6, #4]
	movs	r1, r1, asr #6	
	streq	r8, [r7]
	streq	r4, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
@	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
@	uxtb16		r4, r2			@0, 2	a
@	uxtb16		r5, r2, ror #8		@1, 3	a
@	uxtb16		r6, r3			@0, 2	b
@	uxtb16		r8, r3, ror #8		@1, 3	b		
@	sadd16		r4, r4, r1		@0, 2	a+e
@	sadd16		r5, r5, r1		@1, 3	a+e
@	sadd16		r6, r6, r1		@0, 2	b+e
@	sadd16		r8, r8, r1		@1, 3	b+e		
@	usat16	 r4,#8,r4
@	usat16	 r5,#8,r5
@	usat16	 r6,#8,r6
@	usat16	 r8,#8,r8		
@	orr	r2,r4,r5,lsl #8
@	orr	r3,r6,r8,lsl #8
@	strd		r2, [r7]						
@	mov	pc, lr	

        ldr     r10, MaskCarry 
	blt     armv4_little_begin_4x4 
armv4_big_begin_4x4:	
                                      
	@v |= v << 8@
        orr     r12, r1, r1, lsl #8            
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16    	        

	@a = ((uint32_t*)Src)[0]@                
@        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
@        ldr     r4, [r3], r6                                       
	@b = a + v@
        add     r3, r5, r8
	@c = a & v@
        and     r1, r5, r8
	@a ^= v@	
        eor     r9, r8, r5
	@a &= ~b@
        mvn     r8, r3
        and     r8, r9, r8
	@a |= c@	
        orr     r1, r8, r1
	@a &= MaskCarry@
        and     r12, r1, r10
	@c = a << 1@	b -= c@	
        sub     r3, r3, r12, lsl #1
	@b |= c - (a >> 7)@
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1
        orr     r11, r3, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r1, r5, r4
	@d ^= v@	
        eor     r8, r4, r5
	@d &= ~b@
        mvn     r4, r12
        and     r4, r8, r4
	@d |= c@
        orr     r4, r1, r4
	@d &= MaskCarry@
        and     r4, r4, r10
	@c = d << 1@	b -= c@	
        sub     r3, r12, r4, lsl #1
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1
        orr     r12, r3, r12 		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r7]
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r7, #4]
	mov	pc, lr      
		
armv4_little_begin_4x4:
                      
        rsb   r12, r1, #0
	@v |= v << 8@
        orr     r12, r12, r12, lsl #8
	@v |= v << 16@
        orr     r5, r12, r12, lsl #16           

	@a = ((uint32_t*)Src)[0]@                
@        ldr     r8, [r3], #4  
	@d = ((uint32_t*)Src)[1]@
@        ldr     r4, [r3], r6                     
	@a = ~a@	
        mvn   r8, r8 
       	mvn   r4, r4                                      
	@b = a + v@
        add     r3, r5, r8                      
	@c = a & v@
        and     r1, r5, r8                        
	@a ^= v@	
        eor     r9, r8, r5                        
	@a &= ~b@
        mvn     r8, r3                           
        and     r8, r9, r8                        
	@a |= c@	
        orr     r1, r8, r1                        
	@a &= MaskCarry@
        and     r12, r1, r10                      
	@c = a << 1@	b -= c@	
        sub     r3, r3, r12, lsl #1             
	@b |= c - (a >> 7)@
        mov     r1, r12, lsr #7                   
        rsb     r12, r1, r12, lsl #1              
        orr     r11, r3, r12
	@b = d + v@
        add     r12, r5, r4
	@c = d & v@	
        and     r1, r5, r4                        
	@d ^= v@	
        eor     r8, r4, r5                        
	@d &= ~b@
        mvn     r4, r12                           
        and     r4, r8, r4                        
	@d |= c@
        orr     r4, r1, r4                        
	@d &= MaskCarry@
        and     r4, r4, r10                       
	@c = d << 1@	b -= c@	
        sub     r3, r12, r4, lsl #1              
	@b |= c - (d >> 7)@
        mov     r12, r4, lsr #7                   
        rsb     r12, r12, r4, lsl #1              
        orr     r12, r3, r12                  
	@b = ~b@	
        mvn   r11, r11  
	@b = ~b@
        mvn   r12, r12	      		                        
	@((uint32_t*)Dst)[0] = b@
        str     r11, [r7]
	@((uint32_t*)Dst)[1] = b@
        str     r12, [r7, #4]
	mov	pc, lr
	
only_x1x2t_lab_Rowsrc_4x4:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((Src[0] + ((E + A )>>17))))@
@	Dst[7] = SAT_new(((Src[7] + ((E - A )>>17))))@	
@	Dst[1] = SAT_new(((Src[1] + ((E + Bdd )>>17))))@
@	Dst[6] = SAT_new(((Src[6] + ((E - Bdd )>>17))))@	
@	Dst[3] = SAT_new(((Src[3] + ((E + B )>>17))))@
@	Dst[4] = SAT_new(((Src[4] + ((E - B )>>17))))@	
@	Dst[2] = SAT_new(((Src[2] + ((E + Add )>>17))))@
@	Dst[5] = SAT_new(((Src[5] + ((E - Add )>>17))))@		 				
@}

@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	@E = (Blk[0] + 32) << 11@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr


allx1_4_t_lab_Rowsrc:
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@
@ r2 = x1|x0 r3 = x3|x2
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6
	@	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@-D = M(xC5S3, ip[3*8])

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	add	r6, r2, r4	@Bd = B - D@
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	sub	r4, r2, r4	@Dd = B + D@
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		@G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		@H = M(xC6S2, ip[2*8])	
@ r9, r10, r3, r7 free now	G = r12, H = r2
	add	r3, r1, #32	@E = (Blk[0*8] + 32) << 11
	mov	r3, r3, asl #11
				
@ r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	add	r12, r3, r2	@Hd = F + H@	
	sub	r3, r3, r2	@Gd = F - H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	@Fd + Cd
	sub	r2, r10, r8	@Fd - Cd
	add	r5, r9, r4	@Ed + Dd
	sub	r7, r9, r4	@Ed - Dd	r6	

	add	r8, r12, r11	@Hd + Bdd
	sub	r4, r12, r11	@Hd - Bdd	

	add	r9, r3, r6	@Gd + Add
	sub	r3, r3, r6	@Gd - Add
	mov	r6, r7
	
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #17
	add     r8, r11, r8, asr #17
	add     r9, r12, r9, asr #17
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #17
	add     r6, r11, r6, asr #17
	add     r3, r12, r3, asr #17
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #17
	add     r2, r11, r2, asr #17							

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00							
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP

	.align 4
Row8nosrc_4x4:
@	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	cmp	r3, #0
	bne	allx1_4_t_lab_Rownosrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rownosrc_4x4	@ x2!=0
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r4, [sp, #8]		@ dst_stride = [sp, #8]
	add	r4, r7, r4	
	str	r4, [sp, #12]		@ dst = [sp, #12]		
	cmp	r2, #0
	bne	only_x1t_lab_Rownosrc_4x4	@ x1!=0
@@{
@	Dst[0] =
@	Dst[7] =
@	Dst[1] =
@	Dst[6] =
@	Dst[3] =
@	Dst[4] =
@	Dst[2] =
@	Dst[5] = 0@
@@}	
	mov	r2, #0
@	strd		r2, [r7]
	str	r2, [r7]
	str	r2, [r7, #4]			
	mov	pc, lr
	
only_x1t_lab_Rownosrc_4x4:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = (Blk[0] + 32)>>6@
@@        dst[0] =
@@        dst[7] =
@@
@@        dst[1] =
@@        dst[2] =
@@
@@        dst[3] =
@@        dst[4] =
@@
@@        dst[5] =
@@        dst[6] = SAT_new(E)@@
@}
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	add	r1, r2, #32
	mov	r2, r1, asr #6
	mov	r11, #0xFFFFFF00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00	
	orr     r2, r2, r2, lsl #8
	orr     r2, r2, r2, lsl #16
	str	r2, [r7]						
	str	r2, [r7, #4]						
	mov	pc, lr	
	
only_x1x2t_lab_Rownosrc_4x4:
@//ip0,1		@ r2 = x1|x0
@{
@	A = W1 * Blk[1*8]@
@	B = W7 * Blk[1*8]@
@@	E = Blk[0*8] << 11@	@E = (Blk[0] + 32) << 11
@@	E += 128@		
@	Add = 181 * ((A - B) >> 8)@		
@	Bdd = 181 * ((A + B) >> 8)@	
@	Dst[0] = SAT_new(((E + A )>>17))@
@	Dst[7] = SAT_new(((E - A )>>17))@	
@	Dst[1] = SAT_new(((E + Bdd )>>17))@
@	Dst[6] = SAT_new(((E - Bdd )>>17))@	
@	Dst[3] = SAT_new(((E + B )>>17))@
@	Dst[4] = SAT_new(((E - B )>>17))@	
@	Dst[2] = SAT_new(((E + Add )>>17))@
@	Dst[5] = SAT_new(((E - Add )>>17))@
@}
@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	mov	r12, #0x00b5	@WxW4	
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@
	add	r2, r2, #32	
	mov	r7, r2, lsl #11	@E = (Blk[0] + 32) << 11@
	sub	r3, r5, r6
	add	r4, r5, r6
	mov	r3, r3, asr #8	@(A - B)>> 8
	mov	r4, r4, asr #8	@(A + B)>> 8
	mul	r3, r12, r3	@Add = 181 * ((A - B)>> 8)@
	mul	r4, r12, r4	@Bdd = 181 * ((A + B)>> 8)@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r8, r7, r4	@E + Bdd
	sub	r4, r7, r4	@E - Bdd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r9, r7, r3	@E + Add
	sub	r3, r7, r3	@E - Add
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17

@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r7, r10, r11, r12 free now	
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00								
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	
allx1_4_t_lab_Rownosrc:
@//ip0,1,2,3                                             
@	Cd = A + C@	W1 * Blk[1*8] + W3 * Blk[3*8]
@	Dd = B + D@	W7 * Blk[1*8] - W5 * Blk[3*8]
@	Ad = A - C@	W1 * Blk[1*8] - W3 * Blk[3*8]
@	Bd = B - D@	W7 * Blk[1*8] + W5 * Blk[3*8]
@	Add = (181 * (((Ad - Bd)) >> 8))@		
@	Bdd = (181 * (((Ad + Bd)) >> 8))@				
@@	E = Blk[0*8] << 11@	E = (Blk[0] + 32) << 11@
@@	E += 128@		
@	G = W2 * Blk[2*8]@		
@	H = W6 * Blk[2*8]@ 
@	Ed = E - G@
@	Fd = E + G@
@	Gd = E - H@		
@	Hd = E + H@
@	Blk[0*8] = (idct_t)((Fd + Cd) >> 8)@
@	Blk[7*8] = (idct_t)((Fd - Cd) >> 8)@		
@	Blk[1*8] = (idct_t)((Hd + Bdd) >> 8)@
@	Blk[6*8] = (idct_t)((Hd - Bdd) >> 8)@
@	Blk[3*8] = (idct_t)((Ed + Dd) >> 8)@
@	Blk[4*8] = (idct_t)((Ed - Dd) >> 8)@				
@	Blk[2*8] = (idct_t)((Gd + Add) >> 8)@
@	Blk[5*8] = (idct_t)((Gd - Add) >> 8)@
@ r2 = x1|x0 r3 = x3|x2
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6
	@	ip0~ip7 = r1~8
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16					

@ r9, r10, r11, r12 free now
	ldr	r9, WxW1
	ldr	r10, WxW7

	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
	
@ r9, r10, r11, r8 free now		
	ldr	r9, WxW3
	ldr	r10, WxW5

	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@(-D) = M(xC5S3, ip[3*8])

@ r9, r10, r11, r6 free now
	mov	r10, #0x00b5	@WxW4
	sub	r11, r12, r8	@Ad = A - C
	add	r6, r2, r4	@Bd = B - D@
	sub	r9, r11, r6
	add	r11, r11, r6
	mov	r6, r9, asr #8	@(Ad - Bd)) >> 8
	mov	r11, r11, asr #8	@(Ad + Bd)) >> 8
	mul	r6, r10, r6	@Add = (181 * (((Ad - Bd)) >> 8))@
	mul	r11, r10, r11	@Bdd = (181 * (((Ad + Bd)) >> 8))@	
		
	add	r8, r12, r8	@Cd = A + C
	sub	r4, r2, r4	@Dd = B + D@
@ r9, r10, r2, r12 free now	Add = r6, Bdd = r11, Cd = r8, Dd = r4		
	ldr	r9, WxW2
	ldr	r10, WxW6

	mul     r12, r9,  r3		@G = M(xC2S6, ip[2*8])
	mul     r2,  r10, r3		@H = M(xC6S2, ip[2*8])	
@ r9, r10, r3, r7 free now	G = r12, H = r2
	add	r3, r1, #32	@E = (Blk[0*8] + 32) << 11
	mov	r3, r3, asl #11
				
@ r9, r10 free now		E = r3, F = r3
	sub	r9, r3, r12	@Ed = E - G@
	add	r10, r3, r12	@Fd = E + G@
	add	r12, r3, r2	@Hd = F + H@	
	sub	r3, r3, r2	@Gd = F - H@
	
@ r7, r2  free now     		@Ed = r9, Fd = r10, Gd = r3, Hd = r12
				@Add = r6, Bdd = r11, Cd = r8, Dd = r4
				
	add	r1, r10, r8	@Fd + Cd
	sub	r2, r10, r8	@Fd - Cd
	add	r5, r9, r4	@Ed + Dd
	sub	r7, r9, r4	@Ed - Dd	r6
		
	add	r8, r12, r11	@Hd + Bdd
	sub	r4, r12, r11	@Hd - Bdd
		
	add	r9, r3, r6	@Gd + Add
	sub	r3, r3, r6	@Gd - Add
	mov	r6, r7
	
	mov     r1, r1, asr #17
	mov     r8, r8, asr #17
	mov     r9, r9, asr #17
	mov     r5, r5, asr #17
	mov     r6, r6, asr #17
	mov     r3, r3, asr #17
	mov     r4, r4, asr #17
	mov     r2, r2, asr #17	
@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now

	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r10, [sp, #8]		@ dst_stride = [sp, #8]			
	mov	r11, #0xFFFFFF00
	tst     r1, r11
	movne	r1, #0xFF
	movmi	r1, #0x00
	tst     r8, r11
	movne	r8, #0xFF
	movmi	r8, #0x00
	tst     r9, r11
	movne	r9, #0xFF
	movmi	r9, #0x00
	tst     r5, r11
	movne	r5, #0xFF
	movmi	r5, #0x00
	tst     r6, r11
	movne	r6, #0xFF
	movmi	r6, #0x00
	tst     r3, r11
	movne	r3, #0xFF
	movmi	r3, #0x00
	tst     r4, r11
	movne	r4, #0xFF
	movmi	r4, #0x00
	tst     r2, r11
	movne	r2, #0xFF
	movmi	r2, #0x00								
@	r1, r8, r9, r5, r6, r3, r4, r2							
@	r11, r12 free now
	orr     r1, r1, r8, lsl #8
	orr     r1, r1, r9, lsl #16
	orr     r1, r1, r5, lsl #24
	
	orr     r3, r6, r3, lsl #8
	orr     r3, r3, r4, lsl #16
	orr     r3, r3, r2, lsl #24
	str	r1, [r7]
	str	r3, [r7, #4]	
	add	r7, r7, r10
	str	r7, [sp, #12]		@ dst = [sp, #12]
	mov	pc, lr
	@ENDP

	.align 4
ArmIdctA:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]
	
	bl      Col8_4x4  
	add     r0, r0, #2
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  
	add     r0, r0, #2	
	bl      Col8_4x4  	
	sub     r0, r0, #6
	
	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_lab_4x4		@ Src=0	

	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	sub     r0, r0, #112
		
	mov	r10, #0
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0]
        str     r10, [r0, #4]
                                          		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
only_noSrc_lab_4x4:

	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	
	sub     r0, r0, #112	
	mov	r10, #0
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0]
        str     r10, [r0, #4]
                                         		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        	
	@ENDP

	.align 4
IdctBlock8x8_ARMV4:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]	

	bl      Col8  
	add     r0, r0, #2
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8 
	sub     r0, r0, #14
	
	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_lab		@ Src=0	

	bl		Row8src
	add		r0, r0, #16			@Block += 16		
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	add		r0, r0, #16			@Block += 16	
	bl		Row8src
	add		r0, r0, #16			@Block += 16
	bl		Row8src
	sub     r0, r0, #112	
	mov	r10, #0
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
	
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0]
        str	r10, [r0, #4]                                            		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
        
only_noSrc_lab:
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc
	sub     r0, r0, #112	
	mov	r10, #0
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
	
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0], #4
        str	r10, [r0]
        str	r10, [r0, #4]                                        		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
	.align 4
IdctBlock4x8_ARMV4:
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]	

	bl      Col8  
	add     r0, r0, #2
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8  
	add     r0, r0, #2	
	bl      Col8
	sub     r0, r0, #6

	ldr	r8, [sp, #4]		@ Src = [sp, #4]	
 	cmp	r8, #0				
	beq	only_noSrc_labD		@ Src=0	

	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8src_4x4
	add		r0, r0, #16			@Block += 16
	bl		Row8src_4x4
	sub     r0, r0, #112	
	mov	r10, #0
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0]                                          		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}
        
        
only_noSrc_labD:
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16		
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	add		r0, r0, #16			@Block += 16	
	bl		Row8nosrc_4x4
	sub     r0, r0, #112	
	mov	r10, #0
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0], #16
        str     r10, [r0, #4]
        str     r10, [r0]                                  		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}

	@ENDP
	.END
