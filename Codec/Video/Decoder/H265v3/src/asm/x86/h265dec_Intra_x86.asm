;
; VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
;
; All data and information contained in or disclosed by this document are
; confidential and proprietary information of VisualOn, and all rights
; therein are expressly reserved. By accepting this material, the
; recipient agrees that this material and the information contained
; therein are held in confidence and in trust. The material may only be
; used and/or disclosed as authorized in a license agreement controlling
; such use and disclosure.
;************************************************************************/
    %include "x86util.asm"
	%include "h265dec_ASM_config.asm"
	%include "h265dec_ID.asm"    ;include it to hide your symbol names
	
	
	cextern	CONST_S16_POS4
	cextern CONST_U16_16
	cextern CONST_U8_64
	cextern CONST_U8_1_64
	cextern CONST_U16_POS32
	cextern CONST_U8_1
	cextern CONST_S16_POS1
	cextern CONST_U16_POS8
	cextern CONST_U8_Planar_nTbs4
	cextern CONST_U8_Planar_nTbs8
	cextern CONST_U8_Planar_nTbs16
	cextern CONST_U8_Planar_nTbs32
	cextern	intra_pred_angle
	cextern	inv_angle
	
	section  .text
	
;input	%1 = s_buf
;		%2 = s_stride
;		%3 = d_buf
;		%4 = d_stride
;		%5 = gpr_tmp

%macro Intra_PreAngle_Mode_LT8_Ref_Assign	5
	%define	s_buf		%1
	%define s_stride	%2
	%define d_buf		%3
	%define	d_stride	%4
	%define	gpr_tmp		%5
	
	movq	m0,	[s_buf + 0*s_stride]
	movq	m1,	[s_buf + 1*s_stride]
	movq	m2,	[s_buf + 2*s_stride]
	movq	m3,	[s_buf + 3*s_stride]
	punpcklbw	m0,	m1	;a0 b0 a1 b1 a2 b2 a3 b3 a4 b4 a5 b5 a6 b6 a7 b7
	punpcklbw	m2,	m3	;c0 d0 c1 d1 c2 d2 c3 d3 c4 d4 c5 d5 c6 d6 c7 d7
	
	movq	m1,	[s_buf + 4*s_stride]
	movq	m3,	[s_buf + 5*s_stride]
	movq	m4,	[s_buf + 6*s_stride]
	movq	m5,	[s_buf + 7*s_stride]
	
	punpcklbw	m1,	m3	;e0 f0 e1 f1 e2 f2 e3 f3 e4 f4 e5 f5 e6 f6 e7 f7
	punpcklbw	m4,	m5	;g0 h0 g1 h1 g2 h2 g3 h3 g4 h4 g5 h5 g6 h6 g7 h7
	
	mova	m3,	m0
	punpcklwd	m0,	m2	;a0 b0 c0 d0 a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3
	punpckhwd	m3,	m2	;a4 b4 c4 d4 a5 b5 c5 d5 a6 b6 c6 d6 a7 b7 c7 d7
	mova	m2,	m1
	punpcklwd	m1,	m4	;e0 f0 g0 h0 e1 f1 g1 h1 e2 f2 g2 h2 e3 f3 g3 h3
	punpckhwd	m2,	m4	;e4 f4 g4 h4 e5 f5 g5 h5 e6 f6 g6 h6 e7 f7 g7 h7
	mova	m4,	m0
	punpckldq	m0,	m1	;a0 b0 c0 d0 e0 f0 g0 h0 a1 b1 c1 d1 e1 f1 g1 h1
	punpckhdq	m4,	m1	;a2 b2 c2 d2 e2 f2 g2 h2 a3 b3 c3 d3 e3 f3 g3 h3
	mova	m1,	m3
	punpckldq	m1,	m2	;a4 b4 c4 d4 e4 f4 g4 h4 a5 b5 c5 d5 e5 f5 g5 h5
	punpckhdq	m3,	m2	;a6 b6 c6 d6 e6 f6 g6 h6 a7 b7 c7 d7 e7 f7 g7 h7
	
	lea		gpr_tmp, [3 * d_stride]
	
	movq	[d_buf + 0*d_stride], m0
	movhps	[d_buf + 1*d_stride], m0
	
	movq	[d_buf + 2*d_stride], m4
	movhps	[d_buf + gpr_tmp],	  m4
	
	lea		gpr_tmp, [gpr_tmp + 2*d_stride]
	movq	[d_buf + 4*d_stride], m1
	movhps	[d_buf + gpr_tmp],    m1
	
	lea		gpr_tmp, [gpr_tmp + d_stride]
	movq	[d_buf + gpr_tmp],    m3
	lea		gpr_tmp, [gpr_tmp + d_stride]
	movhps	[d_buf + gpr_tmp],    m3

%endmacro

; predModeIntra == 26 && cIdx == 0 && nTbS < 32 

;input	%1 = src_buf
;		%2 = dest_buf
;		%3 = dest_stride
;		%4 = nTbs
;		%5 = reg_top_0
;		%6 = reg_left_N1
;		%7 = gpr_3strides

%macro Intra_Mode26_cIdx0_nTbsLT32_assign	7
	%define src_buf			%1
	%define dst_buf			%2
	%define dst_stride		%3
	%define block_size		%4
	%define reg_t_0			%5
	%define reg_l_neg1		%6
	%define gpr_3strides	%7
	
	punpcklqdq	reg_t_0, reg_t_0
	punpcklqdq	reg_l_neg1, reg_l_neg1
	
	%assign	%%nIndex	0
	%define nCount		block_size/8
	%rep nCount
	
		movq	m2,	[%1 + 8*%%nIndex]		;left[x]
		punpcklbw	m2, m7
		psubw	m2,	reg_l_neg1
		psraw	m2, 1
		paddw	m2,	reg_t_0			
		packuswb	m2, m2					;m2 = Clip(top[0] + ((left[x] - left[-1]) >> 1))
		
		%if ARCH_X86_64	== 1
			movq	r1, m2
			mov		byte [dst_buf + 0*dst_stride], r1b
			mov		byte [dst_buf + 1*dst_stride], r1h
			
			shr		r1, 16
			mov		byte [dst_buf + 2*dst_stride], r1b
			lea		dst_buf, [dst_buf + gpr_3strides]
			mov		byte [dst_buf], r1h
			
			shr		r1, 16
			mov		byte [dst_buf + 1*dst_stride], r1b
			mov		byte [dst_buf + 2*dst_stride], r1h
			lea		dst_buf, [dst_buf + gpr_3strides]
					
			shr		r1, 16
			mov		byte [dst_buf + 0*dst_stride], r1b
			mov		byte [dst_buf + 1*dst_stride], r1h
		%else
			movd	r1,m2
			mov		byte [dst_buf + 0*dst_stride], r1b
			mov		byte [dst_buf + 1*dst_stride], r1h
			
			shr		r1, 16
			mov		byte [dst_buf + 2*dst_stride], r1b
			mov		byte [dst_buf +   gpr_3strides], r1h
			
			palignr	m2,	m2,	4
			movd	r1,m2
			
			lea		dst_buf,	[dst_buf + 4*dst_stride]
			mov		byte [dst_buf + 0*dst_stride], r1b
			mov		byte [dst_buf + 1*dst_stride], r1h
			
			shr		r1, 16
			mov		byte [dst_buf + 2*dst_stride], r1b
			mov		byte [dst_buf +   gpr_3strides], r1h
			
		%endif
		
		%if ARCH_X86_64 == 1
			lea		dst_buf,	[dst_buf + 2*dst_stride]
		%else
			lea		dst_buf,	[dst_buf + 4*dst_stride]
		%endif
		%assign	%%nIndex	%%nIndex+1
	%endrep
%endmacro

;input	%1 = s_reg
;		%2 = d_buf
;		%3 = d_stride
;		%4 = strides_3

%macro Intra_Ref_Assign_4Lines_LT18	4
	%define	s_mmreg		%1
	%define	d_buf		%2
	%define	d_stride	%3
	%define	strides_3	%4
	
	movd	r2,	s_mmreg
	mov		byte [d_buf + 0*d_stride], r2b
	mov		byte [d_buf + 1*d_stride], r2h
	
	shr		r2,	16
	mov		byte [d_buf + 2*d_stride], r2b
	mov		byte [d_buf +  strides_3], r2h
	
	palignr	s_mmreg,s_mmreg,4
	movd	r2,	s_mmreg
	lea		r0,	 [d_buf +  strides_3]
	mov		byte [d_buf + 0*d_stride], r2b
	mov		byte [d_buf + 1*d_stride], r2h
	
	shr		r2,	16
	mov		byte [d_buf + 2*d_stride], r2b
	mov		byte [d_buf +  strides_3], r2h
%endmacro

;input	%1 = nTbs
;		%2 = GE18	1:GE18	0:LT18
%macro H265_IntraPred_Angular_body_x86	2
	%define	TRANS_BLOCK_SIZE	%1
	%define PRED_MODE_GE18		%2
	
	lea		r0,	[4*r0]
	mov		r3, dword [intra_pred_angle + r0 - 8]	;r3 = angle
	
	mov		r5,	r3
	imul	r5,	%1									;r5 = nTbS * angle
	sar		r5,	5									;r5 = last = nTbS * angle >> 5
	
	%if PRED_MODE_GE18
		lea		r4,	[r1 - 1]						;r4 = p_ref = top - 1
	%else
		lea		r4,	[r2 - 1]						;r4 = p_ref = left - 1
	%endif
	
	cmp		r5,	-2
	jg		.assign_ref_init_%1
	lea		r4,	[rsp + %1]							;r4 = p_ref
	mov		r0,dword [inv_angle + r0 - 44]			;r0 = inv_angle_mode_value

.init_ref_%1:
	
	%if PRED_MODE_GE18
		%define src_1st	r2
		%define src_2nd	r1
	%else
		%define src_1st	r1
		%define src_2nd	r2
	%endif
	
	mov		r3,	r5
	imul	r3,	r0					;r3 = x * inv_angle_mode_value
	add		r3,	128					;r3 = x * inv_angle_mode_value + 128
	sar		r3,	8					;r3 = (x * inv_angle_mode_value + 128) >> 8
	dec		r3						;r3 = (x * inv_angle_mode_value + 128) >> 8 - 1
	mov		r3b, byte [src_1st + r3]		;r3 = left[- 1 + (x * inv_angle_mode_value + 128) >> 8]
	mov		[r4 + r5], byte r3b		
	inc		r5
	jne		.init_ref_%1
	
	%if %1 == 4
		mov	r3d,	dword [src_2nd - 1]
		mov	[r4],	dword r3d
	%elif %1 == 8
		movq	m0,		[src_2nd - 1]
		movq	[r4],	m0
	%elif %1 == 16
		movq	m0,		[src_2nd - 1]
		movq	[r4],	m0
		movq	m1,		[src_2nd - 1 + 8]
		movq	[r4 + 8],	m1
	%elif %1 == 32
		movq	m0,		[src_2nd - 1]
		movq	[r4],	m0
		movq	m1,		[src_2nd - 1 + 8]
		movq	[r4 + 8],	m1
		movq	m2,		[src_2nd - 1 + 16]
		movq	[r4 + 16],	m2
		movq	m3,		[src_2nd - 1 + 24]
		movq	[r4 + 24],	m3
	%endif
	
	mov	r3b, byte [src_2nd + %1 - 1]
	mov	byte [r4 + %1],	r3b
	
	%undef	src_1st
	%undef	src_2nd
	
.assign_ref_init_%1:
	mov		r0, dword [rsp + rsp_offset + arg_pos(5)]	;predModeIntra
	lea		r0,	[4*r0]
	mov		r3, dword [intra_pred_angle + r0 - 8]		;r3 = angle
	
	mov		r5,	0										;r5 is loop index
	mova	m7,	[CONST_U16_16]
	mov		r0,	[rsp + rsp_offset + arg_pos(0)]			;r0 = p_cur
	mov		r6d,dword [rsp + rsp_offset + arg_pos(3)]	;r6 = dst_stride
	
