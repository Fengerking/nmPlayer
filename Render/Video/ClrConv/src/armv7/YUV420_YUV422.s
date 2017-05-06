;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2009		            *
;	written by John							 	                        *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY
	
	EXPORT	|YUV420_YUYV422_32|
	EXPORT	|YUV420_UYVY422_32|	
	EXPORT	|YUV420_YUYV422_16|
	EXPORT	|YUV420_UYVY422_16|
		
YUV420_YUYV422_32

	stmdb	sp!, {r4 - r11, lr}
	ldr	r10, [sp, #36]		;YStride
	ldr	r11, [sp, #40]		;UVStride		
	ldr	r12, [sp, #44]		;w	
	ldr	r14, [sp, #48]		;h			
YUYV422_Height_loop
	ldr	r7, [sp, #52]		;OutStride
	mov	r4, r0			;Des
	mov	r6, r1			;Ybuf	
	add	r5, r0, r7   	;Des1 = Dst + OutStride;
	add	r7, r1, r10		;Ybuf1 = Ybuf + YStride;		
	mov	r8, r2			;Ubuf
	mov	r9, r3			;Vbuf
YUYV422_Width_loop

	VLD1.64  {q4},[r8]!		;u
	VLD1.64  {q5},[r9]!		;v 
	VLD1.64  {q0, q1},[r6]!	;y0
	VLD1.64  {q2, q3},[r7]!	;y1	
			
	VZIP.8  q4, q5
	
	VSHL.s32  q6, q4, #0
	VSHL.s32  q7, q5, #0
	
	VZIP.8  q0, q4
	VZIP.8  q1, q5
	VZIP.8  q2, q6
	VZIP.8  q3, q7			
	
	VST1.64  {q0}, [r4]!
	VST1.64  {q4}, [r4]!	
	VST1.64  {q1}, [r4]!
	VST1.64  {q5}, [r4]!	

	VST1.64  {q2}, [r5]!
	VST1.64  {q6}, [r5]!	
	VST1.64  {q3}, [r5]!	
	VST1.64  {q7}, [r5]!				
					
	subs        r12, r12, #32	;doublewidth  		
	bgt         YUYV422_Width_loop
	
	ldr	r7, [sp, #52]		;OutStride	
	ldr	r12, [sp, #44]		;w	
	subs    r14, r14, #2		
	add	r1, r1, r10, lsl #1	;Y += YStride * 2	
	add	r0, r0, r7, lsl #1	;Dst += OutStride * 2
	add	r2, r2, r11		;U += UVStride
	add	r3, r3, r11		;U += UVStride			
	bne     YUYV422_Height_loop
	ldmia       sp!, {r4 - r11, pc}
	
	
YUV420_YUYV422_16
	stmdb	sp!, {r4 - r11, lr}
	ldr	r10, [sp, #36]		;YStride
	ldr	r11, [sp, #40]		;UVStride		
	ldr	r12, [sp, #44]		;w	
	ldr	r14, [sp, #48]		;h			
YUYV422_Height_loop_16
	ldr	r7, [sp, #52]		;OutStride
	mov	r4, r0			;Des
	mov	r6, r1			;Ybuf	
	add	r5, r0, r7  	;Des1 = Dst + OutStride;
	add	r7, r1, r10		;Ybuf1 = Ybuf + YStride;		
	mov	r8, r2			;Ubuf
	mov	r9, r3			;Vbuf
YUYV422_Width_loop_16

	VLD1.64  {d8},[r8]!		;u
	VLD1.64  {d10},[r9]!		;v 
	VLD1.64  {q0},[r6]!	;y0
	VLD1.64  {q2},[r7]!	;y1	
			
	VZIP.8  d8, d10
	
	VSHL.s32  d9, d8, #0
	VSHL.s32  d11, d10, #0
	
	VZIP.8  d0, d8
	VZIP.8  d1, d10
	VZIP.8  d4, d9
	VZIP.8  d5, d11			
	
	VST1.64  {d0}, [r4]!
	VST1.64  {d8}, [r4]!	
	VST1.64  {d1}, [r4]!
	VST1.64  {d10}, [r4]!	

	VST1.64  {d4}, [r5]!
	VST1.64  {d9}, [r5]!	
	VST1.64  {d5}, [r5]!	
	VST1.64  {d11}, [r5]!				
					
	subs        r12, r12, #16	;doublewidth  		
	bgt         YUYV422_Width_loop_16
	
	ldr	r7, [sp, #52]		;OutStride	
	ldr	r12, [sp, #44]		;w	
	subs    r14, r14, #2		
	add	r1, r1, r10, lsl #1	;Y += YStride * 2	
	add	r0, r0, r7, lsl #1	;Dst += OutStride * 2
	add	r2, r2, r11		;U += UVStride
	add	r3, r3, r11		;U += UVStride			
	bne     YUYV422_Height_loop_16
	ldmia       sp!, {r4 - r11, pc}
			
YUV420_UYVY422_32

	stmdb	sp!, {r4 - r11, lr}
	ldr	r10, [sp, #36]		;YStride
	ldr	r11, [sp, #40]		;UVStride		
	ldr	r12, [sp, #44]		;w	
	ldr	r14, [sp, #48]		;h			
UYVY422_Height_loop
	ldr	r7, [sp, #52]		;OutStride
	mov	r4, r0			;Des
	mov	r6, r1			;Ybuf	
	add	r5, r0, r7  	;Des1 = Dst + OutStride;
	add	r7, r1, r10		;Ybuf1 = Ybuf + YStride;		
	mov	r8, r2			;Ubuf
	mov	r9, r3			;Vbuf
UYVY422_Width_loop

	VLD1.64  {q4},[r8]!		;u
	VLD1.64  {q5},[r9]!		;v 
	VLD1.64  {q0, q1},[r6]!	;y0
	VLD1.64  {q2, q3},[r7]!	;y1	
			
	VZIP.8  q4, q5
	
	VSHL.s32  q6, q4, #0
	VSHL.s32  q7, q5, #0
	
	VZIP.8  q4, q0
	VZIP.8  q5, q1
	VZIP.8  q6, q2
	VZIP.8  q7, q3			
	
	VST1.64  {q4}, [r4]!
	VST1.64  {q0}, [r4]!	
	VST1.64  {q5}, [r4]!
	VST1.64  {q1}, [r4]!	

	VST1.64  {q6}, [r5]!
	VST1.64  {q2}, [r5]!	
	VST1.64  {q7}, [r5]!	
	VST1.64  {q3}, [r5]!				
					
	subs        r12, r12, #32	;doublewidth  		
	bgt         UYVY422_Width_loop
	
	ldr	r7, [sp, #52]		;OutStride	
	ldr	r12, [sp, #44]		;w	
	subs    r14, r14, #2		
	add	r1, r1, r10, lsl #1	;Dst += YStride * 2	
	add	r0, r0, r7, lsl #1	;Dst += OutStride * 2
	add	r2, r2, r11		;U += UVStride
	add	r3, r3, r11		;U += UVStride			
	bne     UYVY422_Height_loop
	ldmia       sp!, {r4 - r11, pc}			


YUV420_UYVY422_16
	stmdb	sp!, {r4 - r11, lr}
	ldr	r10, [sp, #36]		;YStride
	ldr	r11, [sp, #40]		;UVStride		
	ldr	r12, [sp, #44]		;w	
	ldr	r14, [sp, #48]		;h			
UYVY422_Height_loop_16
	ldr	r7, [sp, #52]		;OutStride
	mov	r4, r0			;Des
	mov	r6, r1			;Ybuf	
	add	r5, r0, r7  	;Des1 = Dst + OutStride;
	add	r7, r1, r10		;Ybuf1 = Ybuf + YStride;		
	mov	r8, r2			;Ubuf
	mov	r9, r3			;Vbuf
UYVY422_Width_loop_16

	VLD1.64  {d8},[r8]!		;u
	VLD1.64  {d10},[r9]!		;v 
	VLD1.64  {q0},[r6]!	;y0
	VLD1.64  {q2},[r7]!	;y1	
			
	VZIP.8  d8, d10
	
	VSHL.s32  d9, d8, #0
	VSHL.s32  d11, d10, #0
	
	VZIP.8  d8, d0
	VZIP.8  d10, d1
	VZIP.8  d9, d4
	VZIP.8  d11, d5			
	
	VST1.64  {d8}, [r4]!
	VST1.64  {d0}, [r4]!	
	VST1.64  {d10}, [r4]!
	VST1.64  {d1}, [r4]!	

	VST1.64  {d9}, [r5]!
	VST1.64  {d4}, [r5]!	
	VST1.64  {d11}, [r5]!	
	VST1.64  {d5}, [r5]!				
					
	subs        r12, r12, #16	;doublewidth  		
	bgt         UYVY422_Width_loop_16
	
	ldr	r7, [sp, #52]		;OutStride	
	ldr	r12, [sp, #44]		;w	
	subs    r14, r14, #2		
	add	r1, r1, r10, lsl #1	;Dst += YStride * 2	
	add	r0, r0, r7, lsl #1	;Dst += OutStride * 2
	add	r2, r2, r11		;U += UVStride
	add	r3, r3, r11		;U += UVStride			
	bne     UYVY422_Height_loop_16
	ldmia       sp!, {r4 - r11, pc}		
	
	ENDP
	end

	
