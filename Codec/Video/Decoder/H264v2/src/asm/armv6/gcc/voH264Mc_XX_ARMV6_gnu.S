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
 n_valY    .req r3

 n_w   .req r4

 n_tmp4    .req r5
 n_h  .req r5

 n_tmp1    .req r6

 n_tmp2    .req r7

 n_tmp3    .req r8

 n_valX    .req r9

 n_tmp5    .req r10
 chrPW   .req r10

 n_tmp6    .req r11
 chrPH   .req r11

 xFrac   .req r12

 c32     .req r14
 yFrac   .req r14

@// function exports and imports

    .global  get_chroma_XX_ARMV6
    .global  add_chroma_XX_ARMV6

@//  Function arguments
@//
@//  u8 *p_ref,                   : 0xc4
@//  u8 *predPartChroma,        : 0xc8
@//  i32 n_x0,                    : 0xcc
@//  i32 y0,                    : 0xd0
@//  u32 n_w,                 : 0xf8
@//  u32 n_h,                : 0xfc
@//  u32 xFrac,                 : 0x100
@//  u32 yFrac,                 : 0x104
@//  u32 chromaPartWidth,       : 0x108
@//  u32 chromaPartHeight       : 0x10c

 pSrcStride    =   0x04
 pDstStride    =   0x0C
 pw00          =   0x34
 pw01          =   0x38
 pw10          =   0x3C
 pw11          =   0x40
 pPartW        =   0x44
 pPartH        =   0x48

