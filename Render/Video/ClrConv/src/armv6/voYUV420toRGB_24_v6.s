;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2009		            *
;	written by John							 	                                    *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY
	EXPORT |voyuv420torgb24_arm|

|voyuv420torgb24_arm| PROC
;void voyuv420torgb24_arm(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, 
;			  VO_S32 in_stridey, VO_U8 *out_buf, const VO_S32 out_stride,
;             VO_S32 width, VO_S32 height, VO_S32 in_strideu, VO_S32 in_stridev)	
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
;		in_strideu	ldr	rx, [sp, #68]
;		in_stridev	ldr	rx, [sp, #72]
;		out_buf		ldr	rx, [sp, #84]
;		out_stride	ldr	rx, [sp, #88]
;		width,		ldr	rx, [sp, #68]
;		height		ldr	rx, [sp, #72]

		stmdb	sp!, {r4-r11, lr}		; save regs used
		sub	sp, sp, #64			; sp - 32
		ldr	r6, [sp, #100]			;out_buf
		ldr	r7, [sp, #104]			;out_stride
		ldr	r8, [sp, #108]			;width
		ldr	r9, [sp, #112]			;height
		ldr	r4, [sp, #116]			;in_strideu
		ldr	r5, [sp, #120]			;in_stridev
				
		str	r0, [sp, #4]			;tmp psrc_y
		str	r1, [sp, #8]			;tmp psrc_u
		str	r2, [sp, #12]			;tmp psrc_v
		str	r3, [sp, #16]			;tmp in_stridey		
		str	r4, [sp, #20]			;tmp in_strideu
		str	r5, [sp, #24]			;tmp in_stridev
		str	r6, [sp, #28]			;tmp out_buf
		str	r7, [sp, #32]			;tmp out_stride
		str	r8, [sp, #36]			;tmp width
		str	r9, [sp, #40]			;tmp height			
		
Height_loop

Width_loop
		ldr	r0, [sp, #4]	;y
		ldr	r1, [sp, #8]	;u
		ldr	r2, [sp, #12]	;v
		ldr	r3, [sp, #16]
								
		ldr	r5, [r0, r3]
		ldr	r4, [r0]
		pkhbt		r6, r4, r5, lsl #16	;	
		pkhtb		r7, r5, r4, asr #16	;		
		uxtb16		r4, r6			;[]y0[]y0
		uxtb16		r5, r6, ror #8		;[]y1[]y1
		uxtb16		r6, r7			;[]y2[]y2
		uxtb16		r7, r7, ror #8		;[]y3[]y3
		
		ldr	r14, lab_00100010		;psrc_y - 16
		ssub16	r4, r4, r14
		ssub16	r5, r5, r14
		ssub16	r6, r6, r14
		ssub16	r7, r7, r14		
								
		ldr	r9, [r1], #4
		ldr	r10, [r2], #4
		str	r1, [sp, #8]
		str	r2, [sp, #12]		
		pkhbt		r11, r9, r10, lsl #16	;v1v0u1u0	
		pkhtb		r12, r10, r9, asr #16	;v3v2u3u2		
		uxtb16		r9, r11			;[]v0[]u0
		uxtb16		r10, r11, ror #8	;[]v1[]u1
		uxtb16		r11, r12		;[]v2[]u2
		uxtb16		r12, r12, ror #8	;[]v3[]u3
		mov	r14, r14, lsl #3			;psrc_uvy - 128
		ssub16	r9, r9, r14
		ssub16	r10, r10, r14
		ssub16	r11, r11, r14
		ssub16	r12, r12, r14	
;1_uv
		ldr	r14, ConstV1U1
		smulbb	r0, r9, r14		;a2 = (a0 * ConstU1);				
		smultt	r1, r9, r14		;a1 = (a1 * ConstV1);
		ldr	r14, ConstV2U2	
		smuad	r9, r9, r14		;a0 = (a1 * ConstV2 + a0 *ConstU2);
;1_uv_12_y		
		ldr	r14, ConstY
		smulbb	r2, r4, r14		;a3 = (psrc_y[0] - 16)*ConstY;				
		smulbb	r3, r5, r14		;a33 = (psrc_y[1] - 16)*ConstY;
		smultb	r4, r4, r14		;a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
		smultb	r5, r5, r14		;a77 = (psrc_y[in_stridey+1] - 16)*ConstY;					
;bigin to str	a0=r9, a1=r1, a2=r0, a3=r2, a33=r3.
						;a4  = SAT((a3 + a1)>>12);
						;a5  = SAT((a3 - a0)>>12);
						;a3  = SAT((a3 + a2)>>12);
						;*(out_buf) = a3;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;					
		add	r8, r2, r0
		usat	r8, #8, r8, asr #12
		sub	r14, r2, r9
		usat	r14, #8, r14, asr #12
		add	r2, r2, r1
		orr	r8, r8, r14, lsl #8		
		usat	r2, #8, r2, asr #12
		orr	r8, r8, r2, lsl #16
		add	r14, r3, r0
		usat	r14, #8, r14, asr #12
		orr	r2, r8, r14, lsl #24	;r2 = out_buf[0~3]
						;a4  = SAT((a33 + a1)>>12);
						;a5  = SAT((a33 - a0)>>12);
						;a33  = SAT((a33 + a2)>>12);
						;*(out_buf) = a33;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		sub	r8, r3, r9
		usat	r8, #8, r8, asr #12
		add	r14, r3, r1
		usat	r14, #8, r14, asr #12
		orr	r3, r8, r14, lsl #8	;r3 = out_buf[4~5]
;bigin to str+out_stride a0=r9, a1=r1, a2=r0, a7=r4, a77=r5.		
						;a4  = SAT((a7 + a1)>>12);
						;a5  = SAT((a7 - a0)>>12);
						;a77  = SAT((a7 + a2)>>12);
						;*(out_buf+out_stride) = a77;
						;*(out_buf+1+out_stride) = a5;
						;*(out_buf+2+out_stride) = a4;					
		add	r8, r4, r0
		usat	r8, #8, r8, asr #12
		sub	r14, r4, r9
		usat	r14, #8, r14, asr #12
		add	r4, r4, r1
		orr	r8, r8, r14, lsl #8		
		usat	r4, #8, r4, asr #12
		orr	r8, r8, r4, lsl #16
		add	r14, r5, r0
		usat	r14, #8, r14, asr #12
		orr	r4, r8, r14, lsl #24	;r4 = out_buf[0~3]+out_stride
						;a4  = SAT((a77 + a1)>>12);
						;a5  = SAT((a77 - a0)>>12);
						;a77  = SAT((a77 + a2)>>12);
						;*(out_buf) = a77;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		sub	r8, r5, r9
		usat	r8, #8, r8, asr #12
		add	r14, r5, r1
		usat	r14, #8, r14, asr #12
		orr	r5, r8, r14, lsl #8	;r5 = out_buf[4~5]+out_stride
;2_uv
		ldr	r14, ConstV1U1
		smulbb	r0, r10, r14		;a2 = (a0 * ConstU1);				
		smultt	r1, r10, r14		;a1 = (a1 * ConstV1);
		ldr	r14, ConstV2U2	
		smuad	r9, r10, r14		;a0 = (a1 * ConstV2 + a0 *ConstU2);
;2_uv_12_y		
		ldr	r8, [sp, #28]		;tmp out_buf
		ldr	r10, [sp, #104]		;out_stride
		str	r2, [r8]		
		str	r4, [r8, r10]
		ldr	r14, ConstY
		smulbb	r2, r6, r14		;a3 = (psrc_y[0] - 16)*ConstY;				
		smulbb	r4, r7, r14		;a33 = (psrc_y[1] - 16)*ConstY;
		smultb	r6, r6, r14		;a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
		smultb	r7, r7, r14		;a77 = (psrc_y[in_stridey+1] - 16)*ConstY;					
;bigin to str	a0=r9, a1=r1, a2=r0, a3=r2, a33=r4.
						;a4  = SAT((a3 + a1)>>12);
						;a5  = SAT((a3 - a0)>>12);
						;a3  = SAT((a3 + a2)>>12);
						;*(out_buf) = a3;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;					
		add	r14, r2, r0
		usat	r14, #8, r14, asr #12
		orr	r3, r3, r14, lsl #16	
		sub	r14, r2, r9
		usat	r14, #8, r14, asr #12
		orr	r3, r3, r14, lsl #24	;r3 = out_buf[4~7]		
		str	r3, [r8, #4]		
		add	r2, r2, r1
		usat	r2, #8, r2, asr #12	;r2 = out_buf[8] 		
						;a4  = SAT((a33 + a1)>>12);
						;a5  = SAT((a33 - a0)>>12);
						;a33  = SAT((a33 + a2)>>12);
						;*(out_buf) = a33;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		add	r14, r4, r0
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #8						
		sub	r14, r4, r9
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #16		
		add	r14, r4, r1
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #24	;out_buf[8~11]
		str	r2, [r8, #8]		
;bigin to str+out_stride a0=r9, a1=r1, a2=r0, a7=r6, a77=r7.		
						;a4  = SAT((a7 + a1)>>12);
						;a5  = SAT((a7 - a0)>>12);
						;a77  = SAT((a7 + a2)>>12);
						;*(out_buf+out_stride) = a77;
						;*(out_buf+1+out_stride) = a5;
						;*(out_buf+2+out_stride) = a4;					
		add	r14, r6, r0
		usat	r14, #8, r14, asr #12
		orr	r5, r5, r14, lsl #16	
		sub	r14, r6, r9
		usat	r14, #8, r14, asr #12		
		orr	r5, r5, r14, lsl #24	;r5 = out_buf[4~7]
		add	r8, r8, r10		
		str	r5, [r8, #4]		
		add	r6, r6, r1
		usat	r6, #8, r6, asr #12	;r6 = out_buf[8] 	
						;a4  = SAT((a77 + a1)>>12);
						;a5  = SAT((a77 - a0)>>12);
						;a77  = SAT((a77 + a2)>>12);
						;*(out_buf) = a77;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		add	r14, r7, r0
		usat	r14, #8, r14, asr #12
		orr	r6, r6, r14, lsl #8						
		sub	r5, r7, r9
		usat	r5, #8, r5, asr #12
		orr	r6, r6, r5, lsl #16		
		add	r14, r7, r1
		usat	r14, #8, r14, asr #12
		orr	r6, r6, r14, lsl #24	;out_buf[8~11]
		str	r6, [r8, #8]
		add	r8, r8, #12
		sub	r8, r8, r10
		str	r8, [sp, #28]

;two //////////////////////////////
		ldr	r0, [sp, #4]
		ldr	r3, [sp, #16]
		add	r0, r0, #4								
		ldr	r5, [r0, r3]
		ldr	r4, [r0], #4
		str	r0, [sp, #4]
		pkhbt		r6, r4, r5, lsl #16	;	
		pkhtb		r7, r5, r4, asr #16	;		
		uxtb16		r4, r6			;[]y0[]y0
		uxtb16		r5, r6, ror #8		;[]y1[]y1
		uxtb16		r6, r7			;[]y2[]y2
		uxtb16		r7, r7, ror #8		;[]y3[]y3
		ldr	r14, lab_00100010		;psrc_y - 16
		ssub16	r4, r4, r14
		ssub16	r5, r5, r14
		ssub16	r6, r6, r14
		ssub16	r7, r7, r14		

;3_uv		r9 r10 can use   r11, r12
		ldr	r14, ConstV1U1
		smulbb	r0, r11, r14		;a2 = (a0 * ConstU1);				
		smultt	r1, r11, r14		;a1 = (a1 * ConstV1);
		ldr	r14, ConstV2U2	
		smuad	r11, r11, r14		;a0 = (a1 * ConstV2 + a0 *ConstU2);
;3_uv_12_y		
		ldr	r14, ConstY
		smulbb	r2, r4, r14		;a3 = (psrc_y[0] - 16)*ConstY;				
		smulbb	r3, r5, r14		;a33 = (psrc_y[1] - 16)*ConstY;
		smultb	r4, r4, r14		;a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
		smultb	r5, r5, r14		;a77 = (psrc_y[in_stridey+1] - 16)*ConstY;					
;bigin to str	a0=r11, a1=r1, a2=r0, a3=r2, a33=r3.
						;a4  = SAT((a3 + a1)>>12);
						;a5  = SAT((a3 - a0)>>12);
						;a3  = SAT((a3 + a2)>>12);
						;*(out_buf) = a3;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;					
		add	r8, r2, r0
		usat	r8, #8, r8, asr #12
		sub	r14, r2, r11
		usat	r14, #8, r14, asr #12
		add	r2, r2, r1
		orr	r8, r8, r14, lsl #8		
		usat	r2, #8, r2, asr #12
		orr	r8, r8, r2, lsl #16
		add	r14, r3, r0
		usat	r14, #8, r14, asr #12
		orr	r2, r8, r14, lsl #24	;r2 = out_buf[0~3]
						;a4  = SAT((a33 + a1)>>12);
						;a5  = SAT((a33 - a0)>>12);
						;a33  = SAT((a33 + a2)>>12);
						;*(out_buf) = a33;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		sub	r8, r3, r11
		usat	r8, #8, r8, asr #12
		add	r14, r3, r1
		usat	r14, #8, r14, asr #12
		orr	r3, r8, r14, lsl #8	;r3 = out_buf[4~5]
;bigin to str+out_stride a0=r11, a1=r1, a2=r0, a7=r4, a77=r5.		
						;a4  = SAT((a7 + a1)>>12);
						;a5  = SAT((a7 - a0)>>12);
						;a77  = SAT((a7 + a2)>>12);
						;*(out_buf+out_stride) = a77;
						;*(out_buf+1+out_stride) = a5;
						;*(out_buf+2+out_stride) = a4;					
		add	r8, r4, r0
		usat	r8, #8, r8, asr #12
		sub	r14, r4, r11
		usat	r14, #8, r14, asr #12
		add	r4, r4, r1
		orr	r8, r8, r14, lsl #8		
		usat	r4, #8, r4, asr #12
		orr	r8, r8, r4, lsl #16
		add	r14, r5, r0
		usat	r14, #8, r14, asr #12
		orr	r4, r8, r14, lsl #24	;r4 = out_buf[0~3]+out_stride
						;a4  = SAT((a77 + a1)>>12);
						;a5  = SAT((a77 - a0)>>12);
						;a77  = SAT((a77 + a2)>>12);
						;*(out_buf) = a77;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		sub	r8, r5, r11
		usat	r8, #8, r8, asr #12
		add	r14, r5, r1
		usat	r14, #8, r14, asr #12
		orr	r5, r8, r14, lsl #8	;r5 = out_buf[4~5]+out_stride
;4_uv
		ldr	r14, ConstV1U1
		smulbb	r0, r12, r14		;a2 = (a0 * ConstU1);				
		smultt	r1, r12, r14		;a1 = (a1 * ConstV1);
		ldr	r14, ConstV2U2	
		smuad	r11, r12, r14		;a0 = (a1 * ConstV2 + a0 *ConstU2);
;4_uv_12_y		
		ldr	r8, [sp, #28]		;tmp out_buf
		ldr	r12, [sp, #104]		;out_stride
		str	r2, [r8]		
		str	r4, [r8, r12]
		ldr	r14, ConstY
		smulbb	r2, r6, r14		;a3 = (psrc_y[0] - 16)*ConstY;				
		smulbb	r4, r7, r14		;a33 = (psrc_y[1] - 16)*ConstY;
		smultb	r6, r6, r14		;a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
		smultb	r7, r7, r14		;a77 = (psrc_y[in_stridey+1] - 16)*ConstY;					
;bigin to str	a0=r11, a1=r1, a2=r0, a3=r2, a33=r4.
						;a4  = SAT((a3 + a1)>>12);
						;a5  = SAT((a3 - a0)>>12);
						;a3  = SAT((a3 + a2)>>12);
						;*(out_buf) = a3;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;					
		add	r14, r2, r0
		usat	r14, #8, r14, asr #12
		orr	r3, r3, r14, lsl #16	
		sub	r14, r2, r11
		usat	r14, #8, r14, asr #12
		orr	r3, r3, r14, lsl #24	;r3 = out_buf[4~7]		
		str	r3, [r8, #4]		
		add	r2, r2, r1
		usat	r2, #8, r2, asr #12	;r2 = out_buf[8] 		
						;a4  = SAT((a33 + a1)>>12);
						;a5  = SAT((a33 - a0)>>12);
						;a33  = SAT((a33 + a2)>>12);
						;*(out_buf) = a33;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		add	r14, r4, r0
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #8						
		sub	r14, r4, r11
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #16		
		add	r14, r4, r1
		usat	r14, #8, r14, asr #12
		orr	r2, r2, r14, lsl #24	;out_buf[8~11]
		str	r2, [r8, #8]		
;bigin to str+out_stride a0=r11, a1=r1, a2=r0, a7=r6, a77=r7.		
						;a4  = SAT((a7 + a1)>>12);
						;a5  = SAT((a7 - a0)>>12);
						;a77  = SAT((a7 + a2)>>12);
						;*(out_buf+out_stride) = a77;
						;*(out_buf+1+out_stride) = a5;
						;*(out_buf+2+out_stride) = a4;					
		add	r14, r6, r0
		usat	r14, #8, r14, asr #12
		orr	r5, r5, r14, lsl #16	
		sub	r14, r6, r11
		usat	r14, #8, r14, asr #12		
		orr	r5, r5, r14, lsl #24	;r5 = out_buf[4~7]
		add	r8, r8, r12		
		str	r5, [r8, #4]		
		add	r6, r6, r1
		usat	r6, #8, r6, asr #12	;r6 = out_buf[8] 	
						;a4  = SAT((a77 + a1)>>12);
						;a5  = SAT((a77 - a0)>>12);
						;a77  = SAT((a77 + a2)>>12);
						;*(out_buf) = a77;
						;*(out_buf+1) = a5;
						;*(out_buf+2) = a4;
		add	r14, r7, r0
		usat	r14, #8, r14, asr #12
		orr	r6, r6, r14, lsl #8						
		sub	r5, r7, r11
		usat	r5, #8, r5, asr #12
		orr	r6, r6, r5, lsl #16		
		add	r14, r7, r1
		usat	r14, #8, r14, asr #12
		orr	r6, r6, r14, lsl #24	;out_buf[8~11]
		str	r6, [r8, #8]
		add	r8, r8, #12
		ldr	r14, [sp, #108]		;width		
		sub	r8, r8, r12
		str	r8, [sp, #28]		
		subs	r14, r14, #8
		str	r14, [sp, #108]		;width							
		bne		Width_loop

				
		ldr	r0, [sp, #4]	;y
		ldr	r1, [sp, #8]	;u
		ldr	r2, [sp, #12]	;v		
		ldr	r3, [sp, #16]		;tmp in_stridey		
		ldr	r4, [sp, #20]		;tmp in_strideu
		ldr	r5, [sp, #24]		;tmp in_stridev
		ldr	r6, [sp, #36]		;tmp width			
		ldr	r7, [sp, #28]		;tmp out_buf
		ldr	r8, [sp, #32]		;tmp out_stride
		ldr	r14,[sp,#112]		;height
		str	r6, [sp,#108]		;width
								
		sub	r0, r0, r6
		mov	r9, r6,  lsr #1
		sub	r1, r1, r9
		sub	r2, r2, r9
		add	r9, r6, r6, lsl #1
		sub	r7, r7, r9
		
		add	r0, r0, r3, lsl #1	;y
		add	r1, r1, r4		;u
		add	r2, r2, r5		;v
		add	r7, r7, r8, lsl #1	;d
		
		str	r0, [sp, #4]	;y
		str	r1, [sp, #8]	;u
		str	r2, [sp, #12]	;v		
		str	r7, [sp, #28]		;tmp out_buf
		subs	r14, r14, #2
		str	r14, [sp, #112]		;height					
		bne		Height_loop

        	add     sp, sp, #64
		ldmia	sp!, {r4-r11, pc}		; restore and return 
	ENDP  ;
ConstV2U2	dcd	0x0D020645	;V2U2
ConstV1U1	dcd	0x19892045	;V1U1
ConstY	dcd	0x0000129F	;Y		
lab_00100010
	dcd	0x00100010	;
END
