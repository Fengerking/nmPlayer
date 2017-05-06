%include "voH264Util.asm"

SECTION_RODATA

SECTION .text

vo_global Idct4x4Add_mmx
	push ebp
	mov  ebp, esp
	push ebx
	push esi
	
	mov  eax, [ebp + 8]
	mov  ecx, [ebp + 12]
	mov  edx, [ebp + 16]
	
	movq mm0, [eax]
	movq mm1, [eax + 8]
	movq mm4, [eax + 16]
	movq mm3, [eax + 24]
	
	movq mm2, mm0
	movq mm6, mm4
	
	PUNPCKLWD mm0,mm1
	PUNPCKLWD mm4,mm3
	PUNPCKHWD mm2,mm1
	PUNPCKHWD mm6,mm3
	
	movq mm1, mm0
	movq mm3, mm2
	
	PUNPCKLDQ mm0,mm4
	PUNPCKLDQ mm2,mm6
	PUNPCKHDQ mm1,mm4
	PUNPCKHDQ mm3,mm6
	
	;horizontal
	movq	mm4, mm0
	PADDW	mm0, mm2
	PSUBW  mm4, mm2
	
	movq   mm5, mm1
	PSRAW  mm1, 1
	PSUBW  mm1, mm3
	PSRAW  mm3, 1
	PADDW  mm5, mm3
	
	movq mm6, mm0
	movq mm2, mm4
	
	PADDW mm0, mm5
	PADDW mm4, mm1
	PSUBW mm2, mm1
	PSUBW mm6, mm5
	
	movq  mm1, mm0
	movq  mm3, mm2
	PUNPCKLWD mm0, mm4
	PUNPCKLWD mm2, mm6
	PUNPCKHWD mm1, mm4
	PUNPCKHWD mm3, mm6
	
	movq  mm4, mm0
	movq  mm5, mm1
	
	PUNPCKLDQ mm0, mm2
	PUNPCKLDQ mm1, mm3
	PUNPCKHDQ mm4, mm2
	PUNPCKHDQ mm5, mm3
	
	;mm0, mm4, mm1, mm5
	;vertical
	movq  mm2, mm0
	paddw mm0, mm1
	psubw mm2, mm1
	
	movq  mm6, mm4
	PSRAW mm4, 1
	PSUBW mm4, mm5
	
	PSRAW mm5, 1
	PADDW mm6, mm5
	movq  mm1 , mm2
	movq  mm3 , mm0
	paddw mm0 , mm6
	paddw mm1 , mm4
	psubw mm2 , mm4
	psubw mm3 , mm6
	
	mov			esi, 32
	movd		mm7 , esi
	pshufw		mm7 , mm7 , 0
	
	paddw      mm0, mm7
	paddw      mm1, mm7
	paddw      mm2, mm7
	paddw      mm3, mm7

	PSRAW		mm0, 6
	PSRAW		mm1, 6
	PSRAW		mm2, 6
	PSRAW		mm3, 6
	
	PXOR		mm7, mm7
	movd		mm4, [ecx]
	movd		mm5, [ecx + edx]
	
	PUNPCKLBW  mm4,mm7
	PUNPCKLBW  mm5,mm7
	
	paddw      mm0, mm4
	paddw      mm1, mm5
	
	mov			esi, ecx
	add			esi, edx
	add         esi, edx
	
	movq		mm4, [esi]
	add         esi, edx
	movq		mm5, [esi]
	
	PUNPCKLBW  mm4,mm7
	PUNPCKLBW  mm5,mm7
	
	paddw		mm2,mm4
	paddw		mm3,mm5
	
	packuswb	mm0,mm0
	packuswb	mm1,mm1
	packuswb	mm2,mm2
	packuswb	mm3,mm3

	
	movd		[ecx], mm0
	add			ecx,   edx
	movd		[ecx], mm1
	add			ecx,   edx
	movd		[ecx], mm2
	add			ecx,   edx
	movd		[ecx], mm3
	
	mov			esp, ebp
	sub			esp, 8
	pop			esi
	pop			ebx
	
	leave
	ret
	
