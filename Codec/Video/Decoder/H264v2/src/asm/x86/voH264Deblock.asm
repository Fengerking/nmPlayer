%include "voH264Util.asm"

SECTION_RODATA

pb_3_1: times 4 db 3, 1

SECTION .text

cextern pb_0
cextern pb_1
cextern pb_3
cextern pb_A1

; expands to [base],...,[base+7*stride]
%define VO_PASS_8_ROWS(line, line3, stride, stride3) \
    [line], [line+stride], [line+stride*2], [line3], \
    [line3+stride], [line3+stride*2], [line3+stride3], [line3+stride*4]

%define VO_PASS_8_ROWS(line, line3, stride, stride3, offset) \
    VO_PASS_8_ROWS(line+offset, line3+offset, stride, stride3)

; transpose 8*4 image matrix to 4*8 matrix
; input: 8 rows of 4 bytes in %4..%11
; output: 4 rows of 8 bytes in m0..m3

;m0 = a0 a1 a2 a3
;m1 = b0 b1 b2 b3
;m2 = c0 c1 c2 c3		a0 b0 c0 d0 e0 f0 g0 h0
;m3 = d0 d1 d2 d3		a1 b1 c1 d1 e1 f1 g1 h1
;m4 = e0 e1 e2 e3		a2 b2 c2 d2 e2 f2 g2 h2
;m5 = f0 f1 f2 f3		a3 b3 c3 d3 e3 f3 g3 h3
;m6 = g0 g1 g2 g3
;m7 = h0 h1 h2 h3

;1: bw/wd/dq
;2: bw/wd/dq
;3:	4-pel row e.g. m0 = a0 a1 a2 a3
;4:	4-pel row e.g. m1 = b0 b1 b2 b3
;5:	4-pel row e.g. m2 = c0 c1 c2 c3 
;6:	4-pel row e.g. m3 = d0 d1 d2 d3

;input : %3 %4 %5 %6
;output : %3 %5  ;  %3 = a0 b0 c0 d0 a1 b1 c1 d1  
				 ;	%5 = a2 b2 c2 d2 a3 b3 c3 d3
				 
%macro VO_TRANSPOSE4x8_HALF 6
	punpckl%1	m%4, m%6		;m1 = c0 d0 c1 d1 c2 d2 c3 d3
	punpckl%1	m%3, m%5		;m0 = a0 b0 a1 b1 a2 b2 a3 b3
	mova		m%5, m%3		;m2 = a0 b0 a1 b1 a2 b2 a3 b3
	punpckh%2   m%5, m%4		;m2 = a2 b2 c2 d2 a3 b3 c3 d3	output
	punpckl%2	m%3, m%4		;m0 = a0 b0 c0 d0 a1 b1 c1 d1	output
%endmacro

%macro VO_TRANSPOSE4x8_LOAD 11

    movh       m0, %4		;movd	mm0, [esi + 0*eax]	;movd mm0, [pix - 2]
    movh       m2, %5		;movd	mm2, [esi + 1*eax]	;movd mm2, [pix - 2 + stride]
    movh       m1, %6		;movd	mm1, [esi + 2*eax]	;movd mm1, [pix - 2 + 2*stride]
    movh       m3, %7		;movd   mm3, [esi + 3*eax]	;movd mm3, [pix - 2 + 3*stride]
    
    ;0 1 2 3 row
    VO_TRANSPOSE4x8_HALF  %1, %2, 0, 1, 2, 3

    movh       m4, %8		;movd   mm4, [esi + 4*eax]	;movd mm4,	[pix - 2 + 5*stride] 
    movh       m6, %9		;movd   mm6, [esi + 5*eax]	;movd mm6,	[pix - 2 + 6*stride] 
    movh       m5, %10		;movd   mm5, [esi + 6*eax]	;movd mm6,	[pix - 2 + 6*stride] 
    movh       m7, %11		;movd   mm7, [esi + 7*eax]	;movd mm7,	[pix - 2 + 7*stride]
    
    ;4 5 6 7 row
    VO_TRANSPOSE4x8_HALF  %1, %2, 4, 5, 6, 7

    punpckh%3  m1, m0, m4	;movq mm1, mm0  punpckhdq mm1, mm4		;mm1 = a1 b1 c1 d1 e1 f1 g1 h1
    punpckl%3  m0, m4		;punpckldq mm0, mm4						;mm0 = a0 b0 c0 d0 e0 f0 g0 h0
    punpckh%3  m3, m2, m6	;movq mm3, mm2  punpckhdq mm3, mm6		;mm3 = a3 b3 c3 d3 e3 f3 g3 h3
    punpckl%3  m2, m6		;punpckldq mm2, mm6						;mm2 = a2 b2 c2 d2 e2 f2 g2 h2
%endmacro

; transpose 4*8 matrix to 8*4 matrix
; in: 4 rows of 8 bytes in m0..m3
; out: 8 rows of 4 bytes in %1..%8

									;a0 b0 c0 d0
									;a1 b1 c1 d1
;m0 = a0 a1 a2 a3 a4 a5 a6 a7		;a2 b2 c2 d2
;m1 = b0 b1 b2 b3 b4 b5 b6 b7		;a3 b3 c3 d3
;m2 = c0 c0 c2 c3 c4 c5 c6 c7		;a4 b4 c4 d4
;m3 = d0 d1 d2 d3 d4 d5 d6 d7		;a5 b5 c5 d5
									;a6 b6 c6 d6
									;a7 b7 c7 d7
%macro VO_ROW_STORE 3
	movh		%1, %3
	punpckhdq	%3, %3
	movh		%2, %3
%endmacro

%macro VO_TRANSPOSE8x4B_STORE 8

	punpckhdq  m6, m2, m2		;m6 = c4 c5 c6 c7 c4 c5 c6 c7   m2 = c0 c1 c2 c3 c4 c5 c6 c7
	punpckhdq  m5, m1, m1		;m5 = b4 b5 g6 b7 b4 b5 g6 b7   m1 = b0 b1 g2 b3 b4 b5 g6 b7
    punpckhdq  m4, m0, m0		;m4 = a4 a5 a6 a7 a4 a5 a6 a7	m0 = a0 a1 a2 a3 a4 a5 a6 a7

	punpcklbw  m2, m3			;m2 = c0 d0 c1 d1 c2 d2 c3 d3
    punpcklbw  m0, m1			;m0 = a0 b0 a1 b1 a2 b2 a3 b3
    punpcklwd  m1, m0, m2		;m1 = a0 b0 c0 d0 a1 b1 c1 d1
    punpckhwd  m0, m2			;m0 = a2 b2 c2 d2 a3 b3 c3 d3
    
    VO_ROW_STORE %1, %2, m1
    VO_ROW_STORE %3, %4, m0
    
    punpcklbw  m4, m5			;m4 = a4 b4 a5 b5 a6 b6 a7 b7
    punpckhdq  m3, m3			;m3 = d4 d5 d6 d7 d4 d5 d6 d7
    punpcklbw  m6, m3			;m6 = c4 d4 c5 d5 c6 d6 c7 d7
    punpcklwd  m5, m4, m6		;m5 = a4 b4 c4 d4 a5 b5 c5 d5
    punpckhwd  m4, m6			;m4 = a6 b6 c6 d6 a7 b7 c7 d7
    
    VO_ROW_STORE %5, %6, m5
    VO_ROW_STORE %7, %8, m4
    
