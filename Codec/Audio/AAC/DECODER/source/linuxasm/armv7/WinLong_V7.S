
	@AREA	.text, CODE, READONLY
	.section .text

	@.extern    AAD_srtdata68
	@.extern    AAD_srtdata69
	@.extern    AAD_srtdata70
	@.extern    AAD_srtdata71
	
	.global	AAD_srtidata511

AAD_srtidata511: @PROC
	stmdb   sp!, {r4 - r11, lr}
M3005:
	ldr     r7, [sp, #36]                
	add     r12, r0, #2048         			@ buf0 += (1024 >> 1)@       
	add     r8, r2, #2048 					@ out1  = out0 + 1024@
	@VMOV		D7, r9, r10
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
	VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32	Q5, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
	@VREV64.32		Q5, Q15    
	VSWP				D10, D11
	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q15, Q7, Q5						@ *out1-- = in + f1@
				
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r4]				@ *out1-- = in + f1@	
	@VREV64.32		Q1, Q0
	VSWP				D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
			       	
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
	VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
	VLD2.I32	  	{D0, D1, D2, D3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32		Q14, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
	@VREV64.32		Q14, Q15
	VSWP				D28, D29
	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q15, Q14, Q7					@ *out1-- = in + f1@
				
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r4]
	@VREV64.32		Q1, Q0
	VSWP					D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
		
	cmp     		r1, r9            	
	bcc     		M3016   
M3018:
	ldmia   sp!, {r4 - r11, pc}
@M3019:                      
@	.word     AAD_srtdata70
@	.word     AAD_srtdata71
@	.word     AAD_srtdata68
@	.word     AAD_srtdata69
				
	@ENDP  @ AAD_srtidata511
		
	.global	WinLongStart
		
	@AREA	.text, CODE, READONLY

WinLongStart: @PROC

	stmdb     sp!, {r4 - r11, lr}
