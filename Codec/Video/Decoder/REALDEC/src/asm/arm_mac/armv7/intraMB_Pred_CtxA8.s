@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************

#include "../../voASMPort.h"
	
@R14, never cover r14
@R12 = Tmp

#define pred16x16 TESTDEF
	@AREA	|.text|, CODE
	.text	
	.align 4
	.globl	VOASMFUNCNAME(pred16x16_vertical_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_horizontal_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_128_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_left_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_top_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_plane_CtxA8)
	.globl	VOASMFUNCNAME(pred16x16_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_vertical_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_horizontal_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_128_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_left_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_top_dc_CtxA8)
	.globl	VOASMFUNCNAME(pred8x8_dc_CtxA8)
	
	.macro M_pushLR
    str		lr,  [sp, #-4]!
	.endm

	.macro M_popLR
    ldr		pc,  [sp], #4
	.endm
	
	.macro M_LRtoPC
	mov			pc, lr
	.endm
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_vertical_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_vertical_CtxA8):	@PROC

 @   const U32 a= ((U32*)(src-src_stride))[0]@
 @   const U32 b= ((U32*)(src-src_stride))[1]@
 @	 const U32 c= ((U32*)(src-src_stride))[2]@
 @   const U32 d= ((U32*)(src-src_stride))[3]@
 @   for(i=0@ i<16@ i++){
 @	     ((U32*)dst)[0]= a@
 @       ((U32*)dst)[1]= b@
	@     ((U32*)dst)[2]= c@
	@     ((U32*)dst)[3]= d@
	@ 	dst += dst_stride@
	@ }
	 
	sub			r0, r0, r2
	vld1.64		{q0}, [r0]
	
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1]
		
	M_LRtoPC
	@ENDP

	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_horizontal_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_horizontal_CtxA8):	@PROC
	
  @  for(i=0@ i<16@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]=
  @      ((U32*)dst)[2]=
  @	     ((U32*)dst)[3]= src[-1+i*src_stride]*0x01010101@
  @  	dst += dst_stride@
  @	 }

	sub			r0, r0, #1
	vld1.8		{d0[],d1[]}, [r0], r2
	vld1.8		{d2[],d3[]}, [r0], r2
	vld1.8		{d4[],d5[]}, [r0], r2
	vld1.8		{d6[],d7[]}, [r0], r2
	vld1.8		{d8[],d9[]}, [r0], r2
	vld1.8		{d10[],d11[]}, [r0], r2
	vld1.8		{d12[],d13[]}, [r0], r2
	vld1.8		{d14[],d15[]}, [r0], r2
	vld1.8		{d16[],d17[]}, [r0], r2
	vld1.8		{d18[],d19[]}, [r0], r2
	vld1.8		{d20[],d21[]}, [r0], r2
	vld1.8		{d22[],d23[]}, [r0], r2
	vld1.8		{d24[],d25[]}, [r0], r2
	vld1.8		{d26[],d27[]}, [r0], r2
	vld1.8		{d28[],d29[]}, [r0], r2
	vld1.8		{d30[],d31[]}, [r0]
	
	vst1.64		{q0}, [r1], r3
	vst1.64		{q1}, [r1], r3
	vst1.64		{q2}, [r1], r3
	vst1.64		{q3}, [r1], r3
	vst1.64		{q4}, [r1], r3
	vst1.64		{q5}, [r1], r3
	vst1.64		{q6}, [r1], r3
	vst1.64		{q7}, [r1], r3
	vst1.64		{q8}, [r1], r3
	vst1.64		{q9}, [r1], r3
	vst1.64		{q10}, [r1], r3
	vst1.64		{q11}, [r1], r3
	vst1.64		{q12}, [r1], r3
	vst1.64		{q13}, [r1], r3
	vst1.64		{q14}, [r1], r3
	vst1.64		{q15}, [r1]
		
	M_LRtoPC
	@ENDP
	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_128_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_128_dc_CtxA8):	@PROC
	
  @  for(i=0@ i<16@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]=
  @      ((U32*)dst)[2]=
  @	     ((U32*)dst)[3]= 0x01010101U*128U@
  @  	dst += dst_stride@
  @	 }

	vmov.i8		q0, #0x80  		
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1]
		
	M_LRtoPC
	@ENDP
	
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_left_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_left_dc_CtxA8):	@PROC
	
  @  for(i=0@i<16@ i++){
  @      dc+= src[-1+i*src_stride]@
  @  }
  @  dc= 0x01010101*((dc + 8)>>4)@
  @  for(i=0@ i<16@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]=
