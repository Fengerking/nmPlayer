@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

    .global  get_luma_02_ARMV6
    .global  add_luma_02_ARMV6

	.align 8

 p_Src     .req r0
 p_Dst     .req r2

 n_cnt   .req r1
 n_deltaX  .req r1

 res     .req r3
 n_deltaY  .req r3

 n_tmp1    .req r4

 n_tmp2    .req r5
 n_h  .req r5

 n_tmp3    .req r6
 n_partW   .req r6

 n_tmp4    .req r7
 n_partH   .req r7

 n_tmp5    .req r8
 n_tmp6    .req r9

 tmpa    .req r10
 tmpb    .req r11
 n_w   .req r12

 plus16  .req r14


@extern void get_luma_02_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY)	
get_luma_02_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     n_w, r1                @// n_w
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    MLA     n_tmp2, n_w, n_deltaY, n_deltaX     @// n_deltaY*n_w+n_deltaX
    ADD     p_Src, p_Src, n_tmp2          @// p_Src += n_deltaY*n_w+n_deltaX
 @   ADD     p_Src, p_Src, #8             ;// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partWidth
    LDR     n_partH, [sp,#56]       @// partHeight
    
    ADD     n_cnt, n_partW, n_partH, LSL #16    @// |n_partH|n_partW|
    LDR     n_tmp5, = 0x00010001
    SSUB16  n_cnt, n_cnt, n_tmp5@     ;// |n_partH-1|n_partW-1|
    LDR     plus16, = 0x00100010

    AND     n_tmp1, n_cnt, #0x000000FF @// partWidth


get_loop_y:
    ADD     n_cnt, n_cnt, n_tmp1, LSL #24  @// partWidth-1 to top byte

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
    LDR     n_tmp1, [p_Src], n_w
    LDR     tmpa, = 0xFF00FF00

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divede by 32
    ORR     res, res, tmpa
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
    LDR     n_tmp2, [p_Src], n_w
    LDR     tmpa, = 0xFF00FF00

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa            @// next row

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
    LDR     n_tmp3, [p_Src]
    LDR     tmpa, = 0xFF00FF00

    @// decrement loop_x counter
    SUBS    n_cnt, n_cnt, #4<<24        @// (partWidth-1) -= 4;

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
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
    LDR     n_tmp4, = 0xFF00FF00

    @// calculate "p_Src" address for next round
    SUB     p_Src, p_Src, n_w, LSL #3     @// p_Src -= 8*n_w;
    ADD     p_Src, p_Src, #4@               ;// next column (4 pixels)
    AND     tmpa, n_tmp4, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, [sp,#12]              @ //dst_stride
    STR     res, [p_Dst], tmpa
    SUB     p_Dst, tmpa, lsl #2
    ADD     p_Dst, p_Dst, #4

    BCS     get_loop_x

    ADDS    n_cnt, n_cnt, #252<<16      @// (partHeight-1) -= 4;
    ADD     p_Src, p_Src, n_w, LSL #2     @// p_Src += 4*n_w
    AND     n_tmp1, n_cnt, #0x000000FF    @// partWidth-1
    ADD     n_tmp2, n_tmp1, #1              @// partWidth
    SUB     p_Src, p_Src, n_tmp2              @// p_Src -= partWidth
    ADD     p_Dst, p_Dst, tmpa, lsl #2@
    SUB     p_Dst, p_Dst, n_tmp2@               ;// p_Dst -= partWidth
    STR     p_Dst, [sp, #8]
    BGE     get_loop_y
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}   

@extern void add_luma_02_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY)	
add_luma_02_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     n_w, r1                @// n_w
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    MLA     n_tmp2, n_w, n_deltaY, n_deltaX     @// n_deltaY*n_w+n_deltaX
    ADD     p_Src, p_Src, n_tmp2          @// p_Src += n_deltaY*n_w+n_deltaX
 @   ADD     p_Src, p_Src, #8             ;// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partWidth
    LDR     n_partH, [sp,#56]       @// partHeight
    
    ADD     n_cnt, n_partW, n_partH, LSL #16    @// |n_partH|n_partW|
    LDR     n_tmp5, = 0x00010001
    SSUB16  n_cnt, n_cnt, n_tmp5@     ;// |n_partH-1|n_partW-1|
    LDR     plus16, = 0x00100010

    AND     n_tmp1, n_cnt, #0x000000FF @// partWidth


add_loop_y:
    ADD     n_cnt, n_cnt, n_tmp1, LSL #24  @// partWidth-1 to top byte

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
    LDR     n_tmp1, [p_Src], n_w
    LDR     tmpa, = 0xFF00FF00

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divede by 32
    ORR     res, res, tmpa
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
    LDR     n_tmp2, [p_Src], n_w
    LDR     tmpa, = 0xFF00FF00

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb    
    STR     res, [p_Dst], tmpa            @// next row

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
    LDR     n_tmp3, [p_Src]
    LDR     tmpa, = 0xFF00FF00

    @// decrement loop_x counter
    SUBS    n_cnt, n_cnt, #4<<24        @// (partWidth-1) -= 4;

    AND     tmpa, tmpa, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
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
    LDR     n_tmp4, = 0xFF00FF00

    @// calculate "p_Src" address for next round
    SUB     p_Src, p_Src, n_w, LSL #3     @// p_Src -= 8*n_w;
    ADD     p_Src, p_Src, #4@               ;// next column (4 pixels)
    AND     tmpa, n_tmp4, tmpb, LSL #3    @// mask and divide by 32
    ORR     res, res, tmpa
    LDR     tmpa, [sp,#12]              @ //dst_stride
    LDR     tmpb, [p_Dst]
    UHSUB8  tmpb, res, tmpb
    USUB8   res,  res, tmpb    
    STR     res, [p_Dst], tmpa
    SUB     p_Dst, tmpa, lsl #2
    ADD     p_Dst, p_Dst, #4

    BCS     add_loop_x

    ADDS    n_cnt, n_cnt, #252<<16      @// (partHeight-1) -= 4;
    ADD     p_Src, p_Src, n_w, LSL #2     @// p_Src += 4*n_w
    AND     n_tmp1, n_cnt, #0x000000FF    @// partWidth-1
    ADD     n_tmp2, n_tmp1, #1              @// partWidth
    SUB     p_Src, p_Src, n_tmp2              @// p_Src -= partWidth
    ADD     p_Dst, p_Dst, tmpa, lsl #2@
    SUB     p_Dst, p_Dst, n_tmp2@               ;// p_Dst -= partWidth
    STR     p_Dst, [sp, #8]
    BGE     add_loop_y
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      
	@.end

