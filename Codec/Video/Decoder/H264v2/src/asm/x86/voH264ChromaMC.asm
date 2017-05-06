%include "voH264Util.asm"

SECTION_RODATA

rnd_rv40_2d_tbl: times 4 dw  0
                 times 4 dw 16
                 times 4 dw 32
                 times 4 dw 16
                 times 4 dw 32
                 times 4 dw 28
                 times 4 dw 32
                 times 4 dw 28
                 times 4 dw  0
                 times 4 dw 32
                 times 4 dw 16
                 times 4 dw 32
                 times 4 dw 32
                 times 4 dw 28
                 times 4 dw 32
                 times 4 dw 28
rnd_rv40_1d_tbl: times 4 dw  0
                 times 4 dw  2
                 times 4 dw  4
                 times 4 dw  2
                 times 4 dw  4
                 times 4 dw  3
                 times 4 dw  4
                 times 4 dw  3
                 times 4 dw  0
                 times 4 dw  4
                 times 4 dw  2
                 times 4 dw  4
                 times 4 dw  4
                 times 4 dw  3
                 times 4 dw  4
                 times 4 dw  3

cextern pw_3
cextern pw_4
cextern pw_8
pw_28: times 8 dw 28
cextern pw_32
cextern pw_64

SECTION .text

%macro mv0_pixels_mc8 0
    lea           r4, [r2*2 ]
.next4rows:
    movq         mm0, [r1   ]
    movq         mm1, [r1+r2]
    add           r1, r4
    CHROMAMC_AVG mm0, [r0   ]
    CHROMAMC_AVG mm1, [r0+r2]
    movq     [r0   ], mm0
    movq     [r0+r2], mm1
    add           r0, r4
    movq         mm0, [r1   ]
    movq         mm1, [r1+r2]
    add           r1, r4
    CHROMAMC_AVG mm0, [r0   ]
    CHROMAMC_AVG mm1, [r0+r2]
    movq     [r0   ], mm0
    movq     [r0+r2], mm1
    add           r0, r4
    sub          r3d, 4
    jne .next4rows
%endmacro

%macro chroma_mc8_mmx_func 2-3
%ifidn %2, rv40
%ifdef PIC
%define rnd_1d_rv40 r8
%define rnd_2d_rv40 r8
%define extra_regs 2
%else ; no-PIC
%define rnd_1d_rv40 rnd_rv40_1d_tbl
%define rnd_2d_rv40 rnd_rv40_2d_tbl
%define extra_regs 1
%endif ; PIC
%else
%define extra_regs 0
%endif ; rv40
; put/avg_h264_chroma_mc8_*(uint8_t *dst /*align 8*/, uint8_t *src /*align 1*/,
;                           int stride, int h, int mx, int my)
cglobal %1_%2_chroma_mc8%3, 6, 7 + extra_regs, 0
%if ARCH_X86_64
    movsxd        r2, r2d
%endif
    mov          r6d, r5d
    or           r6d, r4d
    jne .at_least_one_non_zero
    ; mx == 0 AND my == 0 - no filter needed
    mv0_pixels_mc8
    REP_RET

.at_least_one_non_zero:
%ifidn %2, rv40
%if ARCH_X86_64
    mov           r7, r5
    and           r7, 6         ; &~1 for mx/my=[0,7]
    lea           r7, [r7*4+r4]
    sar          r7d, 1
%define rnd_bias r7
%define dest_reg r0
%else ; x86-32
    mov           r0, r5
    and           r0, 6         ; &~1 for mx/my=[0,7]
    lea           r0, [r0*4+r4]
    sar          r0d, 1
%define rnd_bias r0
%define dest_reg r5
%endif
%else ; vc1, h264
%define rnd_bias  0
%define dest_reg r0
%endif

    test         r5d, r5d
    mov           r6, 1
    je .my_is_zero
    test         r4d, r4d
    mov           r6, r2        ; dxy = x ? 1 : stride
    jne .both_non_zero
.my_is_zero:
    ; mx == 0 XOR my == 0 - 1 dimensional filter only
    or           r4d, r5d       ; x + y

%ifidn %2, rv40
%ifdef PIC
    lea           r8, [rnd_rv40_1d_tbl]
%endif
%if ARCH_X86_64 == 0
    mov           r5, r0m
%endif
%endif

    movd          m5, r4d
    movq          m4, [pw_8]
    movq          m6, [rnd_1d_%2+rnd_bias*8] ; mm6 = rnd >> 3
    punpcklwd     m5, m5
    punpckldq     m5, m5        ; mm5 = B = x
    pxor          m7, m7
    psubw         m4, m5        ; mm4 = A = 8-x

.next1drow:
    movq          m0, [r1   ]   ; mm0 = src[0..7]
    movq          m2, [r1+r6]   ; mm1 = src[1..8]

    movq          m1, m0
    movq          m3, m2
    punpcklbw     m0, m7
    punpckhbw     m1, m7
    punpcklbw     m2, m7
    punpckhbw     m3, m7
    pmullw        m0, m4        ; [mm0,mm1] = A * src[0..7]
    pmullw        m1, m4
    pmullw        m2, m5        ; [mm2,mm3] = B * src[1..8]
    pmullw        m3, m5

    paddw         m0, m6
    paddw         m1, m6
    paddw         m0, m2
    paddw         m1, m3
    psrlw         m0, 3
    psrlw         m1, 3
    packuswb      m0, m1
    CHROMAMC_AVG  m0, [dest_reg]
    movq  [dest_reg], m0        ; dst[0..7] = (A * src[0..7] + B * src[1..8] + (rnd >> 3)) >> 3

    add     dest_reg, r2
    add           r1, r2
    dec           r3d
    jne .next1drow
    REP_RET

.both_non_zero: ; general case, bilinear
    movd          m4, r4d         ; x
    movd          m6, r5d         ; y