%endmacro

%macro VO_TRANSPOSE4x8B_LOAD 8
    VO_TRANSPOSE4x8_LOAD bw, wd, dq, %1, %2, %3, %4, %5, %6, %7, %8
%endmacro

%macro VO_SBUTTERFLY3 4
    punpckh%1  %4, %2, %3
    punpckl%1  %2, %3
%endmacro

; this code block is used to transpos 6*8 matrix to 8*6 matrix
; input: 8 rows of 8 (only the middle 6 pels are used) in %1..%8
; output: 6 rows of 8 in [%9+0*16] .. [%9+5*16]

%macro VO_TRANSPOSE6x8_MEM 9
    RESET_MM_PERMUTATION
    movq  m0, %1
    movq  m1, %2
    movq  m2, %3
    movq  m3, %4
    movq  m4, %5
    movq  m5, %6
    movq  m6, %7
    
    ;-------------------bw----------------------
    SBUTTERFLY bw, 0, 1, 7				;m0 = a0 b0 a1 b1 a2 b2 a3 b3	m1 = a4 b4 a5 b5 a6 b6 a7 b7	m1=mm7	m7=mm1
    SBUTTERFLY bw, 2, 3, 7				;m2 = c0 d0 c1 d1 c2 d2 c3 d3	m3 = c4 d4 c5 d5 c6 d6 c7 d7	m3=mm1	m7=mm3
    SBUTTERFLY bw, 4, 5, 7				;m4 = e0 f0 e1 f1 e2 f2 e3 f3   m5 = e4 f4 e5 f5 e6 f6 e7 f7	m5=mm3	m7=mm5
    movq  [%9+0x10], m3					;movq [pix_tmp + 0x10] , m3										m3=mm1
    VO_SBUTTERFLY3 bw, m6, %8, m7		;m6 = g0 h0 g1 h1 g2 h2 g3 h3   m7 = g4 h4 g5 h5 g6 h6 g7 h7	m6=mm6	m7=mm5
    ;-------------------bw----------------------
    
    ;-------------------wd----------------------
    SBUTTERFLY wd, 0, 2, 3					;m0 = a0 b0 c0 d0 a1 b1 c1 d1   m2 = a2 b2 c2 d2 a3 b3 c3 d3	m2=mm1	m3=mm2
    SBUTTERFLY wd, 4, 6, 3					;m4 = e0 f0 g0 h0 e1 f1 g1 h1   m6 = e2 f2 g2 h2 e3 f3 g3 h3	m6=mm2	m3=mm6
    SBUTTERFLY wd, 5, 7, 3					;m5 = e4 f4 g4 h4 e5 f5 g5 h5   m7 = e6 f6 g6 h6 e7 f7 g7 h7
    VO_SBUTTERFLY3 wd, m1, [%9+0x10], m3	;m1 = a4 b4 c4 d4 a5 b5 c5 d5   m3 = a6 b6 c6 d6 a7 b7 c7 d7
    ;-------------------wd----------------------
    
    ;-------------------dq----------------------
    punpckhdq m0, m4					;m0 = a1 b1 c1 d1 e1 f1 g1 h1
    SBUTTERFLY dq, 1, 5, 4				;m1 = a4 b4 c4 d4 e4 f4 g4 h4   m5 = a5 b5 c5 d5 e5 f5 g5 h5
    SBUTTERFLY dq, 2, 6, 4				;m2 = a2 b2 c2 d2 e2 f2 g2 h2   m6 = a3 b3 c3 d3 e3 f3 g3 h3
    punpckldq m3, m7					;m3 = a6 b6 c6 d6 e6 f6 g6 h6   m7 = a7 b7 c7 d7 e7 f7 g7 h7
    ;-------------------dq----------------------
    
    movq  [%9+0x00], m0					;movq [pix_tmp] , m0
    movq  [%9+0x10], m2
    movq  [%9+0x20], m6
    movq  [%9+0x30], m1
    movq  [%9+0x40], m5
    movq  [%9+0x50], m3					;m0,m2,m6,m1,m5,m3 are valid
    
    RESET_MM_PERMUTATION
%endmacro

; the code block is used to transpos 8*8 matrix
; input:  8*8 input matrix in %1..%8
; output: 8*8 output matrix in %9..%16

%macro VO_TRANSPOSE8x8_MEM 16
    RESET_MM_PERMUTATION
    
    movq  m0, %1
    movq  m1, %2
    movq  m2, %3
    movq  m3, %4
    movq  m4, %5
    movq  m5, %6
    movq  m6, %7
    
    ;-------------------bw----------------------
    SBUTTERFLY bw, 0, 1, 7				;m0 = a0 b0 a1 b1 a2 b2 a3 b3	m1 = a4 b4 a5 b5 a6 b6 a7 b7
    SBUTTERFLY bw, 2, 3, 7				;m2 = c0 d0 c1 d1 c2 d2 c3 d3	m3 = c4 d4 c5 d5 c6 d6 c7 d7
    SBUTTERFLY bw, 4, 5, 7				;m4 = e0 f0 e1 f1 e2 f2 e3 f3	m5 = e4 f4 e5 f5 e6 f6 e7 f7
    VO_SBUTTERFLY3 bw, m6, %8, m7		;m6 = g0 h0 g1 h1 g2 h2 g3 h3	m7 = g4 h4 g5 h5 g6 h6 g7 h7
    ;-------------------bw----------------------
    
    ;-------------------wd----------------------
    movq  %9,  m7
    SBUTTERFLY wd, 0, 2, 7				;m0 = a0 b0 c0 d0 a1 b1 c1 d1	m2 = a2 b2 c2 d2 a3 b3 c3 d3
    SBUTTERFLY wd, 1, 3, 7				;m1 = a4 b4 c4 d4 a5 b5 c5 d5	m3 = a6 b6 c6 d6 a7 b7 c7 d7
    SBUTTERFLY wd, 4, 6, 7				;m4 = e0 f0 g0 h0 e1 f1 g1 h1	m6 = e2 f2 g2 h2 e3 f3 g3 h3
    movq  %11, m6
    movq  m6,  %9						;m6 = g4 h4 g5 h5 g6 h6 g7 h7
    SBUTTERFLY wd, 5, 6, 7				;m5 = e4 f4 g4 h4 e5 f5 g5 h5	m6 = e6 f6 g6 h6 e7 f7 g7 h7
    ;-------------------wd----------------------
    
    ;-------------------dq----------------------
    SBUTTERFLY dq, 0, 4, 7				;m0 = a0 b0 c0 d0 e0 f0 g0 h0	m4 = a1 b1 c1 d1 e1 f1 g1 h1
    SBUTTERFLY dq, 1, 5, 7				;m1 = a4 b4 c4 d4 e4 f4 g4 h4   m5 = a5 b5 c5 d5 e5 f5 g5 h5
    VO_SBUTTERFLY3 dq, m2, %11, m7		;m2 = a2 b2 c2 d2 e2 f2 g2 h2	m7 = a3 b3 c3 d3 e3 f3 g3 h3
    movq  %9,  m0
    movq  %10, m4
    movq  %11, m2
    movq  %12, m7
    movq  %13, m1
    movq  %14, m5
    ;VO_SBUTTERFLY3 dq, m2, %11, m0
    SBUTTERFLY dq, 3, 6, 0				;m3 = a6 b6 c6 d6 e6 f6 g6 h6	m5 = a7 b7 c7 d7 e7 f7 g7 h7
    movq  %15, m3						;
    movq  %16, m6
    ;-------------------dq----------------------
    
    RESET_MM_PERMUTATION
