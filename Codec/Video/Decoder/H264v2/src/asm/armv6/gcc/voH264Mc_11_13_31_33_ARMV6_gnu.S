@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

    .global  get_luma_11_13_31_33_ARMV6
    .global  add_luma_11_13_31_33_ARMV6

	.align 8

 p_Src     .req r0

 p_Dst      .req r2
 buff    .req r2

 n_cnt   .req r1
 n_deltaX  .req r1

 n_deltaY  .req r3
 n_x20   .req r3
 res     .req r3

 n_x31   .req r4
 n_tmp1    .req r4

 n_h  .req r5
 n_x64   .req r5
 n_tmp2    .req r5

 n_partW   .req r6
 n_x75   .req r6
 n_tmp3    .req r6

 n_partH   .req r7
 n_tmp4    .req r7

 n_tmp5    .req r8

 n_tmp6    .req r9

 tmpa    .req r10

 n_mul20_01  .req r11
 tmpb        .req r11

 n_mul20_m5  .req r12
 n_w       .req r12

 plus16  .req r14

 pInterBuf  =    0x00
 pPartW     =    0x34
 pPartH     =    0x38
 pDeltaX    =    0x3C
 pDeltaY    =    0x40
 pHorOffset =    0x44
 ppBuff     =    0x48


@extern get_luma_11_13_31_33_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset)	
get_luma_11_13_31_33_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     n_w, r1                @// n_w
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    LDR     n_tmp6, [sp,#68]       @// horVerOffset
    MLA     n_tmp5, n_w, n_deltaY, n_deltaX     @// n_deltaY*n_w+n_deltaX
    ADD     p_Src, p_Src, n_tmp5          @// p_Src += n_deltaY*n_w+n_deltaX
    STR     p_Src, [sp, #0]       @// store "p_ref" for vertical filtering
    AND     n_tmp6, n_tmp6, #2          @// calculate p_ref for horizontal filter
    MOV     tmpa, #2
    ADD     n_tmp6, tmpa, n_tmp6, LSR #1
    MLA     p_Src, n_tmp6, n_w, p_Src
    ADD     p_Src, p_Src, #8             @// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partWidth
    LDR     n_partH, [sp,#56]       @// partHeight

    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     n_cnt, n_w
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_tmp5, n_partH, n_partW, LSL #4
    ADD     n_cnt, n_cnt, n_tmp5, LSL #16


    LDR     n_mul20_01, = 0x00140001    @// constant multipliers
    LDR     n_mul20_m5, = 0x0014FFFB    @// constant multipliers
    MOV     plus16, #16                 @// constant for add
    AND     n_tmp4, n_cnt, #0x000F0000    @// partHeight-1
    AND     n_tmp6, n_cnt, #0x00F00000    @// partWidth-1
    ADD     n_cnt, n_cnt, n_tmp4, LSL #8  @// n_partH-1 to lower part of top byte

@// HORIZONTAL PART

get_loop_y_hor:
    LDR     n_x31, [p_Src, #-8]
    ADD     n_cnt, n_cnt, n_tmp6, LSL #8   @// n_partW-1 to upper part of top byte
    LDR     n_x75, [p_Src, #-4]
    UXTB16  n_x20, n_x31
    UXTB16  n_x31, n_x31, ROR #8
    UXTB16  n_x64, n_x75

get_loop_x_hor:
    UXTB16  n_x75, n_x75, ROR #8

    SMLAD   n_tmp4, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp6, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp5, n_x20, n_mul20_m5, plus16
    SMLATB  tmpa, n_x31, n_mul20_01, plus16

    SMLAD   n_tmp4, n_x31, n_mul20_m5, n_tmp4
    SMLATB  n_tmp6, n_x31, n_mul20_m5, n_tmp6
    SMLAD   n_tmp5, n_x31, n_mul20_01, n_tmp5
    LDR     n_x31, [p_Src], #4
    SMLAD   tmpa, n_x64, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x64, n_mul20_m5, n_tmp4
    SMLADX  n_tmp6, n_x64, n_mul20_m5, n_tmp6
    SMLADX  n_tmp5, n_x64, n_mul20_01, n_tmp5
    SMLADX  tmpa, n_x75, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x75, n_mul20_01, n_tmp4
    UXTB16  n_x20, n_x31
    SMLABB  n_tmp5, n_x75, n_mul20_m5, n_tmp5
    SMLADX  n_tmp6, n_x75, n_mul20_01, n_tmp6
    SMLABB  tmpa, n_x20, n_mul20_01, tmpa

    MOV     n_tmp5, n_tmp5, ASR #5
    MOV     n_tmp4, n_tmp4, ASR #5
    PKHBT   n_tmp5, n_tmp5, tmpa, LSL #(16-5)
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #(16-5)
    USAT16  n_tmp5, #8, n_tmp5
    USAT16  n_tmp4, #8, n_tmp4

    SUBS    n_cnt, n_cnt, #4<<28
    ORR     n_tmp4, n_tmp4, n_tmp5, LSL #8
    STR     n_tmp4, [p_Dst], #4
    BCC     get_next_y_hor

    UXTB16  n_x31, n_x31, ROR #8

    SMLAD   n_tmp4, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp6, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp5, n_x64, n_mul20_m5, plus16
    SMLATB  tmpa, n_x75, n_mul20_01, plus16

    SMLAD   n_tmp4, n_x75, n_mul20_m5, n_tmp4
    SMLATB  n_tmp6, n_x75, n_mul20_m5, n_tmp6
    SMLAD   n_tmp5, n_x75, n_mul20_01, n_tmp5
    LDR     n_x75, [p_Src], #4
    SMLAD   tmpa, n_x20, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x20, n_mul20_m5, n_tmp4
    SMLADX  n_tmp6, n_x20, n_mul20_m5, n_tmp6
    SMLADX  n_tmp5, n_x20, n_mul20_01, n_tmp5
    SMLADX  tmpa, n_x31, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x31, n_mul20_01, n_tmp4
    UXTB16  n_x64, n_x75
    SMLABB  n_tmp5, n_x31, n_mul20_m5, n_tmp5
    SMLADX  n_tmp6, n_x31, n_mul20_01, n_tmp6
    SMLABB  tmpa, n_x64, n_mul20_01, tmpa

    MOV     n_tmp5, n_tmp5, ASR #5
    MOV     n_tmp4, n_tmp4, ASR #5
    PKHBT   n_tmp5, n_tmp5, tmpa, LSL #(16-5)
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #(16-5)
    USAT16  n_tmp5, #8, n_tmp5
    USAT16  n_tmp4, #8, n_tmp4

    SUBS    n_cnt, n_cnt, #4<<28
    ORR     n_tmp4, n_tmp4, n_tmp5, LSL #8
    STR     n_tmp4, [p_Dst], #4
    BCS     get_loop_x_hor

get_next_y_hor:
    AND     n_tmp6, n_cnt, #0x00F00000        @// partWidth-1
    SMLABB  p_Src, n_cnt, n_mul20_01, p_Src     @// +n_w
    LDR     n_tmp4, [sp,#12]                  @ //dst_stride
    ADDS    p_Dst, p_Dst, n_tmp4                    @// +16, Carry=0
    SBC     p_Dst, p_Dst, n_tmp6, LSR #20           @// -(partWidth-1)-1
    SBC     p_Src, p_Src, n_tmp6, LSR #20         @// -(partWidth-1)-1
    ADDS    n_cnt, n_cnt, #(1<<28)-(1<<24)  @// decrement counter (n_partW)
    BGE     get_loop_y_hor



@// VERTICAL PART
@//
@// Approach to vertical interpolation
@//
@// Interpolation is done by using 32-bit loads and stores
@// and by using 16 bit arithmetic. 4x4 block is processed
@// in each round.
@//
@// |a_11|a_11|a_11|a_11|...|a_1n|a_1n|a_1n|a_1n|
@// |b_11|b_11|b_11|b_11|...|b_1n|b_1n|b_1n|b_1n|
@// |c_11|c_11|c_11|c_11|...|c_1n|c_1n|c_1n|c_1n|
@// |d_11|d_11|d_11|d_11|...|d_1n|d_1n|d_1n|d_1n|
@//           ..
@//           ..
@// |a_m1|a_m1|a_m1|a_m1|...
@// |b_m1|b_m1|b_m1|b_m1|...
@// |c_m1|c_m1|c_m1|c_m1|...
@// |d_m1|d_m1|d_m1|d_m1|...

@// Approach to bilinear interpolation to quarter pel position.
@// 4 bytes are processed parallel
@//
@// algorithm (a+b+1)/2. Rouding upwards +1 can be achieved by 
@// negating second operand to get one's complement (instead of 2's)
@// and using subtraction, EOR is used to correct sign.
@//
@// MVN     b, b
@// UHSUB8  a, a, b
@// EOR     a, a, 0x80808080


    LDR     p_Src, [sp, #0]           @// p_Src
    LDR     tmpa, [sp, #68]          @// horVerOffset
    LDR     p_Dst, [sp, #8]            @// p_Dst
    LDR     n_w, [sp, #4]         @// n_w
    ADD     p_Src, p_Src, #2                @// calculate correct position
    AND     tmpa, tmpa, #1
    ADD     p_Src, p_Src, tmpa
    LDR     plus16, = 0x00100010        @// +16 to lower and upperf halfwords
    AND     n_cnt, n_cnt, #0x00FFFFFF   @// partWidth-1

    AND     tmpa, n_cnt, #0x000F0000    @// partHeight-1
    ADD     n_cnt, n_cnt, tmpa, LSL #8

get_loop_y:
    ADD     n_cnt, n_cnt, n_tmp6, LSL #8  @// partWidth-1

get_loop_x:
    LDR     n_tmp1, [p_Src], n_w     @// |a4|a3|a2|a1|
    LDR     n_tmp2, [p_Src], n_w     @// |c4|c3|c2|c1|
    LDR     n_tmp3, [p_Src], n_w     @// |g4|g3|g2|g1|
    LDR     n_tmp4, [p_Src], n_w     @// |m4|m3|m2|m1|
    LDR     n_tmp5, [p_Src], n_w     @// |r4|r3|r2|r1|
    LDR     n_tmp6, [p_Src], n_w     @// |n_t4|n_t3|n_t2|n_t1|

    @// first four pixels 
    UXTB16  tmpa, n_tmp3                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp4            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp2                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)

    UXTAB16 tmpb, tmpb, n_tmp5            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp1            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp6            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp3, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp4, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp2, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp5, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp6, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     n_tmp1, [p_Dst]
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp1, n_tmp1
    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divede by 32
    ORR     res, res, tmpa

    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp1              @// bilinear interpolation
    LDR     n_tmp1, [p_Src], n_w          @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa              @// next row (p_Dst)


    @// n_tmp2 = |a4|a3|a2|a1|
    @// n_tmp3 = |c4|c3|c2|c1|
    @// n_tmp4 = |g4|g3|g2|g1|
    @// n_tmp5 = |m4|m3|m2|m1|
    @// n_tmp6 = |r4|r3|r2|r1|
    @// n_tmp1 = |n_t4|n_t3|n_t2|n_t1|

    @// second four pixels
    UXTB16  tmpa, n_tmp4                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp5            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp3                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp6            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp2            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp1            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp4, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp5, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp3, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp6, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp2, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     n_tmp2, [p_Dst]
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp2, n_tmp2

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp2              @// bilinear interpolation
    LDR     n_tmp2, [p_Src], n_w          @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa              @// next row

    @// n_tmp3 = |a4|a3|a2|a1|
    @// n_tmp4 = |c4|c3|c2|c1|
    @// n_tmp5 = |g4|g3|g2|g1|
    @// n_tmp6 = |m4|m3|m2|m1|
    @// n_tmp1 = |r4|r3|r2|r1|
    @// n_tmp2 = |n_t4|n_t3|n_t2|n_t1|

    @// third four pixels
    UXTB16  tmpa, n_tmp5                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp6            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp4                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp1            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp3            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp2            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp5, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp6, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp4, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp1, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp3, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp2, ROR #8    @// 16+20(G+M)+A+T


    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     n_tmp3, [p_Dst]
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp3, n_tmp3

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp3              @// bilinear interpolation
    LDR     n_tmp3, [p_Src]                 @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa              @// next row

    @// n_tmp4 = |a4|a3|a2|a1|
    @// n_tmp5 = |c4|c3|c2|c1|
    @// n_tmp6 = |g4|g3|g2|g1|
    @// n_tmp1 = |m4|m3|m2|m1|
    @// n_tmp2 = |r4|r3|r2|r1|
    @// n_tmp3 = |n_t4|n_t3|n_t2|n_t1|

    @// fourth four pixels
    UXTB16  tmpa, n_tmp6                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp1            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp5                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp2            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp4            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp3            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp6, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp5, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp2, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp4, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp3, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     n_tmp5, [p_Dst]
    LDR     n_tmp4, = 0xFF00FF00
    MVN     n_tmp5, n_tmp5

    AND     tmpa, n_tmp4, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp5              @// bilinear interpolation

    @// decrement loop_x counter
    SUBS    n_cnt, n_cnt, #4<<28        @// decrement x loop counter

    @// calculate "p_Src" address for next round
    SUB     p_Src, p_Src, n_w, LSL #3     @// p_Src -= 8*n_w;
    ADD     p_Src, p_Src, #4                @// next column (4 pixels)

    EOR     res, res, tmpa              @// correct sign
    
    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa
    SUB     p_Dst, tmpa, lsl #2
    ADD     p_Dst, p_Dst, #4


    BCS     get_loop_x

    ADDS    p_Dst, p_Dst, tmpa, lsl #2@     ;// set Carry=0
    ADD     p_Src, p_Src, n_w, LSL #2     @// p_Src += 4*n_w
    AND     n_tmp6, n_cnt, #0x00F00000    @// partWidth-1
    SBC     p_Src, p_Src, n_tmp6, LSR #20     @// -(partWidth-1)-1
    SBC     p_Dst, p_Dst, n_tmp6, LSR #20       @// -(partWidth-1)-1

    ADDS    n_cnt, n_cnt, #0xC << 24    @// decrement y loop counter
    BGE     get_loop_y

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}   


@extern add_luma_11_13_31_33_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset, VO_S8 *pBuff)	
add_luma_11_13_31_33_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     n_w, r1                @// n_w
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    LDR     n_tmp6, [sp,#68]       @// horVerOffset
    MLA     n_tmp5, n_w, n_deltaY, n_deltaX     @// n_deltaY*n_w+n_deltaX
    ADD     p_Src, p_Src, n_tmp5          @// p_Src += n_deltaY*n_w+n_deltaX
    STR     p_Src, [sp, #0]       @// store "p_ref" for vertical filtering
    AND     n_tmp6, n_tmp6, #2          @// calculate p_ref for horizontal filter
    MOV     tmpa, #2
    ADD     n_tmp6, tmpa, n_tmp6, LSR #1
    MLA     p_Src, n_tmp6, n_w, p_Src
    ADD     p_Src, p_Src, #8             @// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partWidth
    LDR     n_partH, [sp,#56]       @// partHeight

    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     n_cnt, n_w
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_tmp5, n_partH, n_partW, LSL #4
    ADD     n_cnt, n_cnt, n_tmp5, LSL #16


    LDR     n_mul20_01, = 0x00140001    @// constant multipliers
    LDR     n_mul20_m5, = 0x0014FFFB    @// constant multipliers
    MOV     plus16, #16                 @// constant for add
    AND     n_tmp4, n_cnt, #0x000F0000    @// partHeight-1
    AND     n_tmp6, n_cnt, #0x00F00000    @// partWidth-1
    ADD     n_cnt, n_cnt, n_tmp4, LSL #8  @// n_partH-1 to lower part of top byte

    LDR     p_Dst, [sp, #ppBuff]

@// HORIZONTAL PART

add_loop_y_hor:
    LDR     n_x31, [p_Src, #-8]
    ADD     n_cnt, n_cnt, n_tmp6, LSL #8   @// n_partW-1 to upper part of top byte
    LDR     n_x75, [p_Src, #-4]
    UXTB16  n_x20, n_x31
    UXTB16  n_x31, n_x31, ROR #8
    UXTB16  n_x64, n_x75

add_loop_x_hor:
    UXTB16  n_x75, n_x75, ROR #8

    SMLAD   n_tmp4, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp6, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp5, n_x20, n_mul20_m5, plus16
    SMLATB  tmpa, n_x31, n_mul20_01, plus16

    SMLAD   n_tmp4, n_x31, n_mul20_m5, n_tmp4
    SMLATB  n_tmp6, n_x31, n_mul20_m5, n_tmp6
    SMLAD   n_tmp5, n_x31, n_mul20_01, n_tmp5
    LDR     n_x31, [p_Src], #4
    SMLAD   tmpa, n_x64, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x64, n_mul20_m5, n_tmp4
    SMLADX  n_tmp6, n_x64, n_mul20_m5, n_tmp6
    SMLADX  n_tmp5, n_x64, n_mul20_01, n_tmp5
    SMLADX  tmpa, n_x75, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x75, n_mul20_01, n_tmp4
    UXTB16  n_x20, n_x31
    SMLABB  n_tmp5, n_x75, n_mul20_m5, n_tmp5
    SMLADX  n_tmp6, n_x75, n_mul20_01, n_tmp6
    SMLABB  tmpa, n_x20, n_mul20_01, tmpa

    MOV     n_tmp5, n_tmp5, ASR #5
    MOV     n_tmp4, n_tmp4, ASR #5
    PKHBT   n_tmp5, n_tmp5, tmpa, LSL #(16-5)
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #(16-5)
    USAT16  n_tmp5, #8, n_tmp5
    USAT16  n_tmp4, #8, n_tmp4

    SUBS    n_cnt, n_cnt, #4<<28
    ORR     n_tmp4, n_tmp4, n_tmp5, LSL #8
    STR     n_tmp4, [p_Dst], #4
    BCC     add_next_y_hor

    UXTB16  n_x31, n_x31, ROR #8

    SMLAD   n_tmp4, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp6, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp5, n_x64, n_mul20_m5, plus16
    SMLATB  tmpa, n_x75, n_mul20_01, plus16

    SMLAD   n_tmp4, n_x75, n_mul20_m5, n_tmp4
    SMLATB  n_tmp6, n_x75, n_mul20_m5, n_tmp6
    SMLAD   n_tmp5, n_x75, n_mul20_01, n_tmp5
    LDR     n_x75, [p_Src], #4
    SMLAD   tmpa, n_x20, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x20, n_mul20_m5, n_tmp4
    SMLADX  n_tmp6, n_x20, n_mul20_m5, n_tmp6
    SMLADX  n_tmp5, n_x20, n_mul20_01, n_tmp5
    SMLADX  tmpa, n_x31, n_mul20_m5, tmpa

    SMLABB  n_tmp4, n_x31, n_mul20_01, n_tmp4
    UXTB16  n_x64, n_x75
    SMLABB  n_tmp5, n_x31, n_mul20_m5, n_tmp5
    SMLADX  n_tmp6, n_x31, n_mul20_01, n_tmp6
    SMLABB  tmpa, n_x64, n_mul20_01, tmpa

    MOV     n_tmp5, n_tmp5, ASR #5
    MOV     n_tmp4, n_tmp4, ASR #5
    PKHBT   n_tmp5, n_tmp5, tmpa, LSL #(16-5)
    PKHBT   n_tmp4, n_tmp4, n_tmp6, LSL #(16-5)
    USAT16  n_tmp5, #8, n_tmp5
    USAT16  n_tmp4, #8, n_tmp4

    SUBS    n_cnt, n_cnt, #4<<28
    ORR     n_tmp4, n_tmp4, n_tmp5, LSL #8
    STR     n_tmp4, [p_Dst], #4
    BCS     add_loop_x_hor

add_next_y_hor:
    AND     n_tmp6, n_cnt, #0x00F00000        @// partWidth-1
    SMLABB  p_Src, n_cnt, n_mul20_01, p_Src     @// +n_w
@    LDR     n_tmp4, [sp,#12]                  ; //dst_stride
@    ADDS    p_Dst, p_Dst, n_tmp4                    ;// +16, Carry=0
    ADDS    p_Dst, p_Dst, #16                    @// +16, Carry=0
    SBC     p_Dst, p_Dst, n_tmp6, LSR #20           @// -(partWidth-1)-1
    SBC     p_Src, p_Src, n_tmp6, LSR #20         @// -(partWidth-1)-1
    ADDS    n_cnt, n_cnt, #(1<<28)-(1<<24)  @// decrement counter (n_partW)
    BGE     add_loop_y_hor



@// VERTICAL PART
@//
@// Approach to vertical interpolation
@//
@// Interpolation is done by using 32-bit loads and stores
@// and by using 16 bit arithmetic. 4x4 block is processed
@// in each round.
@//
@// |a_11|a_11|a_11|a_11|...|a_1n|a_1n|a_1n|a_1n|
@// |b_11|b_11|b_11|b_11|...|b_1n|b_1n|b_1n|b_1n|
@// |c_11|c_11|c_11|c_11|...|c_1n|c_1n|c_1n|c_1n|
@// |d_11|d_11|d_11|d_11|...|d_1n|d_1n|d_1n|d_1n|
@//           ..
@//           ..
@// |a_m1|a_m1|a_m1|a_m1|...
@// |b_m1|b_m1|b_m1|b_m1|...
@// |c_m1|c_m1|c_m1|c_m1|...
@// |d_m1|d_m1|d_m1|d_m1|...

@// Approach to bilinear interpolation to quarter pel position.
@// 4 bytes are processed parallel
@//
@// algorithm (a+b+1)/2. Rouding upwards +1 can be achieved by 
@// negating second operand to get one's complement (instead of 2's)
@// and using subtraction, EOR is used to correct sign.
@//
@// MVN     b, b
@// UHSUB8  a, a, b
@// EOR     a, a, 0x80808080


    LDR     p_Src, [sp, #0]           @// p_Src
    LDR     tmpa, [sp, #68]          @// horVerOffset
    LDR     p_Dst, [sp, #8]            @// p_Dst
    LDR     n_w, [sp, #4]         @// n_w
    ADD     p_Src, p_Src, #2                @// calculate correct position
    AND     tmpa, tmpa, #1
    ADD     p_Src, p_Src, tmpa
    LDR     plus16, = 0x00100010        @// +16 to lower and upperf halfwords
    AND     n_cnt, n_cnt, #0x00FFFFFF   @// partWidth-1

    AND     tmpa, n_cnt, #0x000F0000    @// partHeight-1
    ADD     n_cnt, n_cnt, tmpa, LSL #8

add_loop_y:
    ADD     n_cnt, n_cnt, n_tmp6, LSL #8  @// partWidth-1

add_loop_x:
    LDR     n_tmp1, [p_Src], n_w     @// |a4|a3|a2|a1|
    LDR     n_tmp2, [p_Src], n_w     @// |c4|c3|c2|c1|
    LDR     n_tmp3, [p_Src], n_w     @// |g4|g3|g2|g1|
    LDR     n_tmp4, [p_Src], n_w     @// |m4|m3|m2|m1|
    LDR     n_tmp5, [p_Src], n_w     @// |r4|r3|r2|r1|
    LDR     n_tmp6, [p_Src], n_w     @// |n_t4|n_t3|n_t2|n_t1|

    @// first four pixels 
    UXTB16  tmpa, n_tmp3                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp4            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp2                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)

    UXTAB16 tmpb, tmpb, n_tmp5            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp1            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp6            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp3, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp4, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp2, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp5, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp6, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     tmpa, [sp, #ppBuff]
    LDR     n_tmp1, [tmpa], #16
    STR     tmpa, [sp, #ppBuff]
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp1, n_tmp1
    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divede by 32
    ORR     res, res, tmpa

    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp1              @// bilinear interpolation
    LDR     n_tmp1, [p_Src], n_w          @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb    
    STR     res, [p_Dst], tmpa              @// next row (p_Dst)
    


    @// n_tmp2 = |a4|a3|a2|a1|
    @// n_tmp3 = |c4|c3|c2|c1|
    @// n_tmp4 = |g4|g3|g2|g1|
    @// n_tmp5 = |m4|m3|m2|m1|
    @// n_tmp6 = |r4|r3|r2|r1|
    @// n_tmp1 = |n_t4|n_t3|n_t2|n_t1|

    @// second four pixels
    UXTB16  tmpa, n_tmp4                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp5            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp3                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp6            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp2            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp1            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp4, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp5, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp3, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp6, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp2, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     tmpa, [sp, #ppBuff]    
    LDR     n_tmp2, [tmpa], #16
    STR     tmpa, [sp, #ppBuff]
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp2, n_tmp2

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp2              @// bilinear interpolation
    LDR     n_tmp2, [p_Src], n_w          @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb
    STR     res, [p_Dst], tmpa              @// next row

    @// n_tmp3 = |a4|a3|a2|a1|
    @// n_tmp4 = |c4|c3|c2|c1|
    @// n_tmp5 = |g4|g3|g2|g1|
    @// n_tmp6 = |m4|m3|m2|m1|
    @// n_tmp1 = |r4|r3|r2|r1|
    @// n_tmp2 = |n_t4|n_t3|n_t2|n_t1|

    @// third four pixels
    UXTB16  tmpa, n_tmp5                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp6            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp4                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp1            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp3            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp2            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp5, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp6, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp4, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp1, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp3, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp2, ROR #8    @// 16+20(G+M)+A+T


    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     tmpa, [sp, #ppBuff] 
    LDR     n_tmp3, [tmpa], #16
    STR     tmpa, [sp, #ppBuff] 
    LDR     tmpa, = 0xFF00FF00
    MVN     n_tmp3, n_tmp3

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp3              @// bilinear interpolation
    LDR     n_tmp3, [p_Src]                 @// load next row
    EOR     res, res, tmpa              @// correct sign

    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb    
    STR     res, [p_Dst], tmpa              @// next row

    @// n_tmp4 = |a4|a3|a2|a1|
    @// n_tmp5 = |c4|c3|c2|c1|
    @// n_tmp6 = |g4|g3|g2|g1|
    @// n_tmp1 = |m4|m3|m2|m1|
    @// n_tmp2 = |r4|r3|r2|r1|
    @// n_tmp3 = |n_t4|n_t3|n_t2|n_t1|

    @// fourth four pixels
    UXTB16  tmpa, n_tmp6                  @// |g3|g1|
    UXTAB16 tmpa, tmpa, n_tmp1            @// |g3+m3|g1+m1|
    UXTB16  tmpb, n_tmp5                  @// |c3|c1|
    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTAB16 tmpb, tmpb, n_tmp2            @// |c3+r3|c1+r1|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpa, tmpa, n_tmp4            @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp3            @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     res, = 0x00FF00FF
    UXTB16  tmpa, n_tmp6, ROR #8          @// |g4|g2|
    UXTAB16 tmpa, tmpa, n_tmp1, ROR #8    @// |g4+m4|g2+m2|
    AND     res, res, tmpb, LSR #5      @// mask and divide by 32

    ADD     tmpa, tmpa, tmpa, LSL #2    @// 5(G+M)
    UXTB16  tmpb, n_tmp5, ROR #8          @// |c4|c2|
    ADD     tmpa, plus16, tmpa, LSL #2  @// 16+20(G+M)
    UXTAB16 tmpb, tmpb, n_tmp2, ROR #8    @// |c4+r4|c2+r2|
    UXTAB16 tmpa, tmpa, n_tmp4, ROR #8    @// 16+20(G+M)+A
    UXTAB16 tmpa, tmpa, n_tmp3, ROR #8    @// 16+20(G+M)+A+T

    ADD     tmpb, tmpb, tmpb, LSL #2    @// 5(C+R)
    SSUB16  tmpa, tmpa, tmpb            @// 16+20(G+M)+(A+T)-5(C+R)

    USAT16  tmpb, #13, tmpa             @// saturate
    LDR     tmpa, [sp, #ppBuff] 
    LDR     n_tmp5, [tmpa], #16
    STR     tmpa, [sp, #ppBuff] 
    LDR     n_tmp4, = 0xFF00FF00
    MVN     n_tmp5, n_tmp5

    AND     tmpa, n_tmp4, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, = 0x80808080
    UHSUB8  res, res, n_tmp5              @// bilinear interpolation

    @// decrement loop_x counter
    SUBS    n_cnt, n_cnt, #4<<28        @// decrement x loop counter

    @// calculate "p_Src" address for next round
    SUB     p_Src, p_Src, n_w, LSL #3     @// p_Src -= 8*n_w;
    ADD     p_Src, p_Src, #4                @// next column (4 pixels)

    EOR     res, res, tmpa              @// correct sign
  
    LDR     tmpa, [sp, #ppBuff]
    SUB     tmpa, #60
    STR     tmpa, [sp, #ppBuff] 
       
    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb    
    STR     res, [p_Dst], tmpa
    SUB     p_Dst, tmpa, lsl #2
    ADD     p_Dst, p_Dst, #4


    BCS     add_loop_x

    ADDS    p_Dst, p_Dst, tmpa, lsl #2@     ;// set Carry=0
    ADD     p_Src, p_Src, n_w, LSL #2     @// p_Src += 4*n_w
    AND     n_tmp6, n_cnt, #0x00F00000    @// partWidth-1
    SBC     p_Src, p_Src, n_tmp6, LSR #20     @// -(partWidth-1)-1
    SBC     p_Dst, p_Dst, n_tmp6, LSR #20       @// -(partWidth-1)-1

    LDR     tmpa, [sp, #ppBuff]
    SBC     tmpa, tmpa, n_tmp6, LSR #20     @// -(partWidth-1)-1
    ADD     tmpa, #64
    STR     tmpa, [sp, #ppBuff] 
    
    ADDS    n_cnt, n_cnt, #0xC << 24    @// decrement y loop counter
    BGE     add_loop_y

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}       
	
	@.end

