

		.text
	  .align 2
    .globl  ARM_GB1
	
	
	
ARM_GB1:
	
	 	

       
        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14} @  14_1
        sub     r13, r13, #68                     @  14_1

        ldr     r9, [r13, #104]                   @  14_1

        cmp     r3, #0                            @  33_2

        ldr     r6, [sp, #112]                   @  21_27
        ldr     r5, [sp, #124]                   @  30_11
        ldr     r8, [sp, #120]                   @  31_11
        ldr     r10, [sp, #116]                   @  21_27
 
        
        bne     _L1_12                            @  33_2
_L1_2:                          @ Preds _L1_1
        cmp     r9, #0                            @  33_2
        bne     _L1_12                            @  33_2
_L1_3:                          @ Preds _L1_2
        sub     r4, r8, r6                        @  37_17
        sub     r5, r2, r6                        @  38_29
        @GetFULL r0,r4,r1,r5,r6,r10		  @@@@@@@@@@@@@@GetFULL src,srcStride,dest,destStride,blockSizeX,blockSizeY	
        ands	r2,r0,#3
	subne	r0,r0,r2
	movne	r2,r2,lsl #3	
	mov	r7,r10

	cmp		r2,#0
	bne		FULL_OUT_
FULL_OUT:
	mov    r14,r6
FULL_INNER:
	ldr    r3,[r0]
	subs   r14,r14,#4
	add    r0,r0,#4
	str    r3,[r1]
	add	   r1,r1,#4
	bne	   FULL_INNER	
FULL_OUTER:
	add	r0,r0,r4
	add	r1,r1,r5
	subs   r7,r7,#1
	bne    FULL_OUT 
	add     r13, r13, #68                     @  554_3
  ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3

FULL_OUT_:
	mov    r14,r6
FULL_INNER_:
	ldr    r3,[r0]
	ldr  r8,[r0,#4]
	rsb  r9,r2,#32
	mov  r3,r3,lsr r2
	eor  r3,r3,r8,lsl r9 
	subs   r14,r14,#4
	add    r0,r0,#4
	str    r3,[r1]
	add	   r1,r1,#4
	bne	   FULL_INNER_	
FULL_OUTER_:
	add	r0,r0,r4
	add	r1,r1,r5
	subs   r7,r7,#1
	bne    FULL_OUT_ 
	add     r13, r13, #68                     @  554_3
  ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
  
_L1_12:       
	sub     r7, r2, r6                        @  68_26
        sub     r4, r8, r6                        @  69_20
        cmp     r9, #0                            @  71_3
        bne     _L1_30                            @  71_3
_L1_13:                         @ Preds _L1_12
        ands    r2, r3, #1                        @  74_12
        beq     _L1_22                            @  74_4
        
        @GetDYP1_DXP1 r0,r4,r1,r7,r6,r3,r5,r10
        
        	mov     r12,r3
	str     r4, [sp,#40]
	str     r7,[sp,#44]
	str     r6 ,[sp,#48]	
	str     r10 ,[sp,#52]	
	@str     r3 ,[sp,#56]
@	mov     r3,r4
@	mov     r4,r7
@	mov     r11,r10
	mov     r14,r5
	@ldr     r12,[sp,#56]
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16
	
	
	@mov     r12,r3
DYP1_DXP1_OUT:
	ldr     r11,[sp,#48] 
DYP1_DXP1_INNER:
	cmp		r12,#1
	ldrb	r7, [r0,#-2]
	ldrb	r5, [r0,#3]
	ldrb    r8, [r0, #-1]                     @  76.49
    ldrb    r4, [r0, #2]                      @  76.62
    add		r7, r7,r5
    add		r9, r8, r4
    ldrb    r2, [r0]                     		@  76.49
    ldrb    r3, [r0, #1]                      @  76.62
@    add		r9, r8, r4
    add		r9,r9,r9,lsl #2		@5*(-1+2)
    sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
    @smlatb	r7,r12,r9,r7		@(-2+3) - 5*(-1+2)	
    
    add		r9, r2,r3
    ldrb	r6,[r0, #4]		
    add		r9,r9,r9,lsl #2		@5*(0+1)					@
    add		r7,r7,#16
    add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	r7,r12,r9,r7			
@    ldrb	r6,[r0, #4]
@    add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@    ldrb	r10,[r14,+r7,asr #5]		@stall
    add		r9,r2,r5
		usat	r10, #8, r7, asr #5
			
	add		r7,r6,r8
@    add		r9,r2,r5
	
	addeq	r10,r10,r2
	addne	r10,r10,r3
    add     r10,r10,#1
    add		r9,r9,r9,lsl #2		@5*(-1+2)
	mov     r10,r10,asr #1
    
@    add		r9,r9,r9,lsl #2		@5*(-1+2)
    sub		r7,r7,r9			@(-2+3) - 5*(-1+2)		
    @smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	

    add		r9, r3,r4
    strb	r10,[r1]
@    add		r9, r3,r4
    add		r9,r9,r9,lsl #2		@5*(0+1)
	ldrb	r8,[r0, #5]						@
    add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r8,[r0, #5]
    add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
    add		r9,r3,r6
		usat	r10, #8, r7, asr #5
		  
	add		r7,r8,r2
@    add		r9,r3,r6
	
	addeq	r10,r10,r3
	addne	r10,r10,r4
    add     r10,r10,#1
    add		r9,r9,r9,lsl #2		@5*(-1+2)
	mov     r10,r10,asr #1   
    

@    add		r9,r9,r9,lsl #2		@5*(-1+2)
    sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
    @smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
    strb	r10,[r1,#1]
   	add		r9, r4,r5
	ldrb	r2,[r0, #6]
   add		r9,r9,r9,lsl #2		@5*(0+1)					@
    add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
   add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r2,[r0, #6]
@    add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add		r3,r2,r3
		usat	r10, #8, r7, asr #5
			
@	add		r7,r2,r3
    add		r9,r4,r8

	addeq	r10,r10,r4
	addne	r10,r10,r5
    add     r10,r10,#1
   add		r9,r9,r9,lsl #2		@5*(-1+2)
	mov     r10,r10,asr #1 
    
        
 @   add		r9,r9,r9,lsl #2		@5*(-1+2)
    sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
    @smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
    strb	r10,[r1,#2]
   	add		r9, r5,r6
	ldrb	r3,[r0, #7]
   	add		r9,r9,r9,lsl #2		@5*(0+1)					@
    add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
   add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
    @smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r3,[r0, #7]
@    add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r0, r0, #4                        @  82.7
		usat	r10, #8, r7, asr #5
		  
@	add     r0, r0, #4                        @  82.7
    add     r1, r1, #4                        @  82.7
	
	addeq	r10,r10,r5
	addne	r10,r10,r6
    add     r10,r10,#1
	subs   r11,r11,#4
	mov     r10,r10,asr #1 
    
    
	strb	r10,[r1,#-1]
@	subs   r11,r11,#4
	@ldr    r12,[sp,#56]
	bne	   DYP1_DXP1_INNER
@@@@@@out loop control
    ldr    r3,[sp,#40]	
	ldr    r4,[sp,#44]
	ldr    r11 ,[sp,#52]

	add	   r0,r0,r3
	add	   r1,r1,r4
	subs   r11,r11,#1
	str    r11 ,[sp,#52]
	bne    DYP1_DXP1_OUT	
	add     r13, r13, #68                     @  554_3
   ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
 
_L1_22:                         @ Preds _L1_13
	@GetDYP1_DXP0 r0,r4,r1,r7,r6,r5,r10
	str     r4, [sp,#40]
	str     r7,[sp,#44]
	str     r6 ,[sp,#48]	@innerCount
@	str     r10 ,[sp,#52]	@r11
@	mov     r3,r4
@	mov     r4,r7
	mov     r12,r10
	mov     r14,r5
	
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16
	
	
	@mov     dxp_2,$dxp
DYP1_DXP0_OUT:
	ldr     r11,[sp,#48] 
DYP1_DXP0_INNER:
	ldrb	r7, [r0,#-2]
	ldrb	r5, [r0,#3]
	ldrb    r8, [r0, #-1]                     @  76.49
	ldrb    r4, [r0, #2]                      @  76.62
	add		r7, r7,r5
	add		r9, r8, r4
	ldrb    r2, [r0]                     		@  76.49
	ldrb    r3, [r0, #1]                      @  76.62
@	add		r9, r8, r4
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	
	add		r9, r2,r3	
	ldrb	r6,[r0, #4]
	add		r9,r9,r9,lsl #2		@5*(0+1)
	add		r7,r7,#16					@
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r6,[r0, #4]
@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add		r9,r2,r5
		usat	r10, #8, r7, asr #5	
	
	
	add		r7,r6,r8
@	add		r9,r2,r5
	@add		r12, r8, r4
	strb	r10,[r1]
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	
@	strb	r10,[r1]
	add		r9, r3,r4
	ldrb	r8,[r0, #5]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r8,[r0, #5]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add		r9,r3,r6
		usat	r10, #8, r7, asr #5	
	
	
	add		r7,r8,r2
@	add		r9,r3,r6
	
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	r10,[r1,#1]
	add		r9, r4,r5

	ldrb	r2,[r0, #6]	
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r2,[r0, #6]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add		r9,r4,r8
		usat	r10, #8, r7, asr #5	
	
	
	
	add		r7,r2,r3
@	add		r9,r4,r8
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	strb	r10,[r1,#2]
	add		r9, r5,r6
	ldrb	r3,[r0, #7]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r3,[r0, #7]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r0, r0, #4
		usat	r10, #8, r7, asr #5
			
@	add     r0, r0, #4							@  82.7
	add     r1, r1, #4                        @  82.7
	strb	r10,[r1,#-1]
	subs   r11,r11,#4
	bne	   DYP1_DXP0_INNER
@@@@@@out loop control
	ldr    r3,[sp,#40]	
	ldr    r4,[sp,#44]
@	ldr    r11 ,[sp,#52]
	
	add	   r0,r0,r3
	add	   r1,r1,r4
	subs   r12,r12,#1
@	str    r11 ,[sp,#52]
	bne    DYP1_DXP0_OUT	
	 add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
_L1_30:                         @ Preds _L1_12
        cmp     r3, #0                            @  155_8
        bne     _L1_48                            @  155_8
_L1_31:                        @ Preds _L1_30
        ands    r2, r9, #1                        @  159_12
        add	r2,r7,r6
        beq     _L1_40                            @  159_4
_L1_311:        
    @GetDXP1_DYP1 r0,r8,r1,r2,r6,r9,r5,r10
  				mov		r12,r9
	str     r10, [sp,#44]	@innerCount_3
	str     r10 ,[sp,#48]	@innerCount_3
	str     r6 ,[sp,#52]	@r8
	@str     r9 ,[sp,#56]
	
	mov		r7,	r8		
	mov     r8,r10		
	mov     r10,r7
	mov     r11,r2
	sub     r0,r0,r10,lsl #1
	sub     r1,r1,r11
@	pld	[r0]
@	pld	[r0,r10]
@	pld	[r0,r10,lsl #1]
@	pld	[r0,r10,lsl #2]

	mov     r14,r5
	@ldr     r12,[sp,#56]
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16

	@add     r7,r10,r10,lsl #1
	
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
DXP1_DYP1_OUT:
	@ldr    r8,[sp,#48] 
	str     r8,[sp,#44]
DXP1_DYP1_INNER:
	
	ldrb	r7, [r0]
	ldrb	r8, [r0,r10]!
	ldrb	r2, [r0,r10]!             @  76.49
	ldrb	r3, [r0,r10]!             @  76.62
	
	ldrb	r4, [r0,r10]!             @  76.62
	ldrb	r5, [r0,r10]!             @  76.49
	cmp		r12,#1
	
	add		r9, r8, r4
	add		r7, r7,r5
	
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
    
	add		r9, r2,r3	
	ldrb	r6, [r0,r10]!
	add		r9,r9,r9,lsl #2		@5*(0+1)
	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16						@
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@@	ldrb	r6, [r0,r10]!
@@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16

@	ldrb	r9,[r14,+r7,asr #5]		@stall
	add		r8,r6,r8
		usat	r9, #8, r7, asr #5

@	pld	[r0,r10]
@	add		r7,r6,r8
	add		r7,r2,r5
	addeq	r9,r9,r2
	addne	r9,r9,r3
	add     r9,r9,#1
	add		r7,r7,r7,lsl #2		@5*(-1+2)
	mov     r9,r9,asr #1
	strb	r9,[r1,r11]!
            
        	
	
@	add		r9,r2,r5
@	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r8,r7			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	

	
	add		r9, r3,r4
	ldrb	r8,[r0,r10]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r8,[r0,r10]!
@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall*/
	add		r2,r8,r2
		usat	r9, #8, r7, asr #5
@	pld	[r0,r10]
	add		r7,r3,r6
@	add		r7,r8,r2
	addeq	r9,r9,r3
	addne	r9,r9,r4
	add     r9,r9,#1
	add		r7,r7,r7,lsl #2		@5*(-1+2)
	mov     r9,r9,asr #1   
	strb	r9,[r1,r11]!
	
	
@	add		r9,r3,r6
@	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r2,r7			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add		r9, r4,r5
	ldrb	r2,[r0,r10]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r2,[r0,r10]!
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall
	add		r3,r2,r3
		usat	r9, #8, r7, asr #5

	@pld	[r0,r10]
   	add		r7,r4,r8
@	add		r7,r2,r3
	addeq	r9,r9,r4
	addne	r9,r9,r5
	add     r9,r9,#1
	add		r7,r7,r7,lsl #2		@5*(-1+2)
	mov     r9,r9,asr #1 
	strb	r9,[r1,r11]!

@   	add		r9,r4,r8
	
@	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r3,r7			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	add		r9, r5,r6
	ldrb	r3,[r0,r10]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r3,[r0,r10]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall
	sub     r0, r0,  r10,lsl #2         @  82.7
		usat	r9, #8, r7, asr #5

@	sub     r0, r0,  r10,lsl #2         @  82.7
	ldr    r8,[sp,#44]
	addeq	r9,r9,r5
	addne	r9,r9,r6
	add     r9,r9,#1
	subs   r8,r8,#4
	mov     r9,r9,asr #1 
	strb	r9,[r1,r11]!	
	str    r8,[sp,#44] 
	@ldr    r12,[sp,#56]
	bne	   DXP1_DYP1_INNER

@@@@@@out loop control
	ldr    r8 ,[sp,#52]
	ldr     r0,  [sp,#36]
	ldr     r1, [sp,#40]
	subs   r8,r8,#1
	str    r8 ,[sp,#52]
	ldr    r8,[sp,#48]
	@rsb    r7,r8,#16
	addne   r0,r0,#1
	addne	r1,r1,#1
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	
	
	bne    DXP1_DYP1_OUT	
	
 add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
_L1_40:                         @ Preds _L1_31
@	GetDXP1_DYP0 r0,r8,r1,r2,r6,r5,r10
	str     r10 ,[sp,#48]	@innerCount_4
	str     r6 ,[sp,#52]	@r8

	mov		r7,	r8
@	mov     r8,r6
	mov     r12,r10
	
	mov     r10,r7
	mov     r11,r2
	mov     r14,r5

	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16

	@add     r7,r10,r10,lsl #1
	sub     r0,r0,r10,lsl #1
	sub     r1,r1,r11
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
DXP1_DYP0_OUT:
	@ldr    r8,[sp,#48] 
	@str     r8,[sp,#44]
DXP1_DYP0_INNER:
	
	ldrb	r7, [r0]
	ldrb	r8, [r0,r10]!
	ldrb	r2, [r0,r10]!             @  76.49
	ldrb	r3, [r0,r10]!             @  76.62
	ldrb	r4, [r0,r10]!             @  76.62
	ldrb	r5, [r0,r10]!             @  76.49
	
	
	add		r9, r8, r4
	add		r7, r7,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	add		r9, r2,r3
	ldrb	r6, [r0,r10]!		
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r6, [r0,r10]!
@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall
	add		r8,r6,r8
		usat	r9, #8, r7, asr #5
	add		r7,r2,r5			
@	add		r7,r6,r8

	strb	r9,[r1,r11]!
@	add		r9,r2,r5
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r8,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	add		r9, r3,r4
	ldrb	r8,[r0,r10]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r8,[r0,r10]!
@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall*/
	add		r2,r8,r2
		usat	r9, #8, r7, asr #5	
	add		r7,r3,r6	
@	add		r7,r8,r2
	strb	r9,[r1,r11]!
@	add		r9,r3,r6
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r2,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r4,r5
	ldrb	r2,[r0,r10]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r2,[r0,r10]!
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall
	add		r3,r2,r3
		usat	r9, #8, r7, asr #5
	add		r7,r4,r8			
@	add		r7,r2,r3
	strb	r9,[r1,r11]!
@	add		r9,r4,r8
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r5,r6
	ldrb	r3,[r0,r10]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r3,[r0,r10]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #5]		@stall
	sub     r0, r0,  r10,lsl #2         @  82.7
		usat	r9, #8, r7, asr #5
			
@	sub     r0, r0,  r10,lsl #2         @  82.7
@	ldr    r8,[sp,#44]
	subs   r12,r12,#4	
	strb	r9,[r1,r11]!
@	subs   r12,r12,#4
@	str    r8,[sp,#44] 
	bne	   DXP1_DYP0_INNER

@@@@@@out loop control
	ldr    r8 ,[sp,#52]
	ldr     r0,  [sp,#36]
	ldr     r1, [sp,#40]
	subs   r8,r8,#1
	str    r8 ,[sp,#52]
	ldr    r12,[sp,#48]
	@rsb    r7,r8,#16
	addne   r0,r0,#1
	addne	r1,r1,#1
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	
	
	bne    DXP1_DYP0_OUT	
	
	add     r13, r13, #68                     @  554_3
  ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
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
        @GetDXP3_DYP3 r0,r8,r1,r2,r6,r5,r10
        @		str     r10,[sp,#44]		@innerCount_9
		str     r10 ,[sp,#48]	@innerCount_9
		str     r6 ,[sp,#52]	@r8

		mov     r12, r10
		mov     r10,r8
		mov     r11,r2
@		ldr     r8,[sp,#44]
		mov     r14,r5


		@mov		r12,#-5
		@mov		r7,#20
		@orr		r12,r7,r12,lsl #16

		@add     r7,r10,r10,lsl #1
		sub     r0,r0,r10,lsl #1
		sub     r1,r1,r11
		str     r0,  [sp,#36]
		str     r1, [sp,#40]	
DXP3_DYP3_OUT:
	@ldr    r8,[sp,#48] 
@	str     r8,[sp,#44]
DXP3_DYP3_INNER:
	
		ldrb	r7, [r0]
		ldrb	r8, [r0,r10]!
		ldrb	r2, [r0,r10]!             @  76.49
		ldrb	r3, [r0,r10]!             @  76.62

		ldrb	r4, [r0,r10]!             @  76.62
		ldrb	r5, [r0,r10]!             @  76.49


		add		r9, r8, r4
		add		r7, r7,r5
		add		r9,r9,r9,lsl #2		@5*(-1+2)
		sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
		add		r9, r2,r3	
		ldrb	r6, [r0,r10]!			
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldrb	r6, [r0,r10]!
@		add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r9,[r14,+r7,asr #5]		@stall
		add		r8,r6,r8
		usat	r9, #8, r7, asr #5
		add		r7,r2,r5
@		add		r7,r6,r8
		strb	r9,[r1,r11]!
@		add		r9,r2,r5
		add		r9,r7,r7,lsl #2		@5*(-1+2)
		sub		r7,r8,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)			add		r9, r3,r4
		add		r9, r3,r4
		ldrb	r8,[r0,r10]!
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldrb	r8,[r0,r10]!
@		add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r9,[r14,+r7,asr #5]		@stall*/
		add		r2,r8,r2
		usat	r9, #8, r7, asr #5
		add		r7,r3,r6
@		add		r7,r8,r2
		strb	r9,[r1,r11]!
@		add		r9,r3,r6
		add		r9,r7,r7,lsl #2		@5*(-1+2)
		sub		r7,r2,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		r9, r4,r5
		ldrb	r2,[r0,r10]!
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldrb	r2,[r0,r10]!
@		add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r9,[r14,+r7,asr #5]		@stall
		add		r3,r2,r3
		usat	r9, #8, r7, asr #5
		add		r7,r4,r8
@		add		r7,r2,r3
		strb	r9,[r1,r11]!
@		add		r9,r4,r8
		add		r9,r7,r7,lsl #2		@5*(-1+2)
		sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		r9, r5,r6
		ldrb	r3,[r0,r10]
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldrb	r3,[r0,r10]
@		add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r9,[r14,+r7,asr #5]		@stall
		subs   r12,r12,#4
		usat	r9, #8, r7, asr #5

		sub     r0, r0,  r10,lsl #2         @  82.7
@		ldr    r8,[sp,#44]
		strb	r9,[r1,r11]!

@		subs   r12,r12,#4
@		str    r8,[sp,#44] 
		bne	   DXP3_DYP3_INNER	

@@@@@@out loop control
		ldr    r8 ,[sp,#52]
		ldr     r0,  [sp,#36]
		ldr     r1, [sp,#40]
		subs   r8,r8,#1
		str    r8 ,[sp,#52]
		ldr    r12,[sp,#48]
		addne   r0,r0,#1
		addne	r1,r1,#1
		str     r0,  [sp,#36]
		str     r1, [sp,#40]
		bne    DXP3_DYP3_OUT
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
        @GetDYP3_DXP3 r0,r8,r1,r2,r6,r5,r10
        str     r8, [sp,#40]
	str     r2,[sp,#44]
	str     r6 ,[sp,#48]	@innerCount
@	str     r10 ,[sp,#52]	@r11
@	mov     r3,r8
@	mov     r4,r2
@	ldr     r11, [sp,#52]
	mov		r12, r10
	mov		r11, r6

	mov     r14,r5
	
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16
	
	
	@mov     dxp_a,$dxp
DYP3_DXP3_OUT:
@	ldr     r11,[sp,#48] 
DYP3_DXP3_INNER:
	ldrb	r7, [r0,#-2]
	ldrb	r5, [r0,#3]
	ldrb    r8, [r0, #-1]                     @  76.49
	ldrb    r4, [r0, #2]                      @  76.62
	add		r7, r7,r5
	ldrb    r2, [r0]                     		@  76.49
	ldrb    r3, [r0, #1]                      @  76.62
	add		r9, r8, r4
	ldrb	r6,[r0, #4]
	add		r10,r9,r9,lsl #2		@5*(-1+2)
@	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	add		r9, r2,r3	
	sub		r7,r7,r10			@(-2+3) - 5*(-1+2)			
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@		@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1)
	ldrb    r9,[r1] 
@	ldrb	r6,[r0, #4]
@	add		r7,r7,#16		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r9,r9,#1
		usat	r10, #8, r7, asr #5

@	add     r9,r9,#1
	add	r7,r6,r8
	add     r10,r9,r10
	add		r9,r2,r5
	mov     r10,r10,asr #1
	
	
@	add		r9,r2,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@	strb	r10,[r1]
	add		r9, r3,r4
	strb	r10,[r1]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    r9,[r1,#1]
	ldrb	r8,[r0, #5]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r9,r9,#1
		usat	r10, #8, r7, asr #5

@	add     r9,r9,#1
	add	r7,r8,r2
	add     r10,r9,r10
	add		r9,r3,r6
	mov     r10,r10,asr #1
	
	
@	add		r9,r3,r6
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	strb	r10,[r1,#1]
	add		r9, r4,r5
	strb	r10,[r1,#1]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add	r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    r9,[r1,#2]
	ldrb	r2,[r0, #6]
@	add	r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r9,r9,#1
		usat	r10, #8, r7, asr #5

@	add     r9,r9,#1
	add	r7,r2,r3
	add     r10,r9,r10
	add		r9,r4,r8
	mov     r10,r10,asr #1
	
	
	
@	add		r9,r4,r8
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	strb	r10,[r1,#2]
	add		r9, r5,r6
	strb	r10,[r1,#2]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 			@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    r9,[r1,#3]
@	add		r7,r7,#16		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r10,[r14,+r7,asr #5]		@stall
	add     r9,r9,#1
		usat	r10, #8, r7, asr #5

@	add     r9,r9,#1
	add     r0, r0, #4
	add     r10,r9,r10
	subs    r11,r11,#4
	mov     r10,r10,asr #1
	
	strb	r10,[r1,#3]												@  82.7
	add     r1, r1, #4                        @  82.7
	
@	subs    r11,r11,#4
	bne	    DYP3_DXP3_INNER
@@@@@@out loop control
	ldr    r3,[sp,#40]	
	ldr    r4,[sp,#44]
@	ldr    r11 ,[sp,#52]

	ldr     r11,[sp,#48]	
	add	   r0,r0,r3
	add	   r1,r1,r4
	subs   r12,r12,#1
@	str    r11 ,[sp,#52]
	bne    DYP3_DXP3_OUT	
	add     r13, r13, #68                     @  554_3
  ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3


_L1_63:                         @ Preds _L1_49
	str     r10, [r13, #4]
        str     r1, [r13, #8]                     @  0_0
        str     r7, [r13, #12]                    @  0_0
        str     r5, [r13, #16]                    @  0_0
        str     r14, [r13, #20]                   @  0_0
        str     r3, [r13, #24]                    @  0_0
		ldr     r1, [sp,#108]
        str     r2, [r13, #28]                    @  0_0
        str     r6, [r13, #32]                    @  0_0
        @add     r1, r14, #108                     @  0_0
                            @  0_0
        sub     r0, r0, r8, lsl #1                @  315_28     
        @GetDYP2_J r0,r8,r1,r6,r10
        mov     r12,r10
	add     r14, r6,#5
	str     r14 ,[sp,#48]	@r14
	
	mov     r10,r8
	@sub     r0,r0,r10,lsl #1
	sub     r0,r0,#2
	sub     r1,r1,#96
@	pld	[r0]
@	pld	[r0,r10]
@	pld	[r0,r10,lsl #1]
@	pld	[r0,r10,lsl #2]


	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
DYP2_J_OUT:
@	ldr    r11,[sp,#48] 
	mov		r11, r12
DYP2_J_INNER:
	ldrb	r7, [r0]
	ldrb	r8, [r0,r10]!
	ldrb	r2, [r0,r10]!             @  76.49
    	ldrb	r3, [r0,r10]!             @  76.62
    
    	ldrb	r4, [r0,r10]!             @  76.62
	ldrb	r5, [r0,r10]!             @  76.49

    	add	r9, r8, r4
	add	r7, r7,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
    	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
    
	add	r9, r2,r3	
	add		r14,r2,r5
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	ldrb	r6, [r0,r10]!
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r6, [r0,r10]!
	str		r7,[r1,#96]!
	
@	add		r9,r2,r5
	add		r7,r6,r8
	add		r9,r14,r14,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	add		r9, r3,r4
	add		r14,r3,r6
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	ldrb	r8,[r0,r10]!
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r8,[r0,r10]!
	str	    r7,[r1,#96]!
	
@	add		r9,r3,r6
	add		r7,r8,r2
	add		r9,r14,r14,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r4,r5
	add		r14,r4,r8
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	ldrb	r2,[r0,r10]!
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldrb	r2,[r0,r10]!
	str	    r7,[r1,#96]!
  	
@	add		r9,r4,r8
	add		r7,r2,r3
	add		r9,r14,r14,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r5,r6
	subs   r11,r11,#4

	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	subne   r0, r0,  r10,lsl #2         	@  82.7
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	str	    r7,[r1,#96]!
	
@	subs   r11,r11,#4
@	subne   r0, r0,  r10,lsl #2         	@  82.7
	bne	   DYP2_J_INNER
	

@@@@@@out loop control
	ldr     r14 ,[sp,#48]	@r14	
	ldr     	r0,  [sp,#36]
	ldr     	r1, [sp,#40]
	
	
	subs   	r14,r14,#1
	addne   r0,r0,#1
	addne	r1,r1,#4
	str     r14 ,[sp,#48]	@r14	
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	bne     DYP2_J_OUT	
   
	 @add     r13, r13, #68                     @  554_3
   @     ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
        ldr     r10, [r13, #4]
	ldr     r1, [r13, #8]                     @  0_0
        ldr     r7, [r13, #12]                    @  0_0
        ldr     r5, [r13, #16]                    @  0_0
        ldr     r14, [r13, #20]                   @  0_0
	ldr     r0, [sp,#108]
        ldr     r3, [r13, #24]                    @  0_0
        ldr     r6, [r13, #32]
        ldr     r2, [r13, #28]                    @  0_0
                            @  0_0
        @add     r0, r14, #108                     @  0_0
        add	r2, r7,r6
        @GetDYP2_DXP r0,r1,r2,r6,r3,r5,r10
        mov	r12,r3
	str     r6 ,[sp,#48]	@innerCount
	str     r10 ,[sp,#52]	@r10
	@str     r3 ,[sp,#56]

	sub     r4,r2,r6
	str     r4,[sp,#44]
	mov	r11,r6
	@mov     r10,r10
	mov     r14,r5
	@ldr     r12,[sp,#56]
	@mov	r12,#-5
	@mov	r7,#20
	@orr	r12,r7,r12,lsl #16	
	
	
DYP2_DXP_OUT:
	@ldr     r11,[sp,#48] 
DYP2_DXP_INNER:
		cmp		r12,#1
		ldr		r7, [r0,#0]
		ldr		r5, [r0,#20]
		ldr     r8, [r0, #4]                     	@  76.49
		ldr     r4, [r0, #16]                      @  76.62
		add		r7, r7,r5
		add		r9, r8, r4
		ldr     r2, [r0,#8]                     		@  76.49
		ldr     r3, [r0, #12]                      @  76.62

@		add		r9, r8, r4

		add		r9,r9,r9,lsl #2		@5*(-1+2)
		add		r6, r2,r3	
		sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@		add		r9, r2,r3	

		add		r9,r6,r6,lsl #2		@5*(0+1)					@
		add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 					@
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		r6,[r0, #24]
@		add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r7,[r14,+r7,asr #10]		@stall
		usat	r7, #8, r7, asr #10

		moveq	r10,r2
		movne	r10,r3
		add     r10,r10,#16
@		ldrb	r10,[r14,+r10,asr #5]
		add     r7,r7,#1
		usat	r10, #8, r10, asr #5

@		add     r7,r7,#1
		add		r9,r2,r5
		add     r10,r7,r10
		add		r7,r6,r8
		mov     r10,r10,asr #1

@		add		r7,r6,r8
		add		r9,r9,r9,lsl #2		@5*(-1+2)
		sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@		strb	r10,[r1]
		add		r9, r3,r4
		strb	r10,[r1]
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#512
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		ldr		r8,[r0, #28]
@		add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r7,[r14,+r7,asr #10]		@stall
		usat	r7, #8, r7, asr #10

		moveq	r10,r3
		movne	r10,r4
		add     r10,r10,#16
@		ldrb	r10,[r14,+r10,asr #5]
		add     r7,r7,#1
		usat	r10, #8, r10, asr #5

@		add     r7,r7,#1
		add		r9,r3,r6
		add     r10,r7,r10
		add		r7,r8,r2
		mov     r10,r10,asr #1

@		add		r7,r8,r2
		add		r9,r9,r9,lsl #2		@5*(-1+2)
		strb		r10,[r1,#1]
		sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
		@smlatb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		strb		r10,[r1,#1]
		add		r9, r4,r5
		ldr	    	r2,[r0, #32]
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		ldr	    	r2,[r0, #32]
@		add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb		r7,[r14,+r7,asr #10]		@stall
		moveq		r10,r4
		movne		r10,r5
		usat	r7, #8, r7, asr #10

@		moveq		r10,r4
@		movne		r10,r5
		add     	r10,r10,#16
@		ldrb		r10,[r14,+r10,asr #5]
		add     	r7,r7,#1
		usat	r10, #8, r10, asr #5

@		add     	r7,r7,#1
		add		r9,r4,r8
		add     	r10,r7,r10
		add		r7,r2,r3
		mov     	r10,r10,asr #1	


@		add		r7,r2,r3
		add		r9,r9,r9,lsl #2		@5*(-1+2)
		sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
		@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@		strb	r10,[r1,#2]
		add		r9, r5,r6
		strb	r10,[r1,#2]
		add		r9,r9,r9,lsl #2		@5*(0+1)					@
		moveq	r10,r5
		movne	r10,r6
		add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
		@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
		add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@		ldrb	r7,[r14,+r7,asr #10]		@stall
		add     r10,r10,#16
		usat	r7, #8, r7, asr #10

@		moveq	r10,r5
@		movne	r10,r6
@		add     r10,r10,#16
@		ldrb	r10,[r14,+r10,asr #5]
		usat	r10, #8, r10, asr #5

		add     r7,r7,#1
		add     r0, r0, #16 
		add     r10,r7,r10
		mov     r10,r10,asr #1
		strb	r10,[r1,#3]	

		                       		@  82.7
		add     r1, r1, #4                                @  82.7

		subs   r11,r11,#4
		@ldr    r12,[sp,#56]
		bne	   DYP2_DXP_INNER
	
@@@@@@out loop control
		ldr    r4,[sp,#44]
		ldr    r10 ,[sp,#52]
		ldr    r11,[sp,#48] 
		add	   r0,r0,#96
		add	   r1,r1,r4
		sub        r0,r0,r11,lsl #2
		subs   r10,r10,#1
		str    r10 ,[sp,#52]
		bne    DYP2_DXP_OUT	
		add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
_L1_88:   
	
	str     r10, [r13, #4]                    @  0_0
	str     r1, [r13, #8]                     @  0_0
        str     r7, [r13, #12]                    @  0_0
        str     r5, [r13, #16]                    @  0_0
        str     r14, [r13, #20]                   @  0_0
        str     r9, [r13, #24]                    @  0_0
		ldr     r1, [sp,#108]
        str     r2, [r13, #28]                    @  0_0
        str     r6, [r13, #32]                    @  0_0
        @add     r1, r14, #108                     @  0_0
        sub     r0, r0, r8, lsl #1                @  315_28     
        @GetDXP2_J r0,r4,r1,r6,r10
        	add     r14,r10,#5
	rsb     r10,r6,#24
	mov	r10,r10,lsl #2 @tempJ is a int array,4 bytes per element
	str     r4, [sp,#40]
	str     r10, [sp,#44]
@	str     r6 ,[sp,#48]
	@str     $blockSize ,[sp,#52]
	mov     r3,r4
	mov     r12,r6
	
	@sub     r0,r0,r4,lsl #1
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16	
GetDXP2_J_OUT:
@	ldr     r11,[sp,#48] 
	mov     r11, r12
GetDXP2_J_INNER:
	ldrb	r7, [r0,#-2]
	ldrb	r5, [r0,#3]
	ldrb    r8, [r0, #-1]                     @  76.49
	ldrb    r4, [r0, #2]                      @  76.62
@	add	r7, r7,r5
	ldrb    r2, [r0]                     		@  76.49
	ldrb    r3, [r0, #1]                      @  76.62
	add	r9, r8, r4
	add	r7, r7,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r10,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@	ldrb    r6, [r0, #4]
	add	r9, r2,r3	
	ldrb    r6, [r0, #4]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add	r7,r6,r8
	add		r10,r10,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r10,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	
@	add	r7,r6,r8
	add	r9,r2,r5
	ldrb    r8, [r0, #5]
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@	ldrb    r8, [r0, #5]
	str	r10,[r1]
	add	r9, r3,r4
	add		r2,r8,r2
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r10,r3,r6
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb  r10,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
@	add		r7,r8,r2
@	add		r9,r3,r6
	str	r7,[r1,#4]
	add		r9,r10,r10,lsl #2		@5*(-1+2)
	sub		r7,r2,r9			@(-2+3) - 5*(-1+2)
	@	smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldrb    r2, [r0, #6]
@	str	r7,[r1,#4]
	add		r9, r4,r5
	add	r3,r2,r3
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add	r2,r4,r8
	add		r10,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	r10,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
	
	
@	add	r7,r2,r3
@	add	r9,r4,r8
	add		r9,r2,r2,lsl #2		@5*(-1+2)
	sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	str	r10,[r1,#8]
	add	r9, r5,r6
	add     r0, r0, #4                        @  82.7
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	subs   r11,r11,#4
	add		r10,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 
	@smlabb	r10,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	
@	add     r0, r0, #4                        @  82.7
	str	r10,[r1,#12]
	add     r1, r1, #16                        @  82.7
@	subs   r11,r11,#4
	
	bne	   GetDXP2_J_INNER
	@@@@@@out loop control
	ldr    r4,[sp,#40]	
	ldr    r10,[sp,#44]
	@ldr     r14 ,[sp,#52]
	
	subs   r14,r14,#1
	addne	   r0,r0,r4
	addne	   r1,r1,r10
	
	@str    r14 ,[sp,#52]
	bne    GetDXP2_J_OUT	
	
	@add     r13, r13, #68                     @  554_3
  @ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
	
	ldr     r10, [r13, #4]                    @  0_0
        ldr     r1, [r13, #8]                     @  0_0
        ldr     r7, [r13, #12]                    @  0_0
        ldr     r5, [r13, #16]                    @  0_0
        ldr     r14, [r13, #20]                   @  0_0
		ldr     r0, [sp,#108]
        ldr     r9, [r13, #24]                    @  0_0
        ldr     r6, [r13, #32]
        ldr     r2, [r13, #28]                    @  0_0
        
        @add     r0, r14, #108                     @  0_0
        add	r2, r7,r6
        @GetDXP2_DYP r0,r1,r2,r6,r9,r5,r10
        mov	r12,r9
	str     r10,[sp,#44]		@innerCount_6
	str     r10 ,[sp,#48]	@innerCount_6
	str     r6 ,[sp,#52]	@r8
	@str     r9 ,[sp,#56]

		mov     r11,r2
	@sub     r0,r0,srcStride_6,lsl #1
	sub     r1,r1,r11
@	pld	[r0]
@	pld	[r0,#96]

	ldr     r8,[sp,#44]
	mov     r14,r5
	@ldr     r12,[sp,#56]
	@mov		r12,#-5
	@mov		r7,#20
	@orr		r12,r7,r12,lsl #16

	@add     r7,srcStride_6,srcStride_6,lsl #1
	
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	cmp     r12,#2
	beq     DXP2_DYP2
DXP2_DYP1_OUT:
	@ldr    r8,[sp,#48] 
	str     r8,[sp,#44]
DXP2_DYP1_INNER:
	ldr	r7, [r0]
	ldr	r8, [r0,#96]!
	ldr	r2, [r0,#96]!             @  76.49
	ldr	r3, [r0,#96]!             @  76.62
	ldr	r4, [r0,#96]!             @  76.62
	ldr	r5, [r0,#96]!             @  76.49
	cmp		r12,#1
	add		r9, r8, r4
	add		r7, r7,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
    	add		r9, r2,r3	
	ldr 	r6, [r0,#96]!
    	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr 	r6, [r0,#96]!
@	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #10]
	moveq	r10,r2
	movne	r10,r3
		usat	r9, #8, r7, asr #10
			
@	pld	[r0,#96]
@	moveq	r10,r2
@	movne	r10,r3
	add     r10,r10,#16
@	ldrb	r10,[r14,+r10,asr #5] @stall
	add     r9,r9,#1
		usat	r10, #8, r10, asr #5

@	add     r9,r9,#1
	add		r7,r6,r8
	add		r9,r9,r10
	mov     r9,r9,asr #1
	strb	r9,[r1,r11]!
	add		r9,r2,r5
	add		r8, r3,r4
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
@	add		r9, r3,r4
	add		r9,r8,r8,lsl #2		@5*(0+1)					@
	add		r7,r7,#512
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	ldr	    r8,[r0,#96]!
@	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #10]		@stall*/
		usat	r9, #8, r7, asr #10

	@pld	    [r0,#96]
	moveq	r10,r3
	movne	r10,r4
	add     r10,r10,#16
@	ldrb	r10,[r14,+r10,asr #5] @stall
	add     r9,r9,#1
		usat	r10, #8, r10, asr #5

@	add     r9,r9,#1
	add		r2,r8,r2
	add		r9,r9,r10
	add		r7,r3,r6
	mov     r9,r9,asr #1
	strb	r9,[r1,r11]!
@	add		r9,r3,r6
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r2,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r4,r5
	ldr	    r2,[r0,#96]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	    r2,[r0,#96]!
@	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #10]		@stall
	moveq	r10,r4
	movne	r10,r5
		usat	r9, #8, r7, asr #10

	@pld	[r0,#96]
@	moveq	r10,r4
@	movne	r10,r5
	add     r10,r10,#16
@	ldrb	r10,[r14,+r10,asr #5] @stall
	add     r9,r9,#1
		usat	r10, #8, r10, asr #5

@	add     r9,r9,#1
	add		r3,r2,r3
	add		r9,r9,r10
	add		r7,r4,r8
	mov     r9,r9,asr #1
	strb	r9,[r1,r11]!
@	add		r9,r4,r8
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
	@smlatb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r5,r6
	ldr	r3,[r0,#96]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 				@
	@smlabb	r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	r3,[r0,#96]
@	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb	r9,[r14,+r7,asr #10]		@stall
	moveq	r10,r5
	movne	r10,r6
		usat	r9, #8, r7, asr #10

@	moveq	r10,r5
@	movne	r10,r6
	add     r10,r10,#16
@	ldrb	r10,[r14,+r10,asr #5] @stall
	add     r9,r9,#1
		usat	r10, #8, r10, asr #5

@	add     r9,r9,#1
	sub     r0, r0,  #384
	ldr    r8,[sp,#44]
	add		r9,r9,r10
	subs   r8,r8,#4
	mov     r9,r9,asr #1
	strb	r9,[r1,r11]!
@	subs   r8,r8,#4
	str    r8,[sp,#44] 
	@ldr    r12,[sp,#56]
	bne	   DXP2_DYP1_INNER
	
	
	
            
 @@@@@@out loop control
	ldr    r8 ,[sp,#52]
	ldr     r0,  [sp,#36]
	ldr     r1, [sp,#40]
	subs   r8,r8,#1
	str    r8 ,[sp,#52]
	ldr    r8,[sp,#48]
	@rsb    r7,r8,#16
	addne   r0,r0,#4
	addne	r1,r1,#1
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	bne    DXP2_DYP1_OUT	
	 add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3

DXP2_DYP2:
DXP2_DYP2_OUT:
	@ldr    r8,[sp,#48] 
	str     r8,[sp,#44]
DXP2_DYP2_INNER:
	
	ldr		r7, [r0]
	ldr		r8, [r0,#96]!
	ldr		r2, [r0,#96]!             @  76.49
	ldr		r3, [r0,#96]!             @  76.62
	
	ldr		r4, [r0,#96]!             @  76.62
	ldr		r5, [r0,#96]!             @  76.49

	
	add		r9, r8, r4
	add		r7, r7,r5
	add		r9,r9,r9,lsl #2		@5*(-1+2)
	sub		r7,r7,r9			@(-2+3) - 5*(-1+2)
	@smlatb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	
	add		r9, r2,r3					@
	ldr 		r6, [r0,#96]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr 		r6, [r0,#96]!
@	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		r9,[r14,+r7,asr #10]	
	add		r8,r6,r8
		usat	r9, #8, r7, asr #10
			
	@pld		[r0,#96]
@	add		r7,r6,r8
	add		r7,r2,r5	
	strb		r9,[r1,r11]!
@	add		r9,r2,r5
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r8,r9			@(-2+3) - 5*(-1+2)
	@smlatb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2)	
	add		r9, r3,r4
	ldr	    	r8,[r0,#96]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	    	r8,[r0,#96]!
@	add		r7,r7,#512		   				@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		r9,[r14,+r7,asr #10]		@stall*/
	add		r2,r8,r2
		usat	r9, #8, r7, asr #10
			
	@pld	    	[r0,#96]
@	add		r7,r8,r2
	add		r7,r3,r6	
	strb		r9,[r1,r11]!
@	add		r9,r3,r6
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r2,r9			@(-2+3) - 5*(-1+2)
	@smlatb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r4,r5
	ldr	        r2,[r0,#96]!
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr	        r2,[r0,#96]!
@	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@	ldrb		r9,[r14,+r7,asr #10]		@stall
	add		r3,r2,r3
		usat	r9, #8, r7, asr #10
	add		r7,r4,r8	
@	pld		[r0,#96]
@	add		r7,r2,r3
	strb		r9,[r1,r11]!
@	add		r9,r4,r8
	add		r9,r7,r7,lsl #2		@5*(-1+2)
	sub		r7,r3,r9			@(-2+3) - 5*(-1+2)
	@smlatb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
	add		r9, r5,r6
	ldr		r3,[r0,#96]
	add		r9,r9,r9,lsl #2		@5*(0+1)					@
	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
	add		r7,r7,r9,lsl #2		@(-2+3) - 5*(-1+2) + 20*(0+1) 	
	@smlabb		r7,r12,r9,r7			@(-2+3) - 5*(-1+2) + 20*(0+1) 
@	ldr		r3,[r0,#96]
@	add		r7,r7,#512		   							@(-2+3) - 5*(-1+2) + 20*(0+1) + 16
@ldrb		r9,[r14,+r7,asr #10]		@stall
	sub     	r0, r0,  #384
		usat	r9, #8, r7, asr #10	
	
@	sub     	r0, r0,  #384
	ldr    		r8,[sp,#44]
	strb		r9,[r1,r11]!
	
	
	subs   		r8,r8,#4
	str    		r8,[sp,#44] 

	bne	   	DXP2_DYP2_INNER
	
	
	
            
 @@@@@@out loop control
	ldr    r8 ,[sp,#52]
	ldr     r0,  [sp,#36]
	ldr     r1, [sp,#40]
	subs   r8,r8,#1
	str    r8 ,[sp,#52]
	ldr    r8,[sp,#48]
	@rsb    r7,r8,#16
	addne   r0,r0,#4
	addne	r1,r1,#1
	str     r0,  [sp,#36]
	str     r1, [sp,#40]
	bne    DXP2_DYP2_OUT	
	add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
	
	

_L1_112:
EXIT_FUNC:
_L1_113:                        @ Preds _L1_11 _L1_21 _L1_14 _L1_29 _L1_22
                               
        add     r13, r13, #68                     @  554_3
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  554_3
	
