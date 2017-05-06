	@AREA	|.text|, CODE, READONLY
	#include "voYYDef_CC.h"
	.text
	
	.align 2
	
	.globl	_Yuv2yuv_R90_inter_arm
	.globl	_Yuv2yuv_L90_inter_arm
	.globl	_Yuv2yuv_180_inter_arm	
	
_Yuv2yuv_R90_inter_arm: @PROC
@void Yuv2yuv_R90_inter_c (uint8_t * pInput, int nWidth, int nHeight, int nInStride, uint8_t * pOutput, int nOutStride)
@{
@	unsigned int * pSourBuf1, * pSourBuf2, * pSourBuf3, * pSourBuf4@
@	unsigned int nPix1, nPix2, nPix3, nPix4@
@	int i, j@
@	unsigned int * pDestStart	= (unsigned int * )(pOutput + nOutStride - 4)@
@	unsigned int * pDestOut	=  pDestStart@
@	for (i = 0@ i < nHeight@ i+=4)
@	{
@		pSourBuf1 = (unsigned int *)(pInput + i * nInStride)@
@		pSourBuf2 = (unsigned int *)(pInput + (i + 1) * nInStride)@
@		pSourBuf3 = (unsigned int *)(pInput + (i + 2) * nInStride)@
@		pSourBuf4 = (unsigned int *)(pInput + (i + 3) * nInStride)@
@		pDestOut = pDestStart - (i >> 2)@

@		for (j = 0@ j < nWidth@ j+=4)
@		{
@			nPix1 = *pSourBuf1++@
@			nPix2 = *pSourBuf2++@
@			nPix3 = *pSourBuf3++@
@			nPix4 = *pSourBuf4++@
@
@			*pDestOut = (nPix4 & 0XFF) + ((nPix3 & 0XFF) << 8) + ((nPix2 & 0XFF) << 16) + ((nPix1 & 0XFF) << 24)@
@			pDestOut += nOutStride / 4@
@
@			*pDestOut = ((nPix4 & 0XFF00) >> 8) + ((nPix3 & 0XFF00)) + ((nPix2 & 0XFF00) << 8) + ((nPix1 & 0XFF00) << 16)@
@			pDestOut += nOutStride / 4@
@
@			*pDestOut = ((nPix4 & 0XFF0000) >> 16) + ((nPix3 & 0XFF0000) >> 8) + ((nPix2 & 0XFF0000)) + ((nPix1 & 0XFF0000) << 8)@
@			pDestOut += nOutStride / 4@
@
@			*pDestOut = ((nPix4 & 0XFF000000) >> 24) + ((nPix3 & 0XFF000000) >> 16) + ((nPix2 & 0XFF000000) >> 8) + ((nPix1 & 0XFF000000))@
@			pDestOut += nOutStride / 4@
@		}
@	}
@}
	stmdb       sp!, {r4 - r11, lr}
	@ r0 = pInput, r1 = nWidth = nInStride, r2 = nHeight = nOutStride, 
	@ [sp, #36] = pOutput,  [sp, #40]
	ldr	r5, [sp, #36]
	mov	r3, r2
							@pDestStart	= (unsigned int * )(pOutput + nOutStride - 4)@	
	add	r5, r5, r2
	sub r5, r5, #4			
	
r90_big:

	mov	r6, r5				@pDestOut
	mov	r14, r0				@pSourBuf1	
	mov	r4, r1
	
r90_lit:	

	mov	r12, r14
	ldr	r7, [r14], #4		@r7 = nPix1
	ldr	r8, [r12, r1]!		@r8 = nPix2	
	ldr	r9, [r12, r1]!		@r9 = nPix3
	ldr	r10, [r12, r1]		@r10 = nPix4
	
@	*pDestOut = (nPix4 & 0XFF) + ((nPix3 & 0XFF) << 8) + ((nPix2 & 0XFF) << 16) + ((nPix1 & 0XFF) << 24)@							pDestOut += nOutStride / 4@
	and	r11, r9, #0xff
	and	r12, r10, #0xff
	add	r11, r12, r11, lsl #8
	and	r12, r8, #0xff
	add	r11, r11, r12, lsl #16
	and	r12, r7, #0xff	
	add	r11, r11, r12, lsl #24	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF00) >> 8) + ((nPix3 & 0XFF00)) + ((nPix2 & 0XFF00) << 8) + ((nPix1 & 0XFF00) << 16)@					pDestOut += nOutStride / 4@
	and	r11, r10, #0XFF00
	and	r12, r9, #0XFF00
	add	r11, r12, r11, lsr #8
	and	r12, r8, #0XFF00
	add	r11, r11, r12, lsl #8
	and	r12, r7, #0XFF00	
	add	r11, r11, r12, lsl #16	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF0000) >> 16) + ((nPix3 & 0XFF0000) >> 8) + ((nPix2 & 0XFF0000)) + ((nPix1 & 0XFF0000) << 8)@			pDestOut += nOutStride / 4@
	and	r12, r10, #0XFF0000
	and	r11, r8, #0XFF0000
	add	r11, r11, r12, lsr #16
	and	r12, r9, #0XFF0000
	add	r11, r11, r12, lsr #8
	and	r12, r7, #0XFF0000	
	add	r11, r11, r12, lsl #8	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF000000) >> 24) + ((nPix3 & 0XFF000000) >> 16) + ((nPix2 & 0XFF000000) >> 8) + ((nPix1 & 0XFF000000))@	pDestOut += nOutStride / 4@
	and	r12, r10, #0XFF000000
	and	r11, r7, #0XFF000000
	add	r11, r11, r12, lsr #24
	and	r12, r9, #0XFF000000
	add	r11, r11, r12, lsr #16
	and	r12, r8, #0XFF000000	
	add	r11, r11, r12, lsr #8	
	str r11, [r6], r2		
	
	subs	r4, r4, #4	
	bne	r90_lit
	
@		pSourBuf1 = (unsigned int *)(pInput + i * nInStride)@
@		pDestOut = pDestStart - (i >> 2)@	
	add	r0, r0, r1, lsl #2
	sub	r5, r5, #4
		
	subs	r3, r3, #4		
	bne	r90_big	
	
	ldmia       sp!, {r4 - r11, pc}
	
	@ENDP  @ |Yuv2yuv_R90_inter_arm|

_Yuv2yuv_L90_inter_arm: @PROC
	stmdb       sp!, {r4 - r11, lr}
	@ r0 = pInput, r1 = nWidth = nInStride, r2 = nHeight = nOutStride, 
	@ [sp, #36] = pOutput,  [sp, #40]
	ldr	r5, [sp, #36]
@							@pDestStart	= (unsigned int * )(pOutput + nOutStride - 4)@	
@	add	r5, r5, r2
@	sub r5, r5, #4		
							@unsigned int * pDestStart	= (unsigned int * )(pOutput + (nWidth - 1) * nOutStride)@
	mul	r6, r2, r1	
	mov	r3, r2
	add	r5, r5, r6
	sub	r5, r5, r2	
	rsb	r2, r2, #0							
	
l90_big:

	mov	r6, r5				@pDestOut
	mov	r14, r0				@pSourBuf1	
	mov	r4, r1
	
l90_lit:	

	mov	r12, r14
	ldr	r7, [r14], #4		@r7 = nPix1
	ldr	r8, [r12, r1]!		@r8 = nPix2	
	ldr	r9, [r12, r1]!		@r9 = nPix3
	ldr	r10, [r12, r1]		@r10 = nPix4
	
@	*pDestOut = (nPix4 & 0XFF) + ((nPix3 & 0XFF) << 8) + ((nPix2 & 0XFF) << 16) + ((nPix1 & 0XFF) << 24)@							pDestOut -= nOutStride / 4@
@	*pDestOut = (nPix1 & 0XFF) + ((nPix2 & 0XFF) << 8) + ((nPix3 & 0XFF) << 16) + ((nPix4 & 0XFF) << 24)@
	and	r11, r8, #0xff
	and	r12, r7, #0xff
	add	r11, r12, r11, lsl #8
	and	r12, r9, #0xff
	add	r11, r11, r12, lsl #16
	and	r12, r10, #0xff	
	add	r11, r11, r12, lsl #24	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF00) >> 8) + ((nPix3 & 0XFF00)) + ((nPix2 & 0XFF00) << 8) + ((nPix1 & 0XFF00) << 16)@					pDestOut -= nOutStride / 4@
@	*pDestOut = ((nPix1 & 0XFF00) >> 8) + ((nPix2 & 0XFF00)) + ((nPix3 & 0XFF00) << 8) + ((nPix4 & 0XFF00) << 16)@
	and	r11, r7, #0XFF00
	and	r12, r8, #0XFF00
	add	r11, r12, r11, lsr #8
	and	r12, r9, #0XFF00
	add	r11, r11, r12, lsl #8
	and	r12, r10, #0XFF00	
	add	r11, r11, r12, lsl #16	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF0000) >> 16) + ((nPix3 & 0XFF0000) >> 8) + ((nPix2 & 0XFF0000)) + ((nPix1 & 0XFF0000) << 8)@			pDestOut -= nOutStride / 4@
@	*pDestOut = ((nPix1 & 0XFF0000) >> 16) + ((nPix2 & 0XFF0000) >> 8) + ((nPix3 & 0XFF0000)) + ((nPix4 & 0XFF0000) << 8)@
	and	r12, r7, #0XFF0000
	and	r11, r9, #0XFF0000
	add	r11, r11, r12, lsr #16
	and	r12, r8, #0XFF0000
	add	r11, r11, r12, lsr #8
	and	r12, r10, #0XFF0000	
	add	r11, r11, r12, lsl #8	
	str r11, [r6], r2	
@	*pDestOut = ((nPix4 & 0XFF000000) >> 24) + ((nPix3 & 0XFF000000) >> 16) + ((nPix2 & 0XFF000000) >> 8) + ((nPix1 & 0XFF000000))@	pDestOut -= nOutStride / 4@
@	*pDestOut = ((nPix1 & 0XFF000000) >> 24) + ((nPix2 & 0XFF000000) >> 16) + ((nPix3 & 0XFF000000) >> 8) + ((nPix4 & 0XFF000000))@
	and	r12, r7, #0XFF000000
	and	r11, r10, #0XFF000000
	add	r11, r11, r12, lsr #24
	and	r12, r8, #0XFF000000
	add	r11, r11, r12, lsr #16
	and	r12, r9, #0XFF000000	
	add	r11, r11, r12, lsr #8	
	str r11, [r6], r2		
	
	subs	r4, r4, #4	
	bne	l90_lit
	
@		pSourBuf1 = (unsigned int *)(pInput + i * nInStride)@
@		pDestOut = pDestStart - (i >> 2)@	
	add	r0, r0, r1, lsl #2
	add	r5, r5, #4
		
	subs	r3, r3, #4	
	bne	l90_big	
	
	ldmia       sp!, {r4 - r11, pc}

	@ENDP  @ |Yuv2yuv_L90_inter_arm|

_Yuv2yuv_180_inter_arm: @PROC
@void Yuv2yuv_180_inter_c (uint8_t * pInput, int nWidth, int nHeight, int nInStride, uint8_t * pOutput, int nOutStride)
@{
@	int * pSourBuf = (int *)pInput@
@	int * pDestBuf = (int *)pOutput@
@
@	int nPix = 0@
@	int i, j@
@
@	for (i = 0@ i < nHeight@ i++)
@	{
@		pSourBuf = (int *)(pInput + i * nInStride + nWidth - 4)@
@		pDestBuf = (int *)(pOutput + (nHeight -i) * nOutStride)@
@
@		for (j = 0@ j < nWidth@ j+=4)
@		{
@			*pDestBuf++ = (nPix & 0XFF) << 24 + (nPix & 0XFF00) << 8 + (nPix & 0XFF0000) >> 8 + (nPix & 0XFF000000) >> 24@
@		}
@	}
@}
	stmdb       sp!, {r4 - r11, lr}
	@ r0 = pInput, r1 = nWidth = nInStride = nOutStride, r2 = nHeight, 
	@ [sp, #36] = pOutput,  [sp, #40]
	ldr	r5, [sp, #36]
	mov	r3, r2	
	add r0, r0, r1
	mul	r6, r2, r1
	sub r6, r6, r1      @huwei bug_fix	
	sub r0, r0, #4		@pInput + nWidth - 4	
	add r5, r5, r6		@pOutput + nHeight* nOutStride
	
_180_BIG:

	mov	r6, r5				@pDestBuf
	mov	r7, r0				@pSourBuf1	
	mov	r4, r1
_180_LIT:	

	ldr	r8, [r7], #-4		@r8 = nPix	
	ldr	r9, [r7], #-4		@r8 = nPix	
					
	and	r12, r8, #0xff00
	and	r14, r8, #0xff0000	
	mov	r12, r12, lsl #8
	add	r12, r12, r14, lsr #8
	add	r12, r12, r8, lsl #24
	add r12, r12, r8, lsr #24
	str r12, [r6], #4
	
	and	r12, r9, #0xff00
	and	r14, r9, #0xff0000	
	mov	r12, r12, lsl #8
	add	r12, r12, r14, lsr #8
	add	r12, r12, r9, lsl #24
	add r12, r12, r9, lsr #24
	str r12, [r6], #4		
		
	subs	r4, r4, #8	
	bne	_180_LIT
@		pSourBuf = (int *)(pInput + i * nInStride + nWidth - 4)@
	add r0, r0, r1	
@		pDestBuf = (int *)(pOutput + (nHeight -i) * nOutStride)@
	sub	r5, r5, r1
	subs	r3, r3, #1	
	bne	_180_BIG	
	
	ldmia       sp!, {r4 - r11, pc}
		@ENDP  @ |Yuv2yuv_180_inter_arm|
	