@	     ((U32*)dst)[2]=
  @      ((U32*)dst)[3]= dc@
	@ 	dst += dst_stride@
	@ }

	sub			r0, r0, #1
	vld1.8		{d0[0]}, [r0], r2
	vld1.8		{d0[1]}, [r0], r2
	vld1.8		{d0[2]}, [r0], r2
	vld1.8		{d0[3]}, [r0], r2
	vld1.8		{d0[4]}, [r0], r2
	vld1.8		{d0[5]}, [r0], r2
	vld1.8		{d0[6]}, [r0], r2
	vld1.8		{d0[7]}, [r0], r2
	vld1.8		{d1[0]}, [r0], r2
	vld1.8		{d1[1]}, [r0], r2
	vld1.8		{d1[2]}, [r0], r2
	vld1.8		{d1[3]}, [r0], r2
	vld1.8		{d1[4]}, [r0], r2
	vld1.8		{d1[5]}, [r0], r2
	vld1.8		{d1[6]}, [r0], r2
	vld1.8		{d1[7]}, [r0]

	vmov.u16	q1, #8
	vaddl.u8	q0, d0, d1
	vadd.u16	d0, d0, d1
	vpaddl.u16	d0, d0
	vpadd.u32	d0, d0
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q1
	vshrn.u16	d0, q1, #4
	vshrn.u16	d1, q1, #4

	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1]
		
	M_LRtoPC
	@ENDP

	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_top_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_top_dc_CtxA8):	@PROC
	
  @  for(i=0@i<16@ i++){
  @      dc+= src[i-src_stride]@
  @  }
  @  dc= 0x01010101*((dc + 8)>>4)@
  @  for(i=0@ i<16@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]=
@	     ((U32*)dst)[2]=
  @      ((U32*)dst)[3]= dc@
	@ 	dst += dst_stride@
	@ }
  
	sub			r0, r0, r2
	vld1.64		{q0}, [r0]
	vmov.u16	q1, #8
	vaddl.u8	q0, d0, d1
	vadd.u16	d0, d0, d1
	vpaddl.u16	d0, d0
	vpadd.u32	d0, d0
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q1
	vshrn.u16	d0, q1, #4
	vshrn.u16	d1, q1, #4
	
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1]
		
	M_LRtoPC
	@ENDP
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_dc_CtxA8):	@PROC
    
 @   for(i=0@i<16@ i++){
 @       dc+= src[-1+i*src_stride]@
 @   }
 @   for(i=0@i<16@ i++){
 @       dc+= src[i-src_stride]@
 @   }
 @   dc= 0x01010101*((dc + 16)>>5)@
 @   for(i=0@ i<16@ i++){
 @       ((U32*)dst)[0]=
 @       ((U32*)dst)[1]=
	@     ((U32*)dst)[2]=
 @       ((U32*)dst)[3]= dc@
	@ 	dst += dst_stride@
	@ }
    
	sub			r12, r0, r2
	sub			r0, r0, #1
	vld1.64		{q1}, [r12]
	vld1.8		{d0[0]}, [r0], r2
	vld1.8		{d0[1]}, [r0], r2
	vld1.8		{d0[2]}, [r0], r2
	vld1.8		{d0[3]}, [r0], r2
	vld1.8		{d0[4]}, [r0], r2
	vld1.8		{d0[5]}, [r0], r2
	vld1.8		{d0[6]}, [r0], r2
	vld1.8		{d0[7]}, [r0], r2
	vld1.8		{d1[0]}, [r0], r2
	vld1.8		{d1[1]}, [r0], r2
	vld1.8		{d1[2]}, [r0], r2
	vld1.8		{d1[3]}, [r0], r2
	vld1.8		{d1[4]}, [r0], r2
	vld1.8		{d1[5]}, [r0], r2
	vld1.8		{d1[6]}, [r0], r2
	vld1.8		{d1[7]}, [r0]
	
	vmov.u16	q15, #16
	vaddl.u8	q0, d0, d1
	vaddl.u8	q1, d2, d3
	vadd.u16	q0, q0, q1
	vadd.u16	d0, d0, d1
	vpaddl.u16	d0, d0
	vpaddl.u32	d0, d0
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q15
	vshrn.u16	d0, q1, #5
	vshrn.u16	d1, q1, #5
	
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1], r3
	vst1.64		{q0}, [r1]
		
	M_LRtoPC
	@ENDP
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred16x16_plane_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.align 8	
	
