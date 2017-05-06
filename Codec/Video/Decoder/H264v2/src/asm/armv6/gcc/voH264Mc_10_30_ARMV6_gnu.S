@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

    .global  get_luma_10_30_ARMV6
    .global  add_luma_10_30_ARMV6
  
	.align 8
 
 p_Src     .req r0

 p_Dst     .req r2
 buff    .req r2

 n_cnt   .req r1

 n_x20   .req r3

 src_stride   .req r4
 n_x31   .req r4

 n_h  .req r5
 n_x64   .req r5

 n_partW   .req r6
 n_x75   .req r6

 n_partH   .req r7
 n_tmp1    .req r7

 n_tmp2    .req r8

 n_tmp3    .req r9

 n_tmp4    .req r10

 n_mul20_01  .req r11
 n_deltaX      .req r11

 n_mul20_m5  .req r12
 n_deltaY      .req r12

 plus16  .req r14


@extern void get_luma_10_30_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset)	
get_luma_10_30_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     src_stride, r1                @// src_stride
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    MLA     n_tmp2, src_stride, n_deltaY, n_deltaX     @// n_deltaY*src_stride+n_deltaX
    ADD     p_Src, p_Src, n_tmp2          @// p_Src += n_deltaY*src_stride+n_deltaX
    ADD     p_Src, p_Src, #8             @// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partsrc_stride
    LDR     n_partH, [sp,#56]       @// partHeight

    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] src_stride
    MOV     n_cnt, src_stride
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_tmp2, n_partH, n_partW, LSL #4
    ADD     n_cnt, n_cnt, n_tmp2, LSL #16

    LDR     n_mul20_01, = 0x00140001
    LDR     n_mul20_m5, = 0x0014FFFB
    MOV     plus16, #16
    AND     n_tmp1, n_cnt, #0x000F0000    @// partHeight-1
    AND     n_tmp3, n_cnt, #0x00F00000    @// partWidth-1
    ADD     n_cnt, n_cnt, n_tmp1, LSL #8
