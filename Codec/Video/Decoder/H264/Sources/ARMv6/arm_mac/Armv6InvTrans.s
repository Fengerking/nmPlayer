	  .text
	  .align 2
    .globl  _itrans_asm
_itrans_asm:
       
        stmfd   r13!, {r4-r11, r14} @
        sub     r13, r13, #64
@//0

@		a = (coef[0] + 32) + coef[2]@						
@		b = (coef[0] + 32) - coef[2]@
@		c = (coef[1]>>1) - coef[3]@
@		d = coef[1] + (coef[3]>>1)@
@		m7[ 0] = a + d@
@		m7[ 4] = b + c@
@		m7[8] = b - c@
@		m7[12] = a - d@
@		coef += 4@	
		@pld	   [r1, #8]
		ldrsh	r4, [r1], #2		@coef[0]
		ldrsh	r5, [r1], #2		@coef[1]
		ldrsh	r6, [r1], #2		@coef[2]
		ldrsh	r7, [r1], #2		@coef[3]
		add		r4, r4, #32			@coef[0] + 32
		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d						

		add		r4, r8, r11		@m7[ 0]		
		add		r5, r9, r10		@m7[ 4]
		sub		r6, r9, r10		@m7[ 8]
		sub		r7, r8, r11		@m7[12]		
		str		r4, [sp, #0]
		str		r5, [sp, #16]
		str		r6, [sp, #32]
		str		r7, [sp, #48]						
@//1
@		a = coef[0] + coef[2]@
@		b = coef[0] - coef[2]@
@		c = (coef[1]>>1) - coef[3]@
@		d = coef[1] + (coef[3]>>1)@		
@		m7[ 1] = a + d@
@		m7[ 5] = b + c@
@		m7[ 9] = b - c@
@		m7[13] = a - d@
@		coef += 4@
		@pld	   [r1, #8]
		ldrsh	r4, [r1], #2		@coef[0]
		ldrsh	r5, [r1], #2		@coef[1]
		ldrsh	r6, [r1], #2		@coef[2]
		ldrsh	r7, [r1], #2		@coef[3]

		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d						

		add		r4, r8, r11		@m7[ 0]		
		add		r5, r9, r10		@m7[ 4]
		sub		r6, r9, r10		@m7[ 8]
		sub		r7, r8, r11		@m7[12]		
		
		str		r4, [sp, #4]
		str		r5, [sp, #20]
		str		r6, [sp, #36]
		str		r7, [sp, #52]
@//2
@		a = coef[0] + coef[2]@
@		b = coef[0] - coef[2]@
@		c = (coef[1]>>1) - coef[3]@
@		d = coef[1] + (coef[3]>>1)@		
@		m7[ 2] = a + d@
@		m7[ 6] = b + c@
@		m7[10] = b - c@
@		m7[14] = a - d@
@		coef += 4@
		@pld	   [r1, #8]
		ldrsh	r4, [r1], #2		@coef[0]
		ldrsh	r5, [r1], #2		@coef[1]
		ldrsh	r6, [r1], #2		@coef[2]
		ldrsh	r7, [r1], #2		@coef[3]

		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d						

		add		r4, r8, r11		@m7[ 0]		
		add		r5, r9, r10		@m7[ 4]
		sub		r6, r9, r10		@m7[ 8]
		sub		r7, r8, r11		@m7[12]		
		
		str		r4, [sp, #8]
		str		r5, [sp, #24]
		str		r6, [sp, #40]
		str		r7, [sp, #56]
@//3
@		a = coef[0] + coef[2]@
@		b = coef[0] - coef[2]@
@		c = (coef[1]>>1) - coef[3]@
@		d = coef[1] + (coef[3]>>1)@		
@		m7[ 3] = a + d@
@		m7[ 7] = b + c@
@		m7[11] = b - c@
@		m7[15] = a - d@
@		coef += 4@

		ldrsh	r4, [r1], #2		@coef[0]
		ldrsh	r5, [r1], #2		@coef[1]
		ldrsh	r6, [r1], #2		@coef[2]
		ldrsh	r7, [r1], #2		@coef[3]

		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d						

		add		r4, r8, r11		@m7[ 0]		
		add		r5, r9, r10		@m7[ 4]
		sub		r6, r9, r10		@m7[ 8]
		sub		r7, r8, r11		@m7[12]		
		
		str		r4, [sp, #12]
		str		r5, [sp, #28]
		str		r6, [sp, #44]
		str		r7, [sp, #60]
@	j = 4@
@	do {
@		a = (m7[0]     +  m7[2])@
@		b = (m7[0]     -  m7[2])@
@		c = (m7[1]>>1) -  m7[3]@
@		d = m7[1]      + (m7[3]>>1)@
@		*(avduint32 *)outstart = 
@			(clip255[((a+d)>>6)+outstart[0]]) |
@			(clip255[((b+c)>>6)+outstart[1]]<<8) |
@			(clip255[((b-c)>>6)+outstart[2]]<<16) |
@			(clip255[((a-d)>>6)+outstart[3]]<<24)@
@		outstart += outlnlength@
@		m7 += 4@
@	} while (--j)@
@//0
		ldrd	r4, [sp]
		ldrd	r6, [sp, #8]		
		ldrb	r1, [r0]
		ldrb	r12, [r0, #1]
		ldrb	r14, [r0, #2]
							
		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d
		
		add		r4, r8, r11		@a+d		
		add		r5, r9, r10		@b+c
		sub		r6, r9, r10		@b-c
		sub		r7, r8, r11		@a-d
		
		add		r4, r1,  r4, asr #6
		ldrb	r1, [r0, #3]	
		usat	r8, #8, r4					
		add		r5, r12, r5, asr #6
		usat	r9, #8, r5	
		add		r6, r14, r6, asr #6
		usat	r10, #8, r6	
		add		r7, r1,  r7, asr #6
		usat	r11, #8, r7	
		
@		ldrb	r8, [r3, r4]
@		ldrb	r9, [r3, r5]
@		ldrb	r10, [r3, r6]
@		ldrb	r11, [r3, r7]
		
		orr		r4, r8, r9, lsl #8
		orr		r4, r4, r10, lsl #16
		orr		r4, r4, r11, lsl #24	
		str	r4, [r0], r2	
@//1		
		ldrd	r4, [sp, #16]
		ldrd	r6, [sp, #24]		
		ldrb	r1, [r0]
		ldrb	r12, [r0, #1]
		ldrb	r14, [r0, #2]
							
		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d
		
		add		r4, r8, r11		@a+d		
		add		r5, r9, r10		@b+c
		sub		r6, r9, r10		@b-c
		sub		r7, r8, r11		@a-d
		
		add		r4, r1,  r4, asr #6
		ldrb	r1, [r0, #3]	
		usat	r8, #8, r4					
		add		r5, r12, r5, asr #6
		usat	r9, #8, r5	
		add		r6, r14, r6, asr #6
		usat	r10, #8, r6	
		add		r7, r1,  r7, asr #6
		usat	r11, #8, r7	
		
@		ldrb	r8, [r3, r4]
@		ldrb	r9, [r3, r5]
@		ldrb	r10, [r3, r6]
@		ldrb	r11, [r3, r7]
		
		orr		r4, r8, r9, lsl #8
		orr		r4, r4, r10, lsl #16
		orr		r4, r4, r11, lsl #24	
		str	r4, [r0], r2
		
@//2		
		ldrd	r4, [sp, #32]
		ldrd	r6, [sp, #40]		
		ldrb	r1, [r0]
		ldrb	r12, [r0, #1]
		ldrb	r14, [r0, #2]
							
		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d
		
		add		r4, r8, r11		@a+d		
		add		r5, r9, r10		@b+c
		sub		r6, r9, r10		@b-c
		sub		r7, r8, r11		@a-d
		
		add		r4, r1,  r4, asr #6
		ldrb	r1, [r0, #3]	
		usat	r8, #8, r4					
		add		r5, r12, r5, asr #6
		usat	r9, #8, r5	
		add		r6, r14, r6, asr #6
		usat	r10, #8, r6	
		add		r7, r1,  r7, asr #6
		usat	r11, #8, r7	
		
@		ldrb	r8, [r3, r4]
@		ldrb	r9, [r3, r5]
@		ldrb	r10, [r3, r6]
@		ldrb	r11, [r3, r7]
		
		orr		r4, r8, r9, lsl #8
		orr		r4, r4, r10, lsl #16
		orr		r4, r4, r11, lsl #24	
		str	r4, [r0], r2
		
@//3		
		ldrd	r4, [sp, #48]
		ldrd	r6, [sp, #56]		
		ldrb	r1, [r0]
		ldrb	r12, [r0, #1]
		ldrb	r14, [r0, #2]
							
		add		r8, r4, r6			@a
		sub		r9, r4, r6			@b
		rsb		r10, r7, r5, asr #1	@c
		add		r11, r5, r7, asr #1	@d
		
		add		r4, r8, r11		@a+d		
		add		r5, r9, r10		@b+c
		sub		r6, r9, r10		@b-c
		sub		r7, r8, r11		@a-d
		
		add		r4, r1,  r4, asr #6
		ldrb	r1, [r0, #3]	
		usat	r8, #8, r4					
		add		r5, r12, r5, asr #6
		usat	r9, #8, r5	
		add		r6, r14, r6, asr #6
		usat	r10, #8, r6	
		add		r7, r1,  r7, asr #6
		usat	r11, #8, r7	
		
@		ldrb	r8, [r3, r4]
@		ldrb	r9, [r3, r5]
@		ldrb	r10, [r3, r6]
@		ldrb	r11, [r3, r7]
		
		orr		r4, r8, r9, lsl #8
		orr		r4, r4, r10, lsl #16
		orr		r4, r4, r11, lsl #24	
		str	r4, [r0], r2														
		
															
        add     r13, r13, #64
        ldmfd   r13!, {r4-r11, r15} @
        
      
	