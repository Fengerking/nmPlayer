@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

 p_ref     .req r0
 ptrA    .req r0

 mb      .req r2
 block   .req r2

 n_x0      .req r1
 n_cnt   .req r1

 y0      .req r3
 n_valX    .req r3

 n_w   .req r4

 n_h  .req r5
 n_tmp7    .req r5

 chrPW   .req r6
 n_tmp8    .req r6

 n_tmp1    .req r7
 chrPH   .req r7

 n_tmp2    .req r8

 n_tmp3    .req r9

 n_tmp4    .req r10

 n_tmp5    .req r11

 n_tmp6    .req r12

 c32     .req r14
 xFrac   .req r14

@// Function exports and imports


    .global  get_chroma_X0_ARMV6
    .global  add_chroma_X0_ARMV6

@//  Function arguments
@//
@//  u8 *p_ref,                   : 0xc4
@//  u8 *predPartChroma,        : 0xc8
@//  i32 n_x0,                    : 0xcc
@//  i32 y0,                    : 0xd0
@//  u32 n_w,                 : 0xf8
@//  u32 n_h,                : 0xfc
@//  u32 xFrac,                 : 0x100
@//  u32 chromaPartWidth,       : 0x104
@//  u32 chromaPartHeight       : 0x108

 pSrcStride    =   0x04
 pDstStride    =   0x0C
 pw00          =   0x34
 pw01          =   0x38
 pw10          =   0x3C
 pPartW        =   0x40
 pPartH        =   0x44

