%include "voH264Util.asm"

SECTION_RODATA 32

cextern pw_16
cextern pw_5
cextern pb_0

SECTION .text


%macro op_avgh 3
    movh   %3, %2
    pavgb  %1, %3
    movh   %2, %1
%endmacro

%macro op_avg 2-3
    pavgb  %1, %2
    mova   %2, %1
%endmacro

%macro op_puth 2-3
    movh   %2, %1
%endmacro

%macro op_put 2-3
    mova   %2, %1
%endmacro

%macro QPEL4_H_LOWPASS_OP 1
cglobal %1_h264_qpel4_h_lowpass, 4,5 ; dst, src, dstStride, srcStride
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    pxor          m7, m7
    mova          m4, [pw_5]
    mova          m5, [pw_16]
    mov          r4d, 4
.loop:
    movh          m1, [r1-1]
    movh          m2, [r1+0]
    movh          m3, [r1+1]
    movh          m0, [r1+2]
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m0, m7
    paddw         m1, m0
    paddw         m2, m3
    movh          m0, [r1-2]
    movh          m3, [r1+3]
    punpcklbw     m0, m7
    punpcklbw     m3, m7
    paddw         m0, m3
    psllw         m2, 2
    psubw         m2, m1
    pmullw        m2, m4
    paddw         m0, m5
    paddw         m0, m2
    psraw         m0, 5
    packuswb      m0, m0
    op_%1h        m0, [r0], m6
    add           r0, r2
    add           r1, r3
    dec          r4d
    jg         .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL4_H_LOWPASS_OP put
QPEL4_H_LOWPASS_OP avg

%macro QPEL8_H_LOWPASS_OP 1
cglobal %1_h264_qpel8_h_lowpass, 4,5 ; dst, src, dstStride, srcStride
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    mov          r4d, 8
    pxor          m7, m7
    mova          m6, [pw_5]
.loop:
    mova          m0, [r1]
    mova          m2, [r1+1]
    mova          m1, m0
    mova          m3, m2
    punpcklbw     m0, m7
    punpckhbw     m1, m7
    punpcklbw     m2, m7
    punpckhbw     m3, m7
    paddw         m0, m2
    paddw         m1, m3
    psllw         m0, 2
    psllw         m1, 2
    mova          m2, [r1-1]
    mova          m4, [r1+2]
    mova          m3, m2
    mova          m5, m4
    punpcklbw     m2, m7
    punpckhbw     m3, m7
    punpcklbw     m4, m7
    punpckhbw     m5, m7
    paddw         m2, m4
    paddw         m5, m3
    psubw         m0, m2
    psubw         m1, m5
    pmullw        m0, m6
    pmullw        m1, m6
    movd          m2, [r1-2]
    movd          m5, [r1+7]
    punpcklbw     m2, m7
    punpcklbw     m5, m7
    paddw         m2, m3
    paddw         m4, m5
    mova          m5, [pw_16]
    paddw         m2, m5
    paddw         m4, m5
    paddw         m0, m2
    paddw         m1, m4
    psraw         m0, 5
    psraw         m1, 5
    packuswb      m0, m1
    op_%1         m0, [r0], m4
    add           r0, r2
    add           r1, r3
    dec          r4d
    jg         .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL8_H_LOWPASS_OP put
QPEL8_H_LOWPASS_OP avg

%macro QPEL8_H_LOWPASS_OP_XMM 1
cglobal %1_h264_qpel8_h_lowpass, 4,5,8 ; dst, src, dstStride, srcStride
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    mov          r4d, 8
    pxor          m7, m7
    mova          m6, [pw_5]
.loop:
    movu          m1, [r1-2]
    mova          m0, m1
    punpckhbw     m1, m7
    punpcklbw     m0, m7
    mova          m2, m1
    mova          m3, m1
    mova          m4, m1
    mova          m5, m1
    palignr       m4, m0, 2
    palignr       m3, m0, 4
    palignr       m2, m0, 6
    palignr       m1, m0, 8
    palignr       m5, m0, 10
    paddw         m0, m5
    paddw         m2, m3
    paddw         m1, m4
    psllw         m2, 2
    psubw         m2, m1
    paddw         m0, [pw_16]
    pmullw        m2, m6
    paddw         m2, m0
    psraw         m2, 5
    packuswb      m2, m2
    op_%1h         m2, [r0], m4
    add           r1, r3
    add           r0, r2
    dec          r4d
    jne        .loop
    REP_RET
%endmacro

INIT_XMM ssse3
QPEL8_H_LOWPASS_OP_XMM put
QPEL8_H_LOWPASS_OP_XMM avg


%macro QPEL4_H_LOWPASS_L2_OP 1
cglobal %1_h264_qpel4_h_lowpass_l2, 5,6 ; dst, src, src2, dstStride, srcStride
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
    pxor          m7, m7
    mova          m4, [pw_5]
    mova          m5, [pw_16]
    mov          r5d, 4
.loop:
    movh          m1, [r1-1]
    movh          m2, [r1+0]
    movh          m3, [r1+1]
    movh          m0, [r1+2]
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m0, m7
    paddw         m1, m0
    paddw         m2, m3
    movh          m0, [r1-2]
    movh          m3, [r1+3]
    punpcklbw     m0, m7
    punpcklbw     m3, m7
    paddw         m0, m3
    psllw         m2, 2
    psubw         m2, m1
    pmullw        m2, m4
    paddw         m0, m5
    paddw         m0, m2
    movh          m3, [r2]
    psraw         m0, 5
    packuswb      m0, m0
    pavgb         m0, m3
    op_%1h        m0, [r0], m6
    add           r0, r3
    add           r1, r3
    add           r2, r4
    dec          r5d
    jg         .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL4_H_LOWPASS_L2_OP put
QPEL4_H_LOWPASS_L2_OP avg


%macro QPEL8_H_LOWPASS_L2_OP 1
cglobal %1_h264_qpel8_h_lowpass_l2, 5,6 ; dst, src, src2, dstStride, srcStride
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
    mov          r5d, 8
    pxor          m7, m7
    mova          m6, [pw_5]
.loop:
    mova          m0, [r1]		;a2 a3 a4 a5 a6 a7 a8 a9
    mova          m2, [r1+1]	;a3 a4 a5 a6 a7 a8 a9 a10
    mova          m1, m0
    mova          m3, m2
    punpcklbw     m0, m7		;m0 = a2 a3 a4 a5
    punpckhbw     m1, m7		;m1 = a6 a7 a8 a9
    punpcklbw     m2, m7		;m2 = a3 a4 a5 a6
    punpckhbw     m3, m7		;m3 = a7 a8 a9 a10
    paddw         m0, m2		;a2 + a3, a3 + a4, a4 + a5, a5 + a6
    paddw         m1, m3		;a6 + a7, a7 + a8, a8 + a9, a9 + a10
    psllw         m0, 2			;m0 = (a2 + a3) << 2
    psllw         m1, 2			;m1 = (a6 + a7) << 2
    mova          m2, [r1-1]	;a1 a2 a3 a4 a5 a6 a7  a8
    mova          m4, [r1+2]	;a4 a5 a6 a7 a8 a9 a10 a11
    mova          m3, m2
    mova          m5, m4
    punpcklbw     m2, m7		;m2 = a1 a2 a3  a4
    punpckhbw     m3, m7		;m3 = a5 a6 a7  a8
    punpcklbw     m4, m7		;m4 = a4 a5 a6  a7
    punpckhbw     m5, m7		;m5 = a8 a9 a10 a11
    paddw         m2, m4		;m2 = (a1 + a4)
    paddw         m5, m3		;m5 = (a5 + a8)
    psubw         m0, m2		;m0 = (a2 + a3) << 2 - (a1 + a4)
    psubw         m1, m5		;m1 = (a6 + a7) << 2 - (a5 + a8)
    pmullw        m0, m6		;m0 = 20*(a2 + a3) - 5*(a1 + a4)
    pmullw        m1, m6		;m1 = 20*(a6 + a7) - 5*(a5 + a8)
    movd          m2, [r1-2]	;m2 = a0 a1  a2  a3
    movd          m5, [r1+7]	;m5 = a9 a10 a11 a12
    punpcklbw     m2, m7		;
    punpcklbw     m5, m7		;
    paddw         m2, m3		;m2 = a0 + a5 : (a0 a1  a2  a3) + (a5 a6  a7  a8)
    paddw         m4, m5		;m4 = a4 + a9 : (a4 a5  a6  a7) + (a9 a10 a11 a12)
    mova          m5, [pw_16]
    paddw         m2, m5
    paddw         m4, m5
    paddw         m0, m2
    paddw         m1, m4
    psraw         m0, 5
    psraw         m1, 5
    mova          m4, [r2]
    packuswb      m0, m1
    pavgb         m0, m4
    op_%1         m0, [r0], m4
    add           r0, r3
    add           r1, r3
    add           r2, r4
    dec          r5d
    jg         .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL8_H_LOWPASS_L2_OP put