%endmacro

; output: %4 = |%1-%2|>%3
; clobbers: %5
%macro VO_DIFF_GT 5
%if avx_enabled == 1
    psubusb %5, %2, %1
    psubusb %4, %1, %2
%else
	mova    %4, %1
    mova    %5, %2
    psubusb %4, %2
    psubusb %5, %1
%endif
	;计算结果是：当 |%1-%2|>%3 时，%4 的对应字节值为 0xFF; 当 |%1-%2|<=%3 时，%4 的对应字节值为 0
    por     %4, %5		;求|%1 - %2| 的值
    psubusb %4, %3		;%4 = |%1 - %2| - %3
%endmacro

; output: %4 = |%1-%2|>%3
; clobbers: %5
%macro VO_DIFF_GT2 5
%ifndef ARCH_X86_64
	mova    %4, %1
	mova    %5, %2
    psubusb %4, %2
    psubusb %5, %1
%else
    psubusb %4, %1, %2
    psubusb %5, %2, %1
%endif
	;下面的 psubusb 和 pcmpeqb 都是压缩无符号饱和字节运算，前两步运算结果是 
	;%5 = (%2 - %1) - %3 , %4 = (%1 - %2) - %3 , 而 (%1 - %2) 和 (%2 - %1) 这两个计算式中， 
	;对于某一个字节,无符号饱和相减后，必有一个为 0, 另一个大于等于0 。假设 (%1 - %2) 饱和
	;相减结果为0, 则 (%2 - %1) 结果必大于等于零，其值是 |%2 - %1|;据此可以推断 %5 =(%1 - %2) - %3,
	;其饱和相减后对应的字节为0,而对于(%2 - %1) - %3, 如果 (%2 - %1) >= %3,则 %4 中对应的值为 0xFF,
	;否则对应的值为 0 。由此可见当 |%2 - %1| >= %3  时, 前两步结束后，%5 %4中对应的字节值必然一个
	;为0 ， 一个为 0xFF，则下面第三步的运算结果对应的字节值是 0，因为 0 和 0xFF 不相等;
	
	;由此可见 , 如果 |%2 - %1| <= %3 , 则 %5 %4中对应的字节值全部为零，所以 pcmpeqb %4, %5 运算的结
	;果对应的位值为 0xFF; 相反,如果 |%2 - %1| > %3 , 则%5 %4中，一个为0，一个为0xFF,所以 
	;pcmpeqb %4, %5 运算的结果必然为 0.
	
	;计算结果是：当 |%1-%2|>%3 时，%4 的对应字节值为 0; 当 |%1-%2|<=%3 时，%4 的对应字节值为 0xFF
	;这个计算结果和 VO_DIFF_GT 恰好相反，这一点值得注意。
	
	psubusb %4, %3
    psubusb %5, %3
    pcmpeqb %4, %5
%endmacro

; in: m0=p1 m1=p0 m2=q0 m3=q1 %1=alpha-1 %2=beta-1
; out: m5=beta-1, m7=mask, %3=alpha-1
; clobbers: m4,m6
%macro VO_LOAD_MASK 2-3
    movd     m4, %1
    movd     m5, %2
    VO_SPLATW   m4, m4
    VO_SPLATW   m5, m5
    packuswb m4, m4  ; 16x alpha-1
    packuswb m5, m5  ; 16x beta-1
    ;上面这部分的作用就是让 alpha-1 和 beta-1 填充到 m4 和 m5 的每一个字节单元
%if %0>2
    mova     %3, m4
%endif
    VO_DIFF_GT  m1, m2, m4, m7, m6 ; |p0-q0| > alpha-1		;mm0 mm1 mm2 mm3 < --- > p1 p0 q0 q1
    VO_DIFF_GT  m0, m1, m5, m4, m6 ; |p1-p0| > beta-1
    por         m7, m4
    VO_DIFF_GT  m3, m2, m5, m4, m6 ; |q1-q0| > beta-1
    por         m7, m4
    pxor        m6, m6
    pcmpeqb    m7, m6
%endmacro

; in: m0=p1 m1=p0 m2=q0 m3=q1 m7=(tc&mask)
; out: m1=p0' m2=q0'
; clobbers: m0,3-6
%macro VO_DEBLOCK_P0_Q0 0
    pcmpeqb m4, m4
    pxor    m5, m1, m2   ; p0^q0
    pxor    m3, m4
    pand    m5, [pb_1]   ; (p0^q0)&1
    pavgb   m3, m0       ; (p1 - q1 + 256)>>1
    pxor    m4, m1
    pavgb   m3, [pb_3]   ; (((p1 - q1 + 256)>>1)+4)>>1 = 64+2+(p1-q1)>>2
    pavgb   m4, m2       ; (q0 - p0 + 256)>>1
    pavgb   m3, m5
    mova    m6, [pb_A1]
    paddusb m3, m4       ; d+128+33
    psubusb m6, m3
    psubusb m3, [pb_A1]
    pminub  m6, m7
    pminub  m3, m7
    psubusb m1, m6
    psubusb m2, m3
    paddusb m1, m3
    paddusb m2, m6
%endmacro

; in: m1=p0 m2=q0
;     %1=p1 %2=q2 %3=[q2] %4=[q1] %5=tc0 %6=tmp
; out: [q1] = clip( (q2+((p0+q0+1)>>1))>>1, q1-tc0, q1+tc0 )
; clobbers: q2, tmp, tc0
%macro VO_LUMA_Q1 6
    pavgb   %6, m1, m2
    pavgb   %2, %6       ; avg(p2,avg(p0,q0))
    pxor    %6, %3
    pand    %6, [pb_1]   ; (p2^avg(p0,q0))&1
    psubusb %2, %6       ; (p2+((p0+q0+1)>>1))>>1
    psubusb %6, %1, %5
    paddusb %5, %1
    pmaxub  %2, %6
    pminub  %2, %5
    mova    %4, %2
