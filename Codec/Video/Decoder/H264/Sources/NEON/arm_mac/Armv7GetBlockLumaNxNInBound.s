

@armasm -cpu xscale (InputPath) "(IntDir)/(InputName).obj"
@@ -- Begin  GetBlockLumaNxNInBound
@@ mark_begin@
      
    .text
	  .align 2
    .globl  _ARM_GB1
    
	.macro b_EXIT_FUNC
	
        add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
	.endm
   					
	.macro GetFULL
	
offset0			.req	r2
OutLoopCount 	.req	r7
InnerLoopCount 	.req	r14
src1			.req	r3
src2			.req	r8
offset1 		.req	r9
	
	mov	OutLoopCount,$5	
	cmp	$4, #8
	@@pld	[$0, $1, lsl #1] 
	bpl	v7_FULL_OUT
		
	ands	offset0,$0,#3
	subne	$0,$0,offset0
	movne	offset0,offset0,lsl #3	

	add	$1,$1,#4
	add	$3,$3,#4	
	cmp		offset0,#0
	bne		FULL_OUT_
FULL_OUT:
	ldr    src1,[$0],$1
	@@pld	[$0, $1, lsl #1] 	
	subs   OutLoopCount,OutLoopCount,#1	
	str    src1,[$2],$3	
	bne    FULL_OUT 
	b_EXIT_FUNC
	
FULL_OUT_:
	rsb  offset1,offset0,#32
FULL_OUT_B:	
	ldr  src2,[$0,#4]
	ldr    src1,[$0],$1
	@@pld	[$0, $1, lsl #1] 	
	subs   OutLoopCount,OutLoopCount,#1	
	mov  src1,src1,lsr offset0
	eor  src1,src1,src2,lsl offset1 
	str    src1,[$2],$3
	bne    FULL_OUT_B 
	b_EXIT_FUNC

v7_FULL_OUT:
	bgt	v7_FULL_OUT_16
	add	$1,$1,#8
	add	$3,$3,#8	
FULL_OUT_v7_8:
	vld1.8 {d0}, [$0],$1
	@@pld	[$0, $1, lsl #1] 		
	subs   OutLoopCount,OutLoopCount,#1	
	vst1.64 {d0}, [$2],$3
	bne    FULL_OUT_v7_8 
	b_EXIT_FUNC

v7_FULL_OUT_16:	
FULL_OUT_v7_16:
	mov    InnerLoopCount,$4
FULL_INNER_v7_16:
	vld1.8 {q0}, [$0]!
	subs   InnerLoopCount,InnerLoopCount,#16
	vst1.64 {q0}, [$2]!
	bne	   FULL_INNER_v7_16	
	add	$0,$0,$1
	add	$2,$2,$3
	subs   OutLoopCount,OutLoopCount,#1
	@@pld	[$0, $1, lsl #1] 	
	bgt    FULL_OUT_v7_16 
	b_EXIT_FUNC
		.endm
		
		.macro GetDYP1_DXP1
GetDYP1_DXP1_v7:	
	vmov.s16 q10, #5
	vmov.s16 q11, #16	
	sub	r0, r0, #2	
	cmp	r3, #1
        add     r7, r7, r6                        @ 	
	bgt	refTmp_srcy_1
	
	cmp     r6,#8
    blt GetDYP1_DXP1_4	
	bgt GetDYP1_DXP1_16			
        add     r4, r4, r6                        @ 	
GetDYP1_DXP1_B8:
@	vld1.8 {d0}, [r0]!				@ref0[-2~5]
@	vld1.8 {d12}, [r0]				@ref0[6~13]
@	@@pld	[r0, r4] 	
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d30, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
					
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3
		
	vqshl.u16 q2, q2, #2
	vsub.s16 q2, q2, q1
	vadd.u16 q0, q0, q11	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @
@        add     r0, r0, r4                        @          		
	vadd.s16 q2, q2, q0						
	vqshrun.s16 d16, q2, #5	
	vrhadd.u8 d16, d16, d30		
	vst1.64	{d16}, [r1], r7     
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP1_B8                           @ 
        b_EXIT_FUNC
  
GetDYP1_DXP1_4:
        add     r4, r4, r6                        @ 
GetDYP1_DXP1_B4:
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 							
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d30, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]	

	
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3	
	
	vqshl.u16 d4, d4, #2
	vsub.s16 d4, d4, d2
	vadd.u16 d0, d0, d22	
	vmul.s16 d4, d4, d20
@        add     r0, r0, r4                        @ 
        subs    r10, r10, #1                        @         	
	vadd.s16 d4, d4, d0						
	vqshrun.s16 d16, q2, #5
	vrhadd.u8 d16, d16, d30	
	vst1.32	{d16[0]}, [r1], r7
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP1_B4                           @ 
        b_EXIT_FUNC
                   
GetDYP1_DXP1_16:

GetDYP1_DXP1_B16:
	vld1.8 {d0}, [r0]!				@ref0[-2~5]
	vld1.8 {d12}, [r0]!				@ref0[6~13]
	vld1.8 {d14}, [r0]				@ref0[14~21]	
	@@pld	[r0, r4] 	
@one								@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vext.u8 d24, d12, d14, #1			@m1    ref0[-1~6]
	vext.u8 d28, d12, d14, #2			@p0    ref0[0~7]
	vext.u8 d25, d12, d14, #3			@p1    ref0[1~8]
	vext.u8 d26, d12, d14, #4			@p2    ref0[2~9]
	vext.u8 d27, d12, d14, #5			@p3    ref0[3~10]	
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d11, d6				@p0+p1
	
	vaddl.u8 q15, d12, d27				@m2+p3
	vaddl.u8 q3, d24, d26				@m1+p2							
	vaddl.u8 q4, d28, d25				@p0+p1
		
	vqshl.u16 q2, q2, #2
	vqshl.u16 q4, q4, #2	
	vsub.s16 q2, q2, q1
	vsub.s16 q4, q4, q3		
	vmul.s16 q2, q2, q10
	vmul.s16 q4, q4, q10
        subs    r10, r10, #1                        @  		
	vadd.u16 q0, q11, q0
	vadd.u16 q15, q11, q15			
	vadd.s16 q2, q2, q0
	vadd.s16 q4, q4, q15							
	vqshrun.s16	d16, q2, #5
	vqshrun.s16	d17, q4, #5	
	vrhadd.u8 d16, d16, d11	
	vrhadd.u8 d17, d17, d28	
        add     r0, r0, r4                        @ 		
@tow									@m2 d12 ref0[-2~5]
	vst1.64	{q8}, [r1], r7	    
@        add     r1, r1, r7                        @          
        bne     GetDYP1_DXP1_B16                           @ 
        b_EXIT_FUNC


refTmp_srcy_1:
	cmp     r6,#8
    blt GetDYP1_DXP1_4_srcy_1	
	bgt GetDYP1_DXP1_16_srcy_1			
        add     r4, r4, r6                        @	
GetDYP1_DXP1_B8_srcy_1:
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 	
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d6, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d30, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
					
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3
		
	vqshl.u16 q2, q2, #2
	vsub.s16 q2, q2, q1
	vadd.u16 q0, q0, q11	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @
@        add     r0, r0, r4                        @          		
	vadd.s16 q2, q2, q0						
	vqshrun.s16 d16, q2, #5	
	vrhadd.u8 d16, d16, d30		
	vst1.64	{d16}, [r1], r7    
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP1_B8_srcy_1                           @ 
        b_EXIT_FUNC
  
GetDYP1_DXP1_4_srcy_1:
        add     r4, r4, r6                        @
GetDYP1_DXP1_B4_srcy_1:
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 	
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d6, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d30, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]	
	
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3	
	
	vqshl.u16 d4, d4, #2
	vsub.s16 d4, d4, d2
	vadd.u16 d0, d0, d22	
	vmul.s16 d4, d4, d20
@        add     r0, r0, r4                        @ 
        subs    r10, r10, #1                        @         	
	vadd.s16 d4, d4, d0						
	vqshrun.s16 d16, q2, #5
	vrhadd.u8 d16, d16, d30	
	vst1.32	{d16[0]}, [r1], r7
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP1_B4_srcy_1                           @ 
        b_EXIT_FUNC
                   
GetDYP1_DXP1_16_srcy_1:

GetDYP1_DXP1_B16_srcy_1:
	vld1.8 {d0}, [r0]!				@ref0[-2~5]
	vld1.8 {d12}, [r0]!				@ref0[6~13]
	vld1.8 {d14}, [r0]				@ref0[14~21]	
	@@pld	[r0, r4] 	
@one								@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d6, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d11, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vext.u8 d24, d12, d14, #1			@m1    ref0[-1~6]
	vext.u8 d25, d12, d14, #2			@p0    ref0[0~7]
	vext.u8 d28, d12, d14, #3			@p1    ref0[1~8]
	vext.u8 d26, d12, d14, #4			@p2    ref0[2~9]
	vext.u8 d27, d12, d14, #5			@p3    ref0[3~10]	
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d11, d6				@p0+p1
	
	vaddl.u8 q15, d12, d27				@m2+p3
	vaddl.u8 q3, d24, d26				@m1+p2							
	vaddl.u8 q4, d28, d25				@p0+p1
		
	vqshl.u16 q2, q2, #2
	vqshl.u16 q4, q4, #2	
	vsub.s16 q2, q2, q1
	vsub.s16 q4, q4, q3		
	vmul.s16 q2, q2, q10
	vmul.s16 q4, q4, q10
        subs    r10, r10, #1                        @  		
	vadd.u16 q0, q11, q0
	vadd.u16 q15, q11, q15			
	vadd.s16 q2, q2, q0
	vadd.s16 q4, q4, q15							
	vqshrun.s16	d16, q2, #5
	vqshrun.s16	d17, q4, #5	
	vrhadd.u8 d16, d16, d11	
	vrhadd.u8 d17, d17, d28	
        add     r0, r0, r4                        @ 		
@tow									@m2 d12 ref0[-2~5]
	vst1.64	{q8}, [r1], r7	    
@        add     r1, r1, r7                        @          
        bne     GetDYP1_DXP1_B16_srcy_1                           @ 
        b_EXIT_FUNC

    .endm	
    
    .macro GetDYP1_DXP0
	vmov.s16 q10, #5
	vmov.s16 q11, #16	
	sub	r0, r0, #2
	cmp     r6,#8
        add     r7, r7, r6                        @	
    blt GetDYP1_DXP0_4	
	bgt GetDYP1_DXP0_16			
        add     r4, r4, r6                        @	
GetDYP1_DXP0_B8:
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 							
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d30, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]	
					
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3
		
	vqshl.u16 q2, q2, #2
	vsub.s16 q2, q2, q1
	vadd.u16 q0, q0, q11	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @
@        add     r0, r0, r4                        @          		
	vadd.s16 q2, q2, q0						
	vqshrun.s16 d16, q2, #5	
	vst1.64	{d16}, [r1], r7     
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP0_B8                           @ 
        b_EXIT_FUNC
  
GetDYP1_DXP0_4:
        add     r4, r4, r6                        @
GetDYP1_DXP0_B4:
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@@pld	[r0, r4] 							
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d30, d0, d1, #2			@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]	
	
	vaddl.u8 q2, d30, d6				@p0+p1
	vaddl.u8 q1, d2, d8				@m1+p2	
	vaddl.u8 q0, d0, d10				@m2+p3	
	
	vqshl.u16 d4, d4, #2
	vsub.s16 d4, d4, d2
	vadd.u16 d0, d0, d22	
	vmul.s16 d4, d4, d20
@        add     r0, r0, r4                        @ 
        subs    r10, r10, #1                        @         	
	vadd.s16 d4, d4, d0						
	vqshrun.s16 d16, q2, #5
	vst1.32	{d16[0]}, [r1], r7
@        add     r1, r1, r7                        @           
        bne     GetDYP1_DXP0_B4                           @ 
        b_EXIT_FUNC
                   
GetDYP1_DXP0_16:

GetDYP1_DXP0_B16:
	vld1.8 {d0}, [r0]!				@ref0[-2~5]
	vld1.8 {d12}, [r0]!				@ref0[6~13]
	vld1.8 {d14}, [r0]				@ref0[14~21]	
	@@pld	[r0, r4] 	
@one								@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vext.u8 d24, d12, d14, #1			@m1    ref0[-1~6]
	vext.u8 d28, d12, d14, #2			@p0    ref0[0~7]
	vext.u8 d25, d12, d14, #3			@p1    ref0[1~8]
	vext.u8 d26, d12, d14, #4			@p2    ref0[2~9]
	vext.u8 d27, d12, d14, #5			@p3    ref0[3~10]	
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d11, d6				@p0+p1
	
	vaddl.u8 q15, d12, d27				@m2+p3
	vaddl.u8 q3, d24, d26				@m1+p2							
	vaddl.u8 q4, d28, d25				@p0+p1
		
	vqshl.u16 q2, q2, #2
	vqshl.u16 q4, q4, #2	
	vsub.s16 q2, q2, q1
	vsub.s16 q4, q4, q3		
	vmul.s16 q2, q2, q10
	vmul.s16 q4, q4, q10
        subs    r10, r10, #1                        @  		
	vadd.u16 q0, q11, q0
	vadd.u16 q15, q11, q15			
	vadd.s16 q2, q2, q0
	vadd.s16 q4, q4, q15							
	vqshrun.s16	d16, q2, #5
	vqshrun.s16	d17, q4, #5	
        add     r0, r0, r4                        @ 		
@tow									@m2 d12 ref0[-2~5]
	vst1.64	{q8}, [r1], r7	    
@        add     r1, r1, r7                        @          
        bne     GetDYP1_DXP0_B16                           @ 
        b_EXIT_FUNC
	.endm
	
	.macro GetDXP1_DYP1
	vmov.s16 q10, #5
	vmov.s16 q15, #16	
	mov	r4, r2			
@	sub	r2, r0, #2
	sub	r2, r0, r8, lsl #1
	mov	r3, r1			
	cmp	r9, #1
	bgt	GetDXP1_DYP1_refTmp_srcy_1

	cmp     r10,#8
    blt GetDXP1_DYP1_4	
	bgt GetDXP1_DYP1_16			
	mov	r0, r2	
GetDXP1_DYP1_B8:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d29

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]	
 
        bne     GetDXP1_DYP1_B8                           @ 
	        
        b_EXIT_FUNC
  
GetDXP1_DYP1_4:
	mov	r0, r2     
GetDXP1_DYP1_B4:
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 d26, d14, d18
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 d26, d14, d18
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 d26, d14, d18
	@@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	@@pld	[r0, r8, lsl #1]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vadd.s16 d26, d14, d18
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d29

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1]	 
        bne     GetDXP1_DYP1_B4                           @

        b_EXIT_FUNC
                   
GetDXP1_DYP1_16:
	mov	r0, r2
GetDXP1_DYP1_B16:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0], r8			@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4	
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5	
	vrhadd.u8 d25, d25, d29

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1], r4			@tmpResJ[7]
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0]				@ref0[5]	
	
	mov	r0, r2	
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d4, d0, #2				@p0    ref0[0~7]
	vext.u8 d10, d4, d0, #3				@p1    ref0[1~8]															
	vext.u8 d8, d4, d0, #1				@m1    ref0[-1~6]
	vext.u8 d11, d4, d0, #4				@p2    ref0[2~9]
	vext.u8 d12, d4, d0, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d4, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d5, d1, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d5, d1, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d5, d1, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d5, d1, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d5, d1, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d5, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d6, d2, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d6, d2, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d6, d2, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d6, d2, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d6, d12				@m2+p3	
	vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d6, d2, #2				@p0    ref0[0~7]
	vext.u8 d8, d7, d3, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d7, d3, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d7, d3, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d7, d3, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d7, d3, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d7, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d29
		
	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]
	
        bne     GetDXP1_DYP1_B16                           @ 
        b_EXIT_FUNC	     	



