%include "voH264Util.asm"

SECTION .text

%macro op_avgh 3
    movh   %3, %2
    pavgb  %1, %3
    movh   %2, %1
%endmacro

%macro op_avg 2
    pavgb  %1, %2
    mova   %2, %1
%endmacro

%macro op_puth 2-3
    movh   %2, %1
%endmacro

%macro op_put 2
    mova   %2, %1
%endmacro

; void pixels4_l2_mmxext(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int src1Stride, int h)
%macro PIXELS4_L2 1
%define OP op_%1h
cglobal %1_pixels4_l2, 6,6
    movsxdifnidn r3, r3d
    movsxdifnidn r4, r4d
    test        r5d, 1
    je        .loop
    movd         m0, [r1]
    movd         m1, [r2]
    add          r1, r4
    add          r2, 4
    pavgb        m0, m1
    OP           m0, [r0], m3
    add          r0, r3
    dec         r5d
.loop:
    mova         m0, [r1]
    mova         m1, [r1+r4]
    lea          r1, [r1+2*r4]
    pavgb        m0, [r2]
    pavgb        m1, [r2+4]
    OP           m0, [r0], m3
    OP           m1, [r0+r3], m3
    lea          r0, [r0+2*r3]
    mova         m0, [r1]
    mova         m1, [r1+r4]
    lea          r1, [r1+2*r4]
    pavgb        m0, [r2+8]
    pavgb        m1, [r2+12]
    OP           m0, [r0], m3
    OP           m1, [r0+r3], m3
    lea          r0, [r0+2*r3]
    add          r2, 16
    sub         r5d, 4
    jne       .loop
    REP_RET
%endmacro

INIT_MMX mmxext
PIXELS4_L2 put
PIXELS4_L2 avg

; void pixels8_l2_mmxext(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int src1Stride, int h)
%macro PIXELS8_L2 1
%define OP op_%1
cglobal %1_pixels8_l2, 6,6
    movsxdifnidn r3, r3d
    movsxdifnidn r4, r4d
    test        r5d, 1
    je        .loop
    mova         m0, [r1]
    mova         m1, [r2]
    add          r1, r4
    add          r2, 8
    pavgb        m0, m1
    OP           m0, [r0]
    add          r0, r3
    dec         r5d
.loop:
    mova         m0, [r1]
    mova         m1, [r1+r4]
    lea          r1, [r1+2*r4]
    pavgb        m0, [r2]
    pavgb        m1, [r2+8]
    OP           m0, [r0]
    OP           m1, [r0+r3]
    lea          r0, [r0+2*r3]
    mova         m0, [r1]
    mova         m1, [r1+r4]
    lea          r1, [r1+2*r4]
    pavgb        m0, [r2+16]
    pavgb        m1, [r2+24]
    OP           m0, [r0]
    OP           m1, [r0+r3]
    lea          r0, [r0+2*r3]
    add          r2, 32
    sub         r5d, 4
    jne       .loop
    REP_RET
%endmacro

INIT_MMX mmxext
PIXELS8_L2 put
PIXELS8_L2 avg

; void pixels16_l2_mmxext(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int src1Stride, int h)
%macro PIXELS16_L2 1
%define OP op_%1
cglobal %1_pixels16_l2, 6,6
    movsxdifnidn r3, r3d
    movsxdifnidn r4, r4d
    test        r5d, 1
    je        .loop
    mova         m0, [r1]
    mova         m1, [r1+8]
    pavgb        m0, [r2]
    pavgb        m1, [r2+8]
    add          r1, r4
    add          r2, 16
    OP           m0, [r0]
    OP           m1, [r0+8]
    add          r0, r3
    dec         r5d
.loop:
    mova         m0, [r1]
    mova         m1, [r1+8]
    add          r1, r4
    pavgb        m0, [r2]
    pavgb        m1, [r2+8]
    OP           m0, [r0]
    OP           m1, [r0+8]
    add          r0, r3
    mova         m0, [r1]
    mova         m1, [r1+8]
    add          r1, r4
    pavgb        m0, [r2+16]
    pavgb        m1, [r2+24]
    OP           m0, [r0]
    OP           m1, [r0+8]
    add          r0, r3
    add          r2, 32
    sub         r5d, 2
    jne       .loop
    REP_RET
