@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2009					*
@*																			*
@*****************************************************************************	
@R0 = pSrc
@R1 = pDst
@R2 = uSrcPitch
@R3 = uDstPitch
@R12 = Tmp
@R14 ,never cover the value of r14 !!!!

#include "../../voASMPort.h"

	@AREA	|.text|, CODE
	.text
	.align 4
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H00V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H01V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H02V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H03V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H00V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H01V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H02V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H03V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H00V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H01V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H02V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H03V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H00V03_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H01V03_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_Chroma_H02V03_CtxA8)
	
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V00_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V01_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V02_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V03_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V03_CtxA8)
	.globl	VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V03_CtxA8)
							   
	.macro	M_pushLR
    str		lr, [sp, #-4]!
	.endm

	.macro M_popLR
    ldr		pc, [sp], #4
	.endm

	.macro M_LRtoPC
    mov		pc, lr
	.endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.macro M_StoreData4x4 @pDst, stride
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	vst1.32		d0[0], [$0], $1
	vst1.32		d1[0], [$0], $1
	vst1.32		d2[0], [$0], $1
	vst1.32		d3[0], [$0]
	.endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

_getSrc_5x4:		@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
@	pld			[r0]
	ands		r12, r0, #0x03
	bic			r0, r0, #0x03
	vld1.64		{d0}, [r0], r2
	vld1.64		{d2}, [r0], r2
	vld1.64		{d4}, [r0], r2
	vld1.64		{d6}, [r0]
	moveq		pc, lr
	
LABx_5x4:
	lsl			  r12, r12, #3
	vmov.i64	q15, #0
	vmov.s32	d31[0], r12
	vsub.u64	d31, d30, d31
	vshl.u64	d0, d0, d31
	vshl.u64	d2, d2, d31
	vshl.u64	d4, d4, d31
	vshl.u64	d6, d6, d31
	M_LRtoPC	

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

_getSrc_5x5:		@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
@	pld			[r0]
	ands		r12, r0, #0x03
	bic			r0, r0, #0x03
	vld1.64		{d0}, [r0], r2
	vld1.64		{d2}, [r0], r2
	vld1.64		{d4}, [r0], r2
	vld1.64		{d6}, [r0], r2
	vld1.64		{d8}, [r0]
	moveq		pc, lr
	
LABx_5x5:
	lsl			r12, r12, #3
	vmov.i64	q15, #0
	vmov.s32	d31[0], r12
	vsub.u64	d31, d30, d31
	vshl.u64	d0, d0, d31
	vshl.u64	d2, d2, d31
	vshl.u64	d4, d4, d31
	vshl.u64	d6, d6, d31
	vshl.u64	d8, d8, d31
	M_LRtoPC	

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

_getSrc_4x5:		@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
@	pld			[r0]
	ands		r12, r0, #0x03
	bic			r0, r0, #0x03
	vld1.64		{d0}, [r0], r2
	vld1.64		{d1}, [r0], r2
	vld1.64		{d2}, [r0], r2
	vld1.64		{d3}, [r0], r2
	vld1.64		{d4}, [r0]
	moveq		pc, lr
	
LABx_4x5:
	lsl			r12, r12, #3
	vmov.i64	q15, #0
	vmov.s32	d31[0], r12
	vsub.u64	d31, d30, d31
	vshl.u64	d0, d0, d31
	vshl.u64	d1, d1, d31
	vshl.u64	d2, d2, d31
	vshl.u64	d3, d3, d31
	vshl.u64	d4, d4, d31
	M_LRtoPC	

	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

AddRow4_Chroma:	@PROC
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
    mov			r12, r1
	vld1.32		{d8[0] }, [r12], r3
	vld1.32		{d9[0] }, [r12], r3
	vld1.32		{d10[0]}, [r12], r3
	vld1.32		{d11[0]}, [r12]
	
	vrhadd.u8	d0, d0, d8
	vrhadd.u8	d1, d1, d9
	vrhadd.u8	d2, d2, d10
	vrhadd.u8	d3, d3, d11	
	
	M_StoreData4x4	r1, r3
    
    mov		pc, lr	@return

	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H00V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H00V00_CtxA8):	@PROC
	M_pushLR
	