Table1:	 
  .short 8,7,6,5,4,3,2,1	

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred16x16_plane_CtxA8):	@PROC
    
@q13 = a, q14 = b, q15 = c

@	iH = 0@	
@	iH +=	(tp[8 ] - tp[6])@
@	iH +=	(tp[9 ] - tp[5]) << 1@
@	iH += 3*(tp[10] - tp[4])@
@	iH +=	(tp[11] - tp[3]) << 2@
@	iH += 5*(tp[12] - tp[2])@
@	iH += 6*(tp[13] - tp[1])@
@	iH += 7*(tp[14] - tp[0])@
@	iH +=	(tp[15] - t0) << 3@

@	iV = 0@
@	iV +=	(lp[8 ] - lp[6])@
@	iV +=	(lp[9 ] - lp[5]) << 1@
@	iV += 3*(lp[10] - lp[4])@
@	iV +=	(lp[11] - lp[3]) << 2@
@	iV += 5*(lp[12] - lp[2])@
@	iV += 6*(lp[13] - lp[1])@
@	iV += 7*(lp[14] - lp[0])@
@	iV +=	(lp[15] - t0) << 3@
@
@	a = (tp[15] + lp[15])<<4@
@	b = (iH + (iH>>2))>>4@
@	c = (iV + (iV>>2))>>4@

	sub			r0, r0, r2
	sub			r12, r0, #1
	vld1.64		{q0}, [r0]
	@ldr			r0, =Table1
	adr			  r0, Table1
	vld1.8		{d8[]}, [r12], r2
	vld1.8		d2[0], [r12], r2
	vld1.8		d2[1], [r12], r2
	vld1.8		d2[2], [r12], r2
	vld1.8		d2[3], [r12], r2
	vld1.8		d2[4], [r12], r2
	vld1.8		d2[5], [r12], r2
	vld1.8		d2[6], [r12], r2
	vld1.8		d2[7], [r12], r2	@this byte is not used.
	vld1.8		d3[7], [r12], r2
	vld1.8		d3[6], [r12], r2
	vld1.8		d3[5], [r12], r2
	vld1.8		d3[4], [r12], r2
	vld1.8		d3[3], [r12], r2
	vld1.8		d3[2], [r12], r2
	vld1.8		d3[1], [r12], r2
	vld1.8		d3[0], [r12]
	vld1.64		{q12}, [r0]
	
	vext.8		d0, d8, d0, #7
	vrev64.8	d1, d1
	vext.8		d2, d8, d2, #7
	vsubl.u8	q2, d1, d0
	vsubl.u8	q3, d3, d2
	vmul.s16	q2, q2, q12		@iH
	vdup.8		d0, d1[0]		@tp[15]
	vdup.8		d1, d3[0]		@lp[15]
	vaddl.u8	q0, d0, d1
	vmul.s16	q3, q3, q12		@iV
	vshl.u16	q13, q0, #4		@a
	vpaddl.s16	q2, q2
	vmov.i16	q6, #16
	vpaddl.s32	q2, q2
	vpaddl.s16	q3, q3
	vadd.s32	d4, d4, d5
	vpaddl.s32	q3, q3
	vadd.s16	q13, q13, q6	@a+16
	vadd.s32	d6, d6, d7
	vdup.16		q0, d4[0]		@iH
	vdup.16		q1, d6[0]		@iV
	vshr.s16	q14, q0, #2	
	vshr.s16	q15, q1, #2
	vadd.s16	q14, q14, q0
	vadd.s16	q15, q15, q1
	vshr.s16	q14, #4			@b
	vshr.s16	q15, #4			@c
	