QPEL8_H_LOWPASS_L2_OP avg


%macro QPEL8_H_LOWPASS_L2_OP_XMM 1
cglobal %1_h264_qpel8_h_lowpass_l2, 5,6,8 ; dst, src, src2, dstStride, src2Stride
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
    mov          r5d, 8
    pxor          m7, m7
    mova          m6, [pw_5]
.loop:
    lddqu         m1, [r1-2]
    mova          m0, m1
    punpckhbw     m1, m7
    punpcklbw     m0, m7
    mova          m2, m1
    mova          m3, m1
    mova          m4, m1
    mova          m5, m1
    palignr       m4, m0, 2
    palignr       m3, m0, 4
    palignr       m2, m0, 6
    palignr       m1, m0, 8
    palignr       m5, m0, 10
    paddw         m0, m5
    paddw         m2, m3
    paddw         m1, m4
    psllw         m2, 2
    movh          m3, [r2]
    psubw         m2, m1
    paddw         m0, [pw_16]
    pmullw        m2, m6
    paddw         m2, m0
    psraw         m2, 5
    packuswb      m2, m2
    pavgb         m2, m3
    op_%1h        m2, [r0], m4
    add           r1, r3
    add           r0, r3
    add           r2, r4
    dec          r5d
    jg         .loop
    REP_RET
%endmacro

INIT_XMM ssse3
QPEL8_H_LOWPASS_L2_OP_XMM put
QPEL8_H_LOWPASS_L2_OP_XMM avg


; All functions that call this are required to have function arguments of
; dst, src, dstStride, srcStride
%macro FILT_V 1
    mova      m6, m2
    movh      m5, [r1]
    paddw     m6, m3
    psllw     m6, 2
    psubw     m6, m1
    psubw     m6, m4
    punpcklbw m5, m7
    pmullw    m6, [pw_5]
    paddw     m0, [pw_16]
    add       r1, r3
    paddw     m0, m5
    paddw     m6, m0
    psraw     m6, 5
    packuswb  m6, m6
    op_%1h    m6, [r0], m0 ; 1
    add       r0, r2
    SWAP       0, 1, 2, 3, 4, 5
%endmacro

%macro QPEL4_V_LOWPASS_OP 1
cglobal %1_h264_qpel4_v_lowpass, 4,4 ; dst, src, dstStride, srcStride
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    sub           r1, r3
    sub           r1, r3
    pxor          m7, m7
    movh          m0, [r1]
    movh          m1, [r1+r3]
    lea           r1, [r1+2*r3]
    movh          m2, [r1]
    movh          m3, [r1+r3]
    lea           r1, [r1+2*r3]
    movh          m4, [r1]
    add           r1, r3
    punpcklbw     m0, m7
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m4, m7
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    RET
%endmacro

INIT_MMX mmxext
QPEL4_V_LOWPASS_OP put
QPEL4_V_LOWPASS_OP avg



%macro QPEL8OR16_V_LOWPASS_OP 1
%if cpuflag(sse2)
cglobal %1_h264_qpel8or16_v_lowpass, 5,5,8 ; dst, src, dstStride, srcStride, h
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    sub           r1, r3
    sub           r1, r3
%else
cglobal %1_h264_qpel8or16_v_lowpass_op, 5,5,8 ; dst, src, dstStride, srcStride, h
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
%endif
    pxor          m7, m7
    movh          m0, [r1]
    movh          m1, [r1+r3]
    lea           r1, [r1+2*r3]
    movh          m2, [r1]
    movh          m3, [r1+r3]
    lea           r1, [r1+2*r3]
    movh          m4, [r1]
    add           r1, r3
    punpcklbw     m0, m7
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m4, m7
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    cmp          r4d, 16
    jne         .end
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
    FILT_V        %1
.end:
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL8OR16_V_LOWPASS_OP put
QPEL8OR16_V_LOWPASS_OP avg

INIT_XMM sse2
QPEL8OR16_V_LOWPASS_OP put
QPEL8OR16_V_LOWPASS_OP avg


; All functions that use this are required to have args:
; src, tmp, srcSize
%macro FILT_HV 1 ; offset
    mova           m6, m2
    movh           m5, [r0]
    paddw          m6, m3
    psllw          m6, 2
    paddw          m0, [pw_16]
    psubw          m6, m1
    psubw          m6, m4
    punpcklbw      m5, m7
    pmullw         m6, [pw_5]
    paddw          m0, m5
    add            r0, r2
    paddw          m6, m0
    mova      [r1+%1], m6
    SWAP            0, 1, 2, 3, 4, 5
%endmacro

%macro QPEL4_HV1_LOWPASS_OP 1
cglobal %1_h264_qpel4_hv_lowpass_v, 3,3 ; src, tmp, srcStride
    movsxdifnidn  r2, r2d
    pxor          m7, m7
    movh          m0, [r0]
    movh          m1, [r0+r2]
    lea           r0, [r0+2*r2]
    movh          m2, [r0]
    movh          m3, [r0+r2]
    lea           r0, [r0+2*r2]
    movh          m4, [r0]
    add           r0, r2
    punpcklbw     m0, m7
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m4, m7
    FILT_HV       0*24
    FILT_HV       1*24
    FILT_HV       2*24
    FILT_HV       3*24
    RET

cglobal %1_h264_qpel4_hv_lowpass_h, 3,4 ; tmp, dst, dstStride
    movsxdifnidn  r2, r2d
    mov          r3d, 4
.loop:
    mova          m0, [r0]
    paddw         m0, [r0+10]
    mova          m1, [r0+2]
    paddw         m1, [r0+8]
    mova          m2, [r0+4]
    paddw         m2, [r0+6]
    psubw         m0, m1
    psraw         m0, 2
    psubw         m0, m1
    paddsw        m0, m2
    psraw         m0, 2
    paddw         m0, m2
    psraw         m0, 6
    packuswb      m0, m0
    op_%1h        m0, [r1], m7
    add           r0, 24
    add           r1, r2
    dec          r3d
    jnz        .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL4_HV1_LOWPASS_OP put
QPEL4_HV1_LOWPASS_OP avg

%macro QPEL8OR16_HV1_LOWPASS_OP 1
cglobal %1_h264_qpel8or16_hv1_lowpass_op, 4,4,8 ; src, tmp, srcStride, size
    movsxdifnidn  r2, r2d
    pxor          m7, m7
    movh          m0, [r0]
    movh          m1, [r0+r2]
    lea           r0, [r0+2*r2]
    movh          m2, [r0]
    movh          m3, [r0+r2]
    lea           r0, [r0+2*r2]
    movh          m4, [r0]
    add           r0, r2
    punpcklbw     m0, m7
    punpcklbw     m1, m7
    punpcklbw     m2, m7
    punpcklbw     m3, m7
    punpcklbw     m4, m7
    FILT_HV     0*48
    FILT_HV     1*48
    FILT_HV     2*48
    FILT_HV     3*48
    FILT_HV     4*48
    FILT_HV     5*48
    FILT_HV     6*48
    FILT_HV     7*48
    cmp          r3d, 16
    jne         .end
    FILT_HV     8*48
    FILT_HV     9*48
    FILT_HV    10*48
    FILT_HV    11*48
    FILT_HV    12*48
    FILT_HV    13*48
    FILT_HV    14*48
    FILT_HV    15*48
.end:
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL8OR16_HV1_LOWPASS_OP put
QPEL8OR16_HV1_LOWPASS_OP avg

INIT_XMM sse2
QPEL8OR16_HV1_LOWPASS_OP put



%macro QPEL8OR16_HV2_LOWPASS_OP 1
; unused is to match ssse3 and mmxext args
cglobal %1_h264_qpel8or16_hv2_lowpass_op, 5,5 ; dst, tmp, dstStride, unused, h
    movsxdifnidn  r2, r2d