@	pld		[r0]
	ands	r12, r0, #0x03
	beq		LAB0_H00V00
	
LABx_H00V00:
	bic			r0, r0, #0x03
	lsl			r12, r12, #3
	vld1.64		{d0}, [r0], r2
	vld1.64		{d1}, [r0], r2
	vld1.64		{d2}, [r0], r2
	vld1.64		{d3}, [r0]
	vmov.i64	q15, #0
	vmov.s32	d31[0], r12
	vsub.u64	d31, d30, d31
	vshl.u64	d0, d0, d31
	vshl.u64	d1, d1, d31
	vshl.u64	d2, d2, d31
	vshl.u64	d3, d3, d31
	M_StoreData4x4	r1, r3
	M_popLR	
	
LAB0_H00V00:
	vld1.32		{d0[0]}, [r0], r2
	vld1.32		{d1[0]}, [r0], r2
	vld1.32		{d2[0]}, [r0], r2
	vld1.32		{d3[0]}, [r0]
	M_StoreData4x4	r1, r3
	M_popLR	
	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V00_CtxA8):	@PROC
	M_pushLR
	
@	pld		[r0]
	ands	r12, r0, #0x03
	beq		LAB0_H00V00_Add
	
LABx_H00V00_Add:
	bic			r0, r0, #0x03
	lsl			r12, r12, #3
	vld1.64		{d0}, [r0], r2
	vld1.64		{d1}, [r0], r2
	vld1.64		{d2}, [r0], r2
	vld1.64		{d3}, [r0]
	vmov.i64	q15, #0
	vmov.s32  d31[0], r12
	vsub.u64	d31, d30, d31
	vshl.u64	d0, d0, d31
	vshl.u64	d1, d1, d31
	vshl.u64	d2, d2, d31
	vshl.u64	d3, d3, d31
	bl			AddRow4_Chroma
	M_popLR	
	
LAB0_H00V00_Add:
	vld1.32		{d0[0]}, [r0], r2
	vld1.32		{d1[0]}, [r0], r2
	vld1.32		{d2[0]}, [r0], r2
	vld1.32		{d3[0]}, [r0]
	bl			AddRow4_Chroma
	M_popLR	

	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H01V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1) 
@	h0 = (3*p00 + p01 + 1 ) >> 2 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H01V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vmov.i8			d31, #1
	vshr.u64		d1, d0, #8
	vmov.i8			d30, #3	
	vaddl.u8		q8, d1, d31
	vshr.u64		d3, d2, #8
	vmlal.u8		q8, d0, d30	
	vshr.u64		d5, d4, #8
	vaddl.u8		q9, d3, d31
	vshr.u64		d7, d6, #8
	vmlal.u8		q9, d2, d30
	vaddl.u8		q10, d5, d31
	vmlal.u8		q10, d4, d30
	vqshrun.s16		d0, q8, #2	
	vaddl.u8		q11, d7, d31
	vmlal.u8		q11, d6, d30
	vqshrun.s16		d1, q9, #2	
	vst1.32			d0[0], [r1], r3
	vst1.32			d1[0], [r1], r3
	vqshrun.s16		d2, q10, #2	
	vqshrun.s16		d3, q11, #2	
	vst1.32			d2[0], [r1], r3
	vst1.32			d3[0], [r1]
	
	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vmov.i8			d31, #1
	vshr.u64		d1, d0, #8
	vmov.i8			d30, #3	
	vaddl.u8		q8, d1, d31
	vshr.u64		d3, d2, #8
	vmlal.u8		q8, d0, d30	
	vshr.u64		d5, d4, #8
	vaddl.u8		q9, d3, d31
	vshr.u64		d7, d6, #8
	vmlal.u8		q9, d2, d30
	vaddl.u8		q10, d5, d31
	vmlal.u8		q10, d4, d30
	vqshrun.s16		d0, q8, #2	
	vaddl.u8		q11, d7, d31
	vmlal.u8		q11, d6, d30
	vqshrun.s16		d1, q9, #2	
	vqshrun.s16		d2, q10, #2	
	vqshrun.s16		d3, q11, #2	
	
	bl				AddRow4_Chroma
	
	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H02V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1) 