%if PRED_MODE_GE18
.assign_ref_body_%1:
	mov		r1,	r5
	inc		r1
	imul	r1,	r3
	mov		r2,	r1
	sar		r1,	5		;idx
	and		r2,	31		;fact
	
	cmp		r2,	0
	je		.fact_eq_0_%1
	imul	r2, 257
	sub		r2b,32
	neg		r2b
	movd	m6,	r2
	pshuflw	m6, m6, 0	;fact,32-fact,fact,32-fact,fact,32-fact,fact,32-fact...
	
	;%if PRED_MODE_GE18
	mov		r2,	r5
	imul	r2,	r6
	
	%if %1 == 4
	
		movq	m0,	[r4 + r1 + 1]
		movq	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4
		
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		movd	[r0 + r2],	m0
		
	%elif %1 == 8
		
		movq	m0,	[r4 + r1 + 1]
		movq	m2,	[r4 + r1 + 9]
		punpcklqdq	m0, m2
		mova	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
		
		pshufd		m6,	m6, 0
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		movq	[r0 + r2],	m0
		
	%elif %1 == 16
		
		movq	m0,	[r4 + r1 + 1]
		movq	m4,	[r4 + r1 + 9]
		punpcklqdq	m0, m4
		mova	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1				;m0 = a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
		
		movq	m2,	[r4 + r1 + 17]
		punpcklqdq	m4, m2
		mova	m2,	m4
		palignr	m4,	m2,	1
		punpcklbw	m2,	m4				;m2 = a8 a9 a9 a10 a10 a11 a11 a12 a12 a13 a13 a14 a14 a15 a15 a16
		
		pshufd		m6,	m6, 0
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		
		pmaddubsw	m2,	m6
		paddw		m2,	m7
		psraw		m2,	5
		packuswb	m2,	m2
		
		movq	[r0 + r2],		m0
		movq	[r0 + r2 + 8],	m2
	
	%elif %1 == 32
		movq	m0,	[r4 + r1 + 1]
		movq	m1,	[r4 + r1 + 9]
		punpcklqdq	m0, m1
		mova	m2,	m0
		palignr	m2,	m0,	1
		punpcklbw	m0,	m2				;m0 = part 0
		
		movq	m2,	[r4 + r1 + 17]
		punpcklqdq	m1, m2
		mova	m3,	m1
		palignr	m3,	m1,	1
		punpcklbw	m1,	m3				;m1 = part 1
		
		movq	m3,	[r4 + r1 + 25]
		punpcklqdq	m2, m3
		mova	m4,	m2
		palignr	m4,	m2,	1
		punpcklbw	m2,	m4				;m2 = part 2
		
		movq	m4,	[r4 + r1 + 33]
		punpcklqdq	m3, m4
		mova	m5,	m3
		palignr	m5,	m3,	1
		punpcklbw	m3,	m5				;m3 = part 2
		
		pshufd		m6,	m6, 0
		
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		
		pmaddubsw	m1,	m6
		paddw		m1,	m7
		psraw		m1,	5
		packuswb	m1,	m1
		
		pmaddubsw	m2,	m6
		paddw		m2,	m7
		psraw		m2,	5
		packuswb	m2,	m2
		
		pmaddubsw	m3,	m6
		paddw		m3,	m7
		psraw		m3,	5
		packuswb	m3,	m3
		
		movq	[r0 + r2 +  0],	m0
		movq	[r0 + r2 +  8],	m1
		movq	[r0 + r2 + 16],	m2
		movq	[r0 + r2 + 24],	m3
	
	%endif
	inc		r5
	cmp		r5,	%1
	jne		.assign_ref_body_%1
	jp		.special_edge_%1
.fact_eq_0_%1:	
	
	mov		r2,	r5		;r5 = y_index
	imul	r2,	r6		;r6 = stride
	%if %1	==	4
		movd	m0,	[r4 + r1 + 1]
		movd	[r0 + r2 + 0],	m0
	%elif %1 == 8
		movq	m0,	[r4 + r1 + 1]
		movq	[r0 + r2 + 0],	m0
	%elif %1 == 16
		movq	m0,	[r4 + r1 + 1]
		movq	[r0 + r2 + 0],	m0
		movq	m0,	[r4 + r1 + 1 + 8]
		movq	[r0 + r2 + 8],	m0
	%elif %1 == 32
		movq	m0,	[r4 + r1 + 1]
		movq	[r0 + r2 + 0],	m0
		movq	m0,	[r4 + r1 + 1 + 8]
		movq	[r0 + r2 + 8],	m0
		movq	m0,	[r4 + r1 + 1 + 16]
		movq	[r0 + r2 + 16],	m0
		movq	m0,	[r4 + r1 + 1 + 24]
		movq	[r0 + r2 + 24],	m0
	%endif
	
	inc		r5
	cmp		r5,	%1
	jne		.assign_ref_body_%1
	
%else ;PRED_MODE_GE18 == 0

	%assign	%%LoopIndex	0
	%define	LoopNum		%1
	%rep LoopNum
		mov		r1,	%%LoopIndex
		inc		r1
		imul	r1,	r3		;r3 = angle
		mov		r2,	r1
		sar		r1,	5		;idx
		and		r2,	31		;fact
		
		imul	r2, 257
		sub		r2b,32
		neg		r2b
		movd	m6,	r2
		pshuflw	m6, m6, 0	;fact,32-fact,fact,32-fact,fact,32-fact,fact,32-fact...
		
		%if %%LoopIndex == 0
			lea	r5,	[rsp + 4*%1 + 16]
			and	r5,	~0x0F
		%endif
		
		%if %1 == 4
	
			movq	m0,	[r4 + r1 + 1]
			movq	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4
			
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			%if %%LoopIndex == 0
				mova	m2,	m0
			%elif %%LoopIndex == 1
				mova	m3,	m0
			%elif %%LoopIndex == 2
				mova	m4,	m0
			%elif %%LoopIndex == 3
				mova	m5,	m0
			%endif
		
		%elif %1 == 8
			
			movq	m0,	[r4 + r1 + 1]
			movq	m2,	[r4 + r1 + 9]
			punpcklqdq	m0, m2
			mova	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
			
			pshufd		m6,	m6, 0
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			%if %%LoopIndex == 0
				mova	m3,	m0
			%elif %%LoopIndex == 1
				mova	m4,	m0
			%elif %%LoopIndex == 2
				mova	m5,	m0
			%elif %%LoopIndex == 7
				mova	m7,	m0
			%else
				movq	[r5 + %1 * %%LoopIndex],	m0
			%endif
			
		%elif %1 == 16
			movq	m0,	[r4 + r1 + 1]
			movq	m4,	[r4 + r1 + 9]
			punpcklqdq	m0, m4
			mova	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1				;m0 = a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
			
			movq	m2,	[r4 + r1 + 17]
			punpcklqdq	m4, m2
			mova	m2,	m4
			palignr	m4,	m2,	1
			punpcklbw	m2,	m4				;m2 = a8 a9 a9 a10 a10 a11 a11 a12 a12 a13 a13 a14 a14 a15 a15 a16
			
			pshufd		m6,	m6, 0
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			pmaddubsw	m2,	m6
			paddw		m2,	m7
			psraw		m2,	5
			packuswb	m2,	m2
			
			punpcklqdq	m0,	m2
			mova	[r5 + %1 * %%LoopIndex],		m0
			
		%elif %1 == 32
			movq	m0,	[r4 + r1 + 1]
			movq	m1,	[r4 + r1 + 9]
			punpcklqdq	m0, m1
			mova	m2,	m0
			palignr	m2,	m0,	1
			punpcklbw	m0,	m2				;m0 = part 0
			
			movq	m2,	[r4 + r1 + 17]
			punpcklqdq	m1, m2
			mova	m3,	m1
			palignr	m3,	m1,	1
			punpcklbw	m1,	m3				;m1 = part 1
			
			movq	m3,	[r4 + r1 + 25]
			punpcklqdq	m2, m3
			mova	m4,	m2
			palignr	m4,	m2,	1
			punpcklbw	m2,	m4				;m2 = part 2
			
			movq	m4,	[r4 + r1 + 33]
			punpcklqdq	m3, m4
			mova	m5,	m3
			palignr	m5,	m3,	1
			punpcklbw	m3,	m5				;m3 = part 2
			
			pshufd		m6,	m6, 0
		
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			pmaddubsw	m1,	m6
			paddw		m1,	m7
			psraw		m1,	5
			packuswb	m1,	m1
			
			pmaddubsw	m2,	m6
			paddw		m2,	m7
			psraw		m2,	5
			packuswb	m2,	m2
			
			pmaddubsw	m3,	m6
			paddw		m3,	m7
			psraw		m3,	5
			packuswb	m3,	m3
			
			punpcklqdq	m0,	m1
			punpcklqdq	m2,	m3
			
			mova	[r5 + %1 * %%LoopIndex +  0],	m0
			mova	[r5 + %1 * %%LoopIndex + 16],	m2
			
		%endif
		%assign	%%LoopIndex	%%LoopIndex + 1
	%endrep
	
	;mov	r2,	r5
	;imul	r2,	r6	;r6 = dest_stride
	%if %1 == 4
		;m2,m3,m4,m5
		punpcklbw	m2,	m3	;a0 b0 a1 b1 a2 b2 a3 b3
		punpcklbw	m4, m5	;c0 d0 c1 d1 c2 d2 c3 d3
		punpcklwd	m2,	m4	;a0 b0 c0 d0 a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3
		
		movd	[r0 + 0*r6],	m2
		palignr	m2,	m2, 4
		movd	[r0 + 1*r6],	m2
		palignr	m2,	m2, 4
		movd	[r0 + 2*r6],	m2
		palignr	m2,	m2, 4
		lea		r6,	[3*r6]
		movd	[r0 + r6],	m2
		
	%elif %1 == 8
		;m3,m4,m5,m7
		;0, 1, 2, 7
			
		movq	m2,	[r5 + 3*8]	;d
		movq	m0,	[r5 + 4*8]	;e
		movq	m1,	[r5 + 5*8]	;f
		
		punpcklbw m3, m4	;a0 b0 a1 b1 a2 b2 a3 b3 a4 b4 a5 b5 a6 b6 a7 b7
		punpcklbw m5, m2	;c0 d0 c1 d1 c2 d2 c3 d3 c4 d4 c5 d5 c6 d6 c7 d7
		punpcklbw m0, m1	;e0 f0 e1 f1 e2 f2 e3 f3 e4 f4 e5 f5 e6 f6 e7 f7
		movq	m1,	[r5 + 6*8]	;g
		;movd	m2,	[r5 + 7*8]	;h
		punpcklbw m1, m7	;g0 h0 g1 h1 g2 h2 g3 h3 g4 h4 g5 h5 g6 h6 g7 h7
		
		;m2,m4,m6,m7
		mova	m2,	m3
		punpcklwd	m2,	m5	;a0 b0 c0 d0 a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3
		punpckhwd	m3,	m5	;a4 b4 c4 d4 a5 b5 c5 d5 a6 b6 c6 d6 a7 b7 c7 d7
		mova	m4,	m0
		punpcklwd	m0,	m1	;e0 f0 g0 h0 e1 f1 g1 h1 e2 f2 g2 h2 e3 f3 g3 h3
		punpckhwd	m4,	m1	;e4 f4 g4 h4 e5 f5 g5 h5 e6 f6 g6 h6 e7 f7 g7 h7
		
		;m1,m5,m6,m7
		mova m1, m2
		punpckldq	m1,	m0	;a0 b0 c0 d0 e0 f0 g0 h0 a1 b1 c1 d1 e1 f1 g1 h1
		punpckhdq	m2, m0	;a2 b2 c2 d2 e2 f2 g2 h2 a3 b3 c3 d3 e3 f3 g3 h3
		mova m0, m3
		punpckldq	m0,	m4	;a4 b4 c4 d4 e4 f4 g4 h4 a5 b5 c5 d5 e5 f5 g5 h5
		punpckhdq	m3, m4	;a6 b6 c6 d6 e6 f6 g6 h6 a7 b7 c7 d7 e7 f7 g7 h7
		
		lea		r2,	[3*r6]
		
		movq	[r0 + 0*r6],	m1
		movhps	[r0 + 1*r6],	m1
		movq	[r0 + 2*r6],	m2
		movhps	[r0 + r2],		m2
		
		movq	[r0 + 4*r6],	m0
		lea		r2,	[r2 + 2*r6]
		movhps	[r0 + r2],	m0
		lea		r2,	[r2 + 1*r6]
		movq	[r0 + r2],	m3
		lea		r2,	[r2 + 1*r6]
		movhps	[r0 + r2],	m3
		
	%elif %1 == 16
		;m3,m4,m5,m6,m7
		
		;lea	r5,	[rsp + 4*%1 + 16]
		;and	r5,	~0x0F
		
		%define BLOCK_NUM	2
		
		%assign	%%block_x	0
		%assign	%%block_y	0
		
		%rep BLOCK_NUM
			%rep BLOCK_NUM

				%define	s_buf		r5 + 16*%%block_x*8 + 8*%%block_y
				%define s_stride	16
				%define d_buf		r0 + 8*%%block_x
				%define	d_stride	r6
				%define	gpr_tmp		r2
				
				Intra_PreAngle_Mode_LT8_Ref_Assign s_buf, s_stride, d_buf, d_stride, gpr_tmp
				
				%assign	%%block_x %%block_x + 1
			%endrep
			
			lea		r0,	[r0 + 8*r6]
			%assign	%%block_x	0
			%assign	%%block_y %%block_y + 1
		%endrep
		
	%elif %1 == 32
		;lea	r5,	[rsp + 4*%1 + 16]
		;and	r5,	~0x0F
		
		%define BLOCK_NUM	4
		
		%assign	%%block_x	0
		%assign	%%block_y	0
		
		%rep BLOCK_NUM
			%rep BLOCK_NUM

				%define	s_buf		r5 + 32*%%block_x*8 + 8*%%block_y
				%define s_stride	32
				%define d_buf		r0 + 8*%%block_x
				%define	d_stride	r6
				%define	gpr_tmp		r2
				
				Intra_PreAngle_Mode_LT8_Ref_Assign s_buf, s_stride, d_buf, d_stride, gpr_tmp
				
				%assign	%%block_x %%block_x + 1
			%endrep
			
			lea		r0,	[r0 + 8*r6]
			%assign	%%block_x	0
			%assign	%%block_y %%block_y + 1
		%endrep
		
	%endif
	
