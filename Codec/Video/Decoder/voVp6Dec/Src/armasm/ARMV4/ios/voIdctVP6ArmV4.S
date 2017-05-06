@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/
	@AREA    |.text|, CODE, READONLY
	.text
	.align 4
	.globl _VP6DEC_VO_Armv4IdctA
	.globl _VP6DEC_VO_Armv4IdctB
	.globl _VP6DEC_VO_Armv4IdctC
		
	.align 4
Col8: @PROC
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
@	E = M(xC4S4, ip[0*8])@
@	E = E>>15@
@	ip[0~7*8] = E@
	ldr	r12, WxW4    	
    	mul	r5, r1, r12
	mov	r5, r5, asr #15
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
@	A = M(xC1S7, ip[1*8])@
@	B = M(xC7S1, ip[1*8])@
@	E = M(xC4S4, ip[0*8])@	
@	Ad = M(xC4S4, A>>15)@
@	Bd = M(xC4S4, B>>15)@	
@	Gd = E - Ad@
@	Add = E + Ad@	
@	/*  Final sequence of operations over-write original inputs. */
@	ip[0*8] = (E + A)>>15 @
@	ip[7*8] = (E - A)>>15 @	
@	ip[1*8] = (Add + Bd)>>15@
@	ip[2*8] = (Add - Bd)>>15@	
@	ip[3*8] = (E + B)>>15 @
@	ip[4*8] = (E - B)>>15 @	
@	ip[5*8] = (Gd + Bd)>>15@
@	ip[6*8] = (Gd - Bd)>>15@

	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4		  				
	mul	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r1, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r3, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r7, r8, r4	@Gd + Bd
	sub	r8, r8, r4	@Gd - Bd
	
	mov	r1, r1, asr #15	@
	mov	r2, r2, asr #15	@
	mov	r3, r3, asr #15	@
	mov	r9, r9, asr #15	@
	mov	r5, r5, asr #15	@
	mov	r6, r6, asr #15	@
	mov	r7, r7, asr #15	@
	mov	r8, r8, asr #15	@
	
	strh	r1, [r0]
	strh	r3, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r7, [r0, #80]
	strh	r8, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr

allx1_8_t_lab:
@//ip0,1,2,3 4,5,6,7
@            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
@            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
@            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@            E = M(xC4S4, (ip[0*8] + ip[4*8]))@
@            F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
@            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
@            Ad = M(xC4S4, (A - C)>>15)@
@            Bd = M(xC4S4, (B - D)>>15)@
@            Cd = A + C@
@            Dd = B + D@
@            Ed = E - G@
@            Fd = E + G@
@            Gd = F - Ad@
@            Hd = Bd + H@
@            Add = F + Ad@
@            Bdd = Bd - H@
@            /*  Final sequence of operations over-write original inputs. */
@            ip[0*8] = (Fd + Cd)>>15 @
@            ip[7*8] = (Fd - Cd)>>15 @
@            ip[1*8] = (Add + Hd)>>15@
@            ip[2*8] = (Add - Hd)>>15@
@            ip[3*8] = (Ed + Dd)>>15 @
@            ip[4*8] = (Ed - Dd)>>15 @
@            ip[5*8] = (Gd + Bdd)>>15@
@            ip[6*8] = (Gd - Bdd)>>15@

@@r1(ip0), r2(ip1), r3~8 (ip3~ip7)
@r9, r10, r11, r12 free now
	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r11, r4,  r9		@M(xC3S5, ip[3*8])
	mla     r11, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r12,  r6, r9		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r12, r12, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
	
@r9, r10, r4, r6 free now		
	ldr	r9, WxW1
	ldr	r10, WxW7
	mul     r4, r2,  r9		@M(xC1S7, ip[1*8])
	mla     r4, r10, r8, r4		@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	mul     r6,  r2, r10		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r6, r6, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@r9, r10, r2, r8 free now
	ldr	r9, WxW4
	sub	r2, r4, r11	@A - C@
	add	r8, r11, r4	@@Cd = A + C@	
	sub	r11, r6, r12	@B - D@
	add	r4, r6, r12	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r11, r11, asr #15	@(B - D)>>15
	mul	r2, r9, r2	@@Ad = M(xC4S4, (A - C)>>15)@
	mul	r11, r9, r11	@@Bd = M(xC4S4, (B - D)>>15)@	
			
@ r6 r12 r10 free now
	ldr	r12, WxW2
	ldr	r10, WxW6
	mul     r6, r3,  r12		@M(xC2S6, ip[2*8])
	mla     r6, r10, r7, r6		@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r10,  r3, r10		@M(xC6S2, ip[2*8])
	mul     r7,  r12,  r7		@M(xC2S6, ip[6*8])
	sub	r12, r10, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@

@ r3 r7 r10 free now
	add	r3, r1, r5
	sub	r7, r1, r5	
	mul     r3, r9,  r3		@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	mul     r7,  r9, r7		@F = M(xC4S4, (ip[0*8] - ip[4*8]))@	
	
@ r9 r10 free now
	
	sub	r9, r3, r6		@@Ed = E - G@
	add	r10, r3, r6		@@Fd = E + G@

@ r3 r6 free now
	sub	r3, r7, r2	@@Gd = F - Ad@
	add	r6, r7, r2	@@Add = F + Ad@	
	sub	r7, r11, r12	@@Bdd = Bd - H@	
	add	r2, r11, r12	@@Hd = Bd + H@
	
@ r11 r12 free now
	add	r11, r10, r8	@Fd + Cd
	sub	r12, r10, r8	@Fd - Cd
	add	r10, r6, r2	@Add + Hd
	sub	r8, r6, r2	@Add - Hd	
	add	r6, r9, r4	@Ed + Dd
	sub	r2, r9, r4	@Ed - Dd	
	add	r9, r3, r7	@Gd + Bdd
	sub	r4, r3, r7	@Gd - Bdd
		
	mov	r11, r11, asr #15	@
	mov	r12, r12, asr #15	@
	mov	r10, r10, asr #15	@
	mov	r8, r8, asr #15	@
	mov	r6, r6, asr #15	@
	mov	r2, r2, asr #15	@
	mov	r9, r9, asr #15	@
	mov	r4, r4, asr #15	@

	strh	r11, [r0]
	strh	r10, [r0, #16]
	strh	r8, [r0, #32]
	strh	r6, [r0, #48]
	strh	r2, [r0, #64]
	strh	r9, [r0, #80]
	strh	r4, [r0, #96]
	strh	r12, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src: @PROC
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
	@pld		[r8]		
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
@	E = M(xC4S4, ip[0])@
@		{  //HACK
@			E += 8<<15@
@		}
@    /* Final sequence of operations over-write original inputs. */
@	{
@	 E = E>>19@
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
	ldr	r12, WxW4    	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
    	mul	r1, r2, r12
			
	mov	r8, #8	
	add	r1, r1, r8, asl #15

	
	ldr	r8, [r6]
	ldr	r4, [r6, #4]
	movs	r1, r1, asr #19
	streq	r8, [r7]
	streq	r4, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
@	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
@	uxtb16		r4, r2			@0, 2	a
@	uxtb16		r5, r2, ror #8		@1, 3	a
@	uxtb16		r6, r3			@0, 2	b
@	uxtb16		r8, r3, ror #8		@1, 3	b	
@	
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
@		A = M(xC1S7, ip[1])@
@		B = M(xC7S1, ip[1])@
@		E = M(xC4S4, ip[0])@		
@		{  //HACK
@			E += 8<<15@
@		}						
@		Ad = M(xC4S4, A>>15)@
@		Bd = M(xC4S4, B>>15)@
@		Gd = E - Ad@
@		Add = E + Ad@
@	
@            /* Final sequence of operations over-write original inputs. */
@            {
@                dst[0] = SAT(((src[0] + ((E + A )  >> 19))))@
@                dst[7] = SAT(((src[7] + ((E - A )  >> 19))))@
@
@                dst[1] = SAT(((src[1] + ((Add + Bd ) >> 19))))@
@                dst[2] = SAT(((src[2] + ((Add - Bd ) >> 19))))@
@
@                dst[3] = SAT(((src[3] + ((E + B )  >> 19))))@
@                dst[4] = SAT(((src[4] + ((E - B )  >> 19))))@
@
@                dst[5] = SAT(((src[5] + ((Gd + Bd ) >> 19))))@
@                dst[6] = SAT(((src[6] + ((Gd - Bd ) >> 19))))@
@		src += stride@
@            }			 				
@}
@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4
		
	mov	r8, #8
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8, asl #15
		
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r11, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r3, r8, r4	@Gd + Bd
	sub	r4, r8, r4	@Gd - Bd
	mov	r8, r11

@r1, r8, r9, r5, r6, r3, r4, r2
@r1, r8, r9, r5, r6, r3, r4, r2	

@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #19
	add     r8, r11, r8, asr #19
	add     r9, r12, r9, asr #19
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #19
	add     r6, r11, r6, asr #19
	add     r3, r12, r3, asr #19
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #19
	add     r2, r11, r2, asr #19							

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
@{
@    A = M(xC1S7, ip[1]) + M(xC7S1, ip[7])@
@    B = M(xC7S1, ip[1]) - M(xC1S7, ip[7])@
@    C = M(xC3S5, ip[3]) + M(xC5S3, ip[5])@
@    D = M(xC3S5, ip[5]) - M(xC5S3, ip[3])@
@    E = M(xC4S4, (ip[0] + ip[4]))@
@    F = M(xC4S4, (ip[0] - ip[4]))@
@	{  //HACK
@		E += 8<<15@
@		F += 8<<15@				
@	}	
@    G = M(xC2S6, ip[2]) + M(xC6S2, ip[6])@
@    H = M(xC6S2, ip[2]) - M(xC2S6, ip[6])@
@    Ad = M(xC4S4, (A - C)>>15)@
@    Bd = M(xC4S4, (B - D)>>15)@
@    Cd = A + C@
@    Dd = B + D@
@    Ed = E - G@
@    Fd = E + G@
@    Gd = F - Ad@
@    Hd = Bd + H@
@    Add = F + Ad@
@    Bdd = Bd - H@
@    /* Final sequence of operations over-write original inputs. */
@    {
@        dst[0] = SAT(((src[0] + ((Fd + Cd )  >> 19))))@
@        dst[7] = SAT(((src[7] + ((Fd - Cd )  >> 19))))@
@        dst[1] = SAT(((src[1] + ((Add + Hd ) >> 19))))@
@        dst[2] = SAT(((src[2] + ((Add - Hd ) >> 19))))@
@        dst[3] = SAT(((src[3] + ((Ed + Dd )  >> 19))))@
@        dst[4] = SAT(((src[4] + ((Ed - Dd )  >> 19))))@
@        dst[5] = SAT(((src[5] + ((Gd + Bdd ) >> 19))))@
@        dst[6] = SAT(((src[6] + ((Gd - Bdd ) >> 19))))@
@	src += stride@
@    }	            
@}
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6

@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
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

	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r11, r4,  r9		@M(xC3S5, ip[3*8])
	mla     r11, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r12,  r6, r9		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r12, r12, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
	
@r9, r10, r4, r6 free now		
	ldr	r9, WxW1
	ldr	r10, WxW7
	mul     r4, r2,  r9		@M(xC1S7, ip[1*8])
	mla     r4, r10, r8, r4		@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	mul     r6,  r2, r10		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r6, r6, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@r9, r10, r2, r8 free now
	ldr	r9, WxW4
	sub	r2, r4, r11	@A - C@
	add	r8, r11, r4	@@Cd = A + C@	
	sub	r11, r6, r12	@B - D@
	add	r4, r6, r12	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r11, r11, asr #15	@(B - D)>>15
	mul	r2, r9, r2	@@Ad = M(xC4S4, (A - C)>>15)@
	mul	r11, r9, r11	@@Bd = M(xC4S4, (B - D)>>15)@	
			
@ r6 r12 r10 free now
	ldr	r12, WxW2
	ldr	r10, WxW6
	mul     r6, r3,  r12		@M(xC2S6, ip[2*8])
	mla     r6, r10, r7, r6		@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r10,  r3, r10		@M(xC6S2, ip[2*8])
	mul     r7,  r12,  r7		@M(xC2S6, ip[6*8])
	sub	r12, r10, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@

@ r3 r7 r10 free now
	add	r3, r1, r5
	sub	r7, r1, r5	
	mul     r3, r9,  r3		@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	mul     r7,  r9, r7		@F = M(xC4S4, (ip[0*8] - ip[4*8]))@	
	
@	{  //HACK
@		E += 8<<15@
@		F += 8<<15@				
@	}
	mov	r10, #8
	mov	r10, r10, asl #15
	add	r3, r3, r10	@E += 8<<15@
	add	r7, r7, r10	@F += 8<<15@		
	
@ r9 r10 free now
	
	sub	r9, r3, r6		@@Ed = E - G@
	add	r10, r3, r6		@@Fd = E + G@

@ r3 r6 free now
	sub	r3, r7, r2	@@Gd = F - Ad@
	add	r6, r7, r2	@@Add = F + Ad@	
	sub	r7, r11, r12	@@Bdd = Bd - H@	
	add	r2, r11, r12	@@Hd = Bd + H@
	
@        dst[0] = SAT(((src[0] + ((Fd + Cd )  >> 19))))@
@        dst[7] = SAT(((src[7] + ((Fd - Cd )  >> 19))))@
@        dst[1] = SAT(((src[1] + ((Add + Hd ) >> 19))))@
@        dst[2] = SAT(((src[2] + ((Add - Hd ) >> 19))))@
@        dst[3] = SAT(((src[3] + ((Ed + Dd )  >> 19))))@
@        dst[4] = SAT(((src[4] + ((Ed - Dd )  >> 19))))@
@        dst[5] = SAT(((src[5] + ((Gd + Bdd ) >> 19))))@
@        dst[6] = SAT(((src[6] + ((Gd - Bdd ) >> 19))))@	
@ r11 r12 free now
	add	r1, r10, r8	@Fd + Cd
	sub	r12, r10, r8	@Fd - Cd
	add	r8, r6, r2	@Add + Hd
	sub	r2, r6, r2	@Add - Hd	
	add	r5, r9, r4	@Ed + Dd
	sub	r6, r9, r4	@Ed - Dd	
	sub	r4, r3, r7	@Gd - Bdd
	add	r3, r3, r7	@Gd + Bdd
	
	mov	r9, r2	
	mov	r2, r12	

@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #19
	add     r8, r11, r8, asr #19
	add     r9, r12, r9, asr #19
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #19
	add     r6, r11, r6, asr #19
	add     r3, r12, r3, asr #19
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #19
	add     r2, r11, r2, asr #19							

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
Row8nosrc: @PROC
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
@@	dst[0]=
@@	dst[1]=
@@	dst[2]=
@@	dst[3]=
@@	dst[4]=
@@	dst[5]=
@@	dst[6]=
@@	dst[7]= 128@					
@@}	
	ldr	r2, MaskCarry
	mov	r3, r2
	str	r2, [r7]
	str	r2, [r7, #4]		
	mov	pc, lr
	
only_x1t_lab_Rownosrc:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = M(xC4S4, ip[0])@
@		{  //HACK
@@			E += (16*128 + 8)<<15@
@		}
@    /* Final sequence of operations over-write original inputs. */
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
@@        dst[6] = SAT(((E ) >> 19))@
@}
	ldr	r12, WxW4	
	@mov	r8, #0x0808
	ldr		r8, Wx04040000	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
    	mul	r1, r2, r12
	add	r1, r1, r8
	mov	r2, r1, asr #19
	
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
@		A = M(xC1S7, ip[1])@
@		B = M(xC7S1, ip[1])@
@		E = M(xC4S4, ip[0])@		
@		{  //HACK
@@			E += (16*128 + 8)<<15@
@		}						
@		Ad = M(xC4S4, A>>15)@
@		Bd = M(xC4S4, B>>15)@
@		Gd = E - Ad@
@		Add = E + Ad@
@	
@            /* Final sequence of operations over-write original inputs. */
@@            if(!src){
@@                dst[0] = SAT(((E + A )  >> 19))@
@@                dst[7] = SAT(((E - A )  >> 19))@
@@
@@                dst[1] = SAT(((Add + Bd ) >> 19))@
@@                dst[2] = SAT(((Add - Bd ) >> 19))@
@@
@@                dst[3] = SAT(((E + B )  >> 19))@
@@                dst[4] = SAT(((E - B )  >> 19))@
@@
@@                dst[5] = SAT(((Gd + Bd ) >> 19))@
@@                dst[6] = SAT(((Gd - Bd ) >> 19))@
@@            }		 				
@}
@ r2 = x1|x0
	@mov	r8, #0x0808
	ldr		r8, Wx04040000
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4
		
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8
		
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r11, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r3, r8, r4	@Gd + Bd
	sub	r4, r8, r4	@Gd - Bd
	mov	r8, r11

@1, 3(8), 9, 5, 6, 7(3), 8(4), 2						

	mov     r1, r1, asr #19
	mov     r8, r8, asr #19
	mov     r9, r9, asr #19
	mov     r5, r5, asr #19
	mov     r6, r6, asr #19
	mov     r3, r3, asr #19
	mov     r4, r4, asr #19
	mov     r2, r2, asr #19
	
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
@{
@    A = M(xC1S7, ip[1]) + M(xC7S1, ip[7])@
@    B = M(xC7S1, ip[1]) - M(xC1S7, ip[7])@
@    C = M(xC3S5, ip[3]) + M(xC5S3, ip[5])@
@    D = M(xC3S5, ip[5]) - M(xC5S3, ip[3])@
@    E = M(xC4S4, (ip[0] + ip[4]))@
@    F = M(xC4S4, (ip[0] - ip[4]))@
@	{  //HACK
@		E += (16*128 + 8)<<15@
@		F += (16*128 + 8)<<15@				
@	}	
@    G = M(xC2S6, ip[2]) + M(xC6S2, ip[6])@
@    H = M(xC6S2, ip[2]) - M(xC2S6, ip[6])@
@    Ad = M(xC4S4, (A - C)>>15)@
@    Bd = M(xC4S4, (B - D)>>15)@
@    Cd = A + C@
@    Dd = B + D@
@    Ed = E - G@
@    Fd = E + G@
@    Gd = F - Ad@
@    Hd = Bd + H@
@    Add = F + Ad@
@    Bdd = Bd - H@
@    /* Final sequence of operations over-write original inputs. */
@@            if(!src){
@@                dst[0] = SAT(((E + A )  >> 19))@
@@                dst[7] = SAT(((E - A )  >> 19))@
@@
@@                dst[1] = SAT(((Add + Bd ) >> 19))@
@@                dst[2] = SAT(((Add - Bd ) >> 19))@
@@
@@                dst[3] = SAT(((E + B )  >> 19))@
@@                dst[4] = SAT(((E - B )  >> 19))@
@@
@@                dst[5] = SAT(((Gd + Bd ) >> 19))@
@@                dst[6] = SAT(((Gd - Bd ) >> 19))@
@@            }	            
@}
@@	r2 = x1|x0 r3 = x3|x2
@@	r4 = x5|x4 r5 = x7|x6


@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
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

	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r11, r4,  r9		@M(xC3S5, ip[3*8])
	mla     r11, r10, r6, r11	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	mul     r12,  r6, r9		@M(xC3S5, ip[5*8])
	mul     r4,  r10,  r4		@M(xC5S3, ip[3*8])
	sub	r12, r12, r4		@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
	
@r9, r10, r4, r6 free now		
	ldr	r9, WxW1
	ldr	r10, WxW7
	mul     r4, r2,  r9		@M(xC1S7, ip[1*8])
	mla     r4, r10, r8, r4		@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	mul     r6,  r2, r10		@M(xC7S1, ip[1*8])
	mul     r8,  r9,  r8		@M(xC1S7, ip[7*8])
	sub	r6, r6, r8		@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@	
	
@r9, r10, r2, r8 free now
	ldr	r9, WxW4
	sub	r2, r4, r11	@A - C@
	add	r8, r11, r4	@@Cd = A + C@	
	sub	r11, r6, r12	@B - D@
	add	r4, r6, r12	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r11, r11, asr #15	@(B - D)>>15
	mul	r2, r9, r2	@@Ad = M(xC4S4, (A - C)>>15)@
	mul	r11, r9, r11	@@Bd = M(xC4S4, (B - D)>>15)@	
			
@ r6 r12 r10 free now
	ldr	r12, WxW2
	ldr	r10, WxW6
	mul     r6, r3,  r12		@M(xC2S6, ip[2*8])
	mla     r6, r10, r7, r6		@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	mul     r10,  r3, r10		@M(xC6S2, ip[2*8])
	mul     r7,  r12,  r7		@M(xC2S6, ip[6*8])
	sub	r12, r10, r7		@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@

@ r3 r7 r10 free now
	add	r3, r1, r5
	sub	r7, r1, r5	
	mul     r3, r9,  r3		@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	mul     r7,  r9, r7		@F = M(xC4S4, (ip[0*8] - ip[4*8]))@	
	
@	{  //HACK
@		E += (16*128 + 8)<<15@
@		F += (16*128 + 8)<<15@				
@	}
	ldr		r10, Wx04040000
	add	r3, r3, r10	@E += 8<<15@
	add	r7, r7, r10	@F += 8<<15@	
	
@ r9 r10 free now
	
	sub	r9, r3, r6		@@Ed = E - G@
	add	r10, r3, r6		@@Fd = E + G@

@ r3 r6 free now
	sub	r3, r7, r2	@@Gd = F - Ad@
	add	r6, r7, r2	@@Add = F + Ad@	
	sub	r7, r11, r12	@@Bdd = Bd - H@	
	add	r2, r11, r12	@@Hd = Bd + H@
	
@        dst[0] = SAT(((src[0] + ((Fd + Cd )  >> 19))))@
@        dst[7] = SAT(((src[7] + ((Fd - Cd )  >> 19))))@
@        dst[1] = SAT(((src[1] + ((Add + Hd ) >> 19))))@
@        dst[2] = SAT(((src[2] + ((Add - Hd ) >> 19))))@
@        dst[3] = SAT(((src[3] + ((Ed + Dd )  >> 19))))@
@        dst[4] = SAT(((src[4] + ((Ed - Dd )  >> 19))))@
@        dst[5] = SAT(((src[5] + ((Gd + Bdd ) >> 19))))@
@        dst[6] = SAT(((src[6] + ((Gd - Bdd ) >> 19))))@	
@ r11 r12 free now
	add	r1, r10, r8	@Fd + Cd
	sub	r12, r10, r8	@Fd - Cd
	add	r8, r6, r2	@Add + Hd
	sub	r2, r6, r2	@Add - Hd	
	add	r5, r9, r4	@Ed + Dd
	sub	r6, r9, r4	@Ed - Dd	
	sub	r4, r3, r7	@Gd - Bdd
	add	r3, r3, r7	@Gd + Bdd
	
	mov	r9, r2	
	mov	r2, r12	

@	r1, r8, r9, r5, r6, r3, r4, r2
	mov     r1, r1, asr #19
	mov     r8, r8, asr #19
	mov     r9, r9, asr #19
	mov     r5, r5, asr #19
	mov     r6, r6, asr #19
	mov     r3, r3, asr #19
	mov     r4, r4, asr #19
	mov     r2, r2, asr #19	
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

@	.align 8	
@WxC1xC7		.word 0x7D8A18F8			@xC7 = D0.S16[0] xC1 = D0.S16[1]		
@WxC3xC5		.word 0x6A6D471D			@xC5 = D0.S16[2] xC3 = D0.S16[3]			
@WxC2xC6		.word 0x764130FC			@xC6 = D1.S16[0] xC2 = D1.S16[1]		
@WxC4xC4		.word 0x5A825A82			@xC4 = D1.S32[1]/D1.S32[2]
@Wx80808080	.word 0x80808080			@
@Wx04040000	.word 0x04040000 
@
@
@WxC5xC7		.word 0x471D18F8			@xC7 = D0.S16[0] xC5 = D0.S16[1]		
@WxC3xC1		.word 0x6A6D7D8A			@xC1 = D0.S16[2] xC3 = D0.S16[3]			
@WxC2xC4		.word 0x76415A82			@xC4 = D1.S16[0] xC2 = D1.S16[1]		
@WxC6xC6		.word 0x30FC30FC			@xC6 = D1.S32[1]/D1.S32[2]

	.align 8
WxW1:		.word 0x7D8A
WxW2:		.word 0x7641
WxW3:		.word 0x6A6D
WxW4:		.word 0x5A82
WxW5:		.word 0x471D
WxW6:		.word 0x30FC
WxW7:		.word 0x18F8
	
MaskCarry:	.word 0x80808080	@uint32_t MaskCarry = 0x80808080U@
Wx04040000:	.word 0x04040000		
	.align 4
Col8_4x4: @PROC
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
@	E = M(xC4S4, ip[0*8])@
@	E = E>>15@
@	ip[0~7*8] = E@    					    																																										    ip[7*8] =
	ldr	r12, WxW4
	mul	r5, r1, r12
	mov	r5, r5, asr #15
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
@	A = M(xC1S7, ip[1*8])@
@	B = M(xC7S1, ip[1*8])@
@	E = M(xC4S4, ip[0*8])@	
@	Ad = M(xC4S4, A>>15)@
@	Bd = M(xC4S4, B>>15)@	
@	Gd = E - Ad@
@	Add = E + Ad@	
@	/*  Final sequence of operations over-write original inputs. */
@	ip[0*8] = (E + A)>>15 @
@	ip[7*8] = (E - A)>>15 @	
@	ip[1*8] = (Add + Bd)>>15@
@	ip[2*8] = (Add - Bd)>>15@	
@	ip[3*8] = (E + B)>>15 @
@	ip[4*8] = (E - B)>>15 @	
@	ip[5*8] = (Gd + Bd)>>15@
@	ip[6*8] = (Gd - Bd)>>15@
			
	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4		  				
	mul	r5, r2, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r1, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r3, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r7, r8, r4	@Gd + Bd
	sub	r8, r8, r4	@Gd - Bd
	
	mov	r1, r1, asr #15	@
	mov	r2, r2, asr #15	@
	mov	r3, r3, asr #15	@
	mov	r9, r9, asr #15	@
	mov	r5, r5, asr #15	@
	mov	r6, r6, asr #15	@
	mov	r7, r7, asr #15	@
	mov	r8, r8, asr #15	@
	
	strh	r1, [r0]
	strh	r3, [r0, #16]
	strh	r9, [r0, #32]
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	strh	r7, [r0, #80]
	strh	r8, [r0, #96]
	strh	r2, [r0, #112]	
	mov	pc, lr

allx1_4_t_lab:            
@//ip0,1,2,3                                             
@            A = M(xC1S7, ip[1*8])@   
@            B = M(xC7S1, ip[1*8])@   
@            C = M(xC3S5, ip[3*8])@   
@            D = -M(xC5S3, ip[3*8])@  
@            E = M(xC4S4, ip[0*8])@                                        
@            G = M(xC2S6, ip[2*8])@   
@            H = M(xC6S2, ip[2*8])@                                      
@            Ad = M(xC4S4, (A - C)>>15)@
@            Bd = M(xC4S4, (B - D)>>15)@                                  
@            Cd = A + C@              
@            Dd = B + D@                                                
@            Ed = E - G@              
@            Fd = E + G@              
@            Gd = E - Ad@             
@            Hd = Bd + H@                                                
@            Add = E + Ad@            
@            Bdd = Bd - H@                                              
@            /*  Final sequence of operations over-write original inputs. */
@            ip[0*8] = (Fd + Cd)>>15 @
@            ip[7*8] = (Fd - Cd)>>15 @                                  
@            ip[1*8] = (Add + Hd)>>15@
@            ip[2*8] = (Add - Hd)>>15@                                   
@            ip[3*8] = (Ed + Dd)>>15 @
@            ip[4*8] = (Ed - Dd)>>15 @                                 
@            ip[5*8] = (Gd + Bdd)>>15@
@            ip[6*8] = (Gd - Bdd)>>15@	
	ldr	r9, WxW1
	ldr	r10, WxW7
	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
		
	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@(-D) = M(xC5S3, ip[3*8])
		
	ldr	r10, WxW4
	sub	r5, r12, r8	@A - C
	add	r6, r2, r4	@B - D@	B + (-D)
	mov	r5, r5, asr #15	@(A - C)>>15
	mov	r6, r6, asr #15	@(B - D)>>15	
	mul	r5, r10, r5	@Ad = M(xC4S4, (A - C)>>15)@
	mul	r6, r10, r6	@Bd = M(xC4S4, (B - D)>>15)@
			
	mul	r11, r1, r10	@E = M(xC4S4, ip[0*8])@
	ldr	r9, WxW2
	ldr	r10, WxW6
	mul     r9, r3,  r9	@G = M(xC2S6, ip[2*8])@
	mul     r10,  r3,  r10	@H = M(xC6S2, ip[2*8])@
	
	add	r7, r12, r8	@@Cd = A + C@
	sub	r8, r2, r4	@@Dd = B + D@	
	sub	r1, r11, r9	@@Ed = E - G@
	add	r9, r11, r9	@@Fd = E + G@ 
	sub	r12, r11, r5	@@Gd = E - Ad@
	add	r11, r11, r5	@@Add = E + Ad@	
	sub	r5, r6, r10	@@Bdd = Bd - H@
	add	r6, r6, r10	@@Hd = Bd + H@ 

	add	r2, r9, r7	@Fd + Cd
	sub	r3, r9, r7	@Fd - Cd
	add	r4, r11, r6	@Add + Hd
	sub	r7, r11, r6	@Add - Hd	
	add	r9, r1, r8	@Ed + Dd
	sub	r6, r1, r8	@Ed - Dd	
	add	r1, r12, r5	@Gd + Bdd
	sub	r8, r12, r5	@Gd - Bdd

@ r3, r6  free now
		
	mov	r2, r2, asr #15		@
	mov	r3, r3, asr #15		@
	mov	r4, r4, asr #15		@
	mov	r7, r7, asr #15	@
	mov	r9, r9, asr #15	@
	mov	r6, r6, asr #15	@
	mov	r1, r1, asr #15		@
	mov	r8, r8, asr #15		@

	strh	r2, [r0]
	strh	r4, [r0, #16]
	strh	r7, [r0, #32]
	strh	r9, [r0, #48]
	strh	r6, [r0, #64]
	strh	r1, [r0, #80]
	strh	r8, [r0, #96]
	strh	r3, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src_4x4: @PROC
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
	@pld		[r8]		
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
@	E = M(xC4S4, ip[0])@
@		{  //HACK
@			E += 8<<15@
@		}
@    /* Final sequence of operations over-write original inputs. */
@	{
@	 E = E>>19@
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
	ldr	r12, WxW4    	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
    	mul	r1, r2, r12
			
	mov	r8, #8	
	add	r1, r1, r8, asl #15

	
	ldr	r8, [r6]
	ldr	r4, [r6, #4]
	movs	r1, r1, asr #19
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
@		A = M(xC1S7, ip[1])@
@		B = M(xC7S1, ip[1])@
@		E = M(xC4S4, ip[0])@		
@		{  //HACK
@			E += 8<<15@
@		}						
@		Ad = M(xC4S4, A>>15)@
@		Bd = M(xC4S4, B>>15)@
@		Gd = E - Ad@
@		Add = E + Ad@
@	
@            /* Final sequence of operations over-write original inputs. */
@            {
@                dst[0] = SAT(((src[0] + ((E + A )  >> 19))))@
@                dst[7] = SAT(((src[7] + ((E - A )  >> 19))))@
@
@                dst[1] = SAT(((src[1] + ((Add + Bd ) >> 19))))@
@                dst[2] = SAT(((src[2] + ((Add - Bd ) >> 19))))@
@
@                dst[3] = SAT(((src[3] + ((E + B )  >> 19))))@
@                dst[4] = SAT(((src[4] + ((E - B )  >> 19))))@
@
@                dst[5] = SAT(((src[5] + ((Gd + Bd ) >> 19))))@
@                dst[6] = SAT(((src[6] + ((Gd - Bd ) >> 19))))@
@		src += stride@
@            }			 				
@}
@ r2 = x1|x0
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4
		
	mov	r8, #8
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8, asl #15
		
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r11, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r3, r8, r4	@Gd + Bd
	sub	r4, r8, r4	@Gd - Bd
	mov	r8, r11

@r1, r8, r9, r5, r6, r3, r4, r2
@r1, r8, r9, r5, r6, r3, r4, r2	

@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #19
	add     r8, r11, r8, asr #19
	add     r9, r12, r9, asr #19
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #19
	add     r6, r11, r6, asr #19
	add     r3, r12, r3, asr #19
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #19
	add     r2, r11, r2, asr #19							

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
@{
@    A = M(xC1S7, ip[1])@
@    B = M(xC7S1, ip[1])@
@    C = M(xC3S5, ip[3])@
@    D = -M(xC5S3, ip[3])@
@    E = M(xC4S4, ip[0])@
@	{  //HACK
@		E += 8<<15@
@	}	            
@    G = M(xC2S6, ip[2])@
@    H = M(xC6S2, ip[2])@
@    Ad = M(xC4S4, (A - C)>>15)@
@    Bd = M(xC4S4, (B - D)>>15)@
@    Cd = A + C@
@    Dd = B + D@
@    Ed = E - G@
@    Fd = E + G@
@    Gd = E - Ad@
@    Hd = Bd + H@
@    Add = E + Ad@
@    Bdd = Bd - H@ 
@    /* Final sequence of operations over-write original inputs. */
@   {
@        dst[0] = SAT(((src[0] + ((Fd + Cd )  >> 19))))@
@        dst[7] = SAT(((src[7] + ((Fd - Cd )  >> 19))))@
@
@        dst[1] = SAT(((src[1] + ((Add + Hd ) >> 19))))@
@        dst[2] = SAT(((src[2] + ((Add - Hd ) >> 19))))@
@
@        dst[3] = SAT(((src[3] + ((Ed + Dd )  >> 19))))@
@        dst[4] = SAT(((src[4] + ((Ed - Dd )  >> 19))))@
@
@        dst[5] = SAT(((src[5] + ((Gd + Bdd ) >> 19))))@
@        dst[6] = SAT(((src[6] + ((Gd - Bdd ) >> 19))))@
@	src += stride@
@    }	            
@}
@ r2 = x1|x0 r3 = x3|x2
	ldr	r9, WxW1
	ldr	r10, WxW7
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16	
		
	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
		
	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@(-D) = M(xC5S3, ip[3*8])
		
	ldr	r10, WxW4
	sub	r5, r12, r8	@A - C
	add	r6, r2, r4	@B - D@	B + (-D)
	mov	r5, r5, asr #15	@(A - C)>>15
	mov	r6, r6, asr #15	@(B - D)>>15	
	mul	r5, r10, r5	@Ad = M(xC4S4, (A - C)>>15)@
	mul	r6, r10, r6	@Bd = M(xC4S4, (B - D)>>15)@
			
	mul	r11, r1, r10	@E = M(xC4S4, ip[0*8])@

@	{  //HACK
@		E += 8<<15@
@	}
	mov	r9, #8
	mov	r9, r9, asl #15
	add	r11, r11, r9	@E += 8<<15@
	
	ldr	r9, WxW2
	ldr	r10, WxW6
	mul     r9, r3,  r9	@G = M(xC2S6, ip[2*8])@
	mul     r10,  r3,  r10	@H = M(xC6S2, ip[2*8])@
	
	add	r7, r12, r8	@@Cd = A + C@
	sub	r8, r2, r4	@@Dd = B + D@	
	sub	r1, r11, r9	@@Ed = E - G@
	add	r9, r11, r9	@@Fd = E + G@ 
	sub	r12, r11, r5	@@Gd = E - Ad@
	add	r11, r11, r5	@@Add = E + Ad@	
	sub	r5, r6, r10	@@Bdd = Bd - H@
	add	r6, r6, r10	@@Hd = Bd + H@ 

	add	r3, r12, r5	@Gd + Bdd	@3
	sub	r4, r12, r5	@Gd - Bdd	@4
	add	r5, r1, r8	@Ed + Dd	@5
	sub	r12, r1, r8	@Ed - Dd	@6	
	add	r1, r9, r7	@Fd + Cd	@1
	sub	r2, r9, r7	@Fd - Cd	@2
	add	r8, r11, r6	@Add + Hd	@8
	sub	r9, r11, r6	@Add - Hd	@9
	mov	r6, r12
	
@ r3, r6  free now

@r1, r8, r9, r5, r6, r3, r4, r2
@r1, r8, r9, r5, r6, r3, r4, r2	

@	r1, r8, r9, r5, r6, r3, r4, r2
@	r7, r10, r11, r12 free now
		
	ldr	r7, [sp, #4]		@ Src = [sp, #4]	
	ldr	r10, [sp, #52]		@ src_stride = [sp, #52]			
	add	r10, r10, r7
	str	r10, [sp, #4]		@ Src = [sp, #4]
	ldrb    r10, [r7]
	ldrb    r11, [r7, #1]
	ldrb    r12, [r7, #2]
	add     r1, r10, r1, asr #19
	add     r8, r11, r8, asr #19
	add     r9, r12, r9, asr #19
	ldrb    r10, [r7, #3]
	ldrb    r11, [r7, #4]
	ldrb    r12, [r7, #5]
	add     r5, r10, r5, asr #19
	add     r6, r11, r6, asr #19
	add     r3, r12, r3, asr #19
	ldrb    r10, [r7, #6]
	ldrb    r11, [r7, #7]
	add     r4, r10, r4, asr #19
	add     r2, r11, r2, asr #19							

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
Row8nosrc_4x4: @PROC
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
@@	dst[0]=
@@	dst[1]=
@@	dst[2]=
@@	dst[3]=
@@	dst[4]=
@@	dst[5]=
@@	dst[6]=
@@	dst[7]= 128@					
@@}	
	ldr	r2, MaskCarry
	mov	r3, r2
	str	r2, [r7]
	str	r2, [r7, #4]		
	mov	pc, lr
	
only_x1t_lab_Rownosrc_4x4:
@if(ip[0])	//ip0		@ r2 = x1|x0
@{
@	E = M(xC4S4, ip[0])@
@		{  //HACK
@@			E += (16*128 + 8)<<15@
@		}
@    /* Final sequence of operations over-write original inputs. */
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
@@        dst[6] = SAT(((E ) >> 19))@
@}
	ldr	r12, WxW4	
	ldr		r8, Wx04040000	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
    	mul	r1, r2, r12
	add	r1, r1, r8
	mov	r2, r1, asr #19
	
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
@		A = M(xC1S7, ip[1])@
@		B = M(xC7S1, ip[1])@
@		E = M(xC4S4, ip[0])@		
@		{  //HACK
@@			E += (16*128 + 8)<<15@
@		}						
@		Ad = M(xC4S4, A>>15)@
@		Bd = M(xC4S4, B>>15)@
@		Gd = E - Ad@
@		Add = E + Ad@
@	
@            /* Final sequence of operations over-write original inputs. */
@@            if(!src){
@@                dst[0] = SAT(((E + A )  >> 19))@
@@                dst[7] = SAT(((E - A )  >> 19))@
@@
@@                dst[1] = SAT(((Add + Bd ) >> 19))@
@@                dst[2] = SAT(((Add - Bd ) >> 19))@
@@
@@                dst[3] = SAT(((E + B )  >> 19))@
@@                dst[4] = SAT(((E - B )  >> 19))@
@@
@@                dst[5] = SAT(((Gd + Bd ) >> 19))@
@@                dst[6] = SAT(((Gd - Bd ) >> 19))@
@@            }		 				
@}
@ r2 = x1|x0
	@mov	r8, #0x0808
	ldr		r8, Wx04040000
	mov	r1, r2, asr #16	@ r1 = x1	
	mov	r2, r2, lsl #16
	mov	r2, r2, asr #16	@ r2 = x0
	
	ldr	r10, WxW1
	ldr	r11, WxW7
	ldr	r12, WxW4
		
	mul	r5, r1, r10	@A = M(xC1S7, ip[1*8])@
	mul	r6, r1, r11	@B = M(xC7S1, ip[1*8])@	
	mul	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8
		
	mul	r3, r12, r3	@Ad = M(xC4S4, A>>15)@
	mul	r4, r12, r4	@Bd = M(xC4S4, B>>15)@
	sub	r8, r7, r3	@Gd = E - Ad@
	add	r9, r7, r3	@Add = E + Ad@
	
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A
	add	r11, r9, r4	@Add + Bd
	sub	r9, r9, r4	@Add - Bd	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B	
	add	r3, r8, r4	@Gd + Bd
	sub	r4, r8, r4	@Gd - Bd
	mov	r8, r11

@1, 3(8), 9, 5, 6, 7(3), 8(4), 2						

	mov     r1, r1, asr #19
	mov     r8, r8, asr #19
	mov     r9, r9, asr #19
	mov     r5, r5, asr #19
	mov     r6, r6, asr #19
	mov     r3, r3, asr #19
	mov     r4, r4, asr #19
	mov     r2, r2, asr #19
	
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
@{
@    A = M(xC1S7, ip[1])@
@    B = M(xC7S1, ip[1])@
@    C = M(xC3S5, ip[3])@
@    D = -M(xC5S3, ip[3])@
@    E = M(xC4S4, ip[0])@
@	if(!src)
@	{  //HACK
@		E += (16*128 + 8)<<15@
@	}
@	else
@	{  //HACK
@		E += 8<<15@
@	}	            
@    G = M(xC2S6, ip[2])@
@    H = M(xC6S2, ip[2])@
@    Ad = M(xC4S4, (A - C)>>15)@
@    Bd = M(xC4S4, (B - D)>>15)@
@    Cd = A + C@
@    Dd = B + D@
@    Ed = E - G@
@    Fd = E + G@
@    Gd = E - Ad@
@    Hd = Bd + H@
@    Add = E + Ad@
@    Bdd = Bd - H@ 
@    /* Final sequence of operations over-write original inputs. */
@    if(!src){
@        dst[0] = SAT(((Fd + Cd )  >> 19))@
@        dst[7] = SAT(((Fd - Cd )  >> 19))@
@
@        dst[1] = SAT(((Add + Hd ) >> 19))@
@        dst[2] = SAT(((Add - Hd ) >> 19))@
@
@        dst[3] = SAT(((Ed + Dd )  >> 19))@
@        dst[4] = SAT(((Ed - Dd )  >> 19))@
@
@        dst[5] = SAT(((Gd + Bdd ) >> 19))@
@        dst[6] = SAT(((Gd - Bdd ) >> 19))@
@    }else{
@        dst[0] = SAT(((src[0] + ((Fd + Cd )  >> 19))))@
@        dst[7] = SAT(((src[7] + ((Fd - Cd )  >> 19))))@
@
@        dst[1] = SAT(((src[1] + ((Add + Hd ) >> 19))))@
@        dst[2] = SAT(((src[2] + ((Add - Hd ) >> 19))))@
@
@        dst[3] = SAT(((src[3] + ((Ed + Dd )  >> 19))))@
@        dst[4] = SAT(((src[4] + ((Ed - Dd )  >> 19))))@
@
@        dst[5] = SAT(((src[5] + ((Gd + Bdd ) >> 19))))@
@        dst[6] = SAT(((src[6] + ((Gd - Bdd ) >> 19))))@
@	src += stride@
@    }	            
@}
@ r2 = x1|x0 r3 = x3|x2
	ldr	r9, WxW1
	ldr	r10, WxW7
	mov	r1, r2, lsl #16
	mov	r1, r1, asr #16
	mov	r2, r2, asr #16
	
	mov	r4, r3, asr #16	
	mov	r3, r3, lsl #16
	mov	r3, r3, asr #16
		
	mul     r12, r9,  r2		@A = M(xC1S7, ip[1*8])
	mul     r2,  r10, r2		@B = M(xC7S1, ip[1*8])	
		
	ldr	r9, WxW3
	ldr	r10, WxW5
	mul     r8, r9,  r4		@C = M(xC3S5, ip[3*8])
	mul     r4,  r10,  r4		@(-D) = M(xC5S3, ip[3*8])
		
	ldr	r10, WxW4
	sub	r5, r12, r8	@A - C
	add	r6, r2, r4	@B - D@	B + (-D)
	mov	r5, r5, asr #15	@(A - C)>>15
	mov	r6, r6, asr #15	@(B - D)>>15	
	mul	r5, r10, r5	@Ad = M(xC4S4, (A - C)>>15)@
	mul	r6, r10, r6	@Bd = M(xC4S4, (B - D)>>15)@
			
	mul	r11, r1, r10	@E = M(xC4S4, ip[0*8])@

@	{  //HACK
@		E += (16*128 + 8)<<15@			
@	}
	ldr		r9, Wx04040000
	add	r11, r11, r9	@E += (16*128 + 8)<<15@
	
	ldr	r9, WxW2
	ldr	r10, WxW6
	mul     r9, r3,  r9	@G = M(xC2S6, ip[2*8])@
	mul     r10,  r3,  r10	@H = M(xC6S2, ip[2*8])@
	
	add	r7, r12, r8	@@Cd = A + C@
	sub	r8, r2, r4	@@Dd = B + D@	
	sub	r1, r11, r9	@@Ed = E - G@
	add	r9, r11, r9	@@Fd = E + G@ 
	sub	r12, r11, r5	@@Gd = E - Ad@
	add	r11, r11, r5	@@Add = E + Ad@	
	sub	r5, r6, r10	@@Bdd = Bd - H@
	add	r6, r6, r10	@@Hd = Bd + H@ 

	add	r3, r12, r5	@Gd + Bdd	@3
	sub	r4, r12, r5	@Gd - Bdd	@4
	add	r5, r1, r8	@Ed + Dd	@5
	sub	r12, r1, r8	@Ed - Dd	@6	
	add	r1, r9, r7	@Fd + Cd	@1
	sub	r2, r9, r7	@Fd - Cd	@2
	add	r8, r11, r6	@Add + Hd	@8
	sub	r9, r11, r6	@Add - Hd	@9
	mov	r6, r12
	
@ r3, r6  free now

@r1, r8, r9, r5, r6, r3, r4, r2
							
	mov     r1, r1, asr #19
	mov     r8, r8, asr #19
	mov     r9, r9, asr #19
	mov     r5, r5, asr #19
	mov     r6, r6, asr #19
	mov     r3, r3, asr #19
	mov     r4, r4, asr #19
	mov     r2, r2, asr #19	
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
_VP6DEC_VO_Armv4IdctA: @PROC
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
_VP6DEC_VO_Armv4IdctB: @PROC
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
		
_VP6DEC_VO_Armv4IdctC: @PROC	
        STMFD    sp!,{r4-r11,lr}
		@ r0 = v, r1 = Dst, r2 = DstPitch, r3 = Src
	ldrsh	r12, [r0]
	mov	r5, #0
	cmp	r3, #0	
	add	r12, r12, #15	
	strh	r5, [r0]	
	mov	r0, r12, asr #5
@	OutD = (INT16)((INT32)(input[0]+15)>>5)@	
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
@	OutD = SAT((128 + OutD))@
@	add	r0, r12, #128
@	usat	r0, #8, r0
	add	r0, r0, #128
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
	 

	@ENDP
	@.END
