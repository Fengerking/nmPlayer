	@EXPORT GetBlockChromaNxN_asm_case0
	@EXPORT GetBlockChromaNxN_asm_case1
	@EXPORT GetBlockChromaNxN_asm_case2		
	      
      .text
	  .align 2
    .globl  GetBlockChromaNxN_asm_case0
    .globl  GetBlockChromaNxN_asm_case1
    .globl  GetBlockChromaNxN_asm_case2
	
@GetBlockChromaNxN_asm_case0(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY)@
GetBlockChromaNxN_asm_case0:
@{
@	width2 = width - blockSizeX@
@	coefB2 = x0 * y0@
@	coefA2 = (x0<<3) - coefB2@ //x0 * (8 - y0j)@
@@	coefB1 = (y0<<3) - coefB2@ //(8 - x0) * y0@
@	coefA1 = 64 - (coefB2 + coefB1 + coefA2)@ //(8 - x0) * (8 - y0)@
@	ref1 = ref0 + width@
@	y0 = blockSizeY@
@	do {
@		x0 = blockSizeX@
@		do {
@			tmpa = (coefA1 * ref0[0] + coefA2 * ref0[1]
@				+ coefB1 * ref1[0] + coefB2 * ref1[1] + 32)>>6@
@			tmpb = (coefA1 * ref0[1] + coefA2 * ref0[2]
@				+ coefB1 * ref1[1] + coefB2 * ref1[2] + 32)>>6@
@			ref1 += 2@
@			ref0 += 2@
@			*byteOutStart++ = tmpa@
@			*byteOutStart++ = tmpb@
@		} while ((x0 -= 2) != 0)@
@		byteOutStart += width3@
@		ref0 += width2@
@		ref1 += width2@
@	} while (--y0)@
@} 
@r0 width@ r1 ref0@ r2 x0@ r3 y0@
@byteOutStart, 	width3,		blockSizeX, blockSizeY
@[r13, #36]		[r13, #40] [r13, #44] 	[r13, #48]
@r0 ref1
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14} @                              @  
        
        mul     r14, r2, r3                       @  coefB2 = r14
        ldr     r5, [r13, #36]                    @  byteOutStart  
        ldr     r9, [r13, #40]                    @  
        ldr     r8, [r13, #44]                    @  
        ldr     r7, [r13, #48]                    @  
        rsb     r12, r14, r2, lsl #3              @  coefA2 = r12
        sub     r6, r0, r8                        @  width2
        rsb     r3, r14, r3, lsl #3               @  coefB1 = r3
        add     r2, r14, r12                      @  
        add     r2, r2, r3                        @  
        rsb     r2, r2, #64                       @  coefA1 = r2
        add     r0, r0, r1                        @  ref1
B_W_C0:
		ldr     r8, [r13, #44]                     @  blockSizeX
        ldrb    r10, [r1]                      @  r10 = ref0[0]
        ldrb    r11, [r0]                      @  r11  = ref1[0]
        mov     r4, r8                            @  blockSizeX countx                
        
L_W_C0:
        mul     r8, r2, r10                       @  r8(tmpa) = coefA1 * ref0[0]
        ldrb    r10, [r1, #1]!                      @  r6 = ref0[1]        
        mla     r8, r3, r11, r8                   @  r8(tmpa) = coefA1 * ref0[0] + coefB1 * ref1[0]
 
        
        ldrb    r11, [r0, #1]!                      @  r7 = ref1[1]
        add     r8, r8, #32                       @     
        mla     r8, r12, r10, r8                   @  r8(tmpa) = coefA1 * ref0[0] + coefB1 * ref1[0] + coefA2 * ref0[1]
        mla     r8, r14, r11, r8                   @  r8(tmpa) = coefA1 * ref0[0] + coefB1 * ref1[0] + coefA2 * ref0[1] + coefB2 * ref1[1]
		subs    r4, r4, #2 						  @  sub count
        mov     r8, r8, asr #6                    @  tmpa
        strb    r8, [r5], #1                      @  *byteOutStart++ = tmpa@
        
        
        mul     r8, r2, r10                        @  r9(tmpb) = coefA1 * ref0[1]
        ldrb    r10, [r1, #1]!                      @  r10 = ref0[2]        
        mla     r8, r3, r11, r8                    @  r9(tmpb) = coefA1 * ref0[1] + coefB1 * ref1[1]        
        
        ldrb    r11, [r0, #1]!                      @  r11 = ref1[2]
        add     r8, r8, #32                       @         
        mla     r8, r12, r10, r8                   @  r9(tmpb) = coefA1 * ref0[1] + coefB1 * ref1[1] + coefA2 * ref0[2]
        mla     r8, r14, r11, r8                   @  r9(tmpb) = coefA1 * ref0[1] + coefB1 * ref1[1] + coefA2 * ref0[2] + coefB2 * ref1[2]
        mov     r8, r8, asr #6                    @  tmpb
        strb    r8, [r5], #1                      @  *byteOutStart++ = tmpb@        
        

        bne     L_W_C0

        add     r1, r1, r6                        @  
        add     r0, r0, r6                        @  
        add     r5, r5, r9                        @  
        subs    r7, r7, #1                        @  
        bne     B_W_C0                            @  
        
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  37_1
@GetBlockChromaNxN_asm_case1(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY)@
GetBlockChromaNxN_asm_case1:
@{
@	width2 = width - blockSizeX@
@	if (x0){
@		y0 = blockSizeY@
@		do {
@			k = blockSizeX@
@			do {
@				tmpa = (((ref0[1] - ref0[0]) * x0 + 4)>>3) + ref0[0]@
@				tmpb = (((ref0[2] - ref0[1]) * x0 + 4)>>3) + ref0[1]@
@				ref0 += 2@
@			*byteOutStart++ = tmpa@
@			*byteOutStart++ = tmpb@
@			} while ((k -= 2) != 0)@
@			byteOutStart += width3@
@			ref0 += width2@
@		} while (--y0)@
@	}
@	else {
@		y0 = blockSizeY@
@		do {
@			x0 = blockSizeX@
@			do {
@				*byteOutStart++ = *ref0++@
@				*byteOutStart++ = *ref0++@
@			} while ((x0 -= 2) != 0)@
@			byteOutStart += width3@
@			ref0 += width2@
@		} while (--y0)@
@	}
@}
@r0 width@ r1 ref0@ r2 x0@ r3 y0@
@byteOutStart, 	width3,		blockSizeX, blockSizeY
@[r13, #32]		[r13, #36] [r13, #40] 	[r13, #44]

        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r14} @  42_1

        ldr     r6, [r13, #32]                    @  byteOutStart
        ldr     r9, [r13, #36]                    @  width3
        ldr     r8, [r13, #40]                    @  blockSizeX
        ldr     r5, [r13, #44]                    @  blockSizeY
        cmp     r2, #0                            @  
        sub     r7, r0, r8                        @  width2
        bne     CASE_TRUE 
@		do {
@			do {
@				*byteOutStart++ = *ref0++@
@				*byteOutStart++ = *ref0++@
@			} while ((blockSizeX -= 2) != 0)@
@			byteOutStart += width3@
@			ref0 += width2@
@		} while (--blockSizeY)@                                    @  
W_LAB:
        mov     r0, r8                            @  42_1
CPY_LAB:
		@@pld		[r1, #2] 
        ldrb    r3, [r1], #1                      @  *ref0++@
        ldrb    r4, [r1], #1                      @  *ref0++@
        subs    r0, r0, #2                        @  count - 2             
        orr		r4, r3, r4, lsl #8  
        strh    r4, [r6], #2                      @  *byteOutStart++ = tmpa@ *byteOutStart++ = tmpb@   
        bne     CPY_LAB                             @  

        add     r6, r6, r9                        @  
        add     r1, r1, r7                        @  
        subs    r5, r5, #1                        @  
        bne     W_LAB                             @  

        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r15} @
CASE_TRUE:                          @ Preds _L2_1
@		y0 = blockSizeY@
@		do {
@			k = blockSizeX@
@			do {
@				tmpa = (((ref0[1] - ref0[0]) * x0 + 4)>>3) + ref0[0]@
@				tmpb = (((ref0[2] - ref0[1]) * x0 + 4)>>3) + ref0[1]@
@				ref0 += 2@
@			*byteOutStart++ = tmpa@
@			*byteOutStart++ = tmpb@
@			} while ((k -= 2) != 0)@
@			byteOutStart += width3@
@			ref0 += width2@
@		} while (--y0)@
W_LAB_T:
        mov     r10, r8 
        ldrb    r12, [r1]	                      @  ref0[0]                                  @  
CPY_LAB_T:
		@pld		[r1, #3] 
        ldrb    r4, [r1, #1]!                      @  ref0[1]
        subs    r10, r10, #2                      @  count - 2        
        sub     r3, r4, r12                       @  (ref0[1] - ref0[0])
        mul     r3, r2, r3                        @  (ref0[1] - ref0[0]) * x0
        add     r0, r3, #4                        @  
        add     r3, r12, r0, asr #3               @  

        
        ldrb    r12, [r1, #1]!                    @  ref0[0]       
                
        sub     r14, r12, r4                      @  (ref0[2] - ref0[1])
        mul     r14, r2, r14                      @  (ref0[2] - ref0[1]) * x0
        
        add     r0, r14, #4                       @  
        add     r0, r4, r0, asr #3            
        orr		r0, r3, r0, lsl #8  
        strh    r0, [r6], #2                      @  *byteOutStart++ = tmpa@ *byteOutStart++ = tmpb@
        bne     CPY_LAB_T                         @  

        add     r6, r6, r9                        @  
        add     r1, r1, r7                        @  
        subs    r5, r5, #1                        @  
        bne     W_LAB_T                           @  

        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r15} @

@GetBlockChromaNxN_asm_case2(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY)@
GetBlockChromaNxN_asm_case2:
@{
@	width2 = width - blockSizeX@
@	ref1 = ref0 + width@
@	k = blockSizeY@
@	do {
@		x0 = blockSizeX@
@		do {
@			tmpa = (((ref1[0] - ref0[0]) * y0 + 4)>>3) + ref0[0]@
@			tmpb = (((ref1[1] - ref0[1]) * y0 + 4)>>3) + ref0[1]@
@			ref0 += 2@
@			ref1 += 2@
@			*byteOutStart++ = tmpa@
@			*byteOutStart++ = tmpb@
@		} while ((x0 -= 2) != 0)@
@		byteOutStart += width3@
@		ref0 += width2@
@		ref1 += width2@
@	} while (--k)@
@}
@r0 width@ r1 ref0@ r2 x0@ r3 y0@
@byteOutStart, 	width3,		blockSizeX, blockSizeY
@[r13, #36]		[r13, #40] [r13, #44] 	[r13, #48]

        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14} @                    @           

        ldr     r5, [r13, #36]                    @  byteOutStart
        ldr     r4, [r13, #40]                    @  width3
        ldr     r14, [r13, #44]                   @  blockSizeX
        ldr     r12, [r13, #48]                   @  blockSizeY
        add     r2, r0, r1                        @  ref1
        sub     r0, r0, r14                       @  width2
B_W_C2:
        mov     r6, r14                           @  blockSizeX count
L_W_C2:
        ldrb    r10, [r1], #1                     @  ref0[0]
        ldrb    r8, [r2], #1                      @  ref1[0]
        ldrb    r9, [r1], #1                      @  ref0[1]
        ldrb    r7, [r2], #1                      @  ref1[1]
        subs    r6, r6, #2                        @  blockSizeX count - 2        
        sub     r8, r8, r10                       @  
        mul     r11, r3, r8                       @  
        sub     r7, r7, r9                        @  
        mul     r7, r3, r7                        @  
        add     r11, r11, #4                      @  
        add     r7, r7, #4                        @  
        add     r8, r10, r11, asr #3              @  
        add     r7, r9, r7, asr #3                @
        orr		r7, r8, r7, lsl #8  
        strh    r7, [r5], #2                      @  *byteOutStart++ = tmpa@ *byteOutStart++ = tmpb@
        
        bne     L_W_C2                            @

        add     r5, r5, r4                        @  byteOutStart += width3@
        add     r1, r1, r0                        @  ref0 += width2@
        add     r2, r2, r0                        @  ref1 += width2@
        subs    r12, r12, #1                      @  blockSizeY count - 1
        bne     B_W_C2                            @  
                   @  
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  