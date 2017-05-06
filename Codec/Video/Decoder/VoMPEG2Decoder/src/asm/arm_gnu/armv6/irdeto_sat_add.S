@************************************************************************
@									                                    *
@	VisualOn, Inc Confidential and Proprietary, 2011		            *
@								 	                                    *
@***********************************************************************/

	@AREA	|.text|, CODE
	.section .text

	.global Bit16Armv6SatAdd
	
	.ALIGN 4
Bit16Armv6SatAdd: @PROC
   @r0 = buf, r1 = src, r2 = dst, r3 = buf_stride
    stmdb   sp!, {r4 - r12, lr}
    ldr		r4, [sp, #40]      @src_stride
    ldr     r5, [sp, #44]      @dst_stride
    mov     r3,  r3, lsl #1
  
    cmp		r1, #0
	beq		SAT_ADD_NOSRC
	
SAT_ADD_SRC:
	mov     r11, #8
SRC_LOOP:
    ldr     r6, [r0, #12]
    ldr     r7, [r0, #8]
    
    ldrb    r8, [r1, #4]
    ldrb    r9, [r1, #5]   
    orr     r8, r8, r9,  lsl #16
    
    ldrb    r9, [r1, #6]
    ldrb    r10,[r1, #7]
    orr     r9, r9, r10, lsl #16
    
    sadd16  r6, r6, r9
    sadd16  r7, r7, r8    
    usat16	r6, #8, r6
	usat16	r7, #8, r7
	
	pkhbt   r9,  r7, r6, lsl #16
	pkhtb   r6,  r6, r7, asr #16
	orr     r9,  r9, r6, lsl #8	
	str     r9,  [r2, #4]
	
	ldr     r6, [r0, #4]
    ldr     r7, [r0],    r3
    
    ldrb    r8, [r1, #3]
    ldrb    r9, [r1, #2]   
    orr     r8, r9, r8,  lsl #16
    
    ldrb    r9, [r1, #1]
    ldrb    r10,[r1],    r4
    orr     r9, r10, r9, lsl #16
    
    sadd16  r6, r6, r8
    sadd16  r7, r7, r9    
    usat16	r6, #8, r6
	usat16	r7, #8, r7
	
	pkhbt   r9,  r7, r6, lsl #16
	pkhtb   r6,  r6, r7, asr #16
	orr     r9,  r9, r6, lsl #8
	str     r9,  [r2], r5
	
	subs    r11, r11, #1
	bgt     SRC_LOOP
      
    ldmia   sp!, {r4 - r12, pc}
     
SAT_ADD_NOSRC:
	mov     r11, #8
NOSRC_LOOP: 
	ldr     r6, [r0, #12]
    ldr     r7, [r0, #8]
    ldr     r8, [r0, #4]
    ldr     r9, [r0],     r3  
    
	usat16	r6, #8, r6
	usat16	r7, #8, r7
	usat16	r8, #8, r8
	usat16	r9, #8, r9
	
	pkhbt   r10, r9, r8, lsl #16
	pkhtb   r8,  r8, r9, asr #16
	orr     r8,  r10, r8,lsl #8
	pkhbt   r9,  r7, r6, lsl #16
	pkhtb   r6,  r6, r7, asr #16
	orr     r9,  r9, r6, lsl #8
	
	subs    r11, r11, #1
	strd    r8,  [r2], r5
	bgt     NOSRC_LOOP
	

    ldmia   sp!, {r4 - r12, pc} 

	@ENDP
	