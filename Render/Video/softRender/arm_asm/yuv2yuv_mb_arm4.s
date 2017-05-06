	AREA	|.rdata|, DATA, READONLY
	EXPORT	|cc_yuv2yuv_mb_s_arm|
	AREA	|.text|, CODE, READONLY

|cc_yuv2yuv_mb_s_arm| PROC
;void cc_yuv2yuv_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
;					VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv, VO_S32 src_stride, VO_S32 dst_stride,
;					 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
;				 const VO_S32 uin_stride, const VO_S32 vin_stride)
;{
;	VO_S32 i;	
;	do{
;		VO_S32 *x_scale_par0 = x_scale_par;
;		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
;		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
;		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
;		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
;		VO_S32 aa0 = y_scale_par[1];
;		VO_S32 bb0 = y_scale_par[2];
;		VO_S32 aa1 = y_scale_par[4];
;		VO_S32 bb1 = y_scale_par[5];		
;		i = width;		
;		do{
;			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
;			VO_S32 aa, bb;
;			// u, v
;			a0 = x_scale_par0[0];//src pos0
;			aa = x_scale_par0[1];
;			bb = x_scale_par0[2];			
;			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
;			a7 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
;			
;			a6 = x_scale_par0[3];
;			aa = x_scale_par0[4];
;			bb = x_scale_par0[5];
;			x_scale_par0 += 6;
;			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
;			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;
;
;			a1 = (a0 + a6 + 1)>>2;
;			a2 = src_u0[a1];
;			a3 = src_v0[a1];
;
;			a4 |= (a5 << 8);
;			a7 |= (a8 << 8);
;			*((VO_U16*)dsty) = a4;
;			*((VO_U16*)(dsty+dst_stride)) = a7;
;			*dstu++ = a2;
;			*dstv++ = a3;
;			dsty += 2;
;		}while((i-=2) != 0);
;		
;		dsty -= width;
;		dsty += (dst_stride<<1);
;
;		dstu -= (width>>1);
;		dstu += (dst_stride>>1);
;
;		dstv -= (width>>1);
;		dstv += (dst_stride>>1);
;		y_scale_par += 6;
;	}while((height-=2) != 0);
;}
;		r0 = width
;		r1 = height
;		r2 = src_y
;		r3 = src_u
;		r4 = src_v		ldr		r4, [sp, #96]
;		r5 = dsty		ldr		r5, [sp, #100]
;		r6 = dstu		ldr		r6, [sp, #104]
;		r7 = dstv		ldr		r7, [sp, #108]
;		r8 = src_stride		ldr		r8, [sp, #112]
;		r9 = dst_stride		ldr		r9, [sp, #116]
;		r10 = x_scale_par	ldr		r8, [sp, #120]
;		r11 = y_scale_par	ldr		r9, [sp, #124]
;		r12 = uin_stride	ldr		r8, [sp, #128]
;		r14 = vin_stride	ldr		r9, [sp, #132]

		stmdb	sp!, {r4-r11, lr}		;save regs used
		sub	sp, sp, #60			;sp - 32

		str	r0,	[sp, #4]		;[sp, #4] = width
		str	r1,	[sp, #8]		;[sp, #8] = height			
		str	r2,	[sp, #12]		;[sp, #12] = src_y
		str	r3,	[sp, #16]		;[sp, #16] = src_u
		;str	r4,	[sp, #96]		;[sp, #96] = src_v
								
;	do{
;		VO_S32 *x_scale_par0 = x_scale_par;
;		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
;		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
;		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
;		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
;		VO_S32 aa0 = y_scale_par[1];
;		VO_S32 bb0 = y_scale_par[2];
;		VO_S32 aa1 = y_scale_par[4];
;		VO_S32 bb1 = y_scale_par[5];		
;		i = width;
BIG_WHILE_D
		ldr	r11, [sp, #124]		;r11 = y_scale_par		
		ldr	r5, [r11, #4]		;y_scale_par[1]
		ldr	r6, [r11, #8]		;y_scale_par[2]			
		ldr	r7, [r11, #16]		;y_scale_par[4]
		ldr	r8, [r11, #20]		;y_scale_par[5]
		orr	r5, r5, r6, lsl #16	;r5 = bb0|aa0
		orr	r6, r7, r8, lsl #16	;r6 = bb1|aa1		
		ldr	r7, [r11, #0]		;r7 = y_scale_par[0]
		ldr	r8, [r11, #12]		;r8 = y_scale_par[3]				
		ldr	r9, [sp, #112]		;r9 = src_stride
		ldr	r2, [sp, #12]		;r2 = src_y0, r11 = src_y1
		ldr	r3, [sp, #16]		;r3 = src_u0							
		ldr	r4, [sp, #96]		;r4 = src_v0		
		add	r11, r11, #24
		mul	r10, r9, r7		;y_scale_par[0] * src_stride
		mul	r8, r9, r8		;y_scale_par[3] * src_stride
		str	r11, [sp, #124]		;r11 = y_scale_par
		add	r11, r2, r8		;r11 = src_y1		
		add	r2, r2, r10		;r2 = src_y0		
		
		mov r7, r7, lsr #1
		ldr	r9, [sp, #128]		;r9 = uin_stride		
		ldr	r10, [sp, #132]		;r10 = vin_stride
		mul	r9, r9, r7		;(y_scale_par[0]>>1) * uin_stride
		mul	r10, r10, r7		;(y_scale_par[0]>>1) * vin_stride
		add	r3, r3, r9		;r3 = src_u0
		add	r4, r4, r10		;r4 = src_v0
		ldr	r0, [sp, #4]	;[sp, #4] = width
		ldr	r1, [sp, #120]	;x_scale_par				
		str	r3, [sp, #20]	;[sp, #20] = src_u
		str	r4, [sp, #24]	;[sp, #24] = src_v
		str	r2, [sp, #28]	;[sp, #28] = src_y0		
		str	r11, [sp, #32]	;[sp, #32] = src_y1
		str	r0,	[sp, #36]	;[sp, #36] = width
		str	r1,	[sp, #40]	;[sp, #40] = x_scale_par		
											
LIT_WHILE_D			
;		do{
;			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
;			VO_S32 aa, bb;
;			a0 = x_scale_par0[0];//src pos0
;			aa = x_scale_par0[1];
;			bb = x_scale_par0[2];			
;			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
;			a7 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
;			
;			a6 = x_scale_par0[3];
;			aa = x_scale_par0[4];
;			bb = x_scale_par0[5];
;			x_scale_par0 += 6;
;			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
;			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;
;
;			a1 = (a0 + a6 + 1)>>2;
;			a2 = src_u0[a1];
;			a3 = src_v0[a1];
;
;			a4 |= (a5 << 8);
;			a7 |= (a8 << 8);
;			*((VO_U16*)dsty) = a4;
;			*((VO_U16*)(dsty+dst_stride)) = a7;
;			*dstu++ = a2;
;			*dstv++ = a3;
;			dsty += 2;
;		}while((i-=2) != 0);

;r5 = bb0|aa0, r6 = bb1|aa1
;r0, r1, r2, r3, r4, r7, r8, r9, r10, r11, r12 can use.
		ldr	r12, [sp, #40]		;r12 = x_scale_par
		ldr	r0, [r12, #0]		;x_scale_par[0]						
		ldr	r7, [r12, #4]		;x_scale_par[1]
		ldr	r8, [r12, #8]		;x_scale_par[2]
		ldr	r1, [r12, #12]		;x_scale_par[3]
		ldr	r9, [r12, #16]		;x_scale_par[4]				
		ldr	r10, [r12, #20]		;x_scale_par[5]
		add	r12, r12, #24
		str	r12, [sp, #40]		;r12 = x_scale_par		
		orr	r7, r7, r8, lsl #16	;r7 = bb|aa_0
		orr	r8, r9, r10, lsl #16	;r8 = bb|aa_1
;y00000000
		ldr	r2,	[sp, #28]	;[sp, #28] = src_y0
;r3, r4, r9, r10, r12, r11 can use, r0 = a0, r1 = a6, ;r5 = bb0|aa0, r6 = bb1|aa1, r7 = bb|aa_0, r8 = bb|aa_6
		ldrb	r3, [r2, r0]		;src_y0[a0]
		ldrb	r4, [r2, r1]		;src_y0[a6]
		add	r0, r0, #1
		add	r1, r1, #1		
		ldrb	r9, [r2, r0]		;src_y0[a0+1]
		ldrb	r12, [r2, r1]		;src_y0[a6+1]
		ldr	r10, [sp, #112]		;r10 = src_stride
		orr	r3, r9, r3, lsl #16	;r3 = src_y0[a0]|src_y0[a0+1]
		orr	r4, r12, r4, lsl #16	;r4 = src_y0[a6]|src_y0[a6+1]
;r9, r12, r11 can use, r10 = src_stride		
		add	r0, r0, r10
		add	r1, r1, r10		
		ldrb	r9, [r2, r0]		;src_y0[a0+src_stride+1]
		ldrb	r10, [r2, r1]		;src_y0[a6+src_stride+1]
		sub	r0, r0, #1
		sub	r1, r1, #1		
		ldrb	r11, [r2, r0]		;src_y0[a0+src_stride]
		ldrb	r12, [r2, r1]		;src_y0[a6+src_stride]
		orr	r9, r9, r11, lsl #16	;r9 = src_y0[a0+src_stride]|src_y0[a0+src_stride+1]
		orr	r10, r10, r12, lsl #16	;r10 = src_y0[a6+src_stride]|src_y0[a6+src_stride+1]
		smuad	r3, r3, r7		;src_y0[a0]*bb + src_y0[a0+1]*aa
		smuad	r4, r4, r8		;src_y0[a6]*bb + src_y0[a6+1]*aa
		smuad	r9, r9, r7		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		smuad	r10, r10, r8		;src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa
						
		smulwt	r3, r3, r5		;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0			
		smlawb	r3, r9, r5, r3	       ;((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>16
		mov	r3, r3, lsr #4
		smulwt	r4, r4, r5		;(src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0			
		smlawb	r4, r10, r5, r4	       ;((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>16
		mov	r4, r4, lsr #4		
		orr	r3, r3, r4, lsl #8
		ldr	r4, [sp, #100]		; r4 = dsty
		strh	r3, [r4]
;y1111111
		ldr	r10, [sp, #112]		;r10 = src_stride
		ldr	r2,	[sp, #32]	;[sp, #28] = src_y1
		sub	r0, r0, r10
		sub	r1, r1, r10				
;r3, r4, r9, r10, r12, r11 can use, r0 = a0, r1 = a6, ;r5 = bb0|aa0, r6 = bb1|aa1, r7 = bb|aa_0, r8 = bb|aa_6
		ldrb	r3, [r2, r0]		;src_y0[a0]
		ldrb	r4, [r2, r1]		;src_y0[a6]
		add	r0, r0, #1
		add	r1, r1, #1		
		ldrb	r9, [r2, r0]		;src_y0[a0+1]
		ldrb	r12, [r2, r1]		;src_y0[a6+1]
		orr	r3, r9, r3, lsl #16	;r3 = src_y0[a0]|src_y0[a0+1]
		orr	r4, r12, r4, lsl #16	;r4 = src_y0[a6]|src_y0[a6+1]
;r9, r12, r11 can use, r10 = src_stride		
		add	r0, r0, r10
		add	r1, r1, r10		
		ldrb	r9, [r2, r0]		;src_y0[a0+src_stride+1]
		ldrb	r10, [r2, r1]		;src_y0[a6+src_stride+1]
		sub	r0, r0, #1
		sub	r1, r1, #1		
		ldrb	r11, [r2, r0]		;src_y0[a0+src_stride]
		ldrb	r12, [r2, r1]		;src_y0[a6+src_stride]
		orr	r9, r9, r11, lsl #16	;r9 = src_y0[a0+src_stride]|src_y0[a0+src_stride+1]
		orr	r10, r10, r12, lsl #16	;r10 = src_y0[a6+src_stride]|src_y0[a6+src_stride+1]
		smuad	r3, r3, r7		;src_y0[a0]*bb + src_y0[a0+1]*aa
		smuad	r4, r4, r8		;src_y0[a6]*bb + src_y0[a6+1]*aa
		smuad	r9, r9, r7		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		smuad	r10, r10, r8		;src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa
						
		smulwt	r3, r3, r6		;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0			
		smlawb	r3, r9, r6, r3	       ;((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>16
		mov	r3, r3, lsr #4
		smulwt	r4, r4, r6		;(src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0			
		smlawb	r4, r10, r6, r4	       ;((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>16
		mov	r4, r4, lsr #4		
		orr	r3, r3, r4, lsl #8
		ldr	r4, [sp, #100]		; r4 = dsty
		ldr	r9, [sp, #116]		;r9 = dst_stride
		add	r11, r4, r9
		ldr	r10, [sp, #112]		;r10 = src_stride		
		strh	r3, [r11]
		add	r4, r4, #2
		sub	r0, r0, r10
		sub	r1, r1, r10	
		str	r4, [sp, #100]		; r4 = dsty
;uuuvvv
;			a1 = (a0 + a6 + 1)>>2;
;			a2 = src_u0[a1];
;			a3 = src_v0[a1];
;r3, r4, r9, r10, r12, r11 can use, r0 = a0, r1 = a6, ;r5 = bb0|aa0, r6 = bb1|aa1, r7 = bb|aa_0, r8 = bb|aa_1
		add	r0, r0, r1
		add	r0, r0, #1
		mov	r0, r0, lsr #2	
		ldr	r9,	[sp, #20]	;[sp, #20] = src_u
		ldr	r10,	[sp, #24]	;[sp, #24] = src_v
		ldrb	r7, [r9, r0]
		ldrb	r8, [r10, r0]				
		ldr	r3, [sp, #104]		; r4 = dstu
		ldr	r4, [sp, #108]		; r4 = dstv
		strb	r7, [r3], #1
		strb	r8, [r4], #1						
		str	r3, [sp, #104]		; r4 = dstu
		str	r4, [sp, #108]		; r4 = dstv							
		
		ldr	r0, [sp, #36]	;[sp, #36] = width
		subs	r0, r0, #2
		strne	r0, [sp, #36]
		bne		LIT_WHILE_D

		ldr	r0, [sp, #8]	; [sp, #8] = height
		ldr	r1, [sp, #4]	; [sp, #4] = width
		ldr	r2, [sp, #100]	;r2 = dsty
		ldr	r3, [sp, #104]	;r3 = dstu
		ldr	r4, [sp, #108]	;r4 = dstv
		ldr	r7, [sp, #116]	;r7 = dst_stride									
		subs	r0, r0, #2			
		mov	r8, r1, lsr #1
		mov	r9, r7, lsr #1
		mov	r7, r7, lsl #1			
		sub	r2, r2, r1			
		sub	r3, r3, r8
		sub	r4, r4, r8
		add	r2, r2, r7
		add	r3, r3, r9 
		add	r4, r4, r9
		str	r2, [sp, #100]	;r2 = dsty
		str	r3, [sp, #104]	;r3 = dstu
		str	r4, [sp, #108]	;r4 = dstv									
		strne	r0, [sp, #8]
;		dsty -= width;
;		dsty += (dst_stride<<1);
;
;		dstu -= (width>>1);
;		dstu += (dst_stride>>1);
;
;		dstv -= (width>>1);
;		dstv += (dst_stride>>1);
;		y_scale_par += 6;
;	}while((height-=2) != 0);								
		bne	BIG_WHILE_D
;}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		; restore and return 

	ENDP  ;
		
END