GetDXP1_DYP1_refTmp_srcy_1:

	cmp     r10,#8
    blt GetDXP1_DYP1_4_srcy_1	
	bgt GetDXP1_DYP1_16_srcy_1			
	mov	r0, r2	
GetDXP1_DYP1_B8_srcy_1:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d29, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d28, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d10, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d28, d10				@p0+p1		
	vext.u8 d29, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d28									@m2 d2 ref0[-2~5]
	vext.u8 d10, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d28, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d10, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d28, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d28

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]	
 
        bne     GetDXP1_DYP1_B8_srcy_1                           @ 
	        
        b_EXIT_FUNC
  
GetDXP1_DYP1_4_srcy_1:
	mov	r0, r2     
GetDXP1_DYP1_B4_srcy_1:
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d29, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d28, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 d26, d14, d18
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d10, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d28, d10				@p0+p1		
	vext.u8 d29, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 d26, d14, d18
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d28									@m2 d2 ref0[-2~5]
	vext.u8 d10, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d28, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 d26, d14, d18
	@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d10, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d28, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vadd.s16 d26, d14, d18
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d28

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1]	 
        bne     GetDXP1_DYP1_B4_srcy_1                           @

        b_EXIT_FUNC
                   
GetDXP1_DYP1_16_srcy_1:
	mov	r0, r2
GetDXP1_DYP1_B16_srcy_1:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0], r8			@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4	
										@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d29, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d28, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d10, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d28, d10				@p0+p1		
	vext.u8 d29, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	vrhadd.u8 d23, d23, d28									@m2 d2 ref0[-2~5]
	vext.u8 d10, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d28, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d10, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d28, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5	
	vrhadd.u8 d25, d25, d28

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1], r4			@tmpResJ[7]
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0]				@ref0[5]	
	
	mov	r0, r2	
										@m2 d0 ref0[-2~5]
	vext.u8 d10, d4, d0, #2				@p0    ref0[0~7]
	vext.u8 d29, d4, d0, #3				@p1    ref0[1~8]															
	vext.u8 d8, d4, d0, #1				@m1    ref0[-1~6]
	vext.u8 d11, d4, d0, #4				@p2    ref0[2~9]
	vext.u8 d12, d4, d0, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d4, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d28, d5, d1, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d5, d1, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d5, d1, #4				@p2    ref0[2~9]	
	vadd.s16 q13, q7, q9
	vext.u8 d12, d5, d1, #5				@p3    ref0[3~10]
	vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d10, d5, d1, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d5, d12				@m2+p3
	vaddl.u8 q9, d28, d10				@p0+p1		
	vext.u8 d29, d6, d2, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d6, d2, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d6, d2, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d6, d2, #5				@p3    ref0[3~10]		
	vadd.s16 q13, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d6, d12				@m2+p3	
	vrhadd.u8 d23, d23, d28									@m2 d2 ref0[-2~5]
	vext.u8 d10, d6, d2, #2				@p0    ref0[0~7]
	vext.u8 d8, d7, d3, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d28, d7, d3, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d7, d3, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d7, d3, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d10, d7, d3, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d28, d10				@p0+p1				
	vaddl.u8 q7, d7, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q13, q7, q9
	
	vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	vrhadd.u8 d25, d25, d28
		
	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]
	
        bne     GetDXP1_DYP1_B16_srcy_1                           @ 
        b_EXIT_FUNC	     	
     	
	
	.endm
	
	.macro GetDXP1_DYP0
	vmov.s16 q10, #5
	vmov.s16 q15, #16	
	mov	r4, r2			
@	sub	r2, r0, #2
	sub	r2, r0, r8, lsl #1
	mov	r3, r1			
	cmp     r10,#8
    blt GetDXP1_DYP0_4	
	bgt GetDXP1_DYP0_16			
	mov	r0, r2	
GetDXP1_DYP0_B8:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q11, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	@vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	@vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q12, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	@vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	@vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	@vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	@vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q14, q7, q9
	
	@vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	@vrhadd.u8 d25, d25, d29
	vqshrun.s16	d22, q11, #5	
	vqshrun.s16	d23, q12, #5
	vqshrun.s16	d24, q13, #5
	vqshrun.s16	d25, q14, #5	

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]	
 
        bne     GetDXP1_DYP0_B8                           @ 
	        
        b_EXIT_FUNC
  
GetDXP1_DYP0_4:
	mov	r0, r2     
GetDXP1_DYP0_B4:
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4
	mov	r0, r2		
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 d22, d14, d18
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	@vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	@vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 d24, d14, d18
	vaddl.u8 q8, d8, d11				@m1+p2						
	@vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	@vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 d26, d14, d18
	@pld	[r0] 
	@vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	@vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	
	vmul.s16 d18, d18, d20
	vadd.s16 d14, d14, d30	
	vadd.s16 d28, d14, d18
	
	@vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	@vrhadd.u8 d25, d25, d29
	vqshrun.s16	d22, q11, #5	
	vqshrun.s16	d23, q12, #5
	vqshrun.s16	d24, q13, #5
	vqshrun.s16	d25, q14, #5	

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1]	 
        bne     GetDXP1_DYP0_B4                           @

        b_EXIT_FUNC
                   
GetDXP1_DYP0_16:
	mov	r0, r2