%endif
	
.special_edge_%1:
	
	%if %1 < 32
		mov	r4d, dword [rsp + rsp_offset + arg_pos(4)]		;cIdx
		mov	r5d, dword [rsp + rsp_offset + arg_pos(5)]		;mode	
		cmp	r4,	0
		jne	.done_end_%1
		%if PRED_MODE_GE18 == 1
			cmp	r5,	26
		%else
			cmp	r5,	10
		%endif
		jne	.done_end_%1
		
		mov	r0,	[rsp + rsp_offset + arg_pos(0)]
		mov	r1,	[rsp + rsp_offset + arg_pos(1)]		;top
		mov	r2,	[rsp + rsp_offset + arg_pos(2)]		;left
		
		%if PRED_MODE_GE_18
			lea	r5,	[r2 - 1]
			mov	r3b, byte  [r1]							;top[0]
			mov	r4d, dword [r5]							;left[-1]
			and r4,	0xFF
		%else
			lea	r5,	[r1 - 1]
			mov	r3b, byte  [r2]							;left[0]
			mov	r4d, dword [r5]							;top[-1]
			and r4,	0xFF
		%endif
		
		movd	m0,	r3
		movd	m1,	r4
		pshuflw	m0, m0, 0							;top[0]
		pshuflw	m1, m1, 0							;left[-1]
		
		pxor	m7,	m7
		mov	r6d, dword [rsp + rsp_offset + arg_pos(3)]		;dst_stride
		lea	r5,	[3*r6]
		
		%if PRED_MODE_GE_18
			%if	  %1 == 4
				
				movd	m2,	[r2]		;left[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(top[0] + ((left[x] - left[-1]) >> 1))
				
				movd	r1d,m2
				mov		byte [r0 + 0*r6], r1b
				mov		byte [r0 + 1*r6], r1h
				shr		r1, 16
				mov		byte [r0 + 2*r6], r1b
				lea		r0,	[r0 + r5]
				mov		byte [r0], r1h		;error: cannot use A/B/C/DH with instruction needing REX
				
			%elif %1 == 8
			
				Intra_Mode26_cIdx0_nTbsLT32_assign r2, r0, r6, 8, m0, m1, r5
				
			%elif %1 == 16
				
				Intra_Mode26_cIdx0_nTbsLT32_assign r2, r0, r6, 16, m0, m1, r5
				
			%endif
		%else
			%if	  %1 == 4
				
				movd	m2,	[r1]		;top[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movd	[r0], m2
				
			%elif %1 == 8
				
				punpcklwd	m0,	m0
				punpcklwd	m1,	m1
				
				movq	m2,	[r1]		;top[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0], m2
				
			%elif %1 == 16
				
				punpcklwd	m0,	m0
				punpcklwd	m1,	m1
				
				movq	m2,	[r1]		;top[x]
				movq	m3,	[r1 + 8]	;top[x + 8]
				
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0], m2
				
				punpcklbw	m3, m7
				psubw	m3,	m1
				psraw	m3, 1
				paddw	m3,	m0			
				packuswb	m3, m3		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0 + 8], m3
				
			%endif
			
		%endif
	%endif
	
.done_end_%1:
	add		rsp,	stack_room
%endmacro

;input	%1 = nTbs
;		%2 = GE18	1:GE18	0:LT18
%macro H265_IntraPred_Angular_body_x64	2
	%define	TRANS_BLOCK_SIZE	%1
	%define PRED_MODE_GE18		%2
	
	%define	pRef		r9
	%define	pTop		r1
	%define	pLeft		r2
	%define	rMode		r5
	%define	rAngle		r7
	
	lea		r11,[intra_pred_angle]
	lea		r6,	[4 * rMode]			;r5 = mode
	lea		r11,[r11 + r6 - 8]
	mov		r7d, dword [r11]	;r7 = angle
	movsxd	r7,	r7d
	
	mov		r8,	r7
	imul	r8,	%1									;r8 = nTbS * angle
	sar		r8,	5									;r8 = last = nTbS * angle >> 5
	
	%if PRED_MODE_GE18
		lea		r9,	[r1 - 1]						;r9 = p_ref = top - 1
	%else
		lea		r9,	[r2 - 1]						;r9 = p_ref = left - 1
	%endif
	
	cmp		r8,	-2
	jg		.assign_ref_init_%1
	lea		r9,[rsp + %1]							;r9 = p_ref = 
	lea		r11,[inv_angle]
	lea		r11,[r11 + r6 - 44]
	mov		r6d,dword [r11]			;r6 = inv_angle_mode_value
	movsxd	r6,	r6d

.init_ref_%1:
	
	%if PRED_MODE_GE18
		%define src_1st	r2
		%define src_2nd	r1
	%else
		%define src_1st	r1
		%define src_2nd	r2
	%endif
	
	mov		r10,r8
	imul	r10,r6					;r10 = x * inv_angle_mode_value
	add		r10,128					;r10 = x * inv_angle_mode_value + 128
	sar		r10,8					;r10 = (x * inv_angle_mode_value + 128) >> 8
	dec		r10						;r10 = (x * inv_angle_mode_value + 128) >> 8 - 1
	mov		r10b, byte [src_1st + r10]	;r9 = top[- 1 + (x * inv_angle_mode_value + 128) >> 8]
	mov		[r9 + r8], byte r10b		
	inc		r8
	jne		.init_ref_%1
	
	%if %1 == 4
		mov	r10d,	dword [src_2nd - 1]
		mov	[r9],	dword r10d
	%elif %1 == 8
		movq	m0,		[src_2nd - 1]
		movq	[r9],	m0
	%elif %1 == 16
		movq	m0,		[src_2nd - 1]
		movq	[r9],	m0
		movq	m1,		[src_2nd - 1 + 8]
		movq	[r9 + 8],	m1
	%elif %1 == 32
		movq	m0,		[src_2nd - 1]
		movq	[r9],	m0
		movq	m1,		[src_2nd - 1 + 8]
		movq	[r9 + 8],	m1
		movq	m2,		[src_2nd - 1 + 16]
		movq	[r9 + 16],	m2
		movq	m3,		[src_2nd - 1 + 24]
		movq	[r9 + 24],	m3
	%endif
	
	mov	r10b, byte [src_2nd + %1 - 1]
	mov	byte [r9 + %1],	r10b
	
	%undef	src_1st
	%undef	src_2nd

.assign_ref_init_%1:
	
	mov		r8,	0
	mova	m7,	[CONST_U16_16]

%if PRED_MODE_GE18
.assign_ref_body_%1:
	mov		r6,	r8
	inc		r6
	imul	r6,	r7
	mov		r10,r6
	sar		r6,	5		;idx
	and		r10,31		;fact
	
	cmp		r10,0
	je		.fact_eq_0_%1
	imul	r10, 257
	sub		r10b,32
	neg		r10b
	movd	m6,	r10
	pshuflw	m6, m6, 0
	
	mov		r10,r8
	imul	r10,r3
	
	%if %1 == 4
	
		movq	m0,	[r9 + r6 + 1]
		movq	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4
		
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		movd	[r0 + r10],	m0
		
	%elif %1 == 8
		
		movq	m0,	[r9 + r6 + 1]
		movq	m2,	[r9 + r6 + 9]
		punpcklqdq	m0, m2
		mova	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
		
		pshufd		m6,	m6, 0
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		movq	[r0 + r10],	m0
		
	%elif %1 == 16
		
		movq	m0,	[r9 + r6 + 1]
		movq	m4,	[r9 + r6 + 9]
		punpcklqdq	m0, m4
		mova	m1,	m0
		palignr	m1,	m0,	1
		punpcklbw	m0,	m1				;m0 = a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
		
		movq	m2,	[r9 + r6 + 17]
		punpcklqdq	m4, m2
		mova	m2,	m4
		palignr	m4,	m2,	1
		punpcklbw	m2,	m4				;m2 = a8 a9 a9 a10 a10 a11 a11 a12 a12 a13 a13 a14 a14 a15 a15 a16
		
		pshufd		m6,	m6, 0
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		
		pmaddubsw	m2,	m6
		paddw		m2,	m7
		psraw		m2,	5
		packuswb	m2,	m2
		
		movq	[r0 + r10],		m0
		movq	[r0 + r10 + 8],	m2
	
	%elif %1 == 32
		movq	m0,	[r9 + r6 + 1]
		movq	m1,	[r9 + r6 + 9]
		punpcklqdq	m0, m1
		mova	m2,	m0
		palignr	m2,	m0,	1
		punpcklbw	m0,	m2				;m0 = part 0
		
		movq	m2,	[r9 + r6 + 17]
		punpcklqdq	m1, m2
		mova	m3,	m1
		palignr	m3,	m1,	1
		punpcklbw	m1,	m3				;m1 = part 1
		
		movq	m3,	[r9 + r6 + 25]
		punpcklqdq	m2, m3
		mova	m4,	m2
		palignr	m4,	m2,	1
		punpcklbw	m2,	m4				;m2 = part 2
		
		movq	m4,	[r9 + r6 + 33]
		punpcklqdq	m3, m4
		mova	m5,	m3
		palignr	m5,	m3,	1
		punpcklbw	m3,	m5				;m3 = part 2
		
		pshufd		m6,	m6, 0
		
		pmaddubsw	m0,	m6
		paddw		m0,	m7
		psraw		m0,	5
		packuswb	m0,	m0
		
		pmaddubsw	m1,	m6
		paddw		m1,	m7
		psraw		m1,	5
		packuswb	m1,	m1
		
		pmaddubsw	m2,	m6
		paddw		m2,	m7
		psraw		m2,	5
		packuswb	m2,	m2
		
		pmaddubsw	m3,	m6
		paddw		m3,	m7
		psraw		m3,	5
		packuswb	m3,	m3
		
		movq	[r0 + r10 +  0],	m0
		movq	[r0 + r10 +  8],	m1
		movq	[r0 + r10 + 16],	m2
		movq	[r0 + r10 + 24],	m3
	
	%endif
	
	inc		r8
	cmp		r8,	%1
	jne		.assign_ref_body_%1
	jp		.special_edge_%1
.fact_eq_0_%1:	
	
	mov		r10,r8		;r5 = y_index
	imul	r10,r3		;r6 = stride
	%if %1	==	4
		movd	m0,	[r9 + r6 + 1]
		movd	[r0 + r10 + 0],	m0
	%elif %1 == 8
		movq	m0,	[r9 + r6 + 1]
		movq	[r0 + r10 + 0],	m0
	%elif %1 == 16
		movq	m0,	[r9 + r6 + 1]
		movq	[r0 + r10 + 0],	m0
		movq	m0,	[r9 + r6 + 1 + 8]
		movq	[r0 + r10 + 8],	m0
	%elif %1 == 32
		movq	m0,	[r9 + r6 + 1]
		movq	[r0 + r10 + 0],	m0
		movq	m0,	[r9 + r6 + 1 + 8]
		movq	[r0 + r10 + 8],	m0
		movq	m0,	[r9 + r6 + 1 + 16]
		movq	[r0 + r10 + 16],	m0
		movq	m0,	[r9 + r6 + 1 + 24]
		movq	[r0 + r10 + 24],m0
	%endif
	
	inc		r8
	cmp		r8,	%1
	jne		.assign_ref_body_%1
