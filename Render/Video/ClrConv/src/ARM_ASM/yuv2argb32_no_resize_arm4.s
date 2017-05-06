;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2010		            *
;	written by Rock							 	                        *
;***********************************************************************/	
	AREA	|.rdata|, DATA, READONLY
	EXPORT	|cc_argb32_mb_16x16_r90_arm|
	IMPORT	|ccClip255|	
	AREA	|.text|, CODE, READONLY

  macro 
  cc_argb32_mb_16x16_r90_arm_num_count $Num
		ldr	r9, =0x645a1				;r9 = ConstU2
		ldr	r8, =0x2045a1				;r8 = ConstU1				
		ldrb r6, [r1, #$Num]			;r6 = psrc_u[num]		
		ldrb r7, [r2, #$Num]			;r7 = psrc_v[num]					
		ldrb r10,[r0, #($Num*2 )]		;r10 = psrc_y[2*num]
		ldr	r12, =0x129fbe				;r12 = ConstY						
		sub	r6, r6, #128				;r6 = a0
		mul	r9, r6, r9					;r9 = a0 *ConstU2
		sub	r10, r10, #16
		mul	r10, r12, r10				;*********r10 = a3 = (psrc_y[2*nu] - 16)*ConstY;
		ldr	r11, =0xD020C				;r11 = ConstV2					
		sub	r7, r7, #128				;r7 = a1			
		mul	r6, r8, r6					;*********r6 = a2 = (a0 * ConstU1);		
		ldr	r8, =0x198937				;r8 = ConstV1
		mla	r11, r7, r11, r9			;*********r11 = a0 = (a1 * ConstV2 + a0 *ConstU2);
		add	r9, r10, r6					;*********r9 = (a3 + a2)
		mul	r7, r8, r7					;*********r7 = a1 = (a1 * ConstV1);


		ldrb r9, [r14, +r9, asr #20]		;r9 = ccClip255[((a3 + a2)>>20)]
		add	 r8, r10, r7
		sub	 r10,r10, r11
		ldrb r10,[r14, +r10, asr #20]		;r10 = a5  = (ccClip255[((a3 - a0)>>20)]);
		ldrb r8, [r14, +r8, asr #20]		;r8 = ccClip255[((a3 + a1)>>20)]						
        orr  r10, r8, r10, lsl #8           ;r10 = (a5<<8)|a3	        	    		
        orr  r10, r10, r9, lsl #16          ;********r10 = a4 = (a4<<16)|(a5<<8)|a3;
        
        mov r9, #0xff
        orr r10, r10, r9, lsl #24 
        
        add	 r9, r0, r3						;r8 = psrc_y+in_stride
		ldrb r9, [r9, #($Num*2 )]			;r10 = psrc_y[in_stride+2*num]
        
        if $Num>0
			mov r8,  r4
			add r8,  r8, #4
        	str	r10, [r8, r5]				;*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;	
		else
        	str	r10, [r4, #4]			;*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;	        	
		endif
		
        sub	r9, r9, #16
		mul	r9, r12, r9						;r9 = a3 = (psrc_y[(2*nu + 1)] - 16)*ConstY;
		
		add	 r8, r9, r6	
		ldrb r8, [r14, +r8, asr #20]		;r8 = ccClip255[((a3 + a2)>>20)]			
		add	 r12, r9, r7
		ldrb r12,[r14, +r12,asr #20]		;r12 = ccClip255[((a3 + a1)>>20)]
		sub	 r9, r9, r11
		ldrb r9, [r14, +r9, asr #20]		;r9 = a3  = (ccClip255[((a3 - a0)>>20)]);		
    	orr  r9, r9, r8, lsl #8            ;********r9 = a3 |= (a5<<6);
    	orr  r8, r12, r9,lsl #8
    	
    	mov r9,  #0xff
    	orr r8,  r8,  r9, lsl #24
			
		ldrb r10, [r0, #(($Num*2)+1)]		;r7 = psrc_y[(2*num + 1)]	    	
		ldr	 r12, =0x129fbe					;r12 = ConstY	
			
		if $Num>0
        	str	r8, [r4, r5]!				;*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;	
		else
        	str	r8, [r4]					;*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;	        	
		endif
		
        sub	r10, r10, #16	
		mul	r10, r12, r10					;*********r10 = a3 = (psrc_y[in_stride+2*nu] - 16)*ConstY;		
		add	r9, r10, r6						;*********r9 = (a3 + a2)	
		
		ldrb r9, [r14, +r9, asr #20]		;r9 = ccClip255[((a3 + a2)>>20)]
		add	 r8, r10, r7
		sub  r10,r10, r11
		ldrb r10,[r14, +r10,asr #20]		;r10 = a5  = (ccClip255[((a3 - a0)>>20)]);
		ldrb r8, [r14, +r8, asr #20]		;r8 = ccClip255[((a3 + a1)>>20)]							
        orr  r10, r8, r10, lsl #8			;r10 = (a5<<8)|a3     	
        orr  r10, r10, r9, lsl #16          ;********r10 = a4 = (a4<<16)|(a5<<8)|a3;
        
        mov r9,  #0xff
    	orr r10,  r10,  r9, lsl #24
    	
        add  r9, r0, r3						;r8 = psrc_y+in_stride        	
		ldrb r9, [r9, #(($Num*2)+1)]		;r7 = psrc_y[(in_stride+2*num + 1)]   
        mov  r8, r4
        add  r8, r8, #4
        str	 r10, [r8, r5]					;*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;	
        sub  r9, r9, #16
		mul  r9, r12, r9					;r9 = a3 = (psrc_y[(2*nu + 1)] - 16)*ConstY;
		
		add	 r8, r9, r6	
		ldrb r8, [r14, +r8, asr #20]		;r8 = ccClip255[((a3 + a2)>>20)]			
		add  r12, r9, r7
		ldrb r12,[r14, +r12,asr #20]		;r12 = ccClip255[((a3 + a1)>>20)]
		sub	 r9, r9, r11
		ldrb r9, [r14, +r9, asr #20]		;r9 = a3  = (ccClip255[((a3 - a0)>>20)]);		
    	orr  r9, r9, r8, lsl #8             ;********r9 = a3 |= (a5<<8);
    	orr  r8, r12, r9,  lsl #8
    	
    	mov r9,  #0xff
    	orr r8,  r8,  r9, lsl #24
    	
    	str	 r8, [r4,r5]!					;*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;						
  mend  

|cc_argb32_mb_16x16_r90_arm| PROC
	stmdb	sp!, {r4-r11, lr}				; save regs used
	sub	sp, sp, #32							; sp - 32
	ldr	r4, [sp, #68]						; r4 = out_buf
	ldr	r5, [sp, #72]						; r5 = out_stride				
	ldr	r14, ccClip255_DATA_r
	ldr	r14, [r14]							; r14 = ccClip255		
	mov	r12, #8								; a6 = 8
	str	r12, [sp, #4]	
ARGB32_BIG_WHILE_r90
	cc_argb32_mb_16x16_r90_arm_num_count 0
	cc_argb32_mb_16x16_r90_arm_num_count 1
	cc_argb32_mb_16x16_r90_arm_num_count 2
	cc_argb32_mb_16x16_r90_arm_num_count 3
	cc_argb32_mb_16x16_r90_arm_num_count 4
	cc_argb32_mb_16x16_r90_arm_num_count 5
	cc_argb32_mb_16x16_r90_arm_num_count 6
	cc_argb32_mb_16x16_r90_arm_num_count 7	
						
	ldr		r6, [sp, #84]					;uin_stride
	ldr		r7, [sp, #88]					;vin_stride
	ldr		r12,[sp, #4]
	add		r4, r4, r5
	sub		r4, r4, #8
	sub		r4, r4, r5, asl	#4
	subs	r12, r12, #1
	strne	r12, [sp, #4]
	add		r0, r0, r3, asl	#1
	add		r1, r1, r6
	add		r2, r2, r7		

	bne		ARGB32_BIG_WHILE_r90

    add     sp, sp, #32
	ldmia	sp!, {r4-r11, pc}		; restore and return 

	ENDP  ;	
	
|ccClip255_DATA_r|
	DCD       |ccClip255|
		
END