@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/

	@AREA    |.text|, CODE, READONLY
#include "../../../../Inc/voVP6DecID.h"
	.section .text
	.global VP6DEC_VO_Armv6IdctA
	.global VP6DEC_VO_Armv6IdctB	
	.global VP6DEC_VO_Armv6IdctC		
 
	.align 4	
VP6DEC_VO_Armv6IdctC: @PROC
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	ldrsh	r12, [r0]
	mov	r5, #0				
	cmp			r3, #0
@	OutD = (INT16)((INT32)(input[0]+15)>>5)@	
	add	r12, r12, #15	
	mov	r12, r12, asr #5		@asr	r12, r12, #5		
	beq			IDCT_CPY_NOSRC	
	ldr	r11,[sp, #36]	@src_stride	
	strh	r5, [r0]

	cmp	r12, #0
	bne	CPYSRCADD_armv6_neq0

		pld		[r3, r11, lsl #1]	
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
		ldr     r7, [r3, #4]			@= 0, 1, 2, 3
		ldr     r6, [r3], r11			@= 0, 1, 2, 3
		ldr     r9, [r3, #4]			@= 0, 1, 2, 3
		ldr     r8, [r3], r11			@= 0, 1, 2, 3				
		strd     r4, [r1], r2
		strd     r6, [r1], r2
		strd     r8, [r1], r2
		
		pld		[r3, r11, lsl #1]		
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
		ldr     r7, [r3, #4]			@= 0, 1, 2, 3
		ldr     r6, [r3], r11			@= 0, 1, 2, 3
		ldr     r9, [r3, #4]			@= 0, 1, 2, 3
		ldr     r8, [r3], r11			@= 0, 1, 2, 3				
		strd     r4, [r1], r2
		strd     r6, [r1], r2
		strd     r8, [r1], r2
		
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
		ldr     r7, [r3, #4]			@= 0, 1, 2, 3
		ldr     r6, [r3], r11			@= 0, 1, 2, 3				
		strd     r4, [r1], r2
		strd     r6, [r1], r2		
        LDMFD    sp!,{r4-r11,pc}	
	
CPYSRCADD_armv6_neq0:	
	@	mov	r12, r12, lsl #16		@lsl	r12, r12, #16
	@	orr     r12, r12, r12, lsr #16
	pkhbt	r12, r12, r12, lsl #16
		pld		[r3, r11]							
	mov	r10, #8
CPYSRCADD_armv6_loop:		
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
		pld		[r3, r11]				
	uxtb16		r8, r5		    	@0, 2
	uxtb16		r9, r5, ror #8		@1, 3		
	uxtb16		r6, r4		    	@0, 2
	uxtb16		r7, r4, ror #8		@1, 3
	        
	sadd16		r6, r12, r6		@0, 2
	sadd16		r7, r12, r7		@1, 3
	sadd16		r8, r12, r8		@0, 2
	sadd16		r9, r12, r9		@1, 3		
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7
	usat16	 r8,#8,r8
	usat16	 r9,#8,r9	
	orr	r6,r6,r7,lsl #8	
	orr	r7,r8,r9,lsl #8	
        	
		ldr     r5, [r3, #4]			@= 0, 1, 2, 3
		ldr     r4, [r3], r11			@= 0, 1, 2, 3
        strd     r6, [r1], r2		
		pld		[r3, r11]				
	uxtb16		r8, r5		    	@0, 2
	uxtb16		r9, r5, ror #8		@1, 3		
	uxtb16		r6, r4		    	@0, 2
	uxtb16		r7, r4, ror #8		@1, 3
	        
	sadd16		r6, r12, r6		@0, 2
	sadd16		r7, r12, r7		@1, 3
	sadd16		r8, r12, r8		@0, 2
	sadd16		r9, r12, r9		@1, 3		
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7
	usat16	 r8,#8,r8
	usat16	 r9,#8,r9	
	orr	r6,r6,r7,lsl #8	
	orr	r7,r8,r9,lsl #8       	
        subs	r10, r10, #2	
        strd     r6, [r1], r2
        bne	CPYSRCADD_armv6_loop 

        LDMFD    sp!,{r4-r11,pc}	
IDCT_CPY_NOSRC:	
@	v = SAT((128 + OutD))@		
	add	r12, r12, #128
	usat	r12, #8, r12
	orr     r10, r12, r12, lsl #8
	orr     r10, r10, r10, lsl #16
	mov	r11, r10
	strh	r5, [r0]	
		
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1], r2 
        strd     r10, [r1]  
              		
        LDMFD    sp!,{r4-r11,pc}		        
	@ENDP
	
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

	pkhbt	r3, r3, r7, lsl #16	@ r3 = ip6|ip2	
	pkhbt	r4, r6, r4, lsl #16	@ r4 = ip3|ip5
	
	pkhbt	r6, r5, r8, lsl #16	@ r6 = ip7|ip4
	
	orr	r7, r3, r4		@ r7 = ip6|ip2 | ip3|ip5
	orrs	r7, r7, r6		@ r7 = ip6|ip2 | ip3|ip5 | ip7|ip4
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
@	ip[0~7*8] = E@    					    																																										    ip[7*8] =
	smulbb	r5, r1, r12
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
		  				
	smulbt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smulbb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r1, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@
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

@@r1(ip0), r2(ip1), r3(ip6|ip2) r4(ip3|ip5) r6(ip7|ip4), (r5, r7, r8 can use)
	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
	pkhbt	r2, r2, r6		@ r2 = ip7|ip1
@ r5, r6, r7, r8 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r4 r5 r6 r9 free now
	
	smuadx	r4, r3, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r6, r3, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
@ r9 r5 r3  free now	

	smuad	r9, r1, r12	@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	smusd	r5, r1, r12	@F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@ r3  r1 free now
	
	sub	r1, r9, r4	@@Ed = E - G@
	add	r3, r9, r4	@@Fd = E + G@	
@ r4 r9 free now

	smuadx	r4, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r9, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@ r2 free now

	sub	r2, r4, r7	@A - C@
	add	r7, r4, r7	@@Cd = A + C@	
	sub	r4, r9, r8	@B - D@
	add	r8, r9, r8	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15
	smulbb	r2, r2, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	
@ r9 free now
	
	sub	r9, r5, r2	@@Gd = F - Ad@
	add	r5, r5, r2	@@Add = F + Ad@	
	sub	r2, r4, r6	@@Bdd = Bd - H@	
	add	r4, r4, r6	@@Hd = Bd + H@
@ r6 free now
	add	r6, r3, r7	@Fd + Cd
	sub	r7, r3, r7	@Fd - Cd
	add	r3, r5, r4	@Add + Hd
	sub	r5, r5, r4	@Add - Hd	
	add	r4, r1, r8	@Ed + Dd
	sub	r1, r1, r8	@Ed - Dd	
	add	r8, r9, r2	@Gd + Bdd
	sub	r9, r9, r2	@Gd - Bdd
		
	mov	r6, r6, asr #15	@
	mov	r7, r7, asr #15	@
	mov	r3, r3, asr #15	@
	mov	r5, r5, asr #15	@
	mov	r4, r4, asr #15	@
	mov	r1, r1, asr #15	@
	mov	r8, r8, asr #15	@
	mov	r9, r9, asr #15	@

	strh	r6, [r0]
	strh	r3, [r0, #16]
	strh	r5, [r0, #32]
	strh	r4, [r0, #48]
	strh	r1, [r0, #64]
	strh	r8, [r0, #80]
	strh	r9, [r0, #96]
	strh	r7, [r0, #112]	
	mov		pc,lr
	@ENDP

	.align 4
Row8src: @PROC
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	
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
	pld		[r8]		
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
	strd		r2, [r7]		
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
	mov	r8, #8
	smulbb	r1, r2, r12
@	ldrd	r2, [r6]	@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7	
	add	r1, r1, r8, asl #15
	movs	r1, r1, asr #19
	
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
		
	streq	r2, [r7]
	streq	r3, [r7, #4]	@strdeq	r2, [r7]		
	moveq	pc, lr
						
	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r8, r3, ror #8		@1, 3	b	
	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r1		@1, 3	a+e
	sadd16		r6, r6, r1		@0, 2	b+e
	sadd16		r8, r8, r1		@1, 3	b+e		
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r8,#8,r8		
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r8,lsl #8
	strd		r2, [r7]						
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
	mov	r8, #8
	smultt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8, asl #15
		
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@
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

@1, 3, 9, 5, 6, 7, 8, 2
	
	mov	r1, r1, asr #19	@
	mov	r3, r3, asr #19	@
	mov	r9, r9, asr #19	@
	mov	r5, r5, asr #19	@
	mov	r6, r6, asr #19	@
	mov	r7, r7, asr #19	@
	mov	r8, r8, asr #19	@
	mov	r2, r2, asr #19	@	
	
	ldr	r4, [sp, #4]		@ Src = [sp, #4]
	pkhbt	r7, r7, r2, lsl #16	@1, 3	b				
	ldr	r2, [sp, #52]		@ src_stride = [sp, #52]			
	pkhbt	r1, r1, r9, lsl #16	@0, 2	a
	pkhbt	r9, r3, r5, lsl #16	@1, 3	a
	pkhbt	r8, r6, r8, lsl #16	@0, 2	b
	add	r2, r2, r4
	pld		[r2]	
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r4]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r4, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
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
@	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
@	pkhbt	r2, r2, r6		@ r2 = ip7|ip1

	pkhbt	r1, r2, r4, lsl #16	@ r1 = ip4|ip0
	pkhtb	r2, r5, r2, asr #16	@ r2 = ip7|ip1
	pkhbt	r6, r3, r5, lsl #16	@ r6 = ip6|ip2
	pkhtb	r4, r3, r4, asr #16	@ r4 = ip3|ip5		
	
	
@ r5, r6, r7, r8 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r4 r5 r6 r9 free now
	
	smuadx	r4, r6, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r6, r6, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
@ r9 r5 r3  free now	

	smuad	r9, r1, r12	@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	smusd	r5, r1, r12	@F = M(xC4S4, (ip[0*8] - ip[4*8]))@
	
@	{  //HACK
@		E += 8<<15@
@		F += 8<<15@				
@	}
	mov	r3, #8
	mov	r3, r3, asl #15
	add	r9, r9, r3	@E += 8<<15@
	add	r5, r5, r3	@F += 8<<15@		
@ r3  r1 free now
	
	sub	r1, r9, r4	@@Ed = E - G@
	add	r3, r9, r4	@@Fd = E + G@	
@ r4 r9 free now

	smuadx	r4, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r9, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@ r2 free now

	sub	r2, r4, r7	@A - C@
	add	r7, r4, r7	@@Cd = A + C@	
	sub	r4, r9, r8	@B - D@
	add	r8, r9, r8	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15
	smulbb	r2, r2, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	
@ r9 free now
	
	sub	r9, r5, r2	@@Gd = F - Ad@
	add	r5, r5, r2	@@Add = F + Ad@	
	sub	r2, r4, r6	@@Bdd = Bd - H@	
	add	r4, r4, r6	@@Hd = Bd + H@
@ r6 free now
	add	r6, r3, r7	@Fd + Cd
	sub	r7, r3, r7	@Fd - Cd
	add	r3, r5, r4	@Add + Hd
	sub	r5, r5, r4	@Add - Hd	
	add	r4, r1, r8	@Ed + Dd
	sub	r1, r1, r8	@Ed - Dd	
	add	r8, r9, r2	@Gd + Bdd
	sub	r9, r9, r2	@Gd - Bdd
		
	mov	r6, r6, asr #19	@
	mov	r3, r3, asr #19	@
	mov	r5, r5, asr #19	@
	mov	r4, r4, asr #19	@
	mov	r1, r1, asr #19	@
	mov	r8, r8, asr #19	@
	mov	r9, r9, asr #19	@
	mov	r7, r7, asr #19	@
	
@6, 3, 5, 4, 1, 8, 9, 7	
	pkhbt	r5, r6, r5, lsl #16	@0, 2	a
	pkhbt	r6, r3, r4, lsl #16	@1, 3	a
	ldr	r2, [sp, #4]		@ Src = [sp, #4]			
	ldr	r4, [sp, #52]		@ src_stride = [sp, #52]	
	pkhbt	r7, r8, r7, lsl #16	@1, 3	b	
	pkhbt	r8, r1, r9, lsl #16	@0, 2	b
	add	r4, r4, r2
	pld		[r4]	
	str	r4, [sp, #4]		@ Src = [sp, #4]		

	ldr	r3, [r2, #4]								
	ldr	r2, [r2]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	uxtb16		r1, r2			@0, 2	a
	uxtb16		r9, r2, ror #8		@1, 3	a
	uxtb16		r4, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b		
	sadd16		r5, r5, r1		@0, 2	a+e
	sadd16		r6, r6, r9		@1, 3	a+e
	sadd16		r8, r8, r4		@0, 2	b+e
	sadd16		r7, r7, r3		@1, 3	b+e	
		
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r8,#8,r8	
	usat16	 r7,#8,r7
	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]				
	orr	r2,r5,r6,lsl #8
	orr	r3,r8,r7,lsl #8
	strd	r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]	
	mov	pc, lr
	@ENDP


	.align 4
Row8nosrc: @PROC
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	ldrd		r4, [r0, #8]	@ r4 = x5|x4 r5 = x7|x6
	
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
	ldr	r2, Wx80808080
	mov	r3, r2
	strd		r2, [r7]		
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
	@mov	r8, #0x0808
	ldr		r8, Wx04040000	
	smulbb	r1, r2, r12
	add	r1, r1, r8
@	mov	r2, r1, asr #19
	usat	r2,#8, r1, asr #19	
	orr	r2, r2, r2, lsl #8
	orr	r2, r2, r2, lsl #16			
	mov	r3, r2
	strd		r2, [r7]						
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
	smultt	r5, r2, r11	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8
		
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@
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

@1, 3, 9, 5, 6, 7, 8, 2
	usat	r1, #8, r1, asr #19
	usat	r3, #8, r3, asr #19
	usat	r9, #8, r9, asr #19
	usat	r5, #8, r5, asr #19
	usat	r6, #8, r6, asr #19
	usat	r7, #8, r7, asr #19
	usat	r8, #8, r8, asr #19
	usat	r2, #8, r2, asr #19						

	orr	r4, r1, r3, lsl #8
	orr	r4, r4, r9, lsl #16
	orr	r4, r4, r5, lsl #24
	orr	r5, r6, r7, lsl #8
	ldr	r1, [sp, #12]			@ dst = [sp, #12]	
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r5, r5, r8, lsl #16
	orr	r5, r5, r2, lsl #24
	strd	r4, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
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
@	pkhbt	r1, r1, r6, lsl #16	@ r1 = ip4|ip0
@	pkhbt	r2, r2, r6		@ r2 = ip7|ip1

	pkhbt	r1, r2, r4, lsl #16	@ r1 = ip4|ip0
	pkhtb	r2, r5, r2, asr #16	@ r2 = ip7|ip1
	pkhbt	r6, r3, r5, lsl #16	@ r6 = ip6|ip2
	pkhtb	r4, r3, r4, asr #16	@ r4 = ip3|ip5		
	
	
@ r5, r6, r7, r8 free now
	ldr	r9, WxC3xC5	@r9 = WxC3xC5
	smuad	r7, r4, r9	@C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
	smusdx	r8, r4, r9	@D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@ r4 r5 r6 r9 free now
	
	smuadx	r4, r6, r10	@G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
	smusd	r6, r6, r10	@H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
@ r9 r5 r3  free now	

	smuad	r9, r1, r12	@E = M(xC4S4, (ip[0*8] + ip[4*8]))@
	smusd	r5, r1, r12	@F = M(xC4S4, (ip[0*8] - ip[4*8]))@
	
@	{  //HACK
@		E += (16*128 + 8)<<15@
@		F += (16*128 + 8)<<15@				
@	}
	@mov	r3, #0x0808
	@mov	r3, r3, asl #15

	ldr		r3, Wx04040000
	add	r9, r9, r3	@E += 8<<15@
	add	r5, r5, r3	@F += 8<<15@		
@ r3  r1 free now
	
	sub	r1, r9, r4	@@Ed = E - G@
	add	r3, r9, r4	@@Fd = E + G@	
@ r4 r9 free now

	smuadx	r4, r2, r11	@A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])
	smusd	r9, r2, r11	@B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@ r2 free now

	sub	r2, r4, r7	@A - C@
	add	r7, r4, r7	@@Cd = A + C@	
	sub	r4, r9, r8	@B - D@
	add	r8, r9, r8	@@Dd = B + D@
	mov	r2, r2, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15
	smulbb	r2, r2, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	
@ r9 free now
	
	sub	r9, r5, r2	@@Gd = F - Ad@
	add	r5, r5, r2	@@Add = F + Ad@	
	sub	r2, r4, r6	@@Bdd = Bd - H@	
	add	r4, r4, r6	@@Hd = Bd + H@
@ r6 free now
	add	r6, r3, r7	@Fd + Cd
	sub	r7, r3, r7	@Fd - Cd
	add	r3, r5, r4	@Add + Hd
	sub	r5, r5, r4	@Add - Hd	
	add	r4, r1, r8	@Ed + Dd
	sub	r1, r1, r8	@Ed - Dd	
	add	r8, r9, r2	@Gd + Bdd
	sub	r9, r9, r2	@Gd - Bdd

@6, 3, 5, 4, 1, 8, 9, 7	
	usat	r6, #8, r6, asr #19
	usat	r3, #8, r3, asr #19
	usat	r5, #8, r5, asr #19
	usat	r4, #8, r4, asr #19
	usat	r1, #8, r1, asr #19
	usat	r8, #8, r8, asr #19
	usat	r9, #8, r9, asr #19
	usat	r7, #8, r7, asr #19
							
	orr	r4, r6, r4, lsl #24
	orr	r4, r4, r3, lsl #8
	orr	r4, r4, r5, lsl #16

	orr	r5, r1, r8, lsl #8
	ldr	r1, [sp, #12]			@ dst = [sp, #12]	
	ldr	r2, [sp, #8]		@ dst_stride = [sp, #8]				
	orr	r5, r5, r9, lsl #16
	orr	r5, r5, r7, lsl #24
	strd	r4, [r1]
	add	r1, r1, r2
	str	r1, [sp, #12]		@ dst = [sp, #12]	
	mov	pc, lr	
	
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
	smulbb	r5, r1, r12
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
			
	smulbb	r5, r2, r9	@A = M(xC1S7, ip[1*8])@
	smulbb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r1, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@

	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B
	
	mov	r1, r1, asr #15	@
	mov	r2, r2, asr #15	@	
	mov	r5, r5, asr #15	@
	mov	r6, r6, asr #15	@
	strh	r1, [r0]
	strh	r2, [r0, #112]	
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
	
	sub	r1, r7, r3	@Gd = E - Ad@
	add	r2, r7, r3	@Add = E + Ad@	
	add	r3, r2, r4	@Add + Bd
	sub	r2, r2, r4	@Add - Bd		
	add	r7, r1, r4	@Gd + Bd
	sub	r1, r1, r4	@Gd - Bd
	mov	r3, r3, asr #15	@
	mov	r2, r2, asr #15	@	
	mov	r7, r7, asr #15	@
	mov	r1, r1, asr #15	@
	strh	r3, [r0, #16]
	strh	r2, [r0, #32]
	strh	r7, [r0, #80]
	strh	r1, [r0, #96]
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
	pkhbt	r2, r2, r4, lsl #16	@ r2 = ip3|ip1
	pkhbt	r1, r1, r3, lsl #16	@ r1 = ip2|ip0		
	smuad	r3, r2, r9	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	smuad	r5, r1, r12	@@Fd = E + G  M(xC4S4, ip[0*8]) + M(xC2S6, ip[2*8])	
	smusd	r6, r1, r12	@@Ed = E - G  M(xC4S4, ip[0*8]) - M(xC2S6, ip[2*8])
	
	add	r7, r5, r3	@Fd + Cd
	sub	r3, r5, r3	@Fd - Cd
	add	r5, r6, r4	@Ed + Dd
	sub	r6, r6, r4	@Ed - Dd
	
	mov	r7, r7, asr #15	@
	mov	r3, r3, asr #15	@
	mov	r5, r5, asr #15	@
	mov	r6, r6, asr #15	@
	strh	r7, [r0]
	strh	r3, [r0, #112]	
	strh	r5, [r0, #48]
	strh	r6, [r0, #64]
		
	smusd	r3, r2, r9	@      A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])			
	smuad	r4, r2, r11	@      B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))
	smulbb	r5, r1, r12	@E = M(xC4S4, ip[0*8])@	
	smultb	r6, r1, r10	@H = M(xC6S2, ip[2*8])@		
	mov	r3, r3, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15	
	smulbb	r3, r3, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	

	sub	r7, r5, r3	@@Gd = E - Ad@
	add	r3, r5, r3	@@Add = E + Ad@	
	add	r5, r4, r6	@@Hd = Bd + H@
	sub	r6, r4, r6	@@Bdd = Bd - H@
			
	add	r1, r3, r5	@Add + Hd
	sub	r2, r3, r5	@Add - Hd		
	add	r3, r7, r6	@Gd + Bdd
	sub	r4, r7, r6	@Gd - Bdd
	
	mov	r1, r1, asr #15	@
	mov	r2, r2, asr #15	@
	mov	r3, r3, asr #15	@
	mov	r4, r4, asr #15	@
	strh	r1, [r0, #16]
	strh	r2, [r0, #32]
	strh	r3, [r0, #80]
	strh	r4, [r0, #96]	
	mov	pc, lr
	@ENDP

	.align 4
Row8src_4x4: @PROC
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
	cmp	r3, #0
	bne	allx1_4_t_lab_Rowsrc	@ (ip2|ip3|ip4|ip5| ip6|ip7)!=0	
	
 	movs	r3, r2, lsr #16		@ r3 = x1	

	bne	only_x1x2t_lab_Rowsrc_4x4	@ x2!=0
	ldr	r6, [sp, #4]		@ Src = [sp, #4]
	ldr	r8, [sp, #68]		@ src_stride = [sp, #68]
	ldr	r7, [sp, #12]		@ dst = [sp, #12]
	ldr	r4, [sp, #8]		@ dst_stride = [sp, #8]
	add	r8, r6, r8
	add	r4, r7, r4
	pld		[r8]		
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
	strd		r2, [r7]		
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
	mov	r8, #8
	smulbb	r1, r2, r12	
	add	r1, r1, r8, asl #15
	movs	r1, r1, asr #19
	
@	ldrd	r2, [r6]	@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
	
	streq	r2, [r7]
	streq	r3, [r7, #4]	@strdeq	r2, [r7]	
	moveq	pc, lr
						
	pkhbt	r1, r1, r1, lsl #16	@ r1 = E|E
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r8, r3, ror #8		@1, 3	b	
	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r1		@1, 3	a+e
	sadd16		r6, r6, r1		@0, 2	b+e
	sadd16		r8, r8, r1		@1, 3	b+e		
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r8,#8,r8		
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r8,lsl #8
	strd		r2, [r7]						
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
	mov	r8, #8
	smultb	r5, r2, r9	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15	
	add	r7, r7, r8, asl #15
		
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@
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

@1, 3, 9, 5, 6, 7, 8, 2
	
	mov	r1, r1, asr #19	@
	mov	r3, r3, asr #19	@
	mov	r9, r9, asr #19	@
	mov	r5, r5, asr #19	@
	mov	r6, r6, asr #19	@
	mov	r7, r7, asr #19	@
	mov	r8, r8, asr #19	@
	mov	r2, r2, asr #19	@	
	
	ldr	r4, [sp, #4]		@ Src = [sp, #4]
	pkhbt	r7, r7, r2, lsl #16	@1, 3	b				
	ldr	r2, [sp, #68]		@ src_stride = [sp, #68]			
	pkhbt	r1, r1, r9, lsl #16	@0, 2	a
	pkhbt	r9, r3, r5, lsl #16	@1, 3	a
	pkhbt	r8, r6, r8, lsl #16	@0, 2	b
	add	r2, r2, r4
	pld		[r2]	
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r4]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r4, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, WxC3xC1	@r9 = WxC3xC1	
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
	mov	r8, #8
	pkhtb	r4, r3, r2, asr #16	@ r2 = ip3|ip1
	pkhbt	r1, r2, r3, lsl #16	@ r1 = ip2|ip0
	mov	r2, r4		
	smuad	r3, r2, r9	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	smuad	r5, r1, r12	@@Fd = E + G  M(xC4S4, ip[0*8]) + M(xC2S6, ip[2*8])	
	smusd	r6, r1, r12	@@Ed = E - G  M(xC4S4, ip[0*8]) - M(xC2S6, ip[2*8])
	add	r5, r5, r8, asl #15
	add	r6, r6, r8, asl #15	
		
	add	r7, r5, r3	@Fd + Cd
	sub	r3, r5, r3	@Fd - Cd
	add	r5, r6, r4	@Ed + Dd
	sub	r6, r6, r4	@Ed - Dd
	
	mov	r7, r7, asr #19	@
	mov	r3, r3, asr #19	@
	mov	r5, r5, asr #19	@
	mov	r6, r6, asr #19	@
	
	pkhbt	r3, r7, r3, lsl #16	@ r3 = ip7|ip0
	pkhbt	r5, r5, r6, lsl #16	@ r5 = ip4|ip3		
	str	r3, [sp, #20]		@ r3 = ip7|ip0
	str	r5, [sp, #24]		@ r5 = ip4|ip3
					
	mov	r7, #8		
	smusd	r3, r2, r9	@      A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])			
	smuad	r4, r2, r11	@      B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))
	smulbb	r5, r1, r12	@E = M(xC4S4, ip[0*8])@	
	smultb	r6, r1, r10	@H = M(xC6S2, ip[2*8])@		
	mov	r3, r3, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15
	add	r5, r5, r7, asl #15
			
	smulbb	r3, r3, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	

	sub	r7, r5, r3	@@Gd = E - Ad@
	add	r3, r5, r3	@@Add = E + Ad@	
	add	r5, r4, r6	@@Hd = Bd + H@
	sub	r6, r4, r6	@@Bdd = Bd - H@
			
	add	r1, r3, r5	@Add + Hd
	sub	r2, r3, r5	@Add - Hd		
	add	r3, r7, r6	@Gd + Bdd
	sub	r4, r7, r6	@Gd - Bdd
							
	mov	r1, r1, asr #19	@ip1
	mov	r2, r2, asr #19	@ip2
	mov	r3, r3, asr #19	@ip5
	mov	r4, r4, asr #19	@ip6	

@	strb	r1, [r8, #1]	
@	strb	r2, [r8, #2]
@	strb	r3, [r8, #5]
@	strb	r4, [r8, #6]	
@	str	r3, [sp, #20]		@ r3 = ip7|ip0
@	str	r5, [sp, #24]		@ r5 = ip4|ip3

	ldr	r6, [sp, #20]		@ r6 = ip7|ip0
	ldr	r5, [sp, #24]		@ r5 = ip4|ip3
	mov	r7, r4, lsl #16
	ldr	r4, [sp, #4]		@ Src = [sp, #4]	
	pkhbt	r9, r1, r5, lsl #16	@1, 3	a	ip3|ip1		
	pkhbt	r1, r6, r2, lsl #16	@0, 2	a	ip2|ip0
	pkhtb	r8, r7, r5, asr #16	@0, 2	b	ip6|ip4	
	ldr	r2, [sp, #68]		@ src_stride = [sp, #68]			
	pkhbt	r7, r3, r6		@1, 3	b	ip7|ip5	
	add	r2, r2, r4
	pld		[r2]	
	str	r2, [sp, #4]		@ Src = [sp, #4]
	
	ldr	r2, [r4]		@r2 = 0, 1, 2, 3 @r3 = 4, 5, 6, 7
	ldr	r3, [r4, #4]								
	uxtb16		r4, r2			@0, 2	a
	uxtb16		r5, r2, ror #8		@1, 3	a
	uxtb16		r6, r3			@0, 2	b
	uxtb16		r3, r3, ror #8		@1, 3	b	
	sadd16		r4, r4, r1		@0, 2	a+e
	sadd16		r5, r5, r9		@1, 3	a+e
	sadd16		r6, r6, r8		@0, 2	b+e
	sadd16		r7, r3, r7		@1, 3	b+e
			
	usat16	 r4,#8,r4
	usat16	 r5,#8,r5
	usat16	 r6,#8,r6
	usat16	 r7,#8,r7	
	ldr	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, [sp, #8]		@ dst_stride = [sp, #8]			
	orr	r2,r4,r5,lsl #8
	orr	r3,r6,r7,lsl #8
	strd		r2, [r1]
	add	r1, r1, r9
	str	r1, [sp, #12]		@ dst = [sp, #12]
	ldr	r9, WxC3xC1	@r9 = WxC3xC1	
	mov	pc, lr
	@ENDP

	.align 4
Row8nosrc_4x4: @PROC
	ldrd		r2, [r0]	@ r2 = x1|x0 r3 = x3|x2
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
	ldr	r2, Wx80808080
	mov	r3, r2
	strd	r2, [r7]		
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
	@mov	r8, #0x0808
	ldr		r8, Wx04040000
	smulbb	r1, r2, r12
	add	r1, r1, r8
@	mov	r2, r1, asr #19
	usat	r2, #8, r1, asr #19	
	orr	r2, r2, r2, lsl #8
	orr	r2, r2, r2, lsl #16			
	mov	r3, r2
	strd		r2, [r7]						
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
	smultb	r5, r2, r9	@A = M(xC1S7, ip[1*8])@
	smultb	r6, r2, r11	@B = M(xC7S1, ip[1*8])@	
	smulbb	r7, r2, r12	@E = M(xC4S4, ip[0*8])@
	mov	r3, r5, asr #15	@A>>15
	mov	r4, r6, asr #15	@B>>15
	add	r7, r7, r8
	smulbb	r3, r3, r12	@Ad = M(xC4S4, A>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, B>>15)@
	
	ldr	r8, [sp, #12]	@ dst = [sp, #12]
	add	r1, r7, r5	@E + A
	sub	r2, r7, r5	@E - A	
	add	r5, r7, r6	@E + B
	sub	r6, r7, r6	@E - B
	
	usat	r1, #8, r1, asr #19
	usat	r2, #8, r2, asr #19
	usat	r5, #8, r5, asr #19
	usat	r6, #8, r6, asr #19
	
	strb	r1, [r8]
	ldr	r1, [sp, #8]	@ dst_stride = [sp, #8]		
	strb	r5, [r8, #3]
	strb	r6, [r8, #4]
	strb	r2, [r8, #7]			
	add	r1, r8, r1
	str	r1, [sp, #12]		@ dst = [sp, #12]
	
	sub	r1, r7, r3	@Gd = E - Ad@
	add	r2, r7, r3	@Add = E + Ad@	
	add	r3, r2, r4	@Add + Bd
	sub	r2, r2, r4	@Add - Bd		
	add	r7, r1, r4	@Gd + Bd
	sub	r1, r1, r4	@Gd - Bd
	
	usat	r3, #8, r3, asr #19
	usat	r2, #8, r2, asr #19
	usat	r7, #8, r7, asr #19
	usat	r1, #8, r1, asr #19	

	strb	r3, [r8, #1]	
	strb	r2, [r8, #2]
	strb	r7, [r8, #5]
	strb	r1, [r8, #6]
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
	@mov	r8, #0x0808
	ldr		r8, Wx04040000
	pkhtb	r4, r3, r2, asr #16	@ r2 = ip3|ip1
	pkhbt	r1, r2, r3, lsl #16	@ r1 = ip2|ip0
	mov	r2, r4		
	smuad	r3, r2, r9	@@Cd = A + C  M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8])
	smusd	r4, r2, r11	@@Dd = B + D  M(xC7S1, ip[1*8]) + (-M(xC5S3, ip[3*8]))
	smuad	r5, r1, r12	@@Fd = E + G  M(xC4S4, ip[0*8]) + M(xC2S6, ip[2*8])	
	smusd	r6, r1, r12	@@Ed = E - G  M(xC4S4, ip[0*8]) - M(xC2S6, ip[2*8])
	add	r5, r5, r8
	add	r6, r6, r8	
		
	add	r7, r5, r3	@Fd + Cd
	sub	r3, r5, r3	@Fd - Cd
	add	r5, r6, r4	@Ed + Dd
	sub	r6, r6, r4	@Ed - Dd
	
	ldr	r8, [sp, #12]	@ dst = [sp, #12]	
	usat	r7, #8, r7, asr #19
	usat	r3, #8, r3, asr #19
	usat	r5, #8, r5, asr #19
	usat	r6, #8, r6, asr #19
	
	strb	r7, [r8]
	ldr	r7, [sp, #8]	@ dst_stride = [sp, #8]		
	strb	r5, [r8, #3]
	strb	r6, [r8, #4]
	strb	r3, [r8, #7]			
	add	r7, r8, r7
	str	r7, [sp, #12]	@ dst = [sp, #12]
		
	@mov	r7, #0x0808
	ldr		r7, Wx04040000	
	smusd	r3, r2, r9	@      A - C  M(xC1S7, ip[1*8]) - M(xC3S5, ip[3*8])			
	smuad	r4, r2, r11	@      B - D  M(xC7S1, ip[1*8]) - (-M(xC5S3, ip[3*8]))
	smulbb	r5, r1, r12	@E = M(xC4S4, ip[0*8])@	
	smultb	r6, r1, r10	@H = M(xC6S2, ip[2*8])@		
	mov	r3, r3, asr #15	@(A - C)>>15
	mov	r4, r4, asr #15	@(B - D)>>15
	add	r5, r5, r7
			
	smulbb	r3, r3, r12	@Ad = M(xC4S4, (A - C)>>15)@
	smulbb	r4, r4, r12	@Bd = M(xC4S4, (B - D)>>15)@	

	sub	r7, r5, r3	@@Gd = E - Ad@
	add	r3, r5, r3	@@Add = E + Ad@	
	add	r5, r4, r6	@@Hd = Bd + H@
	sub	r6, r4, r6	@@Bdd = Bd - H@
			
	add	r1, r3, r5	@Add + Hd
	sub	r2, r3, r5	@Add - Hd		
	add	r3, r7, r6	@Gd + Bdd
	sub	r4, r7, r6	@Gd - Bdd
							
	usat	r1, #8, r1, asr #19
	usat	r2, #8, r2, asr #19
	usat	r3, #8, r3, asr #19
	usat	r4, #8, r4, asr #19	

	strb	r1, [r8, #1]	
	strb	r2, [r8, #2]
	strb	r3, [r8, #5]
	strb	r4, [r8, #6]
	mov	pc, lr
		
	.align 4
VP6DEC_VO_Armv6IdctA: @PROC
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #32	
@	ldr	r9, [sp, #68]		@ src_stride = [sp, #68]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]

	ldr	r9, WxC3xC1	@r9 = WxC3xC1
	ldr	r10, WxC6xC6	@r10 = WxC6xC6
	ldr	r11, WxC5xC7	@r11 = WxC5xC7
	ldr	r12, WxC2xC4	@r12 = WxC2xC4
	
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
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]                                           		
	add	sp, sp, #32
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
	mov	r11, #0	
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16                          
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0], #16
        strd     r10, [r0]

                                         		
	add	sp, sp, #32
        LDMFD    sp!,{r4-r11,pc}
        	
	@ENDP

	.align 4
VP6DEC_VO_Armv6IdctB: @PROC
        STMFD    sp!,{r4-r11,lr}	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	sub	sp, sp, #16	
@	ldr	r9, [sp, #52]		@ src_stride = [sp, #52]
	str	r3, [sp, #4]		@ Src = [sp, #4]
	str	r2, [sp, #8]		@ dst_stride = [sp, #8]
	str	r1, [sp, #12]		@ dst = [sp, #12]	

	ldr	r10, WxC2xC6	@r10 = WxC2xC6
	ldr	r11, WxC1xC7	@r11 = WxC1xC7
	ldr	r12, WxC4xC4	@r12 = WxC4xC4
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
	mov	r11, #0	
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0]                                              		
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
	mov	r11, #0	
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8                          
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0], #8
        strd     r10, [r0]                                             		
	add	sp, sp, #16
        LDMFD    sp!,{r4-r11,pc}

	.align 8	
WxC1xC7:		.word 0x7D8A18F8			@xC7 = D0.S16[0] xC1 = D0.S16[1]		
WxC3xC5:		.word 0x6A6D471D			@xC5 = D0.S16[2] xC3 = D0.S16[3]			
WxC2xC6:		.word 0x764130FC			@xC6 = D1.S16[0] xC2 = D1.S16[1]		
WxC4xC4:		.word 0x5A825A82			@xC4 = D1.S32[1]/D1.S32[2]
Wx80808080:	.word 0x80808080			@
Wx04040000:	.word 0x04040000 


WxC5xC7:		.word 0x471D18F8			@xC7 = D0.S16[0] xC5 = D0.S16[1]		
WxC3xC1:		.word 0x6A6D7D8A			@xC1 = D0.S16[2] xC3 = D0.S16[3]			
WxC2xC4:		.word 0x76415A82			@xC4 = D1.S16[0] xC2 = D1.S16[1]		
WxC6xC6:		.word 0x30FC30FC			@xC6 = D1.S32[1]/D1.S32[2]

	@ENDP
	.END