%else ;PRED_MODE_GE18 == 0
	%assign	%%LoopIndex	0
	%define	LoopNum		%1
	%rep LoopNum
		mov		r6,	%%LoopIndex
		inc		r6
		imul	r6,	r7		;r7 = angle
		mov		r10,r6
		sar		r6,	5		;idx
		and		r10,31		;fact
		
		imul	r10, 257
		sub		r10b,32
		neg		r10b
		movd	m6,	r10
		pshuflw	m6, m6, 0	;fact,32-fact,fact,32-fact,fact,32-fact,fact,32-fact...
		
		%if %%LoopIndex	== 0
			lea	r11,	[rsp + 4*%1 + 32]
			and	r11,	~0x0F
		%endif
		
		%if %1 == 4
			movq	m0,	[r9 + r6 + 1]
			movq	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4
			
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			%if %%LoopIndex == 0
				mova	m2,	m0
			%elif %%LoopIndex == 1
				mova	m3,	m0
			%elif %%LoopIndex == 2
				mova	m4,	m0
			%elif %%LoopIndex == 3
				mova	m5,	m0
			%endif
		
		%elif %1 == 8
			
			movq	m0,	[r9 + r6 + 1]
			movq	m2,	[r9 + r6 + 9]
			punpcklqdq	m0, m2
			mova	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1	;a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
			
			pshufd		m6,	m6, 0
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			%if %%LoopIndex == 0
				mova	m3,	m0
			%elif %%LoopIndex == 1
				mova	m4,	m0
			%elif %%LoopIndex == 2
				mova	m5,	m0
			%elif %%LoopIndex == 7
				mova	m7,	m0
			%else
				movq	[r11 + %1 * %%LoopIndex],	m0
			%endif
			
		%elif %1 == 16
			movq	m0,	[r9 + r6 + 1]
			movq	m4,	[r9 + r6 + 9]
			punpcklqdq	m0, m4
			mova	m1,	m0
			palignr	m1,	m0,	1
			punpcklbw	m0,	m1				;m0 = a0 a1 a1 a2 a2 a3 a3 a4 a4 a5 a5 a6 a6 a7 a7 a8
			
			movq	m2,	[r9 + r6 + 17]
			punpcklqdq	m4, m2
			mova	m2,	m4
			palignr	m4,	m2,	1
			punpcklbw	m2,	m4				;m2 = a8 a9 a9 a10 a10 a11 a11 a12 a12 a13 a13 a14 a14 a15 a15 a16
			
			pshufd		m6,	m6, 0
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
			
			pmaddubsw	m2,	m6
			paddw		m2,	m7
			psraw		m2,	5
			packuswb	m2,	m2
			
			punpcklqdq	m0,	m2
			mova	[r11 + %1 * %%LoopIndex],		m0
			
		%elif %1 == 32
			
			movq	m0,	[r9 + r6 + 1]
			movq	m1,	[r9 + r6 + 9]
			punpcklqdq	m0, m1
			mova	m2,	m0
			palignr	m2,	m0,	1
			punpcklbw	m0,	m2				;m0 = part 0
			
			movq	m2,	[r9 + r6 + 17]
			punpcklqdq	m1, m2
			mova	m3,	m1
			palignr	m3,	m1,	1
			punpcklbw	m1,	m3				;m1 = part 1
			
			movq	m3,	[r9 + r6 + 25]
			punpcklqdq	m2, m3
			mova	m4,	m2
			palignr	m4,	m2,	1
			punpcklbw	m2,	m4				;m2 = part 2
			
			movq	m4,	[r9 + r6 + 33]
			punpcklqdq	m3, m4
			mova	m5,	m3
			palignr	m5,	m3,	1
			punpcklbw	m3,	m5				;m3 = part 2
			
			pshufd		m6,	m6, 0
			
			pmaddubsw	m0,	m6
			paddw		m0,	m7
			psraw		m0,	5
			packuswb	m0,	m0
		
			pmaddubsw	m1,	m6
			paddw		m1,	m7
			psraw		m1,	5
			packuswb	m1,	m1
			
			pmaddubsw	m2,	m6
			paddw		m2,	m7
			psraw		m2,	5
			packuswb	m2,	m2
			
			pmaddubsw	m3,	m6
			paddw		m3,	m7
			psraw		m3,	5
			packuswb	m3,	m3
			
			punpcklqdq	m0,	m1
			punpcklqdq	m2,	m3
			
			mova	[r11 + %1 * %%LoopIndex +  0],	m0
			mova	[r11 + %1 * %%LoopIndex + 16],	m2
			
		%endif
		%assign	%%LoopIndex	%%LoopIndex + 1
	%endrep
	
	;r3 = dest_stride
	%if %1 == 4
		;m2,m3,m4,m5
		punpcklbw	m2,	m3	;a0 b0 a1 b1 a2 b2 a3 b3
		punpcklbw	m4, m5	;c0 d0 c1 d1 c2 d2 c3 d3
		punpcklwd	m2,	m4	;a0 b0 c0 d0 a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3
		
		movd	[r0 + 0*r3],	m2
		palignr	m2,	m2, 4
		movd	[r0 + 1*r3],	m2
		palignr	m2,	m2, 4
		movd	[r0 + 2*r3],	m2
		palignr	m2,	m2, 4
		lea		r3,	[3*r3]
		movd	[r0 + r3],	m2
		
	%elif %1 == 8
		;m3,m4,m5,m7
		;0, 1, 2, 7
			
		movq	m2,	[r11 + 3*8]	;d
		movq	m0,	[r11 + 4*8]	;e
		movq	m1,	[r11 + 5*8]	;f
		
		punpcklbw m3, m4	;a0 b0 a1 b1 a2 b2 a3 b3 a4 b4 a5 b5 a6 b6 a7 b7
		punpcklbw m5, m2	;c0 d0 c1 d1 c2 d2 c3 d3 c4 d4 c5 d5 c6 d6 c7 d7
		punpcklbw m0, m1	;e0 f0 e1 f1 e2 f2 e3 f3 e4 f4 e5 f5 e6 f6 e7 f7
		movq	m1,	[r11 + 6*8]	;g
		punpcklbw m1, m7	;g0 h0 g1 h1 g2 h2 g3 h3 g4 h4 g5 h5 g6 h6 g7 h7
		
		;m2,m4,m6,m7
		mova	m2,	m3
		punpcklwd	m2,	m5	;a0 b0 c0 d0 a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3
		punpckhwd	m3,	m5	;a4 b4 c4 d4 a5 b5 c5 d5 a6 b6 c6 d6 a7 b7 c7 d7
		mova	m4,	m0
		punpcklwd	m0,	m1	;e0 f0 g0 h0 e1 f1 g1 h1 e2 f2 g2 h2 e3 f3 g3 h3
		punpckhwd	m4,	m1	;e4 f4 g4 h4 e5 f5 g5 h5 e6 f6 g6 h6 e7 f7 g7 h7
		
		;m1,m5,m6,m7
		mova m1, m2
		punpckldq	m1,	m0	;a0 b0 c0 d0 e0 f0 g0 h0 a1 b1 c1 d1 e1 f1 g1 h1
		punpckhdq	m2, m0	;a2 b2 c2 d2 e2 f2 g2 h2 a3 b3 c3 d3 e3 f3 g3 h3
		mova m0, m3
		punpckldq	m0,	m4	;a4 b4 c4 d4 e4 f4 g4 h4 a5 b5 c5 d5 e5 f5 g5 h5
		punpckhdq	m3, m4	;a6 b6 c6 d6 e6 f6 g6 h6 a7 b7 c7 d7 e7 f7 g7 h7
		
		lea		r10, [3*r3]
		
		movq	[r0 + 0*r3],	m1
		movhps	[r0 + 1*r3],	m1
		movq	[r0 + 2*r3],	m2
		movhps	[r0 + r10],		m2
		
		movq	[r0 + 4*r3],	m0
		lea		r10,[r10 + 2*r3]
		movhps	[r0 + r10],	m0
		lea		r10,[r10 + 1*r3]
		movq	[r0 + r10],	m3
		lea		r10,[r10 + 1*r3]
		movhps	[r0 + r10],	m3
		
	%elif %1 == 16
		;m3,m4,m5,m6,m7
		
		%define BLOCK_NUM	2
		%assign	%%block_x	0
		%assign	%%block_y	0
		
		mov		r6,	r0
		
		%rep BLOCK_NUM
			%rep BLOCK_NUM

				%define	s_buf		r11 + 16*%%block_x*8 + 8*%%block_y
				%define s_stride	16
				%define d_buf		r6 + 8*%%block_x
				%define	d_stride	r3
				%define	gpr_tmp		r10
				
				Intra_PreAngle_Mode_LT8_Ref_Assign s_buf, s_stride, d_buf, d_stride, gpr_tmp
				
				%assign	%%block_x %%block_x + 1
			%endrep
			
			lea		r6,	[r6 + 8*r3]
			%assign	%%block_x	0
			%assign	%%block_y %%block_y + 1
		%endrep
		
	%elif %1 == 32
		
		%define BLOCK_NUM	4
		%assign	%%block_x	0
		%assign	%%block_y	0
		
		mov		r6,	r0
		
		%rep BLOCK_NUM
			%rep BLOCK_NUM

				%define	s_buf		r11 + 32*%%block_x*8 + 8*%%block_y
				%define s_stride	32
				%define d_buf		r6 + 8*%%block_x
				%define	d_stride	r3
				%define	gpr_tmp		r10
				
				Intra_PreAngle_Mode_LT8_Ref_Assign s_buf, s_stride, d_buf, d_stride, gpr_tmp
				
				%assign	%%block_x %%block_x + 1
			%endrep
			
			lea		r6,	[r6 + 8*r3]
			%assign	%%block_x	0
			%assign	%%block_y %%block_y + 1
		%endrep
		
	%endif
%endif

.special_edge_%1:
	
	%if %1 < 32
		;mov	r4d, dword [rsp + rsp_offset + arg_pos(4)]		;cIdx
		;mov	r5d, dword [rsp + rsp_offset + arg_pos(5)]		;mode	
		cmp	r4,	0
		jne	.done_end_%1
		%if PRED_MODE_GE18 == 1
			cmp	r5,	26
		%else
			cmp	r5,	10
		%endif
		jne	.done_end_%1
		
		%if PRED_MODE_GE_18
			lea	r5,	[r2 - 1]
			mov	r4b, byte  [r1]							;top[0]
			mov	r5d, dword [r5]							;left[-1]
			and r5,	0xFF
		%else
			lea	r5,	[r1 - 1]
			mov	r4b, byte  [r2]							;left[0]
			mov	r5d, dword [r5]							;top[-1]
			and r5,	0xFF
		%endif
		
		movd	m0,	r4
		movd	m1,	r5
		pshuflw	m0, m0, 0							;top[0]
		pshuflw	m1, m1, 0							;left[-1]
		
		pxor	m7,	m7
		mov		r6,	r3				;r3 = dst_stride
		lea	r5,	[3*r3]
		
		%if PRED_MODE_GE_18
			%if	  %1 == 4
				
				movd	m2,	[r2]		;left[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(top[0] + ((left[x] - left[-1]) >> 1))
				
				movd	r1d, m2
				mov		byte [r0 + 0*r6], r1b
				mov		byte [r0 + 1*r6], r1h
				shr		r1, 16
				mov		byte [r0 + 2*r6], r1b
				lea		r0,	[r0 + r5]
				mov		byte [r0], r1h		;error: cannot use A/B/C/DH with instruction needing REX
				
			%elif %1 == 8
			
				Intra_Mode26_cIdx0_nTbsLT32_assign r2, r0, r6, 8, m0, m1, r5
				
			%elif %1 == 16
				
				Intra_Mode26_cIdx0_nTbsLT32_assign r2, r0, r6, 16, m0, m1, r5
				
			%endif
		%else
			%if	  %1 == 4
				
				movd	m2,	[r1]		;top[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movd	[r0], m2
				
			%elif %1 == 8
				
				punpcklwd	m0,	m0
				punpcklwd	m1,	m1
				
				movq	m2,	[r1]		;top[x]
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0], m2
				
			%elif %1 == 16
				
				punpcklwd	m0,	m0
				punpcklwd	m1,	m1
				
				movq	m2,	[r1]		;top[x]
				movq	m3,	[r1 + 8]	;top[x + 8]
				
				punpcklbw	m2, m7
				psubw	m2,	m1
				psraw	m2, 1
				paddw	m2,	m0			
				packuswb	m2, m2		;m2 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0], m2
				
				punpcklbw	m3, m7
				psubw	m3,	m1
				psraw	m3, 1
				paddw	m3,	m0			
				packuswb	m3, m3		;m3 = Clip(left[0] + ((top[x] - top[-1]) >> 1))
				movq	[r0 + 8], m3
				
			%endif
		%endif
	%endif
	