@	pTemp  = (PU32)dst@
@	lTemp1 = a - 7*(b+c) + 16@
@	for (j=0@ j<16@ j++)
@	{
@		temp = lTemp1@
@		for (i=0@ i<16@ i += 4)
@		{
@			lTemp  = ClampVal(temp >> 5)@
@			temp += b@			
@			lTemp |= (ClampVal(temp >> 5) << 8)@
@			temp += b@				
@			lTemp |= (ClampVal(temp >> 5) << 16)@
@			temp += b@
@			lTemp |= (ClampVal(temp >> 5) << 24)@
@			temp += b@
@			*pTemp++ = lTemp@			
@		}
@		lTemp1 += c@
@	}

	vadd.s16	q0, q14, q15	@b+c
	vshl.u16	q1, q0, #3		
	vadd.s16	q13, q13, q0	@
	vrev64.16	q12, q12
	vswp		d24, d25		@q12: 8,7,6,5,4,3,2,1
	vmul.s16	q10, q14, q12	@8b7b6b5b4b3b2b1b
	vsub.s16	q13, q13, q1	@lTemp1 = a - 7*(b+c) + 16@
	vshl.u16	q9, q14, #3		@8b8b8b8b8b8b8b8b
	vsub.s16	q10, q10, q14	@7b6b5b4b3b2b1b0b
	vadd.s16	q11, q10, q9	@15b14b13b12b11b10b9b8b
	vadd.s16	q8, q13, q10
	vadd.s16	q9, q13, q11	@q8,q9 : bechmark of one row 
	
	vqshrun.s16	d0, q8, #5		@row 0
	vqshrun.s16	d1, q9, #5	
	vadd.s16	q8, q8, q15		@row 1
	vadd.s16	q9, q9, q15
	vqshrun.s16	d2, q8, #5
	vqshrun.s16	d3, q9, #5	
	vadd.s16	q8, q8, q15		@row 2
	vadd.s16	q9, q9, q15
	vqshrun.s16	d4, q8, #5	
	vqshrun.s16	d5, q9, #5
	vadd.s16	q8, q8, q15		@row 3
	vadd.s16	q9, q9, q15
	vqshrun.s16	d6, q8, #5	
	vqshrun.s16	d7, q9, #5
	vadd.s16	q8, q8, q15		@row 4
	vadd.s16	q9, q9, q15
	vqshrun.s16	d8, q8, #5
	vqshrun.s16	d9, q9, #5	
	vadd.s16	q8, q8, q15		@row 5
	vadd.s16	q9, q9, q15
	vqshrun.s16	d10, q8, #5
	vqshrun.s16	d11, q9, #5	
	vadd.s16	q8, q8, q15		@row 6
	vadd.s16	q9, q9, q15
	vqshrun.s16	d12, q8, #5	
	vqshrun.s16	d13, q9, #5
	vadd.s16	q8, q8, q15		@row 7
	vadd.s16	q9, q9, q15
	vqshrun.s16	d14, q8, #5	
	vqshrun.s16	d15, q9, #5
	vst1.64		{q0}, [r1], r3
	vst1.64		{q1}, [r1], r3
	vst1.64		{q2}, [r1], r3
	vst1.64		{q3}, [r1], r3
	vst1.64		{q4}, [r1], r3
	vst1.64		{q5}, [r1], r3
	vst1.64		{q6}, [r1], r3
	vst1.64		{q7}, [r1], r3
	vadd.s16	q8, q8, q15		@row 8
	vadd.s16	q9, q9, q15
	vqshrun.s16	d0, q8, #5
	vqshrun.s16	d1, q9, #5	
	vadd.s16	q8, q8, q15		@row 9
	vadd.s16	q9, q9, q15
	vqshrun.s16	d2, q8, #5
	vqshrun.s16	d3, q9, #5	
	vadd.s16	q8, q8, q15		@row 10
	vadd.s16	q9, q9, q15
	vqshrun.s16	d4, q8, #5	
	vqshrun.s16	d5, q9, #5
	vadd.s16	q8, q8, q15		@row 11
	vadd.s16	q9, q9, q15
	vqshrun.s16	d6, q8, #5	
	vqshrun.s16	d7, q9, #5
	vadd.s16	q8, q8, q15		@row 12
	vadd.s16	q9, q9, q15
	vqshrun.s16	d8, q8, #5
	vqshrun.s16	d9, q9, #5	
	vadd.s16	q8, q8, q15		@row 13
	vadd.s16	q9, q9, q15
	vqshrun.s16	d10, q8, #5
	vqshrun.s16	d11, q9, #5	
	vadd.s16	q8, q8, q15		@row 14
	vadd.s16	q9, q9, q15
	vqshrun.s16	d12, q8, #5	
	vqshrun.s16	d13, q9, #5
	vadd.s16	q8, q8, q15		@row 15
	vadd.s16	q9, q9, q15
	vqshrun.s16	d14, q8, #5	
	vqshrun.s16	d15, q9, #5
	vst1.64		{q0}, [r1], r3
	vst1.64		{q1}, [r1], r3
	vst1.64		{q2}, [r1], r3
	vst1.64		{q3}, [r1], r3
	vst1.64		{q4}, [r1], r3
	vst1.64		{q5}, [r1], r3
	vst1.64		{q6}, [r1], r3
	vst1.64		{q7}, [r1]
	
	M_LRtoPC
	@ENDP
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_vertical_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_vertical_CtxA8):	@PROC

 @   const U32 a= ((U32*)(src-src_stride))[0]@
 @   const U32 b= ((U32*)(src-src_stride))[1]@

 @   for(i=0@ i<8@ i++){
 @       ((U32*)dst)[0]= a@
 @       ((U32*)dst)[1]= b@
 @		dst += dst_stride@
 @   }
  
	sub			r0, r0, r2
	vld1.64		{d0}, [r0]
	
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1]
		
	M_LRtoPC
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_horizontal_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_horizontal_CtxA8):	@PROC
	
  @  I32 i@
  @  for(i=0@ i<8@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]= src[-1+i*src_stride]*0x01010101@
  @		dst += dst_stride@
  @  }
  
	sub			r0, r0, #1
	vld1.8		{d0[]}, [r0], r2
	vld1.8		{d1[]}, [r0], r2
	vld1.8		{d2[]}, [r0], r2
	vld1.8		{d3[]}, [r0], r2
	vld1.8		{d4[]}, [r0], r2
	vld1.8		{d5[]}, [r0], r2
	vld1.8		{d6[]}, [r0], r2
	vld1.8		{d7[]}, [r0]
	
	vst1.64		d0, [r1], r3
	vst1.64		d1, [r1], r3
	vst1.64		d2, [r1], r3
	vst1.64		d3, [r1], r3
	vst1.64		d4, [r1], r3
	vst1.64		d5, [r1], r3
	vst1.64		d6, [r1], r3
	vst1.64		d7, [r1]
		
	M_LRtoPC
	@ENDP	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_128_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_128_dc_CtxA8):	@PROC
	
  @  for(i=0@ i<8@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]= 0x01010101U*128U@
  @  	dst += dst_stride@
  @	 }

	vmov.i8		d0, #0x80  		
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1]
		
	M_LRtoPC
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_left_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_left_dc_CtxA8):	@PROC
	
  @  for(i=0@i<8@ i++){
  @      dc+= src[-1+i*src_stride]@
  @  }
  @  dc= 0x01010101*((dc + 4)>>3)@
  @  for(i=0@ i<8@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]= dc@