@	h0 = (p00 + p01 + 1 ) >> 1 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H02V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vshr.u64		d1 , d0, #8
	vshr.u64		d3 , d2, #8
	vaddl.u8		q8 , d0, d1
	vaddl.u8		q9 , d2, d3
	vqrshrun.s16	d0, q8 , #1	
	vqrshrun.s16	d1, q9 , #1	
	vshr.u64		d5 , d4, #8
	vshr.u64		d7 , d6, #8
	vaddl.u8		q10, d4, d5
	vaddl.u8		q11, d6, d7
	vqrshrun.s16	d2, q10, #1	
	vqrshrun.s16	d3, q11, #1	

	@Store d0[0]~d3[0] to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vshr.u64		d1 , d0, #8
	vshr.u64		d3 , d2, #8
	vaddl.u8		q8 , d0, d1
	vaddl.u8		q9 , d2, d3
	vqrshrun.s16	d0, q8 , #1	
	vqrshrun.s16	d1, q9 , #1	
	vshr.u64		d5 , d4, #8
	vshr.u64		d7 , d6, #8
	vaddl.u8		q10, d4, d5
	vaddl.u8		q11, d6, d7
	vqrshrun.s16	d2, q10, #1	
	vqrshrun.s16	d3, q11, #1	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H03V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3) 
@	h0 = (p00 + 3*p01 + 1 ) >> 2 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H03V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vmov.i8			d31, #1
	vmov.i8			d30, #3	
	vshr.u64		d1 , d0, #8
	vaddl.u8		q8 , d0, d31
	vmlal.u8		q8 , d1, d30	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d2, d31
	vmlal.u8		q9 , d3, d30	
	vshr.u64		d5 , d4, #8
	vaddl.u8		q10, d4, d31
	vmlal.u8		q10, d5, d30	
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d0, q8 , #2	
	vaddl.u8		q11, d6, d31
	vqshrun.s16		d1, q9 , #2	
	vmlal.u8		q11, d7, d30	
	vqshrun.s16		d2, q10, #2	
	vst1.32			d0[0], [r1], r3
	vst1.32			d1[0], [r1], r3
	vqshrun.s16		d3, q11, #2	
	vst1.32			d2[0], [r1], r3
	vst1.32			d3[0], [r1]

	M_popLR	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H03V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V00_CtxA8):	@PROC
	M_pushLR

	@Load 5x4 src data to d0,d2,d4,d6
	bl				_getSrc_5x4
	
	vmov.i8			d31, #1
	vmov.i8			d30, #3	
	vshr.u64		d1 , d0, #8
	vaddl.u8		q8 , d0, d31
	vmlal.u8		q8 , d1, d30	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d2, d31
	vmlal.u8		q9 , d3, d30	
	vshr.u64		d5 , d4, #8
	vaddl.u8		q10, d4, d31
	vmlal.u8		q10, d5, d30	
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d0, q8 , #2	
	vaddl.u8		q11, d6, d31
	vqshrun.s16		d1, q9 , #2	
	vmlal.u8		q11, d7, d30	
	vqshrun.s16		d2, q10, #2	
	vqshrun.s16		d3, q11, #2	
	
	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (3,1)