.done_end_%1:
	add		rsp,	stack_room
	
%endmacro
	
;VO_VOID  H265_IntraPred_Angular_mode_GE18_ASM(
	;VO_U8 *p_cur, 
	;VO_U8 *top, 
	;VO_U8 *left,
    ;VO_S32 dst_stride, 
	;VO_S32 cIdx, 
	;VO_S32 predModeIntra)

;input	%1 = nTbs
;		%2 = PREMODE GE18	1:GE18	0:LT18

%macro H265_IntraPred_Angular_ASM	2

cglobal	H265_IntraPred_Angular_%1_8_%2_X86
	%define	stack_room	4*%1 + %1 * %1 * 2 + 32
%if ARCH_X86_64	== 1
%if	WIN64 == 1
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 5 + 8
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	
	mov		r4,	[rsp + rsp_offset + arg_pos(4)]
	mov		r5,	[rsp + rsp_offset + arg_pos(5)]
	movsxd	r4,	r4d
	movsxd	r5,	r5d
	
	sub		rsp,	stack_room
	%define	rsp_offset	0x08 * 5 + 8 + stack_room
%else
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 3 + 8
	push	r9
	push	r10
	push	r11
	sub		rsp,	stack_room
	%define	rsp_offset	0x08 * 3 + 8 + stack_room
%endif
%else
	%define	arg_pos(n)	0x04 * n
	%define	rsp_offset	0x04 * 4 + 4
	
	push	r6
	push	r5
	push	r4
	push	r3
	
	;mov	r0,	[rsp + rsp_offset + arg_pos(0)]		;p_cur
	mov	r1,	[rsp + rsp_offset + arg_pos(1)]			;top
	mov	r2,	[rsp + rsp_offset + arg_pos(2)]			;left
	;mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
	;mov	r4,	[rsp + rsp_offset + arg_pos(4)]		;cIdx
	mov	r0,	[rsp + rsp_offset + arg_pos(5)]			;predModeIntra
	
	sub		rsp,	stack_room
	%define	rsp_offset	0x04 * 4 + 4 + stack_room
	
%endif
	
	%define	TRANS_BLOCK_SIZE	%1
	%define	PRED_MODE_GE_18		1
	%ifidn	%2, LT18
		%define	PRED_MODE_GE_18	0
	%endif
	
%if ARCH_X86_64	== 1
	H265_IntraPred_Angular_body_x64 TRANS_BLOCK_SIZE, PRED_MODE_GE_18
%else
	H265_IntraPred_Angular_body_x86 TRANS_BLOCK_SIZE, PRED_MODE_GE_18
%endif
	
%if ARCH_X86_64	== 1
%if WIN64 == 1
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	ret
%else
	pop	r11
	pop	r10
	pop	r9
	ret
%endif
%else
	pop	r3
	pop	r4
	pop	r5
	pop	r6
	ret
%endif

%endmacro

INIT_XMM	sse2

H265_IntraPred_Angular_ASM	 4,	GE18
H265_IntraPred_Angular_ASM	 8,	GE18
H265_IntraPred_Angular_ASM	16,	GE18
H265_IntraPred_Angular_ASM	32,	GE18

H265_IntraPred_Angular_ASM	 4,	LT18
H265_IntraPred_Angular_ASM	 8,	LT18
H265_IntraPred_Angular_ASM	16,	LT18
H265_IntraPred_Angular_ASM	32,	LT18


;void H265_Intra_64Pix_MultiAdd(VO_U8 * dest_buf, VO_U8 leftMost, VO_U8 rightMost)
;Important Discovery about pmaddubsw:
;set	xmm0 = 80808080808080808080808080808080 
;		xmm2 = 08380739063A053B043C033D023E013F 
;then   pmaddubsw xmm2, xmm0
;the xmm2 will be as below:
;		xmm2 = E000E000E000E000E000E000E000E000
;however if pmaddubsw is executed as below:
;		pmaddubsw xmm0, xmm2
;the result will be saved to xmm0 , xmm0 is as below:
;		xmm0 = 20002000200020002000200020002000
;how to expain about this case?

%macro Intra_64Pix_MultiAdd 0
cglobal H265_Intra_64Pix_MultiAdd_X86
%if ARCH_X86_64 == 1
	
%else
	push	r3
	mov		r0,	[rsp +  8]
	mov		r1,	[rsp + 12]
	mov		r2,	[rsp + 16]
%endif
    
    %define Dest_Pos(LineNo, LineOffset)	r0 + 16 * LineNo + LineOffset
    and		r1,	0xFF
    and		r2,	0xFF
    mov		byte [r0 - 1],r1b
    cmp		r1,	r2
    je		.LeftMost_Equal_RigntMost
    
    shl		r2,	8
    add		r2,	r1
    movd	m0,	r2
    pshuflw	m0,	m0,	0
    punpcklwd	m0,	m0
    mova	m7,	[CONST_U8_64]			;m7 = 64 byte
    mova	m6,	[CONST_U16_POS32]		;m6 = 32 word
    
	%assign	%%nIndex	0
    %define LoopNum 4
    %rep LoopNum
		
		mova	m1,	[CONST_U8_1_64 + 16 * %%nIndex]
		mova	m2,	m7
		psubb	m2,	m1
		mova	m3,	m2
		punpcklbw	m2,	m1
		punpckhbw	m3,	m1
		mova	m4,	m0
		mova	m5,	m0
		pmaddubsw	m4,	m2
		pmaddubsw	m5,	m3
		paddw	m4,	m6
		paddw	m5,	m6
		psraw	m4,	6
		psraw	m5,	6
		packuswb	m4,	m4
		packuswb	m5,	m5
		movq	[Dest_Pos(%%nIndex,0)],	m4
		movhps	[Dest_Pos(%%nIndex,8)],	m5
		
		%assign	%%nIndex	%%nIndex + 1
	%endrep
	
	jmp	.done_end
	
.LeftMost_Equal_RigntMost:
		shl		r1,	6
		add		r1,	32
		shr		r1,	6
		movd	m4,	r1
		pshuflw	m4,	m4, 0
		punpcklqdq	m4,	m4
		packuswb	m4,	m4
		
		movq	[Dest_Pos(0,0)],	m4
		movq	[Dest_Pos(0,8)],	m4
		
		movq	[Dest_Pos(1,0)],	m4
		movq	[Dest_Pos(1,8)],	m4
		
		movq	[Dest_Pos(2,0)],	m4
		movq	[Dest_Pos(2,8)],	m4
		
		movq	[Dest_Pos(3,0)],	m4
		movq	[Dest_Pos(3,8)],	m4

.done_end:
	
%if ARCH_X86_64 == 1
	ret
%else
	pop	r3
	ret
%endif

%endmacro


INIT_XMM	sse2
Intra_64Pix_MultiAdd

;input	%1 = dest_buf
;		%2 = src_buf
%macro Filter_Left_Top	2
	
	%define dest_buf %1
	%define src_buf	 %2
	
	movq	m0,	[src_buf - 1]
	movq	m1,	[src_buf + 7]
	punpcklqdq	m0,	m1
	mova	m1,	m0
	mova	m2,	m0
	palignr	m1,	m1,	1
	palignr	m2,	m2,	2
	
	;m0,m1,m2
	;		(a + 2*b + c + 2) >> 2
	;	=	((a + c)>> 1 + b + 1) >> 1
	;	=	((a + c + 1)>> 1 - (a + c) & 1 + b + 1) >> 1
	mova	m3,	m0
	paddb	m3,	m2	;m3 = a + c
	pand	m3,	m7	;m3 = (a + c) & 1
	pavgb	m0,	m2	;m0 = (a + c + 1) >> 1
	psubb	m0,	m3	;m0 = (a + c + 1) >> 1 - (a + c) & 1
	pavgb	m0,	m1	;m0 = ((a + c + 1)>> 1 - (a + c) & 1 + b + 1) >> 1
	movq	[dest_buf],	m0
	
%endmacro
;void H265_Intra_Filter_Left_Top_X86_sse2(VO_U8* dest_buf1 , VO_U8* dest_buf2 ,VO_U8* src_buf1, VO_U8* src_buf2,VO_S32 nTbs)
%macro Intra_Filter_Left_Top 0
cglobal H265_Intra_Filter_Left_Top_X86
	
%if ARCH_X86_64 == 1
	%define gpr_size	8
	%define rsp_off(num_reg_push)	num_reg_push * gpr_size + gpr_size
	%define arg_rsp_pos(x)	rsp + rsp_off(0) + gpr_size * x
	
%if WIN64 == 1
	mov	r4,	[arg_rsp_pos(4)]
	movsxd	r4,	r4d
%else
	
%endif
%else
	%define gpr_size	4
	%define rsp_off(num_reg_push)	num_reg_push * gpr_size + gpr_size
	%define arg_rsp_pos(x)	rsp + rsp_off(3) + gpr_size * x
	
	push	r3
	push	r4
	push	r5
	
	mov		r0,	[arg_rsp_pos(0)]		;dest_buf1
	mov		r1,	[arg_rsp_pos(1)]		;dest_buf2
	mov		r2,	[arg_rsp_pos(2)]		;src_buf1
	mov		r3,	[arg_rsp_pos(3)]		;src_buf2
	mov		r4,	[arg_rsp_pos(4)]		;nTbs
%endif
    
    %define dest_buf1	r0
    %define dest_buf2	r1
    %define src_buf1	r2
    %define src_buf2	r3
    %define nTbs		r4
	
	mova	m7,	[CONST_U8_1]
	
	cmp	r4,	4
	je	.nTbS_4
	cmp	r4,	8
	je	.nTbS_8
	cmp	r4,	16
	je	.nTbS_16
	cmp	r4,	32
	je	.nTbS_32
	
	
.nTbS_4:
	
	Filter_Left_Top	dest_buf1,	src_buf1
	Filter_Left_Top	dest_buf2,	src_buf2
	
	%define nTb_Size	4
	mov	r4,	r1	;r4 = dest_buf2
	mov	r1b, byte [src_buf1 + 2*nTb_Size - 1]
	mov byte [dest_buf1 + 2*nTb_Size - 1], r1b 
	
	%define dest_buf2	r4
	
	mov	r1b, byte [src_buf2 + 2*nTb_Size - 1]
	mov byte [dest_buf2 + 2*nTb_Size - 1], r1b 
	
	mov r3b, byte [src_buf2]
	and	r3,	0xFF
	mov	r5,	r3
	mov	r3b, byte [src_buf1 - 1]
	and	r3,	0xFF
	shl	r3, 1
	add	r5,	r3
	mov	r3b, byte [src_buf1]
	and	r3,	0xFF
	add	r5,	r3
	add	r5,	2
	sar	r5,	2
	mov	r3,	r5
	
	mov	byte [dest_buf1 - 1],r3b
	mov	byte [dest_buf2 - 1],r3b
	
	jmp	.done_end_filter
.nTbS_8:
	%define dest_buf2	r1
	%assign %%Loop_Index 0
	%define Loop_Num 2
	%rep Loop_Num
		Filter_Left_Top	dest_buf1 + 8*%%Loop_Index,	src_buf1 + 8*%%Loop_Index
		Filter_Left_Top	dest_buf2 + 8*%%Loop_Index,	src_buf2 + 8*%%Loop_Index
		%assign %%Loop_Index %%Loop_Index + 1
	%endrep
	
	%define nTb_Size	8
	mov	r4,	r1	;r4 = dest_buf2
	mov	r1b, byte [src_buf1 + 2*nTb_Size - 1]
	mov byte [dest_buf1 + 2*nTb_Size - 1], r1b 
	
	%define dest_buf2	r4
	
	mov	r1b, byte [src_buf2 + 2*nTb_Size - 1]
	mov byte [dest_buf2 + 2*nTb_Size - 1], r1b 
	
	mov r3b, byte [src_buf2]
	and	r3,	0xFF
	mov	r5,	r3
	mov	r3b, byte [src_buf1 - 1]
	and	r3,	0xFF
	shl	r3, 1
	add	r5,	r3
	mov	r3b, byte [src_buf1]
	and	r3,	0xFF
	add	r5,	r3
	add	r5,	2
	sar	r5,	2
	mov	r3,	r5
	
	mov	byte [dest_buf1 - 1],r3b
	mov	byte [dest_buf2 - 1],r3b
	
	jmp	.done_end_filter
.nTbS_16
	%define dest_buf2	r1
	%assign %%Loop_Index 0
	%define Loop_Num 4
	%rep Loop_Num
		Filter_Left_Top	dest_buf1 + 8*%%Loop_Index,	src_buf1 + 8*%%Loop_Index
		Filter_Left_Top	dest_buf2 + 8*%%Loop_Index,	src_buf2 + 8*%%Loop_Index
		%assign %%Loop_Index %%Loop_Index + 1
	%endrep
	
	%define nTb_Size	16
	mov	r4,	r1	;r4 = dest_buf2
	mov	r1b, byte [src_buf1 + 2*nTb_Size - 1]
	mov byte [dest_buf1 + 2*nTb_Size - 1], r1b 
	
	%define dest_buf2	r4
	
	mov	r1b, byte [src_buf2 + 2*nTb_Size - 1]
	mov byte [dest_buf2 + 2*nTb_Size - 1], r1b 
	
	mov r3b, byte [src_buf2]
	and	r3,	0xFF
	mov	r5,	r3
	mov	r3b, byte [src_buf1 - 1]
	and	r3,	0xFF
	shl	r3, 1
	add	r5,	r3
	mov	r3b, byte [src_buf1]
	and	r3,	0xFF
	add	r5,	r3
	add	r5,	2
	sar	r5,	2
	mov	r3,	r5
	
	mov	byte [dest_buf1 - 1],r3b
	mov	byte [dest_buf2 - 1],r3b
	
	jmp	.done_end_filter
.nTbS_32
	%define dest_buf2	r1
	%assign %%Loop_Index 0
	%define Loop_Num 8
	%rep Loop_Num
		Filter_Left_Top	dest_buf1 + 8*%%Loop_Index,	src_buf1 + 8*%%Loop_Index
		Filter_Left_Top	dest_buf2 + 8*%%Loop_Index,	src_buf2 + 8*%%Loop_Index
		%assign %%Loop_Index %%Loop_Index + 1
	%endrep
	
	%define nTb_Size	32
	mov	r4,	r1	;r4 = dest_buf2
	mov	r1b, byte [src_buf1 + 2*nTb_Size - 1]
	mov byte [dest_buf1 + 2*nTb_Size - 1], r1b 
	
	%define dest_buf2	r4
	
	mov	r1b, byte [src_buf2 + 2*nTb_Size - 1]
	mov byte [dest_buf2 + 2*nTb_Size - 1], r1b 
	
	mov r3b, byte [src_buf2]
	and	r3,	0xFF
	mov	r5,	r3
	mov	r3b, byte [src_buf1 - 1]
	and	r3,	0xFF
	shl	r3, 1
	add	r5,	r3
	mov	r3b, byte [src_buf1]
	and	r3,	0xFF
	add	r5,	r3
	add	r5,	2
	sar	r5,	2
	mov	r3,	r5
	
	mov	byte [dest_buf1 - 1],r3b
	mov	byte [dest_buf2 - 1],r3b
	
.done_end_filter:
	
%if ARCH_X86_64 == 1
	ret
%else
	pop	r5
	pop	r4
	pop	r3
	ret
%endif

%endmacro


INIT_XMM	sse2
Intra_Filter_Left_Top


;void SetMem_X86_mmx(VO_U8* pDest, VO_U8 value, VO_S32 nTbs)
INIT_MMX mmx
cglobal SetMem_X86
%if ARCH_X86_64 == 1

%else
	mov	r0,	[rsp +  4]
	mov	r1,	[rsp +  8]
	mov	r2,	[rsp + 12]
%endif
	and		r1,	0xFF
	movd	m0,	r1
	pshufw	m0,	m0, 0
	packuswb	m0,	m0
	
	cmp	r2,	4
	je	.branch_4
	cmp	r2,	8
	je	.branch_8
	cmp	r2,	16
	je	.branch_16
	cmp	r2,	32
	je	.branch_32
	cmp	r2,	64
	je	.branch_64
	
.branch_4:
	movd	[r0], m0
	jmp	.done_end
.branch_8:
	movq	[r0], m0
	jmp	.done_end
.branch_16:
	movq	[r0], m0
	movq	[r0 + 8], m0
	jmp	.done_end
.branch_32:
	movq	[r0], m0
	movq	[r0 +  8], m0
	movq	[r0 + 16], m0
	movq	[r0 + 24], m0
	jmp	.done_end
.branch_64:
	movq	[r0], m0
	movq	[r0 +  8], m0
	movq	[r0 + 16], m0
	movq	[r0 + 24], m0
	movq	[r0 + 32], m0
	movq	[r0 + 40], m0
	movq	[r0 + 48], m0
	movq	[r0 + 56], m0
	
.done_end:

%if ARCH_X86_64 == 1
	ret
%else
	ret
%endif
	


;void H265_IntraPred_DC_X86_sse2( 
		;VO_U8 *p_ref, 
		;const VO_U8 *top, 
		;const VO_U8 *left,
		;VO_S32 pic_stride, 
		;VO_S32 log2TrafoSize, 
		;VO_S32 cIdx )

%macro IntraPred_DC	0
cglobal	H265_IntraPred_DC_X86
	%define	stack_room	0
%if ARCH_X86_64	== 1
%if	WIN64 == 1
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 3 + 8
	push	r7
	push	r8
	push	r9
	mov		r4,	[rsp + rsp_offset + arg_pos(4)]
	mov		r5,	[rsp + rsp_offset + arg_pos(5)]
	movsxd	r4,	r4d
	movsxd	r5,	r5d
	mov		r8,	r1
	mov		r9,	r2
	%define	rsp_offset	0x08 * 3 + 8 + stack_room
%else
	push	r9
	mov		r8,	r1
	mov		r9,	r2
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 1 + 8
	%define	rsp_offset	0x08 * 1 + 8 + stack_room
%endif
%else
	%define	arg_pos(n)	0x04 * n
	%define	rsp_offset	0x04 * 4 + 4
	push	r6
	push	r5
	push	r4
	push	r3
	
	mov	r0,	[rsp + rsp_offset + arg_pos(0)]			;p_ref
	mov	r1,	[rsp + rsp_offset + arg_pos(1)]			;top
	mov	r2,	[rsp + rsp_offset + arg_pos(2)]			;left
	;mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
	mov	r4,	[rsp + rsp_offset + arg_pos(4)]			;log2TrafoSize
	;mov	r5,	[rsp + rsp_offset + arg_pos(5)]		;cIdx
	
	;sub		rsp,	stack_room
	%define	rsp_offset	0x04 * 4 + 4 + stack_room
	
%endif
	
	cmp	r4,	2
	je	.intra_dc_branch_04
	cmp	r4,	3
	je	.intra_dc_branch_08
	cmp	r4,	4
	je	.intra_dc_branch_16
	
.intra_dc_branch_32:
	
	mova	m4,	[CONST_U8_1]
	mova	m3,	[CONST_S16_POS1]
	xor		r4,	r4
	
	%assign	%%SecNo	0
	%define SecNum	4
	%rep SecNum
		movq	m0,	[r1 + 8*%%SecNo]		;top
		movq	m1,	[r2 + 8*%%SecNo]		;left
		
		%if %%SecNo == 0
			mova	m6,	m0
			mova	m7,	m1
		%endif
		
		punpcklbw	m0,	m1
		pmaddubsw	m0,	m4
		pmaddwd	m0,	m3
		mova		m2,	m0
		palignr		m2,	m0,	8
		paddd		m0,	m2
		mova		m2,	m0
		palignr		m2,	m0,	4
		paddd		m0,	m2
		%if ARCH_X86_64 == 1
			movd		r7d, m0
			add			r4d, r7d
		%else
			movd		r3d, m0
			add			r4d, r3d
		%endif
		
		%assign	%%SecNo	%%SecNo + 1
	%endrep

	add			r4d, 32
	shr			r4d, 6
	movd		m5,	r4d
	pshuflw		m5,	m5,	0
	punpcklqdq	m5, m5
	packuswb	m5,	m5							;dc
	
	%if ARCH_X86_64 == 0
		mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
		mov	r5,	[rsp + rsp_offset + arg_pos(5)]		;cIdx
	%endif
	
	lea	r6,	[3*r3]
	%assign %%LineNo	0
	%define LineNum		32
	%rep LineNum
		
		%assign	%%rmode %%LineNo % 3
		%if %%LineNo < 3
			%define r_dest	r0
		%elif %%LineNo == 3
			lea	r4,	[3*r3]
			lea	r4,	[r0 + r4]
			%define r_dest	r4
		%elif %%rmode == 0
			lea	r4,	[r6 + r4]
			%define r_dest	r4
		%endif
		
		movq	[r_dest + (%%LineNo % 3) * r3 + 0*8], m5
		movq	[r_dest + (%%LineNo % 3) * r3 + 1*8], m5
		movq	[r_dest + (%%LineNo % 3) * r3 + 2*8], m5
		movq	[r_dest + (%%LineNo % 3) * r3 + 3*8], m5
			
		%assign %%LineNo	%%LineNo + 1
	%endrep
	
	jmp	.done_end
.intra_dc_branch_04:
	movd	m0,	[r1]		;top
	movd	m1,	[r2]		;left
	mova	m6,	m0
	mova	m7,	m1
	
	mova	m4,	[CONST_U8_1]
	punpcklbw	m0,	m1
	pmaddubsw	m0,	m4
	pmaddwd	m0,	[CONST_S16_POS1]
	
	%if ARCH_X86_64 == 1
		movd	r7d, m0
		palignr	m0,	m0, 4
		movd	r4d, m0
		add		r4d, r7d
	%else
		movd	r3d, m0
		palignr	m0,	m0, 4
		movd	r4d, m0
		add		r4d, r3d
	%endif
	
	add		r4d, 4
	shr		r4d, 3
	movd	m5,	r4d
	pshuflw	m5,	m5,	0
	punpcklqdq	m5, m5
	packuswb	m5,	m5							;dc
	
	%if ARCH_X86_64 == 0
		mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
		mov	r5,	[rsp + rsp_offset + arg_pos(5)]		;cIdx
	%endif
	
	movd	[r0 + 0*r3], m5
	movd	[r0 + 1*r3], m5
	movd	[r0 + 2*r3], m5
	lea		r4,	[3*r3]
	lea		r4,	[r0 + r4]
	movd	[r4], m5
	
	cmp	r5,	0
	jne	.done_end
	
	;	(top + 3 * dc + 2) >> 2
	;=	((top + dc) + 2 * (dc + 1)) >> 2
	;=	((top + dc) >> 1 + dc + 1) >> 1
	;for (top + dc) >> 1
	;=	(top + dc + 1) >> 1 - (top + dc) & 1
	
	;m5,m6,m7 -> dc, top, left
	mova	m0,	m5
	paddb	m0,	m6	;m0 = dc + top
	pand	m0,	m4	;m0 = (dc + top) & 1
	pavgb	m6,	m5	;m6 = (top + dc + 1) >> 1
	psubb	m6,	m0	;m6 = (top + dc + 1) >> 1 - (top + dc) & 1 = (top + dc) >> 1
	pavgb	m6,	m5	;m6 = ((top + dc) >> 1 + dc + 1) >> 1
					;	= (top + 3 * dc + 2) >> 2
	movd	[r0],	m6
	
	mova	m0,	m5
	paddb	m0,	m7	;m0 = dc + left
	pand	m0,	m4	;m0 = (dc + left) & 1
	pavgb	m7,	m5	;m7 = (left + dc + 1) >> 1
	psubb	m7,	m0	;m7 = (left + dc + 1) >> 1 - (left + dc) & 1
	pavgb	m7,	m5	;m7 = ((left + dc + 1) >> 1 - (left + dc) & 1 +  + dc + 1) >> 1
					;	= (left + 3 * dc + 2) >> 2
	
	movd	r4,	m5
	and		r4,	0xFF	;r4 = dc
	inc		r4			;r4 = dc + 1
	shl		r4,	1		;r4 = 2*dc + 2
	
	mov		r1b	, byte [r1]
	and		r1,	0xFF
	mov		r2b	, byte [r2]
	and		r2,	0xFF
	
	add		r1,	r2
	add		r1,	r4
	shr		r1,	2		;r1 = (left[0] + 2 * dc  + top[0] + 2) >> 2
	
	mov		byte [r0], r1b
	
	movd	r1,	m7
	shr		r1,	8
	mov		byte [r0 + 1*r3], r1b
	lea		r0,	 [r0 + 2*r3]
	mov		byte [r0 + 0*r3], r1h
	shr		r1,	16
	mov		byte [r0 + 1*r3], r1b
	jmp	.done_end