%endmacro

;void vo_deblock_v_luma_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
%macro VO_DEBLOCK_LUMA 0
vo_global deblock_v_luma_8_sse2_aligned_16
	push r6
	mov  r6,rsp
	
	push r3
	push r4
	
	sub  rsp,   32
	and  rsp,   ~15
	
	mov  r0,	[r6 + 8]			;r0  pix
	mov  r1,	[r6 + 12]			;r1  stride
	mov  r2,	[r6 + 16]			;r2  alpha
	mov  r3,	[r6 + 20]			;r3	 beta
	mov  r4,	[r6 + 24]			;r4  tc0
	
	lea     r4, [r1*3]						;lea esi , [ecx*3]	;esi = 3*stride
	neg     r4								;neg esi			;esi = - 3*stride
	add     r4, r0 ; pix-3*stride			;add esi , eax		;esi = pix - 3*stride
	mova    m3, [r0+r1]   ; q1				;movdqa xmm3, [eax + ecx]		;xmm3 = pix + 1*stride
    mova    m2, [r0]      ; q0				;movdqa xmm2, [eax]				;xmm2 = pix - 0*stride
    mova    m1, [r4+2*r1] ; p0				;movdqa xmm1, [esi + 2*ecx]		;xmm1 = pix - 1*stride
	mova    m0, [r4+r1]   ; p1				;movdqa xmm0, [esi + ecx]		;xmm0 = pix - 2*stride
	
    dec     r2     ; alpha-1				;dec edx			;edx = alpha - 1
    dec     r3     ; beta-1					;dec ebx			;ebx = beta - 1
    
	VO_LOAD_MASK r2, r3						;VO_LOAD_MASK alpha - 1, beta - 1

	;add     esp , 4
    mov     r3, [r6 + 24]					;r3  tc0				
    pcmpeqb m3, m3							;m3 = 0xFFFFFFFF FFFFFFFF
    movd    m4, [r3] ; tc0					;xmm4 = tc0
    punpcklbw m4, m4
    punpcklbw m4, m4 ; tc = 4x tc0[3], 4x tc0[2], 4x tc0[1], 4x tc0[0]
    ;上面的结果;m4 = tc0[0] tc0[0] tc0[0] tc0[0] tc0[1] tc0[1] tc0[1] tc0[1] tc0[2] tc0[2] tc0[2] tc0[2] tc0[3] tc0[3] tc0[3] tc0[3]
    mova   [rsp+16], m4 ; tc				;save xmm4 to stack temporarily
    pcmpgtb m4, m3							;
    mova    m3, [r4] ; p2
    pand    m4, m7
    mova   [rsp], m4 ; mask

    VO_DIFF_GT2 m1, m3, m5, m6, m7 ; |p2-p0| > beta-1
    pand    m6, m4
    pand    m4, [rsp+16] ; tc
    psubb   m7, m4, m6
    pand    m6, m4
    VO_LUMA_Q1 m0, m3, [r4], [r4+r1], m6, m4

    mova    m4, [r0+2*r1] ; q2
    VO_DIFF_GT2 m2, m4, m5, m6, m3 ; |q2-q0| > beta-1
    pand    m6, [rsp] ; mask
    mova    m5, [rsp+16] ; tc
    psubb   m7, m6
    pand    m5, m6
    mova    m3, [r0+r1]
    VO_LUMA_Q1 m3, m4, [r0+2*r1], [r0+r1], m5, m6

    VO_DEBLOCK_P0_Q0
    mova    [r4+2*r1], m1
    mova    [r0], m2
	

	mov rsp , r6
	sub rsp , 8
	pop r4
	pop r3
	leave
	ret
	
;void vo_deblock_h_luma_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
INIT_MMX cpuname
vo_global deblock_h_luma_8_sse2_aligned_16
	push r6
	mov  r6, rsp
	push r3
	push r4
	
	sub  rsp, 0x70
	and  rsp, ~15
	sub  rsp, 12
	
	;DO SOMETHING
	;mov    r0, r0mp
	;mov    r3, r1m
	mov     r0, [r6 + 8]										;mov eax, [ebp + 8]		:pix
    mov     r3, [r6 + 12]										;mov ebx, [ebp + 12]	;stride
    lea    r4, [r3*3]											;lea esi, [ebx * 3]		;3 * stride
    sub    r0, 4												;sub eax, 4				;pix - 4
    lea    r1, [r0+r4]											;lea ecx, [eax + esi]	;pix - 4 + 3*stride
%define pix_tmp rsp+12*HAVE_ALIGNED_STACK						;stack align , used as temporal memory space

    ; transpose 6x16 -> tmp space
    ;TRANSPOSE6*8_MEM VO_PASS_8_ROWS(pix-4 , pix-4+3*stride , stride , 3*stride) , pix_tmp
    VO_TRANSPOSE6x8_MEM  VO_PASS_8_ROWS(r0, r1, r3, r4), pix_tmp
    lea    r0, [r0+r3*8]										;lea eax , [eax + 8*stride]		;pix - 4 + 8*stride
    lea    r1, [r1+r3*8]										;lea ecx , [ecx + 8*stride]		;pix - 4 + 3*stride + 8*stride
    VO_TRANSPOSE6x8_MEM  VO_PASS_8_ROWS(r0, r1, r3, r4), pix_tmp+8

    ; vertical filter
    lea    r0, [pix_tmp+0x30]
    PUSH   dword [r6 + 24]
    PUSH   dword [r6 + 20]
    PUSH   dword [r6 + 16]
    PUSH   dword 16
    PUSH   dword r0
    call   vo_prefix %+ _deblock_v_luma_8_sse2_aligned_16
    ADD    rsp, 20

    ; transpose 16x4 -> original space  (only the middle 4 rows were changed by the filter)
    ;mov    r0, r0mp
    mov     r0, [r6 + 8]
    sub    r0, 2

    movq   m0, [pix_tmp+0x10]
    movq   m1, [pix_tmp+0x20]
    lea    r1, [r0+r4]
    movq   m2, [pix_tmp+0x30]
    movq   m3, [pix_tmp+0x40]
    VO_TRANSPOSE8x4B_STORE  VO_PASS_8_ROWS(r0, r1, r3, r4)

    lea    r0, [r0+r3*8]
    lea    r1, [r1+r3*8]
    movq   m0, [pix_tmp+0x18]
    movq   m1, [pix_tmp+0x28]
    movq   m2, [pix_tmp+0x38]
    movq   m3, [pix_tmp+0x48]
    VO_TRANSPOSE8x4B_STORE  VO_PASS_8_ROWS(r0, r1, r3, r4)
	
	mov rsp, r6
	sub rsp, 8
	pop r4
	pop r3
	leave
	ret	