loop_y:
    LDR     n_x31, [p_Src, #-8]
    ADD     n_cnt, n_cnt, n_tmp3, LSL #8
    LDR     n_x75, [p_Src, #-4]
    UXTB16  n_x20, n_x31
    UXTB16  n_x31, n_x31, ROR #8
    UXTB16  n_x64, n_x75

loop_x:
    UXTB16  n_x75, n_x75, ROR #8

    SMLAD   n_tmp1, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp3, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp2, n_x20, n_mul20_m5, plus16
    SMLATB  n_tmp4, n_x31, n_mul20_01, plus16

    SMLAD   n_tmp1, n_x31, n_mul20_m5, n_tmp1
    SMLATB  n_tmp3, n_x31, n_mul20_m5, n_tmp3
    SMLAD   n_tmp2, n_x31, n_mul20_01, n_tmp2
    LDR     n_x31, [p_Src], #4
    SMLAD   n_tmp4, n_x64, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x64, n_mul20_m5, n_tmp1
    SMLADX  n_tmp3, n_x64, n_mul20_m5, n_tmp3
    SMLADX  n_tmp2, n_x64, n_mul20_01, n_tmp2
    SMLADX  n_tmp4, n_x75, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x75, n_mul20_01, n_tmp1
    UXTB16  n_x20, n_x31
    SMLABB  n_tmp2, n_x75, n_mul20_m5, n_tmp2
    SMLADX  n_tmp3, n_x75, n_mul20_01, n_tmp3
    SMLABB  n_tmp4, n_x20, n_mul20_01, n_tmp4

    MOV     n_tmp2, n_tmp2, ASR #5
    MOV     n_tmp1, n_tmp1, ASR #5
    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #(16-5)
    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #(16-5)
    LDR     n_tmp4, [sp, #68]
    USAT16  n_tmp2, #8, n_tmp2
    USAT16  n_tmp1, #8, n_tmp1
    SUB     n_tmp4, n_tmp4, #10

    SUBS    n_cnt, n_cnt, #4<<28
    LDR     n_tmp3, [p_Src, n_tmp4]
    ORR     n_tmp1, n_tmp1, n_tmp2, LSL #8

@// quarter pel position
    LDR     n_tmp2, = 0x80808080
    MVN     n_tmp3, n_tmp3
    UHSUB8  n_tmp1, n_tmp1, n_tmp3
    EOR     n_tmp1, n_tmp1, n_tmp2
    STR     n_tmp1, [p_Dst], #4

    BCC     next_y

    UXTB16  n_x31, n_x31, ROR #8

    SMLAD   n_tmp1, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp3, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp2, n_x64, n_mul20_m5, plus16
    SMLATB  n_tmp4, n_x75, n_mul20_01, plus16

    SMLAD   n_tmp1, n_x75, n_mul20_m5, n_tmp1
    SMLATB  n_tmp3, n_x75, n_mul20_m5, n_tmp3
    SMLAD   n_tmp2, n_x75, n_mul20_01, n_tmp2
    LDR     n_x75, [p_Src], #4
    SMLAD   n_tmp4, n_x20, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x20, n_mul20_m5, n_tmp1
    SMLADX  n_tmp3, n_x20, n_mul20_m5, n_tmp3
    SMLADX  n_tmp2, n_x20, n_mul20_01, n_tmp2
    SMLADX  n_tmp4, n_x31, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x31, n_mul20_01, n_tmp1
    UXTB16  n_x64, n_x75
    SMLABB  n_tmp2, n_x31, n_mul20_m5, n_tmp2
    SMLADX  n_tmp3, n_x31, n_mul20_01, n_tmp3
    SMLABB  n_tmp4, n_x64, n_mul20_01, n_tmp4

    MOV     n_tmp2, n_tmp2, ASR #5
    MOV     n_tmp1, n_tmp1, ASR #5
    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #(16-5)
    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #(16-5)
    LDR     n_tmp4, [sp, #68]
    USAT16  n_tmp2, #8, n_tmp2
    USAT16  n_tmp1, #8, n_tmp1
    SUB     n_tmp4, n_tmp4, #10

    SUBS    n_cnt, n_cnt, #4<<28
    LDR     n_tmp3, [p_Src, n_tmp4]
    ORR     n_tmp1, n_tmp1, n_tmp2, LSL #8

@// quarter pel
    LDR     n_tmp2, = 0x80808080
    MVN     n_tmp3, n_tmp3
    UHSUB8  n_tmp1, n_tmp1, n_tmp3
    EOR     n_tmp1, n_tmp1, n_tmp2

    STR     n_tmp1, [p_Dst], #4
    BCS     loop_x

next_y:
    AND     n_tmp3, n_cnt, #0x00F00000    @// partWidth-1
    SMLABB  p_Src, n_cnt, n_mul20_01, p_Src @// +src_stride
    LDR     n_tmp1, [sp, #12]             @ dst_stride
    ADDS    p_Dst, p_Dst, n_tmp1                 @// +16, Carry=0
    SBC     p_Dst, p_Dst, n_tmp3, LSR #20       @// -(partWidth-1)-1
    SBC     p_Src, p_Src, n_tmp3, LSR #20     @// -(partWidth-1)-1
    ADDS    n_cnt, n_cnt, #(1<<28)-(1<<24)
    BGE     loop_y
	

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}

@extern void add_luma_10_30_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset)	
add_luma_10_30_ARMV6:
    STMFD   sp!, {r0-r11, lr}

    MOV     src_stride, r1                @// src_stride
    LDR     n_deltaX ,[sp,#60]         @// n_deltaX
    LDR     n_deltaY ,[sp,#64]         @// n_deltaY
    MLA     n_tmp2, src_stride, n_deltaY, n_deltaX     @// n_deltaY*src_stride+n_deltaX
    ADD     p_Src, p_Src, n_tmp2          @// p_Src += n_deltaY*src_stride+n_deltaX
    ADD     p_Src, p_Src, #8             @// p_Src = p_Src+8
    LDR     n_partW, [sp,#52]       @// partsrc_stride
    LDR     n_partH, [sp,#56]       @// partHeight

    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] src_stride
    MOV     n_cnt, src_stride
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_tmp2, n_partH, n_partW, LSL #4
    ADD     n_cnt, n_cnt, n_tmp2, LSL #16

    LDR     n_mul20_01, = 0x00140001
    LDR     n_mul20_m5, = 0x0014FFFB
    MOV     plus16, #16
    AND     n_tmp1, n_cnt, #0x000F0000    @// partHeight-1
    AND     n_tmp3, n_cnt, #0x00F00000    @// partWidth-1
    ADD     n_cnt, n_cnt, n_tmp1, LSL #8
add_loop_y:
    LDR     n_x31, [p_Src, #-8]
    ADD     n_cnt, n_cnt, n_tmp3, LSL #8
    LDR     n_x75, [p_Src, #-4]
    UXTB16  n_x20, n_x31
    UXTB16  n_x31, n_x31, ROR #8
    UXTB16  n_x64, n_x75

add_loop_x:
    UXTB16  n_x75, n_x75, ROR #8

    SMLAD   n_tmp1, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp3, n_x20, n_mul20_01, plus16
    SMLATB  n_tmp2, n_x20, n_mul20_m5, plus16
    SMLATB  n_tmp4, n_x31, n_mul20_01, plus16

    SMLAD   n_tmp1, n_x31, n_mul20_m5, n_tmp1
    SMLATB  n_tmp3, n_x31, n_mul20_m5, n_tmp3
    SMLAD   n_tmp2, n_x31, n_mul20_01, n_tmp2
    LDR     n_x31, [p_Src], #4
    SMLAD   n_tmp4, n_x64, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x64, n_mul20_m5, n_tmp1
    SMLADX  n_tmp3, n_x64, n_mul20_m5, n_tmp3
    SMLADX  n_tmp2, n_x64, n_mul20_01, n_tmp2
    SMLADX  n_tmp4, n_x75, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x75, n_mul20_01, n_tmp1
    UXTB16  n_x20, n_x31
    SMLABB  n_tmp2, n_x75, n_mul20_m5, n_tmp2
    SMLADX  n_tmp3, n_x75, n_mul20_01, n_tmp3
    SMLABB  n_tmp4, n_x20, n_mul20_01, n_tmp4

    MOV     n_tmp2, n_tmp2, ASR #5
    MOV     n_tmp1, n_tmp1, ASR #5
    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #(16-5)
    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #(16-5)
    LDR     n_tmp4, [sp, #68]
    USAT16  n_tmp2, #8, n_tmp2
    USAT16  n_tmp1, #8, n_tmp1
    SUB     n_tmp4, n_tmp4, #10

    SUBS    n_cnt, n_cnt, #4<<28
    LDR     n_tmp3, [p_Src, n_tmp4]
    ORR     n_tmp1, n_tmp1, n_tmp2, LSL #8

@// quarter pel position
    LDR     n_tmp2, = 0x80808080
    MVN     n_tmp3, n_tmp3
    UHSUB8  n_tmp1, n_tmp1, n_tmp3
    EOR     n_tmp1, n_tmp1, n_tmp2
    LDR     n_tmp3, [p_Dst]
    UHSUB8  n_tmp3, n_tmp1, n_tmp3
    USUB8   n_tmp1, n_tmp1, n_tmp3
    STR     n_tmp1, [p_Dst], #4

    BCC     add_next_y

    UXTB16  n_x31, n_x31, ROR #8

    SMLAD   n_tmp1, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp3, n_x64, n_mul20_01, plus16
    SMLATB  n_tmp2, n_x64, n_mul20_m5, plus16
    SMLATB  n_tmp4, n_x75, n_mul20_01, plus16

    SMLAD   n_tmp1, n_x75, n_mul20_m5, n_tmp1
    SMLATB  n_tmp3, n_x75, n_mul20_m5, n_tmp3
    SMLAD   n_tmp2, n_x75, n_mul20_01, n_tmp2
    LDR     n_x75, [p_Src], #4
    SMLAD   n_tmp4, n_x20, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x20, n_mul20_m5, n_tmp1
    SMLADX  n_tmp3, n_x20, n_mul20_m5, n_tmp3
    SMLADX  n_tmp2, n_x20, n_mul20_01, n_tmp2
    SMLADX  n_tmp4, n_x31, n_mul20_m5, n_tmp4

    SMLABB  n_tmp1, n_x31, n_mul20_01, n_tmp1
    UXTB16  n_x64, n_x75
    SMLABB  n_tmp2, n_x31, n_mul20_m5, n_tmp2
    SMLADX  n_tmp3, n_x31, n_mul20_01, n_tmp3
    SMLABB  n_tmp4, n_x64, n_mul20_01, n_tmp4

    MOV     n_tmp2, n_tmp2, ASR #5
    MOV     n_tmp1, n_tmp1, ASR #5
    PKHBT   n_tmp2, n_tmp2, n_tmp4, LSL #(16-5)
    PKHBT   n_tmp1, n_tmp1, n_tmp3, LSL #(16-5)
    LDR     n_tmp4, [sp, #68]
    USAT16  n_tmp2, #8, n_tmp2
    USAT16  n_tmp1, #8, n_tmp1
    SUB     n_tmp4, n_tmp4, #10

    SUBS    n_cnt, n_cnt, #4<<28
    LDR     n_tmp3, [p_Src, n_tmp4]
    ORR     n_tmp1, n_tmp1, n_tmp2, LSL #8

@// quarter pel
    LDR     n_tmp2, = 0x80808080
    MVN     n_tmp3, n_tmp3
    UHSUB8  n_tmp1, n_tmp1, n_tmp3
    EOR     n_tmp1, n_tmp1, n_tmp2
    
    LDR     n_tmp3, [p_Dst]
    UHSUB8  n_tmp3, n_tmp1, n_tmp3
    USUB8   n_tmp1, n_tmp1, n_tmp3
    STR     n_tmp1, [p_Dst], #4
    BCS     add_loop_x

add_next_y:
    AND     n_tmp3, n_cnt, #0x00F00000    @// partWidth-1
    SMLABB  p_Src, n_cnt, n_mul20_01, p_Src @// +src_stride
    LDR     n_tmp1, [sp, #12]             @ dst_stride
    ADDS    p_Dst, p_Dst, n_tmp1                 @// +16, Carry=0
    SBC     p_Dst, p_Dst, n_tmp3, LSR #20       @// -(partWidth-1)-1
    SBC     p_Src, p_Src, n_tmp3, LSR #20     @// -(partWidth-1)-1
    ADDS    n_cnt, n_cnt, #(1<<28)-(1<<24)
    BGE     add_loop_y
	

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}     
	@.end