vo_global Idct4x4DCAdd_mmx
	push ebp
	mov  ebp, esp
	push ebx
	
	mov  eax, [ebp + 8]
	mov  ecx, [ebp + 12]
	mov  edx, [ebp + 16]
	
	
	movd		mm4, eax
	punpcklwd	mm4, mm4
	punpcklwd	mm4, mm4
	
	pxor		mm5, mm5
	
	mov			ebx, ecx
	movq		mm0, [ebx]
	add			ebx, edx
	movq		mm1, [ebx]
	add			ebx, edx
	movq		mm2, [ebx]
	add			ebx, edx
	movq		mm3, [ebx]
	
	punpcklbw	mm0, mm5
	paddw		mm0, mm4
	packuswb	mm0, mm5
	
	punpcklbw	mm1, mm5
	paddw		mm1, mm4
	packuswb	mm1, mm5
	
	punpcklbw	mm2, mm5
	paddw		mm2, mm4
	packuswb	mm2, mm5
	
	punpcklbw	mm3, mm5
	paddw		mm3, mm4
	packuswb	mm3, mm5
	
	mov			ebx, ecx
	movd		[ebx], mm0
	add			ebx, edx
	movd		[ebx], mm1
	add			ebx, edx
	movd		[ebx], mm2
	add			ebx, edx
	movd		[ebx], mm3
	
	mov			esp, ebp
	sub			esp, 4
	pop			ebx
	
	leave
	ret
	