@extern void get_chroma_X0_ARMV6(VO_U8 *p_Src, int src_stride ,VO_U8 *p_Dst,VO_S32 dst_stride, int w00, int w01, int w10, int n_partW, int n_partH)
get_chroma_X0_ARMV6:
    STMFD   sp!, {r0-r11,lr}

    LDR     chrPW, [sp, #pPartW]     @// chromaPartWidth
    LDR     n_w, [sp, #pSrcStride]      @// n_w
    LDR     chrPH, [sp, #pPartH]     @// chromaPartHeight
@   MLA     n_tmp3, y0, n_w, n_x0     ;// n_tmp3 = y0*n_w+n_x0
    LDR     xFrac, [sp, #pw00]     @// xFrac
@   ADD     ptrA, p_ref, n_tmp3         ;// ptrA = p_ref + y0*n_w+n_x0
    RSB     n_valX, xFrac, #8         @// n_valX = 8-xFrac
@    LDR     n_valX, [sp, #pw00]

@   LDR     mb, [sp, #0xc8]         ;// predPartChroma

    @// pack values to n_cnt register
    @// [31:28] loop_x (chromaPartWidth-1)
    @// [27:24] loop_y (chromaPartHeight-1)
    @// [23:20] chromaPartWidth-1
    @// [19:16] chromaPartHeight-1
    @// [15:00] nothing

    SUB     n_tmp2, chrPH, #1             @// chromaPartHeight-1
    SUB     n_tmp1, chrPW, #1             @// chromaPartWidth-1
    ADD     n_cnt, n_cnt, n_tmp2, LSL #16 @// chromaPartHeight-1
    ADD     n_cnt, n_cnt, n_tmp2, LSL #24 @// loop_y
    ADD     n_cnt, n_cnt, n_tmp1, LSL #20 @// chromaPartWidth-1
    AND     n_tmp2, n_cnt, #0x00F00000    @// loop_x
    PKHBT   n_valX, n_valX, xFrac, LSL #16  @// |xFrac|n_valX |
    MOV     n_valX, n_valX, LSL #3          @// multiply by 8 in advance
    MOV     c32, #32

    @// 2x2 pels per iteration
    @// bilinear vertical interpolation

loop1_y:
    ADD     n_cnt, n_cnt, n_tmp2, LSL #8
    LDRB    n_tmp1, [ptrA, n_w]
    LDRB    n_tmp2, [ptrA], #1

loop1_x:
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp4, [ptrA], #1

    PKHBT   n_tmp5, n_tmp1, n_tmp3, LSL #16
    PKHBT   n_tmp6, n_tmp2, n_tmp4, LSL #16

    LDRB    n_tmp1, [ptrA, n_w]
    LDRB    n_tmp2, [ptrA], #1

    SMLAD   n_tmp5, n_tmp5, n_valX, c32       @// multiply
    SMLAD   n_tmp6, n_tmp6, n_valX, c32       @// multiply

    PKHBT   n_tmp7, n_tmp3, n_tmp1, LSL #16
    PKHBT   n_tmp8, n_tmp4, n_tmp2, LSL #16

    SMLAD   n_tmp7, n_tmp7, n_valX, c32       @// multiply
    SMLAD   n_tmp8, n_tmp8, n_valX, c32       @// multiply

    LDR     n_tmp3, [sp, #pDstStride]
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    STRB    n_tmp5, [mb,n_tmp3]               @// store row 2 col 1

    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down
    STRB    n_tmp6, [mb],#1               @// store row 1 col 1

    MOV     n_tmp7, n_tmp7, LSR #6          @// scale down
    STRB    n_tmp7, [mb,n_tmp3]               @// store row 2 col 2

    MOV     n_tmp8, n_tmp8, LSR #6          @// scale down
    STRB    n_tmp8, [mb],#1               @// store row 1 col 2

    SUBS    n_cnt, n_cnt, #2<<28
    BCS     loop1_x

    AND     n_tmp2, n_cnt, #0x00F00000

    LDR     n_tmp8, [sp, #pDstStride]
    ADDS    mb, mb, n_tmp8, LSL #1
    SBC     mb, mb, n_tmp2, LSR #20
    ADD     ptrA, ptrA, n_w, LSL #1
    SBC     ptrA, ptrA, n_tmp2, LSR #20
    SUB     ptrA, ptrA, #1

    ADDS    n_cnt, n_cnt, #0xE << 24
    BGE     loop1_y

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11,pc}

@extern void add_chroma_X0_ARMV6(VO_U8 *p_Src, int src_stride ,VO_U8 *p_Dst,VO_S32 dst_stride, int w00, int w01, int w10, int n_partW, int n_partH)
add_chroma_X0_ARMV6:
    STMFD   sp!, {r0-r11,lr}

    LDR     chrPW, [sp, #pPartW]     @// chromaPartWidth
    LDR     n_w, [sp, #pSrcStride]      @// n_w
    LDR     chrPH, [sp, #pPartH]     @// chromaPartHeight
@   MLA     n_tmp3, y0, n_w, n_x0     ;// n_tmp3 = y0*n_w+n_x0
    LDR     xFrac, [sp, #pw00]     @// xFrac
@   ADD     ptrA, p_ref, n_tmp3         ;// ptrA = p_ref + y0*n_w+n_x0
    RSB     n_valX, xFrac, #8         @// n_valX = 8-xFrac
@    LDR     n_valX, [sp, #pw00]

@   LDR     mb, [sp, #0xc8]         ;// predPartChroma

    @// pack values to n_cnt register
    @// [31:28] loop_x (chromaPartWidth-1)
    @// [27:24] loop_y (chromaPartHeight-1)
    @// [23:20] chromaPartWidth-1
    @// [19:16] chromaPartHeight-1
    @// [15:00] nothing

    SUB     n_tmp2, chrPH, #1             @// chromaPartHeight-1
    SUB     n_tmp1, chrPW, #1             @// chromaPartWidth-1
    ADD     n_cnt, n_cnt, n_tmp2, LSL #16 @// chromaPartHeight-1
    ADD     n_cnt, n_cnt, n_tmp2, LSL #24 @// loop_y
    ADD     n_cnt, n_cnt, n_tmp1, LSL #20 @// chromaPartWidth-1
    AND     n_tmp2, n_cnt, #0x00F00000    @// loop_x
    PKHBT   n_valX, n_valX, xFrac, LSL #16  @// |xFrac|n_valX |
    MOV     n_valX, n_valX, LSL #3          @// multiply by 8 in advance
    MOV     c32, #32

    @// 2x2 pels per iteration
    @// bilinear vertical interpolation

loop2_y:
    ADD     n_cnt, n_cnt, n_tmp2, LSL #8
    LDRB    n_tmp1, [ptrA, n_w]
    LDRB    n_tmp2, [ptrA], #1

loop2_x:
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp4, [ptrA], #1

    PKHBT   n_tmp5, n_tmp1, n_tmp3, LSL #16
    PKHBT   n_tmp6, n_tmp2, n_tmp4, LSL #16

    LDRB    n_tmp1, [ptrA, n_w]
    LDRB    n_tmp2, [ptrA], #1

    SMLAD   n_tmp5, n_tmp5, n_valX, c32       @// multiply
    SMLAD   n_tmp6, n_tmp6, n_valX, c32       @// multiply

    PKHBT   n_tmp7, n_tmp3, n_tmp1, LSL #16
    PKHBT   n_tmp8, n_tmp4, n_tmp2, LSL #16

    SMLAD   n_tmp7, n_tmp7, n_valX, c32       @// multiply
    SMLAD   n_tmp8, n_tmp8, n_valX, c32       @// multiply

    LDR     n_tmp3, [sp, #pDstStride]
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    LDRB    n_tmp4, [mb,n_tmp3]
    UHSUB8  n_tmp4, n_tmp5, n_tmp4
    USUB8   n_tmp5, n_tmp5, n_tmp4
    STRB    n_tmp5, [mb,n_tmp3]               @// store row 2 col 1

    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down
    LDRB    n_tmp4, [mb]
    UHSUB8  n_tmp4, n_tmp6, n_tmp4
    USUB8   n_tmp6, n_tmp6, n_tmp4
    STRB    n_tmp6, [mb],#1               @// store row 1 col 1

    MOV     n_tmp7, n_tmp7, LSR #6          @// scale down
    LDRB    n_tmp4, [mb,n_tmp3]
    UHSUB8  n_tmp4, n_tmp7, n_tmp4
    USUB8   n_tmp7, n_tmp7, n_tmp4
    STRB    n_tmp7, [mb,n_tmp3]               @// store row 2 col 2

    MOV     n_tmp8, n_tmp8, LSR #6          @// scale down
    LDRB    n_tmp4, [mb]
    UHSUB8  n_tmp4, n_tmp8, n_tmp4
    USUB8   n_tmp8, n_tmp8, n_tmp4
    STRB    n_tmp8, [mb],#1               @// store row 1 col 2

    SUBS    n_cnt, n_cnt, #2<<28
    BCS     loop2_x

    AND     n_tmp2, n_cnt, #0x00F00000

    LDR     n_tmp8, [sp, #pDstStride]
    ADDS    mb, mb, n_tmp8, LSL #1
    SBC     mb, mb, n_tmp2, LSR #20
    ADD     ptrA, ptrA, n_w, LSL #1
    SBC     ptrA, ptrA, n_tmp2, LSR #20
    SUB     ptrA, ptrA, #1

    ADDS    n_cnt, n_cnt, #0xE << 24
    BGE     loop2_y

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11,pc}
END:
