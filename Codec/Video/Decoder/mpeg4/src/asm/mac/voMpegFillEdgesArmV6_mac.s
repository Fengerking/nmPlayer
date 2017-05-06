#include "../../voMpeg4DID.h"

	.text
	.align 4 
	.globl _fill_edge_y_armv6 
	.globl _fill_edge_uv_armv6
		
_fill_edge_y_armv6:
@void image_setedges_t(uint8_t *src_in, const uint32_t ExWidth, uint32_t width, uint32_t height, uint32_t edge)
@{
@	uint32_t i@
@	uint8_t *dst,*src@
@	int iedge = edge@
@
@	src = src_in@
@	dst = src - (iedge + iedge * ExWidth)@
@
@	for (i = 0@ i < iedge@ i++) {
@		memset(dst, *src, iedge)@
@		memcpy(dst + iedge, src, width)@
@		memset(dst + ExWidth - iedge, *(src + width - 1),
@			   iedge)@
@		dst += ExWidth@
@	}
@	for (i = 0@ i < height@ i++) {
@		memset(dst, *src, iedge)@
@		memset(dst + ExWidth - iedge, src[width - 1], iedge)@
@		dst += ExWidth@
@		src += ExWidth@
@	}
@	src -= ExWidth@
@	for (i = 0@ i < iedge@ i++) {
@		memset(dst, *src, iedge)@
@		memcpy(dst + iedge, src, width)@
@		memset(dst + ExWidth - iedge, *(src + width - 1),
@				   iedge)@
@		dst += ExWidth@
@	}
@} 
@		1111111155555555555555522222222
@		1111111155555555555555522222222
@		1111111155555555555555522222222
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		66666666@@@@@@@@@@@@@@@88888888
@		3333333377777777777777744444444
@		3333333377777777777777744444444
@		3333333377777777777777744444444
								
	stmdb	sp!, {r4 - r11, lr}
	@r0 Y, r1 ExWidth, r2 width, r3  height
	ldr	r14, [sp, #36]		@r4 EDGE_SIZE
	
@Y5	
	mov	r8, r0	
	mov	r12, r2						
Y5_loop: 				@32 x width	
        ldrd     r4, [r8]
        ldrd     r6, [r8, #8]

	sub	r9, r8, r1
	mov	r11, r14						
Y5in_loop: 	
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 			 			                    
	subs        r11, r11, #4	@EDGE_SIZE  		
	bgt         Y5in_loop 
	       
        add	r8, r8, #16                
	subs        r12, r12, #16	@width  		
	bgt         Y5_loop 
		
@Y1 Y2
	sub	r12, r2, #1		@r7 = width - 1	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r12]
	
	mul	r10, r1, r14		@r10 = EDGE_SIZE * ExWidth
	add	r11, r10, r14		@r11 = EDGE_SIZE * ExWidth + EDGE_SIZE
	sub	r8, r0, r11		@r8 = Y -(EDGE_SIZE + EDGE_SIZE * ExWidth)
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	add	r11, r2, r14
	add	r9, r8, r11
	mov	r12, r14						
Y12_loop:				@32x32 	
        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r4, [r8, #16]
        strd     r4, [r8, #24]
        strd     r6, [r9]
        strd     r6, [r9, #8]
        strd     r6, [r9, #16]
        strd     r6, [r9, #24]
        add	r8, r8, r1
        add	r9, r9, r1                       
@/////////////////
	subs        r12, r12, #1	@EDGE_SIZE  		
	bgt         Y12_loop 
	
@Y6 Y8	
@	mov	r8, r0	
	mov	r12, r3
	sub	r11, r2, #1		@r7 = width - 1						
Y68_loop: 	
	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r11]
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	sub	r9, r0, r14							
        strd     r4, [r9], #8
        strd     r4, [r9], #8
        strd     r4, [r9], #8
        strd     r4, [r9], #8
        
	add	r9, r9, r2
        strd     r6, [r9], #8
        strd     r6, [r9], #8
        strd     r6, [r9], #8
        strd     r6, [r9], #8
       
        add	r0, r0, r1                
	subs        r12, r12, #1	@height  		
	bgt         Y68_loop
	@r0    =  3 + r1
        sub	r0, r0, r1		
@Y7	
	mov	r8, r0	
	mov	r12, r2						
Y7_loop: 				@32 x width	
        ldrd     r4, [r8]
        ldrd     r6, [r8, #8]

	add	r9, r8, r1
	mov	r11, r14						
Y7in_loop: 	
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1			 			                    
	subs        r11, r11, #4	@EDGE_SIZE  		
	bgt         Y7in_loop 
	       
        add	r8, r8, #16                
	subs        r12, r12, #16	@width  		

	bgt         Y7_loop 
		
	@r0    =  3 + r1
@Y3 Y4
	sub	r12, r2, #1		@r7 = width - 1	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r12]
	
	sub	r8, r0, r14		@r8 = Y -EDGE_SIZE
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	add	r11, r2, r14
	add	r9, r8, r11
	mov	r12, r14						
Y34_loop: 	
        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r4, [r8, #16]
        strd     r4, [r8, #24]
        strd     r6, [r9]
        strd     r6, [r9, #8]
        strd     r6, [r9, #16]
        strd     r6, [r9, #24]
                
        add	r8, r8, r1
        add	r9, r9, r1                       
@/////////////////
	subs        r12, r12, #1	@EDGE_SIZE  		
	bgt         Y34_loop 	
	
	ldmia       sp!, {r4 - r11, pc}	
		
_fill_edge_uv_armv6:	
	stmdb	sp!, {r4 - r11, lr}	
	@r0 Y, r1 ExWidth, r2 width, r3  height
	ldr	r14, [sp, #36]		@r4 EDGE_SIZE
	
	
@Y5	
	mov	r8, r0	
	mov	r12, r2						
Y5_loopUV: 				@32 x width	
        ldrd     r4, [r8]
        ldrd     r6, [r8, #8]

	sub	r9, r8, r1
	mov	r11, r14						
Y5in_loopUV: 	
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	sub	r9, r9, r1 			 			                    
	subs        r11, r11, #4	@EDGE_SIZE  		
	bgt         Y5in_loopUV 
	       
        add	r8, r8, #16                
	subs        r12, r12, #16	@width  		
	bgt         Y5_loopUV 
		
@Y1 Y2
	sub	r12, r2, #1		@r7 = width - 1	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r12]
	
	mul	r10, r1, r14		@r10 = EDGE_SIZE * ExWidth
	add	r11, r10, r14		@r11 = EDGE_SIZE * ExWidth + EDGE_SIZE
	sub	r8, r0, r11		@r8 = Y -(EDGE_SIZE + EDGE_SIZE * ExWidth)
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	add	r11, r2, r14
	add	r9, r8, r11
	mov	r12, r14						
Y12_loopUV: 	
        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r6, [r9]
        strd     r6, [r9, #8]           
        add	r8, r8, r1
        add	r9, r9, r1                       
@/////////////////

        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r6, [r9]
        strd     r6, [r9, #8]           
        add	r8, r8, r1
        add	r9, r9, r1                       

	subs        r12, r12, #2	@EDGE_SIZE  		
	bgt         Y12_loopUV 
	
@Y6 Y8	
@	mov	r8, r0	
	mov	r12, r3
	sub	r11, r2, #1		@r7 = width - 1						
Y68_loopUV: 	
	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r11]
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	sub	r9, r0, r14							
        strd     r4, [r9], #8
        strd     r4, [r9], #8
        
	add	r9, r9, r2
        strd     r6, [r9], #8
        strd     r6, [r9], #8
       
        add	r0, r0, r1                
	subs        r12, r12, #1	@height  		
	bgt         Y68_loopUV	
	@r0    =  3 + r1
        sub	r0, r0, r1
        
@Y7	
	mov	r8, r0	
	mov	r12, r2						
Y7_loopUV: 	
        ldrd     r4, [r8]
        ldrd     r6, [r8, #8]

	add	r9, r8, r1
	mov	r11, r14						
Y7in_loopUV: 	
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1 
        strd     r4, [r9]
        strd     r6, [r9, #8]
	add	r9, r9, r1 			 			                    
	subs        r11, r11, #4	@EDGE_SIZE  		
	bgt         Y7in_loopUV 
	       
        add	r8, r8, #16                
	subs        r12, r12, #16	@width  		

	bgt         Y7_loopUV         	
		
	@r0    =  3 + r1
@Y3 Y4
	sub	r12, r2, #1		@r7 = width - 1	
	ldrb	r4, [r0]
	ldrb	r6, [r0, r12]
	
	sub	r8, r0, r14		@r8 = Y -EDGE_SIZE
	
	orr	r4, r4, r4, lsl #8
	orr	r6, r6, r6, lsl #8	
	orr	r4, r4, r4, lsl #16
	orr	r6, r6, r6, lsl #16	
	mov	r5, r4
	mov	r7, r6

	add	r11, r2, r14
	add	r9, r8, r11
	mov	r12, r14						
Y34_loopUV: 	
        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r6, [r9]
        strd     r6, [r9, #8]           
        add	r8, r8, r1
        add	r9, r9, r1                       
@/////////////////

        strd     r4, [r8]
        strd     r4, [r8, #8]
        strd     r6, [r9]
        strd     r6, [r9, #8]           
        add	r8, r8, r1
        add	r9, r9, r1                       


	subs        r12, r12, #2	@EDGE_SIZE  		
	bgt         Y34_loopUV 	
	
	ldmia       sp!, {r4 - r11, pc}				
	@.end	