.loop:
    mova          m0, [r1]			;m0 = a0 a1 a2 a3
    mova          m3, [r1+8]		;m3 = a4 a5 a6 a7
    mova          m1, [r1+2]		;m1 = a1 a2 a3 a4
    mova          m4, [r1+10]		;m4 = a5 a6 a7 a8
    paddw         m0, m4			;m0 = a0 + a5
    paddw         m1, m3			;m1 = a1 + a4
    paddw         m3, [r1+18]		;m3 = a4 + a9		a9  a10 a11 a12
    paddw         m4, [r1+16]		;m4 = a5 + a8		a8  a9  a10 a11
    mova          m2, [r1+4]		;m2 = a2 a3 a4 a5
    mova		  m5, [r1+12]		;m5 = a6 a7 a8 a9
    paddw         m2, [r1+6]		;m2 = a2 + a3
    paddw         m5, [r1+14]		;m5 = a6 + a7
    psubw         m0, m1			;m0 = (a0 + a5) - (a1 + a4)
    psubw         m3, m4			;m3 = (a4 + a9) - (a5 + a8)
    psraw         m0, 2				;m0 = (a0 + a5) >> 2  - (a1 + a4) >> 2
    psraw         m3, 2				;m3 = (a4 + a9) >> 2  - (a5 + a8) >> 2
    psubw         m0, m1			;m0 = (a0 + a5) >> 2  - 5*(a1 + a4) >> 2
    psubw         m3, m4			;m3 = (a4 + a9) >> 2  - 5*(a5 + a8) >> 2
    paddsw        m0, m2			;m0 = (a0 + a5) >> 2  - 5*(a1 + a4) >> 2 + (a2 + a3)
    paddsw        m3, m5			;m3 = (a4 + a9) >> 2  - 5*(a5 + a8) >> 2 + (a6 + a7)
    psraw         m0, 2				;m0 = (a0 + a5) >> 4  - 5*(a1 + a4) >> 4 + (a2 + a3)>>2
    psraw         m3, 2				;m3 = (a4 + a9) >> 4  - 5*(a5 + a8) >> 4 + (a6 + a7)>>2
    paddw         m0, m2			;m0 = (a0 + a5) >> 4  - 5*(a1 + a4) >> 4 + 20*(a2 + a3)>>4
    paddw         m3, m5			;m3 = (a4 + a9) >> 4  - 5*(a5 + a8) >> 4 + 20*(a6 + a7)>>4
    psraw         m0, 6				;
    psraw         m3, 6				;
    packuswb      m0, m3
    op_%1         m0, [r0], m7
    add           r1, 48
    add           r0, r2
    dec          r4d
    jne        .loop
    REP_RET
%endmacro

INIT_MMX mmxext
QPEL8OR16_HV2_LOWPASS_OP put
QPEL8OR16_HV2_LOWPASS_OP avg

%macro QPEL8OR16_HV2_LOWPASS_OP_XMM 1
cglobal %1_h264_qpel8or16_hv2_lowpass, 5,5,8 ; dst, tmp, dstStride, tmpStride, size
    movsxdifnidn  r2, r2d
    movsxdifnidn  r3, r3d
    cmp          r4d, 16
    je         .op16
.loop8:
    mova          m1, [r1+16]
    mova          m0, [r1]
    mova          m2, m1
    mova          m3, m1
    mova          m4, m1
    mova          m5, m1
    palignr       m5, m0, 10
    palignr       m4, m0, 8
    palignr       m3, m0, 6
    palignr       m2, m0, 4
    palignr       m1, m0, 2					;m0 m1 m2 m3 m4 m5
    paddw         m0, m5
    paddw         m1, m4
    paddw         m2, m3
    psubw         m0, m1					;m0 = (a0 + a5) - (a1 + a4)
    psraw         m0, 2						;m0 = (a0 + a5) >> 2 - (a1 + a4) >> 2
    psubw         m0, m1					;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2
    paddw         m0, m2					;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2 + (m2 + m3)
    psraw         m0, 2						;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 4 + (m2 + m3) >> 2
    paddw         m0, m2					;m0 = (a0 + a5) >> 4 - 5*(a1 + a4) >> 4 + 20*(m2 + m3) >> 4
    psraw         m0, 6						;m0 = (a0 + a5) >> 10 - 5*(a1 + a4) >> 10 + 20*(m2 + m3) >> 10
    packuswb      m0, m0
    op_%1h        m0, [r0], m7
    add           r1, 48
    add           r0, r2
    dec          r4d
    jne       .loop8
    jmp        .done
.op16:
    mova          m4, [r1+32]	;third  8-pixels
    mova          m5, [r1+16]	;second 8-pixels
    mova          m7, [r1]		;first  8-pixels
    mova          m3, m4
    mova          m2, m4
    mova          m1, m4
    mova          m0, m4
    palignr       m0, m5, 10
    palignr       m1, m5, 8
    palignr       m2, m5, 6
    palignr       m3, m5, 4
    palignr       m4, m5, 2		;m5 m4 m3 m2 m1 m0
    paddw         m0, m5
    paddw         m1, m4
    paddw         m2, m3
    mova          m6, m5
    mova          m4, m5
    mova          m3, m5
    palignr       m4, m7, 8		
    palignr       m6, m7, 2		
    palignr       m3, m7, 10	;m7 m6 [] [] m4 m3
    paddw         m4, m6
    mova          m6, m5
    palignr       m5, m7, 6		
    palignr       m6, m7, 4		;m7 [] m6 m5 [] m3
    paddw         m3, m7		;
    paddw         m5, m6		;
    psubw         m0, m1		;m0 = (a0 + a5) - (a1 + a4)
    psubw         m3, m4		;
    psraw         m0, 2			;m0 = (a0 + a5) >> 2 - (a1 + a4) >> 2
    psraw         m3, 2
    psubw         m0, m1		;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2
    psubw         m3, m4
    paddw         m0, m2		;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2 + (a2 + a3)
    paddw         m3, m5
    psraw         m0, 2			;m0 = (a0 + a5) >> 4 - 5*(a1 + a4) >> 4 + (a2 + a3) >> 2
    psraw         m3, 2
    paddw         m0, m2		;m0 = (a0 + a5) >> 4 - 5*(a1 + a4) >> 4 + 20*(a2 + a3) >> 4
    paddw         m3, m5
    psraw         m0, 6			;m0 = (a0 + a5) >> 10 - 5*(a1 + a4) >> 10 + 20*(a2 + a3) >> 10
    psraw         m3, 6
    packuswb      m3, m0
    op_%1         m3, [r0], m7
    add           r1, 48
    add           r0, r2
    dec          r4d
    jne        .op16
.done:
    REP_RET
%endmacro

INIT_XMM ssse3
QPEL8OR16_HV2_LOWPASS_OP_XMM put
QPEL8OR16_HV2_LOWPASS_OP_XMM avg


%macro PIXELS4_L2_SHIFT5 1
cglobal %1_pixels4_l2_shift5,6,6 ; dst, src16, src8, dstStride, src8Stride, h
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
    mova          m0, [r1]
    mova          m1, [r1+24]
    psraw         m0, 5
    psraw         m1, 5
    packuswb      m0, m0
    packuswb      m1, m1
    pavgb         m0, [r2]
    pavgb         m1, [r2+r4]
    op_%1h        m0, [r0], m4
    op_%1h        m1, [r0+r3], m5
    lea           r2, [r2+r4*2]
    lea           r0, [r0+r3*2]
    mova          m0, [r1+48]
    mova          m1, [r1+72]
    psraw         m0, 5
    psraw         m1, 5
    packuswb      m0, m0
    packuswb      m1, m1
    pavgb         m0, [r2]
    pavgb         m1, [r2+r4]
    op_%1h        m0, [r0], m4
    op_%1h        m1, [r0+r3], m5
    RET
%endmacro

INIT_MMX mmxext
PIXELS4_L2_SHIFT5 put
PIXELS4_L2_SHIFT5 avg


