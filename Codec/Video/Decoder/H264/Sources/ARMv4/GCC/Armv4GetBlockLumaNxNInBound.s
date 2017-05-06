

@armasm -cpu xscale \(InputPath) "\(IntDir)/\(InputName).obj"
@@ -- Begin  GetBlockLumaNxNInBound
@@ mark_begin@

        
       .text
	  .align 2
    .globl  ARM_GB1
	
	.macro GetFULL src,srcStride,dest,destStride,blockSizeX,blockSizeY
offset0			.req	r2
OutLoopCount 	.req	r7
InnerLoopCount 	.req	r14
src1			.req	r3
src2			.req	r8
offset1 		.req	r9
	
	ands	offset0,\src,#3
	subne	\src,\src,offset0
	movne	offset0,offset0,lsl #3	
	mov	OutLoopCount,\blockSizeY
FULL_OUT:
	mov    InnerLoopCount,\blockSizeX
	
FULL_INNER:
	cmp		offset0,#0
	ldr    src1,[\src]
	ldrne  src2,[\src,#4]
	rsbne  offset1,offset0,#32
	movne  src1,src1,lsr offset0
	eorne  src1,src1,src2,lsl offset1 
	subs   InnerLoopCount,InnerLoopCount,#4
	add    \src,\src,#4
	str    src1,[\dest]
	add	   \dest,\dest,#4
	bne	   FULL_INNER
	
	

FULL_OUTER:
	add	\src,\src,\srcStride
	add	\dest,\dest,\destStride

	subs   OutLoopCount,OutLoopCount,#1
	bne    FULL_OUT 
	
	b	EXIT_FUNC

	

	
	
	
	.endm
	.macro GetDYP1_DXP1 src,srcStride,dest,destStride,blockSizeX,dxp,clip255,blockSizeY
OutLoopCount_1		.req r11
InnerLoopCount_1	.req r11
src0_1				.req	r7
src1_1				.req	r8
src2_1				.req	r2
src3_1				.req	r3
src4_1				.req	r4
src5_1				.req	r5
src6_1				.req	r6
temp_1				.req	r7
temp2_1				.req	r9
temp3_1				.req	r12
clip255_1			.req	r14
result_1			.req	r10
dxp_1				.req	r12
srcStride_1			.req	r3
destStride_1		.req	r4
	mov     dxp_1,\dxp
	str     \srcStride, [sp,#40]
	str     \destStride,[sp,#44]
	str     \blockSizeX ,[sp,#48]	
	str     \blockSizeY ,[sp,#52]	
	@str     \dxp ,[sp,#56]
	mov     srcStride_1,\srcStride
	mov     destStride_1,\destStride
	mov     OutLoopCount_1,\blockSizeY
	mov     clip255_1,\clip255
	@ldr     dxp_1,[sp,#56]
	@mov		temp3_1,#-5
	@mov		temp_1,#20
	@orr		temp3_1,temp_1,temp3_1,LSL #16
	
	
	@mov     dxp_1,\dxp
DYP1_DXP1_OUT:
	ldr     InnerLoopCount_1,[sp,#48] 
DYP1_DXP1_INNER:
	cmp		dxp_1,#1
	ldrb	src0_1, [\src,#-2]
	ldrb	src5_1, [\src,#3]
	ldrb    src1_1, [\src, #-1]                     @  76.49
    ldrb    src4_1, [\src, #2]                      @  76.62
    add		temp_1, src0_1,src5_1
    ldrb    src2_1, [\src]                     		@  76.49
    ldrb    src3_1, [\src, #1]                      @  76.62
    add		temp2_1, src1_1, src4_1
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(-1+2)
    sub		temp_1,temp_1,temp2_1			@(-2+3) - 5*(-1+2)
    @smlatb	temp_1,temp3_1,temp2_1,temp_1		@(-2+3) - 5*(-1+2)	
    
    add		temp2_1, src2_1,src3_1	
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(0+1)					@
    add		temp_1,temp_1,temp2_1,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	temp_1,temp3_1,temp2_1,temp_1			
    ldrb	src6_1,[\src, #4]
    add		temp_1,temp_1,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
    ldrb	result_1,[clip255_1,+temp_1,ASR #5]		@stall
	
	add		temp_1,src6_1,src1_1
    add		temp2_1,src2_1,src5_1
	
	addeq	result_1,result_1,src2_1
	addne	result_1,result_1,src3_1
    add     result_1,result_1,#1
	mov     result_1,result_1,asr #1
    
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(-1+2)
    sub		temp_1,temp_1,temp2_1			@(-2+3) - 5*(-1+2)		
    @smlatb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2)	

    strb	result_1,[\dest]
    add		temp2_1, src3_1,src4_1
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(0+1)					@
    add		temp_1,temp_1,temp2_1,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src1_1,[\src, #5]
    add		temp_1,temp_1,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_1,[clip255_1,+temp_1,ASR #5]		@stall
  
	add		temp_1,src1_1,src2_1
    add		temp2_1,src3_1,src6_1
	
	addeq	result_1,result_1,src3_1
	addne	result_1,result_1,src4_1
    add     result_1,result_1,#1
	mov     result_1,result_1,asr #1   
    
    
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(-1+2)
    sub		temp_1,temp_1,temp2_1			@(-2+3) - 5*(-1+2)
    @smlatb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2) + 20*(0+1) 
    strb	result_1,[\dest,#1]
   	add		temp2_1, src4_1,src5_1
   add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(0+1)					@
   add		temp_1,temp_1,temp2_1,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src2_1,[\src, #6]
    add		temp_1,temp_1,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_1,[clip255_1,+temp_1,ASR #5]		@stall
	
	add		temp_1,src2_1,src3_1
    add		temp2_1,src4_1,src1_1

	addeq	result_1,result_1,src4_1
	addne	result_1,result_1,src5_1
    add     result_1,result_1,#1
	mov     result_1,result_1,asr #1 
    
        
    
    add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(-1+2)
    sub		temp_1,temp_1,temp2_1			@(-2+3) - 5*(-1+2)
    @smlatb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2) + 20*(0+1) 
    strb	result_1,[\dest,#2]
   	add		temp2_1, src5_1,src6_1
   	add		temp2_1,temp2_1,temp2_1,lsl #2		@5*(0+1)					@
   add		temp_1,temp_1,temp2_1,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	temp_1,temp3_1,temp2_1,temp_1			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src3_1,[\src, #7]
    add		temp_1,temp_1,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_1,[clip255_1,+temp_1,ASR #5]		@stall
  
	add     \src, \src, #4                        @  82.7
    add     \dest, \dest, #4                        @  82.7
	
	addeq	result_1,result_1,src5_1
	addne	result_1,result_1,src6_1
    add     result_1,result_1,#1
	mov     result_1,result_1,asr #1 
    
    
	strb	result_1,[\dest,#-1]
	subs   InnerLoopCount_1,InnerLoopCount_1,#4
	@ldr    dxp_1,[sp,#56]
	bne	   DYP1_DXP1_INNER
@@@@@@out loop control
    ldr    srcStride_1,[sp,#40]	
	ldr    destStride_1,[sp,#44]
	ldr    OutLoopCount_1 ,[sp,#52]

	add	   \src,\src,srcStride_1
	add	   \dest,\dest,destStride_1
	subs   OutLoopCount_1,OutLoopCount_1,#1
	str    OutLoopCount_1 ,[sp,#52]
	bne    DYP1_DXP1_OUT	
	b	EXIT_FUNC
    .endm	
	.macro GetDYP1_DXP0 src,srcStride,dest,destStride,blockSizeX,clip255,blockSizeY
OutLoopCount_2		.req	r11
InnerLoopCount_2	.req	r11
src0_2				.req	r7
src1_2				.req	r8
src2_2				.req	r2
src3_2				.req	r3
src4_2				.req	r4
src5_2				.req	r5
src6_2				.req	r6
temp_2				.req  	r7
temp2_2				.req  r9
temp3_2				.req  r12
clip255_2			.req  r14
result_2			.req  r10
srcStride_2			.req  r3
destStride_2		.req  r4
	str     \srcStride, [sp,#40]
	str     \destStride,[sp,#44]
	str     \blockSizeX ,[sp,#48]	@innerCount
	str     \blockSizeY ,[sp,#52]	@OutLoopCount_2
	mov     srcStride_2,\srcStride
	mov     destStride_2,\destStride
	mov     OutLoopCount_2,\blockSizeY
	mov     clip255_2,\clip255
	
	@mov		temp3_2,#-5
	@mov		temp_2,#20
	@orr		temp3_2,temp_2,temp3_2,LSL #16
	
	
	@mov     dxp_2,\dxp
DYP1_DXP0_OUT:
	ldr     InnerLoopCount_2,[sp,#48] 
DYP1_DXP0_INNER:
	ldrb	src0_2, [\src,#-2]
	ldrb	src5_2, [\src,#3]
	ldrb    src1_2, [\src, #-1]                     @  76.49
	ldrb    src4_2, [\src, #2]                      @  76.62
	add		temp_2, src0_2,src5_2
	ldrb    src2_2, [\src]                     		@  76.49
	ldrb    src3_2, [\src, #1]                      @  76.62
	add		temp2_2, src1_2, src4_2
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(-1+2)
	sub		temp_2,temp_2,temp2_2			@(-2+3) - 5*(-1+2)
	@smlatb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2)	
	
	add		temp2_2, src2_2,src3_2	
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(0+1)					@
	add		temp_2,temp_2,temp2_2,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src6_2,[\src, #4]
	add		temp_2,temp_2,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_2,[clip255_2,+temp_2,ASR #5]		@stall
	
	
	
	add		temp_2,src6_2,src1_2
	add		temp2_2,src2_2,src5_2
	@add		temp3_2, src1_2, src4_2
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(-1+2)
	sub		temp_2,temp_2,temp2_2			@(-2+3) - 5*(-1+2)
	@smlatb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2)	
	
	strb	result_2,[\dest]
	add		temp2_2, src3_2,src4_2
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(0+1)					@
	add		temp_2,temp_2,temp2_2,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src1_2,[\src, #5]
	add		temp_2,temp_2,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_2,[clip255_2,+temp_2,ASR #5]		@stall
	
	
	
	add		temp_2,src1_2,src2_2
	add		temp2_2,src3_2,src6_2
	
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(-1+2)
	sub		temp_2,temp_2,temp2_2			@(-2+3) - 5*(-1+2)
	@smlatb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	result_2,[\dest,#1]
	add		temp2_2, src4_2,src5_2
	
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(0+1)					@
	add		temp_2,temp_2,temp2_2,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src2_2,[\src, #6]
	add		temp_2,temp_2,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_2,[clip255_2,+temp_2,ASR #5]		@stall
	
	
	
	
	add		temp_2,src2_2,src3_2
	add		temp2_2,src4_2,src1_2
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(-1+2)
	sub		temp_2,temp_2,temp2_2			@(-2+3) - 5*(-1+2)
	@smlatb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	result_2,[\dest,#2]
	add		temp2_2, src5_2,src6_2
	add		temp2_2,temp2_2,temp2_2,lsl #2		@5*(0+1)					@
	add		temp_2,temp_2,temp2_2,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	temp_2,temp3_2,temp2_2,temp_2			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src3_2,[\src, #7]
	add		temp_2,temp_2,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_2,[clip255_2,+temp_2,ASR #5]		@stall
	
	add     \src, \src, #4							@  82.7
	add     \dest, \dest, #4                        @  82.7
	strb	result_2,[\dest,#-1]
	subs   InnerLoopCount_2,InnerLoopCount_2,#4
	bne	   DYP1_DXP0_INNER
@@@@@@out loop control
	ldr    srcStride_2,[sp,#40]	
	ldr    destStride_2,[sp,#44]
	ldr    OutLoopCount_2 ,[sp,#52]
	
	add	   \src,\src,srcStride_2
	add	   \dest,\dest,destStride_2
	subs   OutLoopCount_2,OutLoopCount_2,#1
	str    OutLoopCount_2 ,[sp,#52]
	bne    DYP1_DXP0_OUT	
	b	EXIT_FUNC
	
	
	
	.endm
 	.macro GetDXP1_DYP1 src,srcStride,dest,destStride,blockSizeX,dyp,clip255,blockSizeY
OutLoopCount_3			.req  r8
InnerLoopCount_3		.req  r8
src0_3				.req  r7
src1_3				.req  r8
src2_3				.req  r2
src3_3				.req  r3
src4_3				.req  r4
src5_3				.req  r5
src6_3				.req  r6
temp_3				.req  r7
temp2_3				.req  r9
temp3_3				.req  r12
clip255_3			.req  r14
result_3			.req  r9
dyp_3				.req  r12
srcStride_3			.req  r10
destStride_3		.req  r11
	
	mov		dyp_3,\dyp
	str     \blockSizeY, [sp,#44]	@innerCount_3
	str     \blockSizeY ,[sp,#48]	@innerCount_3
	str     \blockSizeX ,[sp,#52]	@OutLoopCount_3
	@str     \dyp ,[sp,#56]
	
	mov		src0_3,	\srcStride		
	mov     InnerLoopCount_3,\blockSizeY		
	mov     srcStride_3,src0_3
	mov     destStride_3,\destStride
	sub     \src,\src,srcStride_3,lsl #1
	sub     \dest,\dest,destStride_3
@	pld	[\src]
@	pld	[\src,srcStride_3]
@	pld	[\src,srcStride_3,lsl #1]
@	pld	[\src,srcStride_3,lsl #2]

	mov     clip255_3,\clip255
	@ldr     dyp_3,[sp,#56]
	@mov		temp3_3,#-5
	@mov		temp_3,#20
	@orr		temp3_3,temp_3,temp3_3,LSL #16

	@add     temp_3,srcStride_3,srcStride_3,lsl #1
	
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
DXP1_DYP1_OUT:
	@ldr    InnerLoopCount_3,[sp,#48] 
	str     InnerLoopCount_3,[sp,#44]
DXP1_DYP1_INNER:
	
	ldrb	src0_3, [\src]
	ldrb	src1_3, [\src,srcStride_3]!
	ldrb	src2_3, [\src,srcStride_3]!             @  76.49
	ldrb	src3_3, [\src,srcStride_3]!             @  76.62
	
	ldrb	src4_3, [\src,srcStride_3]!             @  76.62
	ldrb	src5_3, [\src,srcStride_3]!             @  76.49
	cmp		dyp_3,#1
	
	add		temp2_3, src1_3, src4_3
	add		temp_3, src0_3,src5_3
	
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(-1+2)
	sub		temp_3,temp_3,temp2_3			@(-2+3) - 5*(-1+2)
	@smlatb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2)	
    
	add		temp2_3, src2_3,src3_3	
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(0+1)					@
	add		temp_3,temp_3,temp2_3,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src6_3, [\src,srcStride_3]!
	add		temp_3,temp_3,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16

	ldrb	result_3,[clip255_3,+temp_3,ASR #5]		@stall
@	pld	[\src,srcStride_3]
	add		temp_3,src6_3,src1_3
	addeq	result_3,result_3,src2_3
	addne	result_3,result_3,src3_3
	add     result_3,result_3,#1
	mov     result_3,result_3,asr #1
	strb	result_3,[\dest,destStride_3]!
            
        	
	
	add		temp2_3,src2_3,src5_3
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(-1+2)
	sub		temp_3,temp_3,temp2_3			@(-2+3) - 5*(-1+2)
	@smlatb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2)	

	
	add		temp2_3, src3_3,src4_3
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(0+1)					@
	add		temp_3,temp_3,temp2_3,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src1_3,[\src,srcStride_3]!
	add		temp_3,temp_3,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_3,[clip255_3,+temp_3,ASR #5]		@stall*/
@	pld	[\src,srcStride_3]

	add		temp_3,src1_3,src2_3
	addeq	result_3,result_3,src3_3
	addne	result_3,result_3,src4_3
	add     result_3,result_3,#1
	mov     result_3,result_3,asr #1   
	strb	result_3,[\dest,destStride_3]!
	
	
	add		temp2_3,src3_3,src6_3
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(-1+2)
	sub		temp_3,temp_3,temp2_3			@(-2+3) - 5*(-1+2)
	@smlatb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add		temp2_3, src4_3,src5_3
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(0+1)					@
	add		temp_3,temp_3,temp2_3,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src2_3,[\src,srcStride_3]!
	add		temp_3,temp_3,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_3,[clip255_3,+temp_3,ASR #5]		@stall
	@pld	[\src,srcStride_3]
	add		temp_3,src2_3,src3_3
	addeq	result_3,result_3,src4_3
	addne	result_3,result_3,src5_3
	add     result_3,result_3,#1
	mov     result_3,result_3,asr #1 
	strb	result_3,[\dest,destStride_3]!

   	add		temp2_3,src4_3,src1_3
	
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(-1+2)
	sub		temp_3,temp_3,temp2_3			@(-2+3) - 5*(-1+2)
	@smlatb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add		temp2_3, src5_3,src6_3
	add		temp2_3,temp2_3,temp2_3,lsl #2		@5*(0+1)					@
	add		temp_3,temp_3,temp2_3,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_3,temp3_3,temp2_3,temp_3			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src3_3,[\src,srcStride_3]
	add		temp_3,temp_3,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_3,[clip255_3,+temp_3,ASR #5]		@stall

	sub     \src, \src,  srcStride_3,lsl #2         @  82.7
	ldr    InnerLoopCount_3,[sp,#44]
	addeq	result_3,result_3,src5_3
	addne	result_3,result_3,src6_3
	add     result_3,result_3,#1
	mov     result_3,result_3,asr #1 
	strb	result_3,[\dest,destStride_3]!
	
	
	subs   InnerLoopCount_3,InnerLoopCount_3,#4
	
	
	str    InnerLoopCount_3,[sp,#44] 
	@ldr    dyp_3,[sp,#56]
	bne	   DXP1_DYP1_INNER

@@@@@@out loop control
	ldr    OutLoopCount_3 ,[sp,#52]
	ldr     \src,  [sp,#36]
	ldr     \dest, [sp,#40]
	subs   OutLoopCount_3,OutLoopCount_3,#1
	str    OutLoopCount_3 ,[sp,#52]
	ldr    InnerLoopCount_3,[sp,#48]
	@rsb    temp_3,OutLoopCount_3,#16
	addne   \src,\src,#1
	addne	\dest,\dest,#1
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	
	
	bne    DXP1_DYP1_OUT	
	
	b	EXIT_FUNC
	.endm
	.macro GetDXP1_DYP0 src,srcStride,dest,destStride,blockSizeX,clip255,blockSizeY
OutLoopCount_4			.req  r8
InnerLoopCount_4		.req  r8
src0_4				.req  r7
src1_4				.req  r8
src2_4				.req  r2
src3_4				.req  r3
src4_4				.req  r4
src5_4				.req  r5
src6_4				.req  r6
temp_4				.req  r7
temp2_4				.req  r9
temp3_4				.req  r12
clip255_4			.req  r14
result_4			.req  r9

srcStride_4			.req  r10
destStride_4		.req  r11
	
	
	str     \blockSizeY,[sp,#44]		@innerCount_4
	str     \blockSizeY ,[sp,#48]	@innerCount_4
	str     \blockSizeX ,[sp,#52]	@OutLoopCount_4

	mov		src0_4,	\srcStride
	mov     InnerLoopCount_4,\blockSizeY	
	mov     srcStride_4,src0_4
	mov     destStride_4,\destStride

	mov     clip255_4,\clip255

	@mov		temp3_4,#-5
	@mov		temp_4,#20
	@orr		temp3_4,temp_4,temp3_4,LSL #16

	@add     temp_4,srcStride_4,srcStride_4,lsl #1
	sub     \src,\src,srcStride_4,lsl #1
	sub     \dest,\dest,destStride_4
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
DXP1_DYP0_OUT:
	@ldr    InnerLoopCount_4,[sp,#48] 
	str     InnerLoopCount_4,[sp,#44]
DXP1_DYP0_INNER:
	
	ldrb	src0_4, [\src]
	ldrb	src1_4, [\src,srcStride_4]!
	ldrb	src2_4, [\src,srcStride_4]!             @  76.49
	ldrb	src3_4, [\src,srcStride_4]!             @  76.62
	ldrb	src4_4, [\src,srcStride_4]!             @  76.62
	ldrb	src5_4, [\src,srcStride_4]!             @  76.49
	
	
	add		temp2_4, src1_4, src4_4
	add		temp_4, src0_4,src5_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(-1+2)
	sub		temp_4,temp_4,temp2_4			@(-2+3) - 5*(-1+2)
	@smlatb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2)	
	add		temp2_4, src2_4,src3_4	
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(0+1)					@
	add		temp_4,temp_4,temp2_4,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src6_4, [\src,srcStride_4]!
	add		temp_4,temp_4,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_4,[clip255_4,+temp_4,ASR #5]		@stall
	
	add		temp_4,src6_4,src1_4
	strb	result_4,[\dest,destStride_4]!
	add		temp2_4,src2_4,src5_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(-1+2)
	sub		temp_4,temp_4,temp2_4			@(-2+3) - 5*(-1+2)
	@smlatb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2)	
	add		temp2_4, src3_4,src4_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(0+1)					@
	add		temp_4,temp_4,temp2_4,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src1_4,[\src,srcStride_4]!
	add		temp_4,temp_4,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_4,[clip255_4,+temp_4,ASR #5]		@stall*/
	
	
	add		temp_4,src1_4,src2_4
	strb	result_4,[\dest,destStride_4]!
	add		temp2_4,src3_4,src6_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(-1+2)
	sub		temp_4,temp_4,temp2_4			@(-2+3) - 5*(-1+2)
	@smlatb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_4, src4_4,src5_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(0+1)					@
	add		temp_4,temp_4,temp2_4,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src2_4,[\src,srcStride_4]!
	add		temp_4,temp_4,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_4,[clip255_4,+temp_4,ASR #5]		@stall
	
	add		temp_4,src2_4,src3_4
	strb	result_4,[\dest,destStride_4]!
	add		temp2_4,src4_4,src1_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(-1+2)
	sub		temp_4,temp_4,temp2_4			@(-2+3) - 5*(-1+2)
	@smlatb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_4, src5_4,src6_4
	add		temp2_4,temp2_4,temp2_4,lsl #2		@5*(0+1)					@
	add		temp_4,temp_4,temp2_4,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_4,temp3_4,temp2_4,temp_4			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src3_4,[\src,srcStride_4]
	add		temp_4,temp_4,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_4,[clip255_4,+temp_4,ASR #5]		@stall
	
	sub     \src, \src,  srcStride_4,lsl #2         @  82.7
	ldr    InnerLoopCount_4,[sp,#44]
	
	strb	result_4,[\dest,destStride_4]!
	subs   InnerLoopCount_4,InnerLoopCount_4,#4
	str    InnerLoopCount_4,[sp,#44] 
	bne	   DXP1_DYP0_INNER

@@@@@@out loop control
	ldr    OutLoopCount_4 ,[sp,#52]
	ldr     \src,  [sp,#36]
	ldr     \dest, [sp,#40]
	subs   OutLoopCount_4,OutLoopCount_4,#1
	str    OutLoopCount_4 ,[sp,#52]
	ldr    InnerLoopCount_4,[sp,#48]
	@rsb    temp_4,OutLoopCount_4,#16
	addne   \src,\src,#1
	addne	\dest,\dest,#1
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	
	
	bne    DXP1_DYP0_OUT	
	
	b	EXIT_FUNC
	.endm
	
	.macro GetDXP3_DYP3 src,srcStride,dest,destStride,blockSizeX,clip255,blockSizeY
OutLoopCount_9			.req  r8
InnerLoopCount_9		.req  r8
src0_9				.req  r7
src1_9				.req  r8
src2_9				.req  r2
src3_9				.req  r3
src4_9				.req  r4
src5_9				.req  r5
src6_9				.req  r6
temp_9				.req  r7
temp2_9				.req  r9
temp3_9				.req  r12
clip255_9			.req  r14
result_9			.req  r9

srcStride_9			.req  r10
destStride_9			.req  r11


		str     \blockSizeY,[sp,#44]		@innerCount_9
		str     \blockSizeY ,[sp,#48]	@innerCount_9
		str     \blockSizeX ,[sp,#52]	@OutLoopCount_9

		mov     srcStride_9,\srcStride
		mov     destStride_9,\destStride
		ldr     InnerLoopCount_9,[sp,#44]
		mov     clip255_9,\clip255

		@mov		temp3_9,#-5
		@mov		temp_9,#20
		@orr		temp3_9,temp_9,temp3_9,LSL #16

		@add     temp_9,srcStride_9,srcStride_9,lsl #1
		sub     \src,\src,srcStride_9,lsl #1
		sub     \dest,\dest,destStride_9
		str     \src,  [sp,#36]
		str     \dest, [sp,#40]	
DXP3_DYP3_OUT:
	@ldr    InnerLoopCount_9,[sp,#48] 
	str     InnerLoopCount_9,[sp,#44]
DXP3_DYP3_INNER:
	
		ldrb	src0_9, [\src]
		ldrb	src1_9, [\src,srcStride_9]!
		ldrb	src2_9, [\src,srcStride_9]!             @  76.49
		ldrb	src3_9, [\src,srcStride_9]!             @  76.62

		ldrb	src4_9, [\src,srcStride_9]!             @  76.62
		ldrb	src5_9, [\src,srcStride_9]!             @  76.49


		add		temp2_9, src1_9, src4_9
		add		temp_9, src0_9,src5_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(-1+2)
		sub		temp_9,temp_9,temp2_9			@(-2+3) - 5*(-1+2)
		@smlatb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2)	
		add		temp2_9, src2_9,src3_9		
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(0+1)					@
		add		temp_9,temp_9,temp2_9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldrb	src6_9, [\src,srcStride_9]!
		add		temp_9,temp_9,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	result_9,[clip255_9,+temp_9,ASR #5]		@stall

		add		temp_9,src6_9,src1_9
		strb	result_9,[\dest,destStride_9]!
		add		temp2_9,src2_9,src5_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(-1+2)
		sub		temp_9,temp_9,temp2_9			@(-2+3) - 5*(-1+2)
		@smlatb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2)			add		temp2_9, src3_9,src4_9
		add		temp2_9, src3_9,src4_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(0+1)					@
		add		temp_9,temp_9,temp2_9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldrb	src1_9,[\src,srcStride_9]!
		add		temp_9,temp_9,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	result_9,[clip255_9,+temp_9,ASR #5]		@stall*/


		add		temp_9,src1_9,src2_9
		strb	result_9,[\dest,destStride_9]!
		add		temp2_9,src3_9,src6_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(-1+2)
		sub		temp_9,temp_9,temp2_9			@(-2+3) - 5*(-1+2)
		@smlatb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		temp2_9, src4_9,src5_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(0+1)					@
		add		temp_9,temp_9,temp2_9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldrb	src2_9,[\src,srcStride_9]!
		add		temp_9,temp_9,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	result_9,[clip255_9,+temp_9,ASR #5]		@stall

		add		temp_9,src2_9,src3_9
		strb	result_9,[\dest,destStride_9]!
		add		temp2_9,src4_9,src1_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(-1+2)
		sub		temp_9,temp_9,temp2_9			@(-2+3) - 5*(-1+2)
		@smlatb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		temp2_9, src5_9,src6_9
		add		temp2_9,temp2_9,temp2_9,lsl #2		@5*(0+1)					@
		add		temp_9,temp_9,temp2_9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	temp_9,temp3_9,temp2_9,temp_9			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldrb	src3_9,[\src,srcStride_9]
		add		temp_9,temp_9,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	result_9,[clip255_9,+temp_9,ASR #5]		@stall

		sub     \src, \src,  srcStride_9,lsl #2         @  82.7
		ldr    InnerLoopCount_9,[sp,#44]
		strb	result_9,[\dest,destStride_9]!

		subs   InnerLoopCount_9,InnerLoopCount_9,#4
		str    InnerLoopCount_9,[sp,#44] 
		bne	   DXP3_DYP3_INNER	

@@@@@@out loop control
		ldr    OutLoopCount_9 ,[sp,#52]
		ldr     \src,  [sp,#36]
		ldr     \dest, [sp,#40]
		subs   OutLoopCount_9,OutLoopCount_9,#1
		str    OutLoopCount_9 ,[sp,#52]
		ldr    InnerLoopCount_9,[sp,#48]
		addne   \src,\src,#1
		addne	\dest,\dest,#1
		str     \src,  [sp,#36]
		str     \dest, [sp,#40]
		bne    DXP3_DYP3_OUT
	.endm
	
	.macro GetDYP3_DXP3 src,srcStride,dest,destStride,blockSizeX,clip255,blockSizeY
OutLoopCount_a		.req  r11
InnerLoopCount_a	.req  r11
src0_a				.req  r7
src1_a				.req  r8
src2_a				.req  r2
src3_a				.req  r3
src4_a				.req  r4
src5_a				.req  r5
src6_a				.req  r6
temp_a				.req  r7
temp2_a				.req  r9
temp3_a				.req  r12
clip255_a			.req  r14
result_a			.req  r10
srcStride_a			.req  r3
destStride_a		.req  r4
	str     \srcStride, [sp,#40]
	str     \destStride,[sp,#44]
	str     \blockSizeX ,[sp,#48]	@innerCount
	str     \blockSizeY ,[sp,#52]	@OutLoopCount_a
	mov     srcStride_a,\srcStride
	mov     destStride_a,\destStride
	ldr     OutLoopCount_a, [sp,#52]
	mov     clip255_a,\clip255
	
	@mov		temp3_a,#-5
	@mov		temp_a,#20
	@orr		temp3_a,temp_a,temp3_a,LSL #16
	
	
	@mov     dxp_a,\dxp
DYP3_DXP3_OUT:
	ldr     InnerLoopCount_a,[sp,#48] 
DYP3_DXP3_INNER:
	ldrb	src0_a, [\src,#-2]
	ldrb	src5_a, [\src,#3]
	ldrb    src1_a, [\src, #-1]                     @  76.49
	ldrb    src4_a, [\src, #2]                      @  76.62
	add		temp_a, src0_a,src5_a
	ldrb    src2_a, [\src]                     		@  76.49
	ldrb    src3_a, [\src, #1]                      @  76.62
	add		temp2_a, src1_a, src4_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(-1+2)
	sub		temp_a,temp_a,temp2_a			@(-2+3) - 5*(-1+2)
	@smlatb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2)	
	add		temp2_a, src2_a,src3_a			
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(0+1)					@
	add		temp_a,temp_a,temp2_a,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@		@
	@smlabb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1)
	ldrb    temp2_a,[\dest] 
	ldrb	src6_a,[\src, #4]
	add		temp_a,temp_a,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_a,[clip255_a,+temp_a,ASR #5]		@stall
	add     temp2_a,temp2_a,#1
	add	temp_a,src6_a,src1_a
	add     result_a,temp2_a,result_a
	mov     result_a,result_a,asr #1
	
	
	add		temp2_a,src2_a,src5_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(-1+2)
	sub		temp_a,temp_a,temp2_a			@(-2+3) - 5*(-1+2)
	@smlatb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2)	
	strb	result_a,[\dest]
	add		temp2_a, src3_a,src4_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(0+1)					@
	add		temp_a,temp_a,temp2_a,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    temp2_a,[\dest,#1]
	ldrb	src1_a,[\src, #5]
	add		temp_a,temp_a,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_a,[clip255_a,+temp_a,ASR #5]		@stall
	add     temp2_a,temp2_a,#1
	add	temp_a,src1_a,src2_a
	add     result_a,temp2_a,result_a
	mov     result_a,result_a,asr #1
	
	
	add		temp2_a,src3_a,src6_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(-1+2)
	sub		temp_a,temp_a,temp2_a			@(-2+3) - 5*(-1+2)
	@smlatb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	result_a,[\dest,#1]
	add		temp2_a, src4_a,src5_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(0+1)					@
	add		temp_a,temp_a,temp2_a,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    temp2_a,[\dest,#2]
	ldrb	src2_a,[\src, #6]
	add	temp_a,temp_a,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_a,[clip255_a,+temp_a,ASR #5]		@stall
	add     temp2_a,temp2_a,#1
	add	temp_a,src2_a,src3_a
	add     result_a,temp2_a,result_a
	mov     result_a,result_a,asr #1
	
	
	
	add		temp2_a,src4_a,src1_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(-1+2)
	sub		temp_a,temp_a,temp2_a			@(-2+3) - 5*(-1+2)
	@smlatb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	result_a,[\dest,#2]
	add		temp2_a, src5_a,src6_a
	add		temp2_a,temp2_a,temp2_a,lsl #2		@5*(0+1)					@
	add		temp_a,temp_a,temp2_a,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	temp_a,temp3_a,temp2_a,temp_a			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    temp2_a,[\dest,#3]
	add		temp_a,temp_a,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_a,[clip255_a,+temp_a,ASR #5]		@stall
	add     temp2_a,temp2_a,#1
	add     \src, \src, #4
	add     result_a,temp2_a,result_a
	mov     result_a,result_a,asr #1
	
	strb	result_a,[\dest,#3]												@  82.7
	add     \dest, \dest, #4                        @  82.7
	
	subs    InnerLoopCount_a,InnerLoopCount_a,#4
	bne	    DYP3_DXP3_INNER
@@@@@@out loop control
	ldr    srcStride_a,[sp,#40]	
	ldr    destStride_a,[sp,#44]
	ldr    OutLoopCount_a ,[sp,#52]
	
	add	   \src,\src,srcStride_a
	add	   \dest,\dest,destStride_a
	subs   OutLoopCount_a,OutLoopCount_a,#1
	str    OutLoopCount_a ,[sp,#52]
	bne    DYP3_DXP3_OUT	
	b	EXIT_FUNC
	
	.endm
	.macro GetDXP2_J src,srcStride,dest,blockSizeX,blockSizeY
OutLoopCount_5		.req  r14
InnerLoopCount_5	.req  r11
src0_5				.req  r7
src1_5				.req  r8
src2_5				.req  r2
src3_5				.req  r3
src4_5				.req  r4
src5_5				.req  r5
src6_5				.req  r6
temp_5				.req  r7
temp2_5				.req  r9
temp3_5				.req  r12
result_5			.req  r10
dxp_5				.req  r12
srcStride_5			.req  r3
destStride_5		.req  r10
	add     OutLoopCount_5,\blockSizeY,#5
	rsb     destStride_5,\blockSizeX,#24
	mov	destStride_5,destStride_5,lsl #2 @tempJ is a int array,4 bytes per element
	str     \srcStride, [sp,#40]
	str     destStride_5, [sp,#44]
	str     \blockSizeX ,[sp,#48]
	@str     \blockSize ,[sp,#52]
	mov     srcStride_5,\srcStride
	
	@sub     \src,\src,\srcStride,lsl #1
	@mov		temp3_5,#-5
	@mov		temp_5,#20
	@orr		temp3_5,temp_5,temp3_5,LSL #16	
GetDXP2_J_OUT:
	ldr     InnerLoopCount_5,[sp,#48] 
GetDXP2_J_INNER:
	ldrb	src0_5, [\src,#-2]
	ldrb	src5_5, [\src,#3]
	ldrb    src1_5, [\src, #-1]                     @  76.49
	ldrb    src4_5, [\src, #2]                      @  76.62
	add	temp_5, src0_5,src5_5
	ldrb    src2_5, [\src]                     		@  76.49
	ldrb    src3_5, [\src, #1]                      @  76.62
	add	temp2_5, src1_5, src4_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(-1+2)
	sub		temp_5,temp_5,temp2_5			@(-2+3) - 5*(-1+2)
	@smlatb	temp_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2)	
	ldrb    src6_5, [\src, #4]
	add	temp2_5, src2_5,src3_5	
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(0+1)					@
	add		result_5,temp_5,temp2_5,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	result_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	
	add	temp_5,src6_5,src1_5
	add	temp2_5,src2_5,src5_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(-1+2)
	sub		temp_5,temp_5,temp2_5			@(-2+3) - 5*(-1+2)
	@smlatb	temp_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2)	
	ldrb    src1_5, [\src, #5]
	str	result_5,[\dest]
	add	temp2_5, src3_5,src4_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(0+1)					@
	add		result_5,temp_5,temp2_5,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb  result_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add		temp_5,src1_5,src2_5
	add		temp2_5,src3_5,src6_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(-1+2)
	sub		temp_5,temp_5,temp2_5			@(-2+3) - 5*(-1+2)
	@	smlatb	temp_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    src2_5, [\src, #6]
	str	result_5,[\dest,#4]
	add		temp2_5, src4_5,src5_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(0+1)					@
	add		result_5,temp_5,temp2_5,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	result_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	
	add	temp_5,src2_5,src3_5
	add	temp2_5,src4_5,src1_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(-1+2)
	sub		temp_5,temp_5,temp2_5			@(-2+3) - 5*(-1+2)
	@smlatb	temp_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	str	result_5,[\dest,#8]
	add	temp2_5, src5_5,src6_5
	add		temp2_5,temp2_5,temp2_5,lsl #2		@5*(0+1)					@
	add		result_5,temp_5,temp2_5,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	result_5,temp3_5,temp2_5,temp_5			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add     \src, \src, #4                        @  82.7
	str	result_5,[\dest,#12]
	add     \dest, \dest, #16                        @  82.7
	subs   InnerLoopCount_5,InnerLoopCount_5,#4
	
	bne	   GetDXP2_J_INNER
	@@@@@@out loop control
	ldr    \srcStride,[sp,#40]	
	ldr    destStride_5,[sp,#44]
	@ldr     OutLoopCount_5 ,[sp,#52]
	
	subs   OutLoopCount_5,OutLoopCount_5,#1
	addne	   \src,\src,\srcStride
	addne	   \dest,\dest,destStride_5
	
	@str    OutLoopCount_5 ,[sp,#52]
	bne    GetDXP2_J_OUT	
	
	@b	EXIT_FUNC
	.endm
	.macro GetDXP2_DYP src,dest,destStride,blockSizeX,dyp,clip255,blockSizeY
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
	mov	dyp_6,\dyp
	str     \blockSizeY,[sp,#44]		@innerCount_6
	str     \blockSizeY ,[sp,#48]	@innerCount_6
	str     \blockSizeX ,[sp,#52]	@OutLoopCount_6
	@str     \dyp ,[sp,#56]
.set srcStride_t	,	96
.set srcStride_t4	, 	384
		mov     destStride_6,\destStride
	@sub     \src,\src,srcStride_6,lsl #1
	sub     \dest,\dest,destStride_6
@	pld	[\src]
@	pld	[\src,#srcStride_t]

	ldr     InnerLoopCount_6,[sp,#44]
	mov     clip255_6,\clip255
	@ldr     dyp_6,[sp,#56]
	@mov		temp3_6,#-5
	@mov		temp_6,#20
	@orr		temp3_6,temp_6,temp3_6,LSL #16

	@add     temp_6,srcStride_6,srcStride_6,lsl #1
	
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	cmp     dyp_6,#2
	beq     DXP2_DYP2
DXP2_DYP1_OUT:
	@ldr    InnerLoopCount_6,[sp,#48] 
	str     InnerLoopCount_6,[sp,#44]
DXP2_DYP1_INNER:
	ldr	src0_6, [\src]
	ldr	src1_6, [\src,#srcStride_t]!
	ldr	src2_6, [\src,#srcStride_t]!             @  76.49
	ldr	src3_6, [\src,#srcStride_t]!             @  76.62
	ldr	src4_6, [\src,#srcStride_t]!             @  76.62
	ldr	src5_6, [\src,#srcStride_t]!             @  76.49
	cmp		dyp_6,#1
	add		temp2_6, src1_6, src4_6
	add		temp_6, src0_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
    	add		temp2_6, src2_6,src3_6	
    	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr 	src6_6, [\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_6,[clip255_6,+temp_6,ASR #10]	
@	pld	[\src,#srcStride_t]
	moveq	result1_6,src2_6
	movne	result1_6,src3_6
	add     result1_6,result1_6,#16
	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
	add		temp_6,src6_6,src1_6
	add		result_6,result_6,result1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[\dest,destStride_6]!
	add		temp2_6,src2_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
	add		temp2_6, src3_6,src4_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	    src1_6,[\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall*/
	@pld	    [\src,#srcStride_t]
	moveq	result1_6,src3_6
	movne	result1_6,src4_6
	add     result1_6,result1_6,#16
	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
	add		temp_6,src1_6,src2_6
	add		result_6,result_6,result1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[\dest,destStride_6]!
	add		temp2_6,src3_6,src6_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src4_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	    src2_6,[\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall
	@pld	[\src,#srcStride_t]
	moveq	result1_6,src4_6
	movne	result1_6,src5_6
	add     result1_6,result1_6,#16
	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
	add		temp_6,src2_6,src3_6
	add		result_6,result_6,result1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[\dest,destStride_6]!
	add		temp2_6,src4_6,src1_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src5_6,src6_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	src3_6,[\src,#srcStride_t]
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb	result_6,[clip255_6,+temp_6,ASR #10]		@stall
	moveq	result1_6,src5_6
	movne	result1_6,src6_6
	add     result1_6,result1_6,#16
	ldrb	result1_6,[clip255_6,+result1_6,ASR #5] @stall
	add     result_6,result_6,#1
	sub     \src, \src,  #srcStride_t4
	ldr    InnerLoopCount_6,[sp,#44]
	add		result_6,result_6,result1_6
	mov     result_6,result_6,asr #1
	strb	result_6,[\dest,destStride_6]!
	subs   InnerLoopCount_6,InnerLoopCount_6,#4
	str    InnerLoopCount_6,[sp,#44] 
	@ldr    dyp_6,[sp,#56]
	bne	   DXP2_DYP1_INNER
	
	
	
            
 @@@@@@out loop control
	ldr    OutLoopCount_6 ,[sp,#52]
	ldr     \src,  [sp,#36]
	ldr     \dest, [sp,#40]
	subs   OutLoopCount_6,OutLoopCount_6,#1
	str    OutLoopCount_6 ,[sp,#52]
	ldr    InnerLoopCount_6,[sp,#48]
	@rsb    temp_6,OutLoopCount_6,#16
	addne   \src,\src,#4
	addne	\dest,\dest,#1
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	bne    DXP2_DYP1_OUT	
	b	EXIT_FUNC

DXP2_DYP2:
DXP2_DYP2_OUT:
	@ldr    InnerLoopCount_6,[sp,#48] 
	str     InnerLoopCount_6,[sp,#44]
DXP2_DYP2_INNER:
	
	ldr		src0_6, [\src]
	ldr		src1_6, [\src,#srcStride_t]!
	ldr		src2_6, [\src,#srcStride_t]!             @  76.49
	ldr		src3_6, [\src,#srcStride_t]!             @  76.62
	
	ldr		src4_6, [\src,#srcStride_t]!             @  76.62
	ldr		src5_6, [\src,#srcStride_t]!             @  76.49

	
	add		temp2_6, src1_6, src4_6
	add		temp_6, src0_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
	
	add		temp2_6, src2_6,src3_6					@
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr 		src6_6, [\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb		result_6,[clip255_6,+temp_6,ASR #10]	
	
	@pld		[\src,#srcStride_t]
	add		temp_6,src6_6,src1_6
	
	strb		result_6,[\dest,destStride_6]!
	add		temp2_6,src2_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2)	
	add		temp2_6, src3_6,src4_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	    	src1_6,[\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall*/
	
	@pld	    	[\src,#srcStride_t]
	add		temp_6,src1_6,src2_6
	
	strb		result_6,[\dest,destStride_6]!
	add		temp2_6,src3_6,src6_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src4_6,src5_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	        src2_6,[\src,#srcStride_t]!
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall
	
@	pld		[\src,#srcStride_t]
	add		temp_6,src2_6,src3_6
	strb		result_6,[\dest,destStride_6]!
	add		temp2_6,src4_6,src1_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(-1+2)
	sub		temp_6,temp_6,temp2_6			@(-2+3) - 5*(-1+2)
	@smlatb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_6, src5_6,src6_6
	add		temp2_6,temp2_6,temp2_6,lsl #2		@5*(0+1)					@
	add		temp_6,temp_6,temp2_6,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		temp_6,temp3_6,temp2_6,temp_6			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr		src3_6,[\src,#srcStride_t]
	add		temp_6,temp_6,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	ldrb		result_6,[clip255_6,+temp_6,ASR #10]		@stall
	
	
	sub     	\src, \src,  #srcStride_t4
	ldr    		InnerLoopCount_6,[sp,#44]
	strb		result_6,[\dest,destStride_6]!
	
	
	subs   		InnerLoopCount_6,InnerLoopCount_6,#4
	str    		InnerLoopCount_6,[sp,#44] 

	bne	   	DXP2_DYP2_INNER
	
	
	
            
 @@@@@@out loop control
	ldr    OutLoopCount_6 ,[sp,#52]
	ldr     \src,  [sp,#36]
	ldr     \dest, [sp,#40]
	subs   OutLoopCount_6,OutLoopCount_6,#1
	str    OutLoopCount_6 ,[sp,#52]
	ldr    InnerLoopCount_6,[sp,#48]
	@rsb    temp_6,OutLoopCount_6,#16
	addne   \src,\src,#4
	addne	\dest,\dest,#1
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	bne    DXP2_DYP2_OUT	
	b	EXIT_FUNC	
	
	
	.endm
	.macro GetDYP2_J src,srcStride,dest,blockSizeX,blockSizeY
OutLoopCount_7			.req  r14
InnerLoopCount_7		.req  r11
src0_7				.req  r7
src1_7				.req  r8
src2_7				.req  r2
src3_7				.req  r3
src4_7				.req  r4
src5_7				.req  r5
src6_7				.req  r6
temp_7				.req  r7
temp2_7				.req  r9
temp3_7				.req  r12
clip255_7			.req  r14
result_7			.req  r9
srcStride_7			.req  r10
destStride_7		.req  r11
	@LCLA	destStride_t
.set destStride_t	,	96
	
	mov     InnerLoopCount_7,\blockSizeY
	add     OutLoopCount_7, \blockSizeX,#5
	str     InnerLoopCount_7 ,[sp,#48]	@innerCount_7
	
	mov     srcStride_7,\srcStride
	@sub     \src,\src,srcStride_7,lsl #1
	sub     \src,\src,#2
	sub     \dest,\dest,#destStride_t
@	pld	[\src]
@	pld	[\src,srcStride_7]
@	pld	[\src,srcStride_7,lsl #1]
@	pld	[\src,srcStride_7,lsl #2]


	@mov		temp3_7,#-5
	@mov		temp_7,#20
	@orr		temp3_7,temp_7,temp3_7,LSL #16
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
DYP2_J_OUT:
	ldr    InnerLoopCount_7,[sp,#48] 
DYP2_J_INNER:
	ldrb	src0_7, [\src]
	ldrb	src1_7, [\src,srcStride_7]!
	ldrb	src2_7, [\src,srcStride_7]!             @  76.49
    	ldrb	src3_7, [\src,srcStride_7]!             @  76.62
    
    	ldrb	src4_7, [\src,srcStride_7]!             @  76.62
	ldrb	src5_7, [\src,srcStride_7]!             @  76.49

    	add	temp2_7, src1_7, src4_7
	add	temp_7, src0_7,src5_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(-1+2)
	sub		temp_7,temp_7,temp2_7			@(-2+3) - 5*(-1+2)
    	@smlatb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2)	
    
	add	temp2_7, src2_7,src3_7	
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(0+1)					@
	add		temp_7,temp_7,temp2_7,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src6_7, [\src,srcStride_7]!
	str		temp_7,[\dest,#destStride_t]!
	
	add		temp2_7,src2_7,src5_7
	add		temp_7,src6_7,src1_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(-1+2)
	sub		temp_7,temp_7,temp2_7			@(-2+3) - 5*(-1+2)
	@smlatb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2)	
	add		temp2_7, src3_7,src4_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(0+1)					@
	add		temp_7,temp_7,temp2_7,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src1_7,[\src,srcStride_7]!
	str	    temp_7,[\dest,#destStride_t]!
	
	add		temp2_7,src3_7,src6_7
	add		temp_7,src1_7,src2_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(-1+2)
	sub		temp_7,temp_7,temp2_7			@(-2+3) - 5*(-1+2)
	@smlatb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_7, src4_7,src5_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(0+1)					@
	add		temp_7,temp_7,temp2_7,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb	src2_7,[\src,srcStride_7]!
	str	    temp_7,[\dest,#destStride_t]!
  	
	add		temp2_7,src4_7,src1_7
	add		temp_7,src2_7,src3_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(-1+2)
	sub		temp_7,temp_7,temp2_7			@(-2+3) - 5*(-1+2)
	@smlatb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		temp2_7, src5_7,src6_7
	add		temp2_7,temp2_7,temp2_7,lsl #2		@5*(0+1)					@
	add		temp_7,temp_7,temp2_7,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	temp_7,temp3_7,temp2_7,temp_7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	str	    temp_7,[\dest,#destStride_t]!
	
	subs   InnerLoopCount_7,InnerLoopCount_7,#4
	subne   \src, \src,  srcStride_7,lsl #2         	@  82.7
	bne	   DYP2_J_INNER
	

@@@@@@out loop control
	
	ldr     	\src,  [sp,#36]
	ldr     	\dest, [sp,#40]
	
	
	subs   	OutLoopCount_7,OutLoopCount_7,#1
	addne   \src,\src,#1
	addne	\dest,\dest,#4
	str     \src,  [sp,#36]
	str     \dest, [sp,#40]
	bne     DYP2_J_OUT	
   
	.endm
	.macro GetDYP2_DXP src,dest,destStride,blockSizeX,dxp,clip255,blockSizeY
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
	mov	dxp_8,\dxp
	str     \blockSizeX ,[sp,#48]	@innerCount
	str     \blockSizeY ,[sp,#52]	@OutLoopCount_8
	@str     \dxp ,[sp,#56]
.set srcStride_8	,     96
	sub     destStride_8,\destStride,\blockSizeX
	str     destStride_8,[sp,#44]
	mov	InnerLoopCount_8,\blockSizeX
	@mov     OutLoopCount_8,\blockSizeY
	mov     clip255_8,\clip255
	@ldr     dxp_8,[sp,#56]
	@mov	temp3_8,#-5
	@mov	temp_8,#20
	@orr	temp3_8,temp_8,temp3_8,LSL #16	
	
	
DYP2_DXP_OUT:
	@ldr     InnerLoopCount_8,[sp,#48] 
DYP2_DXP_INNER:
		cmp		dxp_8,#1
		ldr		src0_8, [\src,#0]
		ldr		src5_8, [\src,#20]
		ldr     src1_8, [\src, #4]                     	@  76.49
		ldr     src4_8, [\src, #16]                      @  76.62
		add		temp_8, src0_8,src5_8
		ldr     src2_8, [\src,#8]                     		@  76.49
		ldr     src3_8, [\src, #12]                      @  76.62

		add		temp2_8, src1_8, src4_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2)	
		add		temp2_8, src2_8,src3_8	
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 					@
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		src6_8,[\src, #24]
		add		temp_8,temp_8,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		moveq	result_8,src2_8
		movne	result_8,src3_8
		add     result_8,result_8,#16
		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		add     temp_8,temp_8,#1
		add		temp2_8,src2_8,src5_8
		add     result_8,temp_8,result_8
		mov     result_8,result_8,asr #1

		add		temp_8,src6_8,src1_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2)	
		strb	result_8,[\dest]
		add		temp2_8, src3_8,src4_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		src1_8,[\src, #28]
		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		moveq	result_8,src3_8
		movne	result_8,src4_8
		add     result_8,result_8,#16
		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		add     temp_8,temp_8,#1
		add		temp2_8,src3_8,src6_8
		add     result_8,temp_8,result_8
		mov     result_8,result_8,asr #1

		add		temp_8,src1_8,src2_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb		temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		strb		result_8,[\dest,#1]
		add		temp2_8, src4_8,src5_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb		temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr	    	src2_8,[\src, #32]
		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb		temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		moveq		result_8,src4_8
		movne		result_8,src5_8
		add     	result_8,result_8,#16
		ldrb		result_8,[clip255_8,+result_8,ASR #5]
		add     	temp_8,temp_8,#1
		add		temp2_8,src4_8,src1_8
		add     	result_8,temp_8,result_8
		mov     	result_8,result_8,asr #1	


		add		temp_8,src2_8,src3_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(-1+2)
		sub		temp_8,temp_8,temp2_8			@(-2+3) - 5*(-1+2)
		@smlatb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		strb	result_8,[\dest,#2]
		add		temp2_8, src5_8,src6_8
		add		temp2_8,temp2_8,temp2_8,lsl #2		@5*(0+1)					@
		add		temp_8,temp_8,temp2_8,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	temp_8,temp3_8,temp2_8,temp_8			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		temp_8,temp_8,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		ldrb	temp_8,[clip255_8,+temp_8,ASR #10]		@stall
		moveq	result_8,src5_8
		movne	result_8,src6_8
		add     result_8,result_8,#16
		ldrb	result_8,[clip255_8,+result_8,ASR #5]
		add     temp_8,temp_8,#1
		add     \src, \src, #16 
		add     result_8,temp_8,result_8
		mov     result_8,result_8,asr #1
		strb	result_8,[\dest,#3]	

		                       		@  82.7
		add     \dest, \dest, #4                                @  82.7

		subs   InnerLoopCount_8,InnerLoopCount_8,#4
		@ldr    dxp_8,[sp,#56]
		bne	   DYP2_DXP_INNER
	
@@@@@@out loop control
		ldr    destStride_8,[sp,#44]
		ldr    OutLoopCount_8 ,[sp,#52]
		ldr    InnerLoopCount_8,[sp,#48] 
		add	   \src,\src,#srcStride_8
		add	   \dest,\dest,destStride_8
		sub        \src,\src,InnerLoopCount_8,lsl #2
		subs   OutLoopCount_8,OutLoopCount_8,#1
		str    OutLoopCount_8 ,[sp,#52]
		bne    DYP2_DXP_OUT	
		b	EXIT_FUNC
	
	.endm
ARM_GB1:
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
_L1_2:                          @ Preds _L1_1
        cmp     r9, #0                            @  33_2
        bne     _L1_12                            @  33_2
_L1_3:                          @ Preds _L1_2
        sub     r4, r8, r6                        @  37_17
        sub     r5, r2, r6                        @  38_29
        GetFULL r0,r4,r1,r5,r6,r10		  @@@@@@@@@@@@@@GetFULL src,srcStride,dest,destStride,blockSizeX,blockSizeY	
_L1_12:       
	sub     r7, r2, r6                        @  68_26
        sub     r4, r8, r6                        @  69_20
        cmp     r9, #0                            @  71_3
        bne     _L1_30                            @  71_3
_L1_13:                         @ Preds _L1_12
        ands    r2, r3, #1                        @  74_12
        beq     _L1_22                            @  74_4
        
        GetDYP1_DXP1 r0,r4,r1,r7,r6,r3,r5,r10
 
_L1_22:                         @ Preds _L1_13
	GetDYP1_DXP0 r0,r4,r1,r7,r6,r5,r10

_L1_30:                         @ Preds _L1_12
        cmp     r3, #0                            @  155_8
        bne     _L1_48                            @  155_8
_L1_31:                         @ Preds _L1_30
        ands    r2, r9, #1                        @  159_12
        add	r2,r7,r6
        beq     _L1_40                            @  159_4
_L1_311:        
    GetDXP1_DYP1 r0,r8,r1,r2,r6,r9,r5,r10
  
_L1_40:                         @ Preds _L1_31
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
_L1_112:
EXIT_FUNC:
_L1_113:                        @ Preds _L1_11 _L1_21 _L1_14 _L1_29 _L1_22
                               
        add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
	@ENDP
	
	@EXPORT ARM_GB1
	
	@END
