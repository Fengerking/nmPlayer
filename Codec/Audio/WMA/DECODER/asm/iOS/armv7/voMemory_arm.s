@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
@	*																		*
@	************************************************************************/
@/***************************************************************
@
@	File:		voMemory.s
@
@	Contains:	memory function
@
@	Written by:	Witten Wen
@
@	Change History (most recent first):
@	2009-05-26		W.W			Create file
@
@****************************************************************/

                #include "../../../inc/audio/v10/include/voWMADecID.h"
		.include     "kxarm.h"
		.include		"wma_arm_version.h"
		.include		"wma_member_arm.inc"

		@AREA    |.text|, CODE, READONLY
		.text .align 4
		
		.if		WMA_OPT_VOMEMORY_ARM == 1
		
		.globl		_voWMAMemset
		.globl		_voWMAMemcpy	
		.globl		_MULT_HI_DWORDN_ARM
		
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for memset
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
pSrc					.req	r6
n						.req	r1
size					.req	r2

PreAddrRest				.req	r3

n_32bits1				.req	r4
n_32bits2				.req	r5
@******************************************************************
@	
@	void *voWMAMemset(void *pSrc, int n, size_t size)@
@
@******************************************************************

	@AREA    |.text|, CODE
_voWMAMemset:
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 	
@r0 = pSrc@
@r1 = n@
@r2 = size@
  	
  	stmfd	sp!, {r4-r6, lr}
  	
  	cmp		r0, #0
  	beq		EndMemset
  	cmp		size, #0
  	beq		EndMemset
  	
  	mov		pSrc, r0
  	and		n, n, #0xff 
  	ands	PreAddrRest, pSrc, #0x3 	
	beq		Align4
	rsb		r4, PreAddrRest, #4
	cmp		r4, size
	movhi	r4, size
	mov		PreAddrRest, r4
PreLoop:
	strb	n, [pSrc], #1  	 
	subs	PreAddrRest, PreAddrRest, #1
	bne		PreLoop
	subs	size, size, r4
	beq		EndMemset
	
Align4:
	movs	r3, size, LSR #3							@ size/8
	beq		PostSize
	orr		n_32bits1, n, n, LSL #8
	orr		n_32bits2, n_32bits1, n_32bits1, LSL #16
	mov		n_32bits1, n_32bits2
MainLoop:
	.if		ARMVERSION	>= 6
	strd	n_32bits1, [pSrc], #8
	.else
	str		n_32bits1, [pSrc], #4
	str		n_32bits2, [pSrc], #4
	.endif
	subs	r3, r3, #1
	bne		MainLoop
	
PostSize:
	ands		size, size, #7
	beq			EndMemset
PostLoop:	
	strb	n, [pSrc], #1
	subs	size, size, #1
	bne		PostLoop	
EndMemset:
	ldmfd	sp!, {r4-r6, pc}
	@ENTRY_END	voWMAMemset
	
	
@******************************************************************
@	
@	void *voWMAMemcpy(void *pDst, const void *pSrc, size_t size)@
@
@******************************************************************

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for voWMAMemcpy
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

pDst					.req		r5
pSrc_cpy					.req		r1
size_cpy					.req		r2

SrcAddrRest				.req		r3
DstAddrRest				.req		r4

DATA1					.req		r6
DATA2					.req		r7

	@AREA    |.text|, CODE
_voWMAMemcpy:
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
@r0 = pDst@
@r1 = pSrc@
@r2 = size@
	@add r1,r2,r2
	stmfd	sp!, {r4-r7, lr}
	
	cmp		size_cpy, #0
	beq		EndMemcpy
	
	mov		pDst, r0
	and		SrcAddrRest, pSrc_cpy, #0x3
	and		DstAddrRest, pDst, #0x3
	cmp		SrcAddrRest, DstAddrRest
	bne		CPYPostSize
	
	cmp		SrcAddrRest, #0
	beq		CPYAlign4
	rsb		r4, SrcAddrRest, #4	
	cmp		r4, size_cpy
	movhi	r4, size_cpy
	mov		SrcAddrRest, r4
CPYPreLoop:
	ldrb	DATA1, [pSrc_cpy], #1
	subs	SrcAddrRest, SrcAddrRest, #1
	strb	DATA1, [pDst], #1	
	bne		CPYPreLoop
	subs	size_cpy, size_cpy, r4
	beq		EndMemcpy
	
CPYAlign4:
	movs	r3, size_cpy, LSR #3							@size/8
	beq		CPYPostSize
CPYMainLoop:
	.if		ARMVERSION	>= 6
	ldrd	DATA1, [pSrc_cpy], #8							@armv6
	subs	r3, r3, #1
	strd	DATA1, [pDst], #8
	.else
	ldr		DATA1, [pSrc_cpy], #4							@armv4
	ldr		DATA2, [pSrc_cpy], #4
	subs	r3, r3, #1
	str		DATA1, [pDst], #4
	str		DATA2, [pDst], #4
	.endif	
	bne		CPYMainLoop
	
	ands		size_cpy, size_cpy, #7
	beq			EndMemcpy
CPYPostSize:
	ldrb	DATA1, [pSrc_cpy], #1
	subs	size_cpy, size_cpy, #1
	strb	DATA1, [pDst], #1	
	bne		CPYPostSize
	
EndMemcpy:
	ldmfd	sp!, {r4-r7, pc}
	@ENTRY_END	voWMAMemcpy

@================================================================================
@
@	I32	MULT_HI_DWORDN_ARM( I32 a, I32 b, I32 N);
@
@================================================================================

@	AREA    |.text|, CODE
_MULT_HI_DWORDN_ARM:
@********************************************************************************
@ r0 = a
@ r1 = b
@ r2 = N
	stmfd	sp!, {lr}	
	
	smull	lr, r3, r0, r1
	rsb		r12, r2, #32
	mov		lr, lr, LSR r12
	mov		r3, r3, LSL r2
	orr		r0, r3, lr
	ldmfd	sp!, {pc}
	
	@ENTRY_END	MULT_HI_DWORDN_ARM
	
	.endif		@WMA_OPT_VOMEMORY_ARM

			
			
			
