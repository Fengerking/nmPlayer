%include "voH264Util.asm"

SECTION .text

INIT_MMX mmxext
; void pixels(uint8_t *block, const uint8_t *pixels, ptrdiff_t line_size, int h)
%macro PIXELS48 2
%if %2 == 4
%define OP movh
%else
%define OP mova
%endif
cglobal %1_pixels%2, 4,5
    movsxdifnidn r2, r2d
    lea          r4, [r2*3]
.loop:
    OP           m0, [r1]
    OP           m1, [r1+r2]
    OP           m2, [r1+r2*2]
    OP           m3, [r1+r4]
    lea          r1, [r1+r2*4]
%ifidn %1, avg
    pavgb        m0, [r0]
    pavgb        m1, [r0+r2]
    pavgb        m2, [r0+r2*2]
    pavgb        m3, [r0+r4]
%endif
    OP         [r0], m0
    OP      [r0+r2], m1
    OP    [r0+r2*2], m2
    OP      [r0+r4], m3
    sub         r3d, 4
    lea          r0, [r0+r2*4]
    jne       .loop
    RET
%endmacro

PIXELS48 put, 4
PIXELS48 avg, 4
PIXELS48 put, 8
PIXELS48 avg, 8


INIT_XMM sse2
; void put_pixels16_sse2(uint8_t *block, const uint8_t *pixels, ptrdiff_t line_size, int h)
cglobal put_pixels16, 4,5,4
    lea          r4, [r2*3]
.loop:
    movu         m0, [r1]
    movu         m1, [r1+r2]
    movu         m2, [r1+r2*2]
    movu         m3, [r1+r4]
    lea          r1, [r1+r2*4]
    mova       [r0], m0
    mova    [r0+r2], m1
    mova  [r0+r2*2], m2
    mova    [r0+r4], m3
    sub         r3d, 4
    lea          r0, [r0+r2*4]
    jnz       .loop
    REP_RET

; void avg_pixels16_sse2(uint8_t *block, const uint8_t *pixels, ptrdiff_t line_size, int h)
cglobal avg_pixels16, 4,5,4
    lea          r4, [r2*3]
.loop:
    movu         m0, [r1]
    movu         m1, [r1+r2]
    movu         m2, [r1+r2*2]
    movu         m3, [r1+r4]
    lea          r1, [r1+r2*4]
    pavgb        m0, [r0]
    pavgb        m1, [r0+r2]
    pavgb        m2, [r0+r2*2]
    pavgb        m3, [r0+r4]
    mova       [r0], m0
    mova    [r0+r2], m1
    mova  [r0+r2*2], m2
    mova    [r0+r4], m3
    sub         r3d, 4
    lea          r0, [r0+r2*4]
    jnz       .loop
    REP_RET
    
    

;--------------------------- rewrite------------------------------

;void vo_put_pixels16_sse2(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
vo_global put_pixels16_sse2_rewrite
	push	ebp
	mov		ebp,	esp
	
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dest:		block
	mov		ecx,	[ebp + 12]		;src:		pixels
	mov		edx,	[ebp + 16]		;stride:	line_size
	mov		ebx,	[ebp + 20]		;height;	h
	
	lea		esi,	[2*edx]
	lea		edi,	[3*edx]
	
.loop:

	movdqu	xmm0,	[ecx]
	movdqu	xmm1,	[ecx + edx]
	movdqu	xmm2,	[ecx + esi]
	movdqu	xmm3,	[ecx + edi]
	
	movdqu	[eax],			xmm0
	movdqu	[eax + edx],	xmm1
	movdqu	[eax + esi],	xmm2
	movdqu	[eax + edi],	xmm3
	
	lea		eax,	[eax + edx*4]
	lea		ecx,	[ecx + edx*4]
	
	sub		ebx,	4
	jnz		.loop
	
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	
	leave
	ret
	

;void vo_put_pixels8_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
vo_global put_pixels8_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;block
	mov		ecx,	[ebp + 12]		;pixels
	mov		edx,	[ebp + 16]		;line_size
	mov		ebx,	[ebp + 20]		;h
	
	lea		esi,	[2*edx]
	lea		edi,	[3*edx]
	
.loop:
	movq	mm0,	[ecx]
	movq	mm1,	[ecx+edx]
	movq	mm2,	[ecx+esi]
	movq	mm3,	[ecx+edi]
	
	movq	[eax],		mm0
	movq	[eax+edx],	mm1
	movq	[eax+esi],	mm2
	movq	[eax+edi],	mm3
	
	lea		eax,	[eax + 4*edx]
	lea		ecx,	[ecx + 4*edx]
	
	sub		ebx,	4
	jnz		.loop
	
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
	
;void vo_put_pixels4_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
vo_global put_pixels4_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp+8]			;block
	mov		ecx,	[ebp+12]		;pixels
	mov		edx,	[ebp+16]		;line_size
	mov		ebx,	[ebp+20]		;h
	
	lea		esi,	[2*edx]
	lea		edi,	[3*edx]
	