%ifidn %2, rv40
%ifdef PIC
    lea           r8, [rnd_rv40_2d_tbl]
%endif
%if ARCH_X86_64 == 0
    mov           r5, r0m
%endif
%endif
    mov           r6, rsp         ; backup stack pointer
    and          rsp, ~(mmsize-1) ; align stack
    sub          rsp, 16          ; AA and DD

    punpcklwd     m4, m4
    punpcklwd     m6, m6
    punpckldq     m4, m4          ; mm4 = x words
    punpckldq     m6, m6          ; mm6 = y words
    movq          m5, m4
    pmullw        m4, m6          ; mm4 = x * y
    psllw         m5, 3
    psllw         m6, 3
    movq          m7, m5
    paddw         m7, m6
    movq     [rsp+8], m4          ; DD = x * y
    psubw         m5, m4          ; mm5 = B = 8x - xy
    psubw         m6, m4          ; mm6 = C = 8y - xy
    paddw         m4, [pw_64]
    psubw         m4, m7          ; mm4 = A = xy - (8x+8y) + 64
    pxor          m7, m7
    movq     [rsp  ], m4

    movq          m0, [r1  ]      ; mm0 = src[0..7]
    movq          m1, [r1+1]      ; mm1 = src[1..8]
.next2drow:
    add           r1, r2

    movq          m2, m0
    movq          m3, m1
    punpckhbw     m0, m7
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpckhbw     m3, m7
    pmullw        m0, [rsp]
    pmullw        m2, [rsp]
    pmullw        m1, m5
    pmullw        m3, m5
    paddw         m2, m1          ; mm2 = A * src[0..3] + B * src[1..4]
    paddw         m3, m0          ; mm3 = A * src[4..7] + B * src[5..8]

    movq          m0, [r1]
    movq          m1, m0
    punpcklbw     m0, m7
    punpckhbw     m1, m7
    pmullw        m0, m6
    pmullw        m1, m6
    paddw         m2, m0
    paddw         m3, m1          ; [mm2,mm3] += C * src[0..7]

    movq          m1, [r1+1]
    movq          m0, m1
    movq          m4, m1
    punpcklbw     m0, m7
    punpckhbw     m4, m7
    pmullw        m0, [rsp+8]
    pmullw        m4, [rsp+8]
    paddw         m2, m0
    paddw         m3, m4          ; [mm2,mm3] += D * src[1..8]
    movq          m0, [r1]

    paddw         m2, [rnd_2d_%2+rnd_bias*8]
    paddw         m3, [rnd_2d_%2+rnd_bias*8]
    psrlw         m2, 6
    psrlw         m3, 6
    packuswb      m2, m3
    CHROMAMC_AVG  m2, [dest_reg]
    movq  [dest_reg], m2          ; dst[0..7] = ([mm2,mm3] + rnd) >> 6

    add     dest_reg, r2
    dec          r3d
    jne .next2drow
    mov          rsp, r6          ; restore stack pointer
    RET
%endmacro

%macro chroma_mc4_mmx_func 2
%define extra_regs 0
%ifidn %2, rv40
%ifdef PIC
%define extra_regs 1
%endif ; PIC
%endif ; rv40
cglobal %1_%2_chroma_mc4, 6, 6 + extra_regs, 0
%if ARCH_X86_64
    movsxd        r2, r2d
%endif
    pxor          m7, m7
    movd          m2, r4d         ; x
    movd          m3, r5d         ; y
    movq          m4, [pw_8]
    movq          m5, [pw_8]
    punpcklwd     m2, m2
    punpcklwd     m3, m3
    punpcklwd     m2, m2
    punpcklwd     m3, m3
    psubw         m4, m2
    psubw         m5, m3

%ifidn %2, rv40
%ifdef PIC
   lea            r6, [rnd_rv40_2d_tbl]
%define rnd_2d_rv40 r6
%else
%define rnd_2d_rv40 rnd_rv40_2d_tbl
%endif
    and           r5, 6         ; &~1 for mx/my=[0,7]
    lea           r5, [r5*4+r4]
    sar          r5d, 1
%define rnd_bias r5
%else ; vc1, h264
%define rnd_bias 0
%endif

    movd          m0, [r1  ]
    movd          m6, [r1+1]
    add           r1, r2
    punpcklbw     m0, m7
    punpcklbw     m6, m7
    pmullw        m0, m4
    pmullw        m6, m2
    paddw         m6, m0

.next2rows:
    movd          m0, [r1  ]
    movd          m1, [r1+1]
    add           r1, r2
    punpcklbw     m0, m7
    punpcklbw     m1, m7
    pmullw        m0, m4
    pmullw        m1, m2
    paddw         m1, m0
    movq          m0, m1

    pmullw        m6, m5
    pmullw        m1, m3
    paddw         m6, [rnd_2d_%2+rnd_bias*8]
    paddw         m1, m6
    psrlw         m1, 6
    packuswb      m1, m1
    CHROMAMC_AVG4 m1, m6, [r0]
    movd        [r0], m1
    add           r0, r2

    movd          m6, [r1  ]
    movd          m1, [r1+1]
    add           r1, r2
    punpcklbw     m6, m7
    punpcklbw     m1, m7
    pmullw        m6, m4
    pmullw        m1, m2
    paddw         m1, m6
    movq          m6, m1
    pmullw        m0, m5
    pmullw        m1, m3
    paddw         m0, [rnd_2d_%2+rnd_bias*8]
    paddw         m1, m0
    psrlw         m1, 6
    packuswb      m1, m1
    CHROMAMC_AVG4 m1, m0, [r0]
    movd        [r0], m1
    add           r0, r2
    sub          r3d, 2
    jnz .next2rows
    REP_RET
%endmacro

%macro chroma_mc2_mmx_func 2
cglobal %1_%2_chroma_mc2, 6, 7, 0
%if ARCH_X86_64
    movsxd        r2, r2d