%macro PIXELS8_L2_SHIFT5 1
cglobal %1_pixels8_l2_shift5, 6, 6 ; dst, src16, src8, dstStride, src8Stride, h
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
.loop:
    mova          m0, [r1]
    mova          m1, [r1+8]
    mova          m2, [r1+48]
    mova          m3, [r1+48+8]
    psraw         m0, 5
    psraw         m1, 5
    psraw         m2, 5
    psraw         m3, 5
    packuswb      m0, m1
    packuswb      m2, m3
    pavgb         m0, [r2]
    pavgb         m2, [r2+r4]
    op_%1         m0, [r0], m4
    op_%1         m2, [r0+r3], m5
    lea           r2, [r2+2*r4]
    add           r1, 48*2
    lea           r0, [r0+2*r3]
    sub          r5d, 2
    jne        .loop
    REP_RET
%endmacro

INIT_MMX mmxext
PIXELS8_L2_SHIFT5 put
PIXELS8_L2_SHIFT5 avg


%if ARCH_X86_64
%macro QPEL16_H_LOWPASS_L2_OP 1
cglobal %1_h264_qpel16_h_lowpass_l2, 5, 6, 16 ; dst, src, src2, dstStride, src2Stride
    movsxdifnidn  r3, r3d
    movsxdifnidn  r4, r4d
    mov          r5d, 16
    pxor         m15, m15
    mova         m14, [pw_5]
    mova         m13, [pw_16]
.loop:
    lddqu         m1, [r1+6]
    lddqu         m7, [r1-2]
    mova          m0, m1
    punpckhbw     m1, m15
    punpcklbw     m0, m15
    punpcklbw     m7, m15
    mova          m2, m1
    mova          m6, m0
    mova          m3, m1
    mova          m8, m0
    mova          m4, m1
    mova          m9, m0
    mova         m12, m0
    mova         m11, m1
    palignr      m11, m0, 10
    palignr      m12, m7, 10
    palignr       m4, m0, 2
    palignr       m9, m7, 2
    palignr       m3, m0, 4
    palignr       m8, m7, 4
    palignr       m2, m0, 6
    palignr       m6, m7, 6
    paddw        m11, m0
    palignr       m1, m0, 8
    palignr       m0, m7, 8
    paddw         m7, m12
    paddw         m2, m3
    paddw         m6, m8
    paddw         m1, m4
    paddw         m0, m9
    psllw         m2, 2
    psllw         m6, 2
    psubw         m2, m1
    psubw         m6, m0
    paddw        m11, m13
    paddw         m7, m13
    pmullw        m2, m14
    pmullw        m6, m14
    lddqu         m3, [r2]
    paddw         m2, m11
    paddw         m6, m7
    psraw         m2, 5
    psraw         m6, 5
    packuswb      m6, m2
    pavgb         m6, m3
    op_%1         m6, [r0], m11
    add           r1, r3
    add           r0, r3
    add           r2, r4
    dec          r5d
    jg         .loop
    REP_RET
%endmacro

INIT_XMM ssse3
QPEL16_H_LOWPASS_L2_OP put
QPEL16_H_LOWPASS_L2_OP avg
%endif


;--------------------------rewrite--------------------------

;void vo_put_h264_qpel8_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
;void vo_avg_h264_qpel8_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);

;m0 = a-2 a-1 a0 a1 a2 a3 a4 a5
;m1 = a-1 a0  a1 a2 a3 a4 a5 a6
;m2 = a0  a1  a2 a3 a4 a5 a6 a7
;m3 = a1  a2  a3 a4 a5 a6 a7 a8
;m4 = a2  a3  a4 a5 a6 a7 a8 a9
;m5 = a3  a4  a5 a6 a7 a8 a9 a10

%macro VO_QPEL8_H_LOWPASS_SSSE3 1
vo_global %1_h264_qpel8_h_lowpass_ssse3_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp+8]		;dst
	mov		ecx,	[ebp+12]	;src
	mov		edx,	[ebp+16]	;dstStride
	mov		ebx,	[ebp+20]	;srcStride
	
	mov		esi,	8
	movdqa	xmm7,	[pw_5]
	pxor	xmm6,	xmm6
	
.loop:
	movdqu		xmm0,	[ecx - 2]	;a-2, a-1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13
	movdqa		xmm1,	xmm0		;
	punpcklbw	xmm0,	xmm6		;m0 = a-2, a-1, a0, a1, a2,  a3,  a4,  a5
	punpckhbw	xmm1,	xmm6		;m1 = a6,  a7,  a8, a9, a10, a11, a12, a13
	
	movdqa		xmm2,	xmm1
	movdqa		xmm3,	xmm1
	movdqa		xmm4,	xmm1
	movdqa		xmm5,	xmm1
	
	palignr		xmm1,	xmm0,	2	;m1 = a-1,a0, a1, a2,  a3,  a4,  a5, a6
	palignr		xmm2,	xmm0,	4	;m2 = a0, a1, a2, a3,  a4,  a5,  a6, a7
	palignr		xmm3,	xmm0,	6	;m3 = a1, a2, a3, a4,  a5,  a6,  a7, a8
	palignr		xmm4,	xmm0,	8	;m4 = a2, a3, a4, a5,  a6,  a7,  a8, a9
	palignr		xmm5,	xmm0,	10	;m5 = a3, a4, a5, a6,  a7,  a8,  a9, a10
	
	paddw		xmm0,	xmm5		;m0 = a-2 + a3,	a-1 + a4, a0 + a5 ...
	paddw		xmm1,	xmm4		;m1 = a-1 + a2, a0  + a3, a1 + a4 ...
	paddw		xmm2,	xmm3		;m2 = a0  + a1, a1  + a2, a2 + a3 ...
	
	psllw		xmm2,	2			;m2 = 4*(a0  + a1), 4*(a1  + a2), 4*(a2 + a3) ...
	psubw		xmm2,	xmm1		;m2 = 4*(a0  + a1) - (a-1 + a2), 4*(a1  + a2) - (a0  + a3), 4*(a2 + a3) - (a1 + a4) ...
	pmullw		xmm2,	xmm7		;m2 = 20*(a0  + a1) - 5*(a-1 + a2), 20*(a1  + a2) - 5*(a0  + a3), 20*(a2 + a3) - 5*(a1 + a4) ...
	paddw		xmm2,	xmm0		;m2 = 20*(a0  + a1) - 5*(a-1 + a2) + (a-2 + a3), 20*(a1  + a2) - 5*(a0  + a3) + (a-1 + a4), 20*(a2 + a3) - 5*(a1 + a4) + (a0 + a5) ...
	paddw		xmm2,	[pw_16]
	psraw		xmm2,	5
	packuswb	xmm2,	xmm2
	
	%ifidn %1, avg
		movq		xmm0,	[eax]
		pavgb		xmm2,	xmm0
		movq		[eax],	xmm2
	%elifidn  %1, put
		movq		[eax],	xmm2
	%endif
	
	add			eax,	edx
	add			ecx,	ebx
	dec			esi
	jnz			.loop
	
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

VO_QPEL8_H_LOWPASS_SSSE3 put
VO_QPEL8_H_LOWPASS_SSSE3 avg

;void vo_put_h264_qpel8_h_lowpass_mmxext_rewrite(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
;void vo_avg_h264_qpel8_h_lowpass_mmxext_rewrite(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);

%macro VO_QPEL8_H_LOWPASS_MMXEXT 1
vo_global %1_h264_qpel8_h_lowpass_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp+8]		;dst
	mov		ecx,	[ebp+12]	;src
	mov		edx,	[ebp+16]	;dstStride
	mov		ebx,	[ebp+20]	;srcStride
	
	pxor	mm6,	mm6
	movq	mm7,	[pw_5]
	mov		esi,	8
	