M4005:	
	ldr     r7, [sp, #36]                
    add     r12, r0, #2048         			@ buf0 += (1024 >> 1)@       
    mov		r11, #0
    add     r8, r2, #2048 					@ out1  = out0 + 1024@
    VDUP.S32  Q15, r11
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
		VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
		VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
    sub				r4, r4, #16				
    VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
    VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
    sub				r5, r5, #16       	
    VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
	
   	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
   	VLD1.I32		D17, [r5]!						@ in = *buf1--@
    VLD1.I32		D16, [r5]!						@ in = *buf1--@
    @VMOV			D21, d10
    VREV64.32	Q0, Q5
		VREV64.32		Q9, Q8
    @VMOV			D20, D11       
       	
    sub				r5, r5, #16

    VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
    @VREV64.32		Q0, Q10
		VSWP				D0, D1
    VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q1, Q0, Q7						@ *out1-- = in + f1@
	
	VSHR.S32		Q9, Q9, #1
				
	VST1.I32		{D30, D31}, [r9]				@ *over1-- = 0@
	VST1.I32		{D2, D3}, [r4]					@ *out1-- = in + f1@
	VST1.I32		{D18, D19}, [r1]!				@ *over0++ = in >> 1@		
	
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
    VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
  	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
  	sub				r4, r4, #16				
  	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
  	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
  	sub				r5, r5, #16       	
  	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
  	VLD2.I32	  	{D0, D1, D2, D3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
  	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
  	VLD1.I32		D17, [r5]!						@ in = *buf1--@
  	VLD1.I32		D16, [r5]!						@ in = *buf1--@
  	@VMOV			D31, d10
  	VREV64.32		Q14, Q5
		VREV64.32		Q9, Q8
  	@VMOV			D30, D11       
       	
  	sub				r5, r5, #16

  	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
  	@VREV64.32		Q14, Q15      	
		VSWP				D28, D29
  	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
  	VADD.S32		Q15, Q14, Q7					@ *out1-- = in + f1@
				
  	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
  	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
  	@VMOV			D1, D8
  	@VMOV			D0, D9
		VREV64.32	Q1, Q4
  	VST1.I32		{D30, D31}, [r4]
  	@VREV64.32		Q1, Q0
		VSWP.I32		D2, D3
  	VST1.I32		{d10, D11}, [r1]!					
  	VST1.I32		{D2, D3}, [r9]
		
  	cmp     		r1, r9            	                   
    bcc     M4014    
                    
M4015:
	ldmia     sp!, {r4 - r11, pc}
@M4016:
@	.word       AAD_srtdata70
@	.word       AAD_srtdata71
@	.word       AAD_srtdata68
@	.word       AAD_srtdata69

	@ENDP  @ WinLongStart
	
	.global	AAD_srtidata509
		
	@AREA	.text, CODE, READONLY
        
AAD_srtidata509: @PROC

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
	VLD2.I32	  	{D0, D1, D2, D3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
    sub				r4, r4, #16				
    VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
    sub				r5, r5, #16       	
	VSHR.S32		Q2, Q2, #1						@ f1 = in >> 1@
	
	VST1.I32		{D6, D7}, [r2]!					@ *out0++

   	VLD1.I32		D17, [r5]!						@ in = *buf1--@
    VLD1.I32		D16, [r5]!						@ in = *buf1--@
    @VMOV			D21, D4
    VREV64.32		Q14, Q2
		VREV64.32		Q9, Q8
    @VMOV			D20, D5       
       	
    sub				r5, r5, #16

    VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
  	@VREV64.32		Q14, Q10
		VSWP.I32			D28, D29
  	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@ 
  	 	
  	VADD.S32		Q15, Q14, Q7					@ *out1-- = in + f1@				
  	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
  	@VMOV			D1, D8
  	@VMOV			D0, D9
		VREV64.32		Q1, Q4
  	VST1.I32		{D30, D31}, [r4]
  	@VREV64.32		Q1, Q0
		VSWP.I32		D2, D3
		
  	VST1.I32		{d10, D11}, [r1]!					
  	VST1.I32		{D2, D3}, [r9]		
	
	subs			r10, r10, #4	                 
	bne     		M5012                      
M5014:
    VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
  	sub				r9, r9, #16        
  	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
  	sub				r4, r4, #16				
  	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
  	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
  	sub				r5, r5, #16       	
  	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
  	VLD2.I32	  	{D0, D1, D2, D3}, [r0]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
       	
  	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
  	VLD1.I32		D17, [r5]!						@ in = *buf1--@
  	VLD1.I32		D16, [r5]!						@ in = *buf1--@
  	@VMOV			D31, d10
  	VREV64.32		Q14, Q5
		VREV64.32		Q9, Q8
  	@VMOV			D30, D11       
       	
  	sub				r5, r5, #16

  	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
  	@VREV64.32		Q14, Q15      	
		VSWP.I32		D28, D29
  	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
  	VADD.S32		Q15, Q14, Q7					@ *out1-- = in + f1@
				
  	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
  	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
  	@VMOV			D1, D8
  	@VMOV			D0, D9
  	VREV64.32		Q1, Q4
		VST1.I32		{D30, D31}, [r4]
  	@VREV64.32		Q1, Q0
		VSWP.I32		D2, D3
		
  	VST1.I32		{d10, D11}, [r1]!					
  	VST1.I32		{D2, D3}, [r9]
		
  	cmp     		r1, r9            	      
	bcc     		M5014            
M5015:                         
	ldmia     		sp!, {r4 - r11, pc}
@M5016:
@	.word       AAD_srtdata70
@	.word       AAD_srtdata71
@	.word       AAD_srtdata68
@	.word       AAD_srtdata69

	@ENDP  @ AAD_srtidata509
	
	.global	AAD_srtidata508

	@AREA	.text, CODE, READONLY

AAD_srtidata508: @PROC

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
    VLD1.I32		{D0, D1, D2, D3}, [r1]!
    VLD1.I32		{D4, D5, D6, D7}, [r1]!    
    subs    		r12, r12, #16             
	VST1.I32		{D0, D1, D2, D3}, [r2]!
	VST1.I32		{D4, D5, D6, D7}, [r2]!            
    bne     		M2900    
    
M2901: 
    add     		r4, r2, #512        @ out1  = out0 + 128@            
    add     		r9, r1, #512        @ over1 = over0 + 128@          
    add     		r12, r0, #256       @ buf0 += 64@            
    add     		r5, r0, #256        @ buf1  = buf0@             
     
M2902:
   	VLD2.I32	  	{D0, D1, D2, D3}, [r6]!			@ w0 = *wndPrev++@ w1 = *wndPrev++@
	sub				r9, r9, #16        
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16       	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
	VLD2.I32	  	{D0, D1, D2, D3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
       	
	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32	Q14, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
	@VREV64.32		Q14, Q15      	
	VSWP				D28, D29
	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q15, Q14, Q7					@ *out1-- = in + f1@
				
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r4]
	@VREV64.32		Q1, Q0
	VSWP				D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
		
	cmp     		r1, r9 
    bcc     		M2902         
     
M2903: 
    mov     		r11, #0          
M2904:                        
    add     		r2, r2, #256 					@ out0 += 64@    
    add     		r1, r1, #256   					@ over0 += 64@  
    add     		r4, r2, #512     				@ out1 = out0 + 128@
    add     		r9, r1, #512    				@ over1 = over0 + 128@
    sub				r7, r1, #512					@ over0 - 128
    sub				r8, r9, #512					@ over1 - 128
    add     		r12, r12, #256   				@ buf0 += 64@
    sub     		r3, r3, #512 					@ wndCurr -= 128@
    mov     		r5, r12              			@ buf1 = buf0@
                
M2905:   
	VLD2.I32	  	{D0, D1, D2, D3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub				r9, r9, #16 
	sub				r8, r8, #16       
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
	sub				r4, r4, #16				
	VLD1.I32		{D6, D7}, [r1]					@ in = *over0@
	VLD1.I32		{D24, D25}, [r7]!				@ in = *(over0 - 128)	
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   
	VADD.S32		Q3, Q3, Q12						@ in = *over0 + *(over0 - 128)     	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
       	
	VLD1.I32		{D24, D25}, [r8]				@ in = *(over1 - 128)@
       	
	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32	Q5, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	VLD1.I32		{D14, D15}, [r9]				@ in = *over1@
	@VREV64.32	  Q5, Q15   
	VSWP				D10, D11
	VADD.S32 		Q7, Q7, Q12						@ in = *over0 + *(over0 - 128) 
	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q15, Q7, Q5						@ *out1-- = in + f1@
			
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r4]				@ *out1-- = in + f1@	
	@VREV64.32		Q1, Q0
	VSWP				D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
			       	
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
	VLD2.I32	  	{D0, D1, D2, D3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub				r9, r9, #16 
	add				r7, r1, #3072    
	add				r8, r1, #3584   
	VLD1.I32		{D4, D5}, [r12]!    			@ in = *buf0++@
	VLD1.I32		{D6, D7}, [r7]					@ in = *(over0 + 768)@
	VLD1.I32		{D24, D25}, [r8]				@ in = *(over0 + 896)				
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   
	VADD.S32		Q3, Q3, Q12						@ in = *(over0 + 768)+ *(over0+ 896)     	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
	
	add				r7, r9, #3072    
	sub				r8, r9, #512  
       	
	VSUB.S32		Q6, Q3, Q4						@ *out0++ = in - f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32		Q5, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	VLD1.I32		{D14, D15}, [r7]				@ in = *(over1 + 768)@
	@VREV64.32		Q5, Q15   
	VSWP				D10, D11
	VST1.I32		{D12, D13}, [r2]!				@ *out0++ 
	VADD.S32		Q15, Q7, Q5						@ *(over1 - 128) = in + f1@
			
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r8]				@ *(over1 - 128) = in + f1@	
	@VREV64.32		Q1, Q0
	VSWP					D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
			       	
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
	VLD2.I32	  	{D0, D1, D2, D3}, [r3]!			@ w0 = *wndCurr++@ w1 = *wndCurr++@
	sub				r9, r9, #16 
	sub				r7, r1, #512    
	VLD1.I32		{D4, D5},	[r12]!    			@ in = *buf0++@
	VLD1.I32		{D6, D7}, [r7]					@ in = *(over0 - 128)@
	sub				r8, r9, #512 
			
	VQDMULH.S32		Q4, Q2, Q0  					@ f0 = MULHIGH(w0, in)@
	sub				r5, r5, #16   	
   	
	VQDMULH.S32		Q5, Q2, Q1    					@ f1 = MULHIGH(w1, in)@
	
	VLD1.I32		{D14, D15}, [r8]				@ in = *(over1 - 128)@
       	
	VSUB.S32		Q6, Q3, Q4						@ *(over0 - 128) -= f0@
	VLD1.I32		D17, [r5]!						@ in = *buf1--@
	VLD1.I32		D16, [r5]!						@ in = *buf1--@
	@VMOV			D31, d10
	VREV64.32		Q5, Q5
	VREV64.32		Q9, Q8
	@VMOV			D30, D11       
       	
	sub				r5, r5, #16

	@VREV64.32		Q5, Q15   
	VSWP				D10, D11
	VST1.I32		{D12, D13}, [r7]				@ *(over0 - 128) -= f0@ 
	VADD.S32		Q15, Q7, Q5						@ *(over1 - 128)+= f1@
			
	VQDMULH.S32		Q4, Q9, Q0  					@ *over1-- = MULHIGH(w0, in)@
	VQDMULH.S32		Q5, Q9, Q1    					@ *over0++ = MULHIGH(w1, in)@
       	
	@VMOV			D1, D8
	@VMOV			D0, D9
	VREV64.32		Q1, Q4
	VST1.I32		{D30, D31}, [r8]				@ *(over1 - 128)+= f1@	
	@VREV64.32		Q1, Q0
	VSWP					D2, D3
		
	VST1.I32		{d10, D11}, [r1]!					
	VST1.I32		{D2, D3}, [r9]
			       	
	cmp     		r1, r9            	  
    bcc     		M2911              
M2912:                       
    add     	r11, r11, #1   
    cmp    		r11, #3        
    blt     	M2910       
     
M2913:
    mov     	r0, #0 
    mov    		r2, #448       
    add     	r1, r1, #256   
    VDUP.S32	Q15, r0    
M2914:
    VST1.I32	{D30, D31}, [r1]!
    VST1.I32	{D30, D31}, [r1]!
    VST1.I32	{D30, D31}, [r1]!
    VST1.I32	{D30, D31}, [r1]!
	subs		r2, r2, #16  
    bne     	M2914     

	ldmia     sp!, {r4 - r11, pc}
@L2796:
@	.word       AAD_srtdata70
@	.word       AAD_srtdata71
@	.word       AAD_srtdata68
@	.word       AAD_srtdata69

	@ENDP  @ AAD_srtidata508
	
	.END
	