.loop:
	movd	mm0,	[ecx]
	movd	mm1,	[ecx+edx]
	movd	mm2,	[ecx+esi]
	movd	mm3,	[ecx+edi]
	
	movd	[eax],		mm0
	movd	[eax+edx],	mm1
	movd	[eax+esi],	mm2
	movd	[eax+edi],	mm3
	
	lea		eax,	[eax + 4*edx]
	lea		ecx,	[ecx + 4*edx]
	
	sub		ebx,	4
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
;void vo_avg_pixels16_sse2(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
%ifdef	VO_NDK_X86
global vo_avg_pixels16_sse2_rewrite
vo_avg_pixels16_sse2_rewrite:
%else
global _vo_avg_pixels16_sse2_rewrite
_vo_avg_pixels16_sse2_rewrite:
%endif
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp+8]			;block
	mov		ecx,	[ebp+12]		;pixels
	mov		edx,	[ebp+16]		;line_size
	mov		ebx,	[ebp+20]		;h
	
	lea		esi,	[3*edx]
	
.loop:
	movdqu	xmm0,	[ecx]
	pavgb	xmm0,	[eax]
	movdqa	[eax],	xmm0
	
	movdqu	xmm0,	[ecx + edx]
	pavgb	xmm0,	[eax + edx]
	movdqa	[eax + edx],	xmm0
	
	movdqu	xmm0,	[ecx + 2*edx]
	pavgb	xmm0,	[eax + 2*edx]
	movdqa	[eax + 2*edx],	xmm0
	
	movdqu	xmm0,	[ecx + esi]
	pavgb	xmm0,	[eax + esi]
	movdqa	[eax + esi],	xmm0
	
	lea		eax,	[eax + 4*edx]
	lea		ecx,	[ecx + 4*edx]
	sub		ebx,	4
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
	
;void vo_avg_pixels8_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
%ifdef	VO_NDK_X86
global vo_avg_pixels8_mmxext_rewrite
vo_avg_pixels8_mmxext_rewrite:
%else
global _vo_avg_pixels8_mmxext_rewrite
_vo_avg_pixels8_mmxext_rewrite:
%endif
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp+8]			;block
	mov		ecx,	[ebp+12]		;pixels
	mov		edx,	[ebp+16]		;line_size
	mov		ebx,	[ebp+20]		;h
	
	lea		esi,	[3*edx]
	
.loop:
	movq	mm0,	[ecx]
	pavgb	mm0,	[eax]
	movq	[eax],	mm0
	
	movq	mm0,	[ecx + edx]
	pavgb	mm0,	[eax + edx]
	movq	[eax + edx],	mm0
	
	movq	mm0,	[ecx + edx*2]
	pavgb	mm0,	[eax + edx*2]
	movq	[eax + edx*2], mm0
	
	movq	mm0,	[ecx + esi]
	pavgb	mm0,	[eax + esi]
	movq	[eax + esi],	mm0
	
	lea		eax,	[eax + 4*edx]
	lea		ecx,	[ecx + 4*edx]
	sub		ebx,	4
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
	
;void vo_avg_pixels4_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
%ifdef	VO_NDK_X86
global vo_avg_pixels4_mmxext_rewrite
vo_avg_pixels4_mmxext_rewrite:
%else
global _vo_avg_pixels4_mmxext_rewrite
_vo_avg_pixels4_mmxext_rewrite:
%endif
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp+8]			;block
	mov		ecx,	[ebp+12]		;pixels
	mov		edx,	[ebp+16]		;line_size
	mov		ebx,	[ebp+20]		;h
	
	lea		esi,	[3*edx]
	
.loop:
	movd	mm0,	[ecx]
	pavgb	mm0,	[eax]
	movd	[eax],	mm0
	
	movd	mm0,	[ecx + edx]
	pavgb	mm0,	[eax + edx]
	movd	[eax + edx],	mm0
	
	movd	mm0,	[ecx + edx*2]
	pavgb	mm0,	[eax + edx*2]
	movd	[eax + edx*2], mm0
	
	movd	mm0,	[ecx + esi]
	pavgb	mm0,	[eax + esi]
	movd	[eax + esi],	mm0
	
	lea		eax,	[eax + 4*edx]
	lea		ecx,	[ecx + 4*edx]
	sub		ebx,	4
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
	