@		dst += dst_stride@
  @  }

	sub			r0, r0, #1
	vld1.8		{d0[0]}, [r0], r2
	vld1.8		{d0[1]}, [r0], r2
	vld1.8		{d0[2]}, [r0], r2
	vld1.8		{d0[3]}, [r0], r2
	vld1.8		{d0[4]}, [r0], r2
	vld1.8		{d0[5]}, [r0], r2
	vld1.8		{d0[6]}, [r0], r2
	vld1.8		{d0[7]}, [r0]

	vmov.u16	q1, #4
	vpaddl.u8	d0, d0
	vpaddl.u16	d0, d0
	vpadd.u32	d0, d0
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q1
	vshrn.u16	d0, q1, #3

	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1]
		
	M_LRtoPC
	@ENDP

	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_top_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_top_dc_CtxA8):	@PROC
	
  @  for(i=0@i<8@ i++){
  @      dc+= src[i-src_stride]@
  @  }
  @  dc= 0x01010101*((dc + 4)>>3)@
  @  for(i=0@ i<8@ i++){
  @      ((U32*)dst)[0]=
  @      ((U32*)dst)[1]= dc@
@		dst += dst_stride@
  @  }
  
	sub			r0, r0, r2
	vld1.64		{d0}, [r0]
	vmov.u16	q1, #4
	vpaddl.u8	d0, d0
	vpaddl.u16	d0, d0
	vpadd.u32	d0, d0
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q1
	vshrn.u16	d0, q1, #3
	
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1]
		
	M_LRtoPC
	@ENDP
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@	static void pred8x8_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@R0 = src
@R1 = dst
@R2 = src_stride
@R3 = dst_stride