%endmacro

INIT_XMM sse2
VO_DEBLOCK_LUMA


%macro VO_LUMA_INTRA_P012 4 ; p0..p3 in memory
    mova  t0, p2
    mova  t1, p0
    pavgb t0, p1
    pavgb t1, q0

    pavgb t0, t1 ; ((p2+p1+1)/2 + (p0+q0+1)/2 + 1)/2
    mova  t5, t1

    mova  t2, p2
    mova  t3, p0
    paddb t2, p1
    paddb t3, q0

    paddb t2, t3
    mova  t3, t2
    mova  t4, t2
    psrlw t2, 1
    pavgb t2, mpb_0
    pxor  t2, t0
    pand  t2, mpb_1
    psubb t0, t2 ; p1' = (p2+p1+p0+q0+2)/4;


    mova  t1, p2
    mova  t2, p2
    pavgb t1, q1
    psubb t2, q1

    paddb t3, t3
    psubb t3, t2 ; p2+2*p1+2*p0+2*q0+q1
    pand  t2, mpb_1
    psubb t1, t2
    pavgb t1, p1
    pavgb t1, t5 ; (((p2+q1)/2 + p1+1)/2 + (p0+q0+1)/2 + 1)/2
    psrlw t3, 2
    pavgb t3, mpb_0
    pxor  t3, t1
    pand  t3, mpb_1
    psubb t1, t3 ; p0'a = (p2+2*p1+2*p0+2*q0+q1+4)/8

    pxor  t3, p0, q1
    pavgb t2, p0, q1
    pand  t3, mpb_1
    psubb t2, t3
    pavgb t2, p1 ; p0'b = (2*p1+p0+q0+2)/4

    pxor  t1, t2
    pxor  t2, p0
    pand  t1, mask1p
    pand  t2, mask0
    pxor  t1, t2
    pxor  t1, p0
    mova  %1, t1 ; store p0

    mova  t1, %4 ; p3
    paddb t2, t1, p2
    pavgb t1, p2
    pavgb t1, t0 ; (p3+p2+1)/2 + (p2+p1+p0+q0+2)/4
    paddb t2, t2
    paddb t2, t4 ; 2*p3+3*p2+p1+p0+q0
    psrlw t2, 2
    pavgb t2, mpb_0
    pxor  t2, t1
    pand  t2, mpb_1
    psubb t1, t2 ; p2' = (2*p3+3*p2+p1+p0+q0+4)/8

    pxor  t0, p1
    pxor  t1, p2
    pand  t0, mask1p
    pand  t1, mask1p
    pxor  t0, p1
    pxor  t1, p2
    mova  %2, t0 ; store p1
    mova  %3, t1 ; store p2
%endmacro

%macro VO_LUMA_INTRA_SWAP_PQ 0
    %define q1 m0
    %define q0 m1
    %define p0 m2
    %define p1 m3
    %define p2 q2
    %define mask1p mask1q
%endmacro


%macro VO_DEBLOCK_LUMA_INTRA 0
    %define p1 m0
    %define p0 m1
    %define q0 m2
    %define q1 m3
    %define t0 m4
    %define t1 m5
    %define t2 m6
    %define t3 m7

    %define spill(x) [esp+16*x]
    %define p2 [r4+r1]
    %define q2 [r0+2*r1]
    %define t4 spill(0)
    %define t5 spill(1)
    %define mask0 spill(2)
    %define mask1p spill(3)
    %define mask1q spill(4)
    %define mpb_0 [pb_0]
    %define mpb_1 [pb_1]


;----------------------------------------------------------------------------------------------------
; void _vo_deblock_v_luma_intra_8_sse2_aligned_16( uint8_t *pix, int stride, int alpha, int beta )
;----------------------------------------------------------------------------------------------------
vo_global deblock_v_luma_intra_8_sse2_aligned_16

	push   r6
	mov    r6, rsp
	
	push   r3
	push   r4
	push   r5
	sub    rsp, 0x50
	and    rsp, ~15
	
	
	mov    r0,[r6 + 8 ]
	mov    r1,[r6 + 12]
	mov    r2,[r6 + 16]
	mov    r3,[r6 + 20]
	
    lea     r4, [r1*4]
    lea     r5, [r1*3] ; 3*stride
    dec     r2d        ; alpha-1
    jl done
    neg     r4
    dec     r3d        ; beta-1
    jl done
    add     r4, r0     ; pix-4*stride
    mova    p1, [r4+2*r1]
    mova    p0, [r4+r5]
    mova    q0, [r0]
    mova    q1, [r0+r1]

    VO_LOAD_MASK r2d, r3d, t5 ; m5=beta-1, t5=alpha-1, m7=mask0
    mova    m4, t5
    mova    mask0, m7
    pavgb   m4, [pb_0]
    pavgb   m4, [pb_1] ; alpha/4+1
    VO_DIFF_GT2 p0, q0, m4, m6, m7 ; m6 = |p0-q0| > alpha/4+1
    pand    m6, mask0
    VO_DIFF_GT2 p0, p2, m5, m4, m7 ; m4 = |p2-p0| > beta-1
    pand    m4, m6
    mova    mask1p, m4
    VO_DIFF_GT2 q0, q2, m5, m4, m7 ; m4 = |q2-q0| > beta-1
    pand    m4, m6
    mova    mask1q, m4

    VO_LUMA_INTRA_P012 [r4+r5], [r4+2*r1], [r4+r1], [r4]
    VO_LUMA_INTRA_SWAP_PQ
    VO_LUMA_INTRA_P012 [r0], [r0+r1], [r0+2*r1], [r0+r5]
done:
	mov rsp, r6
	sub rsp, 12
	
    pop r5
    pop r4
    pop r3
    
    leave
    ret
	lea r4 , [r4]
	
	