@extern void get_chroma_XX_ARMV6(VO_U8 *p_Src, int src_stride ,VO_U8 *p_Dst,VO_S32 dst_stride, int w00, int w01, int w10, int w11, int n_partW, int n_partH)
get_chroma_XX_ARMV6:
    STMFD   sp!, {r0-r11,lr}

    LDR     chrPW, [sp, #pPartW]     @// chromaPartWidth
    LDR     chrPH, [sp, #pPartH]     @// chromaPartWidth
    LDR     n_w, [sp, #pSrcStride]      @// n_w
 
@    MLA     n_tmp3, y0, n_w, n_x0     ;// n_tmp3 = y0*n_w+n_x0
@    LDR     yFrac, [sp, #pw11]       ;// yFrac
@    LDR     xFrac, [sp, #pw10]
    LDR     yFrac, [sp, #pw01]        @// yFrac
    LDR     xFrac, [sp, #pw00]
    
@    ADD     ptrA, p_ref, n_tmp3         ;// ptrA = p_ref + y0*n_w+n_x0
    RSB     n_valX, xFrac, #8         @// n_valX = 8-xFrac
    RSB     n_valY, yFrac, #8         @// n_valY = 8-yFrac

@    LDR     mb, [sp, #0xc8]         ;// predPartChroma


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
    PKHBT   n_valY, n_valY, yFrac, LSL #16  @// |yFrac|n_valY |
    MOV     c32, #32


    @///////////////////////////////////////////////////////////////////////////
    @// Cb
    @///////////////////////////////////////////////////////////////////////////

    @// 2x2 pels per iteration
    @// bilinear vertical and horizontal interpolation

loop1_y:
    LDRB    n_tmp1, [ptrA]
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp5, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #16   @// |n_t3|n_t1|
    PKHBT   n_tmp3, n_tmp3, n_tmp5, LSL #16   @// |n_t5|n_t3|

    SMUAD   n_tmp1, n_tmp1, n_valY            @// n_t1=(n_t1*n_valY + n_t3*yFrac)
    SMUAD   n_tmp3, n_tmp3, n_valY            @// n_t3=(n_t3*n_valY + n_t5*yFrac)

    ADD     n_cnt, n_cnt, n_tmp2, LSL #8
loop1_x:
    @// first
    LDRB    n_tmp2, [ptrA, #1]!
    LDRB    n_tmp4, [ptrA, n_w]
    LDRB    n_tmp6, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #16   @// |n_t4|n_t2|
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #16   @// |n_t6|n_t4|

    SMUAD   n_tmp2, n_tmp2, n_valY            @// n_t2=(n_t2*n_valY + n_t4*yFrac)
    MLA     n_tmp5, n_tmp1, n_valX, c32       @// n_t5=n_t1*n_valX+32
    MLA     n_tmp5, n_tmp2, xFrac, n_tmp5     @// n_t5=n_t2*xFrac+n_t5

    SMUAD   n_tmp4, n_tmp4, n_valY            @// n_t4=(n_t4*n_valY + n_t6*yFrac)
    MLA     n_tmp6, n_tmp3, n_valX, c32       @// n_t3=n_t3*n_valX+32
    MLA     n_tmp6, n_tmp4, xFrac, n_tmp6     @// n_t6=n_t4*xFrac+n_t6

    LDR     n_tmp3, [sp, #pDstStride]
    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down   
    STRB    n_tmp6, [mb, n_tmp3]              @// store pixel
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    STRB    n_tmp5, [mb], #1              @// store pixel

    @// second
    LDRB    n_tmp1, [ptrA, #1]!
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp5, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #16   @// |n_t3|n_t1|
    PKHBT   n_tmp3, n_tmp3, n_tmp5, LSL #16   @// |n_t5|n_t3|

    SMUAD   n_tmp1, n_tmp1, n_valY            @// n_t1=(n_t1*n_valY + n_t3*yFrac)
    MLA     n_tmp5, n_tmp1, xFrac, c32      @// n_t1=n_t1*xFrac+32
    MLA     n_tmp5, n_tmp2, n_valX, n_tmp5      @// n_t5=n_t2*n_valX+n_t5

    SMUAD   n_tmp3, n_tmp3, n_valY            @// n_t3=(n_t3*n_valY + n_t5*yFrac)
    MLA     n_tmp6, n_tmp3, xFrac, c32      @// n_t3=n_t3*xFrac+32
    MLA     n_tmp6, n_tmp4, n_valX, n_tmp6      @// n_t6=n_t4*n_valX+n_t6

    LDR     n_tmp4, [sp, #pDstStride]
    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down
    STRB    n_tmp6, [mb, n_tmp4]              @// store pixel
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    STRB    n_tmp5, [mb], #1              @// store pixel

    SUBS    n_cnt, n_cnt, #2<<28
    BCS     loop1_x

    AND     n_tmp2, n_cnt, #0x00F00000

    LDR     n_tmp3, [sp, #pDstStride]
    ADDS    mb, mb, n_tmp3, LSL #1
    SBC     mb, mb, n_tmp2, LSR #20
    ADD     ptrA, ptrA, n_w, LSL #1
    SBC     ptrA, ptrA, n_tmp2, LSR #20

    ADDS    n_cnt, n_cnt, #0xE << 24
    BGE     loop1_y

    ADD     sp,sp,#0x10
    LDMFD   sp!,{r4-r11,pc}

@extern void add_chroma_XX_ARMV6(VO_U8 *p_Src, int src_stride ,VO_U8 *p_Dst,VO_S32 dst_stride, int w00, int w01, int w10, int w11, int n_partW, int n_partH)
add_chroma_XX_ARMV6:
    STMFD   sp!, {r0-r11,lr}

    LDR     chrPW, [sp, #pPartW]     @// chromaPartWidth
    LDR     chrPH, [sp, #pPartH]     @// chromaPartWidth
    LDR     n_w, [sp, #pSrcStride]      @// n_w
 
@    MLA     n_tmp3, y0, n_w, n_x0     ;// n_tmp3 = y0*n_w+n_x0
@    LDR     yFrac, [sp, #pw11]       ;// yFrac
@    LDR     xFrac, [sp, #pw10]
    LDR     yFrac, [sp, #pw01]        @// yFrac
    LDR     xFrac, [sp, #pw00]
    
@    ADD     ptrA, p_ref, n_tmp3         ;// ptrA = p_ref + y0*n_w+n_x0
    RSB     n_valX, xFrac, #8         @// n_valX = 8-xFrac
    RSB     n_valY, yFrac, #8         @// n_valY = 8-yFrac

@    LDR     mb, [sp, #0xc8]         ;// predPartChroma


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
    PKHBT   n_valY, n_valY, yFrac, LSL #16  @// |yFrac|n_valY |
    MOV     c32, #32


    @///////////////////////////////////////////////////////////////////////////
    @// Cb
    @///////////////////////////////////////////////////////////////////////////

    @// 2x2 pels per iteration
    @// bilinear vertical and horizontal interpolation

loop2_y:
    LDRB    n_tmp1, [ptrA]
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp5, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #16   @// |n_t3|n_t1|
    PKHBT   n_tmp3, n_tmp3, n_tmp5, LSL #16   @// |n_t5|n_t3|

    SMUAD   n_tmp1, n_tmp1, n_valY            @// n_t1=(n_t1*n_valY + n_t3*yFrac)
    SMUAD   n_tmp3, n_tmp3, n_valY            @// n_t3=(n_t3*n_valY + n_t5*yFrac)

    ADD     n_cnt, n_cnt, n_tmp2, LSL #8
loop2_x:
    @// first
    LDRB    n_tmp2, [ptrA, #1]!
    LDRB    n_tmp4, [ptrA, n_w]
    LDRB    n_tmp6, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #16   @// |n_t4|n_t2|
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #16   @// |n_t6|n_t4|

    SMUAD   n_tmp2, n_tmp2, n_valY            @// n_t2=(n_t2*n_valY + n_t4*yFrac)
    MLA     n_tmp5, n_tmp1, n_valX, c32       @// n_t5=n_t1*n_valX+32
    MLA     n_tmp5, n_tmp2, xFrac, n_tmp5     @// n_t5=n_t2*xFrac+n_t5

    SMUAD   n_tmp4, n_tmp4, n_valY            @// n_t4=(n_t4*n_valY + n_t6*yFrac)
    MLA     n_tmp6, n_tmp3, n_valX, c32       @// n_t3=n_t3*n_valX+32
    MLA     n_tmp6, n_tmp4, xFrac, n_tmp6     @// n_t6=n_t4*xFrac+n_t6

    LDR     n_tmp3, [sp, #pDstStride]
    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down   
    LDRB    n_tmp1, [mb, n_tmp3]
    UHSUB8  n_tmp1, n_tmp6, n_tmp1
    USUB8   n_tmp6, n_tmp6, n_tmp1
    STRB    n_tmp6, [mb, n_tmp3]              @// store pixel
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    LDRB    n_tmp1, [mb]
    UHSUB8  n_tmp1, n_tmp5, n_tmp1
    USUB8   n_tmp5, n_tmp5, n_tmp1    
    STRB    n_tmp5, [mb], #1              @// store pixel

    @// second
    LDRB    n_tmp1, [ptrA, #1]!
    LDRB    n_tmp3, [ptrA, n_w]
    LDRB    n_tmp5, [ptrA, n_w, LSL #1]

    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #16   @// |n_t3|n_t1|
    PKHBT   n_tmp3, n_tmp3, n_tmp5, LSL #16   @// |n_t5|n_t3|

    SMUAD   n_tmp1, n_tmp1, n_valY            @// n_t1=(n_t1*n_valY + n_t3*yFrac)
    MLA     n_tmp5, n_tmp1, xFrac, c32      @// n_t1=n_t1*xFrac+32
    MLA     n_tmp5, n_tmp2, n_valX, n_tmp5      @// n_t5=n_t2*n_valX+n_t5

    SMUAD   n_tmp3, n_tmp3, n_valY            @// n_t3=(n_t3*n_valY + n_t5*yFrac)
    MLA     n_tmp6, n_tmp3, xFrac, c32      @// n_t3=n_t3*xFrac+32
    MLA     n_tmp6, n_tmp4, n_valX, n_tmp6      @// n_t6=n_t4*n_valX+n_t6

    LDR     n_tmp4, [sp, #pDstStride]
    MOV     n_tmp6, n_tmp6, LSR #6          @// scale down
    LDRB    n_tmp2, [mb, n_tmp4]
    UHSUB8  n_tmp2, n_tmp6, n_tmp2
    USUB8   n_tmp6, n_tmp6, n_tmp2
    STRB    n_tmp6, [mb, n_tmp4]              @// store pixel
    MOV     n_tmp5, n_tmp5, LSR #6          @// scale down
    LDRB    n_tmp2, [mb]
    UHSUB8  n_tmp2, n_tmp5, n_tmp2
    USUB8   n_tmp5, n_tmp5, n_tmp2    
    STRB    n_tmp5, [mb], #1              @// store pixel

    SUBS    n_cnt, n_cnt, #2<<28
    BCS     loop2_x

    AND     n_tmp2, n_cnt, #0x00F00000

    LDR     n_tmp3, [sp, #pDstStride]
    ADDS    mb, mb, n_tmp3, LSL #1
    SBC     mb, mb, n_tmp2, LSR #20
    ADD     ptrA, ptrA, n_w, LSL #1
    SBC     ptrA, ptrA, n_tmp2, LSR #20

    ADDS    n_cnt, n_cnt, #0xE << 24
    BGE     loop2_y

    ADD     sp,sp,#0x10
    LDMFD   sp!,{r4-r11,pc}    
    @.end