.intra_dc_branch_08:

	mova	m4,	[CONST_U8_1]
	mova	m3,	[CONST_S16_POS1]
	xor		r4,	r4
	
	%assign	%%SecNo	0
	%define SecNum	1
	%rep SecNum
		movq	m0,	[r1 + 8*%%SecNo]		;top
		movq	m1,	[r2 + 8*%%SecNo]		;left
		
		%if %%SecNo == 0
			mova	m6,	m0
			mova	m7,	m1
		%endif
		
		punpcklbw	m0,	m1
		pmaddubsw	m0,	m4
		pmaddwd	m0,	m3
		mova		m2,	m0
		palignr		m2,	m0,	8
		paddd		m0,	m2
		mova		m2,	m0
		palignr		m2,	m0,	4
		paddd		m0,	m2
		
		%if ARCH_X86_64 == 1
			movd		r7d, m0
			add			r4d, r7d
		%else
			movd		r3d, m0
			add			r4d, r3d
		%endif
		
		%assign	%%SecNo	%%SecNo + 1
	%endrep
		
	add			r4d, 8
	shr			r4d, 4
	movd		m5,	r4d
	pshuflw		m5,	m5,	0
	punpcklqdq	m5, m5
	packuswb	m5,	m5						;dc
	
	%if ARCH_X86_64 == 0
		mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
		mov	r5,	[rsp + rsp_offset + arg_pos(5)]		;cIdx
	%endif
	
	lea	r6,	[3*r3]
	%assign %%LineNo	0
	%define LineNum		8
	%rep LineNum
		
		%assign	%%rmode %%LineNo % 3
		%if %%LineNo < 3
			%define r_dest	r0
		%elif %%LineNo == 3
			lea	r4,	[3*r3]
			lea	r4,	[r0 + r4]
			%define r_dest	r4
		%elif %%rmode == 0
			lea	r4,	[r6 + r4]
			%define r_dest	r4
		%endif
		
		movq	[r_dest + (%%LineNo % 3) * r3 + 0*8], m5
		%assign %%LineNo	%%LineNo + 1
	%endrep

	cmp	r5,	0
	jne	.done_end
	
	;m5,m6,m7 -> dc, top, left
	mov	r6,	r0
	%define dreg_0	r0
	%define dreg_1	r6
	%define m_dc	m5
	%define m_top	m6
	%define m_left	m7
		
	%assign	%%LoopIndex	0
	%define	LoopNum	1
	%rep LoopNum
		
		mova	m0,	m_dc
		paddb	m0,	m_top		;m0 = dc + top
		pand	m0,	m4			;m0 = (dc + top) & 1
		pavgb	m_top,	m_dc	;m_top = (top + dc + 1) >> 1
		psubb	m_top,	m0		;m_top = (top + dc + 1) >> 1 - (top + dc) & 1 = (top + dc) >> 1
		pavgb	m_top,	m_dc	;m_top = ((top + dc) >> 1 + dc + 1) >> 1
								;	   = (top + 3 * dc + 2) >> 2
		movq	[r0 + 8*%%LoopIndex],	m_top
		
		mova	m0,	m_dc
		paddb	m0,	m_left		;m0 = dc + left
		pand	m0,	m4			;m0 = (dc + left) & 1
		pavgb	m_left,	m_dc	;m_left = (left + dc + 1) >> 1
		psubb	m_left,	m0		;m_left = (left + dc + 1) >> 1 - (left + dc) & 1
		pavgb	m_left,	m_dc	;m_left = ((left + dc + 1) >> 1 - (left + dc) & 1 +  + dc + 1) >> 1
								;	= (left + 3 * dc + 2) >> 2
		%if %%LoopIndex	== 0
			movd	r4,	m_dc
			and		r4,	0xFF	;r4 = dc
			inc		r4			;r4 = dc + 1
			shl		r4,	1		;r4 = 2*dc + 2
			
			mov		r1b	, byte [r1]
			and		r1,	0xFF
			mov		r2b	, byte [r2]
			and		r2,	0xFF
			
			add		r1,	r2
			add		r1,	r4
			shr		r1,	2		;r1 = (left[0] + 2 * dc  + top[0] + 2) >> 2
			mov		byte [r0], r1b
			
			%if ARCH_X86_64 == 0
				mov	r4,	[rsp + rsp_offset + arg_pos(1)]			;top
				mov	r2,	[rsp + rsp_offset + arg_pos(2)]			;left
			%endif
		%endif

		%if ARCH_X86_64 == 1
			%define vo_gpr_size	8
			%define	mov_gpr	movq
		%else
			%define vo_gpr_size	4
			%define	mov_gpr	movd
		%endif
	
		%assign	%%ByteNo	0
		%define ByteNum		4
		%rep ByteNum
			
			%if ARCH_X86_64 == 1
				
				%if	%%ByteNo % vo_gpr_size == 0
					%if %%ByteNo > 0
						palignr	m_left,	m_left, vo_gpr_size
					%endif
					mov_gpr	r1,	m_left
				%else
					shr	r1, 8
				%endif
				
				%assign	%%P00  %%ByteNo + %%LoopIndex
				%if %%P00 != 0
					mov		byte [dreg_1 + 0 * r3], r1b
				%endif
				shr	r1,	8
				mov	byte [dreg_1 + 1 * r3], r1b
				
			%else
			
				%if	%%ByteNo % vo_gpr_size == 0
					%if %%ByteNo > 0
						palignr	m_left,	m_left, vo_gpr_size
					%endif
					mov_gpr	r1,	m_left
				%else
					shr	r1, 16
				%endif
				
				%assign	%%P00  %%ByteNo + %%LoopIndex
				%if %%P00 != 0
					mov		byte [dreg_1 + 0 * r3], r1b
				%endif
				mov	byte [dreg_1 + 1 * r3], r1h
				
			%endif
			
			lea		dreg_1, [dreg_1 + 2*r3]
			
			%assign	%%ByteNo	%%ByteNo + 2
		%endrep
		
		%assign	%%LoopIndex	%%LoopIndex + 1
		%if %%LoopIndex < LoopNum
			%if ARCH_X86_64 == 1
				movq	m_top,	[r8 + 8*%%LoopIndex]		;top
				movq	m_left,	[r9 + 8*%%LoopIndex]		;left
			%else
				movq	m_top,	[r4 + 8*%%LoopIndex]		;top
				movq	m_left,	[r2 + 8*%%LoopIndex]		;left
			%endif
		%endif
		
	%endrep
	
	jmp	.done_end

.intra_dc_branch_16:
	
	mova	m4,	[CONST_U8_1]
	mova	m3,	[CONST_S16_POS1]
	xor		r4,	r4
	
	%assign	%%SecNo	0
	%define SecNum	2
	%rep SecNum
		movq	m0,	[r1 + 8*%%SecNo]		;top
		movq	m1,	[r2 + 8*%%SecNo]		;left
		
		%if %%SecNo == 0
			mova	m6,	m0
			mova	m7,	m1
		%endif
		
		punpcklbw	m0,	m1
		pmaddubsw	m0,	m4
		pmaddwd	m0,	m3
		mova		m2,	m0
		palignr		m2,	m0,	8
		paddd		m0,	m2
		mova		m2,	m0
		palignr		m2,	m0,	4
		paddd		m0,	m2
		
		%if ARCH_X86_64 == 1
			movd		r7d, m0
			add			r4d, r7d
		%else
			movd		r3d, m0
			add			r4d, r3d
		%endif
		
		%assign	%%SecNo	%%SecNo + 1
	%endrep
		
	add			r4d, 16
	shr			r4d, 5
	movd		m5,	r4d
	pshuflw		m5,	m5,	0
	punpcklqdq	m5, m5
	packuswb	m5,	m5							;dc
	
	%if ARCH_X86_64 == 0
		mov	r3,	[rsp + rsp_offset + arg_pos(3)]		;dst_stride
		mov	r5,	[rsp + rsp_offset + arg_pos(5)]		;cIdx
	%endif
	
	lea	r6,	[3*r3]
	%assign %%LineNo	0
	%define LineNum		16
	%rep LineNum
		
		%assign	%%rmode %%LineNo % 3
		%if %%LineNo < 3
			%define r_dest	r0
		%elif %%LineNo == 3
			lea	r4,	[3*r3]
			lea	r4,	[r0 + r4]
			%define r_dest	r4
		%elif %%rmode == 0
			lea	r4,	[r6 + r4]
			%define r_dest	r4
		%endif
		
		movq	[r_dest + (%%LineNo % 3) * r3 + 0*8], m5
		movq	[r_dest + (%%LineNo % 3) * r3 + 1*8], m5
		%assign %%LineNo	%%LineNo + 1
	%endrep
	
	cmp	r5,	0
	jne	.done_end
	
	;m5,m6,m7 -> dc, top, left
	mov	r6,	r0
	%define dreg_0	r0
	%define dreg_1	r6
	%define m_dc	m5
	%define m_top	m6
	%define m_left	m7
		
	%assign	%%LoopIndex	0
	%define	LoopNum	2
	%rep LoopNum
		
		mova	m0,	m_dc
		paddb	m0,	m_top		;m0 = dc + top
		pand	m0,	m4			;m0 = (dc + top) & 1
		pavgb	m_top,	m_dc	;m_top = (top + dc + 1) >> 1
		psubb	m_top,	m0		;m_top = (top + dc + 1) >> 1 - (top + dc) & 1 = (top + dc) >> 1
		pavgb	m_top,	m_dc	;m_top = ((top + dc) >> 1 + dc + 1) >> 1
								;	   = (top + 3 * dc + 2) >> 2
		movq	[dreg_0 + 8*%%LoopIndex],	m_top
		
		mova	m0,	m_dc
		paddb	m0,	m_left		;m0 = dc + left
		pand	m0,	m4			;m0 = (dc + left) & 1
		pavgb	m_left,	m_dc	;m_left = (left + dc + 1) >> 1
		psubb	m_left,	m0		;m_left = (left + dc + 1) >> 1 - (left + dc) & 1
		pavgb	m_left,	m_dc	;m_left = ((left + dc + 1) >> 1 - (left + dc) & 1 +  + dc + 1) >> 1
								;	= (left + 3 * dc + 2) >> 2
		%if %%LoopIndex	== 0
			movd	r4,	m_dc
			and		r4,	0xFF	;r4 = dc
			inc		r4			;r4 = dc + 1
			shl		r4,	1		;r4 = 2*dc + 2
			
			mov		r1b	, byte [r1]
			and		r1,	0xFF
			mov		r2b	, byte [r2]
			and		r2,	0xFF
			
			add		r1,	r2
			add		r1,	r4
			shr		r1,	2		;r1 = (left[0] + 2 * dc  + top[0] + 2) >> 2
			mov		byte [dreg_0], r1b
			
			%if ARCH_X86_64 == 0
				mov	r4,	[rsp + rsp_offset + arg_pos(1)]			;top
				mov	r2,	[rsp + rsp_offset + arg_pos(2)]			;left
			%endif
			
		%endif

		%if ARCH_X86_64 == 1
			%define vo_gpr_size	8
			%define	mov_gpr	movq
		%else
			%define vo_gpr_size	4
			%define	mov_gpr	movd
		%endif
		
		%assign	%%ByteNo	0
		%define ByteNum		4
		%rep ByteNum
			
			%if ARCH_X86_64 == 1
				
				%if	%%ByteNo % vo_gpr_size == 0
					%if %%ByteNo > 0
						palignr	m_left,	m_left, vo_gpr_size
					%endif
					mov_gpr	r1,	m_left
				%else
					shr	r1, 8
				%endif
				
				%assign	%%P00  %%ByteNo + %%LoopIndex
				%if %%P00 != 0
					mov		byte [dreg_1 + 0 * r3], r1b
				%endif
				shr	r1,	8
				mov	byte [dreg_1 + 1 * r3], r1b
				
			%else
			
				%if	%%ByteNo % vo_gpr_size == 0
					%if %%ByteNo > 0
						palignr	m_left,	m_left, vo_gpr_size
					%endif
					mov_gpr	r1,	m_left
				%else
					shr	r1, 16
				%endif
				
				%assign	%%P00  %%ByteNo + %%LoopIndex
				%if %%P00 != 0
					mov		byte [dreg_1 + 0 * r3], r1b
				%endif
				mov	byte [dreg_1 + 1 * r3], r1h
				
			%endif
			
			lea		dreg_1, [dreg_1 + 2*r3]
			
			%assign	%%ByteNo	%%ByteNo + 2
		%endrep
		
		%assign	%%LoopIndex	%%LoopIndex + 1
		%if %%LoopIndex < LoopNum
			%if ARCH_X86_64 == 1
				movq	m_top,	[r8 + 8*%%LoopIndex]		;top
				movq	m_left,	[r9 + 8*%%LoopIndex]		;left
			%else
				movq	m_top,	[r4 + 8*%%LoopIndex]		;top
				movq	m_left,	[r2 + 8*%%LoopIndex]		;left
			%endif
		%endif
		
	%endrep

.done_end:
	
%if ARCH_X86_64 == 1
%if WIN64 == 1
	pop	r9
	pop	r8
	pop	r7
	ret
%endif
	pop	r9
	ret
%else
	pop	r3
	pop	r4
	pop	r5
	pop	r6
	ret
%endif
%endmacro


INIT_XMM sse2
IntraPred_DC

;void H265_IntraPred_Planar_X86_sse2( VO_U8 *p_cur, const VO_U8 *top, const VO_U8 *left,VO_S32 stride, VO_S32 log2TrafoSize )
%macro IntraPred_Planar 0
cglobal H265_IntraPred_Planar_X86
%define	stack_room	0
%if ARCH_X86_64	== 1
%if	WIN64 == 1
	push	r7
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 1 + 8
	mov		r4,	[rsp + rsp_offset + arg_pos(4)]
	movsxd	r4,	r4d
	mov		r7,	r0
	%define	rsp_offset	0x08 * 1 + 8 + stack_room
%else
	%define	arg_pos(n)	0x08 * n
	%define	rsp_offset	0x08 * 0 + 8
	%define	rsp_offset	0x08 * 0 + 8 + stack_room
%endif
%else
	%define	arg_pos(n)	0x04 * n
	%define	rsp_offset	0x04 * 4 + 4
	push	r6
	push	r5
	push	r4
	push	r3
	
	mov	r0,	[rsp + rsp_offset + arg_pos(0)]			;p_cur
	mov	r1,	[rsp + rsp_offset + arg_pos(1)]			;top
	mov	r2,	[rsp + rsp_offset + arg_pos(2)]			;left
	mov	r3,	[rsp + rsp_offset + arg_pos(3)]			;dst_stride
	mov	r4,	[rsp + rsp_offset + arg_pos(4)]			;log2TrafoSize
	
	%define	rsp_offset	0x04 * 4 + 4 + stack_room
%endif
	
	cmp	r4,	2
	je	.intra_planar_branch_04
	cmp	r4,	3
	je	.intra_planar_branch_08
	cmp	r4,	4
	je	.intra_planar_branch_16
	
.intra_planar_branch_32:
	%define nTbs	32
	mova	m6,	[CONST_U16_POS32]
	mov		r5, [r1 + nTbs]		;r5b = top[nTbS]
	mov		r6, [r2 + nTbs]		;r6b = left[nTbS]
	and		r5,	0xFF
	and		r6,	0xFF
	
	%assign	%%HSecNo	0
	%define	HSecs		nTbs / 8
	%rep	HSecs
		
		movq	m0, [r1 + 8*%%HSecNo]						;m0 = top
		mova	m7,	[CONST_U8_Planar_nTbs32 + 16*%%HSecNo]	;
		%if ARCH_X86_64 == 1
			mov		r0,	r7
		%else
			mov		r0,	[rsp + rsp_offset + arg_pos(0)]			;p_cur
		%endif
		
		%assign	%%LineNo	0
		%define Lines		nTbs
		%rep Lines
			%if %%LineNo % 8 = 0
				movq	m1,	[r2 + %%LineNo]
			%else
				palignr	m1,	m1,	1
			%endif
			
			mova	m2,	m1
			movd	m3,	r5
			punpcklbw	m2,	m3
			pshuflw	m2,	m2,	0
			punpcklqdq	m2,	m2		;m2 = left[y] | top[ nTbS ]
			pmaddubsw	m2,	m7		;(( nTbS - 1 - x ) * left[ %%LineNo ]  + ( x + 1 ) * top[ nTbS ]
			
			mov		r4,	%%LineNo + 1
			shl		r4,	8
			add		r4,	nTbs - %%LineNo - 1
			movd	m3,	r4
			pshuflw	m3,	m3,	0
			punpcklqdq  m3,	m3		;m3 = (y + 1) | (nTbS - 1 - y)
			
			movq	m4,	m0			;m4 = top[x]
			movd	m5,	r6			;m5 = left[nTbS]
			pshuflw	m5,	m5,	0
			punpcklqdq m5,	m5
			packuswb	m5,	m5
			punpcklbw	m4,	m5
			pmaddubsw	m4,	m3		;(nTbS - 1 - y) * top[x] + (y + 1) * left[nTbS]
			paddw		m2,	m4
			paddw		m2,	m6
			psraw		m2,	6
			packuswb	m2,	m2
			
			%if %%LineNo < 3
				movq	[r0 + r3 * %%LineNo + 8*%%HSecNo], m2
			%elif %%LineNo % 3 == 0
				lea		r4,	[3 * r3]
				lea		r0, [r0 + r4]
				movq	[r0 + 8*%%HSecNo], m2
			%else
				movq	[r0 + r3 * (%%LineNo % 3) + 8 * %%HSecNo], m2
			%endif
			
			%assign %%LineNo %%LineNo + 1
		%endrep
		
		%assign	%%HSecNo	%%HSecNo + 1
	%endrep
	
	jmp	.end_done
	
.intra_planar_branch_04:
	%define nTbs	4
	movq	m0, [r1]	;m0 = top
	movq	m1, [r2]	;m1 = left
	movq	m7,	[CONST_U8_Planar_nTbs4]	;
	movq	m6,	[CONST_S16_POS4]
	
	mova	m2,	m0
	palignr	m2,	m2,	4
	movd	r5d,m2
	and		r5,	0xFF
	mova	m2,	m1
	palignr	m2,	m2,	4
	movd	r6d,m2
	and		r6,	0xFF
	
	%assign	%%LineNo	0
	%define Lines	4
	%rep Lines
		movd r4, m1
		%if %%LineNo > 0
			shr	r4, 8*%%LineNo
		%endif
		movd	m2, r4d
		movd	m3,	r5d
		punpcklbw	m2,	m3		;m2 = left[y] | top[ nTbS ]
		pshuflw	m2,	m2,	0
		pmaddubsw	m2,	m7		;(( nTbS - 1 - x ) * left[ %%LineNo ]  + ( x + 1 ) * top[ nTbS ]
		
		mova	m3,	m7
		%if %%LineNo > 0
			palignr	m3,	m3,	2*%%LineNo
		%endif
		pshuflw	m3,	m3,	0
		
		mova	m4,	m0
		movd	m5,	r6d
		pshuflw	m5,	m5, 0
		packuswb	m5,	m5
		punpcklbw	m4,	m5
		pmaddubsw	m4,	m3		;( nTbS - 1 - y ) * top[ x ] + ( y + 1 ) * left[ nTbS ]
		paddw		m2,	m4		
		
		paddw	m2,	m6
		psraw	m2,	3
		packuswb	m2,	m2
		
		%if %%LineNo == 3
			lea		r4,	[3*r3]
			movd	[r0 + r4], m2
		%else
			movd	[r0 + r3 * %%LineNo], m2
		%endif

		%assign	%%LineNo %%LineNo + 1
	%endrep
	
	jmp	.end_done

.intra_planar_branch_08:
	%define nTbs	8
	movq	m0, [r1]					;m0 = top
	movq	m1, [r2]					;m1 = left
	mova	m7,	[CONST_U8_Planar_nTbs8]	;
	mova	m6,	[CONST_U16_POS8]
	mov		r5, [r1 + nTbs]		;r5b = top[8]
	mov		r6, [r2 + nTbs]		;r6b = left[8]
	and		r5,	0xFF
	and		r6,	0xFF
	
	%assign	%%LineNo	0
	%define Lines		8
	%rep Lines
		mova	m2,	m1
		%if %%LineNo > 0
			palignr	m2,	m2,	%%LineNo
		%endif
		movd	m3,	r5
		punpcklbw	m2,	m3
		pshuflw	m2,	m2,	0
		punpcklqdq	m2,	m2		;m2 = left[y] | top[ nTbS ]
		pmaddubsw	m2,	m7		;(( nTbS - 1 - x ) * left[ %%LineNo ]  + ( x + 1 ) * top[ nTbS ]
		
		mova	m3,	m7
		%if %%LineNo > 0
			palignr	m3,	m3,	2*%%LineNo
		%endif
		pshuflw	m3,	m3,	0
		punpcklqdq  m3,	m3
		movq	m4,	m0			;m4 = top[x]
		movd	m5,	r6			;m5 = left[8]
		pshuflw	m5,	m5,	0
		punpcklqdq m5,	m5
		packuswb	m5,	m5
		punpcklbw	m4,	m5
		pmaddubsw	m4,	m3		;( nTbS - 1 - y ) * top[ x ] + ( y + 1 ) * left[ nTbS ]
		paddw		m2,	m4
		paddw		m2,	m6
		psraw		m2,	4
		packuswb	m2,	m2
		
		%if %%LineNo < 3
			movq	[r0 + r3 * %%LineNo], m2
		%elif %%LineNo % 3 == 0
			lea		r4, [3*r3]
			lea		r0,	[r0 + r4]
			movq	[r0], m2
		%else
			movq	[r0 + r3 * (%%LineNo % 3)], m2
		%endif

		%assign	%%LineNo %%LineNo + 1
	%endrep
	
	jmp	.end_done
	
.intra_planar_branch_16:
	%define nTbs	16
	mova	m6,	[CONST_U16_16]
	mov		r5, [r1 + nTbs]		;r5b = top[16]
	mov		r6, [r2 + nTbs]		;r6b = left[16]
	and		r5,	0xFF
	and		r6,	0xFF
	
	%assign	%%HSecNo	0
	%define	HSecs		nTbs / 8
	%rep	HSecs
		
		movq	m0, [r1 + 8*%%HSecNo]						;m0 = top
		mova	m7,	[CONST_U8_Planar_nTbs16 + 16*%%HSecNo]	;
		%if ARCH_X86_64 == 1
			mov		r0,	r7
		%else
			mov		r0,	[rsp + rsp_offset + arg_pos(0)]			;p_cur
		%endif
		
		%assign	%%LineNo	0
		%define Lines		16
		%rep Lines
			%if %%LineNo == 0
				movq	m1,	[r2 + 0]
			%elif %%LineNo == 8
				movq	m1,	[r2 + 8]
			%else
				palignr	m1,	m1,	1
			%endif
			
			mova	m2,	m1
			movd	m3,	r5
			punpcklbw	m2,	m3
			pshuflw	m2,	m2,	0
			punpcklqdq	m2,	m2		;m2 = left[y] | top[ nTbS ]
			pmaddubsw	m2,	m7		;(( nTbS - 1 - x ) * left[ %%LineNo ]  + ( x + 1 ) * top[ nTbS ]
			
			mov		r4,	%%LineNo + 1
			shl		r4,	8
			add		r4,	nTbs - %%LineNo - 1
			movd	m3,	r4
			pshuflw	m3,	m3,	0
			punpcklqdq  m3,	m3		;m3 = (y + 1) | (nTbS - 1 - y)
			
			movq	m4,	m0			;m4 = top[x]
			movd	m5,	r6			;m5 = left[16]
			pshuflw	m5,	m5,	0
			punpcklqdq m5,	m5
			packuswb	m5,	m5
			punpcklbw	m4,	m5
			pmaddubsw	m4,	m3		;( nTbS - 1 - y ) * top[ x ] + ( y + 1 ) * left[ nTbS ]
			paddw		m2,	m4
			paddw		m2,	m6
			psraw		m2,	5
			packuswb	m2,	m2
			
			%if %%LineNo < 3
				movq	[r0 + r3 * %%LineNo + 8*%%HSecNo], m2
			%elif %%LineNo % 3 == 0
				lea		r4,	[3 * r3]
				lea		r0, [r0 + r4]
				movq	[r0 + 8*%%HSecNo], m2
			%else
				movq	[r0 + r3 * (%%LineNo % 3) + 8 * %%HSecNo], m2
			%endif
			
			%assign	%%LineNo %%LineNo + 1
		
		%endrep
		
		%assign	%%HSecNo	%%HSecNo + 1
	%endrep

.end_done:

%if ARCH_X86_64 == 1
%if WIN64 == 1
	pop	r7
%else

%endif
	ret
%else
	pop	r3
	pop	r4
	pop	r5
	pop	r6
	ret
%endif
%endmacro

INIT_XMM sse2
IntraPred_Planar