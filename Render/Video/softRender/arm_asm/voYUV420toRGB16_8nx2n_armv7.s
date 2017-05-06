;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2009		            *
;	written by John							 	                                    *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY
	EXPORT |voyuv420torgb16_8nx2n_armv7|

;void cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
;				 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
;				 const VO_S32 uin_stride, const VO_S32 vin_stride)
				 
|voyuv420torgb16_8nx2n_armv7| PROC
;void voyuv420torgb16_arm(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, 
;			  int32_t in_stridey, 
;			  uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height
;			, int32_t in_strideu, int32_t in_stridev)	
;{
;	int32_t a0, a1, a2, a3, a4, a5, a7, a33, a77;
;	uint32_t i, j;
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
;;			a4  = SAT((a3 + a1)>>12);
;;			a5  = SAT((a3 - a0)>>12);
;;			a3  = SAT((a3 + a2)>>12);
;;			*(out_buf+1) =( (a4 & 0xF8)  | ( a5 >> 5) );
;;			*(out_buf) =( ((a5 & 0x1C) << 3) | ( a3 >> 3) );

;;;;;;;new;;;;;;;;;;;;;
;	a4  = (ccClip31[((a3 + a1)>>15)]);
;	a5  = (ccClip63[((a3 - a0)>>14)]);
;	a3  = (ccClip31[((a3 + a2)>>15)]);
;	a4 = (a4<<11)|(a5<<5)|a3;
;;;;;;;new end;;;;;;;;;;;;;

;			out_buf += 4;
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
;		out_buf -= (width*2);
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
;		ldr	r4, [sp, #100]			;in_strideu
;		ldr	r5, [sp, #104]			;in_stridev
;		ldr	r6, [sp, #108]			;out_buf
;		ldr	r7, [sp, #112]			;out_stride
;		ldr	r8, [sp, #116]			;width
;		ldr	r9, [sp, #120]			;height				

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
		vmov.s16 q1, #16		
		vmov.s16 q2, #128						
								
				
Height_loop
		add	r10, r0, r3
		add	r11, r6, r7
		mov	r14, r8		
Width_loop		
		vld1.32 {d10[0]}, [r1]!	;u  0~3 		
		vld1.32 {d10[1]}, [r2]!	;v  0~3	
 		vld1.64 {d6}, [r0]!	;y  0~7
		vld1.64 {d7}, [r10]!	;y1
;input:y0y1 = q3, uv = d10
		 		
		vmovl.u8 q5, d10	;uv0~3	will extern to 0~3
		vsub.s16 q5, q5, q2	;uv[0~3] - 128;					
;first 0~3 uv
		vmull.s16 q9, d10, D0[2]	;a2[0~3] = (a0 * ConstU1)
		vmull.s16 q10, d10, D0[0]	;a0 = (a0 * ConstU2)		
		vmlal.s16 q10, d11, D0[1]	;a0[0~3] = (a1 * ConstV2 + a0 *ConstU2)
		vmull.s16 q11, d11, D0[3]	;a1[0~3] = (a1 * ConstV1);
		vmov	q12, q9
		vmov	q13, q10
		vmov	q14, q11					
		vtrn.32		q9, q12
		vtrn.32		q10, q13
		vtrn.32		q11, q14
						
		vswp.64		d19, d24	;q9=a2[0011] q12=a2[2233]
		vswp.64		d21, d26	;q10=a0[0011] q13=a0[2233]
		vswp.64		d23, d28	;q11=a1[0011] q14=a1[2233]
;q0~3, q9~14 used q4~8, q15 can use	
;line y0  0~7		
		vmovl.u8 q7, d6			;y  0~7		
		vsub.s16 q7, q7, q1		;y[0~7]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d10, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d12, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d11, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d13, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d14, q5
		vqmovn.u16 d15, q6
		vqmovn.u16 d10, q8
		
		vmovl.u8 q5, d10
		vmovl.u8 q6, d15 
		vmovl.u8 q8, d14
		vshr.u16 q5, q5, #3
		vshr.u16 q6, q6, #2		
		vshr.u16 q8, q8, #3
		vsli.u16 q5, q6, #5
		vsli.u16 q5, q8, #11
		vst1.64 {q5}, [r6]!
												
;line y1  0~7	
		vmovl.u8 q7, d7			;y  0~7		
		vsub.s16 q7, q7, q1		;y[0~7]- 16;
		vmull.s16 q8, d14, D1[0]	;a3[0~3] = (psrc_y[0] - 16)*ConstY;
		vmull.s16 q7, d15, D1[0]	;a3[4~7] = (psrc_y[0] - 16)*ConstY;
		;y[0~3]
		vadd.s32 q4, q8, q11		;a4  = a3 + a1;		
		vsub.s32 q15, q8, q10		;a5  = a3 - a0;
		vadd.s32 q8, q8, q9		;a3  = a3 + a2;
		vqshrun.s32 d10, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d12, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d16, q8, #12	;a3  = SAT((a3 + a2)>>12);
		;y[4~7]
		vadd.s32 q4, q7, q14		;a4  = a3 + a1;		
		vsub.s32 q15, q7, q13		;a5  = a3 - a0;
		vadd.s32 q7, q7, q12		;a3  = a3 + a2;
		vqshrun.s32 d11, q4, #12		;a4  = SAT((a3 + a1)>>12);		
		vqshrun.s32 d13, q15, #12	;a5  = SAT((a3 - a0)>>12);
		vqshrun.s32 d17, q7, #12	;a3  = SAT((a3 + a2)>>12);
		
		vqmovn.u16 d14, q5
		vqmovn.u16 d15, q6
		vqmovn.u16 d10, q8
		
		vmovl.u8 q5, d10
		vmovl.u8 q6, d15 
		vmovl.u8 q8, d14
		vshr.u16 q5, q5, #3
		vshr.u16 q6, q6, #2		
		vshr.u16 q8, q8, #3
		vsli.u16 q5, q6, #5
		vsli.u16 q5, q8, #11
		vst1.64 {q5}, [r11]!		

	
		subs	r14, r14, #8						
		bgt		Width_loop
								
		sub	r0, r0, r8
		mov	r12, r8,  lsr #1
		sub	r1, r1, r12
		sub	r2, r2, r12
		sub	r6, r6, r8, lsl #1
		
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