%endif

    mov          r6d, r4d
    shl          r4d, 16
    sub          r4d, r6d
    add          r4d, 8
    imul         r5d, r4d         ; x*y<<16 | y*(8-x)
    shl          r4d, 3
    sub          r4d, r5d         ; x*(8-y)<<16 | (8-x)*(8-y)

    movd          m5, r4d
    movd          m6, r5d
    punpckldq     m5, m5          ; mm5 = {A,B,A,B}
    punpckldq     m6, m6          ; mm6 = {C,D,C,D}
    pxor          m7, m7
    movd          m2, [r1]
    punpcklbw     m2, m7
    pshufw        m2, m2, 0x94    ; mm0 = src[0,1,1,2]

.nextrow:
    add           r1, r2
    movq          m1, m2
    pmaddwd       m1, m5          ; mm1 = A * src[0,1] + B * src[1,2]
    movd          m0, [r1]
    punpcklbw     m0, m7
    pshufw        m0, m0, 0x94    ; mm0 = src[0,1,1,2]
    movq          m2, m0
    pmaddwd       m0, m6
    paddw         m1, [rnd_2d_%2]
    paddw         m1, m0          ; mm1 += C * src[0,1] + D * src[1,2]
    psrlw         m1, 6
    packssdw      m1, m7
    packuswb      m1, m7
    CHROMAMC_AVG4 m1, m3, [r0]
    movd         r5d, m1
    mov         [r0], r5w
    add           r0, r2
    sub          r3d, 1
    jnz .nextrow
    REP_RET
%endmacro

%define rnd_1d_h264 pw_4
%define rnd_2d_h264 pw_32

%macro NOTHING 2-3
%endmacro
%macro DIRECT_AVG 2
    PAVGB         %1, %2
%endmacro
%macro COPY_AVG 3
    movd          %2, %3
    PAVGB         %1, %2
%endmacro

INIT_MMX mmx
%define CHROMAMC_AVG  NOTHING
%define CHROMAMC_AVG4 NOTHING
chroma_mc8_mmx_func put, h264, _rnd
chroma_mc4_mmx_func put, h264

INIT_MMX mmxext
chroma_mc2_mmx_func put, h264

%define CHROMAMC_AVG  DIRECT_AVG
%define CHROMAMC_AVG4 COPY_AVG
chroma_mc8_mmx_func avg, h264, _rnd
chroma_mc4_mmx_func avg, h264
chroma_mc2_mmx_func avg, h264

INIT_MMX 3dnow
chroma_mc8_mmx_func avg, h264, _rnd
chroma_mc4_mmx_func avg, h264

%macro chroma_mc8_ssse3_func 2-3
cglobal %1_%2_chroma_mc8%3, 6, 7, 8
%if ARCH_X86_64
    movsxd        r2, r2d
%endif
    mov          r6d, r5d
    or           r6d, r4d
    jne .at_least_one_non_zero
    ; mx == 0 AND my == 0 - no filter needed
    mv0_pixels_mc8
    REP_RET

.at_least_one_non_zero:
    test         r5d, r5d
    je .my_is_zero
    test         r4d, r4d
    je .mx_is_zero

    ; general case, bilinear
    mov          r6d, r4d
    shl          r4d, 8
    sub           r4, r6
    mov           r6, 8
    add           r4, 8           ; x*288+8 = x<<8 | (8-x)
    sub          r6d, r5d
    imul          r6, r4          ; (8-y)*(x*255+8) = (8-y)*x<<8 | (8-y)*(8-x)
    imul         r4d, r5d         ;    y *(x*255+8) =    y *x<<8 |    y *(8-x)

    movd          m7, r6d
    movd          m6, r4d
    movdqa        m5, [rnd_2d_%2]
    movq          m0, [r1  ]
    movq          m1, [r1+1]
    pshuflw       m7, m7, 0
    pshuflw       m6, m6, 0
    punpcklbw     m0, m1
    movlhps       m7, m7
    movlhps       m6, m6

.next2rows:
    movq          m1, [r1+r2*1   ]
    movq          m2, [r1+r2*1+1]
    movq          m3, [r1+r2*2  ]
    movq          m4, [r1+r2*2+1]
    lea           r1, [r1+r2*2]
    punpcklbw     m1, m2
    movdqa        m2, m1
    punpcklbw     m3, m4
    movdqa        m4, m3
    pmaddubsw     m0, m7
    pmaddubsw     m1, m6
    pmaddubsw     m2, m7
    pmaddubsw     m3, m6
    paddw         m0, m5
    paddw         m2, m5
    paddw         m1, m0
    paddw         m3, m2
    psrlw         m1, 6
    movdqa        m0, m4
    psrlw         m3, 6
%ifidn %1, avg
    movq          m2, [r0   ]
    movhps        m2, [r0+r2]
%endif
    packuswb      m1, m3
    CHROMAMC_AVG  m1, m2
    movq     [r0   ], m1
    movhps   [r0+r2], m1
    sub          r3d, 2
    lea           r0, [r0+r2*2]
    jg .next2rows
    REP_RET

.my_is_zero:
    mov          r5d, r4d
    shl          r4d, 8
    add           r4, 8
    sub           r4, r5          ; 255*x+8 = x<<8 | (8-x)
    movd          m7, r4d
    movdqa        m6, [rnd_1d_%2]
    pshuflw       m7, m7, 0
    movlhps       m7, m7

.next2xrows:
    movq          m0, [r1     ]
    movq          m1, [r1   +1]
    movq          m2, [r1+r2  ]
    movq          m3, [r1+r2+1]
    punpcklbw     m0, m1
    punpcklbw     m2, m3
    pmaddubsw     m0, m7
    pmaddubsw     m2, m7
%ifidn %1, avg
    movq          m4, [r0   ]
    movhps        m4, [r0+r2]