@	v0 = (3*p00 + p10 + 2 ) >> 2 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H00V01_CtxA8):	@PROC
	M_pushLR

	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vmov.i8			d31, #2
	vmov.i8			d30, #3
	vaddl.u8		q5, d1, d31
	vmlal.u8		q5, d0, d30
	vaddl.u8		q6, d2, d31
	vmlal.u8		q6, d1, d30
	vaddl.u8		q7, d3, d31
	vmlal.u8		q7, d2, d30
	vaddl.u8		q8, d4, d31
	vqshrun.s16		d0, q5, #2	
	vmlal.u8		q8, d3, d30
	vqshrun.s16		d1, q6, #2	
	vqshrun.s16		d2, q7, #2	
	vst1.32			d0[0], [r1], r3
	vst1.32			d1[0], [r1], r3
	vst1.32			d2[0], [r1], r3
	vqshrun.s16		d3, q8, #2	
	vst1.32			d3[0], [r1]

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V01_CtxA8):	@PROC
	M_pushLR

	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vmov.i8			d31, #2
	vmov.i8			d30, #3
	vaddl.u8		q5, d1, d31
	vmlal.u8		q5, d0, d30
	vaddl.u8		q6, d2, d31
	vmlal.u8		q6, d1, d30
	vaddl.u8		q7, d3, d31
	vmlal.u8		q7, d2, d30
	vaddl.u8		q8, d4, d31
	vqshrun.s16		d0, q5, #2	
	vmlal.u8		q8, d3, d30
	vqshrun.s16		d1, q6, #2	
	vqshrun.s16		d2, q7, #2	
	vqshrun.s16		d3, q8, #2	
	
	bl				AddRow4_Chroma
	
	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H01V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (3,1)
@	h0 = (3*p00 + p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (9*p00 + 3*(p01 + p10) + p11 + 7) >> 4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H01V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d3, d31	@p11 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d5, d31	
	vmlal.u8		q9 , d0, d30	@+ 9*p00
	vaddl.u8		q10, d1, d2
	vaddl.u8		q13, d3, d4
	vmlal.u8		q12, d2, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p01 + p10)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d7, d31	@p11 + 7
	vaddl.u8		q12, d9, d31	
	vmlal.u8		q9 , d4, d30	@+ 9*p00
	vaddl.u8		q10, d5, d6
	vaddl.u8		q13, d7, d8
	vmlal.u8		q12, d6, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p01 + p10)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d3, d31	@p11 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d5, d31	
	vmlal.u8		q9 , d0, d30	@+ 9*p00
	vaddl.u8		q10, d1, d2
	vaddl.u8		q13, d3, d4
	vmlal.u8		q12, d2, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p01 + p10)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d7, d31	@p11 + 7
	vaddl.u8		q12, d9, d31	
	vmlal.u8		q9 , d4, d30	@+ 9*p00
	vaddl.u8		q10, d5, d6
	vaddl.u8		q13, d7, d8
	vmlal.u8		q12, d6, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p01 + p10)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H02V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 7 ) >> 4   OR  v0 = (3*(p00 + p01) + p10 + p11 + 4) >> 3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H02V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
	
	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q10, d0, d1		@p00 + p01
	vaddl.u8		q9 , d2, d3		@p10 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p00 + p01)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q12, d2, d3
	vaddl.u8		q11, d4, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q12, d4, d5
	vaddl.u8		q10, d6, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q13, d6 , d7
	vaddl.u8		q12, d8 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	
	
	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
	
	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q10, d0, d1		@p00 + p01
	vaddl.u8		q9 , d2, d3		@p10 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p00 + p01)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q12, d2, d3
	vaddl.u8		q11, d4, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q12, d4, d5
	vaddl.u8		q10, d6, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q13, d6 , d7
	vaddl.u8		q12, d8 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	
	
	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H03V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (3,1)
@	h0 = (p00 + 3*p01)@	v0 = (3*h0 + h1 + 7 ) >> 4   OR  v0 = (3*(p00 + p11) + 9*p01 + p10 + 7) >> 4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													
VOASMFUNCNAME(Interpolate4_Chroma_H03V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d2, d31	@p10 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d4, d31	
	vmlal.u8		q9 , d1, d30	@+ 9*p01
	vaddl.u8		q10, d0, d3
	vaddl.u8		q13, d2, d5
	vmlal.u8		q12, d3, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d6, d31	@p10 + 7
	vaddl.u8		q12, d8, d31	
	vmlal.u8		q9 , d5, d30	@+ 9*p01
	vaddl.u8		q10, d4, d7
	vaddl.u8		q13, d6, d9
	vmlal.u8		q12, d7, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4
	
	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H03V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													
VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V01_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d2, d31	@p10 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d4, d31	
	vmlal.u8		q9 , d1, d30	@+ 9*p01
	vaddl.u8		q10, d0, d3
	vaddl.u8		q13, d2, d5
	vmlal.u8		q12, d3, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d6, d31	@p10 + 7
	vaddl.u8		q12, d8, d31	
	vmlal.u8		q9 , d5, d30	@+ 9*p01
	vaddl.u8		q10, d4, d7
	vaddl.u8		q13, d6, d9
	vmlal.u8		q12, d7, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4
	
	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,1) 
