;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2009		            *
;	written by John							 	                                    *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY
	EXPORT |voyuv420torgb24_armv7|

|voyuv420torgb24_armv7| PROC
;void voyuv420torgb24_arm(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, 
;			  VO_S32 in_stridey,VO_U8 *out_buf, const VO_S32 out_stride,
;       VO_S32 width, VO_S32 height, VO_S32 in_strideu, VO_S32 in_stridev)	
;{
;	VO_S32 a0, a1, a2, a3, a4, a5, a7, a33, a77;
;	VO_U32 i, j;
;	for(i = 0; i < height; i += 2){
;		for(j = 0; j < width; j += 2){
;			a0 = psrc_u[0] - 128;
;			a1 = psrc_v[0] - 128;
;			a2 = (a0 * ConstU1);
;			a0 = (a1 * ConstV2 + a0 *ConstU2);
;			a1 = (a1 * ConstV1);
;
;			a3 = (psrc_y[0] - 16)*ConstY;
;			a33 = (psrc_y[1] - 16)*ConstY;
;
;			a7 = (psrc_y[in_stride+0] - 16)*ConstY;
;			a77 = (psrc_y[in_stride+1] - 16)*ConstY;
;
;			a4  = SAT((a3 + a1)>>12);
;			a5  = SAT((a3 - a0)>>12);
;			a3  = SAT((a3 + a2)>>12);
;			*(out_buf) = a3;
;			*(out_buf+1) = a5;
;			*(out_buf+2) = a4;
;
;			out_buf += 6;
;			psrc_y += 2;
;			psrc_u++;
;			psrc_v++;
;
;		}
;		psrc_y -= width;
;		psrc_y += (in_stride<<1);
;		psrc_u -= (width>>1);
;
;		psrc_u += in_strideu;
;		psrc_v -= (width>>1);
;		psrc_v += in_stridev;
;
;		out_buf -= (width*3);
;		out_buf += (out_stride<<1);
;	}
;}
;		r0 = psrc_y
;		r1 = psrc_u
;		r2 = psrc_v
;		r3 = in_stridey
		stmdb	sp!, {r4-r11, lr}		; save regs used
		sub	sp, sp, #64			; sp - 32				
		ldr	r10, =ConstYUV
		ldr	r6, [sp, #100]			;out_buf
		ldr	r7, [sp, #104]			;out_stride
		ldr	r8, [sp, #108]			;width
		ldr	r9, [sp, #112]			;height
		ldr	r4, [sp, #116]			;in_strideu
		ldr	r5, [sp, #120]			;in_stridev				
		vld1.s32 {q0}, [r10]		;U2 = D0.S16[0]	V2 = D0.S16[1]
						;U1 = D0.S16[2] V1 = D0.S16[3]
						;Y = D1.S16[0]  Y = D1.S16[1]
						;Y = D1.S16[2]  Y = D1.S16[3]
								
				
Height_loop
		add	r10, r0, r3
		add	r11, r6, r7
		mov	r14, r8		
Width_loop		
		vld1.64 {d10}, [r1]!	;u  0~7 		
		vld1.64 {d12}, [r2]!	;v  0~7	
 		vld1.64 {q3}, [r0]!	;y  0~15
		vmov.s16 q1, #16		
		vmov.s16 q2, #128
		 		
		vmovl.u8 q5, d10	;u0~7	will extern to 0~15
		vmovl.u8 q6, d12	;v0~7	will extern to 0~15
		vsub.s16 q5, q5, q2	;u[0~7] - 128;
		vsub.s16 q6, q6, q2	;v[0~7] - 128;					
;first 0~3 uv
		vmull.s16 q9, d10, D0[2]	;a2[0~3] = (a0 * ConstU1)
		vmull.s16 q10, d10, D0[0]	;a0 = (a0 * ConstU2)		
		vmlal.s16 q10, d12, D0[1]	;a0[0~3] = (a1 * ConstV2 + a0 *ConstU2)
		vmull.s16 q11, d12, D0[3]	;a1[0~3] = (a1 * ConstV1);
		vmov	q12, q9
		vmov	q13, q10
		vmov	q14, q11					
		vtrn.32		q9, q12
		vtrn.32		q10, q13
		vtrn.32		q11, q14
						
		vswp.64		d19, d24	;q9=a2[0011] q12=a2[2233]
		vswp.64		d21, d26	;q10=a0[0011] q13=a0[2233]
		vswp.64		d23, d28	;q11=a1[0011] q14=a1[2233]
;line y0  0~7		
		vmovl.u8 q7, d6			;y  0~7		
		vsub.s16 q7, q7, q1		;y[0~7]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d2, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d4, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d3, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d5, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d2, q1
		vqmovn.u16 d3, q2
		vqmovn.u16 d4, q8
		vswp.64 d2, d4
		vst3.8 {d2, d3, d4}, [r6]!
												
;line y1  0~7	
		vld1.64 {q4}, [r10]!	;y1
		vmov.s16 q1, #16		
		vmov.s16 q2, #128		
		vmov.64 d6, d9			; d7:y0 8~15, d6:y1 8~15
		vmovl.u8 q7, d8			;y1  0~7		
		vsub.s16 q7, q7, q1		;y1[0~7]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d2, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d4, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d3, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d5, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d2, q1
		vqmovn.u16 d3, q2
		vqmovn.u16 d4, q8
		vswp.64 d2, d4		
		vst3.8 {d2, d3, d4}, [r11]!		

;last 4~7 uv
		vmull.s16 q9, d11, D0[2]	;a2[0~3] = (a0 * ConstU1)
		vmull.s16 q10, d11, D0[0]	;a0 = (a0 * ConstU2)		
		vmlal.s16 q10, d13, D0[1]	;a0[0~3] = (a1 * ConstV2 + a0 *ConstU2)
		vmull.s16 q11, d13, D0[3]	;a1[0~3] = (a1 * ConstV1);
		vmov	q12, q9
		vmov	q13, q10
		vmov	q14, q11					
		vtrn.32		q9, q12
		vtrn.32		q10, q13
		vtrn.32		q11, q14
						
		vswp.64		d19, d24	;q9=a2[0011] q12=a2[2233]
		vswp.64		d21, d26	;q10=a0[0011] q13=a0[2233]
		vswp.64		d23, d28	;q11=a1[0011] q14=a1[2233]
;line y0  8~15
		vmov.s16 q1, #16		
		vmov.s16 q2, #128		
		vmovl.u8 q7, d7			;y  8~15		
		vsub.s16 q7, q7, q1		;y[8~15]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d2, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d4, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d3, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d5, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d2, q1
		vqmovn.u16 d3, q2
		vqmovn.u16 d4, q8
		vswp.64 d2, d4		
		vst3.8 {d2, d3, d4}, [r6]!
												
;line y1  8~15	
		vmov.s16 q1, #16		
		vmov.s16 q2, #128
		vmovl.u8 q7, d6			;y1  8~15		
		vsub.s16 q7, q7, q1		;y1[8~15]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d2, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d4, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d3, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d5, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d2, q1
		vqmovn.u16 d3, q2
		vqmovn.u16 d4, q8
		vswp.64 d2, d4		
		vst3.8 {d2, d3, d4}, [r11]!		
	
		subs	r14, r14, #16						
		bgt		Width_loop
								
		sub	r0, r0, r8
		mov	r12, r8,  lsr #1
		sub	r1, r1, r12
		sub	r2, r2, r12
		add	r12, r8, r8, lsl #1
		sub	r6, r6, r12
		
		add	r0, r0, r3, lsl #1	;y
		add	r1, r1, r4		;u
		add	r2, r2, r5		;v
		add	r6, r6, r7, lsl #1	;d
		subs	r9, r9, #2				
		bgt		Height_loop

        	add     sp, sp, #64
		ldmia	sp!, {r4-r11, pc}		; restore and return 
	ENDP  ;

	ALIGN 8
ConstYUV	dcd	0x0D020645	;V2U2
ConstV1U1	dcd	0x19892045	;V1U1
ConstY		dcd	0x0000129F	;Y
ConstY1		dcd	0x0000129F	;Y	
END