%endif
    paddw         m0, m6
    paddw         m2, m6
    psrlw         m0, 3
    psrlw         m2, 3
    packuswb      m0, m2
    CHROMAMC_AVG  m0, m4
    movq     [r0   ], m0
    movhps   [r0+r2], m0
    sub          r3d, 2
    lea           r0, [r0+r2*2]
    lea           r1, [r1+r2*2]
    jg .next2xrows
    REP_RET

.mx_is_zero:
    mov          r4d, r5d
    shl          r5d, 8
    add           r5, 8
    sub           r5, r4          ; 255*y+8 = y<<8 | (8-y)
    movd          m7, r5d
    movdqa        m6, [rnd_1d_%2]
    pshuflw       m7, m7, 0
    movlhps       m7, m7

.next2yrows:
    movq          m0, [r1     ]
    movq          m1, [r1+r2  ]
    movdqa        m2, m1
    movq          m3, [r1+r2*2]
    lea           r1, [r1+r2*2]
    punpcklbw     m0, m1
    punpcklbw     m2, m3
    pmaddubsw     m0, m7
    pmaddubsw     m2, m7
%ifidn %1, avg
    movq          m4, [r0   ]
    movhps        m4, [r0+r2]
%endif
    paddw         m0, m6
    paddw         m2, m6
    psrlw         m0, 3
    psrlw         m2, 3
    packuswb      m0, m2
    CHROMAMC_AVG  m0, m4
    movq     [r0   ], m0
    movhps   [r0+r2], m0
    sub          r3d, 2
    lea           r0, [r0+r2*2]
    jg .next2yrows
    REP_RET
%endmacro

%macro chroma_mc4_ssse3_func 2
cglobal %1_%2_chroma_mc4, 6, 7, 0
%if ARCH_X86_64
    movsxd        r2, r2d
%endif
    mov           r6, r4
    shl          r4d, 8
    sub          r4d, r6d
    mov           r6, 8
    add          r4d, 8           ; x*288+8
    sub          r6d, r5d
    imul         r6d, r4d         ; (8-y)*(x*255+8) = (8-y)*x<<8 | (8-y)*(8-x)
    imul         r4d, r5d         ;    y *(x*255+8) =    y *x<<8 |    y *(8-x)

    movd          m7, r6d
    movd          m6, r4d
    movq          m5, [pw_32]
    movd          m0, [r1  ]
    pshufw        m7, m7, 0
    punpcklbw     m0, [r1+1]
    pshufw        m6, m6, 0

.next2rows:
    movd          m1, [r1+r2*1  ]
    movd          m3, [r1+r2*2  ]
    punpcklbw     m1, [r1+r2*1+1]
    punpcklbw     m3, [r1+r2*2+1]
    lea           r1, [r1+r2*2]
    movq          m2, m1
    movq          m4, m3
    pmaddubsw     m0, m7
    pmaddubsw     m1, m6
    pmaddubsw     m2, m7
    pmaddubsw     m3, m6
    paddw         m0, m5
    paddw         m2, m5
    paddw         m1, m0
    paddw         m3, m2
    psrlw         m1, 6
    movq          m0, m4
    psrlw         m3, 6
    packuswb      m1, m1
    packuswb      m3, m3
    CHROMAMC_AVG  m1, [r0  ]
    CHROMAMC_AVG  m3, [r0+r2]
    movd     [r0   ], m1
    movd     [r0+r2], m3
    sub          r3d, 2
    lea           r0, [r0+r2*2]
    jg .next2rows
    REP_RET
%endmacro

%define CHROMAMC_AVG NOTHING
INIT_XMM ssse3
chroma_mc8_ssse3_func put, h264, _rnd
INIT_MMX ssse3
chroma_mc4_ssse3_func put, h264

%define CHROMAMC_AVG DIRECT_AVG
INIT_XMM ssse3
chroma_mc8_ssse3_func avg, h264, _rnd
INIT_MMX ssse3
chroma_mc4_ssse3_func avg, h264


;---------------------------rewrite---------------------------

%macro VO_EMPTY 2-4

%endmacro

%macro VO_AVG 2
    pavgb         %1, %2
%endmacro

%macro VO_COPY_AVG_4 3
    movd   %2, %3
    pavgb  %1, %2
%endmacro

%macro VO_COPY_AVG_8 3
    movq   %2, %3
    pavgb  %1, %2
%endmacro

%macro VO_MERGE_AVG_8 4
	movq		%2,	%3
    movhps		%2, %4
    pavgb		%1, %2
%endmacro

%macro MC8_RND_MMX_FULL_PIXELS 0
.next4rows:
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + edx]
	movq	[eax],			mm0
	movq	[eax + edx],	mm1
	lea		eax,	[eax + 2*edx]	
	lea		ecx,	[ecx + 2*edx]
	
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + edx]
	movq	[eax],			mm0
	movq	[eax + edx],	mm1
	lea		eax,	[eax + 2*edx]	
	lea		ecx,	[ecx + 2*edx]
	
	sub		ebx,	4
	jnz		.next4rows
%endmacro

;void vo_put_h264_chroma_mc8_rnd_mmx(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);
;void vo_put_h264_chroma_mc8_rnd_mmxext(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);

%macro CHROMA_MC8_RND_MMX 2
vo_global %1_h264_chroma_mc8_rnd_%2_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	sub		esp,	20
	and		esp,	~0xff
	mov		[esp + 16],	ebp
	
	mov		eax,	[ebp + 8 ]		;dst
	mov		ecx,	[ebp + 12]		;src
	mov		edx,	[ebp + 16]		;stride
	mov		ebx,	[ebp + 20]		;h
	mov		esi,	[ebp + 24]		;x
	mov		edi,	[ebp + 28]		;y
	
	mov		ebp,	esi
	or		ebp,	edi
	jne		.MC8_RND_MMX_NOT_FULL_PIXELS
	
	MC8_RND_MMX_FULL_PIXELS
	
	mov		ebp,	[esp + 16]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret

.MC8_RND_MMX_NOT_FULL_PIXELS:
	test	edi,	edi
	mov		ebp,	1
	jz		.X_OR_Y_ONE_ZERO
	
	test	esi,	esi
	mov		ebp,	edx
	jnz		.X_AND_Y_NOT_ZERO
	
.X_OR_Y_ONE_ZERO:
	xor		esi,	edi				;x + y
	movd	mm4,	esi				
	punpcklwd		mm4,	mm4
	punpckldq		mm4,	mm4		;m4 = x word
	movq	mm5,	[pw_8]
	psubw	mm5,	mm4				;m5 = 8 - x word
	movq	mm6,	[pw_4]			;m6 = 4 word
	pxor	mm7,	mm7
	
.next_1_row:
	movq	mm0,	[ecx]			;m0 = [pixels : 0-7]
	movq	mm1,	[ecx + ebp]	;m1 = [pixels : 1-8]
	movq	mm2,	mm0
	movq	mm3,	mm1
	punpcklbw	mm0,	mm7
	punpckhbw	mm2,	mm7			;[m0,m2] = [pixels : 0-7] = A
	punpcklbw	mm1,	mm7
	punpckhbw	mm3,	mm7			;[m1,m3] = [pixels : 1-8] = B
	
	
	pmullw	mm0,	mm5
	pmullw	mm2,	mm5				;[m0,m2] = A * (8 - x)
	pmullw	mm1,	mm4
	pmullw	mm3,	mm4				;[m1,m3] = B * x
	paddw	mm0,	mm1
	paddw	mm2,	mm3				;[m0,m2] = A * (8 - x) + B * x
	paddw	mm0,	mm6
	paddw	mm2,	mm6
	psraw	mm0,	3
	psraw	mm2,	3
	packuswb	mm0,	mm2
	vo_chromamc_avg	mm0,	[eax]
	movq		[eax],	mm0
	
	lea		eax,	[eax + edx]
	lea		ecx,	[ecx + edx]
	dec		ebx
	jnz		.next_1_row
	
	mov		ebp,	[esp + 16]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
.X_AND_Y_NOT_ZERO:
	movd		mm0,	esi
	punpcklwd	mm0,	mm0
	punpckldq	mm0,	mm0		;m0 = x word
	movd		mm1,	edi
	punpcklwd	mm1,	mm1
	punpckldq	mm1,	mm1		;m1 = y word
	
	movq	mm2,	mm0
	pmullw	mm0,	mm1			;m0 = D = xy
	psllw	mm2,	3			;m2 = 8x
	psllw	mm1,	3			;m1 = 8y
	movq	mm3,	[pw_64]
	psubw	mm3,	mm1
	psubw	mm3,	mm2
	paddw	mm3,	mm0			;m3 = A = (8-x)*(8-y)
	movq	[esp],	mm3
	psubw	mm1,	mm0			;m1 = C = y*(8 - x)
	psubw	mm2,	mm0			;m2 = B = x*(8 - y)
	pxor	mm7,	mm7	
	
	movq	mm3,	[ecx]
	movq	mm4,	[ecx + 1]
	
.next_row:
	movq		mm5,	mm3
	punpcklbw	mm3,	mm7	
	punpckhbw	mm5,	mm7	
	movq		mm6,	[esp]
	pmullw		mm5,	mm6
	pmullw		mm6,	mm3		;[m6,m5] = (8-x)*(8-y) * [0-7]
	
	movq		mm3,	mm4
	punpcklbw	mm4,	mm7	
	punpckhbw	mm3,	mm7	
	pmullw		mm4,	mm2
	pmullw		mm3,	mm2		;x*(8 - y) * [1-8]
	paddw		mm6,	mm4
	paddw		mm5,	mm3
	
	lea			ecx,	[ecx + edx]
	movq		mm3,	[ecx]
	movq		mm4,	mm3
	punpcklbw	mm3,	mm7
	punpckhbw	mm4,	mm7
	pmullw		mm3,	mm1
	pmullw		mm4,	mm1		;y*(8 - x) * [0-7]
	paddw		mm6,	mm3
	paddw		mm5,	mm4
	
	movq		mm4,	[ecx + 1]
	movq		mm3,	mm4
	punpckhbw	mm3,	mm7
	pmullw		mm3,	mm0
	paddw		mm5,	mm3
	movq		mm3,	mm4
	punpcklbw	mm3,	mm7	
	pmullw		mm3,	mm0
	paddw		mm6,	mm3
	
	movq		mm3,	[pw_32]
	paddw		mm5,	mm3
	paddw		mm6,	mm3
	psrlw		mm5,	6
	psrlw		mm6,	6
	packuswb	mm6,	mm5
	vo_chromamc_avg	mm6,	[eax]
	movq		[eax],	mm6
	movq		mm3,	[ecx]
	
	lea		eax,	[eax + edx]
	dec		ebx
	jnz		.next_row
	
	mov		ebp,	[esp + 16]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

;void vo_put_h264_chroma_mc4_mmx	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)
;void vo_avg_h264_chroma_mc4_mmxext	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)