@	v0 = (p00 + p10 ) >> 1 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H00V02_CtxA8):	@PROC
	M_pushLR
	
	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vaddl.u8		q5 , d0, d1
	vaddl.u8		q6 , d1, d2
	vqshrun.s16		d0, q5 , #1	
	vaddl.u8		q7 , d2, d3
	vqshrun.s16		d1, q6 , #1	
	vaddl.u8		q8 , d3, d4
	vqshrun.s16		d2, q7 , #1	
	vqshrun.s16		d3, q8 , #1	

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V02_CtxA8):	@PROC
	M_pushLR
	
	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vaddl.u8		q5 , d0, d1
	vaddl.u8		q6 , d1, d2
	vqshrun.s16		d0, q5 , #1	
	vaddl.u8		q7 , d2, d3
	vqshrun.s16		d1, q6 , #1	
	vaddl.u8		q8 , d3, d4
	vqshrun.s16		d2, q7 , #1	
	vqshrun.s16		d3, q8 , #1	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP

	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H01V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,1)
@	h0 = (3*p00 + p01)@	v0 = (h0 + h1 + 7 ) >> 4   OR  v0 = (3*(p00 + p10) + p01 + p11 + 4) >> 3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_Chroma_H01V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q10, d0, d2		@p00 + p10
	vaddl.u8		q9 , d1, d3		@p01 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p00 + p10)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q12, d2, d4
	vaddl.u8		q11, d3, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q12, d4, d6
	vaddl.u8		q10, d5, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q13, d6 , d8
	vaddl.u8		q12, d7 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q10, d0, d2		@p00 + p10
	vaddl.u8		q9 , d1, d3		@p01 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p00 + p10)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q12, d2, d4
	vaddl.u8		q11, d3, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q12, d4, d6
	vaddl.u8		q10, d5, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q13, d6 , d8
	vaddl.u8		q12, d7 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H02V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1)
@	v filter (1,1)
@	h0 = (p00 + p01)@	v0 = (h0 + h1 + 1 ) >> 2   OR  v0 = ( (p00 + p01) + (p10 + p11) + 1) >> 2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_Chroma_H02V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
	
	vshr.u64		d1 , d0, #8
	vmov.i16		q15, #1	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d1
	vshr.u64		d5 , d4, #8
	vaddl.u8		q10, d2, d3
	vshr.u64		d7 , d6, #8
	vaddl.u8		q11, d4, d5
	vshr.u64		d9 , d8, #8
	vadd.u16		q9 , q9, q10
	vaddl.u8		q12, d6, d7
	vaddl.u8		q13, d8, d9
	vadd.u16		q10, q10, q11
	vadd.u16		q9 , q9 , q15
	vadd.u16		q10, q10, q15
	vqshrun.s16		d0 , q9 , #2	
	vadd.u16		q11, q11, q12
	vqshrun.s16		d1 , q10, #2	
	vadd.u16		q11, q11, q15
	vadd.u16		q12, q12, q13
	vqshrun.s16		d2 , q11, #2	
	vadd.u16		q12, q12, q15
	vqshrun.s16		d3 , q12, #2	

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
	
	vshr.u64		d1 , d0, #8
	vmov.i16		q15, #1	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d1
	vshr.u64		d5 , d4, #8
	vaddl.u8		q10, d2, d3
	vshr.u64		d7 , d6, #8
	vaddl.u8		q11, d4, d5
	vshr.u64		d9 , d8, #8
	vadd.u16		q9 , q9, q10
	vaddl.u8		q12, d6, d7
	vaddl.u8		q13, d8, d9
	vadd.u16		q10, q10, q11
	vadd.u16		q9 , q9 , q15
	vadd.u16		q10, q10, q15
	vqshrun.s16		d0 , q9 , #2	
	vadd.u16		q11, q11, q12
	vqshrun.s16		d1 , q10, #2	
	vadd.u16		q11, q11, q15
	vadd.u16		q12, q12, q13
	vqshrun.s16		d2 , q11, #2	
	vadd.u16		q12, q12, q15
	vqshrun.s16		d3 , q12, #2	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H03V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (1,1)