.loop:
	movq	mm2,	[ecx]			;m2 = a2 a3 a4 a5 a6 a7 a8 a9
	movq	mm4,	[ecx + 1]		;m4 = a3 a4 a5 a6 a7 a8 a9 a10
	movq	mm3,	mm2
	movq	mm5,	mm4
	punpcklbw	mm2,	mm6			;m2 = a2 a3 a4 a5
	punpckhbw	mm3,	mm6			;m3 = a6 a7 a8 a9
	punpcklbw	mm4,	mm6			;m4 = a3 a4 a5 a6
	punpckhbw	mm5,	mm6			;m5 = a7 a8 a9 a10
	paddw		mm2,	mm4			;m2 = a2 + a3 : (a2 a3 a4 a5) + (a3 a4 a5 a6)
	paddw		mm3,	mm5			;m3 = a6 + a7 : (a6 a7 a8 a9) + (a7 a8 a9 a10)
	
	movq	mm1,	[ecx - 1]		;m1 = a1 a2 a3 a4 a5 a6 a7  a8
	movq	mm5,	[ecx + 2]		;m5 = a4 a5 a6 a7 a8 a9 a10 a11
	movq	mm4,	mm1				
	movq	mm0,	mm5
	punpcklbw	mm1,	mm6			;m1 = a1 a2 a3  a4
	punpckhbw	mm4,	mm6			;m4 = a5 a6 a7  a8
	punpcklbw	mm0,	mm6			;m0 = a4 a5 a6  a7
	punpckhbw	mm5,	mm6			;m5 = a8 a9 a10 a11
	paddw		mm1,	mm0			;m1 = a1 + a4 : (a1 a2 a3  a4) + (a4 a5 a6 a7)
	paddw		mm5,	mm4			;m5 = a5 + a8 : (a5 a6 a7  a8) + (a8 a9 a10 a11)
	
	psllw		mm2,	2			;m2 = 4*(a2 + a3) : 4*(a2 a3 a4 a5) + 4*(a3 a4 a5 a6)
	psllw		mm3,	2			;m3 = 4*(a6 + a7) : 4*(a6 a7 a8 a9) + 4*(a7 a8 a9 a10)
	psubw		mm2,	mm1			;m2 = 4*(a2 + a3) - (a1 + a4)
	psubw		mm3,	mm5			;m3 = 4*(a6 + a7) - (a5 + a8)
	pmullw		mm2,	mm7			;m2 = 20*(a2 + a3) - 5*(a1 + a4)
	pmullw		mm3,	mm7			;m3 = 20*(a6 + a7) - 5*(a5 + a8)
	
	movd		mm5,	[ecx - 2]	;m5 = a0 a1  a2  a3
	movd		mm1,	[ecx + 7]	;m1 = a9 a10 a11 a12
	punpcklbw	mm5,	mm6			;
	punpcklbw	mm1,	mm6			;
	paddw		mm5,	mm4			;m5 = a0 + a5 : (a0 a1  a2  a3) + (a5 a6  a7  a8)
	paddw		mm1,	mm0			;m1 = a4 + a9 : (a4 a5  a6  a7) + (a9 a10 a11 a12)
	movq		mm4,	[pw_16]
	paddw		mm5,	mm4			;m5 = (a0 + a5) + 16
	paddw		mm1,	mm4			;m1 = (a4 + a9) + 16
	paddw		mm5,	mm2			;m5 = (a0 + a5) - 5*(a1 + a4) + 20*(a2 + a3) + 16
	paddw		mm1,	mm3			;m1 = (a4 + a9) - 5*(a5 + a8) + 20*(a6 + a7) + 16
	psraw		mm5,	5
	psraw		mm1,	5
	packuswb	mm5,	mm1
	
	%ifidn %1, avg
		movq		mm0,	[eax]
		pavgb		mm5,	mm0
		movq		[eax],	mm5
	%elifidn  %1, put
		movq		[eax],	mm5
	%endif
	
	add			eax,	edx
	add			ecx,	ebx
	dec			esi
	jg			.loop
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

VO_QPEL8_H_LOWPASS_MMXEXT put
VO_QPEL8_H_LOWPASS_MMXEXT avg
	
;m0 = B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 B10 B11 B12 B13 B14 B15
;m1 = A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12 A13 A14 A15
;m2 = a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15
;m3 = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
;m4 = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15
;m5 = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15

;block width = 8
;block height = 8 or 16
	
;void vo_put_h264_qpel8or16_v_lowpass_sse2_rewrite(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);
;void vo_avg_h264_qpel8or16_v_lowpass_sse2_rewrite(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);

%macro QPEL8OR16_V_LOWPASS_SSE2 1
vo_global %1_h264_qpel8or16_v_lowpass_sse2_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]			;dst
	mov		ecx,	[ebp + 12]			;src
	mov		edx,	[ebp + 16]			;dstStride
	mov		ebx,	[ebp + 20]			;srcStride
	mov		esi,	[ebp + 24]			;h
	
	pxor	xmm6,	xmm6
	
	lea		edi,	[2*ebx]
	neg		edi
	add		edi,	ecx

	movq	xmm0,	[edi]				;m0 = B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 B10 B11 B12 B13 B14 B15
	movq	xmm1,	[edi + ebx]			;m1 = A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12 A13 A14 A15
	movq	xmm2,	[ecx]				;m2 = a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15
	movq	xmm3,	[ecx + ebx]		;m3 = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
	lea		edi,	[edi + 4*ebx]
	movq	xmm4,	[edi]				;m4 = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15
	movq	xmm5,	[edi + ebx]			;m5 = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
	
	punpcklbw	xmm0,	xmm6
	punpcklbw	xmm1,	xmm6
	punpcklbw	xmm2,	xmm6
	punpcklbw	xmm3,	xmm6
	punpcklbw	xmm4,	xmm6
	punpcklbw	xmm5,	xmm6
	
	paddw		xmm0,	xmm5			;m0 m1 m2 m3 m4 m5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	%ifidn %1, avg
		movq		xmm0,	[eax]
		pavgb		xmm7,	xmm0
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	
	lea			edi,	[edi + 2*ebx]
	movq		xmm0,	[edi]
	punpcklbw	xmm0,	xmm6			;m1 m2 m3 m4 m5 m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm1,	[eax]
		pavgb		xmm7,	xmm1
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm1,	[edi]
	punpcklbw	xmm1,	xmm6			;m2 m3 m4 m5 m0 m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm2,	[eax]
		pavgb		xmm7,	xmm2
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	
	lea			edi,	[edi + ebx]
	movq		xmm2,	[edi]			;m3 m4 m5 m0 m1 m2
	punpcklbw	xmm2,	xmm6
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm3,	[eax]
		pavgb		xmm7,	xmm3
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm3,	[edi]
	punpcklbw	xmm3,	xmm6			;m4 m5 m0 m1 m2 m3
	paddw		xmm4,	xmm3
	movdqa		xmm7,	xmm0
	paddw		xmm7,	xmm1
	psllw		xmm7,	2
	psubw		xmm7,	xmm5
	psubw		xmm7,	xmm2
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm4
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm4,	[eax]
		pavgb		xmm7,	xmm4
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm4,	[edi]
	punpcklbw	xmm4,	xmm6			;m5 m0 m1 m2 m3 m4
	paddw		xmm5,	xmm4
	movdqa		xmm7,	xmm1
	paddw		xmm7,	xmm2
	psllw		xmm7,	2
	psubw		xmm7,	xmm0
	psubw		xmm7,	xmm3
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm5
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm5,	[eax]
		pavgb		xmm7,	xmm5
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm5,	[edi]
	punpcklbw	xmm5,	xmm6			;m0 m1 m2 m3 m4 m5
	paddw		xmm0,	xmm5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm0,	[eax]
		pavgb		xmm7,	xmm0
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm0,	[edi]
	punpcklbw	xmm0,	xmm6			;m1 m2 m3 m4 m5 m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm1,	[eax]
		pavgb		xmm7,	xmm1
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
				
	cmp			esi,	8
	je			.end
	
	lea			edi,	[edi + ebx]
	movq		xmm1,	[edi]
	punpcklbw	xmm1,	xmm6			;m2 m3 m4 m5 m0 m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm2,	[eax]
		pavgb		xmm7,	xmm2
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm2,	[edi]
	punpcklbw	xmm2,	xmm6			;m3 m4 m5 m0 m1 m2
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm3,	[eax]
		pavgb		xmm7,	xmm3
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm3,	[edi]
	punpcklbw	xmm3,	xmm6			;m4 m5 m0 m1 m2	m3
	paddw		xmm4,	xmm3
	movdqa		xmm7,	xmm0
	paddw		xmm7,	xmm1
	psllw		xmm7,	2
	psubw		xmm7,	xmm5
	psubw		xmm7,	xmm2
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm4
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm4,	[eax]
		pavgb		xmm7,	xmm4
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm4,	[edi]
	punpcklbw	xmm4,	xmm6		;m5 m0 m1 m2 m3 m4
	paddw		xmm5,	xmm4
	movdqa		xmm7,	xmm1
	paddw		xmm7,	xmm2
	psllw		xmm7,	2
	psubw		xmm7,	xmm0
	psubw		xmm7,	xmm3
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm5
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm5,	[eax]
		pavgb		xmm7,	xmm5
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm5,	[edi]
	punpcklbw	xmm5,	xmm6		;m0 m1 m2 m3 m4 m5
	paddw		xmm0,	xmm5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm0,	[eax]
		pavgb		xmm7,	xmm0
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm0,	[edi]
	punpcklbw	xmm0,	xmm6		;m1 m2 m3 m4 m5 m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm1,	[eax]
		pavgb		xmm7,	xmm1
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm1,	[edi]
	punpcklbw	xmm1,	xmm6		;m2 m3 m4 m5 m0 m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm2,	[eax]
		pavgb		xmm7,	xmm2
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	lea			edi,	[edi + ebx]
	movq		xmm2,	[edi]
	punpcklbw	xmm2,	xmm6		;m3 m4 m5 m0 m1 m2
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	psraw		xmm7,	5
	packuswb	xmm7,	xmm7
	lea			eax,	[eax + edx]
	%ifidn %1, avg
		movq		xmm3,	[eax]
		pavgb		xmm7,	xmm3
		movq		[eax],	xmm7
	%elifidn  %1, put
		movq		[eax],	xmm7
	%endif
	
	
.end:	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro


QPEL8OR16_V_LOWPASS_SSE2 put
QPEL8OR16_V_LOWPASS_SSE2 avg
	
;void vo_put_h264_qpel8_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
;void vo_avg_h264_qpel8_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);

%macro QPEL8_H_LOWPASS_L2_SSSE3 1
vo_global	%1_h264_qpel8_h_lowpass_l2_ssse3_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]			;dst
	mov		ecx,	[ebp + 12]			;src
	mov		edx,	[ebp + 16]			;src2
	mov		ebx,	[ebp + 20]			;dstStride
	mov		esi,	[ebp + 24]			;src2Stride
	
	mov		edi,	8
	pxor	xmm6,	xmm6
	movdqa	xmm7,	[pw_5]
	
.loop:
	movdqu		xmm0,	[ecx - 2]		;a-2, a-1, a0, a1, a2, a3, a4, a5, a6, a7,a8,a9,a10,a11,a12,a13
	movdqa		xmm1,	xmm0
	punpcklbw	xmm0,	xmm6			;a-2, a-1, a0, a1, a2, a3, a4, a5
	punpckhbw	xmm1,	xmm6			;a6,  a7,  a8, a9, a10,a11,a12,a13
	movdqa		xmm2,	xmm1
	movdqa		xmm3,	xmm1
	movdqa		xmm4,	xmm1
	movdqa		xmm5,	xmm1
	palignr		xmm1,	xmm0,	2		;a-1, a0, a1, a2, a3, a4, a5, a6
	palignr		xmm2,	xmm0,	4		;a0, a1, a2, a3, a4, a5, a6,  a7
	palignr		xmm3,	xmm0,	6		;a1, a2, a3, a4, a5, a6,  a7, a8
	palignr		xmm4,	xmm0,	8		;a2, a3, a4, a5, a6,  a7, a8, a9
	palignr		xmm5,	xmm0,	10		;a3, a4, a5, a6,  a7, a8, a9, a10
	
	paddw		xmm0,	xmm5
	paddw		xmm1,	xmm4
	paddw		xmm3,	xmm2
	psllw		xmm3,	2
	psubw		xmm3,	xmm1
	pmullw		xmm3,	xmm7
	paddw		xmm3,	xmm0
	paddw		xmm3,	[pw_16]
	psraw		xmm3,	5
	packuswb	xmm3,	xmm3
	movq		xmm2,	[edx]
	pavgb		xmm3,	xmm2
	
	%ifidn %1, avg
		movq		xmm0,	[eax]
		pavgb		xmm3,	xmm0
		movq		[eax],	xmm3
	%elifidn %1, put
		movq		[eax],	xmm3
	%endif

	add			eax,	ebx
	add			ecx,	ebx
	add			edx,	esi
	dec			edi
	jnz			.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

QPEL8_H_LOWPASS_L2_SSSE3 put
QPEL8_H_LOWPASS_L2_SSSE3 avg	


;void vo_put_h264_qpel8_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
;void vo_avg_h264_qpel8_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);

%macro QPEL8_H_LOWPASS_L2_MMXEXT 1
vo_global %1_h264_qpel8_h_lowpass_l2_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;src
	mov		edx,	[ebp + 16]		;src2
	mov		ebx,	[ebp + 20]		;dstStride
	mov		esi,	[ebp + 24]		;src2Stride
	
	pxor	mm6,	mm6
	movq	mm7,	[pw_5]
	mov		edi,	8
	
.loop:
	movq	mm2,	[ecx]			;m2 = a2 a3 a4 a5 a6 a7 a8 a9
	movq	mm4,	[ecx + 1]		;m4 = a3 a4 a5 a6 a7 a8 a9 a10
	movq	mm3,	mm2
	movq	mm5,	mm4
	punpcklbw	mm2,	mm6			;m2 = a2 a3 a4 a5
	punpckhbw	mm3,	mm6			;m3 = a6 a7 a8 a9
	punpcklbw	mm4,	mm6			;m4 = a3 a4 a5 a6
	punpckhbw	mm5,	mm6			;m5 = a7 a8 a9 a10
	paddw		mm2,	mm4			;m2 = a2 + a3 : (a2 a3 a4 a5) + (a3 a4 a5 a6)
	paddw		mm3,	mm5			;m3 = a6 + a7 : (a6 a7 a8 a9) + (a7 a8 a9 a10)
	
	movq	mm1,	[ecx - 1]		;m1 = a1 a2 a3 a4 a5 a6 a7  a8
	movq	mm5,	[ecx + 2]		;m5 = a4 a5 a6 a7 a8 a9 a10 a11
	movq	mm4,	mm1				
	movq	mm0,	mm5
	punpcklbw	mm1,	mm6			;m1 = a1 a2 a3  a4
	punpckhbw	mm4,	mm6			;m4 = a5 a6 a7  a8
	punpcklbw	mm0,	mm6			;m0 = a4 a5 a6  a7
	punpckhbw	mm5,	mm6			;m5 = a8 a9 a10 a11
	paddw		mm1,	mm0			;m1 = a1 + a4 : (a1 a2 a3  a4) + (a4 a5 a6 a7)
	paddw		mm5,	mm4			;m5 = a5 + a8 : (a5 a6 a7  a8) + (a8 a9 a10 a11)
	
	psllw		mm2,	2			;m2 = 4*(a2 + a3) : 4*(a2 a3 a4 a5) + 4*(a3 a4 a5 a6)
	psllw		mm3,	2			;m3 = 4*(a6 + a7) : 4*(a6 a7 a8 a9) + 4*(a7 a8 a9 a10)
	psubw		mm2,	mm1			;m2 = 4*(a2 + a3) - (a1 + a4)
	psubw		mm3,	mm5			;m3 = 4*(a6 + a7) - (a5 + a8)
	pmullw		mm2,	mm7			;m2 = 20*(a2 + a3) - 5*(a1 + a4)
	pmullw		mm3,	mm7			;m3 = 20*(a6 + a7) - 5*(a5 + a8)
	
	movd		mm5,	[ecx - 2]	;m5 = a0 a1  a2  a3
	movd		mm1,	[ecx + 7]	;m1 = a9 a10 a11 a12
	punpcklbw	mm5,	mm6			;
	punpcklbw	mm1,	mm6			;
	paddw		mm5,	mm4			;m5 = a0 + a5 : (a0 a1  a2  a3) + (a5 a6  a7  a8)
	paddw		mm1,	mm0			;m1 = a4 + a9 : (a4 a5  a6  a7) + (a9 a10 a11 a12)
	movq		mm4,	[pw_16]
	paddw		mm5,	mm4			;m5 = (a0 + a5) + 16
	paddw		mm1,	mm4			;m1 = (a4 + a9) + 16
	paddw		mm5,	mm2			;m5 = (a0 + a5) - 5*(a1 + a4) + 20*(a2 + a3) + 16
	paddw		mm1,	mm3			;m1 = (a4 + a9) - 5*(a5 + a8) + 20*(a6 + a7) + 16
	psraw		mm5,	5
	psraw		mm1,	5
	packuswb	mm5,	mm1
	movq		mm0,	[edx]
	pavgb		mm5,	mm0
	vo_mmx_%1	mm5, [eax],mm0
	
	lea			eax,	[eax + ebx]
	lea			ecx,	[ecx + ebx]
	lea			edx,	[edx + esi]
	dec			edi
	jnz			.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