%macro CHROMA_MC4 2-3 rewrite
vo_global %1_h264_chroma_mc4_%2_%3
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8 ]		;dst
	mov		ecx,	[ebp + 12]		;src
	mov		edx,	[ebp + 16]		;srcStride
	mov		ebx,	[ebp + 20]		;h
	mov		esi,	[ebp + 24]		;x
	mov		edi,	[ebp + 28]		;y
	
	movd		mm0,	esi			;m0 = x
	movd		mm1,	edi			;m1 = y
	movq		mm2,	[pw_8]
	movq		mm3,	mm2
	punpcklwd	mm0,	mm0
	punpcklwd	mm1,	mm1
	punpckldq	mm0,	mm0
	punpckldq	mm1,	mm1
	psubw		mm2,	mm0			;m2 = 8 - x
	psubw		mm3,	mm1			;m3 = 8 - y
	pxor		mm4,	mm4
	
	movq		mm5,	[ecx]
	punpcklbw	mm5,	mm4
	pmullw		mm5,	mm2			;m5 = A * (8 - x)
	movq		mm6,	[ecx + 1]
	punpcklbw	mm6,	mm4
	pmullw		mm6,	mm0			;m6 = B * x
	paddw		mm5,	mm6			;m6 = A * (8 - x) + B * x
	lea			ecx,	[ecx + edx]
	
.next_2_rows:
	movq		mm6,	[ecx]
	punpcklbw	mm6,	mm4
	pmullw		mm6,	mm2			;m6 = C * (8 - x)
	movq		mm7,	[ecx + 1]
	punpcklbw	mm7,	mm4
	pmullw		mm7,	mm0			;m7 = D * x
	paddw		mm7,	mm6
	movq		mm6,	mm7
	
	pmullw		mm7,	mm1			;m6 = C * y * (8 - x) + D * x * y
	pmullw		mm5,	mm3			;m5 = A * (8 - x)*(8 - y) + B * x * (8-y)
	paddw		mm7,	mm5
	paddw		mm7,	[pw_32]
	psrlw		mm7,	6
	packuswb	mm7,	mm7
	vo_chromamc4_avg	mm7,	mm5,	[eax]
	movq		[eax],	mm7
	
	lea			eax,	[eax + edx]
	lea			ecx,	[ecx + edx]
	
	movq		mm5,	[ecx]
	punpcklbw	mm5,	mm4
	pmullw		mm5,	mm2
	movq		mm7,	[ecx + 1]
	punpcklbw	mm7,	mm4
	pmullw		mm7,	mm0
	paddw		mm5,	mm7
	movq		mm7,	mm5
	pmullw		mm6,	mm3
	pmullw		mm7,	mm1
	paddw		mm7,	mm6
	paddw		mm7,	[pw_32]
	psrlw		mm7,	6
	packuswb	mm7,	mm7
	vo_chromamc4_avg	mm7,	mm6,	[eax]
	movq		[eax],	mm7
	
	lea			eax,	[eax + edx]
	lea			ecx,	[ecx + edx]
	sub			ebx,	2
	jnz			.next_2_rows
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

;void vo_put_h264_chroma_mc8_rnd_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)
;void vo_avg_h264_chroma_mc8_rnd_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)

%macro MC8_RND_SSE_FULL_PIXELS 0
.next_4_rows:
	movq	xmm0,	[eax]
	movq	xmm1,	[ecx + edx]
	vo_sse_cp_avg_8	xmm0,xmm3,[eax]
	vo_sse_cp_avg_8	xmm1,xmm3,[eax + edx]
	movq	[eax],			xmm0
	movq	[eax + edx],	xmm1
	
	lea		eax,	[eax + 2*edx]
	lea		ecx,	[ecx + 2*edx]
	
	movq	xmm0,	[ecx]
	movq	xmm1,	[ecx + edx]
	vo_sse_cp_avg_8	xmm0,xmm3,[eax]
	vo_sse_cp_avg_8	xmm1,xmm3,[eax + edx]
	movq	[eax],			xmm0
	movq	[eax + edx],	xmm1
	
	lea		eax,	[eax + 2*edx]
	lea		ecx,	[ecx + 2*edx]
	
	sub		ebx,	4
	jnz	.next_4_rows
%endmacro

%macro CHROMA_MC8_RND_SSSE3 2-3 rewrite
vo_global	%1_h264_chroma_mc8_rnd_%2_%3
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	sub		esp,	36
	and		esp,	~0xf
	mov		[esp + 32],	ebp
	
	mov		eax,	[ebp + 8 ]		;dst
	mov		ecx,	[ebp + 12]		;src
	mov		edx,	[ebp + 16]		;stride
	mov		ebx,	[ebp + 20]		;h
	mov		esi,	[ebp + 24]		;x
	mov		edi,	[ebp + 28]		;y
	
	mov		ebp,	esi
	or		ebp,	edi
	jnz		.X_OR_Y_NOT_ZERO
	
	MC8_RND_SSE_FULL_PIXELS
	
	mov		ebp,	[esp + 32]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
.X_OR_Y_NOT_ZERO:
	test	esi	,	esi
	mov		ebp,	edx
	jz		.X_ZERO
	test	edi,	edi
	mov		ebp,	1
	jz		.Y_ZERO
	
.X_AND_Y_NOT_ZERO:
	mov		ebp,	8
	sub		ebp,	esi		;ebp = 8-x
	shl		ebp,	8		;ebp = (8-x)<<8
	or		ebp,	esi		;ebp = (8-x)<<8 | x

	mov		esi,	8		;
	sub		esi,	edi		;esi = 8-y
	imul	esi,	ebp		;esi = (8-x)*(8-y)<<8 | x*(8-y)
	imul	edi,	ebp		;edi = (8-x)*y<<8 | x*y

	;movlhps <=> punpcklqdq
	movd	xmm0,	esi
	movd	xmm1,	edi
	pshuflw			xmm0,	xmm0,	0
	punpcklqdq		xmm0,	xmm0		;m0 = word (8-x)*(8-y)<<8 | x*(8-y)
	pshuflw	xmm1,	xmm1,	xmm1,	0
	punpcklqdq		xmm1,	xmm1		;m1 = word (8-x)*y<<8 | x*y

	movq			xmm3,	[ecx]
	movq			xmm2,	[ecx + 1]
	punpcklbw		xmm2,	xmm3

	movdqa			xmm7,	[pw_32]
	lea				ebp,	[2*edx]

