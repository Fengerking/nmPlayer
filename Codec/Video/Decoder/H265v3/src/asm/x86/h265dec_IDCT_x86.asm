;/************************************************************************
; VisualOn Proprietary
; Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
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

	cextern	CONST_U16_POS83
	cextern CONST_U16_POS36
	cextern CONST_U16_POS64
	cextern CONST_U32_POS64
	cextern CONST_U16_POS_0X7FF
	cextern CONST_U16_POS_0X8FF
	cextern CONST_U16_POS_0X0800
	cextern CONST_U32_POS_0X0800
	
	cextern CONST_S16_89
	cextern CONST_S16_75
	cextern CONST_S16_50
	cextern CONST_S16_18
	cextern CONST_S16_N89
	cextern CONST_S16_N50
	cextern CONST_S16_N83
	cextern CONST_S16_N64
	cextern CONST_S16_N18
	
	cextern CONST_TR_32x32
	cextern CONST_TR_16x16_1
	cextern CONST_TR_16x16_2
	cextern CONST_TR_16x16_3
	cextern CONST_TR_32X32_2ND_ROW
	cextern CONST_TR_16X16_2ND_ROW
	cextern CONST_S16_1_N1
	cextern CONST_S16_POS74
	cextern CONST_S16_POS1
	cextern CONST_TR_IDST_4X4
	cextern CONST_TR_32X32_8X8
	cextern CONST_TR_16X16_8X8

    section  .text
    
%macro VO_ARG_INIT 0
	%define arg_offset_x64(x)	x*0x08
	%define arg_offset_x86(x)	x*0x04
%if ARCH_X86_64
	%xdefine arg_0	arg_offset_x64(1)
	%xdefine arg_1	arg_offset_x64(2)
	%xdefine arg_2	arg_offset_x64(3)
	%xdefine arg_3	arg_offset_x64(4)
	%xdefine arg_4	arg_offset_x64(5)
	%xdefine arg_5	arg_offset_x64(6)
	%xdefine arg_6	arg_offset_x64(7)
	%xdefine arg_7	arg_offset_x64(8)
%else
	%xdefine arg_0	arg_offset_x86(1)
	%xdefine arg_1	arg_offset_x86(2)
	%xdefine arg_2	arg_offset_x86(3)
	%xdefine arg_3	arg_offset_x86(4)
	%xdefine arg_4	arg_offset_x86(5)
	%xdefine arg_5	arg_offset_x86(6)
	%xdefine arg_6	arg_offset_x86(7)
	%xdefine arg_7	arg_offset_x86(8)
%endif
%endmacro
   
;input	%1 = mov or not the last param
%macro IDCT_FUNC_HEAD	0-1	0
	VO_ARG_INIT
	;r0 = p_invtransform_buff
	;r1 = p_prediction_buff
	;r2 = prediction_stride
	;r3 = typeU32_width
	%if ARCH_X86_64
		mov	r6,	rsp
	%else
		%xdefine st_offset	0x10
		push	r3
		push	r4
		push	r5
		push	r6
		mov		r6,	rsp
		
		mov		r0,	[rsp + st_offset + arg_0]	;r0 = p_invtransform_buff VO_S16
		mov		r1,	[rsp + st_offset + arg_1]	;r1 = p_prediction_buff
		mov		r2,	[rsp + st_offset + arg_2]	;r2 = prediction_stride
		%if %1 == 1
			mov		r3,	[rsp + st_offset + arg_3]	;r3 = typeU32_width
		%endif
	%endif
%endmacro

%macro IDCT_FUNC_FOOT	0
	%if ARCH_X86_64
		mov	rsp, r6
		ret
	%else
		mov		rsp,	r6
		pop		r6
		pop		r5
		pop		r4
		pop		r3
		ret
	%endif
%endmacro
    
%macro vo_xdefine_cat 3
    %xdefine %1%2 %3
%endmacro
    
%macro vo_exchange 2-* ; exchange a serial identifier in order
%rep %0-1
%ifndef m%1
	;the number corresponding m0 m1 ...  must be inferred when the parameters of 
    ;this macro is like as m0,m1 .
    %xdefine %%n1 n %+ %1
    %xdefine %%n2 n %+ %2
    %xdefine temp m %+ %%n1
    vo_xdefine_cat m, %%n1, m %+ %%n2
    vo_xdefine_cat m, %%n2, temp
    vo_xdefine_cat n, m %+ %%n1, %%n1
    vo_xdefine_cat n, m %+ %%n2, %%n2
%else
    %xdefine temp m%1
    %xdefine m%1  m%2
    %xdefine m%2  temp
    vo_xdefine_cat n, m%1, %1
    vo_xdefine_cat n, m%2, %2
%endif
    %undef temp
    %rotate 1
%endrep
%endmacro

;input	%1 = main dimension 32 16 8 4
;		%2 = sub dimension
%macro IDCT_TRANSPOSE_ADAPTER 2
	%if %1 == 32
		IDCT32X32_TRANSPOSE %2
	%elif %1 == 16
		IDCT16X16_TRANSPOSE %2
	%elif %1 == 8
		IDCT8X8_TRANSPOSE
	%elif %1 == 4
		IDCT4X4_TRANSPOSE
	%endif
%endmacro

;input	%1 = error of DWORD
;		%2 = prediction buff
;		%3 = reconstruction buff
;		%4 = zero vector
;		%5 = temp register

%macro SAVE_4_PIX	 5
	movd		%5,	%2
	punpcklbw	%5,	%4
	paddw		%1,	%5
	packuswb	%1,	%1
	movd		%3,	%1
%endmacro

;input	%1 = error of DWORD
;		%2 = prediction buff
;		%3 = prediction stride
;		%4 = reconstruction buff
;		%5 = reconstruction stride
;		%6 = zero vector
;		%7 = temp register

%macro SAVE_4_PIX_3	 7
	movd		%7,	[%2 + gr_tmp_1]
	punpcklbw	%7,	%6
	paddw		%1,	%7
	packuswb	%1,	%1
	movd		[%4 + gr_tmp_1], %1
%endmacro

;input	%1 = main dimension 32 for D32X32 16 for D16X16 8 for D8X8  4 for D4X4
;		%2 = sub  dimension	4 8 16
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_ODD_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7

	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(2*dim_main/4)*8
	
	%assign %%LINE_NO	0
	%define LINES		2*dim_main/4
	
	%if   dim_main == 32
		%define	tr_coef_table CONST_TR_32x32
	%elif dim_main == 16
		%define	tr_coef_table CONST_TR_16x16_1
	%elif dim_main == 8
		%define	tr_coef_table CONST_TR_16x16_2
	%elif dim_main == 4
		%define	tr_coef_table CONST_TR_16x16_3
	%endif
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coef_table + %%LINE_NO * TR_COEF_STRIDE
		
		;-------------One line of odd------------
		%assign %%Line_No	0
		%define Lines  (dim_sub/4)
		%rep Lines
			%define cur_src		s_buf + (%%Line_No * 4 + 1) * s_stride
			
			%if tr_width >= 16
				IDCT_BUTTERFLY_2_LINES_16_PIX %%Line_No, m0,m1,m2,m3, cur_src,s_stride, m4,m5,m6,m7
			%elif tr_width == 8
				IDCT_BUTTERFLY_2_LINES_8_PIX  %%Line_No, m0,m1,cur_src,s_stride, m2,m3,m4,m5
			%elif tr_width == 4
				%if %%LINE_NO == 0
					movq	m1, [cur_src + 0 * s_stride]
					movq	m2, [cur_src + 2 * s_stride]
					punpcklwd	m1, m2	;
				%endif
				mova		m0,	[TR_COEF_BUF]
				pmaddwd	m0, m1
			%endif
			
			%assign %%Line_No %%Line_No + 1
		%endrep
		
		mova	[d_buf + d_stride * %%LINE_NO + 0 * 16],	m0
		%if tr_width >= 8
			mova	[d_buf + d_stride * %%LINE_NO + 1 * 16],	m1
			%if tr_width >= 16
				mova	[d_buf + d_stride * %%LINE_NO + 2 * 16],	m2
				mova	[d_buf + d_stride * %%LINE_NO + 3 * 16],	m3
			%endif
		%endif
		;-------------One line of odd------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep
    
%endmacro

;input	%1 = sub dimension
;		%2 = tr_width
;		%3 = src
;		%4 = src_stride in unit of byte
;		%5 = dest
;		%6 = dest_stride  in unit of byte

%macro IDCT32X32_ODD_SSE4 6
	IDCT_ODD_SSE4 32,%1,%2,%3,%4,%5,%6
%endmacro

;input	%1 = Line No
;		%2 = save 4 pix in unit of DWORD
;		%3 = save 4 pix in unit of DWORD
;		%4 = src
;		%5 = src_stride
;		%6 = temp
;		%7 = temp
;		%8 = temp
;		%9 = temp


%macro IDCT_BUTTERFLY_2_LINES_8_PIX 9
		mova		%9,	[TR_COEF_BUF + %1 * TR_COEF_BLOCK_SIZE]
	%if %1 == 0
		mova		%2,	[%4]				;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%6,	[%4 + 2*%5]			;b0 b1 b2 b3 b4 b5 b6 b7
		mova		%3,	%2
		
		punpcklwd	%2,	%6					;a0 b0 a1 b1 a2 b2 a3 b3
		punpckhwd	%3,	%6					;a4 b4 a5 b5 a6 b6 a7 b7
		pmaddwd	%2,	%9					;%2 = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%3,	%9					;%6 = cof0 * a4 + cof1 * b4 ...
	%else
		mova		%6,	[%4]				;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%7,	[%4 + 2*%5]			;b0 b1 b2 b3 b4 b5 b6 b7
		mova		%8,	%6
		
		punpcklwd	%6,	%7					;a0 b0 a1 b1 a2 b2 a3 b3
		punpckhwd	%8,	%7					;a4 b4 a5 b5 a6 b6 a7 b7
		pmaddwd	%6,	%9					;%6 = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%8,	%9					;%8 = cof0 * a4 + cof1 * b4 ...
		paddd		%2,	%6
		paddd		%3,	%8
	%endif
%endmacro

;input	%1 = Line No
;		%2 = Section 0 : 0 - 7   low
;		%3 = Section 1 : 0 - 7   hi
;		%4 = Section 2 : 8 - 15  low
;		%5 = Section 3 : 8 - 15  hi
;		%6 = src
;		%7 = src_stride
;		%8 = temp
;		%9 = temp
;		%10 = temp
;		%11 = temp

%macro IDCT_BUTTERFLY_2_LINES_16_PIX 11
	%if %1 == 0
		mova		%8,	[TR_COEF_BUF + %1 * TR_COEF_BLOCK_SIZE]
		mova		%2,	[%6 + 0*16]			;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%4,	[%6 + 2*%7 + 0*16]	;b0 b1 b2 b3 b4 b5 b6 b7
		mova		%3,	%2
		
		punpcklwd	%2,	%4					;a0 b0 a1 b1 a2 b2 a3 b3
		punpckhwd	%3,	%4					;a4 b4 a5 b5 a6 b6 a7 b7
		pmaddwd	%2,	%8					;%2 = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%3,	%8					;%3 = cof0 * a4 + cof1 * b4 ...
		
		mova		%4,	[%6 + 1*16]			;a8 a9 a10 a11 a12 a13 a14 a15
		mova		%9,	[%6 + 2*%7 + 1*16]	;b8 b9 b10 b11 b12 b13 b14 b15
		mova		%5,	%4
		punpcklwd	%4,	%9					;%4 = a8 b8 a9 b9 a10 b10 a11 b11
		punpckhwd	%5,	%9					;%5 = a12 b12 a13 b13 a14 b14 a15
		pmaddwd	%4,	%8					;
		pmaddwd	%5,	%8					;
	%else
		mova		%8,		[TR_COEF_BUF + %1 * TR_COEF_BLOCK_SIZE]
		mova		%9,		[%6 + 0*16]			;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%11,	[%6 + 2*%7 + 0*16]	;b0 b1 b2 b3 b4 b5 b6 b7
		mova		%10,	%9
		
		punpcklwd	%9,	%11					;a0 b0 a1 b1 a2 b2 a3 b3
		punpckhwd	%10,%11					;a4 b4 a5 b5 a6 b6 a7 b7
		pmaddwd	%9,	%8					;%9  = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%10,%8					;%10 = cof0 * a4 + cof1 * b4 ...
		paddd		%2,	%9
		paddd		%3,	%10
		
		mova		%9,		[%6 + 1*16]			;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%11,	[%6 + 2*%7 + 1*16]	;b0 b1 b2 b3 b4 b5 b6 b7
		mova		%10,	%9
		
		punpcklwd	%9,	%11					;a0 b0 a1 b1 a2 b2 a3 b3
		punpckhwd	%10,%11					;a4 b4 a5 b5 a6 b6 a7 b7
		pmaddwd	%9,	%8					;%9  = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%10,%8					;%10 = cof0 * a4 + cof1 * b4 ...
		paddd		%4,	%9
		paddd		%5,	%10
	%endif
%endmacro

;input	%1 = Line No
;		%2 = tr_width
;		%2 = Section 0 : 0 - 7   low
;		%3 = Section 1 : 0 - 7   hi
;		%4 = Section 2 : 8 - 15  low
;		%5 = Section 3 : 8 - 15  hi
;		%6 = src
;		%7 = src_stride
;		%8 = temp
;		%9 = temp
;		%10 = temp
;		%11 = temp

%macro IDCT_LINES_PIX 12
		pxor		%12, %12
		mova		%9,	[TR_COEF_BUF + %1 * TR_COEF_BLOCK_SIZE]
	%if %1 == 0
		mova		%3,	[%7 + 0*16]			;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%4,	%3
		punpcklwd	%3,	%12					;a0 0 a1 0 a2 0 a3
		punpckhwd	%4,	%12					;a4 0 a5 0 a6 0 a7
		;wait for do with
		pmaddwd	%3,	%9					;%3 = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%4,	%9					;%4 = cof0 * a4 + cof1 * b4 ...
		
		%if %2 >= 16
			mova		%5,	[%7 + 1*16]			;a8 a9 a10 a11 a12 a13 a14 a15
			mova		%6,	%5
			punpcklwd	%5,	%12					;%5 = a8 b8 a9 b9 a10 b10 a11 b11
			punpckhwd	%6,	%12					;%6 = a12 b12 a13 b13 a14 b14 a15
			pmaddwd	%5,	%9					;
			pmaddwd	%6,	%9					;
		%endif
	%else
		mova		%10,[%7 + 0*16]			;a0 a1 a2 a3 a4 a5 a6 a7
		mova		%11,%10
		punpcklwd	%10,%12					;a0 0 a1 0 a2 0 a3
		punpckhwd	%11,%12					;a4 0 a5 0 a6 0 a7
		pmaddwd	%10,%9					;%10  = cof0 * a0 + cof1 * b0 ...
		pmaddwd	%11,%9					;%11 = cof0 * a4 + cof1 * b4 ...
		paddd		%3,	%10
		paddd		%4,	%11
		
		%if %2 >= 16
			mova		%10,[%7 + 1*16]			;a8 a9 a10 a11 a12 a13 a14 a15
			mova		%11,%10
			punpcklwd	%10,%12					;a0 b0 a1 b1 a2 b2 a3 b3
			punpckhwd	%11,%12					;a4 b4 a5 b5 a6 b6 a7 b7
			pmaddwd	%10,%9					;%10  = cof0 * a0 + cof1 * b0 ...
			pmaddwd	%11,%9					;%11 = cof0 * a4 + cof1 * b4 ...
			paddd		%5,	%10
			paddd		%6,	%11
		%endif
	%endif
%endmacro

;input	%1 = Dimenson 32 for D32X32 16 for D16X16
;		%2 = sub dimension
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_EO_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7
	
	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(2*dim_main/8)*8
	
	%assign %%LINE_NO	0
	%define LINES		2*dim_main/8
	
	%if	  dim_main == 32
		%define tr_coeff_table CONST_TR_16x16_1
	%elif dim_main == 16
		%define tr_coeff_table CONST_TR_16x16_2
	%elif dim_main == 8
		%define tr_coeff_table CONST_TR_16x16_3
	%endif
	
	%rep LINES
		%define	TR_COEF_BUF	 tr_coeff_table + %%LINE_NO * TR_COEF_STRIDE
		
		;-------------One line of EO------------
		%assign %%Line_No	0
		%define Lines dim_sub/8
		%rep Lines
			%define cur_src		s_buf + (%%Line_No * 4 + 1) * s_stride * 2
			%if   tr_width >= 16
				IDCT_BUTTERFLY_2_LINES_16_PIX %%Line_No, m0,m1,m2,m3, cur_src,s_stride*2, m4,m5,m6,m7
			%elif tr_width == 8
				IDCT_BUTTERFLY_2_LINES_8_PIX  %%Line_No, m0,m1, cur_src,s_stride*2, m2,m3,m4,m5
			%endif
			%assign %%Line_No %%Line_No + 1
		%endrep
		
		mova	[d_buf + d_stride * %%LINE_NO + 0 * 16],	m0
		%if tr_width >= 8
			mova	[d_buf + d_stride * %%LINE_NO + 1 * 16],	m1
			%if tr_width >= 16
				mova	[d_buf + d_stride * %%LINE_NO + 2 * 16],	m2
				mova	[d_buf + d_stride * %%LINE_NO + 3 * 16],	m3
			%endif
		%endif
		;-------------One line of EO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = Dimenson 32 for D32X32 16 for D16X16
;		%2 = sub dimension
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_EEO_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7
	
	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(2*dim_main/16)*8
	
	%assign %%LINE_NO	0
	%define LINES		2*dim_main/16
	
	%if dim_main == 32
		%if	dim_sub == 8
			%define tr_coeff_table CONST_TR_32X32_8X8
			%define TR_COEF_BLOCK_SIZE	16
		%else
			%define tr_coeff_table CONST_TR_16x16_2
		%endif
	%elif dim_main == 16
		%define tr_coeff_table CONST_TR_16x16_3
	%endif
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coeff_table + %%LINE_NO * TR_COEF_STRIDE
		
		;-------------One line of EO------------
		%assign %%Line_No	0
		
		%if dim_sub >= 32
			%define Lines dim_sub/16
		%else
			%define Lines 1
		%endif
		
		%rep Lines
			;4,12	20,28
			%define cur_src		s_buf + (%%Line_No * 4 + 1) * s_stride * 4
			
			%if	dim_sub >= 16
				IDCT_BUTTERFLY_2_LINES_16_PIX %%Line_No, m0,m1,m2,m3, cur_src,s_stride*4, m4,m5,m6,m7
			%elif dim_sub == 8
				;%if tr_width == 8
					;IDCT_BUTTERFLY_2_LINES_8_PIX  %%Line_No, m0,m1, cur_src,s_stride*2, m2,m3,m4,m5
				;%else
					%if %%LINE_NO == 0
						pxor		m7,	m7
						mova		m6,	[cur_src + 0]		;a0 a1 a2 a3 a4 a5 a6 a7
						%if tr_width >= 16
							mova		m5,	[cur_src + 16]		;a8 a9 a10 a11 a12 a13 a14 a15
						%endif
					%endif
					mova		m4,	[TR_COEF_BUF]
					
					mova		m0,	m6
					mova		m1,	m6
					punpcklwd	m0,	m7					;a0 0 a1 0 a2 0 a3
					punpckhwd	m1,	m7					;a4 0 a5 0 a6 0 a7
					;wait for do with
					pmaddwd	m0,	m4					;%3 = cof0 * a0 + cof1 * b0 ...
					pmaddwd	m1,	m4					;%4 = cof0 * a4 + cof1 * b4 ...
					
					%if tr_width >= 16
						mova		m2,	m5
						mova		m3,	m5
						punpcklwd	m2,	m7					;a0 0 a1 0 a2 0 a3
						punpckhwd	m3,	m7					;a4 0 a5 0 a6 0 a7
						;wait for do with
						pmaddwd	m2,	m4					;%3 = cof0 * a0 + cof1 * b0 ...
						pmaddwd	m3,	m4					;%4 = cof0 * a4 + cof1 * b4 ...
					%endif
				;%endif
			%elif dim_sub == 4
				
			%endif
			
			%assign %%Line_No %%Line_No + 1
		%endrep
		
		mova	[d_buf + d_stride * %%LINE_NO + 0 * 16],	m0
		%if tr_width >= 8
			mova	[d_buf + d_stride * %%LINE_NO + 1 * 16],	m1
			%if tr_width >= 16
				mova	[d_buf + d_stride * %%LINE_NO + 2 * 16],	m2
				mova	[d_buf + d_stride * %%LINE_NO + 3 * 16],	m3
			%endif
		%endif
		;-------------One line of EO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimenson 8 for D8X8 4 for D4X4

%macro IDCT_EE_SSE4 5
	
	%define TR_COEF_STRIDE		2*8
	%if %5 == 8
		%define TR_COEF_BLOCK_SIZE	2*2*8
	%elif %5 == 4
		%define TR_COEF_BLOCK_SIZE	2*2*8
	%endif
	
	%assign %%LINE_NO	0
	%if %5 == 8
		%define LINES 2
	%elif %5 == 4
		%define LINES 2
		%if const_im_shr == 7
			mova		m7,	[CONST_U32_POS64]		;%7 = 64
		%elif const_im_shr == 12
			mova		m7,	[CONST_U32_POS_0X0800]	;%7 = 0x800
		%endif
	%endif
	
	%if %5 <= 8
		%define tr_coeff_table CONST_TR_16x16_3 + TR_COEF_BLOCK_SIZE
	%endif
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coeff_table + %%LINE_NO * TR_COEF_STRIDE
		
		;-------------One line of EO------------
		%assign %%Line_No	0
		%if %5 == 8
			%define Lines 1
		%elif %5 == 4
			%define Lines 1
		%endif
		
		%rep Lines
			%define cur_src		%1
			%if %5 == 8
				IDCT_BUTTERFLY_2_LINES_8_PIX %%Line_No, m0,m1, cur_src,%2*2,m2,m3,m4,m5
			%elif %5 == 4
				%if %%LINE_NO == 0
					movq	m4,	[cur_src + 0*%2]
					movq	m5,	[cur_src + 2*%2]
					punpcklwd	m4, m5
				%endif
				mova		m0,	[TR_COEF_BUF]
				pmaddwd	m0, m4
			%endif
			%assign %%Line_No %%Line_No + 1
		%endrep
		
		%if %5 == 8
			;8X8
			%if %%LINE_NO == 0
				IDCT_E_EE_0734 %5, 0,%3 ,%4,m0,m4,m5,m6,m7
				IDCT_E_EE_0734 %5, 1,%3 ,%4,m1,m4,m5,m6,m7
			%else
				IDCT_E_EE_1625 %5, 0,%3 + %%LINE_NO * %4,%4,m0,m4,m5,m6,m7
				IDCT_E_EE_1625 %5, 1,%3 + %%LINE_NO * %4,%4,m1,m4,m5,m6,m7
			%endif
		%elif %5 == 4
			IDCT_4X4_BLEND_E_O_2_LINE	%%LINE_NO,%3 + %%LINE_NO * %4,%4,m0,m7,m1,m2,m3
		%endif
		;-------------One line of EO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimenson 32 for D32X32 16 for D16X16

%macro IDCT_EEE_SSE4 5

	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(2*%5/16)*8
	
	%assign %%LINE_NO	0
	%define LINES		2*%5/16
	
	%if %5 == 32
		
	%elif %5 == 16
		%define tr_coeff_table CONST_TR_16x16_3 + TR_COEF_BLOCK_SIZE
	%endif
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coeff_table + %%LINE_NO * TR_COEF_STRIDE
		
		;-------------One line of EO------------
		%assign %%Line_No	0
		%define Lines %5/16
		%rep Lines
			;4,12	20,28
			%define cur_src		%1
			IDCT_BUTTERFLY_2_LINES_16_PIX %%Line_No, m0,m1,m2,m3, cur_src,%2*4, m4,m5,m6,m7
			%assign %%Line_No %%Line_No + 1
		%endrep
		
		%if %5 == 32
			;32X32
			mova	[%3 + %4 * %%LINE_NO + 0 * 16],	m0
			mova	[%3 + %4 * %%LINE_NO + 1 * 16],	m1
			mova	[%3 + %4 * %%LINE_NO + 2 * 16],	m2
			mova	[%3 + %4 * %%LINE_NO + 3 * 16],	m3
		%elif %5 == 16
			;16X16
			%if %%LINE_NO == 0
				IDCT_E_EE_0734 %5,0,%3,%4,m0,m4,m5,m6,m7
				IDCT_E_EE_0734 %5,1,%3,%4,m1,m4,m5,m6,m7
				IDCT_E_EE_0734 %5,2,%3,%4,m2,m4,m5,m6,m7
				IDCT_E_EE_0734 %5,3,%3,%4,m3,m4,m5,m6,m7
			%else
				IDCT_E_EE_1625 %5,0,%3+%4,%4,m0,m4,m5,m6,m7
				IDCT_E_EE_1625 %5,1,%3+%4,%4,m1,m4,m5,m6,m7
				IDCT_E_EE_1625 %5,2,%3+%4,%4,m2,m4,m5,m6,m7
				IDCT_E_EE_1625 %5,3,%3+%4,%4,m3,m4,m5,m6,m7
			%endif
		%endif
		
		;-------------One line of EO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = Dimenson 32 for D32X32 16 for D16X16
;		%2 = sub dimension
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_EEEO_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7
	
	%define TR_16x16_3_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE			2*2*8
	
	%assign %%LINE_NO	0
	%define LINES		dim_main/16
	
	%rep LINES
		%define	TR_COEF_BUF	CONST_TR_16x16_3 + %%LINE_NO * TR_16x16_3_COEF_STRIDE
		
		;-------------One line of EEEO------------
		;src line : 8 24
		%define cur_src		s_buf + s_stride * 8
		
		%if dim_sub == 32
			IDCT_BUTTERFLY_2_LINES_16_PIX 0, m0,m1,m2,m3, cur_src,s_stride*8, m4,m5,m6,m7
		%elif dim_sub <= 16
			IDCT_LINES_PIX  0, tr_width, m0,m1,m2,m3, cur_src,s_stride*8, m4,m5,m6,m7
		%endif
		
		mova	[d_buf + d_stride * %%LINE_NO + 0 * 16],	m0
		%if dim_sub >= 8
			mova	[d_buf + d_stride * %%LINE_NO + 1 * 16],	m1
			%if dim_sub >= 16
				mova	[d_buf + d_stride * %%LINE_NO + 2 * 16],	m2
				mova	[d_buf + d_stride * %%LINE_NO + 3 * 16],	m3
			%endif
		%endif
		
		;-------------One line of EEEO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = Dimenson 32 for D32X32 16 for D16X16
;		%2 = sub dimension
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_EEEE_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7
	
	%define TR_16x16_3_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*2*8
	
	%assign %%LINE_NO	0
	%define LINES		2
	
	%rep LINES
		%define	TR_COEF_BUF	CONST_TR_16x16_3 + (%%LINE_NO + 2) * TR_16x16_3_COEF_STRIDE
		
		;-------------One line of EEEO------------

		%define cur_src		s_buf
		%if dim_sub == 32
			IDCT_BUTTERFLY_2_LINES_16_PIX 0, m0,m1,m2,m3, cur_src,s_stride*8, m4,m5,m6,m7
		%elif dim_sub <= 16
			IDCT_LINES_PIX  0, tr_width, m0,m1,m2,m3, cur_src,s_stride*8, m4,m5,m6,m7
		%endif
		
		%if %%LINE_NO == 0
			IDCT_E_EE_0734 dim_main,0,d_buf,d_stride,m0,m4,m5,m6,m7
			%if tr_width >= 8
				IDCT_E_EE_0734 dim_main,1,d_buf,d_stride,m1,m4,m5,m6,m7
				%if tr_width >= 16
					IDCT_E_EE_0734 dim_main,2,d_buf,d_stride,m2,m4,m5,m6,m7
					IDCT_E_EE_0734 dim_main,3,d_buf,d_stride,m3,m4,m5,m6,m7
				%endif
			%endif
		%else
			IDCT_E_EE_1625 dim_main,0,d_buf+d_stride,d_stride,m0,m4,m5,m6,m7
			%if tr_width >= 8
				IDCT_E_EE_1625 dim_main,1,d_buf+d_stride,d_stride,m1,m4,m5,m6,m7
				%if tr_width >= 16
					IDCT_E_EE_1625 dim_main,2,d_buf+d_stride,d_stride,m2,m4,m5,m6,m7
					IDCT_E_EE_1625 dim_main,3,d_buf+d_stride,d_stride,m3,m4,m5,m6,m7
				%endif
			%endif
		%endif
		;-------------One line of EEEO------------
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = Dimenson 32 for D32X32 16 for D16X16
;		%2 = sub dimension
;		%3 = tr_width
;		%4 = src
;		%5 = src_stride in unit of byte
;		%6 = dest
;		%7 = dest_stride  in unit of byte

%macro IDCT_32EX_EE_SSE4 7
	%define dim_main	%1
	%define dim_sub		%2
	%define tr_width	%3
	%define s_buf		%4
	%define s_stride	%5
	%define d_buf		%6
	%define d_stride	%7
	
	%if tr_width >= 16
		%define section_num	2
	%else
		%define section_num	%3/8
	%endif
	
	%assign %%section_no 0
	
	%if %2 == 8
		pxor	m7,	m7
		%if %%section_no == 0
			mova	m6,	[CONST_TR_16x16_3 + 2*2*8]
		%endif
		
		%define LH_POS(LineNo)	LineNo * d_stride + %%section_no * 32 + 0*16
		%define RH_POS(LineNo)	LineNo * d_stride + %%section_no * 32 + 1*16
		
		%rep section_num
			mova	m0,	[s_buf + %%section_no * 16]
			mova	m1,	m0
			punpcklwd	m0,	m7
			punpckhwd	m1,	m7
			pmaddwd	m0, m6
			pmaddwd	m1, m6
			
			mova	m2,	m0
			mova	m3,	m1
			mova	m4,	[d_buf + LH_POS(-4)]	;EEO[0]
			mova	m5,	[d_buf + RH_POS(-4)]	;EEO[0]
			paddd	m2,	m4
			paddd	m3,	m5
			mova	[d_buf + LH_POS(-4)], m2	;EE_0
			mova	[d_buf + RH_POS(-4)], m3	;EE_0
			
			mova	m2,	m0
			mova	m3,	m1
			psubd	m2,	m4
			psubd	m3,	m5
			mova	[d_buf + LH_POS(0)], m2		;EE_7
			mova	[d_buf + RH_POS(0)], m3		;EE_7
			
			mova	m2,	m0
			mova	m3,	m1
			mova	m4,	[d_buf + LH_POS(-1)]	;EEO[3]
			mova	m5,	[d_buf + RH_POS(-1)]	;EEO[3]
			paddd	m2,	m4
			paddd	m3,	m5
			mova	[d_buf + LH_POS(-1)], m2	;EE_3
			mova	[d_buf + RH_POS(-1)], m3	;EE_3
			mova	m2,	m0
			mova	m3,	m1
			psubd	m2,	m4
			psubd	m3,	m5
			mova	[d_buf + LH_POS(2)], m2		;EE_4
			mova	[d_buf + RH_POS(2)], m3		;EE_4
			
			mova	m2,	m0
			mova	m3,	m1
			mova	m4,	[d_buf + LH_POS(-3)]	;EEO[1]
			mova	m5,	[d_buf + RH_POS(-3)]	;EEO[1]
			paddd	m2,	m4
			paddd	m3,	m5
			mova	[d_buf + LH_POS(-3)], m2	;EE_1
			mova	[d_buf + RH_POS(-3)], m3	;EE_1
			mova	m2,	m0
			mova	m3,	m1
			psubd	m2,	m4
			psubd	m3,	m5
			mova	[d_buf + LH_POS(1)], m2		;EE_6
			mova	[d_buf + RH_POS(1)], m3		;EE_6
			
			mova	m2,	m0
			mova	m3,	m1
			mova	m4,	[d_buf + LH_POS(-2)]	;EEO[2]
			mova	m5,	[d_buf + RH_POS(-2)]	;EEO[2]
			paddd	m2,	m4
			paddd	m3,	m5
			mova	[d_buf + LH_POS(-2)], m2	;EE_2
			mova	[d_buf + RH_POS(-2)], m3	;EE_2
			mova	m2,	m0
			mova	m3,	m1
			psubd	m2,	m4
			psubd	m3,	m5
			mova	[d_buf + LH_POS(3)], m2		;EE_5
			mova	[d_buf + RH_POS(3)], m3		;EE_5
			
			%assign %%section_no	%%section_no + 1
		%endrep
	%endif
%endmacro

;input	%1 = E_O_Index
;		%2 = dest
;		%3 = dest_stride
;		%4 = E
;		%5 = rshift_add
;		%6 = 
;		%7 = 
;		%8 = 


%macro IDCT_4X4_BLEND_E_O_2_LINE	8
		%define shift_add %5
		;%4 = E
		;%define offset_e  0
		mova	%7,	[%2 - %3 * 2]	;%7 = O[0];
		mova	%6,	%4
		paddd	%4,	%7		;%4 = tr[0] = E[0] + O[0];
		psubd	%6,	%7		;%6 = tr[3] = E[0] - O[0];
		
		paddd	%4, shift_add
		paddd	%6, shift_add
		psrad	%4, const_im_shr
		psrad	%6, const_im_shr
		packssdw	%4, %4
		packssdw	%6, %6
		
		;%define line_0	rsp_buff + offset_e + (0 or 1)*st_stride
		;%define line_3	rsp_buff + offset_e + (2 or 3)*st_stride
		movq	[%2 - %3 * 2],	%4
		movq	[%2 - %3 * 0],	%6

%endmacro

;input	%1 = Dimension
;		%2 = part index
;		%3 = dest
;		%4 = dest_stride
;		%5 = EEEE
;		%6 = 
;		%7 =
;		%8 =
;		%9 =

%macro IDCT_E_EE_0734	9
		
		%if %1 == 8
			%if const_im_shr == 7
				mova		%9,	[CONST_U32_POS64]		;m%5 = 64
			%elif const_im_shr == 12
				mova		%9,	[CONST_U32_POS_0X0800]	;m%5 = 0x800
			%endif
		%endif
		
		;%5 = EEEE 0
		;%define offset_e  24*st_stride
		mova	%6,	[%3 - %4 * 2 + %2 * 16]
							;%6 = EEEO[0];
		mova	%7,	%5
		paddd	%5,	%6		;%5 = EEE[0] = EEEE[0] + EEEO[0];
		psubd	%7,	%6		;%7 = EEE[3] = EEEE[0] - EEEO[0];
		
		mova	%6,	[%3 - %4 * 6 + %2 * 16]	
							;%6 = EE0[0]
		mova	%8,	%5		;
		paddd	%5,	%6		;%5 = EE[0] = EEE[0] + EE0[0]
		psubd	%8,	%6		;%8 = EE[7] = EEE[0] - EE0[0]
		
		;%define EE_0	rsp_buff + offset_e + 0*st_stride
		;%define EE_7	rsp_buff + offset_e + 4*st_stride
		%if %1 >= 16
			mova	[%3 - %4 * 6 + %2 * 16],	%5
			mova	[%3 - %4 * 2 + %2 * 16],	%8
		%elif %1 == 8
			paddd	%5, %9
			paddd	%8, %9
			psrad	%5, const_im_shr
			psrad	%8, const_im_shr
			packssdw	%5, %5
			packssdw	%8, %8
			movq	[%3 - %4 * 6 + %2 * 8],	%5
			movq	[%3 - %4 * 2 + %2 * 8],	%8
		%endif
		
		mova	%6,	[%3 - %4 * 3 + %2 * 16]	
							;%6 = EE0[3]
		mova	%8,	%7		;
		paddd	%7,	%6		;%7 = EE[3] = EEE[3] + EE0[3]
		psubd	%8,	%6		;%8 = EE[4] = EEE[3] - EE0[3]
		
		;%define EE_3	rsp_buff + offset_e + 3*st_stride
		;%define EE_4	rsp_buff + offset_e + 6*st_stride
		%if %1 >= 16
			mova	[%3 - %4 * 3 + %2 * 16],	%7
			mova	[%3 - %4 * 0 + %2 * 16],	%8
		%elif %1 == 8
			paddd	%7, %9
			paddd	%8, %9
			psrad	%7, const_im_shr
			psrad	%8, const_im_shr
			packssdw	%7, %7
			packssdw	%8, %8
			movq	[%3 - %4 * 3 + %2 * 8],	%7
			movq	[%3 - %4 * 0 + %2 * 8],	%8
		%endif
		
%endmacro

;input	%1 = Dimension
;		%2 = part index
;		%3 = dest
;		%4 = dest_stride
;		%5 = EEEE
;		%6 = 
;		%7 =
;		%8 =
;		%9 =

%macro IDCT_E_EE_1625	9

		%if %1 == 8
			%if const_im_shr == 7
				mova		%9,	[CONST_U32_POS64]		;m%5 = 64
			%elif const_im_shr == 12
				mova		%9,	[CONST_U32_POS_0X0800]	;m%5 = 0x800
			%endif
		%endif
		;%5 = EEEE 1
		;%define offset_e  24*st_stride
		mova	%6,	[%3 - %4 * 2 + %2 * 16]
							;%6 = EEEO[1];
		mova	%7,	%5
		paddd	%5,	%6		;%5 = EEE[1] = EEEE[1] + EEEO[1];
		psubd	%7,	%6		;%7 = EEE[2] = EEEE[1] - EEEO[1];
		
		mova	%6,	[%3 - %4 * 6 + %2 * 16]	
							;%6 = EE0[1]
		mova	%8,	%5		;
		paddd	%5,	%6		;%5 = EE[1] = EEE[1] + EE0[1]
		psubd	%8,	%6		;%8 = EE[2] = EEE[1] - EE0[1]
		
		;%define EE_1	esp + offset_e + 1*st_stride
		;%define EE_6	esp + offset_e + 5*st_stride
		
		%if %1 >= 16
			mova	[%3 - %4 * 6 + %2 * 16],	%5
			mova	[%3 - %4 * 2 + %2 * 16],	%8
		%elif %1 == 8
			paddd	%5, %9
			paddd	%8, %9
			psrad	%5, const_im_shr
			psrad	%8, const_im_shr
			packssdw	%5, %5
			packssdw	%8, %8
			movq	[%3 - %4 * 6 + %2 * 8],	%5
			movq	[%3 - %4 * 2 + %2 * 8],	%8
		%endif
		
		mova	%6,	[%3 - %4 * 5 + %2 * 16]	
							;%6 = EE0[2]
		mova	%8,	%7		;
		paddd	%7,	%6		;%7 = EE[3] = EEE[2] + EE0[2]
		psubd	%8,	%6		;%8 = EE[4] = EEE[2] - EE0[2]
		
		;%define EE_2	esp + offset_e + 2*st_stride
		;%define EE_5	esp + offset_e + 7*st_stride
		%if %1 >= 16
			mova	[%3 - %4 * 5 + %2 * 16],	%7
			mova	[%3 - %4 * 0 + %2 * 16],	%8
		%elif %1 == 8
			paddd	%7, %9
			paddd	%8, %9
			psrad	%7, const_im_shr
			psrad	%8, const_im_shr
			packssdw	%7, %7
			packssdw	%8, %8
			movq	[%3 - %4 * 5 + %2 * 8],	%7
			movq	[%3 - %4 * 0 + %2 * 8],	%8
		%endif
%endmacro

;input	%1 = main dimenson	32 for D32X32  16 for D16X16
;		%2 = sub dimension  8 16 32
;		%3 = tr_width
%macro IDCT_E_SSE4	3
	
	%assign	%%Line_No 0
	%define	LINES	4*%1/16
	%rep LINES
		IDCT32X32_E_2_LINES EE_ %+ %%Line_No,EO_ %+ %%Line_No,%2, %3
		%assign	%%Line_No %%Line_No + 1
	%endrep
	
%endmacro

;input	%1 = sub dim 8 16 32
;		%2 = tr_width
%macro IDCT32X32_E_SSE4	2
	%define	offset_eo	16 * st_stride
	%define EO_0	rsp_buff + offset_eo + 0 * st_stride
	%define EO_1	rsp_buff + offset_eo + 1 * st_stride
	%define EO_2	rsp_buff + offset_eo + 2 * st_stride
	%define EO_3	rsp_buff + offset_eo + 3 * st_stride
	%define EO_4	rsp_buff + offset_eo + 4 * st_stride
	%define EO_5	rsp_buff + offset_eo + 5 * st_stride
	%define EO_6	rsp_buff + offset_eo + 6 * st_stride
	%define EO_7	rsp_buff + offset_eo + 7 * st_stride
	
	%define	offset_ee	24 * st_stride
	%define EE_0	rsp_buff + offset_ee + 0*st_stride
	%define EE_7	rsp_buff + offset_ee + 4*st_stride
	%define EE_3	rsp_buff + offset_ee + 3*st_stride
	%define EE_4	rsp_buff + offset_ee + 6*st_stride
	%define EE_1	rsp_buff + offset_ee + 1*st_stride
	%define EE_6	rsp_buff + offset_ee + 5*st_stride
	%define EE_2	rsp_buff + offset_ee + 2*st_stride
	%define EE_5	rsp_buff + offset_ee + 7*st_stride
	
	IDCT_E_SSE4	32, %1, %2
%endmacro

;input	%1 = EE_x
;		%2 = EO_x
;		%3 = sub dimension
;		%4 = tr_width

%macro IDCT32X32_E_2_LINES	4
	%define sub_dim		%3
	%define tr_width	%4
	%define EE_X		%1
	%define EO_X		%2
	
	mova	m0,	[EE_X + 0*16]
	mova	m1,	[EO_X + 0*16]
	mova	m2,	m0
	paddd	m0,	m1	;m0 = EE_x + EO_x
	psubd	m2,	m1	;m2 = EE_x - EO_x
	mova	[EE_X + 0*16],	m0
	mova	[EO_X + 0*16],	m2
	
	%if tr_width >= 8
		mova	m0,	[EE_X + 1*16]
		mova	m1,	[EO_X + 1*16]
		mova	m2,	m0
		paddd	m0,	m1	;m0 = EE_x + EO_x
		psubd	m2,	m1	;m2 = EE_x - EO_x
		mova	[EE_X + 1*16],	m0
		mova	[EO_X + 1*16],	m2
		
		%if tr_width >= 16
			mova	m0,	[EE_X + 2*16]
			mova	m1,	[EO_X + 2*16]
			mova	m2,	m0
			paddd	m0,	m1	;m0 = EE_x + EO_x
			psubd	m2,	m1	;m2 = EE_x - EO_x
			mova	[EE_X + 2*16],	m0
			mova	[EO_X + 2*16],	m2
			
			mova	m0,	[EE_X + 3*16]
			mova	m1,	[EO_X + 3*16]
			mova	m2,	m0
			paddd	m0,	m1	;m0 = EE_x + EO_x
			psubd	m2,	m1	;m2 = EE_x - EO_x
			mova	[EE_X + 3*16],	m0
			mova	[EO_X + 3*16],	m2
		
			%if tr_width >= 32
				mova	m0,	[EE_X + 4*16]
				mova	m1,	[EO_X + 4*16]
				mova	m2,	m0
				paddd	m0,	m1	;m0 = EE_x + EO_x
				psubd	m2,	m1	;m2 = EE_x - EO_x
				mova	[EE_X + 4*16],	m0
				mova	[EO_X + 4*16],	m2
				
				mova	m0,	[EE_X + 5*16]
				mova	m1,	[EO_X + 5*16]
				mova	m2,	m0
				paddd	m0,	m1	;m0 = EE_x + EO_x
				psubd	m2,	m1	;m2 = EE_x - EO_x
				mova	[EE_X + 5*16],	m0
				mova	[EO_X + 5*16],	m2
				
				mova	m0,	[EE_X + 6*16]
				mova	m1,	[EO_X + 6*16]
				mova	m2,	m0
				paddd	m0,	m1	;m0 = EE_x + EO_x
				psubd	m2,	m1	;m2 = EE_x - EO_x
				mova	[EE_X + 6*16],	m0
				mova	[%2 + 6*16],	m2
				
				mova	m0,	[EE_X + 7*16]
				mova	m1,	[EO_X + 7*16]
				mova	m2,	m0
				paddd	m0,	m1	;m0 = EE_x + EO_x
				psubd	m2,	m1	;m2 = EE_x - EO_x
				mova	[EE_X + 7*16],	m0
				mova	[EO_X + 7*16],	m2
			%endif
		%endif
	%endif
%endmacro

;input	%1 = E_x
;		%2 = O_x
;		%3 = add_shift
;		%4 = tr_width

%macro IDCT_RECON_2_LINES	4
	%assign %%Part_No	0
	%define	PARTS %4/4
	%rep PARTS
		mova		m0,	[%1 + %%Part_No*16]
		mova		m1,	m0
		mova		m2,	[%2 + %%Part_No*16]
		paddd		m0,	m2		;recon[     x]	= E_x + O_x
		psubd		m1,	m2		;recon[15 - x]	= E_x - O_x
		
		paddd		m0,	%3
		paddd		m1,	%3
		psrad		m0,	const_im_shr
		psrad		m1,	const_im_shr
		
		packssdw	m0,	m0
		packssdw	m1,	m1
		movq		[%1 + %%Part_No*8], m0
		movq		[%2 + %%Part_No*8], m1
		%assign %%Part_No	%%Part_No + 1
	%endrep
	
%endmacro

;input	%1 = Dimenson	32 for 32x32  16 for 16x16
;		%2 = tr_width
%macro IDCT_COMPOSE	2
	%define dim_main	%1
	%define tr_width	%2
	%if const_im_shr == 7
		mova	m7,	[CONST_U32_POS64]		;m7 = 64
	%elif const_im_shr == 12
		mova	m7,	[CONST_U32_POS_0X0800]	;m7 = 0x800
	%endif
	
	%assign %%Line_No	0
	%define LINES	8 * dim_main / 16
	%rep LINES
		IDCT_RECON_2_LINES EVEN_ %+ %%Line_No, ODD_ %+ %%Line_No, m7,tr_width
		%assign %%Line_No	%%Line_No + 1
	%endrep

%endmacro

;input	%1 = tr_width
%macro IDCT32X32_COMPOSE	1
	%define ODD_0		rsp_buff + 0 * st_stride
	%define ODD_1		rsp_buff + 1 * st_stride
	%define ODD_2		rsp_buff + 2 * st_stride
	%define ODD_3		rsp_buff + 3 * st_stride
	%define ODD_4		rsp_buff + 4 * st_stride
	%define ODD_5		rsp_buff + 5 * st_stride
	%define ODD_6		rsp_buff + 6 * st_stride
	%define ODD_7		rsp_buff + 7 * st_stride
	%define ODD_8		rsp_buff + 8 * st_stride
	%define ODD_9		rsp_buff + 9 * st_stride
	%define ODD_10		rsp_buff + 10 * st_stride
	%define ODD_11		rsp_buff + 11 * st_stride
	%define ODD_12		rsp_buff + 12 * st_stride
	%define ODD_13		rsp_buff + 13 * st_stride
	%define ODD_14		rsp_buff + 14 * st_stride
	%define ODD_15		rsp_buff + 15 * st_stride
	
	%define EVEN_0		EE_0
	%define EVEN_1		EE_1
	%define EVEN_2		EE_2
	%define EVEN_3		EE_3
	%define EVEN_4		EE_4
	%define EVEN_5		EE_5
	%define EVEN_6		EE_6
	%define EVEN_7		EE_7
	
	%define EVEN_8		EO_7
	%define EVEN_9		EO_6
	%define EVEN_10		EO_5
	%define EVEN_11		EO_4
	%define EVEN_12		EO_3
	%define EVEN_13		EO_2
	%define EVEN_14		EO_1
	%define EVEN_15		EO_0
	
	IDCT_COMPOSE	32,	%1
	
%endmacro

%macro	cat_define	3-4
	%if %0 == 3
		%xdefine %1%2 %3
	%else
		%xdefine %1%2 %3%4
	%endif
%endmacro

;input	%1 = main dimenson	32 for 32x32  16 for 16x16
;		%2 = sub dimension
%macro	IDCT_TRANSPOSE	2
	%define	size_rate_x	(%2/8)
	%define	size_rate_y	(%1/8)
	
	%if %1 == %2
		%define	dst_stride	4*%1
	%else
		%define	dst_stride	2*%1
	%endif
	
	%define	src_transform	rsp_buff
	%if %1 == %2
		%define	dest_transform	rsp_buff + 2*%2
	%else
		%define	dest_transform	rsp_buff + 4*%1*%2
	%endif
	
	%assign	%%even_no	0
	%assign	%%odd_no	4*size_rate_y - 1
	%assign	%%LINE_NO_UP	0
	%assign	%%LINE_NO_DOWN	4*size_rate_y
	
	%define	LINES	4*size_rate_y
	%rep LINES
		cat_define SRC_, %%LINE_NO_UP,	 EVEN_, %%even_no
		cat_define SRC_, %%LINE_NO_DOWN, ODD_,  %%odd_no
		
		cat_define DEST_, %%LINE_NO_UP,   dest_transform + %%LINE_NO_UP * dst_stride
		cat_define DEST_, %%LINE_NO_DOWN, dest_transform + (%%LINE_NO_UP + 4*size_rate_x) * dst_stride
		
		%assign	%%even_no  %%even_no + 1
		%assign	%%odd_no   %%odd_no - 1
		%assign	%%LINE_NO_UP	%%LINE_NO_UP + 1
		%assign	%%LINE_NO_DOWN	%%LINE_NO_DOWN + 1
	%endrep
	
	%assign	%%BLOCK8X8_NO	0
	%assign	%%BLOCK8X8_NO_X	0
	%assign	%%BLOCK8X8_NO_Y	0
	%define	BLOCK8X8_NUM	size_rate_x*size_rate_y
	%rep BLOCK8X8_NUM
		%assign	%%BLOCK8X8_NO_X	%%BLOCK8X8_NO % size_rate_x
		%assign	%%BLOCK8X8_NO_Y	%%BLOCK8X8_NO / size_rate_x
		
		IDCT_B8X8_TRANSPOSE	%%BLOCK8X8_NO_X,%%BLOCK8X8_NO_Y,src_transform,st_stride,dest_transform,st_stride
		
		%assign	%%BLOCK8X8_NO	%%BLOCK8X8_NO + 1
	%endrep
	
%endmacro

;input	%1 = sub dimension
%macro	IDCT32X32_TRANSPOSE	1
	%define ODD_0		rsp_buff + 0 * st_stride
	%define ODD_1		rsp_buff + 1 * st_stride
	%define ODD_2		rsp_buff + 2 * st_stride
	%define ODD_3		rsp_buff + 3 * st_stride
	%define ODD_4		rsp_buff + 4 * st_stride
	%define ODD_5		rsp_buff + 5 * st_stride
	%define ODD_6		rsp_buff + 6 * st_stride
	%define ODD_7		rsp_buff + 7 * st_stride
	
	%define ODD_8		rsp_buff + 8 * st_stride
	%define ODD_9		rsp_buff + 9 * st_stride
	%define ODD_10		rsp_buff + 10 * st_stride
	%define ODD_11		rsp_buff + 11 * st_stride
	%define ODD_12		rsp_buff + 12 * st_stride
	%define ODD_13		rsp_buff + 13 * st_stride
	%define ODD_14		rsp_buff + 14 * st_stride
	%define ODD_15		rsp_buff + 15 * st_stride
	
	%define EVEN_0		EE_0
	%define EVEN_1		EE_1
	%define EVEN_2		EE_2
	%define EVEN_3		EE_3
	%define EVEN_4		EE_4
	%define EVEN_5		EE_5
	%define EVEN_6		EE_6
	%define EVEN_7		EE_7
	
	%define EVEN_8		EO_7
	%define EVEN_9		EO_6
	%define EVEN_10		EO_5
	%define EVEN_11		EO_4
	%define EVEN_12		EO_3
	%define EVEN_13		EO_2
	%define EVEN_14		EO_1
	%define EVEN_15		EO_0
	
	IDCT_TRANSPOSE	32, %1
	
%endmacro

;input	%1 = block8x8_x
;		%2 = block8x8_y
;		%3 = src
;		%4 = src_stride
;		%5 = dest
;		%6 = dest_stride

%macro IDCT_B8X8_TRANSPOSE 6
	
	;%define	block_stride	2*8
	;%define	st_stride		4*32
	
	%define	src_bk_x	%1*8*2
	%define	src_bk_y	%2*8
	
	%define	dst_bk_x	%2*8*2
	%define	dst_bk_y	%1*8
	
	%define	predict_bk_x	%2*8
	%define	predict_bk_y	%1*8
	%define	recon_bk_x		%2*8
	%define	recon_bk_y		%1*8
	
	%assign	LINE_0	src_bk_y + 0
	%assign	LINE_1	src_bk_y + 1
	%assign	LINE_2	src_bk_y + 2
	%assign	LINE_3	src_bk_y + 3
	
	mova	m0,	[SRC_ %+ LINE_0 + src_bk_x]
	mova	m1,	[SRC_ %+ LINE_1 + src_bk_x]
	mova	m2,	[SRC_ %+ LINE_2 + src_bk_x]
	mova	m3,	[SRC_ %+ LINE_3 + src_bk_x]
	
	mova		m4,	m0
	punpcklwd	m0, m1	;m0 = a0 b0 a1 b1 a2 b2 a3 b3
	punpckhwd	m4, m1	;m4 = a4 b4 a5 b5 a6 b6 a7 b7
	
	mova		m1,	m2
	punpcklwd	m1, m3	;m1	= c0 d0 c1 d1 c2 d2 c3 d3
	punpckhwd	m2, m3	;m2 = c4 d4 c5 d5 c6 d6 c7 d7
	
	mova		m3,	m0
	punpckldq	m0,	m1	;m0 = a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m3,	m1	;m3 = a2 b2 c2 d2 a3 b3 c3 d3
	
	mova		m1,	m4
	punpckldq	m1,	m2	;m1 = a4 b4 c4 d4 a5 b5 c5 d5
	punpckhdq	m4,	m2	;m4 = a6 b6 c6 d6 a7 b7 c7 d7
	
	%assign	LINE_7	dst_bk_y + 7
	%define	tmp_buff	[DEST_ %+ LINE_7 + dst_bk_x]	;[%5 + (dst_bk_y + 7)*%6 + dst_bk_x]
	mova	tmp_buff,	m4	;tmp+buff = a6 b6 c6 d6 a7 b7 c7 d7
	
	;m2 m4 m5 m6 m7 are available
	%assign	LINE_4	src_bk_y + 4
	%assign	LINE_5	src_bk_y + 5
	mova	m2,	[SRC_ %+ LINE_4 + src_bk_x]
	mova	m4,	[SRC_ %+ LINE_5 + src_bk_x]
	
	mova		m5,	m2
	punpcklwd	m2, m4	;m2 = e0 f0 e1 f1 e2 f2 e3 f3
	punpckhwd	m5, m4	;m5 = e4 f4 e5 f5 e6 f6 e7 f7
	
	%assign	LINE_6	src_bk_y + 6
	%assign	LINE_7	src_bk_y + 7
	mova	m4,	[SRC_ %+ LINE_6 + src_bk_x]
	mova	m6,	[SRC_ %+ LINE_7 + src_bk_x]
	mova	m7,	m4		;
	punpcklwd	m4, m6	;m4 = g0 h0 g1 h1 g2 h2 g3 h3
	punpckhwd	m7, m6	;m7 = g4 h4 g5 h5 g6 h6 g7 h7
	
	mova		m6,	m2
	punpckldq	m2,	m4	;m2 = e0 f0 g0 h0 e1 f1 g1 h1
	punpckhdq	m6,	m4	;m6 = e2 f2 g2 h2 e3 f3 g3 h3
	
	mova		m4,	m5
	punpckldq	m4,	m7	;m4 = e4 f4 g4 h4 e5 f5 g5 h5
	punpckhdq	m5,	m7	;m5 = e6 f6 g6 h6 e7 f7 g7 h7
	
	mova		m7,	m0
	punpcklqdq	m0,	m2	;m0 = a0 b0 c0 d0 e0 f0 g0 h0
	punpckhqdq	m7,	m2	;m7 = a1 b1 c1 d1 e1 f1 g1 h1
	
	mova		m2,	m3
	punpcklqdq	m2,	m6	;m2 = a2 b2 c2 d2 e2 f2 g2 h2
	punpckhqdq	m3,	m6	;m3 = a3 b3 c3 d3 e3 f3 g3 h3
	
	mova		m6,	m1	
	punpcklqdq	m1,	m4	;m1 = a4 b4 c4 d4 e4 f4 g4 h4
	punpckhqdq	m6,	m4	;m6 = a5 b5 c5 d5 e5 f5 g5 h5
	
	%assign	LINE_0	dst_bk_y + 0
	%assign	LINE_1	dst_bk_y + 1
	%assign	LINE_2	dst_bk_y + 2
	%assign	LINE_3	dst_bk_y + 3
	%assign	LINE_4	dst_bk_y + 4
	%assign	LINE_5	dst_bk_y + 5
	%assign	LINE_6	dst_bk_y + 6
	%assign	LINE_7	dst_bk_y + 7
	
	%if	save_idct == 1
		mova	[DEST_ %+ LINE_0 + dst_bk_x], m0
		;m0 m4 are available
		
		mov			reg_tmp_0,		predict_stride
		imul		reg_tmp_0,		predict_bk_y + 0
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	[DEST_ %+ LINE_0 + dst_bk_x]
		packuswb	m0, m0
		
		mov			reg_tmp_1,		recon_stride
		imul		reg_tmp_1,		recon_bk_y + 0
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	m7
		packuswb	m0, m0
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	m2
		packuswb	m0, m0
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	m3
		packuswb	m0, m0
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	m1
		packuswb	m0, m0
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		pxor		m4, m4
		movq		m0,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m0, m4
		paddw		m0,	m6
		packuswb	m0, m0
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m0
	
	%else
		mova	[DEST_ %+ LINE_0 + dst_bk_x], m0
		mova	[DEST_ %+ LINE_1 + dst_bk_x], m7
		mova	[DEST_ %+ LINE_2 + dst_bk_x], m2
		mova	[DEST_ %+ LINE_3 + dst_bk_x], m3
		mova	[DEST_ %+ LINE_4 + dst_bk_x], m1
		mova	[DEST_ %+ LINE_5 + dst_bk_x], m6
	%endif
		
	mova		m0,	tmp_buff
	mova		m1,	m0		;m0 = a6 b6 c6 d6 a7 b7 c7 d7
	punpcklqdq	m0,	m5		;m0 = a6 b6 c6 d6 e6 f6 g6 h6
	punpckhqdq	m1, m5		;m1 = a7 b7 c7 d7 e7 f7 g7 h7
	
	%if	save_idct == 1
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		movq		m2,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m2, m4
		paddw		m2,	m0
		packuswb	m2, m2
		
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m2
		
		lea			reg_tmp_0,		[reg_tmp_0 + predict_stride]
		movq		m2,	[predict_buff + reg_tmp_0 + predict_bk_x]
		punpcklbw	m2, m4
		paddw		m2,	m1
		packuswb	m2, m2
		
		lea			reg_tmp_1,		[reg_tmp_1 + recon_stride]
		movq		[recon_buff + reg_tmp_1 + recon_bk_x], m2
	%else
		mova	[DEST_ %+ LINE_6 + dst_bk_x], m0
		mova	[DEST_ %+ LINE_7 + dst_bk_x], m1
	%endif
	
%endmacro

;input	%1 = sub dimension 8 16 32
;		%2 = tr_width
;		%3 = src
;		%4 = src_stride in unit of byte
;		%5 = dest
;		%6 = dest_stride  in unit of byte

%macro IDCT32X32_EE_SSE4 6
	IDCT_EO_SSE4	32,	%1,	%2, %3, %4, %5,	%6				;8 Lines
	IDCT_EEO_SSE4	32,	%1,	%2, %3, %4, %5 + 8*%6, %6		;4 Lines
	
	%if %1 >= 16
		IDCT_EEEO_SSE4	32,	%1,	%2, %3, %4, %5 + 12*%6,%6	;2 Lines
		IDCT_EEEE_SSE4	32,	%1,	%2, %3, %4, %5 + 14*%6,%6	;2 Lines
	%elif %1 == 8
		IDCT_32EX_EE_SSE4	32,	%1,	%2, %3, %4, %5 + 12*%6,%6	;2 Lines
	%endif
%endmacro

;VO_VOID IDCT32X32(const VO_S16 * p_invtransform_buff,
               ;VO_U8 * p_prediction_buff,
               ;const VO_U32 prediction_stride,
               ;VO_S16 * block_array)

INIT_XMM sse4
%macro IDCT32X32_8_16_32 1
%if %1 == 32
	cglobal IDCT32X32_X86
%else
	cglobal IDCT32X32_%1X%1_X86
%endif
	IDCT_FUNC_HEAD
	
	%define dim_main	32	;
	%define dim_sub		%1	;
	%define tr_width	%1
	
	;part 1:
	%define	st_part_1_width		4*%1
	%define	st_part_1_height	32
	
	%if dim_main == dim_sub
	;part 2:
		%define	st_part_2_width		4*32
		%define	st_part_2_height	%1
	;part 3:
		%define	st_part_3_width		st_part_2_width
		%define	st_part_3_height	st_part_2_height
		
		%define st_part_1_buff  rsp
		%define st_part_2_buff  rsp + st_part_1_width*st_part_1_height
		%define st_part_3_buff  st_part_2_buff
		
		%define	st_total_room	st_part_1_width*st_part_1_height \
							+ st_part_2_width*st_part_2_height
	
	%else
	;part 2:
		%define	st_part_2_width		2*32
		%define	st_part_2_height	%1
	;part 3:
		%define	st_part_3_width		4*32
		%define	st_part_3_height	32
		
		%define st_part_1_buff  rsp
		%define st_part_2_buff  rsp + st_part_1_width*st_part_1_height
		%define st_part_3_buff  st_part_2_buff +  + st_part_2_width*st_part_2_height
		
		%define	st_total_room	st_part_1_width*st_part_1_height \
							+ st_part_2_width*st_part_2_height\
							+ st_part_3_width*st_part_3_height
	%endif
	
	sub		rsp,	st_total_room	;
	and		rsp,	~15
	
	%define	rsp_buff	rsp
	%xdefine const_im_shr 7
	
	
	%define	rsp_buff	rsp
	%define st_stride	st_part_1_width
	%define src_buff	r0
	
	IDCT32X32_ODD_SSE4	dim_sub, tr_width, src_buff,	2*dim_main,  st_part_1_buff,	st_part_1_width
	%if dim_sub == 32
		IDCT32X32_ODD_SSE4	dim_sub, tr_width, src_buff + dim_main, 2*dim_main,  st_part_1_buff + st_part_1_width/2,	st_part_1_width
	%endif
	
	IDCT32X32_EE_SSE4	dim_sub, tr_width, src_buff,	2*dim_main,	st_part_1_buff + 16*st_part_1_width,  st_part_1_width
	%if dim_sub == 32
		IDCT32X32_EE_SSE4   dim_sub, tr_width, src_buff + dim_main, 2*dim_main,  st_part_1_buff + 16*st_part_1_width + st_part_1_width/2,st_part_1_width
	%endif
	
	IDCT32X32_E_SSE4	dim_sub, tr_width
	
	%xdefine const_im_shr 7
	IDCT32X32_COMPOSE	tr_width
	
	%define	save_idct	0
	IDCT_TRANSPOSE_ADAPTER 32, dim_sub
	
	%define tr_width	32
	%define	rsp_buff	st_part_3_buff
	%define st_stride	st_part_3_width
	%if dim_main == dim_sub
		%define src_stride	st_stride
		%define src_buff	rsp + 2*dim_main
	%else
		%define src_stride	st_stride/2
		%define src_buff	st_part_2_buff
	%endif
	
	IDCT32X32_ODD_SSE4	dim_sub, tr_width, src_buff,src_stride,	rsp_buff,	st_stride
	IDCT32X32_ODD_SSE4	dim_sub, tr_width, src_buff + dim_main,src_stride, rsp_buff + 2*dim_main,	st_stride
	
	IDCT32X32_EE_SSE4	dim_sub, tr_width, src_buff,src_stride, rsp_buff + 16*st_stride,	st_stride
	IDCT32X32_EE_SSE4	dim_sub, tr_width, src_buff + dim_main, src_stride, rsp_buff + 16*st_stride+2*dim_main,	st_stride
	
	IDCT32X32_E_SSE4	dim_sub, tr_width
	
	%xdefine const_im_shr 12
	IDCT32X32_COMPOSE	tr_width
	
	%define	reg_tmp_0		r4
	%define	reg_tmp_1		r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	
	%define	save_idct		1
	IDCT_TRANSPOSE_ADAPTER 32, dim_main
	
	IDCT_FUNC_FOOT
%endmacro

IDCT32X32_8_16_32 32
IDCT32X32_8_16_32 16
IDCT32X32_8_16_32 8

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimenson 32 for 32x32  16 for 16x16

%macro IDCT_EX_4X4_ODD_UP_SSE4 5

	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(8*%5/16)*8
	
	movq		m0,	[%1 + 1*%2]	;a0 a1 a2 a3
	movq		m1,	[%1 + 3*%2]	;b0 b1 b2 b3
	punpcklwd	m0,	m1			;m0 = a0 b0 a1 b1 a2 b2 a3 b3
	
	%if %5 == 32
		%define tr_coef_table CONST_TR_32x32
	%elif %5 == 16
		%define tr_coef_table CONST_TR_16x16_1
	%endif
	
	%assign %%LINE_NO	1
	%define LINES		8*%5/16
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coef_table + (%%LINE_NO - 1) * TR_COEF_STRIDE
		;-------------One line of odd------------
		
		mova		m5,	[TR_COEF_BUF]
		mova		m1,	m0
		pmaddwd	m1,	m5					;m0 = coef1 * a0 + coef2 * b0
		mova		[%3 + %4 * (%%LINE_NO - 1)],	m1
		
		;-------------One line of odd------------
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep
    
%endmacro


;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimension 32 or 16

%macro IDCT_EX_4X4_ODD_DOWN_SSE4 5

	IDCT_EX_4X4_ODD_DOWN_16PIX_SSE4	%1,%2,%3,%4,%5
	%if %5 == 32
		IDCT_EX_4X4_ODD_DOWN_16PIX_SSE4	%1 + 2*(8*%5/16),%2,%3 + 4*(8*%5/16),%4,%5
	%endif
    
%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimension 32 or 16

%macro IDCT_EX_4X4_ODD_DOWN_16PIX_SSE4 5

	%define TR_COEF_STRIDE		2*8
	%define TR_COEF_BLOCK_SIZE	2*(8*%5/16)*8
	
	movq		m0,	[%1 + 1*%2 + 0*8]	;a0 a1 a2 a3
	movq		m1,	[%1 + 3*%2 + 0*8]	;b0 b1 b2 b3
	punpcklwd	m0,	m1					;m0 = a0 b0 a1 b1 a2 b2 a3 b3
	
	movq		m1,	[%1 + 1*%2 + 1*8]	;a4 a5 a6 a7
	movq		m2,	[%1 + 3*%2 + 1*8]	;b4 b5 b6 b7
	punpcklwd	m1,	m2					;m1 = a4 b4 a5 b5 a6 b6 a7 b7
	
	movq		m2,	[%1 + 1*%2 + 2*8]	;a8 a9 a10 a11
	movq		m3,	[%1 + 3*%2 + 2*8]	;b8 b9 b10 b11
	punpcklwd	m2,	m3					;m2 = a8 b8 a9 b9 a10 b10 a11 b11
	
	movq		m3,	[%1 + 1*%2 + 3*8]	;a13 a14 a15
	movq		m4,	[%1 + 3*%2 + 3*8]	;b9 b10 b11 b12
	punpcklwd	m3,	m4					;m3 = a8 b8 a9 b9 a10 b10 a11 b11
	
	%if %5 == 32
		%define tr_coef_table  CONST_TR_32x32
	%elif %5 == 16
		%define tr_coef_table  CONST_TR_16x16_1
	%endif
	
	%assign %%LINE_NO	1
	%define LINES		8*%5/16
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coef_table + (%%LINE_NO - 1) * TR_COEF_STRIDE
		;-------------One line of odd------------
		
		mova		m5,	[TR_COEF_BUF]
		mova		m6,	m0
		pmaddwd	m6,	m5					;m6 = coef1 * a0 + coef2 * b0
		mova		[%3 + %4 * (%%LINE_NO - 1) + 0*16],	m6
		
		mova		m6,	m1
		pmaddwd	m6,	m5					
		mova		[%3 + %4 * (%%LINE_NO - 1) + 1*16],	m6
		
		mova		m6,	m2
		pmaddwd	m6,	m5					
		mova		[%3 + %4 * (%%LINE_NO - 1) + 2*16],	m6
		
		mova		m6,	m3
		pmaddwd	m6,	m5					
		mova		[%3 + %4 * (%%LINE_NO - 1) + 3*16],	m6
		
		;-------------One line of odd------------
		%assign %%LINE_NO %%LINE_NO + 1
	%endrep
    
%endmacro


;input	%1 = part_no
;		%2 = src
;		%3 = src_stride in unit of byte
;		%4 = dest
;		%5 = dest_stride  in unit of byte
;		%6 = Dimenson 32 for D32X32  16 for 16X16

%macro IDCT_EX_4X4_E_O_BLEND_4PIX_SSE4 6
	pxor		m4,	m4
	;--------midValue = 64 * p_invtransform[0]--------
	movq		m5,	[%2 + 8*%1]
	punpcklwd	m5,	m4
	mova		m6,	[CONST_U32_POS64]
	pmaddwd	m5,	m6		;m5  = midValue
	;--------midValue = 64 * p_invtransform[0]--------
	
	%if const_im_shr == 7
		vo_exchange	6,7
		;mova	m7,	[CONST_U32_POS64]		;m7 = 64
	%elif const_im_shr == 12
		mova	m7,	[CONST_U32_POS_0X0800]	;m7 = 0x800
	%endif

	%define TR_COEF_STRIDE	8
	%if %6 == 32
		%define tr_coef_table CONST_TR_32X32_2ND_ROW
	%elif %6 == 16
		%define tr_coef_table CONST_TR_16X16_2ND_ROW
	%endif
	
	%define even_offset	(8*%6/16)*%5
	%define	EVEN_buf	%4 + even_offset
	%define	ODD_buf		%4
	%define max_index	8*%6/16 - 1
	
	movq		m0,	[%2 + 2*%3 + 8*%1]	;a0 a1 a2 a3
	punpcklwd	m0,	m4
	
	%assign %%LINE_NO	0
	%define LINES		4*%6/16
	
	%rep LINES
		%define	TR_COEF_BUF	tr_coef_table + %%LINE_NO * TR_COEF_STRIDE
		
		movq		m1,	[TR_COEF_BUF]
		punpcklwd	m1,	m4
		pmaddwd	m1,	m0			;m1 = coef1 * a0 + coef2 * b0
		mova		m2,	m5
		mova		m3,	m5
		paddd		m2,	m1			;m2 = EVEN_ %+ %%LINE_NO
		psubd		m3,	m1			;m3 = EVEN_ %+ (max_index - %%LINE_NO)
		
		mova		m1,	[ODD_buf + %%LINE_NO * %5 + 16*%1]
		mova		m6,	m2
		paddd		m2,	m1			;m2 = EVEN_x + ODD_x
		psubd		m6,	m1			;m6 = EVEN_x - ODD_x
		paddd		m2,	m7
		paddd		m6,	m7
		psrad		m2,	const_im_shr	;m2 = (EVEN_x + ODD_x + add) >> const_im_shr
		psrad		m6,	const_im_shr	;m6 = (EVEN_x - ODD_x + add) >> const_im_shr
		packssdw	m2,	m2
		packssdw	m6,	m6
		
		movq		[ODD_buf  +	%%LINE_NO * %5 + 8*%1],	m2	;Line[x]						x				= %%LINE_NO
		movq		[EVEN_buf + %%LINE_NO * %5 + 8*%1],	m6	;Line[2*max_index + 1 - x]	2*max_index + 1 - x	= 2*max_index + 1 - %%LINE_NO
		
		mova		m1,	[ODD_buf + (max_index - %%LINE_NO) * %5 + 16*%1]
		mova		m6,	m3
		paddd		m3,	m1			;m3 = EVEN_y + ODD_y
		psubd		m6,	m1			;m6 = EVEN_y - ODD_y
		paddd		m3,	m7
		paddd		m6,	m7
		psrad		m3,	const_im_shr	;m3 = (EVEN_y + ODD_y + add) >> const_im_shr
		psrad		m6,	const_im_shr	;m6 = (EVEN_y - ODD_y + add) >> const_im_shr
		packssdw	m3,	m3
		packssdw	m6,	m6
		
		movq		[ODD_buf  +	(max_index - %%LINE_NO) * %5 + 8*%1],	m3	;Line[y]								y		 = max_index - %%LINE_NO
		movq		[EVEN_buf + (max_index - %%LINE_NO) * %5 + 8*%1],	m6	;Line[2*max_index + 1 - y]	2*max_index + 1 - y  = max_index + 1 + %%LINE_NO
		
	%assign %%LINE_NO %%LINE_NO + 1
	%endrep

%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimenson 32 for D32X32  16 for D16X16

%macro IDCT_EX_4X4_E_O_BLEND_UP_SSE4 5

	IDCT_EX_4X4_E_O_BLEND_4PIX_SSE4	0,%1,%2,%3,%4,%5
	
%endmacro


;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimension 32 or 16

%macro IDCT_EX_4X4_E_O_BLEND_DOWN_SSE4 5

	%assign %%LINE_NO 0
	%define LINES 4*%5/16
	
	%rep LINES
		IDCT_EX_4X4_E_O_BLEND_4PIX_SSE4	%%LINE_NO,%1,%2,%3,%4,%5
		%assign %%LINE_NO %%LINE_NO + 1
	%endrep
	
%endmacro


;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimenson 32 for D32X32  16 for D16X16

%macro IDCT_EX_4X4_HX4_4XH_TRANSPOSE 5

	%assign %%Block_No	0
	%define	BLOCK_8X4_NUM 2*%5/16
	%rep  BLOCK_8X4_NUM
		%assign	Src_Line_0	8*%%Block_No + 0
		%assign	Src_Line_1	8*%%Block_No + 1
		%assign	Src_Line_2	8*%%Block_No + 2
		%assign	Src_Line_3	8*%%Block_No + 3
		%assign	Src_Line_4	8*%%Block_No + 4
		%assign	Src_Line_5	8*%%Block_No + 5
		%assign	Src_Line_6	8*%%Block_No + 6
		%assign	Src_Line_7	8*%%Block_No + 7
		
		%define	Dst_Line_0	%3 + 0 * %4 + 16 * %%Block_No
		%define	Dst_Line_1	%3 + 1 * %4 + 16 * %%Block_No
		%define	Dst_Line_2	%3 + 2 * %4 + 16 * %%Block_No 
		%define	Dst_Line_3	%3 + 3 * %4 + 16 * %%Block_No

		movq	m0,	[Line_ %+ Src_Line_0]	;a0 a1 a2 a3
		movq	m1,	[Line_ %+ Src_Line_1]	;b0 b1 b2 b3
		movq	m2,	[Line_ %+ Src_Line_2]	;c0 c1 c2 c3
		movq	m3,	[Line_ %+ Src_Line_3]	;d0 d1 d2 d3
		movq	m4,	[Line_ %+ Src_Line_4]	;
		movq	m5,	[Line_ %+ Src_Line_5]	;
		movq	m6,	[Line_ %+ Src_Line_6]	;
		movq	m7,	[Line_ %+ Src_Line_7]	;
		
		punpcklwd	m0,	m1			;m0 = a0 b0 a1 b1 a2 b3 a3 b3
		punpcklwd	m2,	m3			;m2 = c0 d0 c1 d1 c2 d2 c3 d3
		punpcklwd	m4,	m5			;m4 = e0 f0 e1 f1 e2 f2 e3 f3
		punpcklwd	m6,	m7			;m6 = g0 h0 g1 h1 g2 h2 g3 h3
		
		mova		m1,	m0
		punpckldq	m0,	m2		;m0 = a0 b0 c0 d0 a1 b1 c1 d1
		punpckhdq	m1,	m2		;m1 = a2 b3 c2 d2 a3 b3 c3 d3
		mova		m2,	m4
		punpckldq	m2,	m6		;m2 = e0 f0 g0 h0 e1 f1 g1 h1
		punpckhdq	m4,	m6		;m4 = e2 f2 g2 h2 e3 f3 g3 h3
		mova		m6,	m0
		punpcklqdq	m0,	m2		;m0 = a0 b0 c0 d0 e0 f0 g0 h0
		punpckhqdq	m6,	m2		;m6 = a1 b1 c1 d1 e1 f1 g1 h1
		mova		m2,	m1
		punpcklqdq	m1,	m4		;m1 = a2 b3 c2 d2 e2 f2 g2 h2
		punpckhqdq	m2,	m4		;m2 = a3 b3 c3 d3 e3 f3 g3 h3
		
		mova	[Dst_Line_0],	m0
		mova	[Dst_Line_1],	m6
		mova	[Dst_Line_2],	m1
		mova	[Dst_Line_3],	m2
		
		%assign %%Block_No %%Block_No + 1
	%endrep
	
%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte

%macro IDCT32X32_4X4_32X4_4X32_TRANSPOSE_UP 4	;it is in need to be checked again carefully
	%define	Line_0	%1 + 0 * %2
	%define	Line_1	%1 + 1 * %2
	%define	Line_2	%1 + 2 * %2
	%define	Line_3	%1 + 3 * %2
	%define	Line_4	%1 + 4 * %2
	%define	Line_5	%1 + 5 * %2
	%define	Line_6	%1 + 6 * %2
	%define	Line_7	%1 + 7 * %2
	%define	Line_8	%1 + 8 * %2
	%define	Line_9	%1 + 9 * %2
	%define	Line_10	%1 + 10 * %2
	%define	Line_11	%1 + 11 * %2
	%define	Line_12	%1 + 12 * %2
	%define	Line_13	%1 + 13 * %2
	%define	Line_14	%1 + 14 * %2
	%define	Line_15	%1 + 15 * %2
	%define	Line_16	%1 + 31 * %2
	%define	Line_17	%1 + 30 * %2
	%define	Line_18	%1 + 29 * %2
	%define	Line_19	%1 + 28 * %2
	%define	Line_20	%1 + 27 * %2
	%define	Line_21	%1 + 26 * %2
	%define	Line_22	%1 + 25 * %2
	%define	Line_23	%1 + 24 * %2
	%define	Line_24	%1 + 23 * %2
	%define	Line_25	%1 + 22 * %2
	%define	Line_26	%1 + 21 * %2
	%define	Line_27	%1 + 20 * %2
	%define	Line_28	%1 + 19 * %2
	%define	Line_29	%1 + 18 * %2
	%define	Line_30	%1 + 17 * %2
	%define	Line_31	%1 + 16 * %2
	
	IDCT_EX_4X4_HX4_4XH_TRANSPOSE %1,%2,%3,%4,32
	
%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte

%macro IDCT16X16_4X4_16X4_4X16_TRANSPOSE_UP 4	;it is in need to be checked again carefully
	%define	Line_0	%1 + 0 * %2
	%define	Line_1	%1 + 1 * %2
	%define	Line_2	%1 + 2 * %2
	%define	Line_3	%1 + 3 * %2
	%define	Line_4	%1 + 4 * %2
	%define	Line_5	%1 + 5 * %2
	%define	Line_6	%1 + 6 * %2
	%define	Line_7	%1 + 7 * %2
	
	%define	Line_8	%1 + 15 * %2
	%define	Line_9	%1 + 14 * %2
	%define	Line_10	%1 + 13 * %2
	%define	Line_11	%1 + 12 * %2
	%define	Line_12	%1 + 11 * %2
	%define	Line_13	%1 + 10 * %2
	%define	Line_14	%1 +  9 * %2
	%define	Line_15	%1 +  8 * %2
	
	IDCT_EX_4X4_HX4_4XH_TRANSPOSE %1,%2,%3,%4,16
	
%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimension 32 for D32X32  16 for D16X16

%macro IDCT_EX_4X4_TRANSPOSE_UP 5
	%if %5 == 32
		IDCT32X32_4X4_32X4_4X32_TRANSPOSE_UP %1,%2,%3,%4
	%elif %5 == 16
		IDCT16X16_4X4_16X4_4X16_TRANSPOSE_UP %1,%2,%3,%4
	%endif
%endmacro

;input	%1 = src
;		%2 = src_stride
;		%3 = dest
;		%4 = dest_stride
;		%5 = Dimension 32 or 16

%macro	IDCT_EX_4X4_TRANSPOSE_DN	5

	;%define	st_stride	4*32
	%define	src_transform	%1
	%define	dest_transform	%3
	
	%assign	%%line_no	0
	%define	LINES	16*%5/16
	%rep LINES
		cat_define SRC_,  %%line_no, Line_, %%line_no
		cat_define DEST_, %%line_no, dest_transform + %%line_no * %4
		%assign	%%line_no	%%line_no + 1
	%endrep
	
	%assign	%%BLOCK8X8_NO	0
	%assign	%%BLOCK8X8_NO_X	0
	%assign	%%BLOCK8X8_NO_Y	0
	%define	BLOCK8X8_NUM	(2*%5/16)*(2*%5/16)
	%rep BLOCK8X8_NUM
		%assign	%%BLOCK8X8_NO_X	%%BLOCK8X8_NO % (2*%5/16)
		%assign	%%BLOCK8X8_NO_Y	%%BLOCK8X8_NO / (2*%5/16)
		
		IDCT_B8X8_TRANSPOSE	%%BLOCK8X8_NO_X,%%BLOCK8X8_NO_Y,src_transform,%2,dest_transform,%4
		
		%assign	%%BLOCK8X8_NO	%%BLOCK8X8_NO + 1
	%endrep

%endmacro

;input	%1 = src
;		%2 = src_stride
;		%3 = dest
;		%4 = dest_stride

%macro	IDCT32X32_4X4_TRANSPOSE_DOWN	4
	
	%define	Line_0	%1 + 0 * %2
	%define	Line_1	%1 + 1 * %2
	%define	Line_2	%1 + 2 * %2
	%define	Line_3	%1 + 3 * %2
	%define	Line_4	%1 + 4 * %2
	%define	Line_5	%1 + 5 * %2
	%define	Line_6	%1 + 6 * %2
	%define	Line_7	%1 + 7 * %2
	%define	Line_8	%1 + 8 * %2
	%define	Line_9	%1 + 9 * %2
	%define	Line_10	%1 + 10 * %2
	%define	Line_11	%1 + 11 * %2
	%define	Line_12	%1 + 12 * %2
	%define	Line_13	%1 + 13 * %2
	%define	Line_14	%1 + 14 * %2
	%define	Line_15	%1 + 15 * %2
	%define	Line_16	%1 + 31 * %2
	%define	Line_17	%1 + 30 * %2
	%define	Line_18	%1 + 29 * %2
	%define	Line_19	%1 + 28 * %2
	%define	Line_20	%1 + 27 * %2
	%define	Line_21	%1 + 26 * %2
	%define	Line_22	%1 + 25 * %2
	%define	Line_23	%1 + 24 * %2
	%define	Line_24	%1 + 23 * %2
	%define	Line_25	%1 + 22 * %2
	%define	Line_26	%1 + 21 * %2
	%define	Line_27	%1 + 20 * %2
	%define	Line_28	%1 + 19 * %2
	%define	Line_29	%1 + 18 * %2
	%define	Line_30	%1 + 17 * %2
	%define	Line_31	%1 + 16 * %2
	
	IDCT_EX_4X4_TRANSPOSE_DN	%1,%2,%3,%4,32
	
%endmacro

;input	%1 = src
;		%2 = src_stride
;		%3 = dest
;		%4 = dest_stride

%macro	IDCT16X16_4X4_TRANSPOSE_DOWN	4

	%define	Line_0	%1 + 0 * %2
	%define	Line_1	%1 + 1 * %2
	%define	Line_2	%1 + 2 * %2
	%define	Line_3	%1 + 3 * %2
	%define	Line_4	%1 + 4 * %2
	%define	Line_5	%1 + 5 * %2
	%define	Line_6	%1 + 6 * %2
	%define	Line_7	%1 + 7 * %2
	
	%define	Line_8	%1 + 15 * %2
	%define	Line_9	%1 + 14 * %2
	%define	Line_10	%1 + 13 * %2
	%define	Line_11	%1 + 12 * %2
	%define	Line_12	%1 + 11 * %2
	%define	Line_13	%1 + 10 * %2
	%define	Line_14	%1 +  9 * %2
	%define	Line_15	%1 +  8 * %2

	IDCT_EX_4X4_TRANSPOSE_DN	%1,%2,%3,%4,16
	
%endmacro

;input	%1 = src
;		%2 = src_stride
;		%3 = dest
;		%4 = dest_stride
;		%5 = Dimension 32 or 16

%macro	IDCT_EX_4X4_TRANSPOSE_DOWN	5

	%if %5 == 32
		IDCT32X32_4X4_TRANSPOSE_DOWN %1,%2,%3,%4
	%elif %5 == 16
		IDCT16X16_4X4_TRANSPOSE_DOWN %1,%2,%3,%4
	%endif
	
%endmacro

;VO_VOID IDCT32X32_4X4(const VO_S16 * p_invtransform_buff,
                       ;const VO_U8 * p_prediction_buff,
                       ;const VO_U32 prediction_stride,
                       ;VO_S16 * block_array)
%macro IDCT_EX_4X4_X86	1
cglobal IDCT%1X%1_4X4_X86
	IDCT_FUNC_HEAD
	;part 1 : 32H * 4V
	%define	st_part_1_width		4*4
	%define	st_part_1_height	%1
	;part 2 : 4H * 32V
	%define	st_part_2_width		2*%1
	%define	st_part_2_height	4
	;part 3 : 32H * 32V
	%define	st_part_3_width		4*%1
	%define	st_part_3_height	%1
	
	%define st_part_1_buff  rsp
	%define st_part_2_buff  rsp + st_part_1_width*st_part_1_height
	%define st_part_3_buff  st_part_2_buff +  + st_part_2_width*st_part_2_height
	
	%define	st_total_room	st_part_1_width*st_part_1_height \
							+ st_part_2_width*st_part_2_height\
							+ st_part_3_width*st_part_3_height
							
	%define dim_main	%1	;32 for D32X32  16 for D16X16
	
	sub		rsp,	st_total_room	;
	and		rsp,	~15
	
	%define	rsp_buff	rsp
	%xdefine const_im_shr 7
	
	IDCT_EX_4X4_ODD_UP_SSE4			r0, 2*%1, st_part_1_buff, st_part_1_width, dim_main
	IDCT_EX_4X4_E_O_BLEND_UP_SSE4	r0, 2*%1, st_part_1_buff, st_part_1_width, dim_main
	IDCT_EX_4X4_TRANSPOSE_UP		st_part_1_buff, st_part_1_width, st_part_2_buff,st_part_2_width, dim_main
	
	%xdefine const_im_shr 12
	IDCT_EX_4X4_ODD_DOWN_SSE4		st_part_2_buff, st_part_2_width, st_part_3_buff,st_part_3_width, dim_main
	IDCT_EX_4X4_E_O_BLEND_DOWN_SSE4	st_part_2_buff, st_part_2_width, st_part_3_buff,st_part_3_width, dim_main
	
	%define	reg_tmp_0		r4
	%define	reg_tmp_1		r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	
	%define	save_idct		1
	IDCT_EX_4X4_TRANSPOSE_DOWN	st_part_3_buff,st_part_3_width, st_part_3_buff + 32*dim_main/16,st_part_3_width, dim_main
	
	IDCT_FUNC_FOOT
%endmacro

INIT_XMM sse4
IDCT_EX_4X4_X86	32
IDCT_EX_4X4_X86	16

;input	%1 = dim_sub
;		%2 = tr_width
;		%3 = src
;		%4 = src_stride in unit of byte
;		%5 = dest
;		%6 = dest_stride  in unit of byte

%macro IDCT16X16_ODD_SSE4 6
	IDCT_ODD_SSE4 16,%1,%2,%3,%4,%5,%6
%endmacro

;input	%1 = dim_sub
;		%2 = tr_width
;		%3 = src
;		%4 = src_stride in unit of byte
;		%5 = dest
;		%6 = dest_stride  in unit of byte

%macro IDCT16X16_EE_SSE4 6

	IDCT_EO_SSE4	16, %1, %2, %3, %4, %5,	%6				;4 Lines
	%if	dim_sub == 16
		IDCT_EEO_SSE4	16, %1, %2, %3, %4, %5 + 4*%6, %6	;2 Lines
		IDCT_EEE_SSE4	%3, %4, %5 + 6*%6, %6, 16			;2 Lines
	%else
		
	%endif
	
%endmacro

;input	%1 = tr_width
%macro IDCT16X16_COMPOSE 1
	%define ODD_0		rsp_buff + 0 * st_stride
	%define ODD_1		rsp_buff + 1 * st_stride
	%define ODD_2		rsp_buff + 2 * st_stride
	%define ODD_3		rsp_buff + 3 * st_stride
	%define ODD_4		rsp_buff + 4 * st_stride
	%define ODD_5		rsp_buff + 5 * st_stride
	%define ODD_6		rsp_buff + 6 * st_stride
	%define ODD_7		rsp_buff + 7 * st_stride
	
	%define	offset_even	8 * st_stride
	
	%define EVEN_0	rsp_buff + offset_even + 0 * st_stride
	%define EVEN_1	rsp_buff + offset_even + 1 * st_stride
	%define EVEN_2	rsp_buff + offset_even + 2 * st_stride
	%define EVEN_3	rsp_buff + offset_even + 3 * st_stride
	
	%define EVEN_4	rsp_buff + offset_even + 6 * st_stride
	%define EVEN_5	rsp_buff + offset_even + 7 * st_stride
	%define EVEN_6	rsp_buff + offset_even + 5 * st_stride
	%define EVEN_7	rsp_buff + offset_even + 4 * st_stride
	
	IDCT_COMPOSE	16, %1
	
%endmacro

;input	%1 = main dimension 16 8 4
%macro IDCT16X16_TRANSPOSE 1
	%define ODD_0		rsp_buff + 0 * st_stride
	%define ODD_1		rsp_buff + 1 * st_stride
	%define ODD_2		rsp_buff + 2 * st_stride
	%define ODD_3		rsp_buff + 3 * st_stride
	%define ODD_4		rsp_buff + 4 * st_stride
	%define ODD_5		rsp_buff + 5 * st_stride
	%define ODD_6		rsp_buff + 6 * st_stride
	%define ODD_7		rsp_buff + 7 * st_stride
	
	%define	offset_even	8 * st_stride
	
	%define EVEN_0	rsp_buff + offset_even + 0 * st_stride
	%define EVEN_1	rsp_buff + offset_even + 1 * st_stride
	%define EVEN_2	rsp_buff + offset_even + 2 * st_stride
	%define EVEN_3	rsp_buff + offset_even + 3 * st_stride
	
	%define EVEN_4	rsp_buff + offset_even + 6 * st_stride
	%define EVEN_5	rsp_buff + offset_even + 7 * st_stride
	%define EVEN_6	rsp_buff + offset_even + 5 * st_stride
	%define EVEN_7	rsp_buff + offset_even + 4 * st_stride
	
	IDCT_TRANSPOSE	16, 16
	
%endmacro

%macro IDCT8X8_TRANSPOSE 0
	
	%define LINE_0		rsp_buff + 0 * st_stride
	%define LINE_1		rsp_buff + 1 * st_stride
	%define LINE_2		rsp_buff + 2 * st_stride
	%define LINE_3		rsp_buff + 3 * st_stride
	
	%define	offset_even	4 * st_stride
	
	%define LINE_4	rsp_buff + offset_even + 2 * st_stride
	%define LINE_5	rsp_buff + offset_even + 3 * st_stride
	%define LINE_6	rsp_buff + offset_even + 1 * st_stride
	%define LINE_7	rsp_buff + offset_even + 0 * st_stride
	
	%define EVEN_0	LINE_0
	%define EVEN_1	LINE_1
	%define EVEN_2	LINE_2
	%define EVEN_3	LINE_3
	
	%define ODD_0	LINE_7
	%define ODD_1	LINE_6
	%define ODD_2	LINE_5
	%define ODD_3	LINE_4
	
	IDCT_TRANSPOSE	8, 8
	
%endmacro

%macro IDCT4X4_TRANSPOSE 0
	
	%define Line_0	rsp_buff + 0 * st_stride
	%define Line_1	rsp_buff + 1 * st_stride
	%define Line_2	rsp_buff + 3 * st_stride
	%define Line_3	rsp_buff + 2 * st_stride
	
	movq	m0,	[Line_0]
	movq	m1,	[Line_1]
	movq	m2,	[Line_2]
	movq	m3,	[Line_3]
	
	punpcklwd	m0, m1	;a0 b0 a1 b1 a2 b2 a3 b3
	punpcklwd	m2, m3	;c0 d0 c1 d1 c2 d2 c3 d3
	
	mova		m3,	m0
	punpckldq	m0, m2	;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m3, m2	;a2 b2 c2 d2 a3 b3 c3 d3
	
	mova		m1, m0
	palignr		m1, m0, 8	;m1 = a1 b1 c1 d1
	mova		m2, m3
	palignr		m3, m2, 8	;m3 = a3 b3 c3 d3
	
	%if save_idct == 0
		movq	[Line_0 + st_stride/2],m0
		movq	[Line_1 + st_stride/2],m1
		movq	[Line_3 + st_stride/2],m2
		movq	[Line_2 + st_stride/2],m3
	%else
		pxor		m7,	m7
		movd		m4,	[predict_buff + 0 * predict_stride]
		punpcklbw	m4, m7
		paddw		m0,	m4
		packuswb	m0, m0
		movd		[recon_buff + 0 * recon_stride],m0
		
		movd		m4,	[predict_buff + 1 * predict_stride]
		punpcklbw	m4, m7
		paddw		m1,	m4
		packuswb	m1, m1
		movd		[recon_buff + 1 * recon_stride],m1
		
		movd		m4,	[predict_buff + 2 * predict_stride]
		punpcklbw	m4, m7
		paddw		m2,	m4
		packuswb	m2, m2
		movd		[recon_buff + 2 * recon_stride],m2
		
		lea reg_tmp, [3*predict_stride]
		movd		m4,	[predict_buff + reg_tmp]
		punpcklbw	m4, m7
		paddw		m3,	m4
		packuswb	m3, m3
		lea reg_tmp, [3*recon_stride]
		movd		[recon_buff + reg_tmp],m3
	%endif
%endmacro

;input	%1 = src
;		%2 = src_stride in unit of byte
;		%3 = dest
;		%4 = dest_stride  in unit of byte
;		%5 = Dimension 8 or 4

%macro IDCT_8_4_TRANSPOSE	1
	
	%if %1 == 8
		%define Line_0	%1 + 0 * %2
		%define Line_1	%1 + 1 * %2
		%define Line_2	%1 + 2 * %2
		%define Line_3	%1 + 3 * %2
		
		%define Line_4	%1 + 6 * %2
		%define Line_5	%1 + 7 * %2
		%define Line_6	%1 + 5 * %2
		%define Line_7	%1 + 4 * %2
		
		;IDCT_TRANSPOSE_8X8 %1,%2,%3,%4
		
	%elif %1 == 4
		%define Line_0	%1 + 0 * %2
		%define Line_1	%1 + 1 * %2
		%define Line_2	%1 + 3 * %2
		%define Line_3	%1 + 2 * %2
		
		;IDCT_TRANSPOSE_4X4 %1,%2,%3,%4
	%endif
	
%endmacro

;VO_VOID IDCT16X16(const VO_S16 *p_invtransform_buff,
               ;VO_U8 *p_prediction_buff,
               ;const VO_U32  prediction_stride,
               ;VO_S16 * block_array)
INIT_XMM sse4
cglobal IDCT16X16_X86
	IDCT_FUNC_HEAD
	%define st_stride	4*16		;stack stride to save immediate result
	%define st_height	2*16		;stack height : two 16 rows		
	sub		rsp,	st_height * st_stride	;
	and		rsp,	~15
	
	%define dim_sub		16
	%define tr_width	16
	%define	rsp_buff	rsp
	%define const_im_shr 7
	
	IDCT16X16_ODD_SSE4	dim_sub, tr_width, r0,	2*16, rsp, st_stride
	IDCT16X16_EE_SSE4	dim_sub, tr_width, r0,	2*16, rsp + 8*4*16, st_stride
	IDCT16X16_COMPOSE	dim_sub
	
	%define	save_idct	0
	IDCT_TRANSPOSE_ADAPTER 16, dim_sub
	
	;%define st_stride	4*16		;stack stride to save immediate result
	;%define st_height	2*16		;stack height : two 16 rows		
	
	%define rsp_buff	rsp + st_stride*st_height/2
	%define const_im_shr 12
	IDCT16X16_ODD_SSE4	dim_sub, tr_width, rsp + 32, st_stride, rsp_buff, st_stride
	IDCT16X16_EE_SSE4	dim_sub, tr_width, rsp + 32, st_stride, rsp_buff + 8*4*16, st_stride
	IDCT16X16_COMPOSE	dim_sub
	
	%define reg_tmp_0 r4
	%define reg_tmp_1 r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	%define	save_idct	1
	IDCT_TRANSPOSE_ADAPTER 16, dim_sub
	IDCT_FUNC_FOOT

;VO_VOID IDCT8X8(const VO_S16 *p_invtransform_buff,
				;VO_U8 *p_prediction_buff,
				;const VO_U32  prediction_stride,
				;VO_S16 * block_array)
				
%macro IDCT_8X8_4X4	1
cglobal IDCT%1X%1_X86
	IDCT_FUNC_HEAD
	%define	st_stride	4*%1
	%define	st_height	%1
	%define	st_total_room	2*st_stride*st_height
	sub		rsp,	st_total_room	;
	and		rsp,	~15
	
	%define dim_main	%1	;8 for D8X8  4 for D4X4
	%define dim_sub		%1
	
	%define	rsp_buff		rsp
	%xdefine const_im_shr	7
	%define	save_idct		0
	
	IDCT_ODD_SSE4		dim_main, dim_main, dim_main, r0, 2*%1, rsp_buff, st_stride
	%if %1 == 8
		IDCT_EO_SSE4	dim_main, dim_main, dim_main, r0, 2*%1, rsp_buff + 2*(%1/4)*st_stride, st_stride
		IDCT_EE_SSE4	r0, 2*%1, rsp_buff + 3*(%1/4)*st_stride, st_stride, dim_main
	%elif %1 == 4
		IDCT_EE_SSE4	r0, 2*%1, rsp_buff + 2*(%1/4)*st_stride, st_stride, dim_main
	%endif
	IDCT_TRANSPOSE_ADAPTER	dim_main, dim_sub
	
	%xdefine const_im_shr 12
	%define	reg_tmp_0		r4
	%define	reg_tmp_1		r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	
	%define	rsp_buff		rsp + st_stride*st_height
	%xdefine const_im_shr	12
	%define	save_idct		1
	
	IDCT_ODD_SSE4		dim_main, dim_main, dim_main, rsp + st_stride/2, st_stride, rsp_buff, st_stride
	%if %1 == 8
		IDCT_EO_SSE4	dim_main, dim_main, dim_main, rsp + st_stride/2, st_stride, rsp_buff + 2*(%1/4)*st_stride, st_stride
		IDCT_EE_SSE4	rsp + st_stride/2, st_stride, rsp_buff + 3*(%1/4)*st_stride, st_stride, dim_main
	%elif %1 == 4
		IDCT_EE_SSE4	rsp + st_stride/2, st_stride, rsp_buff + 2*(%1/4)*st_stride, st_stride, dim_main
	%endif
	IDCT_TRANSPOSE_ADAPTER	dim_main, dim_sub

	IDCT_FUNC_FOOT
%endmacro

INIT_XMM sse4
IDCT_8X8_4X4 8
;IDCT_8X8_4X4 4

%macro IDCT4X4_BODY	0
	punpcklwd	m0,	m2	;0-2
	mova		m2,	m0
	punpcklwd	m1,	m3	;1-3
	mova		m3,	m1
	
	pmaddwd	m1,	[CONST_TR_16x16_3 + 0*16]	;ODD_0
	pmaddwd	m3,	[CONST_TR_16x16_3 + 1*16]	;ODD_1
	pmaddwd	m0,	[CONST_TR_16x16_3 + 2*16]	;EVEN_0
	pmaddwd	m2,	[CONST_TR_16x16_3 + 3*16]	;EVEN_1
	
	mova		m4,	m0
	paddd		m0,	m1		;EVEN_0 + ODD_0		Line_0
	psubd		m4,	m1		;EVEN_0 - ODD_0		Line_3
	mova		m1,	m2
	paddd		m1,	m3		;EVEN_1 + ODD_1		Line_1
	psubd		m2,	m3		;EVEN_1 - ODD_1		Line_2
	
	%if const_im_shr == 7
		mova		m7,	[CONST_U32_POS64]		;m7 = 64
	%elif const_im_shr == 12
		mova		m7,	[CONST_U32_POS_0X0800]	;m7 = 0x800
	%endif
	
	paddd	m0,	m7
	paddd	m4,	m7
	paddd	m1,	m7
	paddd	m2,	m7
	psrad	m0,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m1,	const_im_shr
	psrad	m2,	const_im_shr
	packssdw	m0,	m0
	packssdw	m4,	m4
	packssdw	m1,	m1
	packssdw	m2,	m2
	
	;m0 m1 m2 m4
	punpcklwd	m0, m1	;a0 b0 a1 b1 a2 b2 a3 b3
	punpcklwd	m2, m4	;c0 d0 c1 d1 c2 d2 c3 d3
	mova		m3,	m0
	punpckldq	m0,	m2	;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m3,	m2	;a2 b2 c2 d2 a3 b3 c3 d3
	mova		m1, m0
	palignr		m1, m0, 8
	mova		m2, m3
	palignr		m3, m2, 8
%endmacro

;VO_VOID IDCT4X4(const VO_S16 *p_invtransform_buff, 
             ;VO_U8 *p_prediction_buff,
             ;const VO_U32  prediction_stride,
             ;VO_S16 * tmp_block_array)
INIT_XMM sse4
cglobal IDCT4X4_X86
	IDCT_FUNC_HEAD
	%define	st_stride	4*4
	%define	st_height	4
	%define	st_total_room	2*st_stride*st_height
	sub		rsp,	st_total_room	;
	and		rsp,	~15
	
	movq	m0,	[r0 + 0*8]
	movq	m1,	[r0 + 1*8]
	movq	m2,	[r0 + 2*8]
	movq	m3,	[r0 + 3*8]
	
	%define const_im_shr 7
	IDCT4X4_BODY
	
	%define const_im_shr 12
	IDCT4X4_BODY
	
	pxor	m7,	m7
	movd	m4,	[r1 + 0 * r2]
	punpcklbw	m4,	m7
	paddw		m0,	m4
	packuswb	m0,	m0
	movd	[r1 + 0*r2], m0
	
	movd	m4,	[r1 + 1 * r2]
	punpcklbw	m4,	m7
	paddw		m1,	m4
	packuswb	m1,	m1
	movd	[r1 + 1*r2], m1
	
	movd	m4,	[r1 + 2 * r2]
	punpcklbw	m4,	m7
	paddw		m2,	m4
	packuswb	m2,	m2
	movd	[r1 + 2*r2], m2
	
	lea		r5,	[3 * r2]
	movd	m4,	[r1 + r5]
	punpcklbw	m4,	m7
	paddw		m3,	m4
	packuswb	m3,	m3

	movd	[r1 + r5], m3

	IDCT_FUNC_FOOT


;VO_VOID IDCT_NZ_1X1(const VO_S16 *p_invtransform_buff,
    ;VO_U8 *p_prediction_buff,
    ;const VO_U32 prediction_stride,
    ;VO_S16 * typeU32_width)
	 
INIT_XMM sse4
cglobal IDCT_NZ_1X1_X86
	IDCT_FUNC_HEAD	1
	
	movsx r5,word [r3]	;width
	movsx r0,word [r0]
	add	r0,	65
	shr	r0,	7
	movd	m1,	r0
	pshuflw	m0,	m1,	0
	pxor	m7,	m7
	
	cmp	r5,	4
	jz	branch_4X4
	cmp	r5,	8
	jz	branch_8X8
	cmp	r5,	16
	jz	branch_16X16
	cmp	r5,	32
	jz	branch_32X32
	
branch_4X4:
	loop_4X4:
	movd		m1,	[r1]
	punpcklbw	m1,	m7
	paddw		m1,	m0
	packuswb	m1,	m1
	movd		[r1],m1
	dec	r5
	lea	r1,	[r1 + r2]
	jnz	loop_4X4
	IDCT_FUNC_FOOT
	
branch_8X8:
	punpcklqdq	m0, m0
	loop_8X8:
	movq		m1,	[r1]
	punpcklbw	m1,	m7
	paddw		m1,	m0
	packuswb	m1,	m1
	movq		[r1],m1
	dec	r5
	lea	r1,	[r1 + r2]
	jnz	loop_8X8
	IDCT_FUNC_FOOT
	
branch_16X16:
	punpcklqdq	m0, m0
	loop_16X16:
	mova		m1,	[r1]
	mova		m2,	m1
	punpcklbw	m1,	m7
	punpckhbw	m2,	m7
	paddw		m1,	m0
	paddw		m2,	m0
	packuswb	m1,	m1
	packuswb	m2,	m2
	movq		[r1],m1
	movq		[r1+8],m2
	dec	r5
	lea	r1,	[r1 + r2]
	jnz	loop_16X16
	IDCT_FUNC_FOOT
	
branch_32X32:
	punpcklqdq	m0, m0
	loop_32X32:
	mova		m1,	[r1]
	mova		m2,	m1
	punpcklbw	m1,	m7
	punpckhbw	m2,	m7
	paddw		m1,	m0
	paddw		m2,	m0
	packuswb	m1,	m1
	packuswb	m2,	m2
	movq		[r1],m1
	movq		[r1 + 8],m2
	
	mova		m1,	[r1 + 16]
	mova		m2,	m1
	punpcklbw	m1,	m7
	punpckhbw	m2,	m7
	paddw		m1,	m0
	paddw		m2,	m0
	packuswb	m1,	m1
	packuswb	m2,	m2
	movq		[r1 + 16],m1
	movq		[r1 + 24],m2
	
	dec	r5
	lea	r1,	[r1 + r2]
	jnz	loop_32X32
	
	IDCT_FUNC_FOOT
	
%macro IDST4X4_BODY	0
	movq	m4,	m0	;L_0
	movq	m5,	m1	;L_2
	movq	m6,	m2	;L_3
	;movq	m7,	m3	;L_1
	
	mova	m7,	[CONST_S16_POS1]
	punpcklwd	m0, m1	;butterfly L_0 and L_2
	pmaddwd	m0,	m7	;c0
	punpcklwd	m1, m2	;butterfly L_2 and L_3
	pmaddwd	m1,	m7	;c1
	mova		m2,	m4
	punpcklwd	m2,	m6	;butterfly L_0 and L_3
	pmaddwd	m2,	[CONST_S16_1_N1]	;c2
	
	pxor		m7,	m7
	punpcklwd	m3,	m7
	pmaddwd	m3,	[CONST_S16_POS74]	;c3
	
	packssdw	m0, m0
	packssdw	m1, m1
	packssdw	m2, m2
	
	psubw		m4,	m5	;m4 = L_0 - L_2
	paddw		m4,	m6	;m4 = L_0 - L_2 + L_3
	punpcklwd	m4,	m7
	pmaddwd	m4,	[CONST_S16_POS74]	;Line_2
	
	movq		m5,	m0
	punpcklwd	m5,	m1	
	pmaddwd	m5,	[CONST_TR_IDST_4X4 + 0 * 16]	;m5 = butterfly c0 c1
	paddd		m5,	m3		;Line_0
	
	movq		m6,	m2
	punpcklwd	m6,	m1
	pmaddwd	m6,	[CONST_TR_IDST_4X4 + 1 * 16]	;m6 = butterfly c2 c1
	paddd		m6,	m3		;Line_1
	
	punpcklwd	m0,	m2
	pmaddwd	m0,	[CONST_TR_IDST_4X4 + 3 * 16]	;m0 = butterfly c0 c2
	psubd		m0,	m3		;Line_3
	
	%if const_im_shr == 7
			mova	m7,	[CONST_U32_POS64]		;%7 = 64
	%elif const_im_shr == 12
			mova	m7,	[CONST_U32_POS_0X0800]	;%7 = 0x800
	%endif
	
	paddd	m5,	m7
	paddd	m6,	m7
	paddd	m4,	m7
	paddd	m0,	m7
	
	psrad	m5,	const_im_shr
	psrad	m6,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m0,	const_im_shr
	
	packssdw	m5,	m5	;a0 a1 a2 a3
	packssdw	m6,	m6	;b0 b1 b2 b3
	packssdw	m4,	m4	;c0 c1 c2 c3
	packssdw	m0,	m0	;d0 d1 d2 d3
	
	punpcklwd	m5,	m6	;a0 b0 a1 b1 a2 b2 a3 b3
	punpcklwd	m4, m0	;c0 d0 c1 d1 c2 d2 c3 d3
	mova		m0,	m5
	punpckldq	m0,	m4		;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m5,	m4		;a2 b2 c2 d2 a3 b3 c3 d3
	mova		m3,	m0		;m0 = a0 b0 c0 d0
	palignr		m3, m0, 8	;m3 = a1 b1 c1 d1
	mova		m1, m5		;m1 = a2 b2 c2 d2
	palignr		m5, m1, 8	;m5 = a3 b3 c3 d3
	
	vo_exchange	2, 5
%endmacro

	
;VO_VOID IDST4X4_X86_sse4(const VO_S16 *p_invtransform_buff,
	 ;const VO_U8 *p_prediction_buff,
	 ;const VO_U32 prediction_stride,
	 ;VO_S16 * typeU32_width)
cglobal IDST4X4_X86
	IDCT_FUNC_HEAD
	
	movq	m0,	[r0 + 0*4]	;L_0
	movq	m1,	[r0 + 4*4]	;L_2
	movq	m2,	[r0 + 6*4]	;L_3
	movq	m3,	[r0 + 2*4]	;L_1
	
	%define const_im_shr 7
	IDST4X4_BODY
	
	%define const_im_shr 12
	IDST4X4_BODY
	
	pxor	m7,	m7
	
	movd	m4,	[r1 + 0*r2]
	punpcklbw	m4,	m7
	paddw	m0,	m4
	packuswb	m0,	m0
	movd	[r1 + 0*r2], m0
	
	movd	m4,	[r1 + 1*r2]
	punpcklbw	m4,	m7
	paddw	m3,	m4
	packuswb	m3,	m3
	movd	[r1 + 1*r2], m3
	
	movd	m4,	[r1 + 2*r2]
	punpcklbw	m4,	m7
	paddw	m1,	m4
	packuswb	m1,	m1
	movd	[r1 + 2*r2], m1
	
	lea		r4,	[3*r2]
	movd	m4,	[r1 + r4]
	punpcklbw	m4,	m7
	paddw	m2,	m4
	packuswb	m2,	m2
	movd	[r1 + r4], m2
	
	IDCT_FUNC_FOOT
	
;Input	%1 = 0 up_half 1 down half
%macro IDST4X4_1X1_BODY 1
	%if %1 == 0
		pmaddwd	m0,	[CONST_TR_IDST_4X4 + 0*2*8]
		pmaddwd	m1,	[CONST_TR_IDST_4X4 + 1*2*8]
		pmaddwd	m2,	[CONST_TR_IDST_4X4 + 2*2*8]
		pmaddwd	m3,	[CONST_TR_IDST_4X4 + 4*2*8]
	%elif %1 == 1
		mova	m4,	[CONST_TR_IDST_4X4 + 5*2*8]
		pmaddwd	m0,	m4
		pmaddwd	m1,	m4
		pmaddwd	m2,	m4
		pmaddwd	m3,	m4
	%endif
	
	%if const_im_shr == 7
		mova		m6,	[CONST_U32_POS64]		;%7 = 64
	%elif const_im_shr == 12
		mova		m6,	[CONST_U32_POS_0X0800]	;%7 = 0x800
	%endif
	
	paddd	m0,	m6
	paddd	m1,	m6
	paddd	m2,	m6
	paddd	m3,	m6
	
	psrad	m0,	const_im_shr
	psrad	m1,	const_im_shr
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	
	packssdw	m0, m0
	packssdw	m1, m1
	packssdw	m2, m2
	packssdw	m3, m3
%endmacro

;VO_VOID IDST4X4_1X1(const VO_S16 *p_invtransform_buff,
         ;VO_U8 *p_prediction_buff,
         ;const VO_U32 prediction_stride,
         ;VO_S16 * tmp_block_array)
INIT_XMM sse4
cglobal IDST4X4_1X1_X86
	IDCT_FUNC_HEAD	1
	movsx	r0,	word [r0]
	movd	m0,	r0
	pshuflw	m1,	m0,	0
	pxor	m7,	m7
	punpcklwd	m1,	m7
	mova	m0, m1
	mova	m2, m1
	mova	m3, m1
	
	%define	DOWN_HALF 0
	%define const_im_shr 7
	IDST4X4_1X1_BODY	DOWN_HALF
	
	punpcklwd	m0,	m7
	punpcklwd	m1,	m7
	punpcklwd	m2,	m7
	punpcklwd	m3,	m7
	
	%define	DOWN_HALF 1
	%define const_im_shr 12
	IDST4X4_1X1_BODY	DOWN_HALF
	
	movd	m4,	[r1 + 0*r2]
	punpcklbw	m4,	m7
	paddw	m0,	m4
	packuswb	m0, m0
	movd	[r1 + 0*r2], m0
	
	movd	m4,	[r1 + 1*r2]
	punpcklbw	m4,	m7
	paddw	m1,	m4
	packuswb	m1, m1
	movd	[r1 + 1*r2], m1
	
	movd	m4,	[r1 + 2*r2]
	punpcklbw	m4,	m7
	paddw	m2,	m4
	packuswb	m2, m2
	movd	[r1 + 2*r2], m2
	
	lea		r4,	[3*r2]
	movd	m4,	[r1 + r4]
	punpcklbw	m4,	m7
	paddw	m3,	m4
	packuswb	m3, m3
	movd	[r1 + r4], m3
	
	IDCT_FUNC_FOOT
	
;VO_VOID IDCT_NONE(const VO_S16 *p_invtransform_buff,
                   ;VO_U8 *p_prediction_buff,
                   ;const VO_U32 prediction_stride,
                   ;VO_S16 * typeU32_width)
INIT_XMM  sse2
cglobal IDCT_NONE_X86
    IDCT_FUNC_HEAD	1
    movsx r3,word [r3]
    cmp	r3,	4
    je	b4X4
    jp	b_exit
    
    b4X4:
    
    pxor	m7,	m7
    movq	m0,	[r0 + 0*8]		;m0 = a0 a1 a2 a3
    movq	m1,	[r0 + 1*8]		;m1 = b0 b1 b2 b3
    movq	m2,	[r0 + 2*8]		;m2 = c0 c1 c2 c3
    movq	m3,	[r0 + 3*8]		;m3 = d0 d1 d2 d3
    
    movd		m4,	[r1 + 0*r2]
    punpcklbw	m4,	m7
    paddw		m0,	m4
    packuswb	m0,	m0
    movd		[r1 + 0*r2],m0
    
    movd		m4,	[r1 + 1*r2]
    punpcklbw	m4,	m7
    paddw		m1,	m4
    packuswb	m1,	m1
    movd		[r1 + 1*r2],m1
    
    movd		m4,	[r1 + 2*r2]
    punpcklbw	m4,	m7
    paddw		m2,	m4
    packuswb	m2,	m2
    movd		[r1 + 2*r2],m2
    
    lea			r4,	[3*r2]
    movd		m4,	[r1 + r4]
    punpcklbw	m4,	m7
    paddw		m3,	m4
    packuswb	m3,	m3
    movd		[r1 + r4],m3
    
    b_exit:
    
    IDCT_FUNC_FOOT
    
    
;VO_VOID IDCT8X8_4x4(const VO_S16 *p_invtransform_buff,
             ;VO_U8 *p_prediction_buff,
             ;const VO_U32  prediction_stride,
             ;VO_S16 * block_array)
INIT_XMM sse4
cglobal IDCT8X8_4X4_X86
	IDCT_FUNC_HEAD
	
	%define	dim_main	8
	%define dim_sub		4
	%define	tr_width	4
	
	%define	st_part_1_width		4*4
	%define	st_part_1_height	8
	
	%define st_part_2_buf		rsp + st_part_1_width*st_part_1_height
	%define	st_part_2_width		2*8
	%define	st_part_2_height	4
	
	%define st_part_3_buf		rsp + st_part_1_width*st_part_1_height\
								+	st_part_2_width*st_part_2_height
	%define	st_part_3_width		4*8
	%define	st_part_3_height	8
	
	%define	st_total_room	st_part_1_width*st_part_1_height\
						+	st_part_2_width*st_part_2_height\
						+	st_part_3_width*st_part_3_height
	
	%define	rsp_buf		rsp
	%define	st_stride	st_part_1_width
	sub		rsp,	st_total_room
	and		rsp,	~0xF
	
	movq		m0,	[r0 + 1*16]
	punpcklwd	m0,	[r0 + 3*16]	;m0 = a0 b0 a1 b1 a2 b2 a3 b3
	
	mova		m1,	m0
	pmaddwd	m1,	[CONST_TR_16x16_2 + 0*16]
	mova		m5,	m0
	pmaddwd	m5,	[CONST_TR_16x16_2 + 1*16]
	mova		m6,	m0
	pmaddwd	m6,	[CONST_TR_16x16_2 + 2*16]
	mova		m7,	m0
	pmaddwd	m7,	[CONST_TR_16x16_2 + 3*16]
	
	mova	[rsp_buf + 0*16],	m1
	;mova	[rsp_buf + 1*16],	m5
	;mova	[rsp_buf + 2*16],	m6
	mova	[rsp_buf + 3*16],	m7
	
	pxor		m4,	m4
	movq		m0,	[r0 + 2*16]
	punpcklwd	m0,	m4
	mova		m1,	m0
	pmaddwd	m0,	[CONST_TR_16x16_3 + 0*16]	;EO[0]
	pmaddwd	m1,	[CONST_TR_16x16_3 + 1*16]	;EO[1]
	
	movq		m2,	[r0 + 0*16]
	punpcklwd	m2,	m4
	pmaddwd	m2,	[CONST_TR_16x16_3 + 3*16]	;EE[0]
	mova		m3,	m2							;EE[1]
	mova		m4,	m3
	
	paddd		m3,	m0							;E[0]
	psubd		m4,	m0							;E[3]
	
	mova		m0,	m2
	paddd		m0,	m1							;E[1]
	psubd		m2,	m1							;E[2]
	
	%define const_im_shr 7
	%if const_im_shr == 7
		mova		m7,	[CONST_U32_POS64]		;%7 = 64
	%elif const_im_shr == 12
		mova		m7,	[CONST_U32_POS_0X0800]	;%7 = 0x800
	%endif
	
	;m3 m0 m2 m4
	;m1 m5 m6 m7
	mova	m1,	m3
	paddd	m1,	[rsp_buf + 0*16]				
	psubd	m3,	[rsp_buf + 0*16]
	paddd	m1,	m7
	paddd	m3,	m7
	psrad	m1,	const_im_shr
	psrad	m3,	const_im_shr
	packssdw	m1,	m1
	packssdw	m3,	m3
	;movq	[rsp_buf + 0*16],	m1				;Line[0]
	movq	[rsp_buf + 7*16],	m3				;Line[7]
	
	mova	m3,	m0
	paddd	m0,	m5								
	psubd	m3,	m5	
	paddd	m0,	m7
	paddd	m3,	m7
	psrad	m0,	const_im_shr
	psrad	m3,	const_im_shr
	packssdw	m0,	m0
	packssdw	m3,	m3							
	;movq	[rsp_buf + 1*16],	m0				;Line[1]
	movq	[rsp_buf + 6*16],	m3				;Line[6]
	
	mova	m3,	m2
	paddd	m2,	m6								
	psubd	m3,	m6		
	paddd	m2,	m7
	paddd	m3,	m7
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr	
	packssdw	m2,	m2
	packssdw	m3,	m3						
	;movq	[rsp_buf + 2*16],	m2				;Line[2]
	movq	[rsp_buf + 5*16],	m3				;Line[5]
	
	mova	m5,	[rsp_buf + 3*16]
	mova	m3,	m4
	paddd	m4,	m5								
	psubd	m3,	m5	
	paddd	m4,	m7
	paddd	m3,	m7
	psrad	m4,	const_im_shr
	psrad	m3,	const_im_shr
	packssdw	m4,	m4
	packssdw	m3,	m3								
	;movq	[rsp_buf + 3*16],	m4				;Line[3]
	movq	[rsp_buf + 4*16],	m3				;Line[4]
	
	;m1 m0 m2 m4
	punpcklwd	m1,	m0	;a0 b0 a1 b1 a2 b2 a3 b3
	punpcklwd	m2,	m4	;c0 d0 c1 d1 c2 d2 c3 d3
	mova	m0,	m1
	punpckldq	m0,	m2	;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m1,	m2	;a2 b2 c2 d2 a3 b3 c3 d3
	
	mova	m2,	m0
	palignr	m2, m0, 8	;a1 b1 c1 d1
	mova	m3,	m1
	palignr	m3, m1, 8	;a3 b3 c3 d3
	
	%define Line_Pos(LineNo, LineOffset)  st_part_2_buf + LineNo*st_part_2_width + LineOffset
	
	movq	[Line_Pos(0,0)], m0
	movq	[Line_Pos(1,0)], m2
	movq	[Line_Pos(2,0)], m1
	movq	[Line_Pos(3,0)], m3
	
	movq	m0,	[rsp_buf + 4*16]
	movq	m1,	[rsp_buf + 5*16]
	movq	m2,	[rsp_buf + 6*16]
	movq	m3,	[rsp_buf + 7*16]
	
	punpcklwd	m0,	m1	;a0 b0 a1 b1 a2 b2 a3 b3
	punpcklwd	m2,	m3	;c0 d0 c1 d1 c2 d2 c3 d3
	mova	m1,	m0
	punpckldq	m0,	m2	;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m1,	m2	;a2 b2 c2 d2 a3 b3 c3 d3
	
	mova	m2,	m0
	palignr	m2, m0, 8	;a1 b1 c1 d1
	mova	m3,	m1
	palignr	m3, m1, 8	;a3 b3 c3 d3
	
	movq	[Line_Pos(0,8)], m0
	movq	[Line_Pos(1,8)], m2
	movq	[Line_Pos(2,8)], m1
	movq	[Line_Pos(3,8)], m3
	
	%define const_im_shr 12
	%define	reg_tmp_0		r4
	%define	reg_tmp_1		r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	
	%define	tr_width		8
	%define src_buf			st_part_2_buf
	%define src_width		st_part_2_width
	%define	rsp_buf		st_part_3_buf
	%define st_stride		st_part_3_width
	%define const_im_shr	12
	%define	save_idct		1
	
	IDCT_ODD_SSE4	dim_main, dim_sub, tr_width, src_buf, src_width, rsp_buf, st_stride
	
	%define	SRC_POS(LineNo, LineOffset)		src_buf + LineNo*src_width + LineOffset
	%define	DST_POS(LineNo, LineOffset)		rsp_buf + LineNo*st_stride + LineOffset
	
	;-----------EO-----------
	pxor		m4,	m4
	mova		m0,	[src_buf + 2*src_width]
	mova		m1,	m0
	punpcklwd	m0,	m4
	punpckhwd	m1,	m4
	mova		m2,	m0
	mova		m3,	m1
	
	mova		m5,	[CONST_TR_16x16_3 + 0*16]
	pmaddwd	m0,	m5			;EO[0]
	pmaddwd	m1,	m5			;EO[0]
	
	mova		m5,	[CONST_TR_16x16_3 + 1*16]
	pmaddwd	m2,	m5			;EO[1]
	pmaddwd	m3,	m5			;EO[1]
	
	;mova		[rsp_buf + 4*st_stride + 0*16],	m0
	;mova		[rsp_buf + 4*st_stride + 1*16],	m1
	;mova		[rsp_buf + 5*st_stride + 0*16],	m2
	;mova		[rsp_buf + 5*st_stride + 1*16],	m3
	;-----------EO-----------
	
	;-----------EE 0-3-----------
	mova		m5,	[src_buf + 0*src_width]
	mova		m6,	m5
	punpcklwd	m5,	m4
	punpckhwd	m6,	m4
	
	mova		m7,	[CONST_TR_16x16_3 + 3*16]
	pmaddwd	m5,	m7			;EE[0]	EE[1]
	pmaddwd	m6,	m7			;EE[0]	EE[1]
	
	mova		[DST_POS(5, 0*16)],	m5
	mova		[DST_POS(5, 1*16)],	m6
	;-----------EE 0-3-----------
	
	%define const_im_shr 12
	%define im_add [CONST_U32_POS_0X0800]
	
	;----------------------
	;[m0,m1] <-> [m5,m6]
	;------ 0 7 3 4 ------
	mova		m7,	m5
	paddd		m5,	m0					;E[0]
	psubd		m7,	m0					;E[3]
	
	mova		m4,	m5
	mova		m0,	[DST_POS(0, 0*16)]	;ODD[0]
	paddd		m4,	m0					;E[0] + O[0]
	psubd		m5,	m0					;E[0] - O[0]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m5,	m0
	psrad		m4,	const_im_shr
	psrad		m5,	const_im_shr
	packssdw	m4,	m4
	packssdw	m5,	m5
	movq		[DST_POS(0, 0*8)],m4	;Line[0]
	movq		[DST_POS(7, 0*8)],m5	;Line[7]
	
	mova		m4,	m7
	mova		m0,	[DST_POS(3, 0*16)]	;ODD[3]
	paddd		m4,	m0					;E[3] + O[3]
	psubd		m7,	m0					;E[3] - O[3]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m7,	m0
	psrad		m4,	const_im_shr
	psrad		m7,	const_im_shr
	packssdw	m4,	m4
	packssdw	m7,	m7
	movq		[DST_POS(3, 0*8)],m4	;Line[3]
	movq		[DST_POS(4, 0*8)],m7	;Line[4]
	
	mova		m7,	m6
	paddd		m6,	m1					;E[0]
	psubd		m7,	m1					;E[3]
	
	mova		m4,	m6
	mova		m0,	[DST_POS(0, 1*16)]	;ODD[0]
	paddd		m4,	m0					;E[0] + O[0]
	psubd		m6,	m0					;E[0] - O[0]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m6,	m0
	psrad		m4,	const_im_shr
	psrad		m6,	const_im_shr
	packssdw	m4,	m4
	packssdw	m6,	m6
	movq		[DST_POS(0, 1*8)],m4	;Line[0]
	movq		[DST_POS(7, 1*8)],m6	;Line[7]
	
	mova		m4,	m7
	mova		m0,	[DST_POS(3, 1*16)]	;ODD[3]
	paddd		m4,	m0					;E[3] + O[3]
	psubd		m7,	m0					;E[3] - O[3]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m7,	m0
	psrad		m4,	const_im_shr
	psrad		m7,	const_im_shr
	packssdw	m4,	m4
	packssdw	m7,	m7
	movq		[DST_POS(3, 1*8)],m4	;Line[3]
	movq		[DST_POS(4, 1*8)],m7	;Line[4]
	;------ 0 7 3 4 ------
	
	;----------------------
	
	;------ 1 6 2 5 ------
	mova		m5, [DST_POS(5, 0*16)]	;EE[1]	0-3
	mova		m6, [DST_POS(5, 1*16)]	;EE[1]	4-7
	
	;[m2,m3] <-> [m5,m6]	EE[1] <-> EO[1]
	mova		m7,	m5
	paddd		m5,	m2					;E[1]
	psubd		m7,	m2					;E[2]
	
	mova		m4,	m5
	mova		m0,	[DST_POS(1, 0*16)]	;O[1]
	paddd		m4,	m0					;E[1] + O[1]
	psubd		m5,	m0					;E[1] - O[1]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m5,	m0
	psrad		m4,	const_im_shr
	psrad		m5,	const_im_shr
	packssdw	m4,	m4
	packssdw	m5,	m5
	movq		[DST_POS(1, 0*8)],m4	;Line[1]
	movq		[DST_POS(6, 0*8)],m5	;Line[6]
	
	mova		m4,	m7
	mova		m0,	[DST_POS(2, 0*16)]	;ODD[2]
	paddd		m4,	m0					;E[2] + O[2]
	psubd		m7,	m0					;E[2] - O[2]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m7,	m0
	psrad		m4,	const_im_shr
	psrad		m7,	const_im_shr
	packssdw	m4,	m4
	packssdw	m7,	m7
	movq		[DST_POS(2, 0*8)],m4	;Line[2]
	movq		[DST_POS(5, 0*8)],m7	;Line[5]
	
	mova		m7,	m6
	paddd		m6,	m3					;E[1]
	psubd		m7,	m3					;E[2]
	
	mova		m4,	m6
	mova		m0,	[DST_POS(1, 1*16)]	;O[1]
	paddd		m4,	m0					;E[1] + O[1]
	psubd		m6,	m0					;E[1] - O[1]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m6,	m0
	psrad		m4,	const_im_shr
	psrad		m6,	const_im_shr
	packssdw	m4,	m4
	packssdw	m6,	m6
	movq		[DST_POS(1, 1*8)],m4	;Line[1]
	movq		[DST_POS(6, 1*8)],m6	;Line[6]
	
	mova		m4,	m7
	mova		m0,	[DST_POS(2, 1*16)]	;ODD[2]
	paddd		m4,	m0					;E[2] + O[2]
	psubd		m7,	m0					;E[2] - O[2]
	
	mova		m0,	im_add
	paddd		m4,	m0
	paddd		m7,	m0
	psrad		m4,	const_im_shr
	psrad		m7,	const_im_shr
	packssdw	m4,	m4
	packssdw	m7,	m7
	movq		[DST_POS(2, 1*8)],m4	;Line[2]
	movq		[DST_POS(5, 1*8)],m7	;Line[5]
	
	;------ 1 6 2 5 ------
	
	mova	m0,	[DST_POS(0, 0)]		;a0 - a7
	mova	m1,	[DST_POS(1, 0)]		;b0 - b7
	mova	m2,	[DST_POS(2, 0)]		;c0 - c7
	mova	m3,	[DST_POS(3, 0)]		;d0 - d7
	
	mova		m4,	m0
	punpcklwd	m0,	m1		;a0 b0 a1 b1 a2 b2 a3 b3
	punpckhwd	m4,	m1		;a4 b4 a5 b5 a6 b6 a7 b7
	mova		m1,	m2
	punpcklwd	m1,	m3		;c0 d0 c1 d1 c2 d2 c3 d3
	punpckhwd	m2,	m3		;c4 d4 c5 d5 c6 d6 c7 d7
	mova		m3,	m0
	punpckldq	m0,	m1		;a0 b0 c0 d0 a1 b1 c1 d1
	punpckhdq	m3, m1		;a2 b2 c2 d2 a3 b3 c3 d3
	mova		m1,	m4		
	punpckldq	m1,	m2		;a4 b4 c4 d4 a5 b5 c5 d5
	punpckhdq	m4, m2		;a6 b6 c6 d6 a7 b7 c7 d7
	
	mova	[DST_POS(0, 0)],	m4
	
	mova	m2,	[DST_POS(4, 0)]		;e0 - e7
	mova	m4,	[DST_POS(5, 0)]		;f0 - f7
	mova	m5,	[DST_POS(6, 0)]		;g0 - g7
	mova	m6,	[DST_POS(7, 0)]		;h0 - h7
	mova	m7,	m2
	punpcklwd	m2,	m4		;e0 f0 e1 f1 e2 f2 e3 f3
	punpckhwd	m7,	m4		;e4 f4 e5 f5 e6 f6 e7 f7
	mova	m4,	m5
	punpcklwd	m4,	m6		;g0 h0 g1 h1 g2 h2 g3 h3
	punpckhwd	m5,	m6		;g4 h4 g5 h5 g6 h6 g7 h7
	mova	m6,	m2
	punpckldq	m2,	m4		;e0 f0 g0 h0 e1 f1 g1 h1
	punpckhdq	m6,	m4		;e2 f2 g2 h2 e3 f3 g3 h3
	mova	m4,	m7
	punpckldq	m4,	m5		;e4 f4 g4 h4 e5 f5 g5 h5
	punpckhdq	m7,	m5		;e6 f6 g6 h6 e7 f7 g7 h7
	
	mova	[DST_POS(1, 0)],	m7
	
	;0 3 1 
	;2 6 4 7
	mova	m5,	m0
	punpcklqdq	m0, m2		;a0 b0 c0 d0 e0 f0 g0 h0
	punpckhqdq	m5, m2		;a1 b1 c1 d1 e1 f1 g1 h1
	mova	m2,	m3
	punpcklqdq	m2, m6		;a2 b2 c2 d2 e2 f2 g2 h2
	punpckhqdq	m3, m6		;a3 b3 c3 d3 e3 f3 g3 h3
	mova	m6,	m1
	punpcklqdq	m1, m4		;a4 b4 c4 d4 e4 f4 g4 h4
	punpckhqdq	m6, m4		;a5 b5 c5 d5 e5 f5 g5 h5
	
	;0 5 2 3 1 6
	pxor	m7, m7
	movq	m4,	[r1 + 0*r2]
	punpcklbw	m4, m7
	paddw	m0, m4
	packuswb	m0,	m0
	movq	[r1 + 0*r2], m0
	
	movq	m4,	[r1 + 1*r2]
	punpcklbw	m4, m7
	paddw	m5, m4
	packuswb	m5,	m5
	movq	[r1 + 1*r2], m5
	
	movq	m4,	[r1 + 2*r2]
	punpcklbw	m4, m7
	paddw	m2, m4
	packuswb	m2,	m2
	movq	[r1 + 2*r2], m2
	
	lea		r4,	[3*r2]
	movq	m4,	[r1 + r4]
	punpcklbw	m4, m7
	paddw	m3, m4
	packuswb	m3,	m3
	movq	[r1 + r4], m3
	
	lea		r4,	[r4 + r2]
	movq	m4,	[r1 + r4]
	punpcklbw	m4, m7
	paddw	m1, m4
	packuswb	m1,	m1
	movq	[r1 + r4], m1
	
	lea		r4,	[r4 + r2]
	movq	m4,	[r1 + r4]
	punpcklbw	m4, m7
	paddw	m6, m4
	packuswb	m6,	m6
	movq	[r1 + r4], m6
	
	mova	m0,	[DST_POS(0, 0)]
	mova	m2,	[DST_POS(1, 0)]
	
	mova	m1,	m0
	punpcklqdq	m0,	m2		;a6 b6 c6 d6 e6 f6 g6 h6
	punpckhqdq	m1,	m2		;a7 b7 c7 d7 e7 f7 g7 h7 
	
	lea		r4,	[r4 + r2]
	movq	m4,	[r1 + r4]
	punpcklbw	m4, m7
	paddw	m0, m4
	packuswb	m0,	m0
	movq	[r1 + r4], m0
	
	lea		r4,	[r4 + r2]
	movq	m5,	[r1 + r4]
	punpcklbw	m5, m7
	paddw	m1, m5
	packuswb	m1,	m1
	movq	[r1 + r4], m1
	
	IDCT_FUNC_FOOT
	
;input		%1 = 0:left half of 8 pixels 1: right half of 8 pixels
;			%2 = src_buf
;			%3 = src_stride
;			%4 = dest_buf
;			%5 = dest_stride

%macro IDCT16X16_8X8_E_O_BLEND	5
	%define RIGHT_HALF	%1
	%define	s_buf		%2 + RIGHT_HALF * %3 / 2
	%define	s_stride	%3
	%define	d_buf		%4 + RIGHT_HALF * %5 / 2
	%define	d_stride	%5

	%define RSP_POS(LineNo, LineOffset)			d_buf + LineNo * d_stride + LineOffset
	%define SAVE_RSP_POS(LineNo, LineOffset)	d_buf + LineNo * d_stride + LineOffset - 16 * RIGHT_HALF
	
	%define Src_EEO		s_buf + 4 * s_stride
	%define	Coef_EEO_0	CONST_TR_16X16_8X8 + 2*16
	%define	Coef_EEO_1	CONST_TR_16X16_8X8 + 3*16
	
	%define Src_EEE		s_buf + 0 * s_stride
	%define	Coef_EEE	CONST_TR_16X16_8X8 + 0*16
	
	pxor	m7,	m7
	
	mova	m6,	[Coef_EEO_0]
	mova	m0,	[Src_EEO]
	mova	m1,	m0
	punpcklwd	m0,	m7
	punpckhwd	m1,	m7
	mova		m2,	m0
	mova		m3,	m1
	pmaddwd	m0,	m6		;EE0[0]	0-3
	pmaddwd	m1,	m6		;EE0[0]	4-7
	
	mova		m6,	[Coef_EEO_1]
	pmaddwd	m2,	m6		;EE0[1]	0-3
	pmaddwd	m3,	m6		;EE0[1]	4-7
	
	mova	m6,	[Coef_EEE]
	mova	m4,	[Src_EEE]
	mova	m5,	m4
	punpcklwd	m4,	m7
	punpckhwd	m5,	m7
	pmaddwd	m4,	m6		;EEE[0]	0-3 EEE[1] 0-3
	pmaddwd	m5,	m6		;EEE[0]	4-7 EEE[1] 4-7
	mova		m7,	m4		;save	EEE[0]	0-3
	
	;m4 m5 <->	m0 m1
	mova	m6,	m4
	paddd	m4,	m0			;EE[0]	0-3
	psubd	m6,	m0			;EE[3]	4-7
	mova	[RSP_POS(12,0)],	m4
	mova	[RSP_POS(15,0)],	m6
	
	mova	m6,	m5			;save	EEE[0]	4-7
	mova	m0,	m5
	paddd	m0,	m1			;EE[0]	0-3
	psubd	m5,	m1			;EE[3]	4-7
	mova	[RSP_POS(12,16)],	m0
	mova	[RSP_POS(15,16)],	m5
	
	;mova	m0,	m7			;EEE[1]	0-3
	;mova	m1,	m6			;EEE[1]	4-7
	;m7 m6 <->	m2 m3
	mova	m4,	m7
	paddd	m4,	m2			;EE[1]	0-3
	psubd	m7,	m2			;EE[2]	0-3
	;mova	[RSP_POS(13,0)],	m4
	;mova	[RSP_POS(14,0)],	m7
	
	;m7 m6 <->	m2 m3
	mova	m0,	m6
	paddd	m0,	m3			;EE[1]	4-7
	psubd	m6,	m3			;EE[2]	4-7
	;mova	[RSP_POS(13,16)],	m0
	;mova	[RSP_POS(14,16)],	m6
	
	;Line1:(m4 m0)
	mova	m1,	[RSP_POS(9,0)]		;EO[1]	0-3
	mova	m2,	[RSP_POS(9,16)]		;EO[1]	4-7
	mova	m3,	m4
	paddd	m3,	m1					;E[1]	0-3
	psubd	m4,	m1					;E[6]	0-3
	mova	[RSP_POS(13,0)],	m3	
	mova	[RSP_POS(9,0)],		m4
	
	mova	m3,	m0
	paddd	m3,	m2					;E[1]	4-7
	psubd	m0,	m2					;E[6]	4-7
	mova	[RSP_POS(13,16)],	m3
	mova	[RSP_POS(9,16)],	m0
	
	;Line2:(m7 m6)
	mova	m1,	[RSP_POS(10,0)]		;EO[2]	0-3
	mova	m2,	[RSP_POS(10,16)]	;EO[2]	4-7
	mova	m3,	m7
	paddd	m3,	m1					;E[2]	0-3
	psubd	m7,	m1					;E[5]	0-3
	mova	[RSP_POS(14,0)],	m3
	mova	[RSP_POS(10,0)],	m7
	
	mova	m3,	m6
	paddd	m3,	m2					;E[2]	4-7
	psubd	m6,	m2					;E[5]	4-7
	mova	[RSP_POS(14,16)],	m3
	mova	[RSP_POS(10,16)],	m6
	
	mova	m0,	[RSP_POS(8,0)]		;EO[0]	0-3
	mova	m1,	[RSP_POS(8,16)]		;EO[0]	4-7
	mova	m2, [RSP_POS(12,0)]		;EE[0]	0-3
	mova	m3, [RSP_POS(12,16)]	;EE[0]	4-7
	
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0	
	psubd	m4,	m0	
	mova	[RSP_POS(12,0)], m2		;E[0]	0-3
	mova	[RSP_POS(8,0)],  m4		;E[7]	0-3
	
	paddd	m3,	m1	
	psubd	m5,	m1	
	mova	[RSP_POS(12,16)], m3	;E[0]	4-7
	mova	[RSP_POS(8,16)],  m5	;E[7]	4-7
	
	mova	m0,	[RSP_POS(11,0)]		;EO[3]	0-3
	mova	m1,	[RSP_POS(11,16)]	;EO[3]	4-7
	mova	m2, [RSP_POS(15,0)]		;EE[3]	0-3
	mova	m3, [RSP_POS(15,16)]	;EE[3]	4-7
	
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0	
	psubd	m4,	m0	
	;mova	[RSP_POS(15,0)], m2		;E[3]	0-3
	;mova	[RSP_POS(11,0)], m4		;E[4]	0-3
	
	paddd	m3,	m1	
	psubd	m5,	m1	
	;mova	[RSP_POS(15,16)], m3	;E[3]	4-7
	;mova	[RSP_POS(11,16)], m5	;E[4]	4-7
	
	%if const_im_shr == 7
		mova	m7,	[CONST_U32_POS64]		;%7 = 64
	%elif const_im_shr == 12
		mova	m7,	[CONST_U32_POS_0X0800]	;%7 = 0x800
	%endif
		
	;------------- Line 3 - 12 -------------
	mova	m0,	[RSP_POS(3,  0)]		;O[3]	0-3
	mova	m1,	[RSP_POS(3, 16)]		;O[3]	4-7
	
	;m2,m3 <-> m0,m1
	mova	m6,	m2
	paddd	m2,	m0	
	psubd	m6,	m0
	paddd	m2,	m7
	paddd	m6,	m7
	psrad	m2,	const_im_shr
	psrad	m6,	const_im_shr
	packssdw	m2,	m2
	packssdw	m6,	m6
	movq	[SAVE_RSP_POS(15,0)], m2		;Line[ 3]	0-3
	movq	[SAVE_RSP_POS( 3,0)], m6		;Line[12]	0-3
	
	mova	m6,	m3
	paddd	m3,	m1	
	psubd	m6,	m1
	paddd	m3,	m7
	paddd	m6,	m7
	psrad	m3,	const_im_shr
	psrad	m6,	const_im_shr
	packssdw	m3,	m3
	packssdw	m6,	m6	
	movq	[SAVE_RSP_POS(15, 8)], m3		;Line[ 3]	4-7
	movq	[SAVE_RSP_POS( 3, 8)], m6		;Line[12]	4-7
	;------------- Line 3 - 12 -------------
	
	;------------- Line 4 - 11 -------------
	mova	m0,	[RSP_POS(4,  0)]		;O[4]	0-3
	mova	m1,	[RSP_POS(4, 16)]		;O[4]	4-7
	;m4,m5 <-> m0,m1
	mova	m6,	m4
	paddd	m4,	m0	
	psubd	m6,	m0	
	paddd	m4,	m7
	paddd	m6,	m7
	psrad	m4,	const_im_shr
	psrad	m6,	const_im_shr
	packssdw	m4,	m4
	packssdw	m6,	m6
	movq	[SAVE_RSP_POS( 11, 0)], m4		;Line[ 4]	0-3
	movq	[SAVE_RSP_POS(  4, 0)], m6		;Line[11]	0-3
	
	mova	m6,	m5
	paddd	m5,	m1	
	psubd	m6,	m1	
	paddd	m5,	m7
	paddd	m6,	m7
	psrad	m5,	const_im_shr
	psrad	m6,	const_im_shr
	packssdw	m5,	m5
	packssdw	m6,	m6
	movq	[SAVE_RSP_POS( 11, 8)], m5	;Line[ 4]	4-7
	movq	[SAVE_RSP_POS(  4, 8)], m6	;Line[11]	4-7
	;------------- Line 4 - 11 -------------
	
	;------------- Line 0 - 15 -------------
	mova	m0,	[RSP_POS( 0, 0)]	;O[ 0]	0-3
	mova	m1,	[RSP_POS( 0,16)]	;O[ 0]	4-7
	mova	m2,	[RSP_POS(12, 0)]	;E[ 0]	0-3
	mova	m3,	[RSP_POS(12,16)]	;E[ 0]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS(12,  0)],	m2	;Line[ 0]	0-3
	movq	[SAVE_RSP_POS(12,  8)],	m3	;Line[ 0]	4-7
	movq	[SAVE_RSP_POS( 0,  0)],	m4	;Line[15]	0-3
	movq	[SAVE_RSP_POS( 0,  8)],	m5	;Line[15]	4-7
	;------------- Line 0 - 15 -------------
	
	;------------- Line 1 - 14 -------------
	mova	m0,	[RSP_POS( 1,  0)]	;O[ 1]	0-3
	mova	m1,	[RSP_POS( 1, 16)]	;O[ 1]	4-7
	mova	m2,	[RSP_POS(13,  0)]	;E[ 1]	0-3
	mova	m3,	[RSP_POS(13, 16)]	;E[ 1]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS(13,  0)],	m2	;Line[ 1]	0-3
	movq	[SAVE_RSP_POS(13,  8)],	m3	;Line[ 1]	4-7
	movq	[SAVE_RSP_POS( 1,  0)],	m4	;Line[14]	0-3
	movq	[SAVE_RSP_POS( 1,  8)],	m5	;Line[14]	4-7
	;------------- Line 1 - 14 -------------
	
	;------------- Line 2 - 13 -------------
	mova	m0,	[RSP_POS( 2, 0)]	;O[ 2]	0-3
	mova	m1,	[RSP_POS( 2,16)]	;O[ 2]	4-7
	mova	m2,	[RSP_POS(14, 0)]	;E[13]	0-3
	mova	m3,	[RSP_POS(14,16)]	;E[13]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS(14,  0)],	m2	;Line[ 2]	0-3
	movq	[SAVE_RSP_POS(14,  8)],	m3	;Line[ 2]	4-7
	movq	[SAVE_RSP_POS( 2,  0)],	m4	;Line[13]	0-3
	movq	[SAVE_RSP_POS( 2,  8)],	m5	;Line[13]	4-7
	;------------- Line 2 - 13 -------------
	
	;------------- Line 5 - 10 -------------
	mova	m0,	[RSP_POS( 5, 0)]	;O[ 5]	0-3
	mova	m1,	[RSP_POS( 5,16)]	;O[ 5]	4-7
	mova	m2,	[RSP_POS(10, 0)]	;E[10]	0-3
	mova	m3,	[RSP_POS(10,16)]	;E[10]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS(10,  0)],	m2	;Line[ 5]	0-3
	movq	[SAVE_RSP_POS(10,  8)],	m3	;Line[ 5]	4-7
	movq	[SAVE_RSP_POS( 5,  0)],	m4	;Line[10]	0-3
	movq	[SAVE_RSP_POS( 5,  8)],	m5	;Line[10]	4-7
	;------------- Line 5 - 10 -------------
	
	;------------- Line 6 - 9 -------------
	mova	m0,	[RSP_POS( 6, 0)]	;O[ 6]	0-3
	mova	m1,	[RSP_POS( 6,16)]	;O[ 6]	4-7
	mova	m2,	[RSP_POS( 9, 0)]	;E[ 9]	0-3
	mova	m3,	[RSP_POS( 9,16)]	;E[ 9]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS( 9,  0)],	m2	;Line[ 6]	0-3
	movq	[SAVE_RSP_POS( 9,  8)],	m3	;Line[ 6]	4-7
	movq	[SAVE_RSP_POS( 6,  0)],	m4	;Line[ 9]	0-3
	movq	[SAVE_RSP_POS( 6,  8)],	m5	;Line[ 9]	4-7
	;------------- Line 6 - 9 -------------
	
	;------------- Line 7 - 8 -------------
	mova	m0,	[RSP_POS( 7, 0)]	;O[ 7]	0-3
	mova	m1,	[RSP_POS( 7,16)]	;O[ 7]	4-7
	mova	m2,	[RSP_POS( 8, 0)]	;E[ 7]	0-3
	mova	m3,	[RSP_POS( 8,16)]	;E[ 7]	4-7
	mova	m4,	m2
	mova	m5,	m3
	paddd	m2,	m0
	psubd	m4,	m0
	paddd	m3,	m1
	psubd	m5,	m1
	
	paddd	m2,	m7
	paddd	m3,	m7
	paddd	m4,	m7
	paddd	m5,	m7
	
	psrad	m2,	const_im_shr
	psrad	m3,	const_im_shr
	psrad	m4,	const_im_shr
	psrad	m5,	const_im_shr
	
	packssdw	m2,	m2
	packssdw	m3,	m3
	packssdw	m4,	m4
	packssdw	m5,	m5
	
	movq	[SAVE_RSP_POS( 8,  0)],	m2	;Line[ 4]	0-3
	movq	[SAVE_RSP_POS( 8,  8)],	m3	;Line[ 4]	4-7
	movq	[SAVE_RSP_POS( 7,  0)],	m4	;Line[11]	0-3
	movq	[SAVE_RSP_POS( 7,  8)],	m5	;Line[11]	4-7
	;------------- Line 7 - 8 -------------
	
	%if RIGHT_HALF == 0
		
		%define RSP_LINE(LineNo, LineOffset)	%4 + LineNo * %5 + LineOffset
		
		%define E_0(line_offset)	RSP_LINE(12,line_offset)
		%define E_1(line_offset)	RSP_LINE(13,line_offset)
		%define E_2(line_offset)	RSP_LINE(14,line_offset)
		%define E_3(line_offset)	RSP_LINE(15,line_offset)
		%define E_4(line_offset)	RSP_LINE(11,line_offset)
		%define E_5(line_offset)	RSP_LINE(10,line_offset)
		%define E_6(line_offset)	RSP_LINE( 9,line_offset)
		%define E_7(line_offset)	RSP_LINE( 8,line_offset)
	
		%define ODD_0		RSP_LINE(0,0)
		%define ODD_1		RSP_LINE(1,0)
		%define ODD_2		RSP_LINE(2,0)
		%define ODD_3		RSP_LINE(3,0)
		%define ODD_4		RSP_LINE(4,0)
		%define ODD_5		RSP_LINE(5,0)
		%define ODD_6		RSP_LINE(6,0)
		%define ODD_7		RSP_LINE(7,0)
		
		%define	offset_even	8 * st_stride
		
		%define EVEN_0		E_0(0)
		%define EVEN_1		E_1(0)
		%define EVEN_2		E_2(0)
		%define EVEN_3		E_3(0)
		%define EVEN_4		E_4(0)
		%define EVEN_5		E_5(0)
		%define EVEN_6		E_6(0)
		%define EVEN_7		E_7(0)
	%endif
	
%endmacro
	
;VO_VOID IDCT16X16_8x8(const VO_S16 *p_invtransform_buff,
					  ;VO_U8 *p_prediction_buff,
					  ;const VO_U32  prediction_stride,
					  ;VO_S16 * block_array)
INIT_XMM sse4
cglobal IDCT16X16_8x8_X86
	IDCT_FUNC_HEAD
	
	%define	st_part_1_width		4*8
	%define	st_part_1_height	16
	
	%define st_part_2_buf		rsp + st_part_1_width*st_part_1_height
	%define	st_part_2_width		2*16
	%define	st_part_2_height	8
	
	%define st_part_3_buf		rsp + st_part_1_width*st_part_1_height\
								+	st_part_2_width*st_part_2_height
	%define	st_part_3_width		4*16
	%define	st_part_3_height	16
	
	%define	st_total_room	st_part_1_width*st_part_1_height\
						+	st_part_2_width*st_part_2_height\
						+	st_part_3_width*st_part_3_height
	
	%define	rsp_buff		rsp
	%define	st_stride	st_part_1_width
	sub		rsp,	st_total_room
	and		rsp,	~0xF
	
	%define	dim_main	16
	%define dim_sub		8
	%define	tr_width	8
	
	%define	rsp_buff	rsp
	%define const_im_shr 7
	
	IDCT16X16_ODD_SSE4	dim_sub, tr_width, r0,  2*16, rsp, st_stride
	IDCT16X16_EE_SSE4	dim_sub, tr_width, r0,	2*16, rsp + 8*4*8, st_stride
	
	IDCT16X16_8X8_E_O_BLEND	0, r0,	2*16, 	rsp, st_stride
	
	%define	save_idct	0
	IDCT_TRANSPOSE	16, 8
	
	%define src_buf		st_part_2_buf
	%define src_stride	st_part_2_width
	
	%define	rsp_buff		st_part_3_buf
	%define	st_stride	st_part_3_width
	%define	dim_main	16
	%define dim_sub		8
	%define	tr_width	16
	
	%define const_im_shr	12
	
	IDCT16X16_ODD_SSE4	dim_sub, tr_width, src_buf, src_stride, rsp_buff, st_stride
	IDCT16X16_EE_SSE4	dim_sub, tr_width, src_buf,	src_stride, rsp_buff + 8*st_stride, st_stride
	
	IDCT16X16_8X8_E_O_BLEND	0, src_buf, src_stride, rsp_buff, st_stride
	IDCT16X16_8X8_E_O_BLEND	1, src_buf, src_stride, rsp_buff, st_stride
	
	%define	rsp_buff		st_part_3_buf
	%define	st_stride		st_part_3_width
	
	%define reg_tmp_0		r4
	%define reg_tmp_1		r5
	%define	recon_buff		r1
	%define	recon_stride	r2
	%define	predict_buff	r1
	%define	predict_stride	r2
	%define	save_idct	1
	
	IDCT_TRANSPOSE	16, 16
	
	%if dim_sub == 16
	
	%endif
	
	IDCT_FUNC_FOOT