VOASMFUNCNAME(pred8x8_dc_CtxA8):	@PROC

 @   for(i=0@i<8@ i++){
 @       dc+= src[-1+i*src_stride]@
 @   }
 @   for(i=0@i<8@ i++){
 @       dc+= src[i-src_stride]@
 @   }
 @   dc= 0x01010101*((dc + 8)>>4)@
 @   for(i=0@ i<8@ i++){
 @       ((U32*)dst)[0]=
 @       ((U32*)dst)[1]= dc@
@		dst += dst_stride@
 @   }	
  
	sub			r12, r0, r2
	sub			r0, r0, #1
	vld1.64		{d1}, [r12]
	vld1.8		{d0[0]}, [r0], r2
	vld1.8		{d0[1]}, [r0], r2
	vld1.8		{d0[2]}, [r0], r2
	vld1.8		{d0[3]}, [r0], r2
	vld1.8		{d0[4]}, [r0], r2
	vld1.8		{d0[5]}, [r0], r2
	vld1.8		{d0[6]}, [r0], r2
	vld1.8		{d0[7]}, [r0]
	
	vmov.u16	q1, #8
	vaddl.u8	q0, d0, d1
	vpaddl.u16	q0, q0
	vpaddl.u32	q0, q0
	vadd.u16	d0, d0, d1
	vdup.16		q0, d0[0]
	vadd.u16	q1, q0, q1
	vshrn.u16	d0, q1, #4
	
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1], r3
	vst1.64		d0, [r1]
		
	M_LRtoPC
	@ENDP
	
	
	@END