vo_global Idct8x8Add_sse2
	push ebp
	mov  ebp, esp
	push ebx
	push esi
	
	mov		eax, [ebp + 8]
	mov		ecx, [ebp + 12]
	mov		edx, [ebp + 16]
	mov		ebx, [ebp + 20]
	
	movdqa	xmm0, [eax]
	movdqa	xmm1, [eax + 16]
	movdqa	xmm2, [eax + 32]
	movdqa	xmm3, [eax + 48]
	
	movdqa	xmm6, xmm0
	movdqa  xmm7, xmm2
	
	punpcklwd	xmm0, xmm1
	punpcklwd	xmm2, xmm3
	punpckhwd	xmm6, xmm1
	punpckhwd	xmm7, xmm3
	
	movdqa		xmm1, xmm0
	movdqa		xmm3, xmm6
	
	punpckldq	xmm0, xmm2
	punpckldq	xmm6, xmm7
	punpckhdq	xmm1, xmm2
	punpckhdq	xmm3, xmm7
	
	movdqa		[ebx], xmm6
	movdqa		[ebx + 32], xmm3
	
	movdqa		xmm4, [eax + 64]
	movdqa		xmm5, [eax + 80]
	movdqa		xmm6, [eax + 96]
	movdqa		xmm7, [eax + 112]
	
	movdqa		xmm2, xmm4
	movdqa		xmm3, xmm6
	
	punpcklwd	xmm2, xmm5
	punpcklwd	xmm3, xmm7
	punpckhwd	xmm4, xmm5
	punpckhwd	xmm6, xmm7
	
	movdqa		xmm5, xmm2
	movdqa		xmm7, xmm4
	
	punpckldq	xmm5, xmm3
	punpckldq	xmm7, xmm6
	punpckhdq	xmm2, xmm3
	punpckhdq	xmm4, xmm6
	
	movdqa		xmm3, xmm0
	movdqa		xmm6, xmm1
	
	punpcklqdq	xmm3, xmm5
	punpcklqdq	xmm6, xmm2
	punpckhqdq	xmm0, xmm5
	punpckhqdq	xmm1, xmm2
	
	movdqa		[ebx + 16], xmm0
	movdqa		[ebx + 48], xmm1
	movdqa		xmm5,	[ebx]
	movdqa		xmm2,	[ebx + 32]
	
	movdqa		xmm0, xmm5
	movdqa		xmm1, xmm2
	
	punpcklqdq	xmm0, xmm7
	punpcklqdq	xmm1, xmm4
	punpckhqdq	xmm5, xmm7
	punpckhqdq	xmm2, xmm4
	
	movdqa		xmm4, xmm3
	movdqa		xmm7, xmm6
	
	paddsw		xmm3, xmm0
	psubsw		xmm4, xmm0
	psraw		xmm7, 1
	psubsw		xmm7, xmm1
	psraw		xmm1, 1
	paddsw		xmm6, xmm1
	
	movdqa		xmm0, xmm3
	movdqa		xmm1, xmm4
	
	paddsw		xmm0, xmm6
	paddsw		xmm1, xmm7
	psubsw		xmm3, xmm6
	psubsw		xmm4, xmm7
	
	movdqa		[ebx + 64], xmm0
	movdqa		[ebx + 80], xmm1
	movdqa		[ebx + 96], xmm4
	movdqa		[ebx + 112], xmm3
	
	movdqa		xmm6, [ebx + 16]
	movdqa		xmm7, [ebx + 48]
	
	movdqa		xmm0, xmm6
	movdqa		xmm1, xmm7
	movdqa		xmm3, xmm5
	movdqa		xmm4, xmm2
	
	psraw		xmm0, 1
	psraw		xmm1, 1
	psraw		xmm3, 1
	psraw		xmm4, 1
	
	paddsw		xmm0, xmm6
	paddsw		xmm1, xmm7
	paddsw		xmm3, xmm5
	paddsw		xmm4, xmm2
	
	paddsw		xmm0, xmm5
	psubsw		xmm1, xmm2
	psubsw		xmm3, xmm6
	paddsw		xmm4, xmm7
	
	paddsw		xmm7, xmm0
	psubsw		xmm6, xmm1
	paddsw		xmm2, xmm3
	psubsw		xmm5, xmm4
	
	movdqa		xmm0, xmm7
	movdqa		xmm1, xmm2
	
	psraw		xmm0, 2
	psraw		xmm1, 2
	paddsw		xmm0, xmm5
	paddsw		xmm1, xmm6
	
	psraw		xmm6, 2
	psraw		xmm5, 2
	psubsw     xmm6, xmm2
	psubsw		xmm7, xmm5
	
	movdqa		xmm2, [ebx + 64]
	movdqa		xmm3, [ebx + 80]
	
	movdqa		xmm4, xmm2
	movdqa		xmm5, xmm3
	
	paddsw		xmm2, xmm7
	psubsw		xmm4, xmm7
	paddsw		xmm3, xmm6
	psubsw		xmm5, xmm6
	
	movdqa		[ebx], xmm2
	movdqa		[ebx + 16], xmm3
	
	movdqa		xmm6, [ebx + 96]
	movdqa		xmm7, [ebx + 112]
	
	movdqa		xmm2, xmm6
	movdqa		xmm3, xmm7
	
	paddsw		xmm2, xmm1
	psubsw		xmm6, xmm1
	paddsw		xmm3, xmm0
	psubsw		xmm7, xmm0
	
	movdqa		xmm0, xmm7
	movdqa		xmm1, xmm5
	
	punpcklwd		xmm0, xmm6
	punpcklwd		xmm1, xmm4
	punpckhwd		xmm7, xmm6
	punpckhwd		xmm5, xmm4
	
	movdqa			xmm6, xmm0
	movdqa          xmm4, xmm7
	punpckldq		xmm6, xmm1
	punpckldq		xmm4, xmm5
	punpckhdq		xmm0, xmm1
	punpckhdq		xmm7, xmm5
	
	movdqa			[ebx + 32], xmm4
	movdqa			[ebx + 48], xmm7
	
	movdqa			xmm1, [ebx]
	movdqa			xmm5, [ebx + 16]
	movdqa			xmm4, xmm1
	movdqa			xmm7, xmm2
	
	punpcklwd		xmm1, xmm5
	punpcklwd		xmm2, xmm3
	punpckhwd		xmm4, xmm5
	punpckhwd		xmm7, xmm3

	movdqa			xmm5, xmm1
	movdqa			xmm3, xmm4
	
	punpckldq		xmm1, xmm2
	punpckldq		xmm4, xmm7
	punpckhdq		xmm5, xmm2
	punpckhdq		xmm3, xmm7
	
	movdqa			xmm2, xmm1
	movdqa			xmm7, xmm5
	
	punpcklqdq		xmm1,xmm6
	punpcklqdq		xmm5,xmm0
	punpckhqdq		xmm2,xmm6
	punpckhqdq		xmm7,xmm0
	
	movdqa			[ebx + 64], xmm2
	movdqa			[ebx + 80], xmm7
	movdqa			xmm6, [ebx + 32]
	movdqa			xmm0, [ebx + 48]
	movdqa			xmm2, xmm4
	movdqa			xmm7, xmm3
	
	punpcklqdq		xmm4, xmm6
	punpcklqdq		xmm3, xmm0
	punpckhqdq		xmm2, xmm6
	punpckhqdq		xmm7, xmm0
	
	movdqa			xmm6, xmm1
	movdqa			xmm0, xmm5
	
	paddsw			xmm1, xmm4
	psubsw			xmm6, xmm4
	psraw			xmm5, 1
	psubsw			xmm5, xmm3
	psraw			xmm3, 1
	paddsw			xmm0, xmm3
	
	movdqa			xmm4, xmm1
	movdqa			xmm3, xmm6
	
	paddsw			xmm4, xmm0
	paddsw			xmm3, xmm5
	psubsw			xmm1, xmm0
	psubsw			xmm6, xmm5
	
	movdqa			[ebx], xmm4
	movdqa			[ebx + 16], xmm3
	movdqa			[ebx + 32], xmm6
	movdqa			[ebx + 48], xmm1
	
	movdqa			xmm0, [ebx + 64]
	movdqa			xmm5, [ebx + 80]
	
	movdqa			xmm1, xmm0
	movdqa			xmm3, xmm5
	movdqa			xmm4, xmm2
	movdqa			xmm6, xmm7
	
	psraw			xmm1, 1
	psraw			xmm3, 1
	psraw			xmm4, 1
	psraw			xmm6, 1
	
	paddsw			xmm1, xmm0
	paddsw			xmm3, xmm5
	paddsw			xmm4, xmm2
	paddsw			xmm6, xmm7
	
	paddsw			xmm1, xmm2
	psubsw			xmm3, xmm7
	psubsw			xmm4, xmm0
	paddsw			xmm6, xmm5
	
	paddsw			xmm5, xmm1
	psubsw			xmm0, xmm3
	paddsw			xmm7, xmm4
	psubsw			xmm2, xmm6
	
	movdqa			xmm1, xmm5
	movdqa			xmm3, xmm7
	
	psraw			xmm1, 2
	psraw			xmm3, 2
	paddsw			xmm1, xmm2
	paddsw			xmm3, xmm0
	
	psraw			xmm2, 2
	psraw			xmm0, 2
	psubsw			xmm5, xmm2
	psubsw			xmm0, xmm7
	
	movdqa			xmm2, [ebx]
	movdqa			xmm4, [ebx + 16]
	movdqa			xmm6, xmm2
	movdqa			xmm7, xmm4
	
	paddsw			xmm2, xmm5
	psubsw			xmm6, xmm5
	paddsw			xmm4, xmm0
	psubsw			xmm7, xmm0
	
	mov				eax,  32
	movd			xmm5, eax
	pshuflw			xmm5, xmm5, 0
	punpcklqdq		xmm5, xmm5
	
	movq			xmm0, [ecx]
	paddsw			xmm2, xmm5
	psraw			xmm2, 6
	paddsw			xmm6, xmm5
	psraw			xmm6, 6
	paddsw			xmm4, xmm5
	psraw			xmm4, 6
	paddsw			xmm7, xmm5
	psraw			xmm7, 6
	
	PXOR			xmm5, xmm5
	punpcklbw		xmm0, xmm5
	paddsw			xmm2, xmm0
	packuswb		xmm2, xmm2
	movq			[ecx], xmm2
	
	movq			xmm0, [ecx + edx]
	punpcklbw		xmm0, xmm5
	paddsw			xmm4, xmm0
	packuswb		xmm4, xmm4
	movq			[ecx + edx], xmm4
	
	movdqa			xmm2, [ebx + 32]
	movdqa			xmm4, xmm2
	paddsw			xmm2, xmm3
	psubsw			xmm4, xmm3
	
	mov				esi, 32
	movd			xmm0, esi
	pshuflw			xmm0, xmm0, 0
	punpcklqdq      xmm0, xmm0
	
	paddsw			xmm2, xmm0
	psraw			xmm2, 6
	paddsw			xmm4, xmm0
	psraw			xmm4, 6
	movq			xmm3, [ecx + edx * 2]
	punpcklbw		xmm3, xmm5
	paddsw			xmm2, xmm3
	packuswb		xmm2, xmm2
	movq			[ecx + edx*2], xmm2
	
	mov				esi, ecx
	add				esi, edx
	add				esi, edx
	add				esi, edx
	
	movdqa			xmm3, [ebx + 48]
	movdqa			xmm2, xmm3
	paddsw			xmm2, xmm1
	psubsw			xmm3, xmm1
	paddsw			xmm2, xmm0
	psraw			xmm2, 6
	paddsw			xmm3, xmm0
	psraw			xmm3, 6
	movq			xmm1, [esi]
	punpcklbw		xmm1, xmm5
	paddsw			xmm2, xmm1
	packuswb		xmm2, xmm2
	movq			[esi], xmm2
	
	add				esi, edx
	movq			xmm1, [esi]
	punpcklbw		xmm1, xmm5
	paddsw			xmm3, xmm1
	packuswb		xmm3, xmm3
	movq			[esi], xmm3
	
	add				esi, edx
	movq			xmm1, [esi]
	punpcklbw		xmm1, xmm5
	paddsw			xmm4, xmm1
	packuswb		xmm4, xmm4
	movq			[esi], xmm4
	
	add				esi, edx
	movq			xmm1, [esi]
	punpcklbw		xmm1, xmm5
	paddsw			xmm7, xmm1
	packuswb		xmm7, xmm7
	movq			[esi], xmm7
	
	add				esi, edx
	movq			xmm1, [esi]
	punpcklbw		xmm1, xmm5
	paddsw			xmm6, xmm1
	packuswb		xmm6, xmm6
	movq			[esi], xmm6
	
	mov				esp, ebp
	sub				esp, 8
	pop				esi
	pop				ebx
	
	leave
	ret