@	h0 = (p00 + 3*p01)@	v0 = (h0 + h1 + 1 ) >> 2   OR  v0 = ( (p00 + p10) + 3*(p01 + p11) + 4) >> 3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_Chroma_H03V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5

	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d2		@p00 + p10
	vaddl.u8		q10, d1, d3		@p01 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p01 + p11)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q11, d2, d4
	vaddl.u8		q12, d3, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q10, d4, d6
	vaddl.u8		q12, d5, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q12, d6 , d8
	vaddl.u8		q13, d7 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H03V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,3)
@	v filter (1,1)
@	h0 = (p00 + 3*p01)@	v0 = (h0 + h1 + 1 ) >> 2   OR  v0 = ( (p00 + p10) + 3*(p01 + p11) + 4) >> 3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													  
VOASMFUNCNAME(Interpolate4_ChromaAdd_H03V02_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5

	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d2		@p00 + p10
	vaddl.u8		q10, d1, d3		@p01 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p01 + p11)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q11, d2, d4
	vaddl.u8		q12, d3, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q10, d4, d6
	vaddl.u8		q12, d5, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q12, d6 , d8
	vaddl.u8		q13, d7 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	v filter (1,3) 
@	v0 = (p00 + 3*p10 + 2 ) >> 2 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_Chroma_H00V03_CtxA8):	@PROC
	M_pushLR
	
	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vmov.i8			d31, #2
	vmov.i8			d30, #3	
	vaddl.u8		q5, d0, d31
	vmlal.u8		q5, d1, d30
	vaddl.u8		q6, d1, d31
	vmlal.u8		q6, d2, d30
	vaddl.u8		q7, d2, d31
	vmlal.u8		q7, d3, d30
	vaddl.u8		q8, d3, d31
	vqshrun.s16		d0, q5, #2	
	vmlal.u8		q8, d4, d30
	vqshrun.s16		d1, q6, #2	
	vqshrun.s16		d2, q7, #2	
	vst1.32			d0[0], [r1], r3
	vst1.32			d1[0], [r1], r3
	vst1.32			d2[0], [r1], r3
	vqshrun.s16		d3, q8, #2	
	vst1.32			d3[0], [r1]

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
VOASMFUNCNAME(Interpolate4_ChromaAdd_H00V03_CtxA8):	@PROC
	M_pushLR
	
	@Load 4x5 src data to d0-d4
	bl				_getSrc_4x5
	
	vmov.i8			d31, #2
	vmov.i8			d30, #3	
	vaddl.u8		q5, d0, d31
	vmlal.u8		q5, d1, d30
	vaddl.u8		q6, d1, d31
	vmlal.u8		q6, d2, d30
	vaddl.u8		q7, d2, d31
	vmlal.u8		q7, d3, d30
	vaddl.u8		q8, d3, d31
	vqshrun.s16		d0, q5, #2	
	vmlal.u8		q8, d4, d30
	vqshrun.s16		d1, q6, #2	
	vqshrun.s16		d2, q7, #2	
	vqshrun.s16		d3, q8, #2	
	
	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H01V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (3,1)
