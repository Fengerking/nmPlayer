	#include "voYYDef_CC.h"
	
	
	AREA	|.rdata|, DATA, READONLY
	EXPORT	|cc_yuv2yuv_8x8_sR180_armv7|
	EXPORT	|cc_yuv2yuv_8x2n_sR180_armv7|	
	AREA	|.text|, CODE, READONLY
  macro	
  Transpose_R180
		ldr		r7, [sp, #128]	; r7 = dsty		
		ldr		r2, [sp, #132]	; r2 = dstu		
		ldr		r0, [sp, #136]	; r0 = dstv
				
	;	Transpose
;START 	R180
		VREV64.8 q6, q6		
		VREV64.8 q4, q4			
		VREV64.8 q3, q3					
		VREV64.8 q2, q2
		;d13, d12, d9, d8, d7, d6, d5, d4
		
		VREV32.8 q5, q5						
		VREV32.8 q15, q15
		;u = d11[0] d10[0] d31[0] d30[0]
		;v = d11[1] d10[1] d31[1] d30[1]
;END R180
; STRAT TO STORE

		vst1.64	{d13}, [r7]!
		vst1.64	{d12}, [r7]!
		vst1.64	{d9}, [r7]!
		vst1.64	{d8}, [r7]!		
		vst1.64	{d7}, [r7]!
		vst1.64	{d6}, [r7]!						
		vst1.64	{d5}, [r7]!												
		vst1.64	{d4}, [r7]!

		;q2, q3, q4, q6, q15, q5	

		vst1.32	{d11[0]}, [r2]!
		vst1.32	{d11[1]}, [r0]!
		vst1.32	{d10[0]}, [r2]!
		vst1.32	{d10[1]}, [r0]!	
		vst1.32	{d31[0]}, [r2]!
		vst1.32	{d31[1]}, [r0]!								
		vst1.32	{d30[0]}, [r2]!
		vst1.32	{d30[1]}, [r0]!		
  mend

  macro 
  yuv2yuv_8x2n_num $Num
		ldr	r10, [sp, #116]		;r10 = y_scale_par
		ldr	r0, [sp, #120]		;r0 = uin_stride		
		ldr	r2, [sp, #124]		;r2 = vin_stride	
		vld1.32	{q6}, [r10]!		
		ldr	r5, [r10], #4		;r5 = y_scale_par[0]
		ldr	r6, [r10], #4		;r6 = y_scale_par[3]
		str	r10, [sp, #116]		;r10 = y_scale_par					
		ldr	r10, [sp, #12]		;r10 = src_y
		ldr	r3, [sp, #16]		;r3 = src_u							
		ldr	r4, [sp, #96]		;r4 = src_v
		mov r7, r5, lsr #1			
		mul	r5, r9, r5		;y_scale_par[0] * src_stride
		mul	r6, r9, r6		;y_scale_par[3] * src_stride	
		add	r5, r10, r5		;r5 = src_y0		
		add	r6, r10, r6		;r6 = src_y1
		mul	r0, r7, r0		;(y_scale_par[0]>>1) * uin_stride
		mul	r2, r7, r2		;(y_scale_par[0]>>1) * vin_stride
		add	r3, r3, r0		;r3 = src_u0
		add	r4, r4, r2		;r4 = src_v0
									
;line 0, 1
;///////////////////////////////////////////////
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14				
;pos0, 1
		ldr	r0, [sp, #20]	
		ldr	r10, [sp, #24]			
		;y0	
		uxtah	r7, r5, r0
		uxtah	r2, r5, r0, ror #16		
		vld2.8 {d16[0], d17[0]}, [r7], r9
		vld2.8 {d16[1], d17[1]}, [r2], r9		
		vld2.8 {d18[0], d19[0]}, [r7]
		vld2.8 {d18[1], d19[1]}, [r2]
		;y1	
		uxtah	r7, r6, r0
		uxtah	r2, r6, r0, ror #16	
		vld2.8 {d14[0], d15[0]}, [r7], r9
		vld2.8 {d14[1], d15[1]}, [r2], r9		
		vld2.8 {d28[0], d29[0]}, [r7]
		vld2.8 {d28[1], d29[1]}, [r2]		
;pos2, 3		
		;y0	
		uxtah	r7, r5, r10
		uxtah	r2, r5, r10, ror #16		
		vld2.8 {d16[2], d17[2]}, [r7], r9
		vld2.8 {d16[3], d17[3]}, [r2], r9		
		vld2.8 {d18[2], d19[2]}, [r7]
		vld2.8 {d18[3], d19[3]}, [r2]
		;y1	
		uxtah	r7, r6, r10
		uxtah	r2, r6, r10, ror #16		
		vld2.8 {d14[2], d15[2]}, [r7], r9
		vld2.8 {d14[3], d15[3]}, [r2], r9	
		vld2.8 {d28[2], d29[2]}, [r7]
		vld2.8 {d28[3], d29[3]}, [r2]
;pos4, 5
		ldr	r0, [sp, #28]		
		;y0		
		uxtah	r7, r5, r0
		uxtah	r2, r5, r0, ror #16		
		vld2.8 {d16[4], d17[4]}, [r7], r9
		vld2.8 {d16[5], d17[5]}, [r2], r9		
		vld2.8 {d18[4], d19[4]}, [r7]
		vld2.8 {d18[5], d19[5]}, [r2]
		;y1		
		uxtah	r7, r6, r0
		uxtah	r2, r6, r0, ror #16	
		vld2.8 {d14[4], d15[4]}, [r7], r9
		vld2.8 {d14[5], d15[5]}, [r2], r9	
		vld2.8 {d28[4], d29[4]}, [r7]
		vld2.8 {d28[5], d29[5]}, [r2]	
;pos6, 7	
		;y0
		uxtah	r7, r5, r12
		uxtah	r2, r5, r12, ror #16			
		vld2.8 {d16[6], d17[6]}, [r7], r9
		vld2.8 {d16[7], d17[7]}, [r2], r9	
		vld2.8 {d18[6], d19[6]}, [r7]
		vld2.8 {d18[7], d19[7]}, [r2]
		;y1		
		uxtah	r7, r6, r12
		uxtah	r2, r6, r12, ror #16		
		vld2.8 {d14[6], d15[6]}, [r7], r9
		vld2.8 {d14[7], d15[7]}, [r2], r9	
		vld2.8 {d28[6], d29[6]}, [r7]
		vld2.8 {d28[7], d29[7]}, [r2]

;now
;q6,        2   v pixel scale coffs 
;q0, q1,    8   h pixel scale coffs
;q8, q9,    8   h pixel vales 0 line
;q7, q14,   8   h pixel vales 1 line
;free register q2, q3, q4, q5, q7, q10, q11, q12, q13, q14
;y_scale_par pos  aa,     bb
;		     	pos0 d12[0], d12[1]	line 0
;		     	pos0 d13[0], d13[1]    line 1	
;x_scale_par pos aa,    bb
;			pos0 d0[0], d1[0]
;			pos1 d0[1], d1[1]
;			pos2 d0[2], d1[2]
;			pos3 d0[3], d1[3]
;		q0[aa0, aa1, aa2, aa3, bb0, bb1, bb2, bb3]
;			pos4 d2[0], d3[0]
;			pos5 d2[1], d3[1]
;			pos6 d2[2], d3[2]
;			pos7 d2[3], d3[3]
;		q1[aa0, aa1, aa2, aa3, bb0, bb1, bb2, bb3]
;y0~7 
;		d16[y0 	   y1 	y2 	 y3   y4 	   y5 	y6 	 y7]
;		d17[y0+1   y1+1 y2+1 y3+1 y4+1   y5+1 y6+1 y7+1]
;		d18[y0+s   y1+s y2+s y3+s y4+s   y5+s y6+s y7+s]
;		d19[y0+s+1 y1+s y2+s y3+s y4+s+1 y5+s y6+s y7+s]
;a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;

; line 0
;free register q2, q3, q4, q5, q7, (q10, q11, q12, q13), q14, add q8, q9
		vmovl.u8 q10, d16			;y  0~7	
		vmovl.u8 q11, d17			;y  0~7
		vmovl.u8 q12, d18			;y  0~7
		vmovl.u8 q13, d19			;y  0~7

;free register q2, q3, q4, q5, q7, (q8, q9, q10, q11), q12, q13, q14 
		vmull.u16 q8, d20, D1		;y  0~3		;src_y0[a0]*bb  
		vmull.u16 q9, d21, D3		;y  4~7		;src_y0[a0]*bb		
		vmlal.u16 q8, d22, D0		;y  0~3		;src_y0[a0]*bb + src_y0[a0+1]*aa		  
		vmlal.u16 q9, d23, D2		;y  4~7		;src_y0[a0]*bb + src_y0[a0+1]*aa		
		
		vmull.u16 q10, d24, D1		;y  0~3		;src_y0[a0+src_stride]*bb
		vmull.u16 q11, d25, D3		;y  4~7		;src_y0[a0+src_stride]*bb
		vmlal.u16 q10, d26, D0		;y  0~3		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		vmlal.u16 q11, d27, D2		;y  4~7		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		
		vmul.u32  q8, q8, d12[1]
		vmla.u32  q8, q10, d12[0]	;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0														
		vmul.u32  q9, q9, d12[1]
		vmla.u32  q9, q11, d12[0]	;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0
		vshrn.u32 d16, q8, #16		;a4  = ()>>16;
		vshrn.u32 d17, q9, #16		;a4  = ()>>16;		
;free register q2, q3, q4, q5, q7, q8, q9, q10, q11, q12, q13, q14
;		yyyy0[0~7] = q8

; line 1
;free register q2, q3, q4, q5, q7, (q10, q11, q12, q13), q14, add q7, q14
		vmovl.u8 q10, d14			;y  0~7	
		vmovl.u8 q11, d15			;y  0~7
		vmovl.u8 q12, d28			;y  0~7
		vmovl.u8 q13, d29			;y  0~7

;free register (q2, q3, q4, q5), q7, q8, q9, q10, q11, q12, q13, q14 
		vmull.u16 q7, d20, D1		;y  0~3		;src_y0[a0]*bb  
		vmull.u16 q14, d21, D3		;y  4~7		;src_y0[a0]*bb		
		vmlal.u16 q7, d22, D0		;y  0~3		;src_y0[a0]*bb + src_y0[a0+1]*aa		  
		vmlal.u16 q14, d23, D2		;y  4~7		;src_y0[a0]*bb + src_y0[a0+1]*aa		
		
		vmull.u16 q10, d24, D1		;y  0~3		;src_y0[a0+src_stride]*bb
		vmull.u16 q11, d25, D3		;y  4~7		;src_y0[a0+src_stride]*bb
		vmlal.u16 q10, d26, D0		;y  0~3		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		vmlal.u16 q11, d27, D2		;y  4~7		;src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa
		
		vmul.u32  q7, q7, d13[1]
		vmla.u32  q7, q10, d13[0]	;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0														
		vmul.u32  q14, q14, d13[1]
		vmla.u32  q14, q11, d13[0]	;(src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0
		vshrn.u32 d14, q7, #16		;a4  = ()>>16;
		vshrn.u32 d15, q14, #16		;a4  = ()>>16;
;free register q2, q3, q4, q5, q7, q8, q9, q10, q11, q12, q13, q14
;		yyyy0[0~7] = q7

  mend 
  
|cc_yuv2yuv_8x8_sR180_armv7| PROC
;void cc_yuv2yuv_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
;					VO_U8 *out_buf, VO_S32 src_stride, VO_S32 dst_stride,
;					 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
;				 const VO_S32 uin_stride, const VO_S32 vin_stride, VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv)
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
;		r4 = src_v			ldr		r4, [sp, #96]
;		r5 = out_buf			ldr		r5, [sp, #100]
;		r6 = src_stride			ldr		r6, [sp, #104]
;		r7 = dst_stride			ldr		r7, [sp, #108]
;		r8 = x_scale_par		ldr		r8, [sp, #112]
;		r9 = y_scale_par		ldr		r9, [sp, #116]
;		uin_stride			ldr		r10, [sp, #120]
;		vin_stride			ldr		r11, [sp, #124]
;		r5 = dsty		ldr		r5, [sp, #128]
;		r6 = dstu		ldr		r6, [sp, #132]
;		r7 = dstv		ldr		r7, [sp, #136]


		stmdb	sp!, {r4-r11, lr}		;save regs used
		sub	sp, sp, #60			;sp - 32

;		str	r0,	[sp, #4]		;[sp, #4] = width
;		str	r1,	[sp, #8]		;[sp, #8] = height			
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
		ldr	r12,	[sp, #112]	;[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		;r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10
;pos0, 1 r0
		uxth	r1, r0
		uxtah	r1, r1, r0, ror #16
		add	r1, r1, #1
		mov	r1, r1, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
;pos2, 3 r2
		uxth	r8, r2
		uxtah	r8, r8, r2, ror #16
		add	r8, r8, #1
		mov	r8, r8, lsr #2		;a1 = (a0 + a6 + 1)>>2;
;pos4, 5 r7
		uxth	r11, r7
		uxtah	r11, r11, r7, ror #16
		add	r11, r11, #1
		mov	r11, r11, lsr #2		;a1 = (a0 + a6 + 1)>>2;	
;pos6, 7 r12
		uxth	r14, r12
		uxtah	r14, r14, r12, ror #16
		add	r14, r14, #1
		mov	r14, r14, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
		
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14, r12					
;BIG_WHILE_D

	yuv2yuv_8x2n_num 0	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d4, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d5, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d30), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d30[0]}, [r7]
		vld1.8 {d30[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d30[1]}, [r7]	
		vld1.8 {d30[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d30[2]}, [r7]	
		vld1.8 {d30[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d30[3]}, [r7]	
		vld1.8 {d30[7]}, [r2]		
	yuv2yuv_8x2n_num 1	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d6, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d7, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d31), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d31[0]}, [r7]
		vld1.8 {d31[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d31[1]}, [r7]	
		vld1.8 {d31[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d31[2]}, [r7]	
		vld1.8 {d31[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d31[3]}, [r7]	
		vld1.8 {d31[7]}, [r2]				
	yuv2yuv_8x2n_num 2	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d8, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d9, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q5(d10), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d10[0]}, [r7]
		vld1.8 {d10[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d10[1]}, [r7]	
		vld1.8 {d10[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d10[2]}, [r7]	
		vld1.8 {d10[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d10[3]}, [r7]	
		vld1.8 {d10[7]}, [r2]		
	yuv2yuv_8x2n_num 3	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d12, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d13, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q5(d11), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d11[0]}, [r7]
		vld1.8 {d11[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d11[1]}, [r7]	
		vld1.8 {d11[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d11[2]}, [r7]	
		vld1.8 {d11[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d11[3]}, [r7]	
		vld1.8 {d11[7]}, [r2]								

;store dest yuv
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14
;		ldr		r7, [sp, #128]	; r7 = dsty		
;		ldr		r2, [sp, #132]	; r2 = dstu		
;		ldr		r0, [sp, #136]	; r0 = dstv
;		vst1.64	{q2}, [r7]!
;		vst1.64	{q3}, [r7]!
;		vst1.64	{q4}, [r7]!
;		vst1.64	{q6}, [r7]!
;		;q2, q3, q4, q6, q15, q5	
;		vst1.32	{d30[0]}, [r2]!
;		vst1.32	{d30[1]}, [r0]!						
;		vst1.32	{d31[0]}, [r2]!
;		vst1.32	{d31[1]}, [r0]!				
;		vst1.32	{d10[0]}, [r2]!
;		vst1.32	{d10[1]}, [r0]!
;		vst1.32	{d11[0]}, [r2]!
;		vst1.32	{d11[1]}, [r0]!

	Transpose_R180
;store end
			
;		subs	r1, r1, #2								
;		bne	BIG_WHILE_D
;}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		; restore and return 


|cc_yuv2yuv_8x2n_sR180_armv7| PROC
;void cc_yuv2yuv_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
;					VO_U8 *out_buf, VO_S32 src_stride, VO_S32 dst_stride,
;					 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
;				 const VO_S32 uin_stride, const VO_S32 vin_stride, VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv)
;		r0 = width
;		r1 = height
;		r2 = src_y
;		r3 = src_u
;		r4 = src_v			ldr		r4, [sp, #96]
;		r5 = out_buf			ldr		r5, [sp, #100]
;		r6 = src_stride			ldr		r6, [sp, #104]
;		r7 = dst_stride			ldr		r7, [sp, #108]
;		r8 = x_scale_par		ldr		r8, [sp, #112]
;		r9 = y_scale_par		ldr		r9, [sp, #116]
;		uin_stride			ldr		r10, [sp, #120]
;		vin_stride			ldr		r11, [sp, #124]
;		r5 = dsty		ldr		r5, [sp, #128]
;		r6 = dstu		ldr		r6, [sp, #132]
;		r7 = dstv		ldr		r7, [sp, #136]


		stmdb	sp!, {r4-r11, lr}		;save regs used
		sub	sp, sp, #60			;sp - 32


		cmp	r1, #2
		beq	lab_2222 
		cmp	r1, #4
		beq	lab_4444 
		cmp	r1, #6
		beq	lab_6666

lab_2222
;aaaaaaaaaaaaaaaaaaaaaa
;		str	r0,	[sp, #4]		;[sp, #4] = width
;		str	r1,	[sp, #8]		;[sp, #8] = height			
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
		ldr	r12,	[sp, #112]	;[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		;r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10
;pos0, 1 r0
		uxth	r1, r0
		uxtah	r1, r1, r0, ror #16
		add	r1, r1, #1
		mov	r1, r1, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
;pos2, 3 r2
		uxth	r8, r2
		uxtah	r8, r8, r2, ror #16
		add	r8, r8, #1
		mov	r8, r8, lsr #2		;a1 = (a0 + a6 + 1)>>2;
;pos4, 5 r7
		uxth	r11, r7
		uxtah	r11, r11, r7, ror #16
		add	r11, r11, #1
		mov	r11, r11, lsr #2		;a1 = (a0 + a6 + 1)>>2;	
;pos6, 7 r12
		uxth	r14, r12
		uxtah	r14, r14, r12, ror #16
		add	r14, r14, #1
		mov	r14, r14, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
		
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14, r12					
;BIG_WHILE_D

	yuv2yuv_8x2n_num 0	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d4, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d5, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d30), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d30[0]}, [r7]
		vld1.8 {d30[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d30[1]}, [r7]	
		vld1.8 {d30[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d30[2]}, [r7]	
		vld1.8 {d30[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d30[3]}, [r7]	
		vld1.8 {d30[7]}, [r2]		
;store dest yuv
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14
;		ldr		r7, [sp, #128]	; r7 = dsty		
;		ldr		r2, [sp, #132]	; r2 = dstu		
;		ldr		r0, [sp, #136]	; r0 = dstv
;		vst1.64	{q2}, [r7]!
;		;q2, q3, q4, q6, q15, q5	
;		vst1.32	{d30[0]}, [r2]!
;		vst1.32	{d30[1]}, [r0]!

	Transpose_R180
;store end
			
;		subs	r1, r1, #2								
;		bne	BIG_WHILE_D
;}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		; restore and return 
;aaaaaaaaaaaaaaaaaaaaaaaaaa


lab_4444
;aaaaaaaaaaaaaaaaaaaaaa
;		str	r0,	[sp, #4]		;[sp, #4] = width
;		str	r1,	[sp, #8]		;[sp, #8] = height			
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
		ldr	r12,	[sp, #112]	;[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		;r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10
;pos0, 1 r0
		uxth	r1, r0
		uxtah	r1, r1, r0, ror #16
		add	r1, r1, #1
		mov	r1, r1, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
;pos2, 3 r2
		uxth	r8, r2
		uxtah	r8, r8, r2, ror #16
		add	r8, r8, #1
		mov	r8, r8, lsr #2		;a1 = (a0 + a6 + 1)>>2;
;pos4, 5 r7
		uxth	r11, r7
		uxtah	r11, r11, r7, ror #16
		add	r11, r11, #1
		mov	r11, r11, lsr #2		;a1 = (a0 + a6 + 1)>>2;	
;pos6, 7 r12
		uxth	r14, r12
		uxtah	r14, r14, r12, ror #16
		add	r14, r14, #1
		mov	r14, r14, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
		
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14, r12					
;BIG_WHILE_D

	yuv2yuv_8x2n_num 0	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d4, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d5, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d30), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d30[0]}, [r7]
		vld1.8 {d30[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d30[1]}, [r7]	
		vld1.8 {d30[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d30[2]}, [r7]	
		vld1.8 {d30[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d30[3]}, [r7]	
		vld1.8 {d30[7]}, [r2]		
	yuv2yuv_8x2n_num 1	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d6, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d7, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d31), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d31[0]}, [r7]
		vld1.8 {d31[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d31[1]}, [r7]	
		vld1.8 {d31[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d31[2]}, [r7]	
		vld1.8 {d31[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d31[3]}, [r7]	
		vld1.8 {d31[7]}, [r2]				
;store dest yuv
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14
;		ldr		r7, [sp, #128]	; r7 = dsty		
;		ldr		r2, [sp, #132]	; r2 = dstu		
;		ldr		r0, [sp, #136]	; r0 = dstv
;		vst1.64	{q2}, [r7]!
;		vst1.64	{q3}, [r7]!
;		;q2, q3, q4, q6, q15, q5	
;		vst1.32	{d30[0]}, [r2]!
;		vst1.32	{d30[1]}, [r0]!						
;		vst1.32	{d31[0]}, [r2]!
;		vst1.32	{d31[1]}, [r0]!
	Transpose_R180
;store end
			
;		subs	r1, r1, #2								
;		bne	BIG_WHILE_D
;}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		; restore and return 
;aaaaaaaaaaaaaaaaaaaaaaaaaa


lab_6666
;aaaaaaaaaaaaaaaaaaaaaa
;		str	r0,	[sp, #4]		;[sp, #4] = width
;		str	r1,	[sp, #8]		;[sp, #8] = height			
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
		ldr	r12,	[sp, #112]	;[sp, #112] = x_scale_par
		ldr	r9, [sp, #104]		;r9 = src_stride
		
		ldr	r0, [r12], #4
		ldr	r2, [r12], #4
		vld1.16	{q0}, [r12]!				
		ldr	r7, [r12], #4
		ldr	r10, [r12], #4
		vld1.16	{q1}, [r12]
		str	r0, [sp, #20]
		str	r2, [sp, #24]
		str	r7, [sp, #28]
		mov	r12, r10
;pos0, 1 r0
		uxth	r1, r0
		uxtah	r1, r1, r0, ror #16
		add	r1, r1, #1
		mov	r1, r1, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
;pos2, 3 r2
		uxth	r8, r2
		uxtah	r8, r8, r2, ror #16
		add	r8, r8, #1
		mov	r8, r8, lsr #2		;a1 = (a0 + a6 + 1)>>2;
;pos4, 5 r7
		uxth	r11, r7
		uxtah	r11, r11, r7, ror #16
		add	r11, r11, #1
		mov	r11, r11, lsr #2		;a1 = (a0 + a6 + 1)>>2;	
;pos6, 7 r12
		uxth	r14, r12
		uxtah	r14, r14, r12, ror #16
		add	r14, r14, #1
		mov	r14, r14, lsr #2		;a1 = (a0 + a6 + 1)>>2;		
		
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14, r12					
;BIG_WHILE_D

	yuv2yuv_8x2n_num 0	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d4, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d5, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d30), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d30[0]}, [r7]
		vld1.8 {d30[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d30[1]}, [r7]	
		vld1.8 {d30[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d30[2]}, [r7]	
		vld1.8 {d30[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d30[3]}, [r7]	
		vld1.8 {d30[7]}, [r2]		
	yuv2yuv_8x2n_num 1	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d6, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d7, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q15(d31), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d31[0]}, [r7]
		vld1.8 {d31[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d31[1]}, [r7]	
		vld1.8 {d31[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d31[2]}, [r7]	
		vld1.8 {d31[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d31[3]}, [r7]	
		vld1.8 {d31[7]}, [r2]				
	yuv2yuv_8x2n_num 2	
;out_put
		;uuuu0[0~3] = d30[0~3], vvvv0[0~3] = d30[4~7]	
		vshrn.u16 d8, q8, #4		;a4  = ()>>20;        yyyy0[0~7]			
		vshrn.u16 d9, q7, #4		;a4  = ()>>20;        yyyy1[0~7]
;q5(d10), 4x2 h pixel uv vales 0 line
		;uv
		add	r7, r1, r3
		add	r2, r1, r4		
		vld1.8 {d10[0]}, [r7]
		vld1.8 {d10[4]}, [r2]		
		;uv
		add	r7, r8, r3
		add	r2, r8, r4					
		vld1.8 {d10[1]}, [r7]	
		vld1.8 {d10[5]}, [r2]		
		;uv
		add	r7, r11, r3
		add	r2, r11, r4					
		vld1.8 {d10[2]}, [r7]	
		vld1.8 {d10[6]}, [r2]		
		;uv		
		add	r7, r14, r3
		add	r2, r14, r4					
		vld1.8 {d10[3]}, [r7]	
		vld1.8 {d10[7]}, [r2]		
;store dest yuv
;now r5 = src_y0, r6 = src_y1, r3 = src_u0, r4 = src_v0 		
;r12 = x_scale_par  ;r9 = src_stride
;free registers r0, r2, r7, r10, havn't used r1, r8, r11, r14
;		ldr		r7, [sp, #128]	; r7 = dsty		
;		ldr		r2, [sp, #132]	; r2 = dstu		
;		ldr		r0, [sp, #136]	; r0 = dstv
;		vst1.64	{q2}, [r7]!
;		vst1.64	{q3}, [r7]!
;		vst1.64	{q4}, [r7]!
;		;q2, q3, q4, q6, q15, q5	
;		vst1.32	{d30[0]}, [r2]!
;		vst1.32	{d30[1]}, [r0]!						
;		vst1.32	{d31[0]}, [r2]!
;		vst1.32	{d31[1]}, [r0]!				
;		vst1.32	{d10[0]}, [r2]!
;		vst1.32	{d10[1]}, [r0]!
	Transpose_R180
;store end
			
;		subs	r1, r1, #2								
;		bne	BIG_WHILE_D
;}
        	add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		; restore and return 
;aaaaaaaaaaaaaaaaaaaaaaaaaa


	ENDP  ;
		
END