GetDXP1_DYP0_B16:
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0], r8			@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #4	
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]															
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]	
	vadd.s16 q11, q7, q9
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]
	@vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	@vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d1, d5, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d1, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]		
	vadd.s16 q12, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	@vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d2, d12				@m2+p3	
	@vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d2, d6, #2				@p0    ref0[0~7]
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	@vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	@vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d3, d7, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d3, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q14, q7, q9
	
	@vqshrun.s16	d25, q13, #5	
	@vrhadd.u8 d25, d25, d29
	vqshrun.s16	d22, q11, #5	
	vqshrun.s16	d23, q12, #5
	vqshrun.s16	d24, q13, #5
	vqshrun.s16	d25, q14, #5	

	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1], r4			@tmpResJ[7]
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0]				@ref0[5]	
	
	mov	r0, r2	
										@m2 d0 ref0[-2~5]
	vext.u8 d29, d4, d0, #2				@p0    ref0[0~7]
	vext.u8 d10, d4, d0, #3				@p1    ref0[1~8]															
	vext.u8 d8, d4, d0, #1				@m1    ref0[-1~6]
	vext.u8 d11, d4, d0, #4				@p2    ref0[2~9]
	vext.u8 d12, d4, d0, #5				@p3    ref0[3~10]

	vaddl.u8 q9, d29, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d4, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d10, d5, d1, #3				@p1    ref0[1~8]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d8, d5, d1, #1				@m1    ref0[-1~6]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vext.u8 d11, d5, d1, #4				@p2    ref0[2~9]	
	vadd.s16 q11, q7, q9
	vext.u8 d12, d5, d1, #5				@p3    ref0[3~10]
	@vqshrun.s16	d22, q13, #5
	vaddl.u8 q8, d8, d11				@m1+p2			
	@vrhadd.u8 d22, d22, d29								@m2 d1 ref0[-2~5]	
	vext.u8 d29, d5, d1, #2				@p0    ref0[0~7]
	vaddl.u8 q7, d5, d12				@m2+p3
	vaddl.u8 q9, d29, d10				@p0+p1		
	vext.u8 d10, d6, d2, #3				@p1    ref0[1~8]	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d6, d2, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d11, d6, d2, #4				@p2    ref0[2~9]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vext.u8 d12, d6, d2, #5				@p3    ref0[3~10]		
	vadd.s16 q12, q7, q9
	vaddl.u8 q8, d8, d11				@m1+p2						
	@vqshrun.s16	d23, q13, #5
	vaddl.u8 q7, d6, d12				@m2+p3	
	@vrhadd.u8 d23, d23, d29									@m2 d2 ref0[-2~5]
	vext.u8 d29, d6, d2, #2				@p0    ref0[0~7]
	vext.u8 d8, d7, d3, #1				@m1    ref0[-1~6]
	vaddl.u8 q9, d29, d10				@p0+p1	
	vext.u8 d10, d7, d3, #3				@p1    ref0[1~8]
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d11, d7, d3, #4				@p2    ref0[2~9]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d12, d7, d3, #5				@p3    ref0[3~10]	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15
	vaddl.u8 q8, d8, d11				@m1+p2			
	vadd.s16 q13, q7, q9
	@pld	[r0] 
	@vqshrun.s16	d24, q13, #5
	@pld	[r0, r8] 	
	@vrhadd.u8 d24, d24, d29										@m2 d3 ref0[-2~5]
	vext.u8 d29, d7, d3, #2				@p0    ref0[0~7]

	vaddl.u8 q9, d29, d10				@p0+p1				
	vaddl.u8 q7, d7, d12				@m2+p3
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	@pld	[r0, r8, lsl #1]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	
	vmul.s16 q9, q9, q10
	vadd.s16 q7, q7, q15	
	vadd.s16 q14, q7, q9
	
	@vqshrun.s16	d25, q13, #5
        subs    r6, r6, #4                        @ 	
	@vrhadd.u8 d25, d25, d29
	vqshrun.s16	d22, q11, #5	
	vqshrun.s16	d23, q12, #5
	vqshrun.s16	d24, q13, #5
	vqshrun.s16	d25, q14, #5	
		
	vst4.8 {d22[0], d23[0], d24[0], d25[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.8 {d22[1], d23[1], d24[1], d25[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.8 {d22[2], d23[2], d24[2], d25[2]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.8 {d22[3], d23[3], d24[3], d25[3]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.8 {d22[4], d23[4], d24[4], d25[4]}, [r1], r4			@tmpResJ[4]
	vst4.8 {d22[5], d23[5], d24[5], d25[5]}, [r1], r4			@tmpResJ[5]
	vst4.8 {d22[6], d23[6], d24[6], d25[6]}, [r1], r4			@tmpResJ[6]
	vst4.8 {d22[7], d23[7], d24[7], d25[7]}, [r1]				@tmpResJ[7]
	
        bne     GetDXP1_DYP0_B16                           @ 
        b_EXIT_FUNC	     	
	     	     	
	.endm
	
	
	.macro GetDXP3_DYP3
GetDXP3_DYP3_v7:
@        GetDXP3_DYP3 r0,r8,r1,r2,r6,r5,r10
@	GetDXP1_DYP0 r0,		r8,		r1,		r2,		r6,			r5,			r10
@	avdNativeInt temp0,temp1,temp2,temp3@
@	avdUInt8 *outStart_t = outStart@		
@	refTmp = srcY + (dyP == 1 ? 0 : width)@	
@	if (dxP != 1)
@		srcY++@
@	j = blockSizeY@
@	do {
@		i = quarterSizeX@
@		do {
@			temp0 = clp255[(AVD_6TAP(srcY[-(width<<1)], srcY[-width],
@					srcY[0], srcY[width], srcY[width<<1],
@					srcY[3*width])+16)>>5]@
@			temp1 = clp255[(AVD_6TAP(srcY[-(width<<1) + 1], srcY[-width + 1],
@					srcY[0 + 1], srcY[width + 1], srcY[(width<<1) + 1],
@					srcY[3*width + 1])+16)>>5]@
@			temp2 = clp255[(AVD_6TAP(srcY[-(width<<1) + 2], srcY[-width + 2],
@					srcY[0 + 2], srcY[width + 2], srcY[(width<<1) + 2],
@					srcY[3*width + 2])+16)>>5]@
@			temp3 = clp255[(AVD_6TAP(srcY[-(width<<1) + 3], srcY[-width + 3],
@					srcY[0 + 3], srcY[width + 3], srcY[(width<<1) + 3],
@					srcY[3*width + 3])+16)>>5]@
@			AVD_SAVE_4CHARS(outStart, temp0, temp1, temp2, temp3)@
@			srcY += 4@
@		} while (--i)@
@	outStart += width1@
@	srcY += width2@
@	} while (--j)@	
	mov	r4, r8
	mov	r7, r2	
	
	
	vmov.s16 q10, #5
	vmov.s16 q11, #16	
	sub	r0, r0, r4, lsl #1
	
	cmp     r6,#8

	bgt GetDXP3_DYP3_16
	vld1.8 {d0}, [r0], r4				@m2 srcY[-(width<<1)][0~7]
	vld1.8 {d2}, [r0], r4				@m1 srcY[-width     ][0~7]
	vld1.8 {d4}, [r0], r4				@p0 srcY[0          ][0~7]
	vld1.8 {d6}, [r0], r4				@p1 srcY[width      ][0~7]
	vld1.8 {d8}, [r0], r4				@p2 srcY[width<<1   ][0~7]
	vld1.8 {d10}, [r0], r4				@p3 srcY[3*width    ][0~7]		
    blt GetDXP3_DYP3_4	
			
GetDXP3_DYP3_B8:
	vld1.8 {d11}, [r0], r4				@p4 srcY[4*width    ][0~7]
	vaddl.u8 q8, d4, d6				@p0+p1	
	vaddl.u8 q7, d2, d8				@m1+p2		
	vaddl.u8 q6, d0, d10				@m2+p3						
	@pld	[r0, r4, lsl #1]	
	vqshl.u16 q8, q8, #2
	vshr.s32 d0, d2, #0	
	vsub.s16 q8, q8, q7
	vshr.s32 d2, d4, #0	
	vmul.s16 q8, q8, q10
	vadd.u16 q6, q6, q11
	vshr.s32 d4, d6, #0
	vshr.s32 d6, d8, #0		
	vadd.s16 q8, q8, q6
	vshr.s32 d8, d10, #0							
	vqshrun.s16	d16, q8, #5				
	vshr.s32 d10, d11, #0
        subs    r10, r10, #1                        @  	
	vst1.32	{d16}, [r1], r7        
@        add     r1, r1, r7                        @         
        bne     GetDXP3_DYP3_B8                           @       
        b     out_func_GetDXP3_DYP3
  
GetDXP3_DYP3_4:

GetDXP3_DYP3_B4:
	vld1.8 {d11}, [r0], r4				@p4 srcY[4*width    ][0~7]
	vaddl.u8 q8, d4, d6				@p0+p1	
	vaddl.u8 q7, d2, d8				@m1+p2		
	vaddl.u8 q6, d0, d10				@m2+p3	
	@pld	[r0, r4, lsl #1]	
	vqshl.u16 d16, d16, #2
	vshr.s32 d0, d2, #0	
	vsub.s16 d16, d16, d14
	vshr.s32 d2, d4, #0	
	vmul.s16 d16, d16, d20
	vadd.u16 d12, d12, d22
	vshr.s32 d4, d6, #0
	vshr.s32 d6, d8, #0		
	vadd.s16 d16, d16, d12
	vshr.s32 d8, d10, #0							
	vqshrun.s16 d16, q8, #5
        subs    r10, r10, #1                        @  	
	vshr.s32 d10, d11, #0
	vst1.32	{d16[0]}, [r1], r7    						
@        add     r1, r1, r7                        @         
        bne     GetDXP3_DYP3_B4                           @

        b     out_func_GetDXP3_DYP3

GetDXP3_DYP3_16:
	vld1.8 {q0}, [r0], r4				@m2 srcY[-(width<<1)][0~7]
	vld1.8 {q1}, [r0], r4				@m1 srcY[-width     ][0~7]
	vld1.8 {q2}, [r0], r4				@p0 srcY[0          ][0~7]
	vld1.8 {q3}, [r0], r4				@p1 srcY[width      ][0~7]
	vld1.8 {q4}, [r0], r4				@p2 srcY[width<<1   ][0~7]
	vld1.8 {q5}, [r0], r4				@p3 srcY[3*width    ][0~7]
GetDXP3_DYP3_B16:
	vld1.8 {q9}, [r0], r4				@p4 srcY[4*width    ][0~7]
	vaddl.u8 q8, d4, d6				@p0+p1
	@pld	[r0, r4, lsl #1]		
	vaddl.u8 q7, d2, d8				@m1+p2		
	vaddl.u8 q6, d0, d10				@m2+p3	
	vaddl.u8 q15, d1, d11				@m2+p3	
	vqshl.u16 q8, q8, #2
	vaddl.u8 q13, d3, d9				@m1+p2	
	vsub.s16 q8, q8, q7
	vaddl.u8 q14, d5, d7				@p0+p1	
	vmul.s16 q8, q8, q10
	vadd.u16 q6, q6, q11
	vqshl.u16 q14, q14, #2	
	vadd.s16 q8, q8, q6	
	vsub.s16 q14, q14, q13						
	vqshrun.s16	d24, q8, #5
	vmul.s16 q14, q14, q10
	vshr.s32 q0, q1, #0
	vshr.s32 q1, q2, #0
	vshr.s32 q2, q3, #0	
	vadd.u16 q15, q15, q11
	vshr.s32 q3, q4, #0	
	vadd.s16 q14, q14, q15
	vshr.s32 q4, q5, #0
	vshr.s32 q5, q9, #0								
	vqshrun.s16	d25, q14, #5	
        subs    r10, r10, #1                        @
	vst1.64	{q12}, [r1], r7  
@        add     r1, r1, r7                        @         
        bne     GetDXP3_DYP3_B16                           @       
out_func_GetDXP3_DYP3:		
	.endm		
	
	
	.macro GetDYP3_DXP3
GetDYP3_DXP3_v7:
@       GetDYP3_DXP3  r0,	r8,	r1,	r2,	r6,		r5,	r10
@	GetDYP1_DXP0  r0,   r4,       r1,   r7,         r6,         r5,      r10
	mov	r4, r8
	mov	r7, r2	
@	j = blockSizeY@
@	do {
@		i = quarterSizeX@
@		do {
@			tmpa =  clp255[(AVD_6TAP(refTmp[-2], refTmp[-1], refTmp[0], 
@					refTmp[1], refTmp[2], refTmp[3])+16)>>5]@
@			tmpb =  clp255[(AVD_6TAP(refTmp[-1], refTmp[0], refTmp[1], 
@					refTmp[2], refTmp[3], refTmp[4])+16)>>5]@
@			tmpc =  clp255[(AVD_6TAP(refTmp[0], refTmp[1], refTmp[2], 
@					refTmp[3], refTmp[4], refTmp[5])+16)>>5]@
@			tmpd =  clp255[(AVD_6TAP(refTmp[1], refTmp[2], refTmp[3], 
@					refTmp[4], refTmp[5], refTmp[6])+16)>>5]@
@			temp0 =  outStart[0]@ 
@			temp1 =  outStart[1]@ 
@			temp2 =  outStart[2]@ 
@			temp3 =  outStart[3]@ 
@			tmpa = (tmpa +temp0 + 1)>>1@
@			tmpb = (tmpb +temp1 + 1)>>1@
@			tmpc = (tmpc +temp2 + 1)>>1@
@			tmpd = (tmpd +temp3 + 1)>>1@
@			AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd)@
@			refTmp += 4@
@		} while (--i)@
@	outStart += width1@
@	refTmp += width2@
@	} while (--j)@

	vmov.s16 q10, #5
	vmov.s16 q11, #16	
	sub	r0, r0, #2
@        add     r7, r7, #8	
	cmp     r6,#8
        add     r7, r7, r6                        @	
    blt GetDYP3_DXP3_4	
	bgt GetDYP3_DXP3_16			
        add     r4, r4, r6                        @	
GetDYP3_DXP3_B8:	
@	vld1.8 {d0}, [r0]!				@ref0[-2~5]
@	vld1.8 {d12}, [r0], r4				@ref0[6~13]
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	vld1.64 {d17}, [r1]				@src0[0~7]
	@pld	[r0, r4]		
							@m2 d0 ref0[-2~5]					
	vext.u8 d4, d0, d1, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]	
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
							
	vaddl.u8 q2, d4, d6				@p0+p1	
	vaddl.u8 q1, d2, d8				@m1+p2	
	vqshl.u16 q2, q2, #2
	vaddl.u8 q0, d0, d10				@m2+p3	
	vsub.s16 q2, q2, q1
	vadd.u16 q0, q0, q11	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @	
	vadd.s16 q2, q2, q0
							
	vqshrun.s16	d16, q2, #5
	
	vrhadd.u8 d16, d16, d17
		
	vst1.64	{d16}, [r1], r7   
@        add     r1, r1, r7                        @             
        bne     GetDYP3_DXP3_B8                           @ 
        b_EXIT_FUNC

GetDYP3_DXP3_4:
        add     r4, r4, r6                        @  
GetDYP3_DXP3_B4:
@	vld1.8 {d0}, [r0]!			@ref0[-2~5]
@	vld1.8 {d12[0]}, [r0],r4		@ref0[6~13]
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	vld1.32 {d17[0]}, [r1]				@src0[0~7]	
	@pld	[r0, r4]	
							@m2 d0 ref0[-2~5]					
	vext.u8 d4, d0, d1, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]	
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
	
	vaddl.u8 q2, d4, d6				@p0+p1	
	vaddl.u8 q1, d2, d8				@m1+p2		
	vqshl.u16 d4, d4, #2
	vaddl.u8 q0, d0, d10				@m2+p3	
	vsub.s16 d4, d4, d2
	vadd.u16 d0, d0, d22	
	vmul.s16 d4, d4, d20
        subs    r10, r10, #1                        @ 	
	vadd.s16 d4, d4, d0
							
	vqshrun.s16 d16, q2, #5
	
	vrhadd.u8 d16, d16, d17
		
	vst1.32	{d16[0]}, [r1], r7			       
@        add     r1, r1, r7                        @           
        bne     GetDYP3_DXP3_B4                           @ 
        b_EXIT_FUNC
                   
GetDYP3_DXP3_16:

GetDYP3_DXP3_B16:	
	vld1.8 {d0}, [r0]!				@ref0[-2~5]
	vld1.8 {d12}, [r0]!				@ref0[6~13]
	vld1.8 {d14}, [r0], r4				@ref0[14~21]
	vld1.64	{q9}, [r1]	
	@pld	[r0]		
@one								@m2 d0 ref0[-2~5]					
	vext.u8 d4, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d5, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]	
	vext.u8 d3, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d1, d0, d12, #5			@p3    ref0[3~10]
	
	vext.u8 d24, d12, d14, #2			@p0    ref0[0~7]
	vext.u8 d25, d12, d14, #3			@p1    ref0[1~8]
	vext.u8 d26, d12, d14, #1			@m1    ref0[-1~6]	
	vext.u8 d27, d12, d14, #4			@p2    ref0[2~9]
	vext.u8 d28, d12, d14, #5			@p3    ref0[3~10]
		
	vaddl.u8 q2, d4, d5				@p0+p1	
	vaddl.u8 q1, d2, d3				@m1+p2	
	vaddl.u8 q0, d0, d1				@m2+p3	
	
	vaddl.u8 q5, d24, d25				@p0+p1	
	vaddl.u8 q4, d26, d27				@m1+p2								
	vaddl.u8 q3, d12, d28				@m2+p3
		
	vqshl.u16 q2, q2, #2
	vqshl.u16 q5, q5, #2	
	vsub.s16 q2, q2, q1
	vsub.s16 q5, q5, q4	
	vmul.s16 q2, q2, q10
	vadd.u16 q0, q11, q0
	vmul.s16 q5, q5, q10
	vadd.u16 q3, q11, q3		
	vadd.s16 q2, q2, q0
	vadd.s16 q5, q5, q3							
	vqshrun.s16	d16, q2, #5
	vqshrun.s16	d17, q5, #5	
@tow									@m2 d12 ref0[-2~5]
        subs    r10, r10, #1                        @ 
	vrhadd.u8 q8, q8, q9		
	vst1.64	{q8}, [r1], r7	
@        add     r1, r1, r7                        @           
        bne     GetDYP3_DXP3_B16                           @ 
        b_EXIT_FUNC	
	.endm
	
	.macro GetDXP2_J
GetDXP2_J_v7:
@	GetDXP2_J r0,	r4,			r1,		r6,			r10	
@	avdNativeInt *tmpM7 = img->m7[0]@
@	avdNativeInt quarterSizeY = (blockSizeY>>2)@
@	refTmp = srcY - (width<<1)@
@	j = blockSizeY + 5@
@	do {
@		tmpResJ = tmpM7@
@		i = quarterSizeX@
@		do {
@			tmpa =  AVD_6TAP(refTmp[-2], refTmp[-1], refTmp[0], 
@					refTmp[1], refTmp[2], refTmp[3])@
@			tmpb =  AVD_6TAP(refTmp[-1], refTmp[0], refTmp[1], 
@					refTmp[2], refTmp[3], refTmp[4])@
@			tmpc =  AVD_6TAP(refTmp[0], refTmp[1], refTmp[2], 
@					refTmp[3], refTmp[4], refTmp[5])@
@			tmpd =  AVD_6TAP(refTmp[1], refTmp[2], refTmp[3], 
@					refTmp[4], refTmp[5], refTmp[6])@
@	
@			*tmpResJ++ = tmpa@
@			*tmpResJ++ = tmpb@
@			*tmpResJ++ = tmpc@
@			*tmpResJ++ = tmpd@
@			refTmp += 4@					
@		} while (--i)@
@		refTmp += width2@
@		tmpM7  += M7_WIDTH@ 
@	} while (--j)@
    add    r10, r10, #5
	vmov.s16 q10, #5
	sub	r0, r0, #2	
	cmp     r6,#8
    blt GetDXP2_J_4	
	bgt GetDXP2_J_16			
	
	mov	r7, #64						@M7_WIDTH = #96	 - #32
        add     r4, r4, r6                        @	
GetDXP2_J_B8:
@	vld1.8 {d0}, [r0]!				@ref0[-2~5]
@	vld1.8 {d12}, [r0], r4				@ref0[6~13]
@	@pld	[r0]
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@pld	[r0, r4]
			
							@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]													
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d1, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d4, d6				@p0+p1
	
	vqshl.u16 q2, q2, #2
	
	vsub.s16 q2, q2, q1
	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @	
	vadd.s16 q2, q2, q0
							
	vmovl.s16 q0, d4
	vmovl.s16 q1, d5	
	vst1.32	{q0}, [r1]!
	vst1.32	{q1}, [r1]!	
        
        add     r1, r1, r7                        @           
  
        bne     GetDXP2_J_B8                           @ 
        b	GetDXP2_Jcontine_FUNC
  
GetDXP2_J_4:
        add     r4, r4, r6                        @
	mov	r7, #96						@M7_WIDTH = #96	 - #16       
GetDXP2_J_B4:
@	vld1.8 {d0}, [r0]!			@ref0[-2~5]
@	vld1.8 {d12[0]}, [r0],r4		@ref0[6~13]
@	@pld	[r0]
	vld1.8 {q0}, [r0], r4				@ref0[-2~5]
	@pld	[r0, r4]
								@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d1, #5			@p3    ref0[3~10]
	vext.u8 d8, d0, d1, #4				@p2    ref0[2~9]													
	vext.u8 d2, d0, d1, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d1, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d1, #3				@p1    ref0[1~8]	
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d4, d6				@p0+p1
	
	vqshl.u16 d4, d4, #2
	
	vsub.s16 d4, d4, d2
	
	vmul.s16 d4, d4, d20
        subs    r10, r10, #1                        @ 	
	vadd.s16 d4, d4, d0
	
	vmovl.s16 q0, d4
								
	vst1.32	{q0}, [r1], r7		
         
@        add     r1, r1, r7                        @           
 
        bne     GetDXP2_J_B4                           @ 
        b	GetDXP2_Jcontine_FUNC
                   
GetDXP2_J_16:
	mov	r7, #32						@M7_WIDTH = #96	 - #64
GetDXP2_J_B16:
	vld1.8 {d0}, [r0]!				@ref0[-2~5]
	vld1.8 {d12}, [r0]!				@ref0[6~13]
	vld1.8 {d14}, [r0], r4				@ref0[14~21]	
@one	
	@pld	[r0]
							@m2 d0 ref0[-2~5]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]													
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	
	vext.u8 d27, d12, d14, #5			@p3    ref0[3~10]
	vext.u8 d28, d12, d14, #4			@p2    ref0[2~9]					
	vext.u8 d22, d12, d14, #1			@m1    ref0[-1~6]
	vext.u8 d24, d12, d14, #2			@p0    ref0[0~7]
	vext.u8 d26, d12, d14, #3			@p1    ref0[1~8]		
	
	vaddl.u8 q0, d0, d10				@m2+p3
	vaddl.u8 q1, d2, d8				@m1+p2							
	vaddl.u8 q2, d4, d6				@p0+p1
	
	vaddl.u8 q4, d12, d27				@m2+p3
	vaddl.u8 q5, d22, d28				@m1+p2							
	vaddl.u8 q6, d24, d26				@p0+p1	
	
	vqshl.u16 q2, q2, #2
	vqshl.u16 q6, q6, #2	
	vsub.s16 q2, q2, q1
	vsub.s16 q6, q6, q5	
	vmul.s16 q2, q2, q10
        subs    r10, r10, #1                        @	
	vmul.s16 q6, q6, q10	
	vadd.s16 q8, q2, q0
	vmovl.s16 q0, d16		
	vadd.s16 q9, q6, q4	
@tow									@m2 d12 ref0[-2~5]
	vmovl.s16 q1, d17
	vmovl.s16 q2, d18
	vmovl.s16 q3, d19		
	vst1.32	{q0}, [r1]!
	vst1.32	{q1}, [r1]!	
	vst1.32	{q2}, [r1]!
	vst1.32	{q3}, [r1]!			         
        add     r1, r1, r7                        @           
  
        bne     GetDXP2_J_B16                           @ 
GetDXP2_Jcontine_FUNC:	
	@b	EXIT_FUNC
	.endm
	
	.macro GetDXP2_DYP
OutLoopCount_6			.req  r8
InnerLoopCount_6		.req  r8
src0_6				.req  r7
src1_6				.req  r8
src2_6				.req  r2
src3_6				.req  r3
src4_6				.req  r4
src5_6				.req  r5
src6_6				.req  r6
temp_6				.req  r7
temp2_6				.req  r9
temp3_6				.req  r12
clip255_6			.req  r14
result_6			.req  r9
result1_6			.req  r10
dyp_6				.req  r12
destStride_6		.req  r11
	@LCLA	srcStride_t
	@LCLA	srcStride_t4
	mov	dyp_6,$4
	str     $6,[sp,#44]		@innerCount_6
	str     $6 ,[sp,#48]	@innerCount_6
	str     $3 ,[sp,#52]	@OutLoopCount_6
.set    srcStride_t,		96
.set	srcStride_t4,		384
	mov     destStride_6,$2
	sub     $1,$1,destStride_6

	ldr     InnerLoopCount_6,[sp,#44]
	mov     clip255_6,$5
	@ldr     dyp_6,[sp,#56]
	@mov		temp3_6,#-5
	@mov		temp_6,#20
	@orr		temp3_6,temp_6,temp3_6,LSL #16

	@add     temp_6,srcStride_6,srcStride_6,lsl #1
	
	str     $0,  [sp,#36]
	str     $1, [sp,#40]
	cmp     dyp_6,#2
	beq     DXP2_DYP2
DXP2_DYP1_OUT: 
	@ldr    InnerLoopCount_6,[sp,#48] 
	str     InnerLoopCount_6,[sp,#44]
DXP2_DYP1_INNER: 
	ldr	src0_6, [$0]
	ldr	src1_6, [$0,#srcStride_t]!
	ldr	src2_6, [$0,#srcStride_t]!             @  76.49
	ldr	src3_6, [$0,#srcStride_t]!             @  76.62
	ldr	src4_6, [$0,#srcStride_t]!             @  76.62
	ldr	src5_6, [$0,#srcStride_t]!             @  76.49
	cmp		dyp_6,#1
	add		temp2_6, src1_6, src4_6
	add		temp_6, src0_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
    	add		temp2_6, src2_6,src3_6	
	ldr 	src6_6, [$0,#srcStride_t]!
    	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	moveq	result1_6,src2_6
	movne	result1_6,src3_6
	usat	result_6, #8, temp_6, asr #10
			
	add     result1_6,result1_6,#16
@	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
		usat	result1_6, #8, result1_6, asr #5

@	add     result_6,result_6,#1
	add		temp_6,src6_6,src1_6
	add		result_6,result_6,result1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[$1,destStride_6]!
	add		temp2_6,src2_6,src5_6
	add		src1_6, src3_6,src4_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
@	add		temp2_6, src3_6,src4_6
	add		temp2_6,src1_6,src1_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	    src1_6,[$0,#srcStride_t]!
@	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall*/
		usat	result_6, #8, temp_6, asr #10

	@@pld	    [\src,#srcStride_t]
	moveq	result1_6,src3_6
	movne	result1_6,src4_6
	add     result1_6,result1_6,#16
@	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
		usat	result1_6, #8, result1_6, asr #5

@	add     result_6,result_6,#1
	add		src2_6,src1_6,src2_6
	add		result_6,result_6,result1_6
	add		temp_6,src3_6,src6_6
	mov     result_6,result_6,asr #1
	strb	result_6,[$1,destStride_6]!
@	add		temp2_6,src3_6,src6_6
	add		temp2_6,temp_6,temp_6,lsl #2		@5*(-1+2)
	sub		temp_6,src2_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src4_6,src5_6
	ldr	    src2_6,[$0,#srcStride_t]!
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	    src2_6,[$0,#srcStride_t]!
@	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall
	moveq	result1_6,src4_6
	movne	result1_6,src5_6
		usat	result_6, #8, temp_6, asr #10

	@@pld	[$0,#srcStride_t]
@	moveq	result1_6,src4_6
@	movne	result1_6,src5_6
	add     result1_6,result1_6,#16
@	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
		usat	result1_6, #8, result1_6, asr #5

@	add     result_6,result_6,#1
	add		src3_6,src2_6,src3_6
	add		result_6,result_6,result1_6
	add		temp_6,src4_6,src1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[$1,destStride_6]!
@	add		temp2_6,src4_6,src1_6
	add		temp2_6,temp_6,temp_6,lsl #2		@5*(-1+2)
	sub		temp_6,src3_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src5_6,src6_6
	ldr	src3_6,[$0,#srcStride_t]
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	src3_6,[$0,#srcStride_t]
@	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall
	moveq	result1_6,src5_6
	movne	result1_6,src6_6
		usat	result_6, #8, temp_6, asr #10

@	moveq	result1_6,src5_6
@	movne	result1_6,src6_6
	add     result1_6,result1_6,#16
@	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
		usat	result1_6, #8, result1_6, asr #5

@	add     result_6,result_6,#1
	sub     $0, $0,  #srcStride_t4
	ldr    InnerLoopCount_6,[sp,#44]
	add		result_6,result_6,result1_6
	subs   InnerLoopCount_6,InnerLoopCount_6,#4
	mov     result_6,result_6,asr #1
	strb	result_6,[$1,destStride_6]!
@	subs   InnerLoopCount_6,InnerLoopCount_6,#4
	str    InnerLoopCount_6,[sp,#44] 
	@ldr    dyp_6,[sp,#56]
	bne	   DXP2_DYP1_INNER
	
	
	
            
 @@@@@@out loop control
	ldr    OutLoopCount_6 ,[sp,#52]
	ldr     $0,  [sp,#36]
	ldr     $1, [sp,#40]
	subs   OutLoopCount_6,OutLoopCount_6,#1
	str    OutLoopCount_6 ,[sp,#52]
	ldr    InnerLoopCount_6,[sp,#48]
	@rsb    temp_6,OutLoopCount_6,#16
	addne   $0,$0,#4
	addne	$1,$1,#1
	str     $0,  [sp,#36]
	str     $1, [sp,#40]
	bne    DXP2_DYP1_OUT	
	b_EXIT_FUNC

DXP2_DYP2: 
DXP2_DYP2_OUT: 
	@ldr    InnerLoopCount_6,[sp,#48] 
	str     InnerLoopCount_6,[sp,#44]
DXP2_DYP2_INNER: 
	
	ldr		src0_6, [$0]
	ldr		src1_6, [$0,#srcStride_t]!
	ldr		src2_6, [$0,#srcStride_t]!             @  76.49
	ldr		src3_6, [$0,#srcStride_t]!             @  76.62
	
	ldr		src4_6, [$0,#srcStride_t]!             @  76.62
	ldr		src5_6, [$0,#srcStride_t]!             @  76.49

	
	add		temp2_6, src1_6, src4_6
	add		temp_6, src0_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
	
	add		temp2_6, src2_6,src3_6					@
	ldr 		src6_6, [$0,#srcStride_t]!
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr 		src6_6, [$0,#srcStride_t]!
@	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		result_6,[clip255_6,+temp_6,ASR #10]	
	add		src1_6,src6_6,src1_6
		usat	result_6, #8, temp_6, asr #10
			
	@@pld		[$0,#srcStride_t]
@	add		temp_6,src6_6,src1_6
	add		temp_6,src2_6,src5_6	
	strb		result_6,[$1,destStride_6]!
@	add		temp2_6,src2_6,src5_6
	add		temp2_6,temp_6,temp_6,lsl #2		@5*(-1+2)
	sub		temp_6,src1_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
	add		temp2_6, src3_6,src4_6
	ldr	    	src1_6,[$0,#srcStride_t]!
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	    	src1_6,[$0,#srcStride_t]!
@	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall*/
	add		src2_6,src1_6,src2_6
		usat	result_6, #8, temp_6, asr #10
			
	@@pld	    	[$0,#srcStride_t]
@	add		temp_6,src1_6,src2_6
	add		temp_6,src3_6,src6_6	
	strb		result_6,[$1,destStride_6]!
@	add		temp2_6,src3_6,src6_6
	add		temp2_6,temp_6,temp_6,lsl #2		@5*(-1+2)
	sub		temp_6,src2_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src4_6,src5_6
	ldr	        src2_6,[$0,#srcStride_t]!
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	        src2_6,[$0,#srcStride_t]!
@	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall
	add		src3_6,src2_6,src3_6
		usat	result_6, #8, temp_6, asr #10
	add		temp_6,src4_6,src1_6	
@	@pld		[$0,#srcStride_t]
@	add		temp_6,src2_6,src3_6
	strb		result_6,[$1,destStride_6]!
@	add		temp2_6,src4_6,src1_6
	add		temp2_6,temp_6,temp_6,lsl #2		@5*(-1+2)
	sub		temp_6,src3_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src5_6,src6_6
	ldr		src3_6,[$0,#srcStride_t]
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr		src3_6,[$0,#srcStride_t]
@	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall
	sub     	$0, $0,  #srcStride_t4
		usat	result_6, #8, temp_6, asr #10	
	
@	sub     	$0, $0,  #srcStride_t4
	ldr    		InnerLoopCount_6,[sp,#44]
	strb		result_6,[$1,destStride_6]!
	
	
	subs   		InnerLoopCount_6,InnerLoopCount_6,#4
	str    		InnerLoopCount_6,[sp,#44] 

	bne	   	DXP2_DYP2_INNER
	        
 @@@@@@out loop control
	ldr    OutLoopCount_6 ,[sp,#52]
	ldr     $0,  [sp,#36]
	ldr     $1, [sp,#40]
	subs   OutLoopCount_6,OutLoopCount_6,#1
	str    OutLoopCount_6 ,[sp,#52]
	ldr    InnerLoopCount_6,[sp,#48]
	@rsb    temp_6,OutLoopCount_6,#16
	addne   $0,$0,#4
	addne	$1,$1,#1
	str     $0,  [sp,#36]
	str     $1, [sp,#40]
	bne    DXP2_DYP2_OUT	
	b_EXIT_FUNC	
		
	.endm
	
	.macro GetDYP2_J
  add    r6, r6, #4
	vmov.s16 q10, #5
@	sub	r0, r0, r8, lsl #1	
	sub	r2, r0, #2
	mov	r3, r1
	mov	r4, #96				
	cmp     r10,#8
    blt GetDYP2_J_4	
	bgt GetDYP2_J_16			
GetDYP2_J_B8: 	
	mov	r0, r2		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #16	
	mov	r0, r2	
										@m2 d0 ref0[-2~5]					
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d9, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]
	
	vaddl.u8 q9, d9, d10				@p0+p1		
	vaddl.u8 q8, d8, d11				@m1+p2							
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d1, d5, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]		
	vadd.s16 q11, q7, q9
	
										@m2 d1 ref0[-2~5]

	vaddl.u8 q9, d9, d10				@p0+p1		
	vaddl.u8 q8, d8, d11				@m1+p2							
	vaddl.u8 q7, d1, d12				@m2+p3

	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d2, d6, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]	
	vadd.s16 q12, q7, q9					

										@m2 d2 ref0[-2~5]
	
	vaddl.u8 q9, d9, d10				@p0+p1		
	vaddl.u8 q8, d8, d11				@m1+p2							
	vaddl.u8 q7, d2, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d3, d7, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vadd.s16 q13, q7, q9

										@m2 d3 ref0[-2~5]					

	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d3, d12				@m2+p3
	vmovl.s16 q0, d22	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vmovl.s16 q4, d23	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vmovl.s16 q1, d24	
	vmul.s16 q9, q9, q10
	vmovl.s16 q5, d25
	vmovl.s16 q2, d26		
	vadd.s16 q14, q7, q9
	vmovl.s16 q6, d27	
	vmovl.s16 q3, d28
	vmovl.s16 q7, d29	
        subs    r6, r6, #4                        @	 
	vst4.32 {d0[0], d2[0], d4[0], d6[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.32 {d0[1], d2[1], d4[1], d6[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.32 {d1[0], d3[0], d5[0], d7[0]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.32 {d1[1], d3[1], d5[1], d7[1]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.32 {d8[0], d10[0], d12[0], d14[0]}, [r1], r4			@tmpResJ[4]
	vst4.32 {d8[1], d10[1], d12[1], d14[1]}, [r1], r4			@tmpResJ[5]
	vst4.32 {d9[0], d11[0], d13[0], d15[0]}, [r1], r4			@tmpResJ[6]
	vst4.32 {d9[1], d11[1], d13[1], d15[1]}, [r1]			@tmpResJ[7]	 
        bne     GetDYP2_J_B8                           @ 
        
	mov	r0, r2		
	vld1.8 {d0[0]}, [r0], r8			@ref0[-2]
	vld1.8 {d0[1]}, [r0], r8			@ref0[-1]
	vld1.8 {d0[2]}, [r0], r8			@ref0[0]
	vld1.8 {d0[3]}, [r0], r8			@ref0[1]
	vld1.8 {d0[4]}, [r0], r8			@ref0[2]
	vld1.8 {d0[5]}, [r0], r8			@ref0[3]
	vld1.8 {d0[6]}, [r0], r8			@ref0[4]
	vld1.8 {d0[7]}, [r0], r8			@ref0[5]
								
	vld1.8 {d12[0]}, [r0], r8			@ref0[6]
	vld1.8 {d12[1]}, [r0], r8			@ref0[7]
	vld1.8 {d12[2]}, [r0], r8			@ref0[8]
	vld1.8 {d12[3]}, [r0], r8			@ref0[9]
	vld1.8 {d12[4]}, [r0], r8			@ref0[10]
	vld1.8 {d12[5]}, [r0], r8			@ref0[11]
	vld1.8 {d12[6]}, [r0], r8			@ref0[12]
	vld1.8 {d12[7]}, [r0]				@ref0[13]
	mov	r1, r3	
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vaddl.u8 q2, d4, d6				@p0+p1	
	vaddl.u8 q1, d2, d8				@m1+p2								
	vaddl.u8 q0, d0, d10				@m2+p3
	
	vqshl.u16 q2, q2, #2
	
	vsub.s16 q2, q2, q1
	
	vmul.s16 q2, q2, q10
	
	vadd.s16 q2, q2, q0
							
	vmovl.s16 q0, d4
	vmovl.s16 q1, d5	
	vst1.32 {d0[0]}, [r1], r4			@tmpResJ[0]
	vst1.32 {d0[1]}, [r1], r4			@tmpResJ[1]
	vst1.32 {d1[0]}, [r1], r4			@tmpResJ[2]
	vst1.32 {d1[1]}, [r1], r4			@tmpResJ[3]
	vst1.32 {d2[0]}, [r1], r4			@tmpResJ[4]
	vst1.32 {d2[1]}, [r1], r4			@tmpResJ[5]
	vst1.32 {d3[0]}, [r1], r4			@tmpResJ[6]
	vst1.32 {d3[1]}, [r1]				@tmpResJ[7]	        
        b	GetDYP2_Jcontine_FUNC
  
GetDYP2_J_4: 
    
GetDYP2_J_B4: 
	mov	r0, r2 		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0]				@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #16		
										@m2 d0 ref0[-2~5]					
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d9, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]
	
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d1, d5, #2				@p0    ref0[0~7]	
	vmul.s16 d18, d18, d20
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]	
	vadd.s16 d22, d14, d18
		
										@m2 d1 ref0[-2~5]			
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2	
	vaddl.u8 q7, d1, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d2, d6, #2				@p0    ref0[0~7]	
	vmul.s16 d18, d18, d20
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]	
	vadd.s16 d24, d14, d18					

										@m2 d2 ref0[-2~5]
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d2, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d3, d7, #2				@p0    ref0[0~7]	
	vmul.s16 d18, d18, d20
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vadd.s16 d26, d14, d18

										@m2 d3 ref0[-2~5]					
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d3, d12				@m2+p3
	
	vqshl.u16 d18, d18, #2				@4(p0+p1)
	vmovl.s16 q0, d22	
	vsub.s16 d18, d18, d16					@4(p0+p1) - (m1+p2)
	vmovl.s16 q1, d24	
	vmul.s16 d18, d18, d20
	vmovl.s16 q2, d26	
	vadd.s16 d28, d14, d18
        subs    r6, r6, #4                        @
	vmovl.s16 q3, d28
	vst4.32 {d0[0], d2[0], d4[0], d6[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.32 {d0[1], d2[1], d4[1], d6[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.32 {d1[0], d3[0], d5[0], d7[0]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.32 {d1[1], d3[1], d5[1], d7[1]}, [r1], r4			@tmpResJ[3]
   
        bne     GetDYP2_J_B4                           @
        
	mov	r0, r2
		
	vld1.8 {d0[0]}, [r0], r8			@ref0[-2]
	vld1.8 {d0[1]}, [r0], r8			@ref0[-1]
	vld1.8 {d0[2]}, [r0], r8			@ref0[0]
	vld1.8 {d0[3]}, [r0], r8			@ref0[1]
	vld1.8 {d0[4]}, [r0], r8			@ref0[2]
	vld1.8 {d0[5]}, [r0], r8			@ref0[3]
	vld1.8 {d0[6]}, [r0], r8			@ref0[4]
	vld1.8 {d0[7]}, [r0], r8			@ref0[5]
								
	vld1.8 {d12[0]}, [r0], r8			@ref0[6]
	vld1.8 {d12[1]}, [r0], r8			@ref0[7]
	vld1.8 {d12[2]}, [r0], r8			@ref0[8]
	vld1.8 {d12[3]}, [r0], r8			@ref0[9]
	vld1.8 {d12[4]}, [r0], r8			@ref0[10]
	vld1.8 {d12[5]}, [r0], r8			@ref0[11]
	vld1.8 {d12[6]}, [r0], r8			@ref0[12]
	vld1.8 {d12[7]}, [r0]				@ref0[13]
	mov	r1, r3	
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vaddl.u8 q2, d4, d6				@p0+p1	
	vaddl.u8 q1, d2, d8				@m1+p2								
	vaddl.u8 q0, d0, d10				@m2+p3
	
	vqshl.u16 d4, d4, #2
	
	vsub.s16 d4, d4, d2
	
	vmul.s16 d4, d4, d20
	
	vadd.s16 d4, d4, d0
						
	vmovl.s16 q0, d4
	vst1.32 {d0[0]}, [r1], r4			@tmpResJ[0]
	vst1.32 {d0[1]}, [r1], r4			@tmpResJ[1]
	vst1.32 {d1[0]}, [r1], r4			@tmpResJ[2]
	vst1.32 {d1[1]}, [r1], r4			@tmpResJ[3]
        b	GetDYP2_Jcontine_FUNC
                   
GetDYP2_J_16: 

GetDYP2_J_B16: 
	mov	r0, r2		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0], r8			@ref0[5]
								
	vld4.8 {d4[0], d5[0], d6[0], d7[0]}, [r0], r8			@ref0[6]
	vld4.8 {d4[1], d5[1], d6[1], d7[1]}, [r0], r8			@ref0[7]
	vld4.8 {d4[2], d5[2], d6[2], d7[2]}, [r0], r8			@ref0[8]
	vld4.8 {d4[3], d5[3], d6[3], d7[3]}, [r0], r8			@ref0[9]
	vld4.8 {d4[4], d5[4], d6[4], d7[4]}, [r0], r8			@ref0[10]
	vld4.8 {d4[5], d5[5], d6[5], d7[5]}, [r0], r8			@ref0[11]
	vld4.8 {d4[6], d5[6], d6[6], d7[6]}, [r0], r8			@ref0[12]
	vld4.8 {d4[7], d5[7], d6[7], d7[7]}, [r0], r8			@ref0[13]
	
	mov	r1, r3
	add	r2, r2, #4	
	add	r3, r3, #16		
										@m2 d0 ref0[-2~5]					
	vext.u8 d8, d0, d4, #1				@m1    ref0[-1~6]
	vext.u8 d9, d0, d4, #2				@p0    ref0[0~7]
	vext.u8 d10, d0, d4, #3				@p1    ref0[1~8]
	vext.u8 d11, d0, d4, #4				@p2    ref0[2~9]
	vext.u8 d12, d0, d4, #5				@p3    ref0[3~10]
	
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d0, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d1, d5, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d1, d5, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d1, d5, #3				@p1    ref0[1~8]
	vext.u8 d11, d1, d5, #4				@p2    ref0[2~9]
	vext.u8 d12, d1, d5, #5				@p3    ref0[3~10]		
	vadd.s16 q11, q7, q9
	
										@m2 d1 ref0[-2~5]					
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d1, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d2, d6, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d2, d6, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d2, d6, #3				@p1    ref0[1~8]
	vext.u8 d11, d2, d6, #4				@p2    ref0[2~9]
	vext.u8 d12, d2, d6, #5				@p3    ref0[3~10]	
	vadd.s16 q12, q7, q9					

										@m2 d2 ref0[-2~5]					

	vaddl.u8 q9, d9, d10				@p0+p1		
	vaddl.u8 q8, d8, d11				@m1+p2							
	vaddl.u8 q7, d2, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d3, d7, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d3, d7, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d3, d7, #3				@p1    ref0[1~8]
	vext.u8 d11, d3, d7, #4				@p2    ref0[2~9]
	vext.u8 d12, d3, d7, #5				@p3    ref0[3~10]	
	vadd.s16 q13, q7, q9

										@m2 d3 ref0[-2~5]					

	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d3, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vmovl.s16 q4, d22	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vmovl.s16 q11, d23	
	vmul.s16 q9, q9, q10
	vmovl.s16 q5, d24
	vmovl.s16 q12, d25
	vmovl.s16 q6, d26			
	vadd.s16 q14, q7, q9
	vmovl.s16 q13, d27	
	vmovl.s16 q7, d28
	vmovl.s16 q14, d29		

	vst4.32 {d8[0], d10[0], d12[0], d14[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.32 {d8[1], d10[1], d12[1], d14[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.32 {d9[0], d11[0], d13[0], d15[0]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.32 {d9[1], d11[1], d13[1], d15[1]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.32 {d22[0], d24[0], d26[0], d28[0]}, [r1], r4			@tmpResJ[4]
	vst4.32 {d22[1], d24[1], d26[1], d28[1]}, [r1], r4			@tmpResJ[5]
	vst4.32 {d23[0], d25[0], d27[0], d29[0]}, [r1], r4			@tmpResJ[6]
	vst4.32 {d23[1], d25[1], d27[1], d29[1]}, [r1], r4		@tmpResJ[7]	
		
	vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0], r8			@ref0[-2]
	vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0], r8			@ref0[-1]
	vld4.8 {d0[2], d1[2], d2[2], d3[2]}, [r0], r8			@ref0[0]
	vld4.8 {d0[3], d1[3], d2[3], d3[3]}, [r0], r8			@ref0[1]
	vld4.8 {d0[4], d1[4], d2[4], d3[4]}, [r0], r8			@ref0[2]
	vld4.8 {d0[5], d1[5], d2[5], d3[5]}, [r0], r8			@ref0[3]
	vld4.8 {d0[6], d1[6], d2[6], d3[6]}, [r0], r8			@ref0[4]
	vld4.8 {d0[7], d1[7], d2[7], d3[7]}, [r0]				@ref0[5]	
	
										@m2 d0 ref0[-2~5]					
	vext.u8 d8, d4, d0, #1				@m1    ref0[-1~6]
	vext.u8 d9, d4, d0, #2				@p0    ref0[0~7]
	vext.u8 d10, d4, d0, #3				@p1    ref0[1~8]
	vext.u8 d11, d4, d0, #4				@p2    ref0[2~9]
	vext.u8 d12, d4, d0, #5				@p3    ref0[3~10]
	
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d4, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d5, d1, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d5, d1, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d5, d1, #3				@p1    ref0[1~8]
	vext.u8 d11, d5, d1, #4				@p2    ref0[2~9]
	vext.u8 d12, d5, d1, #5				@p3    ref0[3~10]	
	vadd.s16 q11, q7, q9
	
										@m2 d1 ref0[-2~5]					
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d5, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d6, d2, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d6, d2, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d6, d2, #3				@p1    ref0[1~8]
	vext.u8 d11, d6, d2, #4				@p2    ref0[2~9]
	vext.u8 d12, d6, d2, #5				@p3    ref0[3~10]	
	vadd.s16 q12, q7, q9					

										@m2 d2 ref0[-2~5]					
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d6, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vext.u8 d8, d7, d3, #1				@m1    ref0[-1~6]	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vext.u8 d9, d7, d3, #2				@p0    ref0[0~7]	
	vmul.s16 q9, q9, q10
	vext.u8 d10, d7, d3, #3				@p1    ref0[1~8]
	vext.u8 d11, d7, d3, #4				@p2    ref0[2~9]
	vext.u8 d12, d7, d3, #5				@p3    ref0[3~10]	
	vadd.s16 q13, q7, q9

										@m2 d3 ref0[-2~5]					
	vaddl.u8 q9, d9, d10				@p0+p1	
	vaddl.u8 q8, d8, d11				@m1+p2								
	vaddl.u8 q7, d7, d12				@m2+p3
	
	vqshl.u16 q9, q9, #2				@4(p0+p1)
	vmovl.s16 q4, d22	
	vsub.s16 q9, q9, q8					@4(p0+p1) - (m1+p2)
	vmovl.s16 q11, d23	
	vmul.s16 q9, q9, q10
	vmovl.s16 q5, d24
	vmovl.s16 q12, d25
	vmovl.s16 q6, d26			
	vadd.s16 q14, q7, q9
	vmovl.s16 q13, d27	
	vmovl.s16 q7, d28
	vmovl.s16 q14, d29		
        subs    r6, r6, #4                        @ 
	vst4.32 {d8[0], d10[0], d12[0], d14[0]}, [r1], r4			@tmpResJ[0]                                                        
	vst4.32 {d8[1], d10[1], d12[1], d14[1]}, [r1], r4			@tmpResJ[1]                                                        
	vst4.32 {d9[0], d11[0], d13[0], d15[0]}, [r1], r4			@tmpResJ[2]                                                        
	vst4.32 {d9[1], d11[1], d13[1], d15[1]}, [r1], r4			@tmpResJ[3]                                                        
	vst4.32 {d22[0], d24[0], d26[0], d28[0]}, [r1], r4			@tmpResJ[4]
	vst4.32 {d22[1], d24[1], d26[1], d28[1]}, [r1], r4			@tmpResJ[5]
	vst4.32 {d23[0], d25[0], d27[0], d29[0]}, [r1], r4			@tmpResJ[6]
	vst4.32 {d23[1], d25[1], d27[1], d29[1]}, [r1]				@tmpResJ[7]        
        bne     GetDYP2_J_B16                           @ 
        
	mov	r0, r2
		
	vld1.8 {d0[0]}, [r0], r8			@ref0[-2]
	vld1.8 {d0[1]}, [r0], r8			@ref0[-1]
	vld1.8 {d0[2]}, [r0], r8			@ref0[0]
	vld1.8 {d0[3]}, [r0], r8			@ref0[1]
	vld1.8 {d0[4]}, [r0], r8			@ref0[2]
	vld1.8 {d0[5]}, [r0], r8			@ref0[3]
	vld1.8 {d0[6]}, [r0], r8			@ref0[4]
	vld1.8 {d0[7]}, [r0], r8			@ref0[5]
								
	vld1.8 {d12[0]}, [r0], r8			@ref0[6]
	vld1.8 {d12[1]}, [r0], r8			@ref0[7]
	vld1.8 {d12[2]}, [r0], r8			@ref0[8]
	vld1.8 {d12[3]}, [r0], r8			@ref0[9]
	vld1.8 {d12[4]}, [r0], r8			@ref0[10]
	vld1.8 {d12[5]}, [r0], r8			@ref0[11]
	vld1.8 {d12[6]}, [r0], r8			@ref0[12]
	vld1.8 {d12[7]}, [r0], r8			@ref0[13]
	
	vld1.8 {d13[0]}, [r0], r8			@ref0[6]
	vld1.8 {d13[1]}, [r0], r8			@ref0[7]
	vld1.8 {d13[2]}, [r0], r8			@ref0[8]
	vld1.8 {d13[3]}, [r0], r8			@ref0[9]
	vld1.8 {d13[4]}, [r0], r8			@ref0[10]
	vld1.8 {d13[5]}, [r0], r8			@ref0[11]
	vld1.8 {d13[6]}, [r0], r8			@ref0[12]
	vld1.8 {d13[7]}, [r0]				@ref0[13]	
		
							@m2 d0 ref0[-2~5]					
	vext.u8 d2, d0, d12, #1				@m1    ref0[-1~6]
	vext.u8 d4, d0, d12, #2				@p0    ref0[0~7]
	vext.u8 d6, d0, d12, #3				@p1    ref0[1~8]
	vext.u8 d8, d0, d12, #4				@p2    ref0[2~9]
	vext.u8 d10, d0, d12, #5			@p3    ref0[3~10]
	
	vaddl.u8 q15, d4, d6				@p0+p1	
	vaddl.u8 q14, d2, d8				@m1+p2							
	vaddl.u8 q13, d0, d10				@m2+p3
		
	vqshl.u16 q15, q15, #2
	vext.u8 d2, d12, d13, #1				@m1    ref0[-1~6]	
	vsub.s16 q15, q15, q14
	vext.u8 d4, d12, d13, #2				@p0    ref0[0~7]	
	vmul.s16 q15, q15, q10
	vext.u8 d6, d12, d13, #3				@p1    ref0[1~8]
	vext.u8 d8, d12, d13, #4				@p2    ref0[2~9]
	vext.u8 d10, d12, d13, #5			@p3    ref0[3~10]	
	vadd.s16 q12, q15, q13						

							@m2 d0 ref0[-2~5]					
	vaddl.u8 q2, d4, d6				@p0+p1	
	vaddl.u8 q1, d2, d8				@m1+p2								
	vaddl.u8 q0, d12, d10				@m2+p3
	
	vqshl.u16 q2, q2, #2
	vmovl.s16 q14, d24	
	vsub.s16 q2, q2, q1
	vmovl.s16 q15, d25	
	vmul.s16 q2, q2, q10
	
	vadd.s16 q13, q2, q0	
	mov	r1, r3						
	vmovl.s16 q2, d26
	vmovl.s16 q3, d27
		
	vst1.32 {d28[0]}, [r1], r4			@tmpResJ[0]
	vst1.32 {d28[1]}, [r1], r4			@tmpResJ[1]
	vst1.32 {d29[0]}, [r1], r4			@tmpResJ[2]
	vst1.32 {d29[1]}, [r1], r4			@tmpResJ[3]
	vst1.32 {d30[0]}, [r1], r4			@tmpResJ[4]
	vst1.32 {d30[1]}, [r1], r4			@tmpResJ[5]
	vst1.32 {d31[0]}, [r1], r4			@tmpResJ[6]
	vst1.32 {d31[1]}, [r1], r4			@tmpResJ[7]
	
	vst1.32 {d4[0]}, [r1], r4			@tmpResJ[0]
	vst1.32 {d4[1]}, [r1], r4			@tmpResJ[1]
	vst1.32 {d5[0]}, [r1], r4			@tmpResJ[2]
	vst1.32 {d5[1]}, [r1], r4			@tmpResJ[3]
	vst1.32 {d6[0]}, [r1], r4			@tmpResJ[4]
	vst1.32 {d6[1]}, [r1], r4			@tmpResJ[5]
	vst1.32 {d7[0]}, [r1], r4			@tmpResJ[6]
	vst1.32 {d7[1]}, [r1]				@tmpResJ[7]	        
GetDYP2_Jcontine_FUNC:   
	.endm		
	
	.macro 	GetDYP2_DXP
OutLoopCount_8		.req  r10
InnerLoopCount_8	.req  r11
src0_8				.req  r7
src1_8				.req  r8
src2_8				.req  r2
src3_8				.req  r3
src4_8				.req  r4
src5_8				.req  r5
src6_8				.req  r6
temp_8				.req  r7
temp2_8				.req  r9
temp3_8				.req  r12
clip255_8			.req  r14
result_8			.req  r10
dxp_8				.req  r12

destStride_8		.req r4
	@LCLA	srcStride_8
	mov	dxp_8,$4
	str     $3 ,[sp,#48]	@innerCount
	str     $6 ,[sp,#52]	@OutLoopCount_8
.set srcStride_8,   96
	sub     destStride_8,$2,$3
	str     destStride_8,[sp,#44]
	mov	InnerLoopCount_8,$3
	mov     clip255_8,$5
		
DYP2_DXP_OUT: 
	@ldr     InnerLoopCount_8,[sp,#48] 
DYP2_DXP_INNER: 
		cmp		dxp_8,#1
		ldr		src0_8, [$0,#0]
		ldr		src5_8, [$0,#20]
		ldr     src1_8, [$0, #4]                     	@  76.49
		ldr     src4_8, [$0, #16]                      @  76.62
		add		temp_8, src0_8,src5_8
		add		temp2_8, src1_8, src4_8
		ldr     src2_8, [$0,#8]                     		@  76.49
		ldr     src3_8, [$0, #12]                      @  76.62

@		add		temp2_8, src1_8, src4_8

		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		add		src6_8, src2_8,src3_8	
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2)	
@		add		temp2_8, src2_8,src3_8	

		add		temp2_8,src6_8,src6_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 					@
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		src6_8,[$0, #24]
@		add		temp_8,temp_8,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		usat	temp_8, #8, temp_8, asr #10

		moveq	result_8,src2_8
		movne	result_8,src3_8
		add     result_8,result_8,#16
@		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		add     temp_8,temp_8,#1
		usat	result_8, #8, result_8, asr #5

@		add     temp_8,temp_8,#1
		add		temp2_8,src2_8,src5_8
		add     result_8,temp_8,result_8
		add		temp_8,src6_8,src1_8
		mov     result_8,result_8,asr #1

@		add		temp_8,src6_8,src1_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2)	
@		strb	result_8,[$1]
		add		temp2_8, src3_8,src4_8
		strb	result_8,[$1]
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,#512
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		src1_8,[$0, #28]
@		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		usat	temp_8, #8, temp_8, asr #10

		moveq	result_8,src3_8
		movne	result_8,src4_8
		add     result_8,result_8,#16
@		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		add     temp_8,temp_8,#1
		usat	result_8, #8, result_8, asr #5

@		add     temp_8,temp_8,#1
		add		temp2_8,src3_8,src6_8
		add     result_8,temp_8,result_8
		add		temp_8,src1_8,src2_8
		mov     result_8,result_8,asr #1

@		add		temp_8,src1_8,src2_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		strb		result_8,[$1,#1]
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb		temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		strb		result_8,[$1,#1]
		add		temp2_8, src4_8,src5_8
		ldr	    	src2_8,[$0, #32]
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb		temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldr	    	src2_8,[$0, #32]
@		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb		temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		moveq		result_8,src4_8
		movne		result_8,src5_8
		usat	temp_8, #8, temp_8, asr #10

@		moveq		result_8,src4_8
@		movne		result_8,src5_8
		add     	result_8,result_8,#16
@		ldrb		result_8,[clip255_8,+result_8,ASR #5]
		add     	temp_8,temp_8,#1
		usat	result_8, #8, result_8, asr #5

@		add     	temp_8,temp_8,#1
		add		temp2_8,src4_8,src1_8
		add     	result_8,temp_8,result_8
		add		temp_8,src2_8,src3_8
		mov     	result_8,result_8,asr #1	


@		add		temp_8,src2_8,src3_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		strb	result_8,[$1,#2]
		add		temp2_8, src5_8,src6_8
		strb	result_8,[$1,#2]
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		moveq	result_8,src5_8
		movne	result_8,src6_8
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		add     result_8,result_8,#16
		usat	temp_8, #8, temp_8, asr #10

@		moveq	result_8,src5_8
@		movne	result_8,src6_8
@		add     result_8,result_8,#16
@		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		usat	result_8, #8, result_8, asr #5

		add     temp_8,temp_8,#1
		add     $0, $0, #16 
		add     result_8,temp_8,result_8
		mov     result_8,result_8,asr #1
		strb	result_8,[$1,#3]	

		                       		@  82.7
		add     $1, $1, #4                                @  82.7

		subs   InnerLoopCount_8,InnerLoopCount_8,#4
		@ldr    dxp_8,[sp,#56]
		bne	   DYP2_DXP_INNER
	
@@@@@@out loop control
		ldr    destStride_8,[sp,#44]
		ldr    OutLoopCount_8 ,[sp,#52]
		ldr    InnerLoopCount_8,[sp,#48] 
		add	   $0,$0,#srcStride_8
		add	   $1,$1,destStride_8
		sub    $0,$0,InnerLoopCount_8,lsl #2
		subs   OutLoopCount_8,OutLoopCount_8,#1
		str    OutLoopCount_8 ,[sp,#52]
		bne    DYP2_DXP_OUT	
		b_EXIT_FUNC
	
	.endm
		
	
	
_ARM_GB1:  @PROC
@GetBlockLumaNxNInBound(srcY,outStart,outLnLength,dxP,dyP,img->m7,blockSizeX,blockSizeY,width,clp255)@
@r0 = srcY, r1 = outStart, r2 = outLnLength, r3 = dxP@ dyP = [r13, #104]
label1:
	
	 	@GBLA	m7_offset
		@GBLA	blockSizeX_offset
		@GBLA	srcwidth_offset
		@GBLA	clip255_offset
		@GBLA	blockSizeY_offset
		.set m7_offset 	  ,        108
.set blockSizeX_offset , 	      112      
.set blockSizeY_offset ,		  116
.set srcwidth_offset	  ,		  120
.set clip255_offset	  ,		  124

       
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14} @  14_1
        sub     r13, r13, #68                     @  14_1

        ldr     r9, [r13, #104]                   @  14_1

        cmp     r3, #0                            @  33_2

        ldr     r6, [sp, #blockSizeX_offset]                   @  21_27
        ldr     r5, [sp, #clip255_offset]                   @  30_11
        ldr     r8, [sp, #srcwidth_offset]                   @  31_11
        ldr     r10, [sp, #blockSizeY_offset]                   @  21_27
 
        
        bne     _L1_12                            @  33_2
_L1_2:                           @ Preds _L1_1
        cmp     r9, #0                            @  33_2
        bne     _L1_12                            @  33_2
_L1_3:                           @ Preds _L1_2
        sub     r4, r8, r6                        @  37_17
        sub     r5, r2, r6                        @  38_29
        GetFULL r0,r4,r1,r5,r6,r10		  @@@@@@@@@@@@@@GetFULL src,srcStride,dest,destStride,blockSizeX,blockSizeY	
_L1_12:        
	sub     r7, r2, r6                        @  68_26
        sub     r4, r8, r6                        @  69_20
        cmp     r9, #0                            @  71_3
        bne     _L1_30                            @  71_3
_L1_13:                          @ Preds _L1_12
        ands    r2, r3, #1                        @  74_12
        beq     _L1_22                            @  74_4
        
        GetDYP1_DXP1 r0,r4,r1,r7,r6,r3,r5,r10
 
_L1_22:                          @ Preds _L1_13
	GetDYP1_DXP0 r0,r4,r1,r7,r6,r5,r10

_L1_30:                          @ Preds _L1_12
        cmp     r3, #0                            @  155_8
        bne     _L1_48                            @  155_8
_L1_31:                          @ Preds _L1_30
        ands    r2, r9, #1                        @  159_12
        add	r2,r7,r6
        beq     _L1_40                            @  159_4
_L1_311:         
    GetDXP1_DYP1 r0,r8,r1,r2,r6,r9,r5,r10
  
_L1_40:                          @ Preds _L1_31
	GetDXP1_DYP0 r0,r8,r1,r2,r6,r5,r10

_L1_48:                         @ Preds _L1_30
        cmp     r3, #2                            @  240_8
        beq     _L1_88                            @  240_8
_L1_49:                         @ Preds _L1_48
        cmp     r9, #2                            @  240_8
        beq     _L1_63                            @  240_8
_L1_50:                         @ Preds _L1_49
	str     r0, [r13, #4]                    @  0_0
        str     r1, [r13, #8]                   @  0_0
        str     r2, [r13, #12]                    @  0_0
        str     r6, [r13, #16]                     @  0_0
        str     r5, [r13, #20]                     @  0_0
        str     r8, [r13, #24]                    @  0_0
        str     r9, [r13, #28]                    @  0_0
        str     r10, [r13, #32]                    @  0_0
        cmp     r3, #1                            @  243_28
        addne   r0, r0, #1 
        GetDXP3_DYP3 r0,r8,r1,r2,r6,r5,r10
        ldr     r9, [r13, #28]                    @  0_0
        ldr     r0, [r13, #4]                    @  0_0
        ldr     r1, [r13, #8]                   @  0_0
        ldr     r2, [r13, #12]                    @  0_0
        ldr     r6, [r13, #16]                     @  0_0
        ldr     r8, [r13, #24]                    @  0_0
        ldr     r5, [r13, #20]                     @  0_0
        ldr     r10, [r13, #32]                    @  0_0
        cmp     r9,#1
        addne   r0,r0,r8
        sub     r8,r8,r6
        sub     r2,r2,r6
        GetDYP3_DXP3 r0,r8,r1,r2,r6,r5,r10
        


_L1_63:                         @ Preds _L1_49
	str     r10, [r13, #4]
        str     r1, [r13, #8]                     @  0_0
        str     r7, [r13, #12]                    @  0_0
        str     r5, [r13, #16]                    @  0_0
        str     r14, [r13, #20]                   @  0_0
        str     r3, [r13, #24]                    @  0_0
		ldr     r1, [sp,#m7_offset]
        str     r2, [r13, #28]                    @  0_0
        str     r6, [r13, #32]                    @  0_0
        @add     r1, r14, #m7_offset                     @  0_0
                            @  0_0
        sub     r0, r0, r8, lsl #1                @  315_28     
        GetDYP2_J r0,r8,r1,r6,r10
        
        ldr     r10, [r13, #4]
	ldr     r1, [r13, #8]                     @  0_0
        ldr     r7, [r13, #12]                    @  0_0
        ldr     r5, [r13, #16]                    @  0_0
        ldr     r14, [r13, #20]                   @  0_0
	ldr     r0, [sp,#m7_offset]
        ldr     r3, [r13, #24]                    @  0_0
        ldr     r6, [r13, #32]
        ldr     r2, [r13, #28]                    @  0_0
                            @  0_0
        @add     r0, r14, #m7_offset                     @  0_0
        add	r2, r7,r6
        GetDYP2_DXP r0,r1,r2,r6,r3,r5,r10
_L1_88:   
	
	str     r10, [r13, #4]                    @  0_0
	str     r1, [r13, #8]                     @  0_0
        str     r7, [r13, #12]                    @  0_0
        str     r5, [r13, #16]                    @  0_0
        str     r14, [r13, #20]                   @  0_0
        str     r9, [r13, #24]                    @  0_0
		ldr     r1, [sp,#m7_offset]
        str     r2, [r13, #28]                    @  0_0
        str     r6, [r13, #32]                    @  0_0
        @add     r1, r14, #m7_offset                     @  0_0
        sub     r0, r0, r8, lsl #1                @  315_28     
        GetDXP2_J r0,r4,r1,r6,r10
	
	ldr     r10, [r13, #4]                    @  0_0
        ldr     r1, [r13, #8]                     @  0_0
        ldr     r7, [r13, #12]                    @  0_0
        ldr     r5, [r13, #16]                    @  0_0
        ldr     r14, [r13, #20]                   @  0_0
		ldr     r0, [sp,#m7_offset]
        ldr     r9, [r13, #24]                    @  0_0
        ldr     r6, [r13, #32]
        ldr     r2, [r13, #28]                    @  0_0
        
        @add     r0, r14, #m7_offset                     @  0_0
        add	r2, r7,r6
        GetDXP2_DYP r0,r1,r2,r6,r9,r5,r10
	@end
	
	
	