QPEL8_H_LOWPASS_L2_MMXEXT put
QPEL8_H_LOWPASS_L2_MMXEXT avg
	
;void vo_put_h264_qpel8or16_hv1_lowpass_op_sse2(VO_U8 *src, VO_S16 *tmp, int srcStride, int size);
vo_global put_h264_qpel8or16_hv1_lowpass_op_sse2_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp + 8]		;src
	mov		ecx,	[ebp + 12]		;tmp
	mov		edx,	[ebp + 16]		;srcStride
	mov		ebx,	[ebp + 20]		;size
	
	lea		esi,	[2 * edx]		;2*srcStride
	pxor	xmm6,	xmm6
	
	movq	xmm0,	[eax]
	movq	xmm1,	[eax + edx]
	lea		eax,	[eax + esi]
	movq	xmm2,	[eax]
	movq	xmm3,	[eax + edx]
	lea		eax,	[eax + esi]
	movq	xmm4,	[eax]			
	movq	xmm5,	[eax + edx]
	lea		eax,	[eax + esi]
	
	punpcklbw	xmm0,	xmm6
	punpcklbw	xmm1,	xmm6
	punpcklbw	xmm2,	xmm6
	punpcklbw	xmm3,	xmm6
	punpcklbw	xmm4,	xmm6
	punpcklbw	xmm5,	xmm6
	
	paddw		xmm0,	xmm5		;m0,m1,m2,m3,m4,m5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx],	xmm7
	
	movq		xmm0,	[eax]
	punpcklbw	xmm0,	xmm6		;m1,m2,m3,m4,m5,m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 48],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm1,	[eax]
	punpcklbw	xmm1,	xmm6		;m2,m3,m4,m5,m0,m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 96],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm2,	[eax]
	punpcklbw	xmm2,	xmm6		;m3,m4,m5,m0,m1,m2
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 144],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm3,	[eax]
	punpcklbw	xmm3,	xmm6		;m4,m5,m0,m1,m2,m3
	paddw		xmm4,	xmm3
	movdqa		xmm7,	xmm0
	paddw		xmm7,	xmm1
	psllw		xmm7,	2
	psubw		xmm7,	xmm5
	psubw		xmm7,	xmm2
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm4
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 192],	xmm7
	
	
	lea			eax,	[eax + edx]
	movq		xmm4,	[eax]
	punpcklbw	xmm4,	xmm6		;m5,m0,m1,m2,m3,m4
	paddw		xmm5,	xmm4
	movdqa		xmm7,	xmm1
	paddw		xmm7,	xmm2
	psllw		xmm7,	2
	psubw		xmm7,	xmm0
	psubw		xmm7,	xmm3
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm5
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 240],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm5,	[eax]
	punpcklbw	xmm5,	xmm6		;m0,m1,m2,m3,m4,m5
	paddw		xmm0,	xmm5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 288],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm0,	[eax]
	punpcklbw	xmm0,	xmm6		;m1,m2,m3,m4,m5,m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 336],	xmm7
	
	cmp			ebx,	8
	je			.done
	
	lea			eax,	[eax + edx]
	movq		xmm1,	[eax]
	punpcklbw	xmm1,	xmm6		;m2,m3,m4,m5,m0,m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 384],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm2,	[eax]
	punpcklbw	xmm2,	xmm6		;m3,m4,m5,m0,m1,m2
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 432],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm3,	[eax]
	punpcklbw	xmm3,	xmm6		;m4,m5,m0,m1,m2,m3
	paddw		xmm4,	xmm3
	movdqa		xmm7,	xmm0
	paddw		xmm7,	xmm1
	psllw		xmm7,	2
	psubw		xmm7,	xmm5
	psubw		xmm7,	xmm2
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm4
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 480],	xmm7
	
	
	lea			eax,	[eax + edx]
	movq		xmm4,	[eax]
	punpcklbw	xmm4,	xmm6		;m5,m0,m1,m2,m3,m4
	paddw		xmm5,	xmm4
	movdqa		xmm7,	xmm1
	paddw		xmm7,	xmm2
	psllw		xmm7,	2
	psubw		xmm7,	xmm0
	psubw		xmm7,	xmm3
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm5
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 528],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm5,	[eax]
	punpcklbw	xmm5,	xmm6		;m0,m1,m2,m3,m4,m5
	paddw		xmm0,	xmm5
	movdqa		xmm7,	xmm2
	paddw		xmm7,	xmm3
	psllw		xmm7,	2
	psubw		xmm7,	xmm1
	psubw		xmm7,	xmm4
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm0
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 576],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm0,	[eax]
	punpcklbw	xmm0,	xmm6		;m1,m2,m3,m4,m5,m0
	paddw		xmm1,	xmm0
	movdqa		xmm7,	xmm3
	paddw		xmm7,	xmm4
	psllw		xmm7,	2
	psubw		xmm7,	xmm2
	psubw		xmm7,	xmm5
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm1
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 624],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm1,	[eax]
	punpcklbw	xmm1,	xmm6		;m2,m3,m4,m5,m0,m1
	paddw		xmm2,	xmm1
	movdqa		xmm7,	xmm4
	paddw		xmm7,	xmm5
	psllw		xmm7,	2
	psubw		xmm7,	xmm3
	psubw		xmm7,	xmm0
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm2
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 672],	xmm7
	
	lea			eax,	[eax + edx]
	movq		xmm2,	[eax]
	punpcklbw	xmm2,	xmm6		;m3,m4,m5,m0,m1,m2
	paddw		xmm3,	xmm2
	movdqa		xmm7,	xmm5
	paddw		xmm7,	xmm0
	psllw		xmm7,	2
	psubw		xmm7,	xmm4
	psubw		xmm7,	xmm1
	pmullw		xmm7,	[pw_5]
	paddw		xmm7,	xmm3
	paddw		xmm7,	[pw_16]
	;psraw		xmm7,	5
	movdqa		[ecx + 720],	xmm7
	
.done
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
	
;alogrithm is as below:

;m0 = (a0 + a5) - (a1 + a4)
;m0 = (a0 + a5) >> 2 - (a1 + a4) >> 2
;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2
;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 2 + (m2 + m3)
;m0 = (a0 + a5) >> 2 - 5*(a1 + a4) >> 4 + (m2 + m3) >> 2
;m0 = (a0 + a5) >> 4 - 5*(a1 + a4) >> 4 + 20*(m2 + m3) >> 4
;m0 = (a0 + a5) >> 10 - 5*(a1 + a4) >> 10 + 20*(m2 + m3) >> 10
    
;void vo_put_h264_qpel8or16_hv2_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size)
;void vo_avg_h264_qpel8or16_hv2_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size)

%macro QPEL8OR16_HV2_LOWPASS_SSSE3 1
vo_global %1_h264_qpel8or16_hv2_lowpass_ssse3_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;tmp
	mov		edx,	[ebp + 16]		;dstStride
	mov		ebx,	[ebp + 20]		;tmpStride
	mov		esi,	[ebp + 24]		;size
	
	cmp		esi,	16
	je		.loop16
	
.loop8:
	movdqa	xmm0,	[ecx]			;a-2 a-1 a0 a1  a2  a3  a4  a5
	movdqa	xmm1,	[ecx + 16]		;a6  a7  a8 a9  a10 a11 a12 a13
	movdqa	xmm2,	xmm1			;
	movdqa	xmm3,	xmm1			;
	movdqa	xmm4,	xmm1			;
	movdqa	xmm5,	xmm1
	palignr	xmm5,	xmm0,	10
	paddw	xmm5,	xmm0
	palignr	xmm1,	xmm0,	2		;
	palignr	xmm4,	xmm0,	8
	paddw	xmm1,	xmm4
	palignr	xmm2,	xmm0,	4
	palignr	xmm3,	xmm0,	6
	paddw	xmm2,	xmm3
	
	psubw	xmm5,	xmm1
	psraw	xmm5,	2
	psubw	xmm5,	xmm1
	paddw	xmm5,	xmm2
	psraw	xmm5,	2
	paddw	xmm5,	xmm2
	psraw	xmm5,	6
	packuswb	xmm5,	xmm5
	vo_sse_%1_half xmm5, [eax], xmm4
	
	lea		ecx,	[ecx + 48]
	lea		eax,	[eax + edx]

	dec		esi
	jnz		.loop8
	jmp		.end
	