.next_2_rows:
	movq			xmm4,	[ecx + edx]
	movq			xmm3,	[ecx + edx + 1]
	punpcklbw		xmm3,	xmm4
	movdqa			xmm4,	xmm3
	pmaddubsw		xmm2,	xmm0		;m2 = A * (8-x)*(8-y) + B * x*(8-y)
	pmaddubsw		xmm3,	xmm1		;m3 = C * (8-x)*y + D * x*y
	paddw			xmm2,	xmm3
	paddw			xmm2,	xmm7		;m2 = A * (8-x)*(8-y) + B * x*(8-y) + C * (8-x)*y + D * x*y + 32
	psrlw			xmm2,	6
	packuswb		xmm2,	xmm2
	vo_sse_cp_avg_8	xmm2,xmm5,[eax]
	movq			[eax],	xmm2

	lea				ecx,	[ecx + ebp]

	movq			xmm3,	[ecx]
	movq			xmm2,	[ecx + 1]
	punpcklbw		xmm2,	xmm3
	movdqa			xmm3,	xmm2
	pmaddubsw		xmm4,	xmm0
	pmaddubsw		xmm3,	xmm1
	paddw			xmm3,	xmm4
	paddw			xmm3,	xmm7
	psrlw			xmm3,	6


	packuswb		xmm3,	xmm3
	vo_sse_cp_avg_8	xmm3,xmm5,[eax + edx]
	movq			[eax + edx],	xmm3

	lea				eax,	[eax + ebp]
	sub				ebx,	2
	jnz				.next_2_rows
	
	mov		ebp,	[esp + 32]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
.X_ZERO:
	mov		ebp,	8
	sub		ebp,	edi				;ebp = 8 - y
	shl		edi,	8				;ebp = y << 8
	or		edi,	ebp				;edi =  y << 8 | (8 - y)
	
	movd		xmm0,	edi
	pshuflw		xmm0,	xmm0,	0
	movlhps	xmm0,	xmm0		;m0 = word y << 8 | (8 - y)
	movdqa		xmm7,	[pw_4]
	movq		xmm1,	[ecx]
.next2_by_y:
	movq		xmm2,	[ecx + edx]
	punpcklbw	xmm1,	xmm2
	pmaddubsw	xmm1,	xmm0
	paddw		xmm1,	xmm7		;m1 = A * (8-y) + C * y + 32
	psrlw		xmm1,	3			;m1 = (A * (8-y) + C * y + 32) >> 3
	
	lea			ecx,	[ecx + 2*edx]
	
	movq		xmm3,	[ecx]
	punpcklbw	xmm2,	xmm3
	pmaddubsw	xmm2,	xmm0
	paddw		xmm2,	xmm7		;m1 = A * (8-y) + C * y + 32
	psrlw		xmm2,	3			;m1 = (A * (8-y) + C * y + 32) >> 3
	packuswb	xmm1,	xmm2
	vo_sse_merge_avg_8	xmm1,xmm4,[eax],[eax + edx]
	movq		[eax],			xmm1
	movhps		[eax + edx],	xmm1
	movdqa		xmm1,	xmm3
	
	lea			eax,	[eax + 2*edx]
	
	sub			ebx,	2
	jnz			.next2_by_y
	
	mov		ebp,	[esp + 32]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret


.Y_ZERO:
	mov		ebp,	8
	sub		ebp,	esi			;ebp = 8 - x
	shl		esi,	8
	or		esi,	ebp			;esi = x << 8 | (8 - x)
	
	movd		xmm0,	esi
	pshuflw		xmm0,	xmm0,	0
	movlhps	xmm0,	xmm0		;m0 = word y x << 8 | (8 - x)
	movdqa		xmm7,	[pw_4]

.next2_by_x:
	movq		xmm1,	[ecx]
	movq		xmm2,	[ecx + 1]
	punpcklbw	xmm1,	xmm2
	pmaddubsw	xmm1,	xmm0
	paddw		xmm1,	xmm7
	psrlw		xmm1,	3
	
	movq		xmm3,	[ecx + edx]
	movq		xmm4,	[ecx  + edx + 1]
	punpcklbw	xmm3,	xmm4
	pmaddubsw	xmm3,	xmm0
	paddw		xmm3,	xmm7
	psrlw		xmm3,	3
	packuswb	xmm1,	xmm3
	vo_sse_merge_avg_8	xmm1,xmm3,[eax], [eax + edx]
	movq		[eax],	xmm1
	movhps		[eax + edx],	xmm1
	
	lea			eax,	[eax + 2*edx]
	lea			ecx,	[ecx + 2*edx]
	
	sub			ebx,	2
	jnz			.next2_by_x
	
	mov		ebp,	[esp + 32]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret

%endmacro

;void vo_put_h264_chroma_mc4_ssse3	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)
;void vo_avg_h264_chroma_mc4_ssse3	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y)

%macro CHROMA_MC4_SSSE3 2-3 rewrite
vo_global %1_h264_chroma_mc4_%2_%3
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8 ]		;dst
	mov		ecx,	[ebp + 12]		;src
	mov		edx,	[ebp + 16]		;stride
	mov		ebx,	[ebp + 20]		;h
	mov		esi,	[ebp + 24]		;x
	mov		edi,	[ebp + 28]		;y
	sub		esp,	8
	and		esp,	~0x7
	mov		[esp],	ebp
	
	test	esi,	esi
	jz		.X_ZERO_E3_MC4
	test	edi,	edi
	jz		.Y_ZERO_E3_MC4
	
	mov		ebp,	8
	sub		ebp,	esi
	shl		esi,	8
	or		esi,	ebp		;esi = (8-x) | x<<8
	mov		ebp,	8
	sub		ebp,	edi		;ebp = 8 - y
	imul	edi,	esi		;edi =		(8-x)y | xy<<8
	imul	esi,	ebp		;esi =  (8-x)(8-y) | x(8-y)<<8
	
	movd	mm0,	esi
	movd	mm1,	edi
	pshufw	mm0,	mm0,	0	;word	(8-x)(8-y) | x(8-y)<<8
	pshufw	mm1,	mm1,	0	;word		(8-x)y | xy<<8
	movq	mm7,	[pw_32]
	
	movd	mm2,	[ecx]
	movd	mm3,	[ecx+1]
	punpcklbw	mm2,	mm3	;m2 = AB AB AB AB
	
