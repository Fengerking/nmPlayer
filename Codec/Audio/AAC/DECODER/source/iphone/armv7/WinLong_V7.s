
	@AREA	.text, CODE, READONLY
	.text
        .align    4
	@.globl    _AAD_srtdata68
	@.globl    _AAD_srtdata69
	@.globl    _AAD_srtdata70
	@.globl    _AAD_srtdata71
	
	.globl  	_AAD_srtidata511

@#ifdef ASM_IOS
@       WinLong(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, 
@		ics->window_shape, decoder->prevWinShape[chOut], &sinWindow[0], &kbdWindow[0]);
@#else
@	WinLong(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, ics->window_shape, decoder->prevWinShape[chOut]);
@#endif
        @ AAD_srtdata68 --- sinWindow
        @ AAD_srtdata69 --- sinWindowOffset = {0, 128}
        @ AAD_srtdata70 --- kdbWindow
        @ AAD_srtdata71 --- kdbWindowOffset = {0, 128}
        @ r5 --- sinWindow   [sp, #40]
        @ r6 --- kdbWindow   [sp, #44]

_AAD_srtidata511: @PROC
	stmdb   sp!, {r4 - r11, lr}
M3005:
	ldr     r7, [sp, #36]                
	add     r12, r0, #2048         			@ buf0 += (1024 >> 1)@       
	add     r8, r2, #2048 					@ out1  = out0 + 1024@
	mov     r5, r12                 		@ buf1  = buf0@
	add     r10, r1, #2048 					@ over1 = over0 + 1024@                       
	add     r4, r8, #2048					@ out1  = out0 + 1024@                     
	add     r9, r10, #2048                	@ over1 = over0 + 1024
	cmp     r7, #1                        
	bne     M3007                         
M3006:   
        ldr     r6, [sp, #44]                       
	@ldr     r6, M3019 
        mov     r0, #128                  
	@ldr     r0, M3019+4                 
	@ldr     r0, [r0, #4]                 
	add     r6, r6, r0, lsl #2           
	b       M3008                        
M3007:
        ldr     r6, [sp, #40]
	@ldr     r6, M3019+8  
        mov     r0, #128
	@ldr     r0, M3019+12      
	@ldr     r0, [r0, #4]       
	add     r6, r6, r0, lsl #2 
M3008:   
	cmp     r3, r7  
	bne     M3012
M3010:
	vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32	q5, q5
	vrev64.32		q9, q8
       	
	sub				r5, r5, #16

	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
	vswp				d10, d11
	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q15, q7, q5						@ *out1-- = in + f1@
				
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r4]				@ *out1-- = in + f1@	
	vswp				d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
			       	
	cmp     		r1, r9            	
	bcc     		M3010        
M3011:                        
	b       M3018        
M3012:
	cmp     r3, #1          
	bne     M3014        
M3013:   
        ldr     r3, [sp, #44]                     
	@ldr     r3, M3019 
        mov     r0, #128                 
	@ldr     r0, M3019+4                
	@ldr     r0, [r0, #4]                  
	add     r0, r3, r0, lsl #2            
	b       M3015                      
M3014:
        ldr     r3, [sp, #40]
	@ldr     r3, M3019+8
         
        mov     r0, #128     
	@ldr     r0, M3019+12             
	@ldr     r0, [r0, #4]                
	add     r0, r3, r0, lsl #2          
M3015:
M3016:                       
	vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
	vld2.i32	  	{d0, d1, d2, d3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32		q14, q5
	vrev64.32		q9, q8
      
       	
	sub				r5, r5, #16

	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
	vswp				d28, d29
	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q15, q14, q7					@ *out1-- = in + f1@
				
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r4]
	vswp					d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
		
	cmp     		r1, r9            	
	bcc     		M3016   
M3018:
	ldmia   sp!, {r4 - r11, pc}
@M3019:                      
@	.word     _AAD_srtdata70
@	.word     _AAD_srtdata71
@	.word     _AAD_srtdata68
@	.word     _AAD_srtdata69
				
	@ENDP  @ AAD_srtidata511
		
	.globl  _WinLongStart
		
	@AREA	.text, CODE, READONLY

_WinLongStart: @PROC

	stmdb     sp!, {r4 - r11, lr}
M4005:	
	ldr     r7, [sp, #36]                
    add     r12, r0, #2048         			@ buf0 += (1024 >> 1)@       
    mov		r11, #0
    add     r8, r2, #2048 					@ out1  = out0 + 1024@
    vdup.s32  q15, r11
    mov     r5, r12                 		@ buf1  = buf0@
    add     r10, r1, #2048 					@ over1 = over0 + 1024@                       
    add     r4, r8, #2048					@ out1  = out0 + 1024@                     
    add     r9, r10, #2048                	@ over1 = over0 + 1024
    cmp     r7, #1                          
    bne     M4007           
M4006:
    ldr     r6, [sp, #44]
    @ldr     r6, M4016  
    mov     r0, #128       
    @ldr     r0, M4016+4              
    @ldr     r0, [r0, #4]              
    add     r6, r6, r0, lsl #2      
    b       M4008                     
M4007:
    ldr     r6, [sp, #40]                          
    @ldr     r6, M4016+8  
    mov     r0, #128      
    @ldr     r0, M4016+12        
    @ldr     r0, [r0, #4]         
    add     r6, r6, r0, lsl #2 
M4008:         
    mov     r0, #448                   
M4009:   
		vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
		vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
    sub				r4, r4, #16				
    vld1.i32		{d6, d7}, [r1]					@ in = *over0@
    vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
    sub				r5, r5, #16       	
    vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
	
   	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
   	vld1.i32		d17, [r5]!						@ in = *buf1--@
    vld1.i32		d16, [r5]!						@ in = *buf1--@
    vrev64.32	q0, q5
		vrev64.32		q9, q8
       	
    sub				r5, r5, #16

    vld1.i32		{d14, d15}, [r9]				@ in = *over1@
		vswp				d0, d1
    vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q1, q0, q7						@ *out1-- = in + f1@
	
	vshr.s32		q9, q9, #1
				
	vst1.i32		{d30, d31}, [r9]				@ *over1-- = 0@
	vst1.i32		{d2, d3}, [r4]					@ *out1-- = in + f1@
	vst1.i32		{d18, d19}, [r1]!				@ *over0++ = in >> 1@		
	
	subs	r0, r0, #4	  
    bne     M4009                   
M4010:                         
    cmp     r3, #1             
    bne     M4012              
M4011: 
    ldr     r3, [sp, #44]                         
    @ldr     r3, M4016 
    mov     r0, #0          
    @ldr     r0, M4016+4             
    @ldr     r0, [r0, #0]             
    add     r0, r3, r0, lsl #2       
    b       M4013                    
M4012:
    ldr     r3, [sp, #40]                   
    @ldr     r3, M4016+8  
    mov     r0, #0                 
    @ldr     r0, M4016+12                  
    @ldr     r0, [r0, #0]             
    add     r0, r3, r0, lsl #2       
M4013:
M4014:
    vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
  	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
  	sub				r4, r4, #16				
  	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
  	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
  	sub				r5, r5, #16       	
  	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
  	vld2.i32	  	{d0, d1, d2, d3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
  	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
  	vld1.i32		d17, [r5]!						@ in = *buf1--@
  	vld1.i32		d16, [r5]!						@ in = *buf1--@
  	vrev64.32		q14, q5
		vrev64.32		q9, q8
      
       	
  	sub				r5, r5, #16

  	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
		vswp				d28, d29
  	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
  	vadd.s32		q15, q14, q7					@ *out1-- = in + f1@
				
  	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
  	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
		vrev64.32	q1, q4
  	vst1.i32		{d30, d31}, [r4]
		vswp.i32		d2, d3
  	vst1.i32		{d10, d11}, [r1]!					
  	vst1.i32		{d2, d3}, [r9]
		
  	cmp     		r1, r9            	                   
    bcc     M4014    
                    
M4015:
	ldmia     sp!, {r4 - r11, pc}
@M4016:
@	.word       _AAD_srtdata70
@	.word       _AAD_srtdata71
@	.word       _AAD_srtdata68
@	.word       _AAD_srtdata69

	@ENDP  @ WinLongStart
	
	.globl  _AAD_srtidata509
		
	@AREA	.text, CODE, READONLY
        
_AAD_srtidata509: @PROC

	stmdb     sp!, {r4 - r11, lr}
M5005:
	ldr     r7, [r13, #36]
	add     r12, r0, #2048         			@ buf0 += (1024 >> 1)@       
    add     r8, r2, #2048 					@ out1  = out0 + 1024@
    mov     r5, r12                 		@ buf1  = buf0@
    add     r10, r1, #2048 					@ over1 = over0 + 1024@                       
    add     r4, r8, #2048					@ out1  = out0 + 1024@                     
    add     r9, r10, #2048                	@ over1 = over0 + 1024
	cmp     r7, #1        
	bne     M5007         
M5006:  
        ldr     r6, [sp, #44]                      
	@ldr     r6, M5016 
        mov     r0, #0                   
	@ldr     r0, M5016+4                  
	@ldr     r0, [r0, #0]                  
	add     r6, r6, r0, lsl #2            
	b       M5008
M5007:  
        ldr     r6, [sp, #40]                       
	@ldr     r6, M5016+8 
        mov     r0, #0          
	@ldr     r0, M5016+12          
	@ldr     r0, [r0, #0]           
	add     r6, r6, r0, lsl #2     
M5008:    
	cmp     r3, #1                     
	bne     M5010                      
M5009:
        ldr     r0, [sp, #44]
	@ldr     r0, M5016 
        mov     r3, #128        
	@ldr     r3, M5016+4       
	@ldr     r3, [r3, #4]       
	add     r0, r0, r3, lsl #2 
	b       M5011
M5010:
        ldr     r0, [sp, #40]  
	@ldr     r0, M5016+8
        mov     r3, #128
               
	@ldr     r3, M5016+12              
	@ldr     r3, [r3, #4]               
	add     r0, r0, r3, lsl #2         
M5011:      
	mov     r10, #448     	             
M5012:  
	vld2.i32	  	{d0, d1, d2, d3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
    sub				r4, r4, #16				
    vld1.i32		{d6, d7}, [r1]					@ in = *over0@
    sub				r5, r5, #16       	
	vshr.s32		q2, q2, #1						@ f1 = in >> 1@
	
	vst1.i32		{d6, d7}, [r2]!					@ *out0++

   	vld1.i32		d17, [r5]!						@ in = *buf1--@
    vld1.i32		d16, [r5]!						@ in = *buf1--@
    vrev64.32		q14, q2
		vrev64.32		q9, q8
     
       	
    sub				r5, r5, #16

    vld1.i32		{d14, d15}, [r9]				@ in = *over1@
		vswp.i32			d28, d29
  	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@ 
  	 	
  	vadd.s32		q15, q14, q7					@ *out1-- = in + f1@				
  	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
		vrev64.32		q1, q4
  	vst1.i32		{d30, d31}, [r4]
		vswp.i32		d2, d3
		
  	vst1.i32		{d10, d11}, [r1]!					
  	vst1.i32		{d2, d3}, [r9]		
	
	subs			r10, r10, #4	                 
	bne     		M5012                      
M5014:
    vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
  	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
  	sub				r4, r4, #16				
  	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
  	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
  	sub				r5, r5, #16       	
  	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
  	vld2.i32	  	{d0, d1, d2, d3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
  	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
  	vld1.i32		d17, [r5]!						@ in = *buf1--@
  	vld1.i32		d16, [r5]!						@ in = *buf1--@
  	vrev64.32		q14, q5
		vrev64.32		q9, q8
       	
  	sub				r5, r5, #16

  	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
		vswp.i32		d28, d29
  	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
  	vadd.s32		q15, q14, q7					@ *out1-- = in + f1@
				
  	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
  	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
  	vrev64.32		q1, q4
		vst1.i32		{d30, d31}, [r4]
  	@vrev64.32		q1, q0
		vswp.i32		d2, d3
		
  	vst1.i32		{d10, d11}, [r1]!					
  	vst1.i32		{d2, d3}, [r9]
		
  	cmp     		r1, r9            	      
	bcc     		M5014            
M5015:                         
	ldmia     		sp!, {r4 - r11, pc}
@M5016:
@	.word       _AAD_srtdata70
@	.word       _AAD_srtdata71
@	.word       _AAD_srtdata68
@	.word       _AAD_srtdata69

	@ENDP  @ AAD_srtidata509
	
	.globl	_AAD_srtidata508

	@AREA	.text, CODE, READONLY

_AAD_srtidata508: @PROC

	stmdb     sp!, {r4 - r11, lr}
M2893:
	ldr     r12, [r13, #36]
    	cmp     r12, #1        
   	bne     M2895                 
M2894:
        ldr     r14, [sp, #44]
   	@ldr     r14, L2796  
   	mov     r12, #0                
    	@ldr     r12, L2796+4                
   	@ldr     r12, [r12, #0]               
    	add     r6, r14, r12, lsl #2         
    	b       M2896                        
M2895:
        ldr     r14, [sp, #40]
        @ldr     r14, L2796+8 
    	mov     r12, #0      
    	@ldr     r12, L2796+12         
    	@ldr     r12, [r12, #0]         
    	add     r6, r14, r12, lsl #2  
M2896: 
    	cmp     r3, #1            
    	bne     M2898          
M2897:  
        ldr     r12, [sp, #44]                     
    	@ldr     r12, L2796   
        mov     r3, #0            
    	@ldr     r3, L2796+4              
    	@ldr     r3, [r3, #0]              
    	add     r3, r12, r3, lsl #2       
    	b       M2899                    
M2898: 
        ldr     r12, [sp, #40]  
    	@ldr     r12, L2796+8 
        mov     r3, #0               
    	@ldr     r3, L2796+12               
    	@ldr     r3, [r3, #0]               
    	add     r3, r12, r3, lsl #2       
M2899:
   	mov     r12, #448    
M2900:                      
    	vld1.i32		{d0, d1, d2, d3}, [r1]!
    	vld1.i32		{d4, d5, d6, d7}, [r1]!    
    	subs    		r12, r12, #16             
	vst1.i32		{d0, d1, d2, d3}, [r2]!
	vst1.i32		{d4, d5, d6, d7}, [r2]!            
    	bne     		M2900    
    
M2901: 
    	add     		r4, r2, #512        @ out1  = out0 + 128@            
    	add     		r9, r1, #512        @ over1 = over0 + 128@          
    	add     		r12, r0, #256       @ buf0 += 64@            
    	add     		r5, r0, #256        @ buf1  = buf0@             
     
M2902:
  	vld2.i32	  	{d0, d1, d2, d3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
	vld2.i32	  	{d0, d1, d2, d3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
       	
	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32	q14, q5
	vrev64.32		q9, q8
       	
	sub				r5, r5, #16

	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
	vswp				d28, d29
	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q15, q14, q7					@ *out1-- = in + f1@
				
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r4]
	vswp				d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
		
	cmp     		r1, r9 
    	bcc     		M2902         
     
M2903: 
    	mov     		r11, #0          
M2904:                        
    	add     		r2, r2, #256 					@ out0 += 64@    
    	add     		r1, r1, #256   					@ over0 += 64@  
    	add     		r4, r2, #512     				@ out1 = out0 + 128@
    	add     		r9, r1, #512    				@ over1 = over0 + 128@
    	sub			r7, r1, #512					@ over0 - 128
    	sub			r8, r9, #512					@ over1 - 128
    	add     		r12, r12, #256   				@ buf0 += 64@
    	sub     		r3, r3, #512 					@ wndCurr -= 128@
    	mov     		r5, r12              			@ buf1 = buf0@
                
M2905:   
	vld2.i32	  	{d0, d1, d2, d3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub				r9, r9, #16 
	sub				r8, r8, #16       
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	vld1.i32		{d6, d7}, [r1]					@ in = *over0@
	vld1.i32		{d24, d25}, [r7]!				@ in = *(over0 - 128)	
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   
	vadd.s32		q3, q3, q12						@ in = *over0 + *(over0 - 128)     	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
       	
	vld1.i32		{d24, d25}, [r8]				@ in = *(over1 - 128)@
       	
	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32	q5, q5
	vrev64.32		q9, q8
       	
	sub				r5, r5, #16

	vld1.i32		{d14, d15}, [r9]				@ in = *over1@
	vswp				d10, d11
	vadd.s32 		q7, q7, q12						@ in = *over0 + *(over0 - 128) 
	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q15, q7, q5						@ *out1-- = in + f1@
			
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r4]				@ *out1-- = in + f1@	
	vswp				d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
			       	
	cmp     		r1, r9            	              
    	bcc     		M2905                
M2906:                        
    	add     		r11, r11, #1     
    	cmp     		r11, #3        
    	blt     		M2904                    


M2907:
    	sub     		r1, r1, #3328       			@ over0 -= 832@
    	add     		r2, r2, #256         			@ out0 += 64@   
    	add     		r9, r1, #512            		@ over1 = over0 + 128@
    	add     		r12, r12, #256          		@ buf0 += 64@
    	sub     		r3, r3, #512            		@ wndCurr -= 128@
    	mov				r5, r12							@ buf1 = buf0@
M2908: 
	vld2.i32	  	{d0, d1, d2, d3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub				r9, r9, #16 
	add				r7, r1, #3072    
	add				r8, r1, #3584   
	vld1.i32		{d4, d5}, [r12]!    			@ in = *buf0++@
	vld1.i32		{d6, d7}, [r7]					@ in = *(over0 + 768)@
	vld1.i32		{d24, d25}, [r8]				@ in = *(over0 + 896)				
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   
	vadd.s32		q3, q3, q12						@ in = *(over0 + 768)+ *(over0+ 896)     	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
	
	add				r7, r9, #3072    
	sub				r8, r9, #512  
       	
	vsub.s32		q6, q3, q4						@ *out0++ = in - f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32		q5, q5
	vrev64.32		q9, q8
       
       	
	sub				r5, r5, #16

	vld1.i32		{d14, d15}, [r7]				@ in = *(over1 + 768)@
	vswp				d10, d11
	vst1.i32		{d12, d13}, [r2]!				@ *out0++ 
	vadd.s32		q15, q7, q5						@ *(over1 - 128) = in + f1@
			
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r8]				@ *(over1 - 128) = in + f1@	
	vswp					d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
			       	
	cmp     		r1, r9            	  
    	bcc     		M2908     
     
M2909:    
    	mov     		r11, #0    
M2910:  
    	add     		r1, r1, #256 			@ over0 += 64@
    	add    			r9, r1, #512			@ over1 = over0 + 128   
    	add     		r12, r12, #256 			@ buf0 += 64@
    	sub     		r3, r3, #512			@ wndCurr -= 128@
    	mov     		r5, r12   				@ buf1 = buf0@
M2911:
	vld2.i32	  	{d0, d1, d2, d3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub			r9, r9, #16 
	sub			r7, r1, #512    
	vld1.i32		{d4, d5},	[r12]!    			@ in = *buf0++@
	vld1.i32		{d6, d7}, [r7]					@ in = *(over0 - 128)@
	sub			r8, r9, #512 
			
	vqdmulh.s32		q4, q2, q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   	
   	
	vqdmulh.s32		q5, q2, q1    					@ f1 = MULHIGH(w1, in)@
	
	vld1.i32		{d14, d15}, [r8]				@ in = *(over1 - 128)@
       	
	vsub.s32		q6, q3, q4						@ *(over0 - 128) -= f0@
	vld1.i32		d17, [r5]!						@ in = *buf1--@
	vld1.i32		d16, [r5]!						@ in = *buf1--@
	vrev64.32		q5, q5
	vrev64.32		q9, q8
       	
	sub				r5, r5, #16

	vswp				d10, d11
	vst1.i32		{d12, d13}, [r7]				@ *(over0 - 128) -= f0@ 
	vadd.s32		q15, q7, q5						@ *(over1 - 128)+= f1@
			
	vqdmulh.s32		q4, q9, q0  					@ *over1-- = MULHIGH(w0, in)@
	vqdmulh.s32		q5, q9, q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	vrev64.32		q1, q4
	vst1.i32		{d30, d31}, [r8]				@ *(over1 - 128)+= f1@	
	vswp					d2, d3
		
	vst1.i32		{d10, d11}, [r1]!					
	vst1.i32		{d2, d3}, [r9]
			       	
	cmp     		r1, r9            	  
    	bcc     		M2911              
M2912:                       
    	add     		r11, r11, #1   
    	cmp    			r11, #3        
    	blt     		M2910       
     
M2913:
    	mov     		r0, #0 
    	mov    			r2, #448       
    	add     		r1, r1, #256   
    	vdup.s32		q15, r0    
M2914:
    	vst1.i32		{d30, d31}, [r1]!
    	vst1.i32		{d30, d31}, [r1]!
    	vst1.i32		{d30, d31}, [r1]!
    	vst1.i32		{d30, d31}, [r1]!
	subs			r2, r2, #16  
    	bne     		M2914     

	ldmia    		sp!, {r4 - r11, pc}
@L2796:
@	.word       		_AAD_srtdata70
@	.word       _AAD_srtdata71
@	.word       _AAD_srtdata68
@	.word       _AAD_srtdata69

	@ENDP  @ AAD_srtidata508
	
	@.END
	