INIT_MMX cpuname
;----------------------------------------------------------------------------------------------------
; void _vo_deblock_h_luma_intra_8_sse2_aligned_16( uint8_t *pix, int stride, int alpha, int beta )
;----------------------------------------------------------------------------------------------------
vo_global deblock_h_luma_intra_8_sse2_aligned_16
	push   r6
	mov    r6, rsp
	push   r3
	
	sub    rsp, 0x90
	and    rsp, ~15
	
	mov    r0, [r6 + 8]
	mov    r1, [r6 + 12]
	
    lea    r3,  [r1*3]
    sub    r0,  4
    lea    r2,  [r0+r3]
    %define pix_tmp rsp

    ; transpose 8x16 -> tmp space
    VO_TRANSPOSE8x8_MEM  VO_PASS_8_ROWS(r0, r2, r1, r3), VO_PASS_8_ROWS(pix_tmp, pix_tmp+0x30, 0x10, 0x30)
    lea    r0,  [r0+r1*8]
    lea    r2,  [r2+r1*8]
    VO_TRANSPOSE8x8_MEM  VO_PASS_8_ROWS(r0, r2, r1, r3), VO_PASS_8_ROWS(pix_tmp+8, pix_tmp+0x38, 0x10, 0x30)

    lea    r0,  [pix_tmp+0x40]
    PUSH   dword [r6 + 20]
    PUSH   dword [r6 + 16]
    PUSH   dword 16
    PUSH   r0
    call   vo_prefix %+ _deblock_v_luma_intra_8_sse2_aligned_16
    ADD    rsp, 16
    mov    r1,  [r6 + 12]
    mov    r0,  [r6 + 8]
    lea    r3,  [r1*3]
    sub    r0,  4
    lea    r2,  [r0+r3]
    ; transpose 16x6 -> original space (but we can't write only 6 pixels, so really 16x8)
    VO_TRANSPOSE8x8_MEM  VO_PASS_8_ROWS(pix_tmp, pix_tmp+0x30, 0x10, 0x30), VO_PASS_8_ROWS(r0, r2, r1, r3)
    lea    r0,  [r0+r1*8]
    lea    r2,  [r2+r1*8]
    VO_TRANSPOSE8x8_MEM  VO_PASS_8_ROWS(pix_tmp+8, pix_tmp+0x38, 0x10, 0x30), VO_PASS_8_ROWS(r0, r2, r1, r3)
    
    mov rsp, r6
    sub rsp, 4
    pop r3
    leave
    ret
%endmacro

INIT_XMM sse2
VO_DEBLOCK_LUMA_INTRA


%macro VO_CHROMA_V_START 0
    dec    r2d      ; alpha-1
    dec    r3d      ; beta-1
    mov    t5, r0
    sub    t5, r1
    sub    t5, r1
%endmacro

%macro VO_CHROMA_H_START 0
    dec    r2d
    dec    r3d
    sub    r0, 2
    lea    t6, [r1*3]
    mov    t5, r0
    add    r0, t6
%endmacro

%macro DEBLOCK_CHROMA 0

%define t5 r5
%define t6 r6

vo_global deblock_v_chroma_8_mmxext_aligned_16
	  push        r3  
	  push        r4  
	  push        r5  
	  mov         r0,[rsp+10h] 
	  mov         r1,[rsp+14h] 
	  mov         r2,[rsp+18h] 
	  mov         r3,[rsp+1Ch] 
	  mov         r4,[rsp+20h] 
	  dec         r2  
	  dec         r3  
	  mov         r5,r0 
	  sub         r5,r1 
	  sub         r5,r1 
	  movq        m0, [r5] 
	  movq        m1, [r5+r1] 
	  movq        m2, [r0] 
	  movq        m3, [r0+r1] 
	  call        vo_chroma_inter_body_mmxext
	  movq       [r5+r1],m1 
	  movq       [r0],m2 
	  pop         r5  
	  pop         r4  
	  pop         r3  
	  ret              
	  lea         r4,[r4]

vo_global deblock_h_chroma_8_mmxext_aligned_16
	push r3
	push r4
	push r5
	push r6
	
	mov  r0, [rsp + 20]
	mov  r1, [rsp + 24]
	mov  r2, [rsp + 28]
	mov  r3, [rsp + 32]
	mov  r4, [rsp + 36]
	
	%define buf0 [rsp + 20]
    %define buf1 [rsp + 28]
    
    VO_CHROMA_H_START
    VO_TRANSPOSE4x8_LOAD  bw, wd, dq, VO_PASS_8_ROWS(t5, r0, r1, t6)
    movq  buf0, m0
    movq  buf1, m3
    VO_LOAD_MASK  r2, r3
    movd       m6, [r4] ; tc0
    punpcklbw  m6, m6
    pand       m7, m6
    VO_DEBLOCK_P0_Q0
    movq  m0, buf0
    movq  m3, buf1
    VO_TRANSPOSE8x4B_STORE VO_PASS_8_ROWS(t5, r0, r1, t6)
	
	pop r6
	pop r5
	pop r4
	pop r3
	
	ret
	lea r4 , [r4]
	
ALIGN 16
vo_chroma_inter_body_mmxext:
    VO_LOAD_MASK  r2d, r3d
    movd       m6, [r4] ; tc0
    punpcklbw  m6, m6
    pand       m7, m6
    VO_DEBLOCK_P0_Q0
    ret
	
%endmacro 

; in: %1=p0 %2=p1 %3=q1
; out: p0 = (p0 + q1 + 2*p1 + 2) >> 2
%macro VO_CHROMA_INTRA_P0 3	;这部分运算没有看懂
    movq    m4, %1			;movq mm4 , p0
    pxor    m4, %3			;pxor mm4 , q1	; mm4 = p0 ^ q1
    pand    m4, [pb_1]		; m4 = (p0^q1)&1
    pavgb   %1, %3			;(p0 + q1 + 1) >> 1
    psubusb %1, m4			;(p0 + q1 + 1) >> 1  - (p0^q1)&1
    pavgb   %1, %2         ; dst = avg(p1, avg(p0,q1) - ((p0^q1)&1))
							;((po + q1 + 1 + 2*p1) >> 1 - (p0^q1)&1 ) >> 1
%endmacro


INIT_MMX mmxext
DEBLOCK_CHROMA

	
%macro DEBLOCK_CHROMA_INTRA 0

;-------------------------------------------------------------------------------------------------------
; void vo_deblock_v_chroma_intra_8_mmxext_aligned_16( uint8_t *pix, int stride, int alpha, int beta )
;-------------------------------------------------------------------------------------------------------

%define t5 r4
%define t6 r5

vo_global deblock_v_chroma_intra_8_mmxext_aligned_16
	push r6
	mov  r6, rsp
	push r3
	push r4
	
	mov  r0, [r6 + 8]
	mov  r1, [r6 + 12]
	mov  r2, [r6 + 16]
	mov  r3, [r6 + 20]
	
	
    VO_CHROMA_V_START
    movq  m0, [t5]
    movq  m1, [t5+r1]
    movq  m2, [r0]
    movq  m3, [r0+r1]
    call vo_chroma_intra_body_mmxext
    movq  [t5+r1], m1
    movq  [r0], m2
	
	mov rsp , r6
	sub rsp, 8
	pop r4
	pop r3
	
	leave
	ret
	;lea esi , [esi]
	
INIT_MMX mmxext

;-----------------------------------------------------------------------------
; void vo_deblock_h_chroma_intra_8_mmxext_aligned_16( uint8_t *pix, int stride, int alpha, int beta )
;-----------------------------------------------------------------------------
vo_global deblock_h_chroma_intra_8_mmxext_aligned_16
	push r6
	mov  r6, rsp
	push r3
	push r4
	push r5
	
	mov r0 , [rsp + 20]
	mov r1 , [rsp + 24]
	mov r2 , [rsp + 28]
	mov r3 , [rsp + 32]
	
	VO_CHROMA_H_START
    ;VO_PASS_8_ROWS(pix - 2 , pix - 2 - 3*stride , stride , 3*stride)
    ;VO_PASS_8_ROWS(esi , eax , ecx , edi)
    VO_TRANSPOSE4x8_LOAD  bw, wd, dq, VO_PASS_8_ROWS(t5, r0, r1, t6)
    call vo_chroma_intra_body_mmxext
    VO_TRANSPOSE8x4B_STORE VO_PASS_8_ROWS(t5, r0, r1, t6)

	mov rsp, r6
	sub rsp, 12
	pop r5
	pop r4
	pop r3
	
	leave
	ret
	lea r4 , [r4]
	
%endmacro

INIT_MMX mmxext
DEBLOCK_CHROMA_INTRA


ALIGN 16
vo_chroma_intra_body_mmxext:
    VO_LOAD_MASK r2d, r3d
    movq   m5, m1
    movq   m6, m2
    VO_CHROMA_INTRA_P0  m1, m0, m3		;VO_CHROMA_INTRA_P0 p1, p0, q1  m1 <-- p0
    VO_CHROMA_INTRA_P0  m2, m3, m0		;VO_CHROMA_INTRA_P0 q0, q1, p1	 m2 <-- q0
    psubb  m1, m5					;
    psubb  m2, m6
    pand   m1, m7
    pand   m2, m7
    paddb  m1, m5
    paddb  m2, m6
    ret
    
    
;void _vo_deblock_v_luma_8_sse2(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
vo_global deblock_v_luma_8_sse2
	push	ebp
	mov		ebp,	esp
	push	edi
	push	esi
	push	ebx
	
	sub		esp,	48
	and		esp,	~15
	
	;----------------main body begin----------------
	
	mov		eax,	[ebp + 8]		;eax = pix
	mov		ecx,	[ebp + 12]		;ecx = stride
	mov		edx,	[ebp + 16]		;edx = alpha
	mov		ebx,	[ebp + 20]		;ebx = beta
	mov		esi,	[ebp + 24]		;esi = tc0
	
	mov		edi,	eax
	sub		edi,	ecx
	sub		edi,	ecx
	sub		edi,	ecx					;edi  = pix - 3*stride
	
	movdqa	xmm0,	[edi + ecx]		;xmm0 = pix - 2*stride	;p1
	movdqa	xmm1,	[edi + 2*ecx]		;xmm1 = pix - stride	;p0
	movdqa	xmm2,	[eax]				;xmm2 = pix				;q0
	movdqa	xmm3,	[eax + ecx]		;xmm3 = pix + stride	;q1
	
	movdqa	[esp+32],	xmm3
	
	;-------------if condition:(tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)---------------
	;-----|p0 - q0| - (alpha - 1)
	movdqa		xmm6,	xmm1
	psubusb	xmm6,	xmm2		;p0 - q0
	movdqa		xmm7,	xmm2
	psubusb	xmm7,	xmm1		;q0 - p0
	por			xmm6,	xmm7		;|p0 - q0|
	
	dec			edx
	movd		xmm4,	edx			;
	pshuflw		xmm4,	xmm4,	0
    punpcklqdq	xmm4,	xmm4
    packuswb	xmm4,	xmm4
    psubusb	xmm6,	xmm4		;xmm6 = |p0 - q0| - (alpha - 1)
    ;-----|p0 - q0| - (alpha - 1)
    
	
	;-----|p1 - p0| - (beta - 1)
	movdqa		xmm5,	xmm0
	psubusb	xmm5,	xmm1		;p1 - p0
	movdqa		xmm7,	xmm1
	psubusb	xmm7,	xmm0		;p0 - p1
	por			xmm5,	xmm7		;xmm5 = |p1 - p0|
	
	dec			ebx					;
	movd		xmm4,	ebx			;
	pshuflw		xmm4,	xmm4,	0	;
    punpcklqdq	xmm4,	xmm4		;
    packuswb	xmm4,	xmm4
    psubusb	xmm5,	xmm4		;xmm5 = |p1 - p0| - (beta - 1)
    ;-----|p1 - p0| - (beta - 1)
	
    por			xmm5,	xmm6		;(|p1 - p0| - (beta - 1)) || (|p0 - q0| - (alpha - 1))
    
    ;-----|q1 - q0| - (beta - 1)
	movdqa		xmm6,	xmm2
	psubusb	xmm6,	xmm3		;q1 - q0
	movdqa		xmm7,	xmm3
	psubusb	xmm7,	xmm2		;q0 - q1
	por			xmm6,	xmm7		;|q1 - q0|
	
    psubusb	xmm6,	xmm4		;|q1 - q0| - (beta - 1)
    ;-----|q1 - q0| - (beta - 1)
	
	pxor		xmm7,	xmm7		;xmm7 = 0
	por			xmm5,	xmm6		;(|p1 - p0| > (beta - 1)) || (|p0 - q0| > (alpha - 1)) || (|q1 - q0| > (beta - 1))
	pcmpeqb	xmm5,	xmm7		;(|p1 - p0| <= (beta - 1)) && (|p0 - q0| <= (alpha - 1)) && (|q1 - q0| <= (beta - 1))
									;(|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	
	;xmm5 is the output of this block, mask
	;xmm4 is the beta , please keep not changing
	movd		xmm7,	[esi]
	punpcklbw	xmm7,	xmm7
	punpcklbw	xmm7,	xmm7		;
	;xmm7 = tc0[0] tc0[0] tc0[0] tc0[0] tc0[1] tc0[1] tc0[1] tc0[1] tc0[2] tc0[2] tc0[2] tc0[2] tc0[3] tc0[3] tc0[3] tc0[3]
	movdqa		[esp + 16] , xmm7	;save xmm4 to esp + 16 temporarily
	
	pcmpeqb	xmm3,	xmm3		;xmm3 = -1
	pcmpgtb	xmm7,	xmm3		;tc > -1
	pand		xmm7,	xmm5		;m7 = (tc > -1) && (|p1 - p0| <= (beta - 1)) && (|p0 - q0| <= (alpha - 1)) && (|q1 - q0| <= (beta - 1))
									;m7 = (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	movdqa		[esp],	xmm7		
	
	;-------------if condition:(tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)---------------
	
	
	;------------update q1 ------------
	movdqa		xmm3,	[eax + 2*ecx]		;q2 = [eax + 2*ecx]		q0 = [eax]
	movdqa		xmm5,	xmm3
	psubusb	xmm5,	xmm2
	movdqa		xmm6,	xmm2
	psubusb	xmm6,	xmm3
	
	;------- wait for test -------
	por			xmm5,	xmm6
	psubusb	xmm5,	xmm4
	pcmpeqb	xmm5,	[pb_0]		;m5 = |q2-q0| < beta
	;------- wait for test -------
	
	;mask	to		[esp]			;m7 = (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	;tc		to		[esp + 16]		;tc0
	pand		xmm5,	xmm7		;m5 = (|q2-q0| < beta) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	pand		xmm7,	[esp + 16]	;m7 = tc0 && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	movdqa		xmm6,	xmm7
	psubb		xmm6,	xmm5		;m6 = (tc0 - (|q2-q0|<beta)) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
									;attention: the value of (|q2-q0|<beta) is whether 0 or 0xFF , if the value is 0xFF , it is equal -1 , then
									;the value of tc0 - (|q2-q0|<beta) is whether tc0 or tc0 + 1
	pand		xmm5,	xmm7		;m5 = tc0 && (|q2-q0| < beta) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
									;m5 = tc0 & mask
	
	;xmm3 = q2
	movdqa		xmm7,	xmm2				;m7 = q0
	pavgb		xmm7,	xmm1				;m7 = (p0+q0+1)>>1
	pavgb		xmm3,	xmm7				;m3 = (q2 + (p0+q0+1)>>1 + 1) >> 1
	pxor		xmm7,	[eax + 2*ecx]		;m7 = ((p0+q0+1)>>1) ^ q2
	pand		xmm7,	[pb_1]			;m7 = (((p0+q0+1)>>1) ^ q2) & 1
	psubb		xmm3,	xmm7				;m3 = (q2 + (p0+q0+1)>>1 + 1) >> 1 - (((p0+q0+1)>>1) ^ q2) & 1
	
	movdqa		xmm7,	[eax+ecx]			;q1
	psubb		xmm7,	xmm5				;q1 - tc0 
	paddb		xmm5,	[eax+ecx]			;q1 + tc0
	pmaxub		xmm7,	xmm3
	pminub		xmm7,	xmm5
	movdqa		[eax+ecx],	xmm7			;update q1
	;------------update q1 ------------
	
	
	;------------update p1 ------------
	movdqa		xmm3,	[edi]				;xmm3 = p2
	movdqa		xmm5,	xmm3
	psubusb	xmm5,	xmm1				;p2 - p0
	movdqa		xmm7,	xmm1
	psubusb	xmm7,	xmm3				;p0 - p2
	por			xmm5,	xmm7				;|p2 - p0|
	psubusb	xmm5,	xmm4				;|p2 - p0| - beta
	pcmpeqb	xmm5,	[pb_0]			;|p2 - p0| < beta
	
	movdqa		xmm7,	[esp]				;m7 = (t0 >=0) && |p0-q0| < alpha && |p1-p0| < beta && |q1-q0| < beta
	pand		xmm5,	xmm7				;m5 = (|q2-q0| < beta) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	pand		xmm7,	[esp + 16]			;m7 = tc0 && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
	psubb		xmm6,	xmm5				;m6 = (tc0 - (|q2-q0|<beta) - (|p2-p0|<beta)) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
											;attention: the value of (|q2-q0|<beta) is whether 0 or 0xFF , if the value is 0xFF , it is equal -1 , then
											;the value of tc0 - (|q2-q0|<beta) is whether tc0 or tc0 + 1
	pand		xmm5,	xmm7				;m5 = tc0 && (|q2-q0| < beta) && (tc >= 0) && (|p1 - p0| < beta) && (|p0 - q0| < alpha) && (|q1 - q0| < beta)
											;m5 = tc0 & mask
	movdqa		xmm4,	[eax+ecx]			;xmm4 = q1
											
	;xmm3 = p2
	movdqa		xmm7,	xmm2
	pavgb		xmm7,	xmm1				;m7 = (p0+q0+1)>>1
	pavgb		xmm3,	xmm7				;m3 = (p2 + (p0+q0+1)>>1 + 1) >> 1
	pxor		xmm7,	[edi]				;m7 = ((p0+q0+1)>>1) ^ p2
	pand		xmm7,	[pb_1]			;m7 = (((p0+q0+1)>>1) ^ p2) & 1
	psubb		xmm3,	xmm7				;m3 = (p2 + (p0+q0+1)>>1 + 1) >> 1 - (((p0+q0+1)>>1) ^ p2) & 1
	
	movdqa		xmm7,	[edi+ecx]			;p1
	psubusb	xmm7,	xmm5				;p1 - tc0 
	paddusb	xmm5,	[edi+ecx]			;p1 + tc0
	pmaxub		xmm7,	xmm3
	pminub		xmm7,	xmm5
	
	movdqa		[edi+ecx],	xmm7			;update p1
	
	;------------ update p1 ------------
	;m6 = tc & mask
	;m4 = q1
	
	movdqa		xmm4,	[esp+32]
	
	pcmpeqb	xmm3,	xmm3
	movdqa		xmm5,	xmm1
	pxor		xmm5,	xmm2				;m5 = p0^q0
	pxor		xmm4,	xmm3				;|m4| = 255 - q1
	pand		xmm5,	[pb_1]			;m5 = (p0^q0)&1
	pavgb		xmm4,	xmm0				;m4 = (p1 - q1 + 256)>>1
	pxor		xmm3,	xmm1				;|m4| = 255 - p0
	pavgb		xmm4,	[pb_3]			;m4 = ( ((p1 - q1 + 256)>>1) + 4)>>1 = 66 + (p1-q1)>>2
	pavgb		xmm3,	xmm2				;m3 = (q0 - p0 + 256)>>1 = (q0-p0)>>1 + 128
	pavgb		xmm4,	xmm5				;m4 = ( 66 + (p1-q1)>>2 + (p0^q0)&1 + 1)>>1
	movdqa		xmm7,	[pb_A1]			;m6 = 0xA1 = 161
	paddusb	xmm4,	xmm3				;m4 = d+128+33 = 128 + 33 + ((q0-p0)>>1 + (p1-q1)>>2 + (p0^q0)&1 + 1)>>1)>>1
	psubusb	xmm7,	xmm4				;m6 = - ((q0-p0)>>1 + (p1-q1)>>2 + (p0^q0)&1 + 1)>>1)>>1
	psubusb	xmm4,	[pb_A1]			;m4 = ((q0-p0)>>1 + (p1-q1)>>2 + (p0^q0)&1 + 1)>>1)>>1
	
	pminub		xmm7,	xmm6			;
    pminub		xmm4,	xmm6			;
    psubusb	xmm1,	xmm7			;m1 = p0 - ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1 
    psubusb	xmm2,	xmm4			;m2 = q0 + ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1
    paddusb	xmm1,	xmm4			;m1 = p0 - ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1  + ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1
    paddusb	xmm2,	xmm7			;m2 = q0 + ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1  - ( (p1-q1)>>2 + (p0^q0)&1 + (q0-p0) + 1)>>1
	
	movdqa		[edi+2*ecx],	xmm1
    movdqa		[eax],			xmm2
	
	
	;----------------main body end  ----------------
	
	mov		esp,	ebp
	sub		esp,	12
	pop		ebx
	pop		esi
	pop		edi
	leave
	ret