%endmacro

INIT_MMX mmxext
PIXELS16_L2 put
PIXELS16_L2 avg


;---------------------------------rewrite---------------------------------
;dst = round((src1 + src2) >> 1)
;block : width = 16 ; height = h
;void vo_put_pixels16_l2_mmxext_rewrite(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);
;void vo_avg_pixels16_l2_mmxext_rewrite(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);

%macro PIXELS16_L2_MMXEXT 1
vo_global %1_pixels16_l2_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;src1
	mov		edx,	[ebp + 16]		;src2
	mov		ebx,	[ebp + 20]		;dstStride
	mov		esi,	[ebp + 24]		;src1Stride
	mov		edi,	[ebp + 28]		;h
	
	test	edi,	1
	jz		.loop
	
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + 8]
	pavgb	mm0,	[edx]
	pavgb	mm1,	[edx + 8]
	vo_mmx_%1 mm0,	[eax],		mm2
	vo_mmx_%1 mm1,	[eax + 8],	mm3
	
	add		eax,	ebx
	add		ecx,	esi
	add		edx,	16
	
.loop:
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + 8]
	pavgb	mm0,	[edx]
	pavgb	mm1,	[edx + 8]
	vo_mmx_%1 mm0,	[eax],		mm2
	vo_mmx_%1 mm1,	[eax + 8],	mm3
	
	add		eax,	ebx
	add		ecx,	esi
	add		edx,	16
	
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + 8]
	pavgb	mm0,	[edx]
	pavgb	mm1,	[edx + 8]
	
	vo_mmx_%1 mm0,	[eax],		mm2
	vo_mmx_%1 mm1,	[eax + 8],	mm3
	
	add		eax,	ebx
	add		ecx,	esi
	add		edx,	16
	
	sub		edi,	2
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro 

PIXELS16_L2_MMXEXT avg
PIXELS16_L2_MMXEXT put

;pixels8_l2: the number of 8 stands for the pixel width of current block , "l2"  stands for 1/4 pixel position mc
;the level of mc is divided into 4 level , they are full , half, quarter , one eighth , "l2" is for quarter
;void vo_put_pixels8_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h)
;void vo_avg_pixels8_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h)

%macro PIXELS8_L2_MMXEXT 1
vo_global %1_pixels8_l2_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;src1
	mov		edx,	[ebp + 16]		;src2
	mov		ebx,	[ebp + 20]		;dstStride
	mov		esi,	[ebp + 24]		;src1Stride
	mov		edi,	[ebp + 28]		;h
	
	test	edi,	1
	jz		.loop
	
	movq	mm0,	[ecx]
	pavgb	mm0,	[edx]
	;movq	[eax],	mm0
	vo_mmx_%1	mm0,[eax],mm1
	
	lea		eax,	[eax + ebx]
	lea		ecx,	[ecx + esi]
	lea		edx,	[edx + 8]
	
.loop:

	movq	mm0,	[ecx]
	movq	mm1,	[ecx + esi]
	pavgb	mm0,	[edx]
	pavgb	mm1,	[edx + 8]
	;movq	[eax],			mm0
	;movq	[eax + ebx],	mm1
	vo_mmx_%1	mm0,[eax],			mm2
	vo_mmx_%1	mm1,[eax + ebx],	mm3
	
	lea		eax,	[eax + 2*ebx]
	lea		ecx,	[ecx + 2*esi]
	lea		edx,	[edx + 16]
	
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + esi]
	pavgb	mm0,	[edx]
	pavgb	mm1,	[edx + 8]
	;movq	[eax],			mm0
	;movq	[eax + ebx],	mm1
	vo_mmx_%1	mm0,[eax],			mm2
	vo_mmx_%1	mm1,[eax + ebx],	mm3
	
	lea		eax,	[eax + 2*ebx]
	lea		ecx,	[ecx + 2*esi]
	lea		edx,	[edx + 16]
	
	sub		edi,	4
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

PIXELS8_L2_MMXEXT avg
PIXELS8_L2_MMXEXT put