@	v filter (1,3)
@	h0 = (3*p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( 3*(p00 + p11) + 9*p10 + p01 + 7) >> 4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													    
VOASMFUNCNAME(Interpolate4_Chroma_H01V03_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d1, d31	@p01 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d3, d31	
	vmlal.u8		q9 , d2, d30	@+ 9*p10
	vaddl.u8		q10, d0, d3
	vaddl.u8		q13, d2, d5
	vmlal.u8		q12, d4, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d5, d31	@p01 + 7
	vaddl.u8		q12, d7, d31	
	vmlal.u8		q9 , d6, d30	@+ 9*p10
	vaddl.u8		q10, d4, d7
	vaddl.u8		q13, d6, d9
	vmlal.u8		q12, d8, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H01V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													    
VOASMFUNCNAME(Interpolate4_ChromaAdd_H01V03_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5
		
	@Row 0,1
	vshr.u64		d3 , d2, #8
	vmov.i8			d31, #7
	vshr.u64		d1 , d0, #8
	vaddl.u8		q9 , d1, d31	@p01 + 7
	vshr.u64		d5 , d4, #8
	vmov.i8			d30, #9
	vaddl.u8		q12, d3, d31	
	vmlal.u8		q9 , d2, d30	@+ 9*p10
	vaddl.u8		q10, d0, d3
	vaddl.u8		q13, d2, d5
	vmlal.u8		q12, d4, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d0 , q9 , #4	@>> 4
	vshr.u64		d7 , d6, #8
	vqshrun.s16		d1 , q12, #4

	@Row 2,3
	vshr.u64		d9 , d8, #8
	vaddl.u8		q9 , d5, d31	@p01 + 7
	vaddl.u8		q12, d7, d31	
	vmlal.u8		q9 , d6, d30	@+ 9*p10
	vaddl.u8		q10, d4, d7
	vaddl.u8		q13, d6, d9
	vmlal.u8		q12, d8, d30	
	vshl.u16		q11, q10, #1
	vshl.u16		q14, q13, #1
	vadd.u16		q10, q10, q11	
	vadd.u16		q13, q13, q14	
	vadd.u16		q9 , q9 , q10	@+ 3*(p00 + p11)
	vadd.u16		q12, q12, q13	
	vqshrun.s16		d2 , q9 , #4	@>> 4
	vqshrun.s16		d3 , q12, #4

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4_H02V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@
@	h filter (1,1)
@	v filter (1,3)
@	h0 = (p00 + p01)@	v0 = (h0 + 3*h1 + 1 ) >> 2   OR  v0 = ( (p00 + p01) + 3*(p10 + p11) + 4) >> 3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													    
VOASMFUNCNAME(Interpolate4_Chroma_H02V03_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5

	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d1		@p00 + p01
	vaddl.u8		q10, d2, d3		@p10 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p10 + p11)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q11, d2, d3
	vaddl.u8		q12, d4, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q10, d4, d5
	vaddl.u8		q12, d6, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q12, d6 , d7
	vaddl.u8		q13, d8 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	@Store d0~d3 to dst buffer
	M_StoreData4x4	r1, r3

	M_popLR	
	@ENDP
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	void RV_FASTCALL C_MCCopyChroma4Add_H02V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
													    
VOASMFUNCNAME(Interpolate4_ChromaAdd_H02V03_CtxA8):	@PROC
	M_pushLR

	@Load 5x5 src data to d0 d2 d4 d6 d8
	bl				_getSrc_5x5

	@Row 0,1,2,3
	vshr.u64		d1 , d0, #8
	vmov.i16		q14, #3	
	vshr.u64		d3 , d2, #8
	vaddl.u8		q9 , d0, d1		@p00 + p01
	vaddl.u8		q10, d2, d3		@p10 + p11
	vmov.i16		q15, #4	
	vmla.u16		q9 , q10, q14	@3*(p10 + p11)
	vshr.u64		d5 , d4, #8
	vaddl.u8		q11, d2, d3
	vaddl.u8		q12, d4, d5		
	vshr.u64		d7 , d6, #8
	vmla.u16		q11, q12, q14	
	vaddl.u8		q10, d4, d5
	vaddl.u8		q12, d6, d7		
	vshr.u64		d9 , d8, #8
	vmla.u16		q10, q12, q14	
	vaddl.u8		q12, d6 , d7
	vaddl.u8		q13, d8 , d9		
	vmla.u16		q12, q13, q14	
	vadd.u16		q9 , q9, q15	@+ 4
	vadd.u16		q11, q11, q15	
	vqshrun.s16		d0 , q9 , #3	@>> 3
	vqshrun.s16		d1 , q11, #3	
	vadd.u16		q10, q10, q15	
	vadd.u16		q12, q12, q15	
	vqshrun.s16		d2 , q10, #3	
	vqshrun.s16		d3 , q12, #3	

	bl				AddRow4_Chroma

	M_popLR	
	@ENDP
	

	@END