.NEXT_2_ROWS_XY_SE3_MC4:
	movd	mm4,	[ecx + edx]
	movd	mm5,	[ecx + edx + 1]
	punpcklbw	mm4,	mm5	;m4 = CD CD CD CD
	movq		mm5,	mm4
	pmaddubsw	mm2,	mm0
	pmaddubsw	mm4,	mm1
	paddw		mm2,	mm4
	paddw		mm2,	mm7
	psrlw		mm2,	6
	packuswb	mm2,	mm2
	vo_chromamc_avg	mm2,	[eax]
	movd		[eax],	mm2
	
	lea		ecx,	[ecx + 2*edx]
	
	movd	mm2,	[ecx]
	movd	mm3,	[ecx+1]
	punpcklbw	mm2,	mm3
	movq		mm3,	mm2
	pmaddubsw	mm5,	mm0
	pmaddubsw	mm3,	mm1
	paddw		mm5,	mm3
	paddw		mm5,	mm7
	psrlw		mm5,	6
	packuswb	mm5,	mm5
	vo_chromamc_avg	mm5,	[eax+edx]
	movd		[eax+edx],	mm5
	
	lea		eax,	[eax + 2*edx]
	sub		ebx,	2
	jnz		.NEXT_2_ROWS_XY_SE3_MC4
	
	
	mov		ebp,	[esp]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
	
.X_ZERO_E3_MC4:
	mov		ebp,	8
	sub		ebp,	edi		;8 - y
	shl		edi,	8
	or		edi,	ebp		;(8-y) | y<<8
	
	movd	mm0,	edi
	pshufw	mm0,	mm0,	0	;m0 = 4 words	(8-y) | y<<8
	movq	mm7,	[pw_4]
	
	movd	mm1,	[ecx]
	
.NEXT_2_ROWS_Y_ORDER_SE3_MC4:
	movd		mm2,	[ecx+edx]
	punpcklbw	mm1,	mm2
	pmaddubsw	mm1,	mm0
	paddw		mm1,	mm7
	psrlw		mm1,	3
	packuswb	mm1,	mm1
	vo_chromamc_avg mm1,[eax]
	movd		[eax],	mm1
	
	lea			ecx,	[ecx + 2*edx]
	
	movd		mm1,	[ecx]
	punpcklbw	mm2,	mm1
	pmaddubsw	mm2,	mm0
	paddw		mm2,	mm7
	psrlw		mm2,	3
	packuswb	mm2,	mm2
	vo_chromamc_avg mm2,[eax + edx]
	movd		[eax + edx],	mm2
	
	lea			eax,	[eax + 2*edx]
	sub			ebx,	2
	jnz			.NEXT_2_ROWS_Y_ORDER_SE3_MC4

	mov		ebp,	[esp]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret

.Y_ZERO_E3_MC4:
	mov		ebp,	8
	sub		ebp,	esi
	shl		esi,	8
	or		esi,	ebp			;(8-x) | x<<8
	
	movd	mm0,	esi
	pshufw	mm0,	mm0,	0	;m0 = 4 words (8-x) | x<<8
	movq	mm7,	[pw_4]	
	
.NEXT_2_ROWS_X_ORDER_SE3_MC4:
	movd		mm1,	[ecx]
	movd		mm2,	[ecx+1]
	punpcklbw	mm1,	mm2
	pmaddubsw	mm1,	mm0
	paddw		mm1,	mm7
	psrlw		mm1,	3
	packuswb	mm1,	mm1
	vo_chromamc_avg	mm1,	[eax]
	movd		[eax],	mm1
	
	movd		mm1,	[ecx+edx]
	movd		mm2,	[ecx+edx+1]
	punpcklbw	mm1,	mm2
	pmaddubsw	mm1,	mm0
	paddw		mm1,	mm7
	psrlw		mm1,	3
	packuswb	mm1,	mm1
	vo_chromamc_avg	mm1,	[eax+edx]
	movd		[eax+edx],	mm1
	
	lea			eax,	[eax+2*edx]
	lea			ecx,	[ecx+2*edx]
	sub			ebx,	2
	jnz			.NEXT_2_ROWS_X_ORDER_SE3_MC4
	
	mov		ebp,	[esp]
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

%define vo_chromamc_avg VO_EMPTY
%define vo_chromamc4_avg VO_EMPTY
CHROMA_MC8_RND_MMX	put,	mmx
CHROMA_MC4			put,	mmx

%define vo_chromamc_avg VO_AVG
%define vo_chromamc4_avg VO_COPY_AVG_4
CHROMA_MC8_RND_MMX	avg,	mmxext
CHROMA_MC4			avg,	mmxext

%define vo_sse_cp_avg_8		VO_EMPTY
%define vo_sse_merge_avg_8	VO_EMPTY
CHROMA_MC8_RND_SSSE3	put,	ssse3
%define vo_sse_cp_avg_8		VO_COPY_AVG_8
%define vo_sse_merge_avg_8	VO_MERGE_AVG_8
CHROMA_MC8_RND_SSSE3	avg,	ssse3


%define vo_chromamc_avg VO_EMPTY
CHROMA_MC4_SSSE3	put,	ssse3
%define vo_chromamc_avg VO_AVG
CHROMA_MC4_SSSE3	avg,	ssse3