.loop16:
	movdqa	xmm0,	[ecx]
	movdqa	xmm1,	[ecx + 16]
	movdqa	xmm2,	[ecx + 32]
	
	movdqa	xmm3,	xmm1
	movdqa	xmm4,	xmm1
	movdqa	xmm5,	xmm1
	
	palignr	xmm3,	xmm0,	10		;m0,[],[],[],[],m3
	paddw	xmm3,	xmm0			;m3 = (a0 + a5)
	palignr	xmm4,	xmm0,	2
	palignr	xmm5,	xmm0,	8		;m0,a4,[],[],a5,m3
	paddw	xmm4,	xmm5			;m4 = (a1 + a4)
	psubw	xmm3,	xmm4
	psraw	xmm3,	2
	psubw	xmm3,	xmm4
	movdqa	xmm5,	xmm1
	movdqa	xmm6,	xmm1
	palignr	xmm5,	xmm0,	4
	palignr	xmm6,	xmm0,	6		
	paddw	xmm5,	xmm6			;m5 = (a2 + a3)
	paddw	xmm3,	xmm5
	psraw	xmm3,	2
	paddw	xmm3,	xmm5
	psraw	xmm3,	6
	
	movdqa	xmm0,	xmm2
	movdqa	xmm4,	xmm2	
	movdqa	xmm5,	xmm2
	
	palignr	xmm0,	xmm1,	10
	paddw	xmm0,	xmm1
	palignr	xmm4,	xmm1,	2
	palignr	xmm5,	xmm1,	8
	paddw	xmm4,	xmm5
	psubw	xmm0,	xmm4
	psraw	xmm0,	2
	psubw	xmm0,	xmm4
	movdqa	xmm4,	xmm2	
	;movdqa	xmm5,	xmm2
	palignr	xmm4,	xmm1,	4
	palignr	xmm2,	xmm1,	6
	paddw	xmm2,	xmm4
	paddw	xmm0,	xmm2
	psraw	xmm0,	2
	paddw	xmm0,	xmm2
	psraw	xmm0,	6
	
	packuswb	xmm3,	xmm0
	vo_sse_%1   xmm3, [eax], xmm4

	lea		ecx,	[ecx + 48]
	lea		eax,	[eax + edx]

	dec		esi
	jnz		.loop16
	
.end:
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

QPEL8OR16_HV2_LOWPASS_SSSE3 put
QPEL8OR16_HV2_LOWPASS_SSSE3 avg


;void vo_put_h264_qpel8or16_hv2_lowpass_op_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int unused, int h);
;void vo_avg_h264_qpel8or16_hv2_lowpass_op_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int unused, int h);
%macro QPEL8OR16_HV2_LOWPASS_OP_MMXEXT 1
vo_global %1_h264_qpel8or16_hv2_lowpass_op_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	
	mov		eax,	[ebp+8]			;dst
	mov		ecx,	[ebp+12]		;tmp
	mov		edx,	[ebp+16]		;dstStride
	mov		ebx,	[ebp+20]		;unused
	mov		esi,	[ebp+24]		;h
	
.loop:
	movq	mm0,	[ecx]			;a0 a1 a2 a3
	movq	mm1,	[ecx + 2]		;a1 a2 a3 a4
	movq	mm4,	[ecx + 8]		;a4 a5 a6 a7
	movq	mm5,	[ecx + 10]		;a5 a6 a7 a8
	paddw	mm0,	mm5
	paddw	mm1,	mm4
	psubw	mm0,	mm1
	psraw	mm0,	2
	psubw	mm0,	mm1
	
	paddw	mm4,	[ecx + 18]		;a9  a10 a11 a12
	paddw	mm5,	[ecx + 16]		;a8  a9  a10 a11
	psubw	mm4,	mm5
	psraw	mm4,	2
	psubw	mm4,	mm5
	
	movq	mm2,	[ecx + 4]		;a2 a3 a4 a5
	;movq	mm3,	[ecx + 6]		;a3 a4 a5 a6
	paddw	mm2,	[ecx + 6]
	paddw	mm0,	mm2
	psraw	mm0,	2
	paddw	mm0,	mm2
	psraw	mm0,	6
	
	movq	mm2,	[ecx + 12]		;a6 a7 a8 a9
	;movq	mm3,	[ecx + 14]		;a7 a8 a9 a10
	paddw	mm2,	[ecx + 14]
	paddw	mm4,	mm2
	psraw	mm4,	2
	paddw	mm4,	mm2
	psraw	mm4,	6
	
	packuswb	mm0,	mm4
	;movq		[eax],	mm0
	vo_mmx_%1	mm0,[eax],mm4
	
	lea		eax,	[eax + edx]
	lea		ecx,	[ecx + 48]
	dec		esi
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	8
	pop		esi
	pop		ebx
	leave
	ret
%endmacro

QPEL8OR16_HV2_LOWPASS_OP_MMXEXT put
QPEL8OR16_HV2_LOWPASS_OP_MMXEXT avg
	
;vo_put_pixels8_l2_shift5_mmxext : dst, src16, src8, dstStride, src8Stride, h
;vo_avg_pixels8_l2_shift5_mmxext : dst, src16, src8, dstStride, src8Stride, h

%macro PIXELS8_L2_SHIFT5_MMXEXT 1
vo_global %1_pixels8_l2_shift5_mmxext_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;src16
	mov		edx,	[ebp + 16]		;src8
	mov		ebx,	[ebp + 20]		;dstStride
	mov		esi,	[ebp + 24]		;src8Stride
	mov		edi,	[ebp + 28]		;h
	
.loop:
	movq	mm0,	[ecx]
	movq	mm1,	[ecx + 8]
	movq	mm2,	[ecx + 48]
	movq	mm3,	[ecx + 56]
	psraw	mm0,	5
	psraw	mm1,	5
	psraw	mm2,	5
	psraw	mm3,	5
	packuswb	mm0,	mm1
	packuswb	mm2,	mm3
	pavgb	mm0,	[edx]
	pavgb	mm2,	[edx + esi]
	;movq	[eax],	mm0
	;movq	[eax + ebx],	mm2
	vo_mmx_%1 mm0,[eax],		mm1
	vo_mmx_%1 mm2,[eax + ebx],	mm1
	
	lea		eax,	[eax + 2*ebx]
	lea		edx,	[edx + 2*esi]
	lea		ecx,	[ecx + 96]
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

PIXELS8_L2_SHIFT5_MMXEXT put
PIXELS8_L2_SHIFT5_MMXEXT avg
	
;vo_put_pixels8_l2_shift5_sse2 : dst, src16, src8, dstStride, src8Stride, h
vo_global put_pixels8_l2_shift5_sse2_rewrite
	push	ebp
	mov		ebp,	esp
	push	ebx
	push	esi
	push	edi
	
	mov		eax,	[ebp + 8]		;dst
	mov		ecx,	[ebp + 12]		;src16
	mov		edx,	[ebp + 16]		;src8
	mov		ebx,	[ebp + 20]		;dstStride
	mov		esi,	[ebp + 24]		;src8Stride
	mov		edi,	[ebp + 28]		;h
	
.loop:
	movdqu	xmm0,	[ecx]
	movdqu	xmm1,	[ecx + 48]
	psraw	xmm0,	5
	psraw	xmm1,	5
	packuswb	xmm0,	xmm0
	packuswb	xmm1,	xmm1
	
	movdqu	xmm2,	[edx]
	movdqu	xmm3,	[edx + esi]
	
	pavgb	xmm0,	xmm2
	pavgb	xmm1,	xmm3
	movq	[eax],	xmm0
	movq	[eax + ebx],	xmm1
	
	lea		eax,	[eax + 2*ebx]
	lea		edx,	[edx + 2*esi]
	lea		ecx,	[ecx + 96]
	sub		edi,	2
	jnz		.loop
	
	mov		esp,	ebp
	sub		esp,	12
	pop		edi
	pop		esi
	pop		ebx
	leave
	ret