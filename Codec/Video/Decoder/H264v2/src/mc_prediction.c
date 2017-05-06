#include "global.h"
#include "block.h"
#include "mc_prediction.h"
//#include "mbuffer.h"
//#include "mb_access.h"
//#include "macroblock.h"
//#include "memalloc.h"
#include "transform.h"
//#include "mv_prediction.h"
#include "ifunctions.h"

#if defined(VOARMV7)
extern void MCChroma8x8_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma8x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma4x8_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma4x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma4x2_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma2x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void MCChroma2x2_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma8x8_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma8x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma4x8_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma4x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma4x2_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma2x4_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void ADDMCChroma2x2_ARMV7(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern void AVG16x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG16x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG8x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG8x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG8x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG4x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG4x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG4x2_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG2x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void AVG2x2_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_U8 *src,VO_S32 src_stride);
extern void offset_mc_prediction_16x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_16x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_8x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_8x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_8x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_4x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_4x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_4x2_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_2x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void offset_mc_prediction_2x2_ARMV7(VO_U8 *dst, VO_S32 dst_stride ,VO_S32 offset);
extern void weighted_mc_prediction_16x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_16x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_8x16_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_8x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_8x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_4x8_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_4x4_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_mc_prediction_4x2_ARMV7(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern void weighted_bi_prediction_16x16_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_16x8_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_8x16_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_8x8_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_8x4_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_4x8_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_4x4_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern void weighted_bi_prediction_4x2_ARMV7(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);

void offset_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void get_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_h_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void offset_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void get_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_v_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void offset_luma_c_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_c_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_c_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void offset_luma_c_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void get_luma_c_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void get_luma_c_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void add_luma_c_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_offset_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_h_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_add_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_h_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_dst_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_offset_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_offset_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
void avg_dst_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_v_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_v_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_dst_luma_v_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
void avg_add_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_add_luma_v_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_offset_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_get_luma_h_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_h_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_8x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_4x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_get_luma_v_4x4_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride);
void avg_offset_luma_v_16x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_v_16x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_v_8x16_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);
void avg_offset_luma_v_8x8_ARMV7(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 wp_offset);

#endif
static inline VO_U8 Clip255( VO_S32 x )
{
	return (VO_U8)(x&(~255) ? (-x)>>31 : x);
}

#if defined(VOARMV6)
extern void get_luma_20_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void get_luma_10_30_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);
extern void get_luma_02_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void get_luma_01_03_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);
extern void get_luma_11_13_31_33_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);
extern void get_luma_22_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void get_luma_21_23_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 verOffset);
extern void get_luma_12_32_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);

extern void get_luma_00_16_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_8_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_4_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);

extern void get_luma_00_16x16_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_16x8_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_8x16_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_8x8_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_8x4_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_4x8_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern void get_luma_00_4x4_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);

extern void add_luma_20_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void add_luma_10_30_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);
extern void add_luma_02_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void add_luma_01_03_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);
extern void add_luma_11_13_31_33_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset, VO_S8 *pBuff);
extern void add_luma_22_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY);
extern void add_luma_21_23_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 verOffset);
extern void add_luma_12_32_ARMV6(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U32 partW, VO_U32 partH, VO_S32 deltaX, VO_S32 deltaY, VO_U32 horOffset);

extern void get_chroma_X0_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int partW, int partH);
extern void get_chroma_0X_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int partW, int partH);
extern void get_chroma_XX_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int w11, int partW, int partH);

extern void add_chroma_X0_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int partW, int partH);
extern void add_chroma_0X_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int partW, int partH);
extern void add_chroma_XX_ARMV6(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, int w00, int w01, int w10, int w11, int partW, int partH);

extern void wp_16_armv6(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height);
extern void wp_8_armv6(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height);
extern void wp_4_armv6(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height);
extern void wp_2_armv6(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height);


extern void of_16_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height);
extern void of_8_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height);
extern void of_4_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height);
extern void of_2_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height);

extern void  wbi_16_armv6(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
						VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height);
extern void  wbi_8_armv6(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
						VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height);
extern void  wbi_4_armv6(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
						VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height);
extern void  wbi_2_armv6(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
						VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height);

extern void bi_16_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height);
extern void bi_8_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height);
extern void bi_4_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height);
extern void bi_2_armv6(VO_U8 *dst, VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height);


#endif

#if defined(VOSSSE3)
#ifdef __cplusplus
extern "C" {
#endif 

//typedef struct xmm_reg { VO_U64 a, b; } xmm_reg;

MC_PREDICTION_LUMA_FUNC GetLumaBlock[7][16];
MC_PREDICTION_LUMA_FUNC AddLumaBlock[7][16] ;
MC_WEIGHTED_FUNC GetWeightedBlock[10];
MC_BIW_FUNC GetBIWBlock[10];
MC_PREDICTION_CHROMA_FUNC GetChromaBlock[7][4] ;
MC_PREDICTION_CHROMA_FUNC AddChromaBlock[7][4] ;


extern void vo_put_pixels4_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_avg_pixels4_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_put_pixels8_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_avg_pixels8_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_put_pixels16_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_avg_pixels16_mmxext(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_put_pixels16_sse2(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);
extern void vo_avg_pixels16_sse2(VO_U8 *block, const VO_U8 *pixels, VO_S32 line_size, VO_S32 h);

extern void vo_put_pixels8_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);
extern void vo_avg_pixels8_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);
extern void vo_put_pixels16_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);
extern void vo_avg_pixels16_l2_mmxext(VO_U8 *dst, VO_U8 *src1, VO_U8 *src2, int dstStride, int src1Stride, int h);


extern void vo_put_h264_qpel4_h_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_put_h264_qpel8_h_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_put_h264_qpel8_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_put_h264_qpel4_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_put_h264_qpel8_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_put_h264_qpel8_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_put_h264_qpel4_v_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_put_h264_qpel8or16_v_lowpass_op_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);
extern void vo_put_h264_qpel8or16_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);
extern void vo_put_h264_qpel4_hv_lowpass_v_mmxext(VO_U8 *src, VO_S16 *tmp, int srcStride);
extern void vo_put_h264_qpel4_hv_lowpass_h_mmxext(VO_S16 *tmp, VO_U8 *dst, int dstStride);
extern void vo_put_h264_qpel8or16_hv1_lowpass_op_mmxext(VO_U8 *src, VO_S16 *tmp, int srcStride, int size);
extern void vo_put_h264_qpel8or16_hv1_lowpass_op_sse2(VO_U8 *src, VO_S16 *tmp, int srcStride, int size);
extern void vo_put_h264_qpel8or16_hv2_lowpass_op_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int unused, int h);
extern void vo_put_h264_qpel8or16_hv2_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size);
extern void vo_put_pixels4_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h);
extern void vo_put_pixels8_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h);

extern void vo_avg_h264_qpel4_h_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_avg_h264_qpel8_h_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_avg_h264_qpel8_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_avg_h264_qpel4_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_avg_h264_qpel8_h_lowpass_l2_mmxext(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_avg_h264_qpel8_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride);
extern void vo_avg_h264_qpel4_v_lowpass_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride);
extern void vo_avg_h264_qpel8or16_v_lowpass_op_mmxext(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);
extern void vo_avg_h264_qpel8or16_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride, int h);
extern void vo_avg_h264_qpel4_hv_lowpass_v_mmxext(VO_U8 *src, VO_S16 *tmp, int srcStride);
extern void vo_avg_h264_qpel4_hv_lowpass_h_mmxext(VO_S16 *tmp, VO_U8 *dst, int dstStride);
extern void vo_avg_h264_qpel8or16_hv1_lowpass_op_mmxext(VO_U8 *src, VO_S16 *tmp, int srcStride, int size);
extern void vo_avg_h264_qpel8or16_hv1_lowpass_op_sse2(VO_U8 *src, VO_S16 *tmp, int srcStride, int size);
extern void vo_avg_h264_qpel8or16_hv2_lowpass_op_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int unused, int h);
extern void vo_avg_h264_qpel8or16_hv2_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size);
extern void vo_avg_pixels4_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h);
extern void vo_avg_pixels8_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h);

extern void vo_h264_biweight_4_mmxext(VO_U8 *dst, VO_U8 *src, int stride, int height, int log2_denom, int weightd, int weights, int offset);
extern void vo_h264_weight_4_mmxext(VO_U8 *dst, int stride, int height, int log2_denom, int weight, int offset);
extern void vo_h264_biweight_8_sse2(VO_U8 *dst, VO_U8 *src, int stride, int height, int log2_denom, int weightd, int weights, int offset);
extern void vo_h264_weight_8_sse2(VO_U8 *dst, int stride, int height, int log2_denom, int weight, int offset);
extern void vo_h264_biweight_16_sse2(VO_U8 *dst, VO_U8 *src, int stride, int height, int log2_denom, int weightd, int weights, int offset);
extern void vo_h264_weight_16_sse2(VO_U8 *dst, int stride, int height, int log2_denom, int weight, int offset);
extern void vo_h264_biweight_8_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int height, int log2_denom, int weightd, int weights, int offset);
extern void vo_h264_weight_8_ssse3(VO_U8 *dst, int stride, int height, int log2_denom, int weight, int offset);
extern void vo_h264_biweight_16_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int height, int log2_denom, int weightd, int weights, int offset);
extern void vo_h264_weight_16_ssse3(VO_U8 *dst, int stride, int height, int log2_denom, int weight, int offset);

extern 	void vo_put_h264_chroma_mc8_rnd_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);
extern	void vo_put_h264_chroma_mc4_ssse3	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);	
extern	void vo_avg_h264_chroma_mc8_rnd_ssse3(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);
extern	void vo_avg_h264_chroma_mc4_ssse3	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);

extern 	void vo_put_h264_chroma_mc8_rnd_mmx(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);
extern	void vo_put_h264_chroma_mc4_mmx	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);	
extern	void vo_avg_h264_chroma_mc8_rnd_mmxext(VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);
extern	void vo_avg_h264_chroma_mc4_mmxext	 (VO_U8 *dst, VO_U8 *src, int stride, int h, int x, int y);


#ifdef __cplusplus
}
#endif 

DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_16)   = { 0x0010001000100010ULL, 0x0010001000100010ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_5)    = { 0x0005000500050005ULL, 0x0005000500050005ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pb_0)    = { 0x0000000000000000ULL, 0x0000000000000000ULL };

DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_3)    = { 0x0003000300030003ULL, 0x0003000300030003ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_4)    = { 0x0004000400040004ULL, 0x0004000400040004ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_8)    = { 0x0008000800080008ULL, 0x0008000800080008ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_32)   = { 0x0020002000200020ULL, 0x0020002000200020ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pw_64)   = { 0x0040004000400040ULL, 0x0040004000400040ULL };

static  void put_h264_qpel8or16_hv1_lowpass_sse2(VO_S16 *tmp,
        VO_U8 *src,
        int tmpStride,
        int srcStride,
        int size)
{
    int w = (size + 8) >> 3;
    src -= 2 * srcStride + 2;
    while(w--)
    {
        vo_put_h264_qpel8or16_hv1_lowpass_op_sse2(src, tmp, srcStride, size);
        tmp += 8;
        src += 8;
    }
}

static  void vo_put_h264_qpel8_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_put_h264_qpel8or16_v_lowpass_sse2(dst , src , dstStride, srcStride, 8);
}

static  void vo_put_pixels16_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h)
{
    vo_put_pixels8_l2_shift5_mmxext(dst , src16 , src8 , dstStride, src8Stride, h);
    vo_put_pixels8_l2_shift5_mmxext(dst + 8, src16 + 8, src8 + 8, dstStride, src8Stride, h);
}

static  void vo_put_h264_qpel16_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_put_h264_qpel8or16_v_lowpass_sse2(dst , src , dstStride, srcStride, 16);
    vo_put_h264_qpel8or16_v_lowpass_sse2(dst + 8, src + 8, dstStride, srcStride, 16);
}


static  void vo_avg_pixels16_l2_shift5_mmxext(VO_U8 *dst, VO_S16 *src16, VO_U8 *src8, int dstStride, int src8Stride, int h)
{
    vo_avg_pixels8_l2_shift5_mmxext(dst , src16 , src8 , dstStride, src8Stride, h);
    vo_avg_pixels8_l2_shift5_mmxext(dst + 8, src16 + 8, src8 + 8, dstStride, src8Stride, h);
}


static void put_h264_qpel16_mc00_sse2 (VO_U8 *dst, VO_U8 *src,
                                       VO_S32 stride)
{
    vo_put_pixels16_sse2(dst, src, stride, 16);
}

static void avg_h264_qpel16_mc00_sse2 (VO_U8 *dst, VO_U8 *src,
                                       VO_S32 stride)
{
    vo_avg_pixels16_sse2(dst, src, stride, 16);
}

static  void vo_avg_h264_qpel8_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_avg_h264_qpel8or16_v_lowpass_sse2(dst , src , dstStride, srcStride, 8);
}

static  void vo_avg_h264_qpel16_v_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_avg_h264_qpel8or16_v_lowpass_sse2(dst , src , dstStride, srcStride, 16);
    vo_avg_h264_qpel8or16_v_lowpass_sse2(dst + 8, src + 8, dstStride, srcStride, 16);
}


static  void vo_put_h264_qpel16_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst , src , src2 , dstStride, src2Stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
    src += 8 * dstStride;
    dst += 8 * dstStride;
    src2 += 8 * src2Stride;
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst , src , src2 , dstStride, src2Stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
}
static  void vo_put_h264_qpel16_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_put_h264_qpel8_h_lowpass_ssse3(dst , src , dstStride, srcStride);
    vo_put_h264_qpel8_h_lowpass_ssse3(dst + 8, src + 8, dstStride, srcStride);
    src += 8 * srcStride;
    dst += 8 * dstStride;
    vo_put_h264_qpel8_h_lowpass_ssse3(dst , src , dstStride, srcStride);
    vo_put_h264_qpel8_h_lowpass_ssse3(dst + 8, src + 8, dstStride, srcStride);
}
static  void vo_avg_h264_qpel16_h_lowpass_l2_ssse3(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst , src , src2 , dstStride, src2Stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
    src += 8 * dstStride;
    dst += 8 * dstStride;
    src2 += 8 * src2Stride;
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst , src , src2 , dstStride, src2Stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
}
static  void vo_avg_h264_qpel16_h_lowpass_ssse3(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst , src , dstStride, srcStride);
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst + 8, src + 8, dstStride, srcStride);
    src += 8 * srcStride;
    dst += 8 * dstStride;
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst , src , dstStride, srcStride);
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst + 8, src + 8, dstStride, srcStride);
}
static  void vo_put_h264_qpel8or16_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride, int size)
{
    put_h264_qpel8or16_hv1_lowpass_sse2(tmp, src, tmpStride, srcStride, size);
    vo_put_h264_qpel8or16_hv2_lowpass_ssse3(dst, tmp, dstStride, tmpStride, size);
}
static  void vo_put_h264_qpel8_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_put_h264_qpel8or16_hv_lowpass_ssse3(dst, tmp, src, dstStride, tmpStride, srcStride, 8);
}
static  void vo_put_h264_qpel16_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_put_h264_qpel8or16_hv_lowpass_ssse3(dst, tmp, src, dstStride, tmpStride, srcStride, 16);
}
static  void vo_avg_h264_qpel8or16_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride, int size)
{
    put_h264_qpel8or16_hv1_lowpass_sse2(tmp, src, tmpStride, srcStride, size);
    vo_avg_h264_qpel8or16_hv2_lowpass_ssse3(dst, tmp, dstStride, tmpStride, size);
}
static  void vo_avg_h264_qpel8_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_avg_h264_qpel8or16_hv_lowpass_ssse3(dst, tmp, src, dstStride, tmpStride, srcStride, 8);
}
static  void vo_avg_h264_qpel16_hv_lowpass_ssse3(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_avg_h264_qpel8or16_hv_lowpass_ssse3(dst, tmp, src, dstStride, tmpStride, srcStride, 16);
}


static void get_block_00_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels16_sse2(dst, src, src_stride, 16);
}
static void get_block_00_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels16_sse2(dst, src, src_stride, 8);
}
static void get_block_00_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 16);
}
static void get_block_00_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_8x4_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 4);
}
static void get_block_00_4x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels4_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_4x4_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels4_mmxext(dst, src, src_stride, 4);
}


static void get_luma_10_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
}
static void get_luma_10_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_10_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;
   vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);	
}
static void get_luma_10_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
}



static void get_luma_20_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_ssse3(dst, src, src_stride, src_stride);
}
static void get_luma_20_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_ssse3(dst + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_20_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;	
    vo_put_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);	
}
static void get_luma_20_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
}


static void get_luma_30_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
}
static void get_luma_30_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst + 8, src + 8, src + 1 + 8, src_stride, src_stride);	
}
static void get_luma_30_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;	
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);	
}
static void get_luma_30_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
}


static void get_luma_01_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_pixels16_l2_mmxext(dst, src, temp, src_stride, src_stride, 16);
}
static void get_luma_01_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src+8, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst+8, src+8, temp, src_stride, src_stride, 8);	
}
static void get_luma_01_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;		
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void get_luma_01_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
}


static void get_luma_02_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_v_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void get_luma_02_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    vo_put_h264_qpel8_v_lowpass_sse2(dst + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_02_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;		
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void get_luma_02_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
}


static void get_luma_03_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_pixels16_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 16);
}
static void get_luma_03_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;	
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void get_luma_03_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;			
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void get_luma_03_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
}



static void get_luma_11_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 16);
}
static void get_luma_11_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);	
}
static void get_luma_11_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);	
}
static void get_luma_11_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
}

static void get_luma_12_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_pixels16_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 16, 16);
}
static void get_luma_12_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}
static void get_luma_12_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}
static void get_luma_12_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}

static void get_luma_13_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 16);
}
static void get_luma_13_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}
static void get_luma_13_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}
static void get_luma_13_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}


static void get_luma_21_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 16);
}
static void get_luma_21_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);	
}
static void get_luma_21_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);	
}
static void get_luma_21_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
}


static void get_luma_22_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[16*(16<8?12:24)];
    vo_put_h264_qpel16_hv_lowpass_ssse3(dst, temp, src, src_stride, 16, src_stride);
}
static void get_luma_22_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);	
}
static void get_luma_22_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);	
}
static void get_luma_22_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
}


static void get_luma_23_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 16);
}
static void get_luma_23_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void get_luma_23_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void get_luma_23_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
}


static void get_luma_31_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 16);
}
static void get_luma_31_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
	
}
static void get_luma_31_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
	
}
static void get_luma_31_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
}


static void get_luma_32_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_pixels16_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 16, 16);
}
static void get_luma_32_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void get_luma_32_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void get_luma_32_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
}


static void get_luma_33_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 16);
}
static void get_luma_33_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);	
}
static void get_luma_33_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);	
}
static void get_luma_33_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}


static void add_block_00_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels16_sse2(dst, src, src_stride, 16);
}
static void add_block_00_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels16_sse2(dst, src, src_stride, 8);
}
static void add_block_00_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 16);
}
static void add_block_00_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 8);
}
static void add_block_00_8x4_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 4);
}
static void add_block_00_4x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 8);
}
static void add_block_00_4x4_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 4);
}

static void add_luma_01_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_pixels16_l2_mmxext(dst, src, temp, src_stride, src_stride, 16);
}
static void add_luma_01_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void add_luma_01_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void add_luma_01_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
}

static void add_luma_02_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_v_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void add_luma_02_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void add_luma_02_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void add_luma_02_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
}


static void add_luma_03_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_pixels16_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 16);
}
static void add_luma_03_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void add_luma_03_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void add_luma_03_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
}

static void add_luma_10_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
}
static void add_luma_10_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
	
}
static void add_luma_10_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
	
}
static void add_luma_10_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src, src_stride, src_stride);
}


static void add_luma_20_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_ssse3(dst, src, src_stride, src_stride);
}
static void add_luma_20_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
	
}
static void add_luma_20_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);	
}
static void add_luma_20_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_ssse3(dst, src, src_stride, src_stride);
}


static void add_luma_30_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
}
static void add_luma_30_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);	
}
static void add_luma_30_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);	
}
static void add_luma_30_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, src + 1, src_stride, src_stride);
}


static void add_luma_11_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 16);
}
static void add_luma_11_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
	
}
static void add_luma_11_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);	
}
static void add_luma_11_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
}


static void add_luma_12_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_pixels16_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 16, 16);
}
static void add_luma_12_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
	
}
static void add_luma_12_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
	
}
static void add_luma_12_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}


static void add_luma_13_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 16);
}
static void add_luma_13_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
	
}
static void add_luma_13_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
	
}
static void add_luma_13_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}


static void add_luma_21_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 16);
}
static void add_luma_21_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);	
}
static void add_luma_21_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);	
}
static void add_luma_21_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, halfHV, src_stride, 8);
}

static void add_luma_22_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[16*(16<8?12:24)];
    vo_avg_h264_qpel16_hv_lowpass_ssse3(dst, temp, src, src_stride, 16, src_stride);
}
static void add_luma_22_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
    src += 8;
    dst += 8;				
    vo_avg_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
	
}
static void add_luma_22_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
	
}
static void add_luma_22_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_ssse3(dst, temp, src, src_stride, 8, src_stride);
}


static void add_luma_23_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 16);
}
static void add_luma_23_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void add_luma_23_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void add_luma_23_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, halfHV, src_stride, 8);
}


static void add_luma_31_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 16);
}
static void add_luma_31_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);	
}
static void add_luma_31_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);	
}
static void add_luma_31_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src, temp, src_stride, 8);
}

static void add_luma_32_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_ssse3(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_pixels16_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 16, 16);
}
static void add_luma_32_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void add_luma_32_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void add_luma_32_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_ssse3(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
}

static void add_luma_33_16x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 16);
}
static void add_luma_33_16x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);	
}
static void add_luma_33_8x16_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);	
}
static void add_luma_33_8x8_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_ssse3(dst, src + src_stride, temp, src_stride, 8);
}


static void get_block_00_8x8_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels8_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_8x4_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels8_mmxext(dst, src, src_stride, 4);
}
static void get_block_00_4x8_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels4_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_4x4_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels4_mmxext(dst, src, src_stride, 4);
}

void add_block_00_8x8_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 8);
}
void add_block_00_8x4_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 4);
}
void add_block_00_4x8_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 8);
}
void add_block_00_4x4_chroma_ssse3(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 4);
}

void get_chroma_0X_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_X0_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_XX_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}

void get_chroma_0X_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_X0_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_XX_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}

void get_chroma_0X_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_X0_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_XX_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}


void get_chroma_0X_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_X0_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_XX_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}


void add_chroma_0X_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_X0_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_XX_8x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 8, w00, w01);
}

void add_chroma_0X_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_X0_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_XX_8x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_ssse3(dst, src, src_stride, 4, w00, w01);
}

void add_chroma_0X_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_X0_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_XX_4x8_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 8, w00, w01);
}


void add_chroma_0X_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_X0_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_XX_4x4_ssse3(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_ssse3(dst, src, src_stride, 4, w00, w01);
}



static void weighted_bi_prediction_16x16_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_16_ssse3(dst, src, src_stride,  16, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_16x8_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_16_ssse3(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x16_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_ssse3(dst, src, src_stride,  16, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x8_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_ssse3(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x4_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_ssse3(dst, src, src_stride,  4, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_4x8_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_4_mmxext(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_4x4_ssse3(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_4_mmxext(dst, src, src_stride,  4, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}


void weighted_mc_prediction_16x16_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_16_sse2(dst, dst_stride, 16, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_16x8_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_16_sse2(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x16_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 16, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x8_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x4_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 4, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_4x8_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_4_mmxext(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_4x4_ssse3(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_4_mmxext(dst, dst_stride, 4, weight_denom,  wp_scale, wp_offset);
}


// sse2

static  void vo_put_h264_qpel8or16_hv2_lowpass_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size)
{
    int w = size >> 4;
    do
    {
        vo_put_h264_qpel8or16_hv2_lowpass_op_mmxext(dst, tmp, dstStride, 0, size);
        tmp += 8;
        dst += 8;
    }
    while(w--);
}

static  void vo_avg_h264_qpel8or16_hv2_lowpass_mmxext(VO_U8 *dst, VO_S16 *tmp, int dstStride, int tmpStride, int size)
{
    int w = size >> 4;
    do
    {
       vo_avg_h264_qpel8or16_hv2_lowpass_op_mmxext(dst, tmp, dstStride, 0, size);
        tmp += 8;
        dst += 8;
    }
    while(w--);
}


static  void vo_put_h264_qpel16_h_lowpass_l2_sse2(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst , src , src2 , dstStride, src2Stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
    src += 8 * dstStride;
    dst += 8 * dstStride;
    src2 += 8 * src2Stride;
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst , src , src2 , dstStride, src2Stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
}
static  void vo_put_h264_qpel16_h_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_put_h264_qpel8_h_lowpass_mmxext(dst , src , dstStride, srcStride);
    vo_put_h264_qpel8_h_lowpass_mmxext(dst + 8, src + 8, dstStride, srcStride);
    src += 8 * srcStride;
    dst += 8 * dstStride;
    vo_put_h264_qpel8_h_lowpass_mmxext(dst , src , dstStride, srcStride);
    vo_put_h264_qpel8_h_lowpass_mmxext(dst + 8, src + 8, dstStride, srcStride);
}
static  void vo_avg_h264_qpel16_h_lowpass_l2_sse2(VO_U8 *dst, VO_U8 *src, VO_U8 *src2, int dstStride, int src2Stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst , src , src2 , dstStride, src2Stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
    src += 8 * dstStride;
    dst += 8 * dstStride;
    src2 += 8 * src2Stride;
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst , src , src2 , dstStride, src2Stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src2 + 8, dstStride, src2Stride);
}
static  void vo_avg_h264_qpel16_h_lowpass_sse2(VO_U8 *dst, VO_U8 *src, int dstStride, int srcStride)
{
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst , src , dstStride, srcStride);
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst + 8, src + 8, dstStride, srcStride);
    src += 8 * srcStride;
    dst += 8 * dstStride;
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst , src , dstStride, srcStride);
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst + 8, src + 8, dstStride, srcStride);
}
static  void vo_put_h264_qpel8or16_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride, int size)
{
    put_h264_qpel8or16_hv1_lowpass_sse2(tmp, src, tmpStride, srcStride, size);
    vo_put_h264_qpel8or16_hv2_lowpass_mmxext(dst, tmp, dstStride, tmpStride, size);
}
static  void vo_put_h264_qpel8_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_put_h264_qpel8or16_hv_lowpass_sse2(dst, tmp, src, dstStride, tmpStride, srcStride, 8);
}
static  void vo_put_h264_qpel16_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_put_h264_qpel8or16_hv_lowpass_sse2(dst, tmp, src, dstStride, tmpStride, srcStride, 16);
}
static  void vo_avg_h264_qpel8or16_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride, int size)
{
    put_h264_qpel8or16_hv1_lowpass_sse2(tmp, src, tmpStride, srcStride, size);
    vo_avg_h264_qpel8or16_hv2_lowpass_mmxext(dst, tmp, dstStride, tmpStride, size);
}
static  void vo_avg_h264_qpel8_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_avg_h264_qpel8or16_hv_lowpass_sse2(dst, tmp, src, dstStride, tmpStride, srcStride, 8);
}
static  void vo_avg_h264_qpel16_hv_lowpass_sse2(VO_U8 *dst, VO_S16 *tmp, VO_U8 *src, int dstStride, int tmpStride, int srcStride)
{
    vo_avg_h264_qpel8or16_hv_lowpass_sse2(dst, tmp, src, dstStride, tmpStride, srcStride, 16);
}


static void get_block_00_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels16_sse2(dst, src, src_stride, 16);
}
static void get_block_00_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels16_sse2(dst, src, src_stride, 8);
}
static void get_block_00_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 16);
}
static void get_block_00_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_8x4_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels8_mmxext(dst, src, src_stride, 4);
}
static void get_block_00_4x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels4_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_4x4_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_pixels4_mmxext(dst, src, src_stride, 4);
}


static void get_luma_10_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src, src, src_stride, src_stride);
}
static void get_luma_10_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_10_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;
   vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);	
}
static void get_luma_10_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
}



static void get_luma_20_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void get_luma_20_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_mmxext(dst + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_20_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;	
    vo_put_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);	
}
static void get_luma_20_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
}


static void get_luma_30_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src, src + 1, src_stride, src_stride);
}
static void get_luma_30_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst + 8, src + 8, src + 1 + 8, src_stride, src_stride);	
}
static void get_luma_30_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;	
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);	
}
static void get_luma_30_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
}


static void get_luma_01_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_pixels16_l2_mmxext(dst, src, temp, src_stride, src_stride, 16);
}
static void get_luma_01_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src+8, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst+8, src+8, temp, src_stride, src_stride, 8);	
}
static void get_luma_01_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;		
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void get_luma_01_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
}


static void get_luma_02_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel16_v_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void get_luma_02_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    vo_put_h264_qpel8_v_lowpass_sse2(dst + 8, src + 8, src_stride, src_stride);	
}
static void get_luma_02_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;		
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void get_luma_02_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_put_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
}


static void get_luma_03_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_pixels16_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 16);
}
static void get_luma_03_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;	
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void get_luma_03_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;			
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void get_luma_03_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
}



static void get_luma_11_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src, temp, src_stride, 16);
}
static void get_luma_11_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);	
}
static void get_luma_11_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);	
}
static void get_luma_11_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
}

static void get_luma_12_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_pixels16_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 16, 16);
}
static void get_luma_12_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}
static void get_luma_12_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}
static void get_luma_12_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}

static void get_luma_13_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, temp, src_stride, 16);
}
static void get_luma_13_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}
static void get_luma_13_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}
static void get_luma_13_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
   DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}


static void get_luma_21_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src, halfHV, src_stride, 16);
}
static void get_luma_21_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);	
}
static void get_luma_21_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);	
}
static void get_luma_21_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
}


static void get_luma_22_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[16*(16<8?12:24)];
    vo_put_h264_qpel16_hv_lowpass_sse2(dst, temp, src, src_stride, 16, src_stride);
}
static void get_luma_22_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);	
}
static void get_luma_22_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);	
}
static void get_luma_22_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_put_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
}


static void get_luma_23_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, halfHV, src_stride, 16);
}
static void get_luma_23_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void get_luma_23_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void get_luma_23_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
}


static void get_luma_31_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src, temp, src_stride, 16);
}
static void get_luma_31_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
	
}
static void get_luma_31_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
	
}
static void get_luma_31_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
}


static void get_luma_32_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_put_pixels16_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 16, 16);
}
static void get_luma_32_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void get_luma_32_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void get_luma_32_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_put_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
}


static void get_luma_33_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_put_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, temp, src_stride, 16);
}
static void get_luma_33_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);	
}
static void get_luma_33_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);	
}
static void get_luma_33_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_put_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}


static void add_block_00_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels16_sse2(dst, src, src_stride, 16);
}
static void add_block_00_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels16_sse2(dst, src, src_stride, 8);
}
static void add_block_00_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 16);
}
static void add_block_00_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 8);
}
static void add_block_00_8x4_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 4);
}
static void add_block_00_4x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 8);
}
static void add_block_00_4x4_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 4);
}

static void add_luma_01_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_pixels16_l2_mmxext(dst, src, temp, src_stride, src_stride, 16);
}
static void add_luma_01_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void add_luma_01_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);	
}
static void add_luma_01_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src, temp, src_stride, src_stride, 8);
}

static void add_luma_02_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_v_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void add_luma_02_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void add_luma_02_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);	
}
static void add_luma_02_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_v_lowpass_sse2(dst, src, src_stride, src_stride);
}


static void add_luma_03_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_pixels16_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 16);
}
static void add_luma_03_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void add_luma_03_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);	
}
static void add_luma_03_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_pixels8_l2_mmxext(dst, src + src_stride, temp, src_stride, src_stride, 8);
}

static void add_luma_10_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src, src, src_stride, src_stride);
}
static void add_luma_10_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
	
}
static void add_luma_10_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
	
}
static void add_luma_10_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src, src_stride, src_stride);
}


static void add_luma_20_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_sse2(dst, src, src_stride, src_stride);
}
static void add_luma_20_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
	
}
static void add_luma_20_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);	
}
static void add_luma_20_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_mmxext(dst, src, src_stride, src_stride);
}


static void add_luma_30_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src, src + 1, src_stride, src_stride);
}
static void add_luma_30_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
    src = src + 8;
    dst = dst + 8;
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);	
}
static void add_luma_30_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);	
}
static void add_luma_30_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, src + 1, src_stride, src_stride);
}


static void add_luma_11_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src, temp, src_stride, 16);
}
static void add_luma_11_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src = src + 8;
    dst = dst + 8;
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
	
}
static void add_luma_11_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);	
}
static void add_luma_11_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
}


static void add_luma_12_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_pixels16_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 16, 16);
}
static void add_luma_12_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
	
}
static void add_luma_12_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
	
}
static void add_luma_12_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 2, halfHV, src_stride, 8, 8);
}


static void add_luma_13_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, temp, src_stride, 16);
}
static void add_luma_13_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
	
}
static void add_luma_13_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
	
}
static void add_luma_13_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}


static void add_luma_21_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src, halfHV, src_stride, 16);
}
static void add_luma_21_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);	
}
static void add_luma_21_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);	
}
static void add_luma_21_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, halfHV, src_stride, 8);
}

static void add_luma_22_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[16*(16<8?12:24)];
    vo_avg_h264_qpel16_hv_lowpass_sse2(dst, temp, src, src_stride, 16, src_stride);
}
static void add_luma_22_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
    src += 8;
    dst += 8;				
    vo_avg_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
	
}
static void add_luma_22_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_avg_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
	
}
static void add_luma_22_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_S16, temp)[8*(8<8?12:24)];
    vo_avg_h264_qpel8_hv_lowpass_sse2(dst, temp, src, src_stride, 8, src_stride);
}


static void add_luma_23_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, halfHV, src_stride, 16);
}
static void add_luma_23_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void add_luma_23_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);	
}
static void add_luma_23_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, halfHV, src_stride, 8);
}


static void add_luma_31_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src, temp, src_stride, 16);
}
static void add_luma_31_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);	
}
static void add_luma_31_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);	
}
static void add_luma_31_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src, temp, src_stride, 8);
}

static void add_luma_32_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*(16<8?12:24)*2 + 16*16];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 16 * 16);
    
    vo_put_h264_qpel16_hv_lowpass_sse2(halfHV, halfV, src, 16, 16, src_stride);
    vo_avg_pixels16_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 16, 16);
}
static void add_luma_32_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void add_luma_32_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);	
}
static void add_luma_32_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*(8<8?12:24)*2 + 8*8];
    VO_U8 *const halfHV = temp;
    VO_S16 *const halfV = (VO_S16 *)(temp + 8 * 8);
    
    vo_put_h264_qpel8_hv_lowpass_sse2(halfHV, halfV, src, 8, 8, src_stride);
    vo_avg_pixels8_l2_shift5_mmxext(dst, halfV + 3, halfHV, src_stride, 8, 8);
}

static void add_luma_33_16x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[16*16];
    vo_put_h264_qpel16_v_lowpass_sse2(temp, src + 1, 16, src_stride);
    vo_avg_h264_qpel16_h_lowpass_l2_sse2(dst, src + src_stride, temp, src_stride, 16);
}
static void add_luma_33_16x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8;
    dst += 8;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);	
}
static void add_luma_33_8x16_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
    src += 8 * src_stride;
    dst += 8 * dst_stride;				
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);	
}
static void add_luma_33_8x8_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    DECLARE_ALIGNED_EXT(16, VO_U8, temp)[8*8];
    vo_put_h264_qpel8_v_lowpass_sse2(temp, src + 1, 8, src_stride);
    vo_avg_h264_qpel8_h_lowpass_l2_mmxext(dst, src + src_stride, temp, src_stride, 8);
}

static void get_block_00_8x8_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels8_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_8x4_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels8_mmxext(dst, src, src_stride, 4);
}
static void get_block_00_4x8_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels4_mmxext(dst, src, src_stride, 8);
}
static void get_block_00_4x4_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
     vo_put_pixels4_mmxext(dst, src, src_stride, 4);
}

void add_block_00_8x8_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 8);
}
void add_block_00_8x4_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels8_mmxext(dst, src, src_stride, 4);
}
void add_block_00_4x8_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 8);
}
void add_block_00_4x4_chroma_sse2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_pixels4_mmxext(dst, src, src_stride, 4);
}

void get_chroma_0X_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_X0_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_XX_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 8, w00, w01);
}

void get_chroma_0X_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_X0_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_XX_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc8_rnd_mmx(dst, src, src_stride, 4, w00, w01);
}

void get_chroma_0X_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_X0_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 8, w00, w01);
}
void get_chroma_XX_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 8, w00, w01);
}


void get_chroma_0X_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_X0_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 4, w00, w01);
}
void get_chroma_XX_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_put_h264_chroma_mc4_mmx(dst, src, src_stride, 4, w00, w01);
}


void add_chroma_0X_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_X0_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_XX_8x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 8, w00, w01);
}

void add_chroma_0X_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_X0_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_XX_8x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc8_rnd_mmxext(dst, src, src_stride, 4, w00, w01);
}

void add_chroma_0X_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_X0_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 8, w00, w01);
}
void add_chroma_XX_4x8_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 8, w00, w01);
}


void add_chroma_0X_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_X0_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 4, w00, w01);
}
void add_chroma_XX_4x4_sse2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
    vo_avg_h264_chroma_mc4_mmxext(dst, src, src_stride, 4, w00, w01);
}

static void weighted_bi_prediction_16x16_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_16_sse2(dst, src, src_stride,  16, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_16x8_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_16_sse2(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x16_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_sse2(dst, src, src_stride,  16, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x8_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_sse2(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_8x4_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_8_sse2(dst, src, src_stride,  4, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_4x8_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_4_mmxext(dst, src, src_stride,  8, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}
static void weighted_bi_prediction_4x4_sse2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
     vo_h264_biweight_4_mmxext(dst, src, src_stride,  4, weight_denom,  wp_scale_l0, wp_scale_l1, wp_offset);
}


void weighted_mc_prediction_16x16_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_16_sse2(dst, dst_stride, 16, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_16x8_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_16_sse2(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x16_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 16, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x8_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_8x4_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_8_sse2(dst, dst_stride, 4, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_4x8_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_4_mmxext(dst, dst_stride, 8, weight_denom,  wp_scale, wp_offset);
}
void weighted_mc_prediction_4x4_sse2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
     vo_h264_weight_4_mmxext(dst, dst_stride, 4, weight_denom,  wp_scale, wp_offset);
}
#endif



#define Load2(a,o) \
{       \
 a=src[o+0]|(src[o+1]<<8); \
}

#define Save2(a) \
{      \
 ((VO_U16*)dst)[0]=a;\
}
#define Load4(a,o) \
{       \
 a=src[o+0]|(src[o+1]<<8)|(src[o+2]<<16)|(src[o+3]<<24); \
}

#define Save4(a) \
{      \
 ((VO_U32*)dst)[0]=a;\
}
#define Load8(a,b,o) \
{       \
 a=src[o+0]|(src[o+1]<<8)|(src[o+2]<<16)|(src[o+3]<<24); \
 b=src[o+4]|(src[o+5]<<8)|(src[o+6]<<16)|(src[o+7]<<24); \
}

#define Save8(a,b) \
{      \
 ((VO_U32*)dst)[0]=a;\
 ((VO_U32*)dst)[1]=b;\
}
#define Load16(a,b,c,d,o) \
{       \
 a=src[o+0]|(src[o+1]<<8)|(src[o+2]<<16)|(src[o+3]<<24); \
 b=src[o+4]|(src[o+5]<<8)|(src[o+6]<<16)|(src[o+7]<<24); \
 c=src[o+8]|(src[o+9]<<8)|(src[o+10]<<16)|(src[o+11]<<24); \
 d=src[o+12]|(src[o+13]<<8)|(src[o+14]<<16)|(src[o+15]<<24); \
}

#define Save16(a,b,c,d) \
{      \
 ((VO_U32*)dst)[0]=a;\
 ((VO_U32*)dst)[1]=b;\
 ((VO_U32*)dst)[2]=c;\
 ((VO_U32*)dst)[3]=d;\
}

static VO_U8 chromadx[8] = {0,2,2,2,2,2,2,2};
static VO_U8 chromady[8] = {0,1,1,1,1,1,1,1};


static const int COEF[6] = { 1, -5, 20, 20, -5, 1 };
void wp_2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
	wp_2_armv6(dst, dst_stride, wp_scale, wp_offset, weight_denom, height);    
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(rshift_rnd((wp_scale*dst[0]), weight_denom)+wp_offset);      
        dst[1] = Clip255(rshift_rnd((wp_scale*dst[1]), weight_denom)+wp_offset); 
		dst+=dst_stride;
	}
#endif		
}

void wbi_2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height)
{
  VO_S32 j;
#if  defined(VOARMV6)
	wbi_2_armv6(dst, dst_stride, src, src_stride, wp_scale_l1, wp_scale_l0, wp_offset, weight_denom, height);		
#else
  for(j = height; j != 0; j--)
  {    
    dst[0] = Clip255(rshift_rnd_sf((wp_scale_l1*src[0]+wp_scale_l0*dst[0]),weight_denom)+wp_offset);
    dst[1] = Clip255(rshift_rnd_sf((wp_scale_l1*src[1]+wp_scale_l0*dst[1]),weight_denom)+wp_offset); 
    dst+=dst_stride;
	src+=src_stride;
  }
#endif	
}

void weighted_mc_prediction_2x4(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_2(dst,dst_stride,wp_scale,wp_offset,weight_denom,4);
}
//end of add
//add by Really Yang 20110407
void weighted_mc_prediction_2x2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_2(dst,dst_stride,wp_scale,wp_offset,weight_denom,2);
}

void weighted_bi_prediction_2x2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_2(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,2);
}
void weighted_bi_prediction_2x4(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_2(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,4);
}

#if !defined(VOARMV7)

//add by Really Yang 20110413

//end of add
//add by Really Yang 20110413
void wp_4(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
    wp_4_armv6(dst, dst_stride, wp_scale, wp_offset, weight_denom, height);    
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(rshift_rnd((wp_scale*dst[0]), weight_denom)+wp_offset);      
        dst[1] = Clip255(rshift_rnd((wp_scale*dst[1]), weight_denom)+wp_offset);
		dst[2] = Clip255(rshift_rnd((wp_scale*dst[2]), weight_denom)+wp_offset);      
        dst[3] = Clip255(rshift_rnd((wp_scale*dst[3]), weight_denom)+wp_offset);
		dst+=dst_stride;
	}
#endif		
}
//end of add
//add by Really Yang 20110413
void wp_8(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
    wp_8_armv6(dst, dst_stride, wp_scale, wp_offset, weight_denom, height);    
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(rshift_rnd((wp_scale*dst[0]), weight_denom)+wp_offset);      
        dst[1] = Clip255(rshift_rnd((wp_scale*dst[1]), weight_denom)+wp_offset);
		dst[2] = Clip255(rshift_rnd((wp_scale*dst[2]), weight_denom)+wp_offset);      
        dst[3] = Clip255(rshift_rnd((wp_scale*dst[3]), weight_denom)+wp_offset);
		dst[4] = Clip255(rshift_rnd((wp_scale*dst[4]), weight_denom)+wp_offset);      
        dst[5] = Clip255(rshift_rnd((wp_scale*dst[5]), weight_denom)+wp_offset);
		dst[6] = Clip255(rshift_rnd((wp_scale*dst[6]), weight_denom)+wp_offset);      
        dst[7] = Clip255(rshift_rnd((wp_scale*dst[7]), weight_denom)+wp_offset);
		dst+=dst_stride;
	}
#endif	
}
//end of add
//add by Really Yang 20110413
void wp_16(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom,VO_S32 height)
{
	VO_S32 j,i;
#if  defined(VOARMV6)
    wp_16_armv6(dst, dst_stride, wp_scale, wp_offset, weight_denom, height);    
#else
	for(j = height;j != 0;j--)
	{
		for(i = 2;i != 0;i--)
		{
			dst[0] = Clip255(rshift_rnd((wp_scale*dst[0]), weight_denom)+wp_offset);      
	        dst[1] = Clip255(rshift_rnd((wp_scale*dst[1]), weight_denom)+wp_offset);
			dst[2] = Clip255(rshift_rnd((wp_scale*dst[2]), weight_denom)+wp_offset);      
	        dst[3] = Clip255(rshift_rnd((wp_scale*dst[3]), weight_denom)+wp_offset);
			dst[4] = Clip255(rshift_rnd((wp_scale*dst[4]), weight_denom)+wp_offset);      
	        dst[5] = Clip255(rshift_rnd((wp_scale*dst[5]), weight_denom)+wp_offset);
			dst[6] = Clip255(rshift_rnd((wp_scale*dst[6]), weight_denom)+wp_offset);      
	        dst[7] = Clip255(rshift_rnd((wp_scale*dst[7]), weight_denom)+wp_offset);
			dst+=8;
		}
		dst+=(dst_stride-16);
	}
#endif	
}

void of_2(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
	of_2_armv6(dst, dst_stride,  wp_offset,  height);	  
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(dst[0]+wp_offset);      
	    dst[1] = Clip255(dst[1]+wp_offset);
		dst+=dst_stride;
	}
#endif	
}

void of_4(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
	of_4_armv6(dst, dst_stride,  wp_offset,  height);	  
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(dst[0]+wp_offset);      
	    dst[1] = Clip255(dst[1]+wp_offset);
		dst[2] = Clip255(dst[2]+wp_offset);      
	    dst[3] = Clip255(dst[3]+wp_offset);
		dst+=dst_stride;
	}
#endif	
}

void of_8(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height)
{
	VO_S32 j;
#if  defined(VOARMV6)
	of_8_armv6(dst, dst_stride,  wp_offset,  height);	  
#else
	for(j = height;j != 0;j--)
	{
		dst[0] = Clip255(dst[0]+wp_offset);      
	    dst[1] = Clip255(dst[1]+wp_offset);
		dst[2] = Clip255(dst[2]+wp_offset);      
	    dst[3] = Clip255(dst[3]+wp_offset);
		dst[4] = Clip255(dst[4]+wp_offset);      
	    dst[5] = Clip255(dst[5]+wp_offset);
		dst[6] = Clip255(dst[6]+wp_offset);      
	    dst[7] = Clip255(dst[7]+wp_offset);
		dst+=dst_stride;
	}
#endif	
}

void of_16(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset,VO_S32 height)
{
	VO_S32 j,i;
#if  defined(VOARMV6)
    of_16_armv6(dst, dst_stride,  wp_offset,  height);    
#else
	for(j = height;j != 0;j--)
	{
		for(i = 2;i != 0;i--)
		{
			dst[0] = Clip255(dst[0]+wp_offset);      
	        dst[1] = Clip255(dst[1]+wp_offset);
			dst[2] = Clip255(dst[2]+wp_offset);      
	        dst[3] = Clip255(dst[3]+wp_offset);
			dst[4] = Clip255(dst[4]+wp_offset);      
	        dst[5] = Clip255(dst[5]+wp_offset);
			dst[6] = Clip255(dst[6]+wp_offset);      
	        dst[7] = Clip255(dst[7]+wp_offset);
			dst+=8;
		}
		dst+=(dst_stride-16);
	}
#endif	
}

void weighted_mc_prediction_16x16(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_16(dst,dst_stride,wp_scale,wp_offset,weight_denom,16);
}

void weighted_mc_prediction_16x8(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_16(dst,dst_stride,wp_scale,wp_offset,weight_denom,8);
}

void weighted_mc_prediction_8x16(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_8(dst,dst_stride,wp_scale,wp_offset,weight_denom,16);
}

void weighted_mc_prediction_8x8(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_8(dst,dst_stride,wp_scale,wp_offset,weight_denom,8);
}
//end of add
//add by Really Yang 20110407
void weighted_mc_prediction_8x4(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_8(dst,dst_stride,wp_scale,wp_offset,weight_denom,4);
}
//end of add
//add by Really Yang 20110407
void weighted_mc_prediction_4x8(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_4(dst,dst_stride,wp_scale,wp_offset,weight_denom,8);
}
//end of add
//add by Really Yang 20110407
void weighted_mc_prediction_4x4(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_4(dst,dst_stride,wp_scale,wp_offset,weight_denom,4);
}
//end of add
//add by Really Yang 20110407
void weighted_mc_prediction_4x2(VO_U8 *dst, VO_S32 dst_stride, VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom)
{
  wp_4(dst,dst_stride,wp_scale,wp_offset,weight_denom,2);
}
//end of add
//add by Really Yang 20110407

void offset_mc_prediction_16x16(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_16(dst,dst_stride,wp_offset,16);
}

void offset_mc_prediction_16x8(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_16(dst,dst_stride,wp_offset,8);
}

void offset_mc_prediction_8x16(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_8(dst,dst_stride,wp_offset,16);
}

void offset_mc_prediction_8x8(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_8(dst,dst_stride,wp_offset,8);
}

void offset_mc_prediction_8x4(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_8(dst,dst_stride,wp_offset,4);
}

void offset_mc_prediction_4x8(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_4(dst,dst_stride,wp_offset,8);
}

void offset_mc_prediction_4x4(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_4(dst,dst_stride,wp_offset,4);
}

void offset_mc_prediction_4x2(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_4(dst,dst_stride,wp_offset,2);
}

void offset_mc_prediction_2x4(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_2(dst,dst_stride,wp_offset,4);
}

void offset_mc_prediction_2x2(VO_U8 *dst, VO_S32 dst_stride,VO_S32 wp_offset)
{
  of_2(dst,dst_stride,wp_offset,2);
}

void bi_2(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height)
{
  VO_U32 j;
#if  defined(VOARMV6)
  bi_2_armv6(dst, dst_stride, src, src_stride, height);	
#else      
  for(j = height;j != 0;j--)
  {
    dst[0] = (src[0]+dst[0]+1) >> 1;
    dst[1] = (src[1]+dst[1]+1) >> 1;
    src+=src_stride;
	dst+=dst_stride;
  }
#endif	
}

void bi_4(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height)
{
  VO_U32 j;
  VO_S32 a,b,c,d;
#if  defined(VOARMV6)
   bi_4_armv6(dst, dst_stride, src, src_stride, height);  
#else      
  for(j = height;j != 0;j--)
  {
    a = (src[0]+dst[0]+1) >> 1;
    b = (src[1]+dst[1]+1) >> 1;
	c = (src[2]+dst[2]+1) >> 1;
    d = (src[3]+dst[3]+1) >> 1;
	((VO_U32*)dst)[0] = a|(b<<8)|(c<<16)|(d<<24);
    src+=src_stride;
	dst+=dst_stride;
  }
#endif	
}
//end of add
//add by Really Yang 20110414
void bi_8(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height)
{
  VO_U32 j;
  VO_S32 a,b,c,d,e,f;
#if  defined(VOARMV6)
	 bi_8_armv6(dst, dst_stride, src, src_stride, height); 	
#else    
  for(j = height;j != 0;j--)
  {
    a = (src[0]+dst[0]+1) >> 1;
    b = (src[1]+dst[1]+1) >> 1;
	c = (src[2]+dst[2]+1) >> 1;
    d = (src[3]+dst[3]+1) >> 1;
	e = a|(b<<8)|(c<<16)|(d<<24);
	a = (src[4]+dst[4]+1) >> 1;
    b = (src[5]+dst[5]+1) >> 1;
	c = (src[6]+dst[6]+1) >> 1;
    d = (src[7]+dst[7]+1) >> 1;
	f = a|(b<<8)|(c<<16)|(d<<24);
	((VO_U32*)dst)[0] = e;
	((VO_U32*)dst)[1] = f;
    src+=src_stride;
	dst+=dst_stride;
  }
#endif	
}

//end of add
//add by Really Yang 20110414
void bi_16(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 height)
{
  VO_U32 j,i;
  VO_S32 a,b,c,d,e,f;
#if  defined(VOARMV6)
   bi_16_armv6(dst, dst_stride, src, src_stride, height); 	  
#else  
  for(j = height;j != 0;j--)
  {
  	for(i = 2;i != 0;i--)
  	{
	  a = (src[0]+dst[0]+1) >> 1;
      b = (src[1]+dst[1]+1) >> 1;
	  c = (src[2]+dst[2]+1) >> 1;
      d = (src[3]+dst[3]+1) >> 1;
	  e = a|(b<<8)|(c<<16)|(d<<24);
	  a = (src[4]+dst[4]+1) >> 1;
      b = (src[5]+dst[5]+1) >> 1;
	  c = (src[6]+dst[6]+1) >> 1;
      d = (src[7]+dst[7]+1) >> 1;
	  f = a|(b<<8)|(c<<16)|(d<<24);
	  ((VO_U32*)dst)[0] = e;
	  ((VO_U32*)dst)[1] = f;
      src+=8;
	  dst+=8;
  	}
    src+=(src_stride-16);
	dst+=(dst_stride-16);  
  }
#endif	
}

void bi_prediction_16x16(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_16(dst,dst_stride,src,src_stride,16);
}

void bi_prediction_16x8(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_16(dst,dst_stride,src,src_stride,8);
}

void bi_prediction_8x16(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_8(dst,dst_stride,src,src_stride,16);
}

void bi_prediction_8x8(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_8(dst,dst_stride,src,src_stride,8);
}

void bi_prediction_8x4(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_8(dst,dst_stride,src,src_stride,4);
}

void bi_prediction_4x8(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_4(dst,dst_stride,src,src_stride,8);
}

void bi_prediction_4x4(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_4(dst,dst_stride,src,src_stride,4);
}

void bi_prediction_4x2(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_4(dst,dst_stride,src,src_stride,2);
}

void bi_prediction_2x4(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_2(dst,dst_stride,src,src_stride,4);
}

void bi_prediction_2x2(VO_U8 *dst,VO_S32 dst_stride,VO_U8* src,VO_S32 src_stride)
{
  bi_2(dst,dst_stride,src,src_stride,2);
}

//end of add
//#endif

void wbi_4(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height)
{
  VO_S32 j;
#if  defined(VOARMV6)
  wbi_4_armv6(dst, dst_stride, src, src_stride, wp_scale_l1, wp_scale_l0, wp_offset, weight_denom, height); 	  
#else
  for(j = height; j != 0; j--)
  {    
    dst[0] = Clip255(rshift_rnd_sf((wp_scale_l1*src[0]+wp_scale_l0*dst[0]),weight_denom)+wp_offset);
    dst[1] = Clip255(rshift_rnd_sf((wp_scale_l1*src[1]+wp_scale_l0*dst[1]),weight_denom)+wp_offset); 
	dst[2] = Clip255(rshift_rnd_sf((wp_scale_l1*src[2]+wp_scale_l0*dst[2]),weight_denom)+wp_offset);
    dst[3] = Clip255(rshift_rnd_sf((wp_scale_l1*src[3]+wp_scale_l0*dst[3]),weight_denom)+wp_offset); 
    dst+=dst_stride;
	src+=src_stride;
  }
#endif	
}
//end of add
//add by Really Yang 20110414
void wbi_8(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height)
{
  VO_S32 j;
#if  defined(VOARMV6)
	wbi_8_armv6(dst, dst_stride, src, src_stride, wp_scale_l1, wp_scale_l0, wp_offset, weight_denom, height);		
#else
  for(j = height; j != 0; j--)
  {    
    dst[0] = Clip255(rshift_rnd_sf((wp_scale_l1*src[0]+wp_scale_l0*dst[0]),weight_denom)+wp_offset);
    dst[1] = Clip255(rshift_rnd_sf((wp_scale_l1*src[1]+wp_scale_l0*dst[1]),weight_denom)+wp_offset); 
	dst[2] = Clip255(rshift_rnd_sf((wp_scale_l1*src[2]+wp_scale_l0*dst[2]),weight_denom)+wp_offset);
    dst[3] = Clip255(rshift_rnd_sf((wp_scale_l1*src[3]+wp_scale_l0*dst[3]),weight_denom)+wp_offset);
	dst[4] = Clip255(rshift_rnd_sf((wp_scale_l1*src[4]+wp_scale_l0*dst[4]),weight_denom)+wp_offset);
    dst[5] = Clip255(rshift_rnd_sf((wp_scale_l1*src[5]+wp_scale_l0*dst[5]),weight_denom)+wp_offset); 
	dst[6] = Clip255(rshift_rnd_sf((wp_scale_l1*src[6]+wp_scale_l0*dst[6]),weight_denom)+wp_offset);
    dst[7] = Clip255(rshift_rnd_sf((wp_scale_l1*src[7]+wp_scale_l0*dst[7]),weight_denom)+wp_offset);
    dst+=dst_stride;
	src+=src_stride;
  }
#endif	
}
//end of add
//add by Really Yang 20110414
void wbi_16(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, 
                    VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom,VO_S32 height)
{
  VO_S32 j,i;
#if  defined(VOARMV6)
  wbi_16_armv6(dst, dst_stride, src, src_stride, wp_scale_l1, wp_scale_l0, wp_offset, weight_denom, height);	  
#else
  for(j = height; j != 0; j--)
  { 
    for(i = 2; i != 0; i--)
    { 
      dst[0] = Clip255(rshift_rnd_sf((wp_scale_l1*src[0]+wp_scale_l0*dst[0]),weight_denom)+wp_offset);
      dst[1] = Clip255(rshift_rnd_sf((wp_scale_l1*src[1]+wp_scale_l0*dst[1]),weight_denom)+wp_offset); 
	  dst[2] = Clip255(rshift_rnd_sf((wp_scale_l1*src[2]+wp_scale_l0*dst[2]),weight_denom)+wp_offset);
      dst[3] = Clip255(rshift_rnd_sf((wp_scale_l1*src[3]+wp_scale_l0*dst[3]),weight_denom)+wp_offset);
	  dst[4] = Clip255(rshift_rnd_sf((wp_scale_l1*src[4]+wp_scale_l0*dst[4]),weight_denom)+wp_offset);
      dst[5] = Clip255(rshift_rnd_sf((wp_scale_l1*src[5]+wp_scale_l0*dst[5]),weight_denom)+wp_offset); 
	  dst[6] = Clip255(rshift_rnd_sf((wp_scale_l1*src[6]+wp_scale_l0*dst[6]),weight_denom)+wp_offset);
      dst[7] = Clip255(rshift_rnd_sf((wp_scale_l1*src[7]+wp_scale_l0*dst[7]),weight_denom)+wp_offset);
	  dst+=8;
	  src+=8;
    }
    dst+=(dst_stride-16);
	src+=(src_stride-16);
  }
#endif	
}
//end of add
//add by Really Yang 20110414

void weighted_bi_prediction_4x2(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_4(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,2);
}
void weighted_bi_prediction_4x4(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_4(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,4);
}
void weighted_bi_prediction_4x8(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_4(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,8);
}
void weighted_bi_prediction_8x4(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_8(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,4);
}
void weighted_bi_prediction_8x8(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_8(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,8);
}
void weighted_bi_prediction_8x16(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_8(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,16);
}
void weighted_bi_prediction_16x8(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_16(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,8);
}
void weighted_bi_prediction_16x16(VO_U8 *dst,VO_S32 dst_stride, VO_U8 *src,VO_S32 src_stride, 
                                   VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom)
{
	wbi_16(dst,dst_stride,src,src_stride,wp_scale_l0,wp_scale_l1,(wp_offset+1)>>1,weight_denom+1,16);
}
#endif

void get_block_00_2x2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy2x2(src, src_stride, dst, dst_stride);
#else

  VO_U16 a;
  
  Load2(a,0);
  Save2(a);
  dst += dst_stride;
  src += src_stride;
  Load2(a,0);
  Save2(a);
#endif  
}
//end of add
//add by Really Yang 20110412
void get_block_00_2x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy2x4(src, src_stride, dst, dst_stride);
#else

  VO_U16 a;
  
  Load2(a,0);
  Save2(a);
  dst += dst_stride;
  src += src_stride;
  Load2(a,0);
  Save2(a);
  dst += dst_stride;
  src += src_stride;
  Load2(a,0);
  Save2(a);
  dst += dst_stride;
  src += src_stride;
  Load2(a,0);
  Save2(a);
#endif
}
//end of add


//#if !defined(VOARMV7)
//add by Really Yang 20110412
static void get_block_00_4x2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy4x2(src, src_stride, dst, dst_stride);
#else

  VO_U32 a;
  
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
#endif  
}
//end of add
static void get_block_00_4x4_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy4x4(src, src_stride, dst, dst_stride);
#elif defined(VOARMV6)
  get_luma_00_4_ARMV6(src, src_stride, dst, dst_stride); 
#else
  VO_U32 a;
  
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_4x8_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy4x8(src, src_stride, dst, dst_stride);
#elif defined(VOARMV6)
  get_luma_00_4x8_ARMV6(src, src_stride, dst, dst_stride); 
#else
  get_block_00_4x4_chroma(src,src_stride,dst,dst_stride,0,0,0,0);
  dst += (dst_stride<<2);
  src += (src_stride<<2);
  get_block_00_4x4_chroma(src,src_stride,dst,dst_stride,0,0,0,0);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_8x4_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy8x4(src, src_stride, dst, dst_stride);
#elif defined(VOARMV6)
  get_luma_00_8x4_ARMV6(src, src_stride, dst, dst_stride); 
#else
  VO_U32 a,b;
  
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);  
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_8x8_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7)
  MCCopy8x8(src, src_stride, dst, dst_stride);
#elif defined(VOARMV6)
  get_luma_00_8x8_ARMV6(src, src_stride, dst, dst_stride); 
#else
  get_block_00_8x4_chroma(src,src_stride,dst,dst_stride,0,0,0,0);
  dst += (dst_stride<<2);
  src += (src_stride<<2);
  get_block_00_8x4_chroma(src,src_stride,dst,dst_stride,0,0,0,0);
#endif
}
#if !defined(VOARMV7)
//add by Really Yang 20110412
static void get_block_00_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_4_ARMV6(src, src_stride, dst, dst_stride); 
#else
  VO_U32 a;
  
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
  dst += dst_stride;
  src += src_stride;
  Load4(a,0);
  Save4(a);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_4x8_ARMV6(src, src_stride, dst, dst_stride); 
#else
  get_block_00_4x4(src,src_stride,dst,dst_stride);
  dst += (dst_stride<<2);
  src += (src_stride<<2);
  get_block_00_4x4(src,src_stride,dst,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_8x4_ARMV6(src, src_stride, dst, dst_stride); 
#else
  VO_U32 a,b;
  
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);
  dst += dst_stride;
  src += src_stride;
  Load8(a,b,0);
  Save8(a,b);  
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_8x8_ARMV6(src, src_stride, dst, dst_stride); 
#else
  get_block_00_8x4(src,src_stride,dst,dst_stride);
  dst += (dst_stride<<2);
  src += (src_stride<<2);
  get_block_00_8x4(src,src_stride,dst,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_8x16_ARMV6(src, src_stride, dst, dst_stride);
#else
  get_block_00_8x8(src,src_stride,dst,dst_stride);
  dst += (dst_stride<<3);
  src += (src_stride<<3);
  get_block_00_8x8(src,src_stride,dst,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
static void get_block_00_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_16x8_ARMV6(src, src_stride, dst, dst_stride);
#else
  VO_U32 a,b,c,d;
  VO_U32 i;
  for(i = 2;i != 0;i-- )
  {
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
	dst += dst_stride;
  	src += src_stride;
  }
#endif
}
//end of add
//add by Really Yang 20110412
void get_block_00_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
  get_luma_00_16x16_ARMV6(src, src_stride, dst, dst_stride);
#else
  VO_U32 a,b,c,d;
  VO_U32 i;
  for(i = 4;i != 0;i-- )
  {
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
  	dst += dst_stride;
  	src += src_stride;
  	Load16(a,b,c,d,0);
  	Save16(a,b,c,d);
	dst += dst_stride;
  	src += src_stride;
  }
#endif

}



#endif

//end of add
void add_block_00_2x2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG2x2_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_2(dst,dst_stride,src,src_stride,2);
#endif
}

void add_block_00_2x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG2x4_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_2(dst,dst_stride,src,src_stride,4);
#endif
}

void add_block_00_4x2(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG4x2_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_4(dst,dst_stride,src,src_stride,2);
#endif
}
void add_block_00_4x4_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG4x4_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_4(dst,dst_stride,src,src_stride,4);
#endif
}

void add_block_00_4x8_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG4x8_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_4(dst,dst_stride,src,src_stride,8);
#endif
}

void add_block_00_8x4_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG8x4_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_8(dst,dst_stride,src,src_stride,4);
#endif
}

void add_block_00_8x8_chroma(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV7) 
  AVG8x8_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_8(dst,dst_stride,src,src_stride,8);
#endif
}

void add_block_00_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG4x4_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_4(dst,dst_stride,src,src_stride,4);
#endif
}

void add_block_00_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG4x8_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_4(dst,dst_stride,src,src_stride,8);
#endif
}

void add_block_00_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG8x4_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_8(dst,dst_stride,src,src_stride,4);
#endif
}

void add_block_00_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG8x8_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_8(dst,dst_stride,src,src_stride,8);
#endif
}

void add_block_00_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG8x16_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_8(dst,dst_stride,src,src_stride,16);
#endif
}

void add_block_00_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG16x8_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_16(dst,dst_stride,src,src_stride,8);
#endif
}

void add_block_00_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
  AVG16x16_ARMV7(dst,dst_stride,src,src_stride);
#else
  bi_16(dst,dst_stride,src,src_stride,16);
#endif
}

#if !defined(VOARMV7) && !defined(VOARMV6)

//add by Really Yang 20110412
void avg_get_luma_h_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {	
    dst[0]  = (plus[0]+Clip255(((src[-2]+src[3])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
    dst[1]  = (plus[1]+Clip255(((src[-1]+src[4])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
	dst[2]  = (plus[2]+Clip255(((src[0 ]+src[5])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
    dst[3]  = (plus[3]+Clip255(((src[1 ]+src[6])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
	src += src_stride;
	dst += dst_stride;
	plus += plus_stride;
  }
}
//end of add
//add by Really Yang 20110412
void avg_get_luma_h_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {	
    dst[0]  = (plus[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
    dst[1]  = (plus[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
	dst[2]  = (plus[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
    dst[3]  = (plus[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
	dst[4]  = (plus[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1;
    dst[5]  = (plus[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1;
	dst[6]  = (plus[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1;
    dst[7]  = (plus[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1;
	src += src_stride;
	dst += dst_stride;
	plus += plus_stride;
  }
}
//end of add
//add by Really Yang 20110412
void avg_get_luma_h_16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j,i;
  
  for (j = height; j != 0; j--)
  {
	for(i = 2;i !=0;i--)
	{
    	dst[0]=(plus[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
    	dst[1]=(plus[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
		dst[2]=(plus[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
	    dst[3]=(plus[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
		dst[4]=(plus[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1;
	    dst[5]=(plus[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1;
		dst[6]=(plus[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1;
	    dst[7]=(plus[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1;
		src+=8;
		dst+=8;
		plus+=8;
	}
	src = src+src_stride-16;
	dst = dst+dst_stride-16;
	plus = plus+plus_stride-16;
  }
}

void avg_add_luma_h_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {	
    dst[0]  = (dst[0]+((plus[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1)+1)>>1;
    dst[1]  = (dst[1]+((plus[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1)+1)>>1;
	dst[2]  = (dst[2]+((plus[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1)+1)>>1;
    dst[3]  = (dst[3]+((plus[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1)+1)>>1;
	src += src_stride;
	dst += dst_stride;
	plus += plus_stride;
  }
}

void avg_add_luma_h_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {	
    dst[0]  = (dst[0]+((plus[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1)+1)>>1;
    dst[1]  = (dst[1]+((plus[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1)+1)>>1;
	dst[2]  = (dst[2]+((plus[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1)+1)>>1;
    dst[3]  = (dst[3]+((plus[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1)+1)>>1;
	dst[4]  = (dst[4]+((plus[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1)+1)>>1;
    dst[5]  = (dst[5]+((plus[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1)+1)>>1;
	dst[6]  = (dst[6]+((plus[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1)+1)>>1;
    dst[7]  = (dst[7]+((plus[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1)+1)>>1;
	src += src_stride;
	dst += dst_stride;
	plus += plus_stride;
  }
}

void avg_add_luma_h_16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 height)
{
  VO_U32 j,i;
  
  for (j = height; j != 0; j--)
  {
	for(i = 2;i !=0;i--)
	{
    	dst[0]=(dst[0]+((plus[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1)+1)>>1;
    	dst[1]=(dst[1]+((plus[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1)+1)>>1;
		dst[2]=(dst[2]+((plus[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1)+1)>>1;
	    dst[3]=(dst[3]+((plus[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1)+1)>>1;
		dst[4]=(dst[4]+((plus[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1)+1)>>1;
	    dst[5]=(dst[5]+((plus[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1)+1)>>1;
		dst[6]=(dst[6]+((plus[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1)+1)>>1;
	    dst[7]=(dst[7]+((plus[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1)+1)>>1;
		src+=8;
		dst+=8;
		plus+=8;
	}
	src = src+src_stride-16;
	dst = dst+dst_stride-16;
	plus = plus+plus_stride-16;
  }
}

//end of add
//add by Really Yang 20110412
void avg_get_luma_v_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	
    dst[0*dst_stride]=(plus[0*plus_stride]+Clip255(((src_2+src3)-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1;
    dst[1*dst_stride]=(plus[1*plus_stride]+Clip255(((src_1+src4)-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1;
	dst[2*dst_stride]=(plus[2*plus_stride]+Clip255(((src0 +src5)-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1;
    dst[3*dst_stride]=(plus[3*plus_stride]+Clip255(((src1 +src6)-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1;
	src++;
	dst++;
	plus++;
  }
}
//end of add
//add by Really Yang 20110412
void avg_get_luma_v_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	src7  = src[7*src_stride];
	src8  = src[8*src_stride];
	src9  = src[9*src_stride];
	src10 = src[10*src_stride];
	
    dst[0*dst_stride]=(plus[0*plus_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1;
    dst[1*dst_stride]=(plus[1*plus_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1;
	dst[2*dst_stride]=(plus[2*plus_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1;
    dst[3*dst_stride]=(plus[3*plus_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1;
	dst[4*dst_stride]=(plus[4*plus_stride]+Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+16)>>5)+1)>>1;
    dst[5*dst_stride]=(plus[5*plus_stride]+Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+16)>>5)+1)>>1;
	dst[6*dst_stride]=(plus[6*plus_stride]+Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+16)>>5)+1)>>1;
    dst[7*dst_stride]=(plus[7*plus_stride]+Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+16)>>5)+1)>>1;
	src++;
	dst++;
	plus++;
  }
}

void avg_add_luma_v_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+((plus[0*plus_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+((plus[1*plus_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+((plus[2*plus_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+((plus[3*plus_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1)+1)>>1;
	src++;
	dst++;
	plus++;
  }
}

void avg_add_luma_v_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_U8 *plus,VO_S32 plus_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	src7  = src[7*src_stride];
	src8  = src[8*src_stride];
	src9  = src[9*src_stride];
	src10 = src[10*src_stride];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+((plus[0*plus_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+((plus[1*plus_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+((plus[2*plus_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+((plus[3*plus_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1)+1)>>1;
	dst[4*dst_stride]=(dst[4*dst_stride]+((plus[4*plus_stride]+Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+16)>>5)+1)>>1)+1)>>1;
    dst[5*dst_stride]=(dst[5*dst_stride]+((plus[5*plus_stride]+Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+16)>>5)+1)>>1)+1)>>1;
	dst[6*dst_stride]=(dst[6*dst_stride]+((plus[6*plus_stride]+Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+16)>>5)+1)>>1)+1)>>1;
    dst[7*dst_stride]=(dst[7*dst_stride]+((plus[7*plus_stride]+Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+16)>>5)+1)>>1)+1)>>1;
	src++;
	dst++;
	plus++;
  }
}

//end of add
//add by Really Yang 20110412
void get_luma_h_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {	
    dst[0]  = Clip255(((src[-2]+src[3])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5);
    dst[1]  = Clip255(((src[-1]+src[4])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5);
	dst[2]  = Clip255(((src[0 ]+src[5])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5);
    dst[3]  = Clip255(((src[1 ]+src[6])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5);
	src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110412
void get_luma_h_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {
    dst[0]  = Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5);
    dst[1]  = Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5);
	dst[2]  = Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5);
    dst[3]  = Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5);
	dst[4]  = Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5);
    dst[5]  = Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5);
	dst[6]  = Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5);
    dst[7]  = Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5);
	src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110412
void get_luma_h_16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j,i;
  
  for (j = height; j != 0; j--)
  {
	for(i = 2;i !=0;i--)
	{
    	dst[0]  = Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5);
	    dst[1]  = Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5);
		dst[2]  = Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5);
	    dst[3]  = Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5);
		dst[4]  = Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5);
	    dst[5]  = Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5);
		dst[6]  = Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5);
	    dst[7]  = Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5);
		src+=8;
		dst+=8;
	}
	src = src+src_stride-16;
	dst = dst+dst_stride-16;
  }
}
//end of add

void add_luma_h_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {
    dst[0]  = (dst[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
    dst[1]  = (dst[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
	dst[2]  = (dst[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
    dst[3]  = (dst[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
	src += src_stride;
	dst += dst_stride;
  }
}

void add_luma_h_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j;
  
  for (j = height; j != 0; j--)
  {
    dst[0]  = (dst[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
    dst[1]  = (dst[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
	dst[2]  = (dst[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
    dst[3]  = (dst[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
	dst[4]  = (dst[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1;
    dst[5]  = (dst[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1;
	dst[6]  = (dst[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1;
    dst[7]  = (dst[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1;
	src += src_stride;
	dst += dst_stride;
  }
}

void add_luma_h_16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 height)
{
  VO_U32 j,i;
  
  for (j = height; j != 0; j--)
  {
	for(i = 2;i !=0;i--)
	{
    	dst[0]  = (dst[0]+Clip255(((src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1])+16)>>5)+1)>>1;
	    dst[1]  = (dst[1]+Clip255(((src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2])+16)>>5)+1)>>1;
		dst[2]  = (dst[2]+Clip255(((src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3])+16)>>5)+1)>>1;
	    dst[3]  = (dst[3]+Clip255(((src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4])+16)>>5)+1)>>1;
		dst[4]  = (dst[4]+Clip255(((src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5])+16)>>5)+1)>>1;
	    dst[5]  = (dst[5]+Clip255(((src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6])+16)>>5)+1)>>1;
		dst[6]  = (dst[6]+Clip255(((src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7])+16)>>5)+1)>>1;
	    dst[7]  = (dst[7]+Clip255(((src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8])+16)>>5)+1)>>1;
		src+=8;
		dst+=8;
	}
	src = src+src_stride-16;
	dst = dst+dst_stride-16;
  }
}


//add by Really Yang 20110412
void get_luma_v_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	
    dst[0*dst_stride]=Clip255(((src_2+src3)-5*(src_1+src2)+20*(src0+src1)+16)>>5);
    dst[1*dst_stride]=Clip255(((src_1+src4)-5*(src0 +src3)+20*(src1+src2)+16)>>5);
	dst[2*dst_stride]=Clip255(((src0 +src5)-5*(src1 +src4)+20*(src2+src3)+16)>>5);
    dst[3*dst_stride]=Clip255(((src1 +src6)-5*(src2 +src5)+20*(src3+src4)+16)>>5);
	src++;
	dst++;
  }
}
//end of add
//add by Really Yang 20110412
void get_luma_v_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	src7  = src[7*src_stride];
	src8  = src[8*src_stride];
	src9  = src[9*src_stride];
	src10 = src[10*src_stride];
	
    dst[0*dst_stride]=Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5);
    dst[1*dst_stride]=Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5);
	dst[2*dst_stride]=Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5);
    dst[3*dst_stride]=Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5);
	dst[4*dst_stride]=Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+16)>>5);
    dst[5*dst_stride]=Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+16)>>5);
	dst[6*dst_stride]=Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+16)>>5);
    dst[7*dst_stride]=Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+16)>>5);
	src++;
	dst++;
  }
}

void add_luma_v_4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1;
	src++;
	dst++;
  }
}

void add_luma_v_8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 width)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  
  for (j = width; j != 0; j--)
  {
	src_2 = src[-2*src_stride];
	src_1 = src[-1*src_stride];
	src0  = src[0];
	src1  = src[1*src_stride];
	src2  = src[2*src_stride];
	src3  = src[3*src_stride];
	src4  = src[4*src_stride];
	src5  = src[5*src_stride];
	src6  = src[6*src_stride];
	src7  = src[7*src_stride];
	src8  = src[8*src_stride];
	src9  = src[9*src_stride];
	src10 = src[10*src_stride];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+16)>>5)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+16)>>5)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+16)>>5)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+16)>>5)+1)>>1;
	dst[4*dst_stride]=(dst[4*dst_stride]+Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+16)>>5)+1)>>1;
    dst[5*dst_stride]=(dst[5*dst_stride]+Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+16)>>5)+1)>>1;
	dst[6*dst_stride]=(dst[6*dst_stride]+Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+16)>>5)+1)>>1;
    dst[7*dst_stride]=(dst[7*dst_stride]+Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+16)>>5)+1)>>1;
	src++;
	dst++;
  }
}

//end of add
//add by Really Yang 20110412
void get_luma_c_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  VO_S16 tmp_buf[36];
  VO_S16* tmp = tmp_buf;
  src -= 2*src_stride;
  for (j = 9; j != 0; j--)
  {	
    tmp[0] = (src[-2]+src[3])-5*(src[-1]+src[2])+20*(src[0]+src[1]);
    tmp[1] = (src[-1]+src[4])-5*(src[0 ]+src[3])+20*(src[1]+src[2]);
	tmp[2] = (src[0 ]+src[5])-5*(src[1 ]+src[4])+20*(src[2]+src[3]);
    tmp[3] = (src[1 ]+src[6])-5*(src[2 ]+src[5])+20*(src[3]+src[4]);
	src += src_stride;
	tmp += 4;
  }
  tmp -= 28;
  for (j = 4; j != 0; j--)
  {
	src_2 = tmp[-2*4];
	src_1 = tmp[-1*4];
	src0  = tmp[0];
	src1  = tmp[1*4];
	src2  = tmp[2*4];
	src3  = tmp[3*4];
	src4  = tmp[4*4];
	src5  = tmp[5*4];
	src6  = tmp[6*4];
	
    dst[0*dst_stride]=Clip255(((src_2+src3)-5*(src_1+src2)+20*(src0+src1)+512)>>10);
    dst[1*dst_stride]=Clip255(((src_1+src4)-5*(src0 +src3)+20*(src1+src2)+512)>>10);
	dst[2*dst_stride]=Clip255(((src0 +src5)-5*(src1 +src4)+20*(src2+src3)+512)>>10);
    dst[3*dst_stride]=Clip255(((src1 +src6)-5*(src2 +src5)+20*(src3+src4)+512)>>10);
	tmp++;
	dst++;
  }
}
//end of add
//add by Really Yang 20110412
void get_luma_c_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  VO_S16 tmp_buf[104];
  VO_S16* tmp = tmp_buf;
  src -= 2*src_stride;
  for (j = 13; j != 0; j--)
  {	
    tmp[0] = (src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1]);
    tmp[1] = (src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2]);
	tmp[2] = (src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3]);
    tmp[3] = (src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4]);
	tmp[4] = (src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5]);
    tmp[5] = (src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6]);
	tmp[6] = (src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7]);
    tmp[7] = (src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8]);
	src += src_stride;
	tmp += 8;
  }
  tmp -= 88;
  for (j = 8; j != 0; j--)
  {
	src_2 = tmp[-2*8];
	src_1 = tmp[-1*8];
	src0  = tmp[0];
	src1  = tmp[1*8];
	src2  = tmp[2*8];
	src3  = tmp[3*8];
	src4  = tmp[4*8];
	src5  = tmp[5*8];
	src6  = tmp[6*8];
	src7  = tmp[7*8];
	src8  = tmp[8*8];
	src9  = tmp[9*8];
	src10 = tmp[10*8];
	
    dst[0*dst_stride]=Clip255(((src_2+src3)-5*(src_1+src2)+20*(src0+src1)+512)>>10);
    dst[1*dst_stride]=Clip255(((src_1+src4)-5*(src0 +src3)+20*(src1+src2)+512)>>10);
	dst[2*dst_stride]=Clip255(((src0 +src5)-5*(src1 +src4)+20*(src2+src3)+512)>>10);
    dst[3*dst_stride]=Clip255(((src1 +src6)-5*(src2 +src5)+20*(src3+src4)+512)>>10);
	dst[4*dst_stride]=Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+512)>>10);
    dst[5*dst_stride]=Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+512)>>10);
	dst[6*dst_stride]=Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+512)>>10);
    dst[7*dst_stride]=Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+512)>>10);
	tmp++;
	dst++;
  }
}

void add_luma_c_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6;
  VO_S16 tmp_buf[36];
  VO_S16* tmp = tmp_buf;
  src -= 2*src_stride;
  for (j = 9; j != 0; j--)
  {	
    tmp[0] = (src[-2]+src[3])-5*(src[-1]+src[2])+20*(src[0]+src[1]);
    tmp[1] = (src[-1]+src[4])-5*(src[0 ]+src[3])+20*(src[1]+src[2]);
	tmp[2] = (src[0 ]+src[5])-5*(src[1 ]+src[4])+20*(src[2]+src[3]);
    tmp[3] = (src[1 ]+src[6])-5*(src[2 ]+src[5])+20*(src[3]+src[4]);
	src += src_stride;
	tmp += 4;
  }
  tmp -= 28;
  for (j = 4; j != 0; j--)
  {
	src_2 = tmp[-2*4];
	src_1 = tmp[-1*4];
	src0  = tmp[0];
	src1  = tmp[1*4];
	src2  = tmp[2*4];
	src3  = tmp[3*4];
	src4  = tmp[4*4];
	src5  = tmp[5*4];
	src6  = tmp[6*4];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+Clip255(((src_2+src3)-5*(src_1+src2)+20*(src0+src1)+512)>>10)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+Clip255(((src_1+src4)-5*(src0 +src3)+20*(src1+src2)+512)>>10)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+Clip255(((src0 +src5)-5*(src1 +src4)+20*(src2+src3)+512)>>10)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+Clip255(((src1 +src6)-5*(src2 +src5)+20*(src3+src4)+512)>>10)+1)>>1;
	tmp++;
	dst++;
  }
}

void add_luma_c_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
  VO_U32 j;
  VO_S32 src_2,src_1,src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10;
  VO_S16 tmp_buf[104];
  VO_S16* tmp = tmp_buf;
  src -= 2*src_stride;
  for (j = 13; j != 0; j--)
  {	
    tmp[0] = (src[-2]+src[3 ])-5*(src[-1]+src[2])+20*(src[0]+src[1]);
    tmp[1] = (src[-1]+src[4 ])-5*(src[0 ]+src[3])+20*(src[1]+src[2]);
	tmp[2] = (src[0 ]+src[5 ])-5*(src[1 ]+src[4])+20*(src[2]+src[3]);
    tmp[3] = (src[1 ]+src[6 ])-5*(src[2 ]+src[5])+20*(src[3]+src[4]);
	tmp[4] = (src[2 ]+src[7 ])-5*(src[3 ]+src[6])+20*(src[4]+src[5]);
    tmp[5] = (src[3 ]+src[8 ])-5*(src[4 ]+src[7])+20*(src[5]+src[6]);
	tmp[6] = (src[4 ]+src[9 ])-5*(src[5 ]+src[8])+20*(src[6]+src[7]);
    tmp[7] = (src[5 ]+src[10])-5*(src[6 ]+src[9])+20*(src[7]+src[8]);
	src += src_stride;
	tmp += 8;
  }
  tmp -= 88;
  for (j = 8; j != 0; j--)
  {
	src_2 = tmp[-2*8];
	src_1 = tmp[-1*8];
	src0  = tmp[0];
	src1  = tmp[1*8];
	src2  = tmp[2*8];
	src3  = tmp[3*8];
	src4  = tmp[4*8];
	src5  = tmp[5*8];
	src6  = tmp[6*8];
	src7  = tmp[7*8];
	src8  = tmp[8*8];
	src9  = tmp[9*8];
	src10 = tmp[10*8];
	
    dst[0*dst_stride]=(dst[0*dst_stride]+Clip255(((src_2+src3 )-5*(src_1+src2)+20*(src0+src1)+512)>>10)+1)>>1;
    dst[1*dst_stride]=(dst[1*dst_stride]+Clip255(((src_1+src4 )-5*(src0 +src3)+20*(src1+src2)+512)>>10)+1)>>1;
	dst[2*dst_stride]=(dst[2*dst_stride]+Clip255(((src0 +src5 )-5*(src1 +src4)+20*(src2+src3)+512)>>10)+1)>>1;
    dst[3*dst_stride]=(dst[3*dst_stride]+Clip255(((src1 +src6 )-5*(src2 +src5)+20*(src3+src4)+512)>>10)+1)>>1;
	dst[4*dst_stride]=(dst[4*dst_stride]+Clip255(((src2 +src7 )-5*(src3 +src6)+20*(src4+src5)+512)>>10)+1)>>1;
    dst[5*dst_stride]=(dst[5*dst_stride]+Clip255(((src3 +src8 )-5*(src4 +src7)+20*(src5+src6)+512)>>10)+1)>>1;
	dst[6*dst_stride]=(dst[6*dst_stride]+Clip255(((src4 +src9 )-5*(src5 +src8)+20*(src6+src7)+512)>>10)+1)>>1;
    dst[7*dst_stride]=(dst[7*dst_stride]+Clip255(((src5 +src10)-5*(src6 +src9)+20*(src7+src8)+512)>>10)+1)>>1;
	tmp++;
	dst++;
  }
}

//end of add
#endif

//add by Really Yang 20110412
void get_luma_10_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride); 
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 0);
#else
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_10_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 0);
#else
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_10_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 0);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif		
}
//end of add
//add by Really Yang 20110412
void get_luma_10_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 0);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_10_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 0);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_10_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 0);
#else
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif
}
//end of add
void get_luma_10_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 0);
#else
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

void add_luma_10_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 0);
#else
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}

void add_luma_10_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 0);
#else
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}

void add_luma_10_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 0);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif
}

void add_luma_10_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 0);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif
}

void add_luma_10_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 0);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

void add_luma_10_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 0);
#else
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif
}

void add_luma_10_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 0);
#else
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

#if !defined(VOARMV7)
//add by Really Yang 20110412
void get_luma_20_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
    get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0);
#else
	get_luma_h_4(src,src_stride,dst,dst_stride,4);
#endif
}
//end of edit
//add by Really Yang 20110412
void get_luma_20_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0);
#else
	get_luma_h_4(src,src_stride,dst,dst_stride,8);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_20_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0);
#else
	get_luma_h_8(src,src_stride,dst,dst_stride,4);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_20_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0);
#else
	get_luma_h_8(src,src_stride,dst,dst_stride,8);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_20_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0);
#else
	get_luma_h_8(src,src_stride,dst,dst_stride,16);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_20_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0);
#else
	get_luma_h_16(src,src_stride,dst,dst_stride,8);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_20_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_20_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0);
#else
	get_luma_h_16(src,src_stride,dst,dst_stride,16);
#endif

}
//end of add

void add_luma_20_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0);
#else
	add_luma_h_4(src,src_stride,dst,dst_stride,4);
#endif
}

void add_luma_20_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0);
#else
	add_luma_h_4(src,src_stride,dst,dst_stride,8);
#endif
}

void add_luma_20_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0);
#else
	add_luma_h_8(src,src_stride,dst,dst_stride,4);
#endif
}

void add_luma_20_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0);
#else
	add_luma_h_8(src,src_stride,dst,dst_stride,8);
#endif
}

void add_luma_20_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0);
#else
	add_luma_h_8(src,src_stride,dst,dst_stride,16);
#endif
}

void add_luma_20_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0);
#else
	add_luma_h_16(src,src_stride,dst,dst_stride,8);
#endif
}

void add_luma_20_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_20_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0);
#else
	add_luma_h_16(src,src_stride,dst,dst_stride,16);
#endif
}

#endif

void get_luma_30_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_get_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 1);
#else
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,src+1,src_stride,4);
#endif		
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 1);
#else
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif		
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 1);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,4);
#endif	
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 1);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif	
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 1);
#else
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,16);
#endif	
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 1);
#else
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif	
}
//end of edit
//add by Really Yang 20110412
void get_luma_30_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    get_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 1);
#else
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,src+1,src_stride,16);
#endif
}
//end of edit

void add_luma_30_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 1);
#else
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,src+1,src_stride,4);
#endif		
}

void add_luma_30_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 1);
#else
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif		
}

void add_luma_30_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 1);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,4);
#endif	
}

void add_luma_30_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 1);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif	
}

void add_luma_30_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 1);
#else
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,src+1,src_stride,16);
#endif	
}

void add_luma_30_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 1);
#else
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,src+1,src_stride,8);
#endif	
}

void add_luma_30_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,src+1,src_stride);
#elif defined(VOARMV6)
    add_luma_10_30_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 1);
#else
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,src+1,src_stride,16);
#endif
}

void get_luma_01_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,4,0,-2,0);
#else
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_01_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,8,0,-2,0);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_01_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
       get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,4,0,-2,0);
#else
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_01_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
       get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,8,0,-2,0);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_01_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
       get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,16,0,-2,0);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_01_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
       get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,8,0,-2,0);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412

void get_luma_01_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,16,0,-2,0);
#else
  	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

void add_luma_01_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,4,0,-2,0);
#else
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}

void add_luma_01_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,8,0,-2,0);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,4);
#endif	
}

void add_luma_01_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,4,0,-2,0);
#else
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}

void add_luma_01_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,8,0,-2,0);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}


void add_luma_01_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,16,0,-2,0);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,8);
#endif	
}

void add_luma_01_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,8,0,-2,0);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

void add_luma_01_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride,src,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,16,0,-2,0);
#else
  	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src,src_stride,16);
#endif
}

#if !defined(VOARMV7)
//add by Really Yang 20110412
void get_luma_02_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 4, 4, 0, -2);
#else
  	get_luma_v_4(src,src_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_02_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 4, 8, 0, -2);
#else
  	get_luma_v_8(src,src_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_02_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 4, 0, -2);
#else
  	get_luma_v_4(src,src_stride,dst,dst_stride,8);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_02_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 8, 0, -2);
#else
  	get_luma_v_8(src,src_stride,dst,dst_stride,8);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_02_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 16, 0, -2);
#else
  	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_02_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 16, 8, 0, -2);
#else
  	get_luma_v_8(src,src_stride,dst,dst_stride,16);
#endif

}
//end of add
//add by Really Yang 20110412
void get_luma_02_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_02_ARMV6(src,src_stride,dst,dst_stride, 16, 16, 0, -2);
#else
  	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
#endif

}

void add_luma_02_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 4, 4, 0, -2);
#else
  	add_luma_v_4(src,src_stride,dst,dst_stride,4);
#endif  
}

void add_luma_02_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 4, 8, 0, -2);
#else
  	add_luma_v_8(src,src_stride,dst,dst_stride,4);
#endif
}

void add_luma_02_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 4, 0, -2);
#else

  	add_luma_v_4(src,src_stride,dst,dst_stride,8);
#endif
}

void add_luma_02_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 8, 0, -2);
#else

  	add_luma_v_8(src,src_stride,dst,dst_stride,8);
#endif
}


void add_luma_02_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 8, 16, 0, -2);
#else

  	add_luma_v_8(src,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	add_luma_v_8(src,src_stride,dst,dst_stride,8);
#endif
}

void add_luma_02_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 16, 8, 0, -2);
#else
  	add_luma_v_8(src,src_stride,dst,dst_stride,16);
#endif
}

void add_luma_02_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_02_ARMV6(src,src_stride,dst,dst_stride, 16, 16, 0, -2);
#else
  	add_luma_v_8(src,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	add_luma_v_8(src,src_stride,dst,dst_stride,16);
#endif
}

#endif

void get_luma_03_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,4,0,-2,1);
#else
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,src+src_stride,src_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_03_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,8,0,-2,1);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_03_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,4,0,-2,1);
#else
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_03_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,8,0,-2,1);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_03_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,16,0,-2,1);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_03_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,8,0,-2,1);
#else
  	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_03_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_get_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    get_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,16,0,-2,1);
#else
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
#endif	
}
//end of add

void add_luma_03_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,4,0,-2,1);
#else
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,src+src_stride,src_stride,4);
#endif	
}

void add_luma_03_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 4,8,0,-2,1);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,4);
#endif	
}

void add_luma_03_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,4,0,-2,1);
#else
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}

void add_luma_03_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,8,0,-2,1);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}


void add_luma_03_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	avg_add_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 8,16,0,-2,1);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,8);
#endif	
}

void add_luma_03_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,8,0,-2,1);
#else
  	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
#endif	
}

void add_luma_03_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	avg_add_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride,src+src_stride,src_stride);
#elif defined(VOARMV6)
    add_luma_01_03_ARMV6(src,src_stride,dst,dst_stride, 16,16,0,-2,1);
#else
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,src+src_stride,src_stride,16);
#endif	
}


//add by Really Yang 20110412
void get_luma_21_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_c_4x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	get_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 0);		
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 0);		
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 0);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_21_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 0);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add

void add_luma_21_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[16];
#if defined(VOARMV7)
	get_luma_c_4x4_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,4);
#endif
}

void add_luma_21_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_4x8_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
       add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	src += (src_stride << 2);
	get_luma_c_4x4(src,src_stride,tmp+16,4);
	src -= (src_stride << 2);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,8);
#endif
}

void add_luma_21_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_8x4_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,8);
	get_luma_c_4x4(src+4,src_stride,tmp+4,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,4);
#endif
}

void add_luma_21_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[64];
#if defined(VOARMV7)
	get_luma_c_8x8_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,8);
#endif
}

void add_luma_21_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_8x16_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+64,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,16);
#endif
}

void add_luma_21_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_16x8_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,8);
#endif
}

void add_luma_21_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
	VO_U8 tmp[256];
#if defined(VOARMV7)
	get_luma_c_16x16_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
    add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 0);	
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+128,16);
	get_luma_c_8x8(src+8,src_stride,tmp+128+8,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,16);
#endif
}


//add by Really Yang 20110412
#if !defined(VOARMV7)
void get_luma_22_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2);
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
#endif      				
}
//end of add
//add by Really Yang 20110412
void get_luma_22_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2);
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
#endif      					
}
//end of add
//add by Really Yang 20110412
void get_luma_22_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2);
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	get_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
#endif      					
}
//end of add
//add by Really Yang 20110412
void get_luma_22_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_22_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_22_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_22_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
     get_luma_22_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2);
//     get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
#endif
}

void add_luma_22_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2);
			//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_4x4(src,src_stride,dst,dst_stride);
#endif
}

void add_luma_22_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2);
			//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	add_luma_c_4x4(src,src_stride,dst,dst_stride);
#endif
}

void add_luma_22_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2);
			//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_4x4(src,src_stride,dst,dst_stride);
	add_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
#endif	
}

void add_luma_22_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
		add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2);
			//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
#endif
}

void add_luma_22_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
		add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2);
			//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
#endif
}

void add_luma_22_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2);
		//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
	add_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
#endif	
}

void add_luma_22_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6)
	add_luma_22_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2);
	//	   get_luma_22_wxy(src,src_stride,dst,dst_stride, 16, 16);
#else
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
	add_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	add_luma_c_8x8(src,src_stride,dst,dst_stride);
	add_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
#endif
}

//end of add
#endif


//add by Really Yang 20110412
void get_luma_23_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 1);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 1);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 1);		
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	get_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 1);		
#else	
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 1);		
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 1);		
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_23_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 1);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}

void add_luma_23_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[16];
#if defined(VOARMV7) 
	get_luma_c_4x4_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 1); 		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,4);
#endif
}

void add_luma_23_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_4x8_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 1); 		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	src += (src_stride << 2);
	get_luma_c_4x4(src,src_stride,tmp+16,4);
	src -= (src_stride << 2);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,8);
#endif
}

void add_luma_23_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_8x4_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 1); 		
#else
	get_luma_c_4x4(src,src_stride,tmp,8);
	get_luma_c_4x4(src+4,src_stride,tmp+4,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,4);
#endif
}

void add_luma_23_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[64];
#if defined(VOARMV7)
	get_luma_c_8x8_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 1); 		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,8);
#endif
}

void add_luma_23_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_8x16_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 1); 		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+64,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,16);
#endif
}

void add_luma_23_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_16x8_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 1); 		
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,8);
#endif
}

void add_luma_23_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7)
	get_luma_c_16x16_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_21_23_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 1); 	
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+128,16);
	get_luma_c_8x8(src+8,src_stride,tmp+128+8,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,16);
#endif
}

//end of add

//add by Really Yang 20110412
void get_luma_12_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 0);	
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_12_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 0);	
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_12_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 0);	
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	get_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
	avg_get_luma_v_4(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_12_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 0);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_12_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 0);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_12_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 0);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_12_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	get_luma_c_16x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 0);
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}

void add_luma_12_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[16];
#if defined(VOARMV7)
	get_luma_c_4x4_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,tmp,4,4);
#endif	
}

void add_luma_12_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,tmp+16,4);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp,4,4);
#endif	
}

void add_luma_12_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_8x4_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 0);		
#else
	get_luma_c_4x4(src,src_stride,tmp,8);
	get_luma_c_4x4(src+4,src_stride,tmp+4,8);
	avg_add_luma_v_4(src,src_stride,dst,dst_stride,tmp,8,8);
#endif	
}

void add_luma_12_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[64];
#if defined(VOARMV7)
	get_luma_c_8x8_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp,8,8);
#endif
}

void add_luma_12_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_8x16_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+64,8);
	src -= (src_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp,8,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp+64,8,8);
#endif
}

void add_luma_12_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_16x8_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 0);		
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp,16,16);
#endif	
}

void add_luma_12_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7)
	get_luma_c_16x16_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 0);	
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+128,16);
	get_luma_c_8x8(src+8,src_stride,tmp+128+8,16);
	src -= (src_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp,16,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src,src_stride,dst,dst_stride,tmp+128,16,16);
#endif	
}

//add by Really Yang 20110412
void get_luma_32_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_4x4_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
	get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 1);					
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	avg_get_luma_v_4(src+1,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_32_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_4x8_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
	get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 1);					
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_32_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x4_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
	get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 1);				
#else
	get_luma_c_4x4(src,src_stride,dst,dst_stride);
	get_luma_c_4x4(src+4,src_stride,dst+4,dst_stride);
	avg_get_luma_v_4(src+1,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_32_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x8_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
	get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 1);			
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_32_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x8_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_16x8_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
	get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 1);			
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_32_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_8x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_8x16_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
		get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 1);		
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_32_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_c_16x16_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_v_16x16_ARMV7(src+1,src_stride,dst,dst_stride);
#elif defined(VOARMV6)
    get_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 1);	
#else
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_c_8x8(src,src_stride,dst,dst_stride);
	get_luma_c_8x8(src+8,src_stride,dst+8,dst_stride);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_get_luma_v_8(src+1,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add

void add_luma_32_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
	VO_U8 tmp[16];
#if defined(VOARMV7)
	get_luma_c_4x4_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_v_4x4_ARMV7(src+1,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, 0, 1);			
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	avg_add_luma_v_4(src+1,src_stride,dst,dst_stride,tmp,4,4);
#endif	
}

void add_luma_32_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
	VO_U8 tmp[32];
#if defined(VOARMV7) 
	get_luma_c_4x8_ARMV7(src,src_stride,tmp,4);
	avg_add_luma_v_4x8_ARMV7(src+1,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, 0, 1);			
#else
	get_luma_c_4x4(src,src_stride,tmp,4);
	src += (src_stride << 2);
	dst += (dst_stride << 2);
	get_luma_c_4x4(src,src_stride,tmp+16,4);
	src -= (src_stride << 2);
	dst -= (dst_stride << 2);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp,4,4);
#endif	
}

void add_luma_32_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[32];
#if defined(VOARMV7)
	get_luma_c_8x4_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x4_ARMV7(src+1,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, 0, 1);			
#else
	get_luma_c_4x4(src,src_stride,tmp,8);
	get_luma_c_4x4(src+4,src_stride,tmp+4,8);
	avg_add_luma_v_4(src+1,src_stride,dst,dst_stride,tmp,8,8);
#endif	
}

void add_luma_32_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[64];
#if defined(VOARMV7)
	get_luma_c_8x8_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x8_ARMV7(src+1,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, 0, 1);			
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp,8,8);
#endif	
}

void add_luma_32_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7) 
	get_luma_c_8x16_ARMV7(src,src_stride,tmp,8);
	avg_add_luma_v_8x16_ARMV7(src+1,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, 0, 1);			
#else
	get_luma_c_8x8(src,src_stride,tmp,8);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+64,8);
	src -= (src_stride << 3);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp,8,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp+64,8,8);
#endif	
}

void add_luma_32_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[128];
#if defined(VOARMV7)
	get_luma_c_16x8_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_v_16x8_ARMV7(src+1,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, 0, 1);			
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp,16,16);
#endif	
}

void add_luma_32_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7) 
	get_luma_c_16x16_ARMV7(src,src_stride,tmp,16);
	avg_add_luma_v_16x16_ARMV7(src+1,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6)
	add_luma_12_32_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, 0, 1);		
#else
	get_luma_c_8x8(src,src_stride,tmp,16);
	get_luma_c_8x8(src+8,src_stride,tmp+8,16);
	src += (src_stride << 3);
	get_luma_c_8x8(src,src_stride,tmp+128,16);
	get_luma_c_8x8(src+8,src_stride,tmp+128+8,16);
	src -= (src_stride << 3);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp,16,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	avg_add_luma_v_8(src+1,src_stride,dst,dst_stride,tmp+128,16,16);
#endif	
}

//add by Really Yang 20110412
void get_luma_33_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_4x4_ARMV7(src+1,src_stride,dst,dst_stride);
    avg_dst_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 3);		
#else	
	get_luma_v_4(src+1,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_33_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_4x8_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 3);		
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_33_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
    get_luma_v_8x4_ARMV7(src+1,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 3);		
#else
	get_luma_v_4(src+1,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_33_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x8_ARMV7(src+1,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 3);		
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_33_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x16_ARMV7(src+1,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 3);		
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_33_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_16x8_ARMV7(src+1,src_stride,dst,dst_stride);
    avg_dst_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 3);		
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_33_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_16x16_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 3);	
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}

void add_luma_33_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[16];
#endif
#if defined(VOARMV7)
	get_luma_v_4x4_ARMV7(src+1,src_stride,tmp,4);
	avg_add_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 3, tmp);
#else
	get_luma_v_4(src+1,src_stride,tmp,4,4);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,4);
#endif
}

void add_luma_33_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[32];
#endif
#if defined(VOARMV7)
	get_luma_v_4x8_ARMV7(src+1,src_stride,tmp,4);
	avg_add_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 3, tmp);		
#else
	get_luma_v_8(src+1,src_stride,tmp,4,4);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,8);
#endif
}

void add_luma_33_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[32];
#endif
#if defined(VOARMV7)
    get_luma_v_8x4_ARMV7(src+1,src_stride,tmp,8);
    avg_add_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 3, tmp);		
#else
	get_luma_v_4(src+1,src_stride,tmp,8,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,4);
#endif	
}

void add_luma_33_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[64];
#endif
#if defined(VOARMV7)
    get_luma_v_8x8_ARMV7(src+1,src_stride,tmp,8);
    avg_add_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 3, tmp);		
#else
	get_luma_v_8(src+1,src_stride,tmp,8,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,8);
#endif	
}

void add_luma_33_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
    get_luma_v_8x16_ARMV7(src+1,src_stride,tmp,8);
    avg_add_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 3, tmp);		
#else
	get_luma_v_8(src+1,src_stride,tmp,8,8);
	src += (src_stride << 3);
	get_luma_v_8(src+1,src_stride,tmp+64,8,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,16);
#endif	
}

void add_luma_33_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
    get_luma_v_16x8_ARMV7(src+1,src_stride,tmp,16);
    avg_add_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 3, tmp);		
#else
	get_luma_v_8(src+1,src_stride,tmp,16,16);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,8);
#endif
}

void add_luma_33_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
	VO_U8 tmp[256];
#if defined(VOARMV7) 
	get_luma_v_16x16_ARMV7(src+1,src_stride,tmp,16);
	avg_add_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 3, tmp);	
#else
	get_luma_v_8(src+1,src_stride,tmp,16,16);
	src += (src_stride << 3);
	get_luma_v_8(src+1,src_stride,tmp+128,16,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,16);
#endif	
}

//end of add

//add by Really Yang 20110412
void get_luma_11_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 0);		
#else
	get_luma_v_4(src,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 0);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 0);		
#else
	get_luma_v_4(src,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 0);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 0);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 0);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_11_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
    get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 0);	
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add

void add_luma_11_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[16];
#endif
#if defined(VOARMV7)
    get_luma_v_4x4_ARMV7(src,src_stride,tmp,4);
    avg_add_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 0, tmp);				
#else
	get_luma_v_4(src,src_stride,tmp,4,4);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,4);
#endif
}

void add_luma_11_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[32];
#endif
#if defined(VOARMV7) 
    get_luma_v_4x8_ARMV7(src,src_stride,tmp,4);
    avg_add_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 0, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,4,4);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,8);
#endif
}

void add_luma_11_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[32];
#endif
#if defined(VOARMV7) 
    get_luma_v_8x4_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 0, tmp);				
#else
	get_luma_v_4(src,src_stride,tmp,8,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,4);
#endif
}

void add_luma_11_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[64];
#endif
#if defined(VOARMV7)
    get_luma_v_8x8_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 0, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,8,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,8);
#endif
}

void add_luma_11_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[128];
#endif
#if defined(VOARMV7) 
    get_luma_v_8x16_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 0, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,8,8);
	src += (src_stride << 3);
	get_luma_v_8(src,src_stride,tmp+64,8,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,16);
#endif
}

void add_luma_11_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[128];
#endif
#if defined(VOARMV7) 
    get_luma_v_16x8_ARMV7(src,src_stride,tmp,16);
    avg_add_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 0, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,16,16);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,8);
#endif
}

void add_luma_11_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7)
    get_luma_v_16x16_ARMV7(src,src_stride,tmp,16);
    avg_add_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 0, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,16,16);
	src += (src_stride << 3);
	get_luma_v_8(src,src_stride,tmp+128,16,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,16);
#endif
}


//add by Really Yang 20110412
void get_luma_13_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_4x4_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 2);		
#else
	get_luma_v_4(src,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_13_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_4x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
		get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 2);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_13_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x4_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 2);		
#else
	get_luma_v_4(src,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_13_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 2);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_13_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_8x16_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 2);		
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,8);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_13_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
    get_luma_v_16x8_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 2);	
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif	
}
//end of add
//add by Really Yang 20110412
void get_luma_13_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
    get_luma_v_16x16_ARMV7(src,src_stride,dst,dst_stride);
    avg_dst_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 2);	
#else
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src,src_stride,dst,dst_stride,16);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src+src_stride,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif	
}
//end of add

void add_luma_13_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[16];
#endif
#if defined(VOARMV7)
    get_luma_v_4x4_ARMV7(src,src_stride,tmp,4);
    avg_add_luma_h_4x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 2, tmp);
#else
	get_luma_v_4(src,src_stride,tmp,4,4);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,4);
#endif	
}

void add_luma_13_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[32];
#endif
#if defined(VOARMV7)
    get_luma_v_4x8_ARMV7(src,src_stride,tmp,4);
    avg_add_luma_h_4x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 2, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,4,4);
	avg_add_luma_h_4(src+src_stride,src_stride,dst,dst_stride,tmp,4,8);
#endif	
}

void add_luma_13_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[32];
#endif
#if defined(VOARMV7)
    get_luma_v_8x4_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x4_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 2, tmp);				
#else
	get_luma_v_4(src,src_stride,tmp,8,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,4);
#endif	
}

void add_luma_13_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[64];
#endif
#if defined(VOARMV7)
    get_luma_v_8x8_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 2, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,8,8);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,8);
#endif	
}

void add_luma_13_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else

    VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
    get_luma_v_8x16_ARMV7(src,src_stride,tmp,8);
    avg_add_luma_h_8x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 2, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,8,8);
	src += (src_stride << 3);
	get_luma_v_8(src,src_stride,tmp+64,8,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src+src_stride,src_stride,dst,dst_stride,tmp,8,16);
#endif	
}

void add_luma_13_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
    get_luma_v_16x8_ARMV7(src,src_stride,tmp,16);
    avg_add_luma_h_16x8_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 2, tmp);				
#else
	get_luma_v_8(src,src_stride,tmp,16,16);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,8);
#endif	
}

void add_luma_13_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7)
    get_luma_v_16x16_ARMV7(src,src_stride,tmp,16);
    avg_add_luma_h_16x16_ARMV7(src+src_stride,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 2, tmp);			
#else
	get_luma_v_8(src,src_stride,tmp,16,16);
	src += (src_stride << 3);
	get_luma_v_8(src,src_stride,tmp+128,16,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src+src_stride,src_stride,dst,dst_stride,tmp,16,16);
#endif	
}


//add by Really Yang 20110412
void get_luma_31_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_4x4_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 1);			
#else
	get_luma_v_4(src+1,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_4x8_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 1);			
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,4);
	avg_get_luma_h_4(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_8x4_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 1);			
#else
	get_luma_v_4(src+1,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,4);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7)
	get_luma_v_8x8_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 1);			
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_8x16_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 1);			
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src+1,src_stride,dst,dst_stride,8);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_8(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_16x8_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 1);			
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,8);
#endif
}
//end of add
//add by Really Yang 20110412
void get_luma_31_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV7) 
	get_luma_v_16x16_ARMV7(src+1,src_stride,dst,dst_stride);
	avg_dst_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride);
#elif defined(VOARMV6) 	
	get_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 1);		
#else
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	src += (src_stride << 3);
	dst += (dst_stride << 3);
	get_luma_v_8(src+1,src_stride,dst,dst_stride,16);
	src -= (src_stride << 3);
	dst -= (dst_stride << 3);
	avg_get_luma_h_16(src,src_stride,dst,dst_stride,dst,dst_stride,16);
#endif
}
//end of add

void add_luma_31_4x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[32];
#endif
#if defined(VOARMV7) 
	get_luma_v_4x4_ARMV7(src+1,src_stride,tmp,4);
	avg_add_luma_h_4x4_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 4, -2, -2, 1, tmp);
#else
	get_luma_v_4(src+1,src_stride,tmp,4,4);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,4);
#endif
}

void add_luma_31_4x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[32];
#endif
#if defined(VOARMV7) 
	get_luma_v_4x8_ARMV7(src+1,src_stride,tmp,4);
	avg_add_luma_h_4x8_ARMV7(src,src_stride,dst,dst_stride,tmp,4);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 4, 8, -2, -2, 1, tmp);			
#else
	get_luma_v_8(src+1,src_stride,tmp,4,4);
	avg_add_luma_h_4(src,src_stride,dst,dst_stride,tmp,4,8);
#endif
}

void add_luma_31_8x4(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[32];
#endif
#if defined(VOARMV7)
	get_luma_v_8x4_ARMV7(src+1,src_stride,tmp,8);
	avg_add_luma_h_8x4_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 4, -2, -2, 1, tmp);			
#else
	get_luma_v_4(src+1,src_stride,tmp,8,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,4);
#endif
}


void add_luma_31_8x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[64];
#endif
#if defined(VOARMV7)
	get_luma_v_8x8_ARMV7(src+1,src_stride,tmp,8);
	avg_add_luma_h_8x8_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 8, -2, -2, 1, tmp);			
#else
	get_luma_v_8(src+1,src_stride,tmp,8,8);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,8);
#endif
}

void add_luma_31_8x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
	VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
	get_luma_v_8x16_ARMV7(src+1,src_stride,tmp,8);
	avg_add_luma_h_8x16_ARMV7(src,src_stride,dst,dst_stride,tmp,8);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 8, 16, -2, -2, 1, tmp);			
#else
	get_luma_v_8(src+1,src_stride,tmp,8,8);
	src += (src_stride << 3);
	get_luma_v_8(src+1,src_stride,tmp+64,8,8);
	src -= (src_stride << 3);
	avg_add_luma_h_8(src,src_stride,dst,dst_stride,tmp,8,16);
#endif
}

void add_luma_31_16x8(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
#if defined(VOARMV6) 
	VO_U8 tmp[256];
#else
    VO_U8 tmp[128];
#endif
#if defined(VOARMV7)
	get_luma_v_16x8_ARMV7(src+1,src_stride,tmp,16);
	avg_add_luma_h_16x8_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 8, -2, -2, 1, tmp);			
#else
	get_luma_v_8(src+1,src_stride,tmp,16,16);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,8);
#endif
}

void add_luma_31_16x16(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride)
{
    VO_U8 tmp[256];
#if defined(VOARMV7)
	get_luma_v_16x16_ARMV7(src+1,src_stride,tmp,16);
	avg_add_luma_h_16x16_ARMV7(src,src_stride,dst,dst_stride,tmp,16);
#elif defined(VOARMV6) 	
	add_luma_11_13_31_33_ARMV6(src,src_stride,dst,dst_stride, 16, 16, -2, -2, 1, tmp);		
#else
	get_luma_v_8(src+1,src_stride,tmp,16,16);
	src += (src_stride << 3);
	get_luma_v_8(src+1,src_stride,tmp+128,16,16);
	src -= (src_stride << 3);
	avg_add_luma_h_16(src,src_stride,dst,dst_stride,tmp,16,16);
#endif
}


#if !defined(VOARMV7)

//add by Really Yang 20110413
void get_chroma_edge_2(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]),6);  
    src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110413
void get_chroma_edge_4(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]),6);  
	dst[2] = rshift_rnd_sf((w00*src[2]+w01*src1[2]),6);
    dst[3] = rshift_rnd_sf((w00*src[3]+w01*src1[3]),6);  
    src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110413
void get_chroma_edge_8(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]),6);  
	dst[2] = rshift_rnd_sf((w00*src[2]+w01*src1[2]),6);
    dst[3] = rshift_rnd_sf((w00*src[3]+w01*src1[3]),6);
	dst[4] = rshift_rnd_sf((w00*src[4]+w01*src1[4]),6);
    dst[5] = rshift_rnd_sf((w00*src[5]+w01*src1[5]),6);  
	dst[6] = rshift_rnd_sf((w00*src[6]+w01*src1[6]),6);
    dst[7] = rshift_rnd_sf((w00*src[7]+w01*src1[7]),6);
    src += src_stride;
	dst += dst_stride;
  }
}

void add_chroma_edge_2(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]),6)+1)>>1;  
    src += src_stride;
	dst += dst_stride;
  }
}

void add_chroma_edge_4(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]),6)+1)>>1;  
	dst[2] = (dst[2]+rshift_rnd_sf((w00*src[2]+w01*src1[2]),6)+1)>>1;
    dst[3] = (dst[3]+rshift_rnd_sf((w00*src[3]+w01*src1[3]),6)+1)>>1;
    src += src_stride;
	dst += dst_stride;
  }
}

void add_chroma_edge_8(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 height,VO_S32 plus_stride)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+plus_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]),6)+1)>>1;  
	dst[2] = (dst[2]+rshift_rnd_sf((w00*src[2]+w01*src1[2]),6)+1)>>1;
    dst[3] = (dst[3]+rshift_rnd_sf((w00*src[3]+w01*src1[3]),6)+1)>>1;
	dst[4] = (dst[4]+rshift_rnd_sf((w00*src[4]+w01*src1[4]),6)+1)>>1;
    dst[5] = (dst[5]+rshift_rnd_sf((w00*src[5]+w01*src1[5]),6)+1)>>1;  
	dst[6] = (dst[6]+rshift_rnd_sf((w00*src[6]+w01*src1[6]),6)+1)>>1;
    dst[7] = (dst[7]+rshift_rnd_sf((w00*src[7]+w01*src1[7]),6)+1)>>1;
    src += src_stride;
	dst += dst_stride;
  }
}

//end of add
//add by Really Yang 20110413
void get_chroma_in_2(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6);  
    src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110413
void get_chroma_in_4(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6);  
	dst[2] = rshift_rnd_sf((w00*src[2]+w01*src1[2]+w10*src[3]+w11*src1[3]),6);
    dst[3] = rshift_rnd_sf((w00*src[3]+w01*src1[3]+w10*src[4]+w11*src1[4]),6);  
    src += src_stride;
	dst += dst_stride;
  }
}
//end of add
//add by Really Yang 20110413
void get_chroma_in_8(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6);
    dst[1] = rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6);  
	dst[2] = rshift_rnd_sf((w00*src[2]+w01*src1[2]+w10*src[3]+w11*src1[3]),6);
    dst[3] = rshift_rnd_sf((w00*src[3]+w01*src1[3]+w10*src[4]+w11*src1[4]),6); 
	dst[4] = rshift_rnd_sf((w00*src[4]+w01*src1[4]+w10*src[5]+w11*src1[5]),6);
    dst[5] = rshift_rnd_sf((w00*src[5]+w01*src1[5]+w10*src[6]+w11*src1[6]),6);  
	dst[6] = rshift_rnd_sf((w00*src[6]+w01*src1[6]+w10*src[7]+w11*src1[7]),6);
    dst[7] = rshift_rnd_sf((w00*src[7]+w01*src1[7]+w10*src[8]+w11*src1[8]),6); 
    src += src_stride;
	dst += dst_stride;
  }
}
//end of add

void add_chroma_in_2(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6)+1)>>1;  
	src += src_stride;
	dst += dst_stride;
  }
}

void add_chroma_in_4(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6)+1)>>1;  
	dst[2] = (dst[2]+rshift_rnd_sf((w00*src[2]+w01*src1[2]+w10*src[3]+w11*src1[3]),6)+1)>>1;
    dst[3] = (dst[3]+rshift_rnd_sf((w00*src[3]+w01*src1[3]+w10*src[4]+w11*src1[4]),6)+1)>>1; 
	src += src_stride;
	dst += dst_stride;
  }
}

void add_chroma_in_8(VO_U8 *src, int src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11, VO_S32 height)
{
  VO_U8* src1;
  int j;
  for (j = height; j !=0; j--)
  {
    src1 = src+src_stride;

    dst[0] = (dst[0]+rshift_rnd_sf((w00*src[0]+w01*src1[0]+w10*src[1]+w11*src1[1]),6)+1)>>1;
    dst[1] = (dst[1]+rshift_rnd_sf((w00*src[1]+w01*src1[1]+w10*src[2]+w11*src1[2]),6)+1)>>1;  
	dst[2] = (dst[2]+rshift_rnd_sf((w00*src[2]+w01*src1[2]+w10*src[3]+w11*src1[3]),6)+1)>>1;
    dst[3] = (dst[3]+rshift_rnd_sf((w00*src[3]+w01*src1[3]+w10*src[4]+w11*src1[4]),6)+1)>>1; 
	dst[4] = (dst[4]+rshift_rnd_sf((w00*src[4]+w01*src1[4]+w10*src[5]+w11*src1[5]),6)+1)>>1;
    dst[5] = (dst[5]+rshift_rnd_sf((w00*src[5]+w01*src1[5]+w10*src[6]+w11*src1[6]),6)+1)>>1;  
	dst[6] = (dst[6]+rshift_rnd_sf((w00*src[6]+w01*src1[6]+w10*src[7]+w11*src1[7]),6)+1)>>1;
    dst[7] = (dst[7]+rshift_rnd_sf((w00*src[7]+w01*src1[7]+w10*src[8]+w11*src1[8]),6)+1)>>1; 
    src += src_stride;
	dst += dst_stride;
  }
}

void get_chroma_0X_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 2, 2);
#else
	get_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w01,2,src_stride);
#endif
}

void get_chroma_0X_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 2, 4);
#else
	get_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif
}

void get_chroma_0X_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 2);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,2,src_stride);
#endif
}

void get_chroma_0X_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 4);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif
}

void get_chroma_0X_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 8);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,8,src_stride);
#endif
}

void get_chroma_0X_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 8, 4);
#else
	get_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif 
}

void get_chroma_0X_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 8, 8);
#else
	get_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w01,8,src_stride);
#endif
}

void add_chroma_0X_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 2, 2);
#else
	add_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w01,2,src_stride);
#endif
}

void add_chroma_0X_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 2, 4);
#else
	add_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif
}

void add_chroma_0X_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 2);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,2,src_stride);
#endif
}

void add_chroma_0X_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 4);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif
}

void add_chroma_0X_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 4, 8);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w01,8,src_stride);
#endif
}

void add_chroma_0X_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 8, 4);
#else
	add_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w01,4,src_stride);
#endif
}

void add_chroma_0X_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_0X_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w01, 8, 8);
#else
	add_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w01,8,src_stride);
#endif
}

void get_chroma_X0_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 2, 2);
#else
	get_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w10,2,1);
#endif
}

void get_chroma_X0_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 2, 4);
#else
	get_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void get_chroma_X0_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
		get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 2);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,2,1);
#endif
}

void get_chroma_X0_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
		get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 4);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void get_chroma_X0_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 8);
#else
	get_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,8,1);
#endif
}

void get_chroma_X0_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 8, 4);
#else
	get_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void get_chroma_X0_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 8, 8);
#else
	get_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w10,8,1);
#endif
}

void add_chroma_X0_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 2, 2);
#else
	add_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w10,2,1);
#endif
}

void add_chroma_X0_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
    add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 2, 4);
#else
	add_chroma_edge_2(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void add_chroma_X0_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 2);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,2,1);
#endif
}

void add_chroma_X0_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 4);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void add_chroma_X0_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 4, 8);
#else
	add_chroma_edge_4(src,src_stride,dst,dst_stride,w00,w10,8,1);
#endif
}

void add_chroma_X0_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 8, 4);
#else
	add_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w10,4,1);
#endif
}

void add_chroma_X0_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_X0_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, 8, 8);
#else
	add_chroma_edge_8(src,src_stride,dst,dst_stride,w00,w10,8,1);
#endif
}

void get_chroma_XX_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 2, 2);
#else
	get_chroma_in_2(src,src_stride,dst,dst_stride,w00,w01,w10,w11,2);
#endif
}

void get_chroma_XX_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 2, 4);
#else
	get_chroma_in_2(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void get_chroma_XX_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 2);
#else
	get_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,2);
#endif
}

void get_chroma_XX_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 4);
#else
	get_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void get_chroma_XX_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 8);
#else
	get_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,8);
#endif
}

void get_chroma_XX_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 8, 4);
#else
	get_chroma_in_8(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void get_chroma_XX_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	get_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 8, 8);
#else
	get_chroma_in_8(src,src_stride,dst,dst_stride,w00,w01,w10,w11,8);
#endif
}

void add_chroma_XX_2x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 2, 2);
#else
	add_chroma_in_2(src,src_stride,dst,dst_stride,w00,w01,w10,w11,2);
#endif
}

void add_chroma_XX_2x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 2, 4);
#else
	add_chroma_in_2(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void add_chroma_XX_4x2(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 2);
#else
	add_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,2);
#endif
}

void add_chroma_XX_4x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 4);
#else
	add_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void add_chroma_XX_4x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 4, 8);
#else
	add_chroma_in_4(src,src_stride,dst,dst_stride,w00,w01,w10,w11,8);
#endif
}

void add_chroma_XX_8x4(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 8, 4);
#else
	add_chroma_in_8(src,src_stride,dst,dst_stride,w00,w01,w10,w11,4);
#endif
}

void add_chroma_XX_8x8(VO_U8 *src, VO_S32 src_stride ,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11)
{
#if defined(VOARMV6)
	add_chroma_XX_ARMV6(src,src_stride,dst,dst_stride, w00, w01, w10, w11, 8, 8);
#else
	add_chroma_in_8(src,src_stride,dst,dst_stride,w00,w01,w10,w11,8);
#endif
}

#endif
static VO_S32 W00[64] = 
{
  64,56,48,40,32,24,16, 8, //dy 0
  56,49,42,35,28,21,14, 7, //dy 1
  48,42,36,30,24,18,12, 6, //dy 2
  40,35,30,25,20,15,10, 5, //dy 3
  32,28,24,20,16,12, 8, 4, //dy 4
  24,21,18,15,12, 9, 6, 3, //dy 5
  16,14,12,10, 8, 6, 4, 2, //dy 6
   8, 7, 6, 5, 4, 3, 2, 1  //dy 7
};

static VO_S32 W01[64] = 
{
   0, 0, 0, 0, 0, 0, 0, 0, //dy 0
   8, 7, 6, 5, 4, 3, 2, 1, //dy 1
  16,14,12,10, 8, 6, 4, 2, //dy 2 
  24,21,18,15,12, 9, 6, 3, //dy 3
  32,28,24,20,16,12, 8, 4, //dy 4
  40,35,30,25,20,15,10, 5, //dy 5
  48,42,36,30,24,18,12, 6, //dy 6
  56,49,42,35,28,21,14, 7  //dy 7
};

static VO_S32 W10[64] = 
{
   0, 8,16,24,32,40,48,56, //dy 0
   0, 7,14,21,28,35,42,49, //dy 1
   0, 6,12,18,24,30,36,42, //dy 2 
   0, 5,10,15,20,25,30,35, //dy 3
   0, 4, 8,12,16,20,24,28, //dy 4
   0, 3, 6, 9,12,15,18,21, //dy 5
   0, 2, 4, 6, 8,10,12,14, //dy 6
   0, 1, 2, 3, 4, 5, 6, 7  //dy 7
};

static VO_S32 W11[64] = 
{
   0, 0, 0, 0, 0, 0, 0, 0, //dy 0
   0, 1, 2, 3, 4, 5, 6, 7, //dy 1
   0, 2, 4, 6, 8,10,12,14, //dy 2
   0, 3, 6, 9,12,15,18,21, //dy 3
   0, 4, 8,12,16,20,24,28, //dy 4
   0, 5,10,15,20,25,30,35, //dy 5
   0, 6,12,18,24,30,36,42, //dy 6
   0, 7,14,21,28,35,42,49  //dy 7
};

void get_edge(VO_U8 *buf, VO_U8 *src, VO_S32 stride, VO_S32 in_w, VO_S32 in_h,
                VO_S32 pic_x, VO_S32 pic_y, VO_S32 width, VO_S32 height)
{
    VO_S32 x, y;
    VO_S32 starty, startx, endy, endx;

    if(pic_y>= height){
        src+= (height-1-pic_y)*stride;
        pic_y=height-1;
    }else if(pic_y<=-in_h){
        src+= (1-in_h-pic_y)*stride;
        pic_y=1-in_h;
    }
    if(pic_x>= width){
        src+= (width-1-pic_x);
        pic_x=width-1;
    }else if(pic_x<=-in_w){
        src+= (1-in_w-pic_x);
        pic_x=1-in_w;
    }

    starty= imax(0, -pic_y);
    startx= imax(0, -pic_x);
    endy= imin(in_h, height-pic_y);
    endx= imin(in_w, width-pic_x);

    // copy existing part
    for(y=starty; y<endy; y++){
        for(x=startx; x<endx; x++){
            buf[x + y*stride]= src[x + y*stride];
        }
    }

    //top
    for(y=0; y<starty; y++){
        for(x=startx; x<endx; x++){
            buf[x + y*stride]= buf[x + starty*stride];
        }
    }

    //bottom
    for(y=endy; y<in_h; y++){
        for(x=startx; x<endx; x++){
            buf[x + y*stride]= buf[x + (endy-1)*stride];
        }
    }

    for(y=0; y<in_h; y++){
       //left
        for(x=0; x<startx; x++){
            buf[x + y*stride]= buf[startx + y*stride];
        }

       //right
        for(x=endx; x<in_w; x++){
            buf[x + y*stride]= buf[endx - 1 + y*stride];
        }
    }
}


void perform_mc_single(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int pred_dir, int i, int j,int size_mode)
{
  //VideoParameters *p_Vid = currMB->p_Vid;  
  Slice *currSlice = pDecLocal->p_Slice;

  int ioff = (i<<4)+pDecLocal->mb.x*64;
  int joff = (j<<4)+(pDecLocal->mb.y>>pDecLocal->mb_field)*64;  

  VO_S32 cache_index = pDecLocal->mb_xy*16 + j*4 + i;
  VO_S32 ref_idx = pDecGlobal->dec_picture->ref_idx[pred_dir][cache_index];
  VO_S32 mv = pDecGlobal->dec_picture->mv[pred_dir][cache_index];
  int vec1_x = ioff+(mv<<16>>16);
  int vec1_y = joff+(mv>>16);
  int ref_idx_wp = ref_idx;
  //int list_offset = pDecLocal->list_offset;
  StorablePicture *list = currSlice->listX[currSlice->slice_number][pred_dir][ref_idx];
  // vars for get_block_luma
  int maxold_x = pDecGlobal->width-1;
  int maxold_y = (currSlice->field_pic_flag||pDecLocal->mb_field)?(pDecGlobal->height>>1)-1:pDecGlobal->height-1; //(pDecLocal->mb_field) ? (dec_picture->size_y >> 1) - 1 : dec_picture->size_y_m1;  
  VO_S32 dst_stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
  int list_offset = 0;
  VO_U8* pDst = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y+(j<<2))*dst_stride+pDecLocal->pix_x+(i<<2);
  int dx = (vec1_x & 3);
  int dy = (vec1_y & 3);
  VO_U8* ref;
  VO_S32 fix_edg = 0;
  VO_S32 vy = iClip3(-18, maxold_y+2, (vec1_y>>2));
  VO_S32 vx = iClip3(-18, maxold_x+2, (vec1_x>>2));

  if(ref_idx>>4)
  	return;


  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    VO_S32 edg_height = (vec1_y&7)?-3:0;
	VO_S32 edg_width = (vec1_x&7)?-3:0;
    pDst = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	pDst = pDst+(j<<2)*dst_stride+(i<<2);
	list_offset = (pDecLocal->mb.y&1)?4:2;
	list = currSlice->listX[currSlice->slice_number][pred_dir+list_offset][ref_idx];
	assert(list != pDecGlobal->dec_picture);
	if(list==NULL)
      return;
#if USE_FRAME_THREAD
		  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		  {
			  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
			  const VO_S32 max_y = ((vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE)*2-1;
			  //const VO_S32 max_y = 10000;
			  VO_S32 errFlag;
#if CHECK_REF_WRONG
			  errFlag = list->frame_num - currSlice->frame_num;
			  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
			  {
				  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
			  }
			  else
#endif
			  {
				  errFlag = WaitForProgress(pInfo, list, max_y, 0);
			  }
			  if(errFlag)
			  {
				  //ref is error
				  pInfo->errFlag = errFlag;
				  return;
			  }
		  }
#endif

	//if(pDecLocal->mb.y&1)
	//  vec1_y-=4;
	if((vec1_y>>2)<0-edg_height||(vec1_y>>2)+16>(pDecGlobal->height>>pDecLocal->mb_field)+edg_height||
	   (vec1_x>>2)<0-edg_width||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width)
	{
	  fix_edg = 1;
	  ref = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref, list->refY+vy*dst_stride+vx - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, vx-2, vy-2, pDecGlobal->width, (pDecGlobal->height>>pDecLocal->mb_field));
      ref= ref + 2 + 2*dst_stride;
	}
	else
	{
	  ref = list->refY+vy*dst_stride+vx;
	}
  }
  else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list&&list->iCodingType!=FRAME_CODING))
  {
    VO_S32 edg_height = (vec1_y&7)?-3:0;
	VO_S32 edg_width = (vec1_x&7)?-3:0;
	pDst = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	pDst = pDst+(j<<2)*dst_stride+(i<<2);
	if(list==NULL)
		return;
#if USE_FRAME_THREAD
			  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			  {
				  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
				  const VO_S32 max_y = ((vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE)*2-1;
				  //const VO_S32 max_y = 10000;
				  VO_S32 errFlag;
#if CHECK_REF_WRONG
				  errFlag = list->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list, max_y, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
			  }
#endif

	assert(list != pDecGlobal->dec_picture);
	if((vec1_y>>2)<0-edg_height||(vec1_y>>2)+16>(pDecGlobal->height>>currSlice->field_pic_flag)+edg_height||
	   (vec1_x>>2)<0-edg_width||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width)
	{
	  fix_edg = 1;
	  ref = pDecGlobal->edge_buffer+ 16*pDecLocal->mb.x;
	  get_edge(ref, list->refY+vy*dst_stride+vx - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, vx-2, vy-2, pDecGlobal->width, (pDecGlobal->height>>currSlice->field_pic_flag));
      ref= ref + 2 + 2*dst_stride;
	}
	else
	{
		assert(list != pDecGlobal->dec_picture);
	  ref = list->refY+vy*dst_stride+vx;
	}
  }
  else
  {
    VO_S32 edg_height = (vec1_y&7)?-3:0;
	VO_S32 edg_width = (vec1_x&7)?-3:0;
	if(list==NULL)
		return;
#if USE_FRAME_THREAD
			  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			  {
				  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
				  const VO_S32 max_y = (vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE-1;
				  //const VO_S32 max_y = 10000;
				  VO_S32 errFlag;
#if CHECK_REF_WRONG
				  errFlag = list->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list, max_y, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
			  }
#endif

	if((vec1_y>>2)<0-edg_height||(vec1_y>>2)+16>(pDecGlobal->height)+edg_height||
	   (vec1_x>>2)<0-edg_width||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width)
	{
	  ref = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref, list->refY+vy*dst_stride+vx - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, vx-2, vy-2, pDecGlobal->width, (pDecGlobal->height>>currSlice->field_pic_flag));
      ref= ref + 2 + 2*dst_stride;
	}
	else
	{
      ref = list->refY+vy*dst_stride+vx;
    }
  }
  GetLumaBlock[size_mode][(dy << 2) + dx](ref,dst_stride,pDst,dst_stride);
  if (pDecLocal->mb_field && ((currSlice->slice_type==P_SLICE)||(currSlice->active_pps->weighted_bipred_idc==1 && (currSlice->slice_type==B_SLICE))))
	  ref_idx_wp >>=1;
  if(currSlice->weighted_pred_flag&&currSlice->weighted_flag[pred_dir][ref_idx_wp][0])
  {
    int alpha_l0, wp_offset;
	//int type = currSlice->slice_type;
    
    alpha_l0  = currSlice->wp_weight[pred_dir][ref_idx_wp][0];
    wp_offset = currSlice->wp_offset[pred_dir][ref_idx_wp][0];
    //weighted_mc_prediction(&currSlice->mb_pred[pl][joff], block_size_y, block_size_x, ioff, &currSlice->mb_pred[pl][joff], alpha_l0, wp_offset, currSlice->luma_log2_weight_denom, max_imgpel_value);
#if defined(VOSSSE3)
      GetWeightedBlock[size_mode](pDst,dst_stride, alpha_l0, wp_offset, currSlice->luma_log2_weight_denom);
#else
     if(alpha_l0 == 1 && currSlice->luma_log2_weight_denom==0)
	  GetOffsetBlock[size_mode](pDst,dst_stride, wp_offset);
	else
      GetWeightedBlock[size_mode](pDst,dst_stride, alpha_l0, wp_offset, currSlice->luma_log2_weight_denom);
#endif	
  }

  //if ((chroma_format_idc != YUV400) && (chroma_format_idc != YUV444) ) 
  {
    int ioff_cr = i<<1;
	int joff_cr = j<<1;
    int vec1_y_cr = vec1_y;// + ((active_sps->chroma_format_idc == 1)? currSlice->chroma_vector_adjustment[list_offset + pred_dir][ref_idx] : 0);
    int maxold_x = pDecGlobal->width_cr-1;
    int maxold_y = (currSlice->field_pic_flag||pDecLocal->mb_field)?(pDecGlobal->height_cr>>1)-1:pDecGlobal->height_cr-1; //(pDecLocal->mb_field) ? (dec_picture->size_y >> 1) - 1 : dec_picture->size_y_m1;  
	VO_S32 dst_stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
    VO_U8 *pDst0 = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
	VO_U8 *pDst1 = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
    VO_U8 *ref0,*ref1;
	VO_S32 w00,w01,w10,w11;
	
    vx = iClip3(-pDecGlobal->iChromaPadX, maxold_x, (vec1_x>>3));
	if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
    {
      vec1_y_cr += 2 * ((pDecLocal->mb.y & 1) - ((list->structure%3)- 1));
	  vy = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg|=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>1)||
	  	       (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
      pDst0 = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  pDst1 = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  pDst0 = pDst0+joff_cr*dst_stride+ioff_cr;
	  pDst1 = pDst1+joff_cr*dst_stride+ioff_cr;
	  if(fix_edg)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list->refUV[0]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
		ref1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list->refUV[1]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
	  }
	  else
	  {
	    ref0 = list->refUV[0]+vy*dst_stride+vx;
	    ref1 = list->refUV[1]+vy*dst_stride+vx;
	  }
    }
	else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list->iCodingType!=FRAME_CODING))
	{
	  if(currSlice->field_pic_flag)
	    vec1_y_cr += 2 * ((currSlice->structure==BOTTOM_FIELD?1:0) - ((list->structure%3)- 1));
	  vy = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg|=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>currSlice->field_pic_flag)||
	  	       (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
      pDst0 = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  pDst1 = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  pDst0 = pDst0+joff_cr*dst_stride+ioff_cr;
	  pDst1 = pDst1+joff_cr*dst_stride+ioff_cr;
	  if(fix_edg)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list->refUV[0]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
		ref1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list->refUV[1]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
	  }
	  else
	  {
	    ref0 = list->refUV[0]+vy*dst_stride+vx;
	    ref1 = list->refUV[1]+vy*dst_stride+vx;
	  }
	}
	else
	{
	  vy = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg|=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >=(pDecGlobal->height_cr)||
	  	       (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
	  if(fix_edg)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list->refUV[0]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
		ref1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list->refUV[1]+vy*dst_stride+vx, dst_stride, 9, 9, vx, vy, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
	  }
	  else
	  {
	    ref0 = list->refUV[0]+vy*dst_stride+vx;
	    ref1 = list->refUV[1]+vy*dst_stride+vx;
	  }
	}
	dx = (int) (vec1_x & 7);
    dy = (int) (vec1_y_cr & 7);
#if defined(VOARMV6)
      w00 = dx;
      w01 = dy;
      w10 = dx;
      w11 = dy;	  
#else	
	w00 = W00[(dy<<3)+dx];
    w01 = W01[(dy<<3)+dx];
    w10 = W10[(dy<<3)+dx];
    w11 = W11[(dy<<3)+dx];
#endif	
#if defined(VOSSSE3)
    if(size_mode < 4)
    {
       w00 = dx;
	   w01 = dy;
    }
#endif
	GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref0, dst_stride,pDst0,dst_stride , w00, w01, w10, w11);
    GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref1, dst_stride,pDst1,dst_stride , w00, w01, w10, w11); 
	if(currSlice->weighted_pred_flag&&(currSlice->weighted_flag[pred_dir][ref_idx][1]||currSlice->weighted_flag[pred_dir][ref_idx][2]))
	{
      int *weight = currSlice->wp_weight[pred_dir][ref_idx_wp];
      int *offset = currSlice->wp_offset[pred_dir][ref_idx_wp];
	  int chroma_log2_weight = currSlice->chroma_log2_weight_denom;

#if defined(VOSSSE3)
        GetWeightedBlock[size_mode+3](pDst0,dst_stride, weight[1], offset[1], chroma_log2_weight);
	  GetWeightedBlock[size_mode+3](pDst1,dst_stride, weight[2], offset[2], chroma_log2_weight);
#else
	  if(weight[1] == 1 &&weight[2] == 1 && chroma_log2_weight==0)
	  {
	    GetOffsetBlock[size_mode+3](pDst0,dst_stride, offset[1]);
		GetOffsetBlock[size_mode+3](pDst1,dst_stride, offset[2]);
	  }
	  else
	  {
        GetWeightedBlock[size_mode+3](pDst0,dst_stride, weight[1], offset[1], chroma_log2_weight);
	    GetWeightedBlock[size_mode+3](pDst1,dst_stride, weight[2], offset[2], chroma_log2_weight);
	  }
#endif	  
    }
  }
}

void perform_mc_bi(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int i, int j,int size_mode)
{   
  Slice *currSlice = pDecLocal->p_Slice;
  int ioff = (i<<4)+pDecLocal->mb.x*64;
  int joff = (j<<4)+(pDecLocal->mb.y>>pDecLocal->mb_field)*64;

  VO_S32 cache_index = pDecLocal->mb_xy*16 + j*4 + i;
  VO_S32 l0_refframe = pDecGlobal->dec_picture->ref_idx[LIST_0][cache_index];
  VO_S32 l1_refframe = pDecGlobal->dec_picture->ref_idx[LIST_1][cache_index];
  VO_S32 mv0 = pDecGlobal->dec_picture->mv[LIST_0][cache_index];
  VO_S32 vec1_x = ioff+(mv0<<16>>16);
  VO_S32 vec1_y = joff+(mv0>>16);
  VO_S32 mv1 = pDecGlobal->dec_picture->mv[LIST_1][cache_index];
  VO_S32 vec2_x = ioff+(mv1<<16>>16);
  VO_S32 vec2_y = joff+(mv1>>16);
  //int list_offset = pDecLocal->list_offset;
  
  int maxold_y = (currSlice->field_pic_flag||pDecLocal->mb_field)?(pDecGlobal->height>>1)-1:pDecGlobal->height-1; //(pDecLocal->mb_field) ? (dec_picture->size_y >> 1) - 1 : dec_picture->size_y_m1;  
  //int minold_y = (currSlice->field_pic_flag||pDecLocal->mb_field)? -9:-18;  
  StorablePicture *list0 = currSlice->listX[currSlice->slice_number][LIST_0][l0_refframe];
  StorablePicture *list1 = currSlice->listX[currSlice->slice_number][LIST_1][l1_refframe];
  

  // vars for get_block_luma
  int maxold_x = pDecGlobal->width-1;
  int weighted_bipred_idc = currSlice->weighted_bipred_idc;
  int l0_ref_idx  = l0_refframe;//(pDecLocal->mb_field && weighted_bipred_idc == 1) ? l0_refframe >> 1: l0_refframe;
  int l1_ref_idx  = l1_refframe;//(pDecLocal->mb_field && weighted_bipred_idc == 1) ? l1_refframe >> 1: l1_refframe;
  //int wt_list_offset =0;// (weighted_bipred_idc==2)? list_offset : 0;
  VO_S32 dst_stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
  VO_U8* pDst = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y+(j<<2))*dst_stride+pDecLocal->pix_x+(i<<2);
  int list_offset = 0;
  int dx = (vec1_x & 3);
  int dy = (vec1_y & 3);
  VO_U8* ref0,*ref1;
  VO_U8* mb_pred1;
  VO_U8* mb_pred2;
  VO_S32 fix_edg1=0,fix_edg2=0;
  
  VO_S32 v1y = iClip3(-18, maxold_y+2, (vec1_y>>2));
  VO_S32 v1x = iClip3(-18, maxold_x+2, (vec1_x>>2));
  VO_S32 v2y = iClip3(-18, maxold_y+2, (vec2_y>>2));
  VO_S32 v2x = iClip3(-18, maxold_x+2, (vec2_x>>2));
  assert(list0 != pDecGlobal->dec_picture && list1 != pDecGlobal->dec_picture);
  if((l0_ref_idx>>4)||(l1_ref_idx>>4))
  	return;

  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    VO_S32 edg_height1 = (vec1_y&7)?-3:0;
	VO_S32 edg_width1 = (vec1_x&7)?-3:0;
    pDst = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	pDst = pDst+(j<<2)*dst_stride+(i<<2);
	list_offset = (pDecLocal->mb.y&1)?4:2;
	list0 = currSlice->listX[currSlice->slice_number][LIST_0+list_offset][l0_refframe];
	list1 = currSlice->listX[currSlice->slice_number][LIST_1+list_offset][l1_refframe];
	if(list0==NULL||list1==NULL)
		return;
#if USE_FRAME_THREAD
		  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		  {
			  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
			  const VO_S32 max_y1 = ((vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE)*2-1;
			  const VO_S32 max_y2 = ((vec2_y>>2) + 3*(!!(vec2_y&3)) + MB_BLOCK_SIZE)*2-1;
			  //const VO_S32 max_y1 = 10000;
			  //const VO_S32 max_y2 = 10000;
			  VO_S32 errFlag;
#if CHECK_REF_WRONG
			  errFlag = list0->frame_num - currSlice->frame_num;
			  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
			  {
				  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
			  }
			  else
#endif
			  {
				  errFlag = WaitForProgress(pInfo, list0, max_y1, 0);
			  }
			  if(errFlag)
			  {
				  //ref is error
				  pInfo->errFlag = errFlag;
				  return;
			  }
	  
#if CHECK_REF_WRONG
			  errFlag = list1->frame_num - currSlice->frame_num;
			  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
			  {
				  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
			  }
			  else
#endif
			  {
				  errFlag = WaitForProgress(pInfo, list1, max_y2, 0);
			  }
			  if(errFlag)
			  {
				  //ref is error
				  pInfo->errFlag = errFlag;
				  return;
			  }
		  }
#endif

	
	if((vec1_y>>2)<0-edg_height1||(vec1_y>>2)+16>(pDecGlobal->height>>pDecLocal->mb_field)+edg_height1||
	   (vec1_x>>2)<0-edg_width1||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width1)
	{
	  fix_edg1 = 1;
	  ref0 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref0, list0->refY+v1y*dst_stride+v1x - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v1x-2, v1y-2, pDecGlobal->width, (pDecGlobal->height>>pDecLocal->mb_field));
      ref0= ref0 + 2 + 2*dst_stride;
	}
	else
	{
	  ref0 = list0->refY+v1y*dst_stride+v1x;
	}
  }
  else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list0&&list0->iCodingType!=FRAME_CODING))
  {
    VO_S32 edg_height1 = (vec1_y&7)?-3:0;
	VO_S32 edg_width1 = (vec1_x&7)?-3:0;
	if(list0==NULL||list1==NULL)
		return;
#if USE_FRAME_THREAD
			  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			  {
				  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
				  const VO_S32 max_y1 = ((vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE)*2-1;
				  const VO_S32 max_y2 = ((vec2_y>>2) + 3*(!!(vec2_y&3)) + MB_BLOCK_SIZE)*2-1;
				  //const VO_S32 max_y1 = 10000;
				  //const VO_S32 max_y2 = 10000;
				  VO_S32 errFlag;
#if CHECK_REF_WRONG
				  errFlag = list0->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list0, max_y1, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
		  
#if CHECK_REF_WRONG
				  errFlag = list1->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list1, max_y2, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
			  }
#endif

    pDst = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	pDst = pDst+(j<<2)*dst_stride+(i<<2);
	if((vec1_y>>2)<0-edg_height1||(vec1_y>>2)+16>(pDecGlobal->height>>currSlice->field_pic_flag)+edg_height1||
	   (vec1_x>>2)<0-edg_width1||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width1)
	{
	  fix_edg1 = 1;
	  ref0 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref0, list0->refY+v1y*dst_stride+v1x- 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v1x-2, v1y-2, pDecGlobal->width, (pDecGlobal->height>>currSlice->field_pic_flag));
      ref0= ref0 + 2 + 2*dst_stride;
	}
	else
	{
	  ref0 = list0->refY+v1y*dst_stride+v1x;
	}
  }
  else
  {
    VO_S32 edg_height1 = (vec1_y&7)?-3:0;
	VO_S32 edg_width1 = (vec1_x&7)?-3:0;
	if(list0==NULL||list1==NULL)
		return;
#if USE_FRAME_THREAD
			  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			  {
				  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
				  const VO_S32 max_y1 = (vec1_y>>2) + 3*(!!(vec1_y&3)) + MB_BLOCK_SIZE-1;
				  const VO_S32 max_y2 = (vec2_y>>2) + 3*(!!(vec2_y&3)) + MB_BLOCK_SIZE-1;
				  //const VO_S32 max_y1 = 10000;
				  //const VO_S32 max_y2 = 10000;
				  VO_S32 errFlag;
#if CHECK_REF_WRONG
				  errFlag = list0->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list0, max_y1, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
		  
#if CHECK_REF_WRONG
				  errFlag = list1->frame_num - currSlice->frame_num;
				  if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
				  {
					  errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
				  else
#endif
				  {
					  errFlag = WaitForProgress(pInfo, list1, max_y2, 0);
				  }
				  if(errFlag)
				  {
					  //ref is error
					  pInfo->errFlag = errFlag;
					  return;
				  }
			  }
#endif


	if((vec1_y>>2)<0-edg_height1||(vec1_y>>2)+16>(pDecGlobal->height)+edg_height1||
	   (vec1_x>>2)<0-edg_width1||(vec1_x>>2)+16>(pDecGlobal->width)+edg_width1)
	{
	  fix_edg1 = 1;
	  ref0 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref0, list0->refY+v1y*dst_stride+v1x - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v1x-2, v1y-2, pDecGlobal->width, (pDecGlobal->height>>pDecLocal->mb_field));
      ref0= ref0 + 2 + 2*dst_stride;
	}
	else
	{
      ref0 = list0->refY+v1y*dst_stride+v1x;
	}
  }

  GetLumaBlock[size_mode][(dy << 2) + dx](ref0,dst_stride,pDst,dst_stride);
  dx = (vec2_x & 3);
  dy = (vec2_y & 3);
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    VO_S32 edg_height2 = (vec2_y&7)?-3:0;
	VO_S32 edg_width2 = (vec2_x&7)?-3:0;
    if((vec2_y>>2)<0-edg_height2||(vec2_y>>2)+16>(pDecGlobal->height>>pDecLocal->mb_field)+edg_height2||
	   (vec2_x>>2)<0-edg_width2||(vec2_x>>2)+16>(pDecGlobal->width)+edg_width2)
	{
	  fix_edg2 = 1;
	  ref1 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref1, list1->refY+v2y*dst_stride+v2x - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v2x-2, v2y-2, pDecGlobal->width, (pDecGlobal->height>>pDecLocal->mb_field));
      ref1= ref1 + 2 + 2*dst_stride;
	}
	else
	{
	  ref1 = list1->refY+v2y*dst_stride+v2x;
	}
  }
  else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list1&&list1->iCodingType!=FRAME_CODING))
  {
    VO_S32 edg_height2 = (vec2_y&7)?-3:0;
	VO_S32 edg_width2 = (vec2_x&7)?-3:0;
	if((vec2_y>>2)<0-edg_height2||(vec2_y>>2)+16>(pDecGlobal->height>>currSlice->field_pic_flag)+edg_height2||
	   (vec2_x>>2)<0-edg_width2||(vec2_x>>2)+16>(pDecGlobal->width)+edg_width2)
	{
	  fix_edg2 = 1;
	  ref1 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref1, list1->refY+v2y*dst_stride+v2x - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v2x-2, v2y-2, pDecGlobal->width, (pDecGlobal->height>>currSlice->field_pic_flag));
      ref1= ref1 + 2 + 2*dst_stride;
	}
	else
	{
	  ref1 = list1->refY+v2y*dst_stride+v2x;
	}
  }
  else
  {
    VO_S32 edg_height2 = (vec2_y&7)?-3:0;
	VO_S32 edg_width2 = (vec2_x&7)?-3:0;
	if((vec2_y>>2)<0-edg_height2||(vec2_y>>2)+16>(pDecGlobal->height)+edg_height2||
	   (vec2_x>>2)<0-edg_width2||(vec2_x>>2)+16>(pDecGlobal->width)+edg_width2)
	{
	  fix_edg2 = 1;
	  ref1 = pDecGlobal->edge_buffer + 16*pDecLocal->mb.x;
	  get_edge(ref1, list1->refY+v2y*dst_stride+v2x - 2 - 2*dst_stride, dst_stride, 16+5, 16+5, v2x-2, v2y-2, pDecGlobal->width, (pDecGlobal->height>>currSlice->field_pic_flag));
      ref1= ref1 + 2 + 2*dst_stride;
	}
	else
	{
	  ref1 = list1->refY+v2y*dst_stride+v2x;
	}
  }
  if(!weighted_bipred_idc)
  {
    AddLumaBlock[size_mode][(dy << 2) + dx](ref1,dst_stride,pDst,dst_stride);
  }
  else
  {
    int wt_list_offset = (currSlice->active_pps->weighted_bipred_idc==2)? list_offset : 0;
	int *weight0 = currSlice->wbp_weight[LIST_0+wt_list_offset][l0_ref_idx][l1_ref_idx];
    int *weight1 = currSlice->wbp_weight[LIST_1+wt_list_offset][l0_ref_idx][l1_ref_idx];
    int *offset0 = currSlice->wp_offset[LIST_0+wt_list_offset][l0_ref_idx];
    int *offset1 = currSlice->wp_offset[LIST_1+wt_list_offset][l1_ref_idx];
    int wp_offset = (offset0[0] + offset1[0] );
#if !defined(VOSSSE3)
	VO_U8 tmp_block[256];
#endif
	if(weight1[0]==weight0[0]&&wp_offset==0)
	{
	  //add_block_luma(pDecGlobal,list1, vec2_x, vec2_y,pDst,dst_stride,dst_stride,maxold_x,maxold_y,size_mode);
	  AddLumaBlock[size_mode][(dy << 2) + dx](ref1,dst_stride,pDst,dst_stride);
	}
	else
	{
	  //get_block_luma(pDecGlobal,list1, vec2_x, vec2_y,tmp_block,MB_BLOCK_SIZE,dst_stride,maxold_x,maxold_y,size_mode);
#if defined(VOSSSE3)
     GetLumaBlock[size_mode][(dy << 2) + dx](ref1,dst_stride,pDecGlobal->tmp_block,dst_stride);
     GetBIWBlock[size_mode](pDst,dst_stride, pDecGlobal->tmp_block,dst_stride, weight0[0], weight1[0], wp_offset, currSlice->luma_log2_weight_denom);
#else
	  GetLumaBlock[size_mode][(dy << 2) + dx](ref1,dst_stride,tmp_block,MB_BLOCK_SIZE);
  	  GetBIWBlock[size_mode](pDst,dst_stride, tmp_block,MB_BLOCK_SIZE, weight0[0], weight1[0], wp_offset, currSlice->luma_log2_weight_denom);
#endif
	}
  }


  //if ((chroma_format_idc != YUV400) && (chroma_format_idc != YUV444) ) 
  {  
    int ioff_cr, joff_cr;
	int vec2_y_cr,vec1_y_cr;    
	VO_S32 w00,w01,w10,w11;
	VO_U8* refv0,*refv1;
    int maxold_x = pDecGlobal->width_cr-1;
    int maxold_y = pDecGlobal->height_cr-1; //(pDecLocal->mb_field) ? (dec_picture->size_y >> 1) - 1 : dec_picture->size_y_m1;  
    ///int minold_y = (currSlice->field_pic_flag||pDecLocal->mb_field)? -(pDecGlobal->iChromaPadY>>1):-pDecGlobal->iChromaPadY; 
    VO_S32 dst_stride = pDecGlobal->iChromaStride<<pDecLocal->mb_field;
    //VO_U8 *pDst0 = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
	//VO_U8 *pDst1 = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
	ioff_cr = i<<1;
	joff_cr = j<<1;
	vec1_y_cr = vec1_y;// + currSlice->chroma_vector_adjustment[LIST_0 + list_offset][l0_refframe]; 
    vec2_y_cr = vec2_y;// + currSlice->chroma_vector_adjustment[LIST_1 + list_offset][l1_refframe]; 
	mb_pred1 = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
	mb_pred2 = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+joff_cr)*dst_stride+pDecLocal->pix_c_x+ioff_cr;
    v1x = iClip3(-pDecGlobal->iChromaPadX, maxold_x, (vec1_x>>3));
	if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
    {
      vec1_y_cr += 2 * ((pDecLocal->mb.y & 1) - ((list0->structure%3)- 1));
	  v1y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg1 |=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>1)||
	  	         (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
      mb_pred1 = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  mb_pred2 = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  mb_pred1 = mb_pred1+joff_cr*dst_stride+ioff_cr;
	  mb_pred2 = mb_pred2+joff_cr*dst_stride+ioff_cr;
	  if(fix_edg1)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list0->refUV[0]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
		refv0 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv0, list0->refUV[1]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
	  }
	  else
	  {
	    ref0 = list0->refUV[0]+v1y*dst_stride+v1x;
	    refv0 = list0->refUV[1]+v1y*dst_stride+v1x;
	  }
    }
	else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list0->iCodingType!=FRAME_CODING))
	{
	  if(currSlice->field_pic_flag)
	  {
	    vec1_y_cr += 2 * ((currSlice->structure==BOTTOM_FIELD?1:0) - ((list0->structure%3)- 1));
	    //vec2_y_cr += 2 * ((currSlice->structure==BOTTOM_FIELD?1:0) - (list1->structure- 1));
	  }
	  v1y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg1 |=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>currSlice->field_pic_flag)||
	  	         (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
      mb_pred1 = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  mb_pred2 = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  mb_pred1 = mb_pred1+joff_cr*dst_stride+ioff_cr;
	  mb_pred2 = mb_pred2+joff_cr*dst_stride+ioff_cr;
	  if(fix_edg1)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list0->refUV[0]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
		refv0 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv0, list0->refUV[1]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
	  }
	  else
	  {
	    ref0 = list0->refUV[0]+v1y*dst_stride+v1x;
	    refv0 = list0->refUV[1]+v1y*dst_stride+v1x;
	  }
	}
	else
	{
	  v1y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec1_y_cr>>3));
	  fix_edg1 |=(vec1_y_cr>>3) < 0 || (vec1_y_cr>>3) + 8 >= (pDecGlobal->height_cr)||
	  	         (vec1_x>>3) < 0 || (vec1_x>>3) + 8 >= (pDecGlobal->width_cr);
	  if(fix_edg1)
	  {
	    ref0 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref0, list0->refUV[0]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
		refv0 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv0, list0->refUV[1]+v1y*dst_stride+v1x, dst_stride, 9, 9, v1x, v1y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
	  }
	  else
	  {
	    ref0 = list0->refUV[0]+v1y*dst_stride+v1x;
	    refv0 = list0->refUV[1]+v1y*dst_stride+v1x;
	  }
	}
	dx = (int) (vec1_x & 7);
    dy = (int) (vec1_y_cr & 7);
#if defined(VOARMV6)
      w00 = dx;
      w01 = dy;
      w10 = dx;
      w11 = dy;	  
#else	
	w00 = W00[(dy<<3)+dx];
    w01 = W01[(dy<<3)+dx];
    w10 = W10[(dy<<3)+dx];
    w11 = W11[(dy<<3)+dx];
#endif	
#if defined(VOSSSE3)
		if(size_mode < 4)
		{
		   w00 = dx;
		   w01 = dy;
		}
#endif
    
    GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref0, dst_stride,mb_pred1,dst_stride , w00, w01, w10, w11);
    GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](refv0, dst_stride,mb_pred2,dst_stride , w00, w01, w10, w11);
	v2x = iClip3(-pDecGlobal->iChromaPadX, maxold_x, (vec2_x>>3));
	if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
    {
      vec2_y_cr += 2 * ((pDecLocal->mb.y & 1) - ((list1->structure%3)- 1));
	  v2y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec2_y_cr>>3));
	  fix_edg2|=(vec2_y_cr>>3) < 0 || (vec2_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>1)||
	  	        (vec2_x>>3) < 0 || (vec2_x>>3) + 8 >= (pDecGlobal->width_cr);
	  if(fix_edg2)
	  {
	    ref1 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list1->refUV[0]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
		refv1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv1, list1->refUV[1]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>pDecLocal->mb_field));
	  }
	  else
	  {
	    ref1 = list1->refUV[0]+v2y*dst_stride+v2x;
	    refv1 = list1->refUV[1]+v2y*dst_stride+v2x;
	  }
    }
	else if(currSlice->field_pic_flag||(!currSlice->field_pic_flag&&list1->iCodingType!=FRAME_CODING))
	{
	  if(currSlice->field_pic_flag)
	  {
	    //vec1_y_cr += 2 * ((currSlice->structure==BOTTOM_FIELD?1:0) - (list0->structure- 1));
	    vec2_y_cr += 2 * ((currSlice->structure==BOTTOM_FIELD?1:0) - ((list1->structure%3)- 1));
	  }
	  v2y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec2_y_cr>>3));
	  fix_edg2 |=(vec2_y_cr>>3) < 0 || (vec2_y_cr>>3) + 8 >= (pDecGlobal->height_cr>>currSlice->field_pic_flag)||
	  	        (vec2_x>>3) < 0 || (vec2_x>>3) + 8 >= (pDecGlobal->width_cr);
	  if(fix_edg2)
	  {
	    ref1 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list1->refUV[0]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
		refv1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv1, list1->refUV[1]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
	  }
	  else
	  {
	    ref1 = list1->refUV[0]+v2y*dst_stride+v2x;
	    refv1 = list1->refUV[1]+v2y*dst_stride+v2x;
	  }
	}
	else
	{
	  v2y = iClip3(-pDecGlobal->iChromaPadY, maxold_y, (vec2_y_cr>>3));
	  fix_edg2|=(vec2_y_cr>>3) < 0 || (vec2_y_cr>>3) + 8 >= (pDecGlobal->height_cr)||
	  	        (vec2_x>>3) < 0 || (vec2_x>>3) + 8 >= (pDecGlobal->width_cr);
	  if(fix_edg2)
	  {
	    ref1 = pDecGlobal->edge_ubuffer + 8*pDecLocal->mb.x;
	    get_edge(ref1, list1->refUV[0]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
		refv1 = pDecGlobal->edge_vbuffer + 8*pDecLocal->mb.x;
	    get_edge(refv1, list1->refUV[1]+v2y*dst_stride+v2x, dst_stride, 9, 9, v2x, v2y, pDecGlobal->width_cr, (pDecGlobal->height_cr>>currSlice->field_pic_flag));
	  }
	  else
	  {
	    ref1 = list1->refUV[0]+v2y*dst_stride+v2x;
	    refv1 = list1->refUV[1]+v2y*dst_stride+v2x;
	  }
	}
	dx = (int) (vec2_x & 7);
    dy = (int) (vec2_y_cr & 7);
#if defined(VOARMV6)
      w00 = dx;
      w01 = dy;
      w10 = dx;
      w11 = dy;	  
#else
	w00 = W00[(dy<<3)+dx];
    w01 = W01[(dy<<3)+dx];
    w10 = W10[(dy<<3)+dx];
    w11 = W11[(dy<<3)+dx];
#endif	
	if(!(weighted_bipred_idc))
	{
#if defined(VOSSSE3)
					if(size_mode < 4)
					{
					   w00 = dx;
					   w01 = dy;
					}
#endif	
	  AddChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref1, dst_stride,mb_pred1,dst_stride , w00, w01, w10, w11);
      AddChromaBlock[size_mode][chromadx[dx]+chromady[dy]](refv1, dst_stride,mb_pred2,dst_stride , w00, w01, w10, w11);
	}
	else
	{
	  int wt_list_offset = (currSlice->active_pps->weighted_bipred_idc==2)? list_offset : 0;
	  int *weight0 = currSlice->wbp_weight[LIST_0+wt_list_offset][l0_ref_idx][l1_ref_idx];
      int *weight1 = currSlice->wbp_weight[LIST_1+wt_list_offset][l0_ref_idx][l1_ref_idx];
      int *offset0 = currSlice->wp_offset[LIST_0+wt_list_offset][l0_ref_idx];
      int *offset1 = currSlice->wp_offset[LIST_1+wt_list_offset][l1_ref_idx];
  	  int chroma_log2 = currSlice->chroma_log2_weight_denom;
  	  int wp_offset = (offset0[1] + offset1[1] );
	  int wp_offset1 = (offset0[2] + offset1[2]);
#if !defined(VOSSSE3)	  
      VO_U8 tmp_block[256];
#endif
	  if(weight1[1]==weight0[1]&&weight1[2]==weight0[2]&&wp_offset==0&&wp_offset1==0)
	  {
#if defined(VOSSSE3)
					  if(size_mode < 4)
					  {
						 w00 = dx;
						 w01 = dy;
					  }
#endif	  
		AddChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref1, dst_stride,mb_pred1,dst_stride , w00, w01, w10, w11);
        AddChromaBlock[size_mode][chromadx[dx]+chromady[dy]](refv1, dst_stride,mb_pred2,dst_stride , w00, w01, w10, w11);
	  }
	  else
	  {
	    //get_block_chroma(list1,vec2_x,vec2_y_cr,maxold_x,maxold_y,tmp_block,tmp_block+8*MB_BLOCK_SIZE,MB_BLOCK_SIZE,dst_stride,pDecGlobal,size_mode);
#if defined(VOSSSE3)
			if(size_mode < 4)
			{
			   w00 = dx;
			   w01 = dy;
			}
#endif
#if defined(VOSSSE3)
        GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref1, dst_stride,pDecGlobal->tmp_block,dst_stride , w00, w01, w10, w11);
        GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](refv1, dst_stride,pDecGlobal->tmp_block+8*dst_stride,dst_stride , w00, w01, w10, w11); 
		GetBIWBlock[size_mode+3](mb_pred1,dst_stride,pDecGlobal->tmp_block,dst_stride,weight0[1],weight1[1],wp_offset,chroma_log2);
        
        GetBIWBlock[size_mode+3](mb_pred2,dst_stride,pDecGlobal->tmp_block+8*dst_stride,dst_stride,weight0[2],weight1[2],wp_offset1,chroma_log2);
#else	    
        GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](ref1, dst_stride,tmp_block,MB_BLOCK_SIZE , w00, w01, w10, w11);
        GetChromaBlock[size_mode][chromadx[dx]+chromady[dy]](refv1, dst_stride,tmp_block+8*MB_BLOCK_SIZE,MB_BLOCK_SIZE , w00, w01, w10, w11); 
		GetBIWBlock[size_mode+3](mb_pred1,dst_stride,tmp_block,MB_BLOCK_SIZE,weight0[1],weight1[1],wp_offset,chroma_log2);
        
        GetBIWBlock[size_mode+3](mb_pred2,dst_stride,tmp_block+8*MB_BLOCK_SIZE,MB_BLOCK_SIZE,weight0[2],weight1[2],wp_offset1,chroma_log2);
#endif		
	  }
	}
  }  
}
void perform_mc(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int pred_dir, int i, int j,int size_mode)
{
  //Slice *currSlice = pDecLocal->p_Slice;
  //assert (pred_dir<=2);
  if(size_mode<0||size_mode>6||pred_dir<0||pred_dir>2)
  	return;
  if (pred_dir != 2)
  {
    perform_mc_single(pDecGlobal,pDecLocal, pred_dir, i, j,size_mode);
  }
  else
  {
    perform_mc_bi(pDecGlobal,pDecLocal, i, j,size_mode);
  }
}
#if defined(VOARMV7)


MC_PREDICTION_LUMA_FUNC GetLumaBlock[7][16] = {
	{MCCopy16x16,get_luma_10_16x16,get_luma_h_16x16_ARMV7,get_luma_30_16x16,get_luma_01_16x16,get_luma_11_16x16,
	 get_luma_21_16x16,get_luma_31_16x16,get_luma_v_16x16_ARMV7,get_luma_12_16x16,get_luma_c_16x16_ARMV7,get_luma_32_16x16,
	 get_luma_03_16x16,get_luma_13_16x16,get_luma_23_16x16,get_luma_33_16x16},
	{MCCopy16x8,get_luma_10_16x8,get_luma_h_16x8_ARMV7,get_luma_30_16x8,get_luma_01_16x8,get_luma_11_16x8,
	 get_luma_21_16x8,get_luma_31_16x8,get_luma_v_16x8_ARMV7,get_luma_12_16x8,get_luma_c_16x8_ARMV7,get_luma_32_16x8,
	 get_luma_03_16x8,get_luma_13_16x8,get_luma_23_16x8,get_luma_33_16x8},
	{MCCopy8x16,get_luma_10_8x16,get_luma_h_8x16_ARMV7,get_luma_30_8x16,get_luma_01_8x16,get_luma_11_8x16,
	 get_luma_21_8x16,get_luma_31_8x16,get_luma_v_8x16_ARMV7,get_luma_12_8x16,get_luma_c_8x16_ARMV7,get_luma_32_8x16,
	 get_luma_03_8x16,get_luma_13_8x16,get_luma_23_8x16,get_luma_33_8x16},
	{MCCopy8x8,get_luma_10_8x8,get_luma_h_8x8_ARMV7,get_luma_30_8x8,get_luma_01_8x8,get_luma_11_8x8,
	 get_luma_21_8x8,get_luma_31_8x8,get_luma_v_8x8_ARMV7,get_luma_12_8x8,get_luma_c_8x8_ARMV7,get_luma_32_8x8,
	 get_luma_03_8x8,get_luma_13_8x8,get_luma_23_8x8,get_luma_33_8x8},
	{MCCopy8x4,get_luma_10_8x4,get_luma_h_8x4_ARMV7,get_luma_30_8x4,get_luma_01_8x4,get_luma_11_8x4,
	 get_luma_21_8x4,get_luma_31_8x4,get_luma_v_8x4_ARMV7,get_luma_12_8x4,get_luma_c_8x4_ARMV7,get_luma_32_8x4,
	 get_luma_03_8x4,get_luma_13_8x4,get_luma_23_8x4,get_luma_33_8x4},
	{MCCopy4x8,get_luma_10_4x8,get_luma_h_4x8_ARMV7,get_luma_30_4x8,get_luma_01_4x8,get_luma_11_4x8,
	 get_luma_21_4x8,get_luma_31_4x8,get_luma_v_4x8_ARMV7,get_luma_12_4x8,get_luma_c_4x8_ARMV7,get_luma_32_4x8,
	 get_luma_03_4x8,get_luma_13_4x8,get_luma_23_4x8,get_luma_33_4x8},
	{MCCopy4x4,get_luma_10_4x4,get_luma_h_4x4_ARMV7,get_luma_30_4x4,get_luma_01_4x4,get_luma_11_4x4,
	 get_luma_21_4x4,get_luma_31_4x4,get_luma_v_4x4_ARMV7,get_luma_12_4x4,get_luma_c_4x4_ARMV7,get_luma_32_4x4,
	 get_luma_03_4x4,get_luma_13_4x4,get_luma_23_4x4,get_luma_33_4x4}
};

MC_PREDICTION_LUMA_FUNC AddLumaBlock[7][16] = {
	{add_block_00_16x16,add_luma_10_16x16,avg_dst_luma_h_16x16_ARMV7,add_luma_30_16x16,add_luma_01_16x16,add_luma_11_16x16,
	 add_luma_21_16x16,add_luma_31_16x16,avg_dst_luma_v_16x16_ARMV7,add_luma_12_16x16,add_luma_c_16x16_ARMV7,add_luma_32_16x16,
	 add_luma_03_16x16,add_luma_13_16x16,add_luma_23_16x16,add_luma_33_16x16},
	{add_block_00_16x8,add_luma_10_16x8,avg_dst_luma_h_16x8_ARMV7,add_luma_30_16x8,add_luma_01_16x8,add_luma_11_16x8,
	 add_luma_21_16x8,add_luma_31_16x8,avg_dst_luma_v_16x8_ARMV7,add_luma_12_16x8,add_luma_c_16x8_ARMV7,add_luma_32_16x8,
	 add_luma_03_16x8,add_luma_13_16x8,add_luma_23_16x8,add_luma_33_16x8},
	{add_block_00_8x16,add_luma_10_8x16,avg_dst_luma_h_8x16_ARMV7,add_luma_30_8x16,add_luma_01_8x16,add_luma_11_8x16,
	 add_luma_21_8x16,add_luma_31_8x16,avg_dst_luma_v_8x16_ARMV7,add_luma_12_8x16,add_luma_c_8x16_ARMV7,add_luma_32_8x16,
	 add_luma_03_8x16,add_luma_13_8x16,add_luma_23_8x16,add_luma_33_8x16},
	{add_block_00_8x8,add_luma_10_8x8,avg_dst_luma_h_8x8_ARMV7,add_luma_30_8x8,add_luma_01_8x8,add_luma_11_8x8,
	 add_luma_21_8x8,add_luma_31_8x8,avg_dst_luma_v_8x8_ARMV7,add_luma_12_8x8,add_luma_c_8x8_ARMV7,add_luma_32_8x8,
	 add_luma_03_8x8,add_luma_13_8x8,add_luma_23_8x8,add_luma_33_8x8},
	{add_block_00_8x4,add_luma_10_8x4,avg_dst_luma_h_8x4_ARMV7,add_luma_30_8x4,add_luma_01_8x4,add_luma_11_8x4,
	 add_luma_21_8x4,add_luma_31_8x4,avg_dst_luma_v_8x4_ARMV7,add_luma_12_8x4,add_luma_c_8x4_ARMV7,add_luma_32_8x4,
	 add_luma_03_8x4,add_luma_13_8x4,add_luma_23_8x4,add_luma_33_8x4},
	{add_block_00_4x8,add_luma_10_4x8,avg_dst_luma_h_4x8_ARMV7,add_luma_30_4x8,add_luma_01_4x8,add_luma_11_4x8,
	 add_luma_21_4x8,add_luma_31_4x8,avg_dst_luma_v_4x8_ARMV7,add_luma_12_4x8,add_luma_c_4x8_ARMV7,add_luma_32_4x8,
	 add_luma_03_4x8,add_luma_13_4x8,add_luma_23_4x8,add_luma_33_4x8},
	{add_block_00_4x4,add_luma_10_4x4,avg_dst_luma_h_4x4_ARMV7,add_luma_30_4x4,add_luma_01_4x4,add_luma_11_4x4,
	 add_luma_21_4x4,add_luma_31_4x4,avg_dst_luma_v_4x4_ARMV7,add_luma_12_4x4,add_luma_c_4x4_ARMV7,add_luma_32_4x4,
	 add_luma_03_4x4,add_luma_13_4x4,add_luma_23_4x4,add_luma_33_4x4}
};

MC_PREDICTION_CHROMA_FUNC GetChromaBlock[7][4] = {
	{get_block_00_8x8_chroma,MCChroma8x8_ARMV7,MCChroma8x8_ARMV7,MCChroma8x8_ARMV7},
	{get_block_00_8x4_chroma,MCChroma8x4_ARMV7,MCChroma8x4_ARMV7,MCChroma8x4_ARMV7},
	{get_block_00_4x8_chroma,MCChroma4x8_ARMV7,MCChroma4x8_ARMV7,MCChroma4x8_ARMV7},
	{get_block_00_4x4_chroma,MCChroma4x4_ARMV7,MCChroma4x4_ARMV7,MCChroma4x4_ARMV7},
	{get_block_00_4x2,MCChroma4x2_ARMV7,MCChroma4x2_ARMV7,MCChroma4x2_ARMV7},
	{get_block_00_2x4,MCChroma2x4_ARMV7,MCChroma2x4_ARMV7,MCChroma2x4_ARMV7},
	{get_block_00_2x2,MCChroma2x2_ARMV7,MCChroma2x2_ARMV7,MCChroma2x2_ARMV7}
};
MC_PREDICTION_CHROMA_FUNC AddChromaBlock[7][4] = {
	{add_block_00_8x8_chroma,ADDMCChroma8x8_ARMV7,ADDMCChroma8x8_ARMV7,ADDMCChroma8x8_ARMV7},
	{add_block_00_8x4_chroma,ADDMCChroma8x4_ARMV7,ADDMCChroma8x4_ARMV7,ADDMCChroma8x4_ARMV7},
	{add_block_00_4x8_chroma,ADDMCChroma4x8_ARMV7,ADDMCChroma4x8_ARMV7,ADDMCChroma4x8_ARMV7},
	{add_block_00_4x4_chroma,ADDMCChroma4x4_ARMV7,ADDMCChroma4x4_ARMV7,ADDMCChroma4x4_ARMV7},
	{add_block_00_4x2,ADDMCChroma4x2_ARMV7,ADDMCChroma4x2_ARMV7,ADDMCChroma4x2_ARMV7},
	{add_block_00_2x4,ADDMCChroma2x4_ARMV7,ADDMCChroma2x4_ARMV7,ADDMCChroma2x4_ARMV7},
	{add_block_00_2x2,ADDMCChroma2x2_ARMV7,ADDMCChroma2x2_ARMV7,ADDMCChroma2x2_ARMV7}
};

MC_BI_FUNC GetBIBlock[10] = {
	AVG16x16_ARMV7,AVG16x8_ARMV7,AVG8x16_ARMV7,AVG8x8_ARMV7,AVG8x4_ARMV7,
	AVG4x8_ARMV7,AVG4x4_ARMV7,AVG4x2_ARMV7,AVG2x4_ARMV7,AVG2x2_ARMV7
};

MC_OFFSET_FUNC GetOffsetBlock[10] = {
	offset_mc_prediction_16x16_ARMV7,offset_mc_prediction_16x8_ARMV7,offset_mc_prediction_8x16_ARMV7,offset_mc_prediction_8x8_ARMV7,
	offset_mc_prediction_8x4_ARMV7,offset_mc_prediction_4x8_ARMV7,offset_mc_prediction_4x4_ARMV7,offset_mc_prediction_4x2_ARMV7,
	offset_mc_prediction_2x4_ARMV7,offset_mc_prediction_2x2_ARMV7
};
MC_WEIGHTED_FUNC GetWeightedBlock[10] = {
	weighted_mc_prediction_16x16_ARMV7,weighted_mc_prediction_16x8_ARMV7,weighted_mc_prediction_8x16_ARMV7,weighted_mc_prediction_8x8_ARMV7,
	weighted_mc_prediction_8x4_ARMV7,weighted_mc_prediction_4x8_ARMV7,weighted_mc_prediction_4x4_ARMV7,weighted_mc_prediction_4x2_ARMV7,
	weighted_mc_prediction_2x4,weighted_mc_prediction_2x2
};
/*
MC_WEIGHTED_FUNC GetWeightedBlock[10] = {
	weighted_mc_prediction_16x16_ARMV7,weighted_mc_prediction_16x8_ARMV7,weighted_mc_prediction_8x16_ARMV7,weighted_mc_prediction_8x8_ARMV7,
	weighted_mc_prediction_8x4_ARMV7,weighted_mc_prediction_4x8_ARMV7,weighted_mc_prediction_4x4_ARMV7,weighted_mc_prediction_4x2_ARMV7,
	weighted_mc_prediction_2x4_ARMV7,weighted_mc_prediction_2x2_ARMV7
};*/

MC_BIW_FUNC GetBIWBlock[10] = {
	weighted_bi_prediction_16x16_ARMV7,weighted_bi_prediction_16x8_ARMV7,weighted_bi_prediction_8x16_ARMV7,
	weighted_bi_prediction_8x8_ARMV7,weighted_bi_prediction_8x4_ARMV7,weighted_bi_prediction_4x8_ARMV7,
	weighted_bi_prediction_4x4_ARMV7,weighted_bi_prediction_4x2_ARMV7,weighted_bi_prediction_2x4,weighted_bi_prediction_2x2
};

#else

#if defined(VOSSSE3)
MC_PREDICTION_LUMA_FUNC GetLumaBlockSSSE3[7][16] = {
	 {
	   get_block_00_16x16_ssse3,get_luma_10_16x16_ssse3,get_luma_20_16x16_ssse3,get_luma_30_16x16_ssse3,
	   get_luma_01_16x16_ssse3,get_luma_11_16x16_ssse3,get_luma_21_16x16_ssse3,get_luma_31_16x16_ssse3,
	   get_luma_02_16x16_ssse3,get_luma_12_16x16_ssse3,get_luma_22_16x16_ssse3,get_luma_32_16x16_ssse3,
	   get_luma_03_16x16_ssse3,get_luma_13_16x16_ssse3,get_luma_23_16x16_ssse3,get_luma_33_16x16_ssse3
	 },
	 {
	   get_block_00_16x8_ssse3,get_luma_10_16x8_ssse3,get_luma_20_16x8_ssse3,get_luma_30_16x8_ssse3,
	   get_luma_01_16x8_ssse3,get_luma_11_16x8_ssse3, get_luma_21_16x8_ssse3,get_luma_31_16x8_ssse3,
	   get_luma_02_16x8_ssse3,get_luma_12_16x8_ssse3,get_luma_22_16x8_ssse3,get_luma_32_16x8_ssse3,
	   get_luma_03_16x8_ssse3,get_luma_13_16x8_ssse3,get_luma_23_16x8_ssse3,get_luma_33_16x8_ssse3
	 },
	 {
	   get_block_00_8x16_ssse3,get_luma_10_8x16_ssse3,get_luma_20_8x16_ssse3,get_luma_30_8x16_ssse3,
	   get_luma_01_8x16_ssse3,get_luma_11_8x16_ssse3, get_luma_21_8x16_ssse3,get_luma_31_8x16_ssse3,
	   get_luma_02_8x16_ssse3,get_luma_12_8x16_ssse3,get_luma_22_8x16_ssse3,get_luma_32_8x16_ssse3,
	   get_luma_03_8x16_ssse3,get_luma_13_8x16_ssse3,get_luma_23_8x16_ssse3,get_luma_33_8x16_ssse3
	 },
	 {
	   get_block_00_8x8_ssse3,get_luma_10_8x8_ssse3,get_luma_20_8x8_ssse3,get_luma_30_8x8_ssse3,
	   get_luma_01_8x8_ssse3,get_luma_11_8x8_ssse3,get_luma_21_8x8_ssse3,get_luma_31_8x8_ssse3,
	   get_luma_02_8x8_ssse3,get_luma_12_8x8_ssse3,get_luma_22_8x8_ssse3,get_luma_32_8x8_ssse3,
	   get_luma_03_8x8_ssse3,get_luma_13_8x8_ssse3,get_luma_23_8x8_ssse3,get_luma_33_8x8_ssse3
	 },
	 {
	   get_block_00_8x4_ssse3,get_luma_10_8x4,get_luma_20_8x4,get_luma_30_8x4,get_luma_01_8x4,get_luma_11_8x4,
	   get_luma_21_8x4,get_luma_31_8x4,get_luma_02_8x4,get_luma_12_8x4,get_luma_22_8x4,get_luma_32_8x4,
	   get_luma_03_8x4,get_luma_13_8x4,get_luma_23_8x4,get_luma_33_8x4
	 },
	 {
	   get_block_00_4x8_ssse3,get_luma_10_4x8,get_luma_20_4x8,get_luma_30_4x8,get_luma_01_4x8,get_luma_11_4x8,
	   get_luma_21_4x8,get_luma_31_4x8,get_luma_02_4x8,get_luma_12_4x8,get_luma_22_4x8,get_luma_32_4x8,
	   get_luma_03_4x8,get_luma_13_4x8,get_luma_23_4x8,get_luma_33_4x8
	 },
	 { get_block_00_4x4_ssse3,get_luma_10_4x4,get_luma_20_4x4,get_luma_30_4x4,get_luma_01_4x4,get_luma_11_4x4,
	   get_luma_21_4x4,get_luma_31_4x4,get_luma_02_4x4,get_luma_12_4x4,get_luma_22_4x4,get_luma_32_4x4,
	   get_luma_03_4x4,get_luma_13_4x4,get_luma_23_4x4,get_luma_33_4x4
	 }

};

MC_PREDICTION_LUMA_FUNC GetLumaBlockSSE2[7][16] = {
	 {
	   get_block_00_16x16_sse2,get_luma_10_16x16_sse2,get_luma_20_16x16_sse2,get_luma_30_16x16_sse2,
	   get_luma_01_16x16_sse2,get_luma_11_16x16_sse2,get_luma_21_16x16_sse2,get_luma_31_16x16_sse2,
	   get_luma_02_16x16_sse2,get_luma_12_16x16_sse2,get_luma_22_16x16_sse2,get_luma_32_16x16_sse2,
	   get_luma_03_16x16_sse2,get_luma_13_16x16_sse2,get_luma_23_16x16_sse2,get_luma_33_16x16_sse2
	 },
	 {
	   get_block_00_16x8_sse2,get_luma_10_16x8_sse2,get_luma_20_16x8_sse2,get_luma_30_16x8_sse2,
	   get_luma_01_16x8_sse2,get_luma_11_16x8_sse2, get_luma_21_16x8_sse2,get_luma_31_16x8_sse2,
	   get_luma_02_16x8_sse2,get_luma_12_16x8_sse2,get_luma_22_16x8_sse2,get_luma_32_16x8_sse2,
	   get_luma_03_16x8_sse2,get_luma_13_16x8_sse2,get_luma_23_16x8_sse2,get_luma_33_16x8_sse2
	 },
	 {
	   get_block_00_8x16_sse2,get_luma_10_8x16_sse2,get_luma_20_8x16_sse2,get_luma_30_8x16_sse2,
	   get_luma_01_8x16_sse2,get_luma_11_8x16_sse2, get_luma_21_8x16_sse2,get_luma_31_8x16_sse2,
	   get_luma_02_8x16_sse2,get_luma_12_8x16_sse2,get_luma_22_8x16_sse2,get_luma_32_8x16_sse2,
	   get_luma_03_8x16_sse2,get_luma_13_8x16_sse2,get_luma_23_8x16_sse2,get_luma_33_8x16_sse2
	 },
	 {
	   get_block_00_8x8_sse2,get_luma_10_8x8_sse2,get_luma_20_8x8_sse2,get_luma_30_8x8_sse2,
	   get_luma_01_8x8_sse2,get_luma_11_8x8_sse2,get_luma_21_8x8_sse2,get_luma_31_8x8_sse2,
	   get_luma_02_8x8_sse2,get_luma_12_8x8_sse2,get_luma_22_8x8_sse2,get_luma_32_8x8_sse2,
	   get_luma_03_8x8_sse2,get_luma_13_8x8_sse2,get_luma_23_8x8_sse2,get_luma_33_8x8_sse2
	 },
	 {
	   get_block_00_8x4_sse2,get_luma_10_8x4,get_luma_20_8x4,get_luma_30_8x4,get_luma_01_8x4,get_luma_11_8x4,
	   get_luma_21_8x4,get_luma_31_8x4,get_luma_02_8x4,get_luma_12_8x4,get_luma_22_8x4,get_luma_32_8x4,
	   get_luma_03_8x4,get_luma_13_8x4,get_luma_23_8x4,get_luma_33_8x4
	 },
	 {
	   get_block_00_4x8_sse2,get_luma_10_4x8,get_luma_20_4x8,get_luma_30_4x8,get_luma_01_4x8,get_luma_11_4x8,
	   get_luma_21_4x8,get_luma_31_4x8,get_luma_02_4x8,get_luma_12_4x8,get_luma_22_4x8,get_luma_32_4x8,
	   get_luma_03_4x8,get_luma_13_4x8,get_luma_23_4x8,get_luma_33_4x8
	 },
	 { get_block_00_4x4_sse2,get_luma_10_4x4,get_luma_20_4x4,get_luma_30_4x4,get_luma_01_4x4,get_luma_11_4x4,
	   get_luma_21_4x4,get_luma_31_4x4,get_luma_02_4x4,get_luma_12_4x4,get_luma_22_4x4,get_luma_32_4x4,
	   get_luma_03_4x4,get_luma_13_4x4,get_luma_23_4x4,get_luma_33_4x4
	 }

};


MC_PREDICTION_LUMA_FUNC AddLumaBlockSSSE3[7][16] = {
	{
          add_block_00_16x16_ssse3,  add_luma_10_16x16_ssse3,  add_luma_20_16x16_ssse3,  add_luma_30_16x16_ssse3,
	   add_luma_01_16x16_ssse3,   add_luma_11_16x16_ssse3,  add_luma_21_16x16_ssse3,  add_luma_31_16x16_ssse3,
	   add_luma_02_16x16_ssse3,   add_luma_12_16x16_ssse3,  add_luma_22_16x16_ssse3,  add_luma_32_16x16_ssse3,
	   add_luma_03_16x16_ssse3,   add_luma_13_16x16_ssse3,  add_luma_23_16x16_ssse3,  add_luma_33_16x16_ssse3
	},
	{
	   add_block_00_16x8_ssse3,    add_luma_10_16x8_ssse3,    add_luma_20_16x8_ssse3,    add_luma_30_16x8_ssse3,
	   add_luma_01_16x8_ssse3,     add_luma_11_16x8_ssse3,    add_luma_21_16x8_ssse3,    add_luma_31_16x8_ssse3,
	   add_luma_02_16x8_ssse3,     add_luma_12_16x8_ssse3,    add_luma_22_16x8_ssse3,    add_luma_32_16x8_ssse3,
	   add_luma_03_16x8_ssse3,     add_luma_13_16x8_ssse3,    add_luma_23_16x8_ssse3,    add_luma_33_16x8_ssse3
	},
	{
	   add_block_00_8x16_ssse3,    add_luma_10_8x16_ssse3,    add_luma_20_8x16_ssse3,    add_luma_30_8x16_ssse3,
	   add_luma_01_8x16_ssse3,     add_luma_11_8x16_ssse3,    add_luma_21_8x16_ssse3,    add_luma_31_8x16_ssse3,
	   add_luma_02_8x16_ssse3,     add_luma_12_8x16_ssse3,    add_luma_22_8x16_ssse3,    add_luma_32_8x16_ssse3,
	   add_luma_03_8x16_ssse3,     add_luma_13_8x16_ssse3,    add_luma_23_8x16_ssse3,    add_luma_33_8x16_ssse3
	},
	{
	   add_block_00_8x8_ssse3,      add_luma_10_8x8_ssse3,      add_luma_20_8x8_ssse3,      add_luma_30_8x8_ssse3,
	   add_luma_01_8x8_ssse3,       add_luma_11_8x8_ssse3,      add_luma_21_8x8_ssse3,      add_luma_31_8x8_ssse3,
	   add_luma_02_8x8_ssse3,       add_luma_12_8x8_ssse3,      add_luma_22_8x8_ssse3,      add_luma_32_8x8_ssse3,
	   add_luma_03_8x8_ssse3,       add_luma_13_8x8_ssse3,      add_luma_23_8x8_ssse3,      add_luma_33_8x8_ssse3
	},
	{
	   add_block_00_8x4_ssse3,add_luma_10_8x4,add_luma_20_8x4,add_luma_30_8x4,add_luma_01_8x4,add_luma_11_8x4,
	   add_luma_21_8x4,add_luma_31_8x4,add_luma_02_8x4,add_luma_12_8x4,add_luma_22_8x4,add_luma_32_8x4,
	   add_luma_03_8x4,add_luma_13_8x4,add_luma_23_8x4,add_luma_33_8x4
	},
	{
	   add_block_00_4x8_ssse3,add_luma_10_4x8,add_luma_20_4x8,add_luma_30_4x8,add_luma_01_4x8,add_luma_11_4x8,
	   add_luma_21_4x8,add_luma_31_4x8,add_luma_02_4x8,add_luma_12_4x8,add_luma_22_4x8,add_luma_32_4x8,
	   add_luma_03_4x8,add_luma_13_4x8,add_luma_23_4x8,add_luma_33_4x8
	},
	{
	   add_block_00_4x4_ssse3,add_luma_10_4x4,add_luma_20_4x4,add_luma_30_4x4,add_luma_01_4x4,add_luma_11_4x4,
	   add_luma_21_4x4,add_luma_31_4x4,add_luma_02_4x4,add_luma_12_4x4,add_luma_22_4x4,add_luma_32_4x4,
	   add_luma_03_4x4,add_luma_13_4x4,add_luma_23_4x4,add_luma_33_4x4
	}
};

MC_PREDICTION_LUMA_FUNC AddLumaBlockSSE2[7][16] = {
	{
          add_block_00_16x16_sse2,  add_luma_10_16x16_sse2,  add_luma_20_16x16_sse2,  add_luma_30_16x16_sse2,
	   add_luma_01_16x16_sse2,   add_luma_11_16x16_sse2,  add_luma_21_16x16_sse2,  add_luma_31_16x16_sse2,
	   add_luma_02_16x16_sse2,   add_luma_12_16x16_sse2,  add_luma_22_16x16_sse2,  add_luma_32_16x16_sse2,
	   add_luma_03_16x16_sse2,   add_luma_13_16x16_sse2,  add_luma_23_16x16_sse2,  add_luma_33_16x16_sse2
	},
	{
	   add_block_00_16x8_sse2,    add_luma_10_16x8_sse2,    add_luma_20_16x8_sse2,    add_luma_30_16x8_sse2,
	   add_luma_01_16x8_sse2,     add_luma_11_16x8_sse2,    add_luma_21_16x8_sse2,    add_luma_31_16x8_sse2,
	   add_luma_02_16x8_sse2,     add_luma_12_16x8_sse2,    add_luma_22_16x8_sse2,    add_luma_32_16x8_sse2,
	   add_luma_03_16x8_sse2,     add_luma_13_16x8_sse2,    add_luma_23_16x8_sse2,    add_luma_33_16x8_sse2
	},
	{
	   add_block_00_8x16_sse2,    add_luma_10_8x16_sse2,    add_luma_20_8x16_sse2,    add_luma_30_8x16_sse2,
	   add_luma_01_8x16_sse2,     add_luma_11_8x16_sse2,    add_luma_21_8x16_sse2,    add_luma_31_8x16_sse2,
	   add_luma_02_8x16_sse2,     add_luma_12_8x16_sse2,    add_luma_22_8x16_sse2,    add_luma_32_8x16_sse2,
	   add_luma_03_8x16_sse2,     add_luma_13_8x16_sse2,    add_luma_23_8x16_sse2,    add_luma_33_8x16_sse2
	},
	{
	   add_block_00_8x8_sse2,      add_luma_10_8x8_sse2,      add_luma_20_8x8_sse2,      add_luma_30_8x8_sse2,
	   add_luma_01_8x8_sse2,       add_luma_11_8x8_sse2,      add_luma_21_8x8_sse2,      add_luma_31_8x8_sse2,
	   add_luma_02_8x8_sse2,       add_luma_12_8x8_sse2,      add_luma_22_8x8_sse2,      add_luma_32_8x8_sse2,
	   add_luma_03_8x8_sse2,       add_luma_13_8x8_sse2,      add_luma_23_8x8_sse2,      add_luma_33_8x8_sse2
	},
	{
	   add_block_00_8x4_sse2,add_luma_10_8x4,add_luma_20_8x4,add_luma_30_8x4,add_luma_01_8x4,add_luma_11_8x4,
	   add_luma_21_8x4,add_luma_31_8x4,add_luma_02_8x4,add_luma_12_8x4,add_luma_22_8x4,add_luma_32_8x4,
	   add_luma_03_8x4,add_luma_13_8x4,add_luma_23_8x4,add_luma_33_8x4
	},
	{
	   add_block_00_4x8_sse2,add_luma_10_4x8,add_luma_20_4x8,add_luma_30_4x8,add_luma_01_4x8,add_luma_11_4x8,
	   add_luma_21_4x8,add_luma_31_4x8,add_luma_02_4x8,add_luma_12_4x8,add_luma_22_4x8,add_luma_32_4x8,
	   add_luma_03_4x8,add_luma_13_4x8,add_luma_23_4x8,add_luma_33_4x8
	},
	{
	   add_block_00_4x4_sse2,add_luma_10_4x4,add_luma_20_4x4,add_luma_30_4x4,add_luma_01_4x4,add_luma_11_4x4,
	   add_luma_21_4x4,add_luma_31_4x4,add_luma_02_4x4,add_luma_12_4x4,add_luma_22_4x4,add_luma_32_4x4,
	   add_luma_03_4x4,add_luma_13_4x4,add_luma_23_4x4,add_luma_33_4x4
	}
};
#else
MC_PREDICTION_LUMA_FUNC GetLumaBlock[7][16] = {
	{get_block_00_16x16,get_luma_10_16x16,get_luma_20_16x16,get_luma_30_16x16,get_luma_01_16x16,get_luma_11_16x16,
	 get_luma_21_16x16,get_luma_31_16x16,get_luma_02_16x16,get_luma_12_16x16,get_luma_22_16x16,get_luma_32_16x16,
	 get_luma_03_16x16,get_luma_13_16x16,get_luma_23_16x16,get_luma_33_16x16},
	{get_block_00_16x8,get_luma_10_16x8,get_luma_20_16x8,get_luma_30_16x8,get_luma_01_16x8,get_luma_11_16x8,
	 get_luma_21_16x8,get_luma_31_16x8,get_luma_02_16x8,get_luma_12_16x8,get_luma_22_16x8,get_luma_32_16x8,
	 get_luma_03_16x8,get_luma_13_16x8,get_luma_23_16x8,get_luma_33_16x8},
	{get_block_00_8x16,get_luma_10_8x16,get_luma_20_8x16,get_luma_30_8x16,get_luma_01_8x16,get_luma_11_8x16,
	 get_luma_21_8x16,get_luma_31_8x16,get_luma_02_8x16,get_luma_12_8x16,get_luma_22_8x16,get_luma_32_8x16,
	 get_luma_03_8x16,get_luma_13_8x16,get_luma_23_8x16,get_luma_33_8x16},
	{get_block_00_8x8,get_luma_10_8x8,get_luma_20_8x8,get_luma_30_8x8,get_luma_01_8x8,get_luma_11_8x8,
	 get_luma_21_8x8,get_luma_31_8x8,get_luma_02_8x8,get_luma_12_8x8,get_luma_22_8x8,get_luma_32_8x8,
	 get_luma_03_8x8,get_luma_13_8x8,get_luma_23_8x8,get_luma_33_8x8},
	{get_block_00_8x4,get_luma_10_8x4,get_luma_20_8x4,get_luma_30_8x4,get_luma_01_8x4,get_luma_11_8x4,
	 get_luma_21_8x4,get_luma_31_8x4,get_luma_02_8x4,get_luma_12_8x4,get_luma_22_8x4,get_luma_32_8x4,
	 get_luma_03_8x4,get_luma_13_8x4,get_luma_23_8x4,get_luma_33_8x4},
	{get_block_00_4x8,get_luma_10_4x8,get_luma_20_4x8,get_luma_30_4x8,get_luma_01_4x8,get_luma_11_4x8,
	 get_luma_21_4x8,get_luma_31_4x8,get_luma_02_4x8,get_luma_12_4x8,get_luma_22_4x8,get_luma_32_4x8,
	 get_luma_03_4x8,get_luma_13_4x8,get_luma_23_4x8,get_luma_33_4x8},
	{get_block_00_4x4,get_luma_10_4x4,get_luma_20_4x4,get_luma_30_4x4,get_luma_01_4x4,get_luma_11_4x4,
	 get_luma_21_4x4,get_luma_31_4x4,get_luma_02_4x4,get_luma_12_4x4,get_luma_22_4x4,get_luma_32_4x4,
	 get_luma_03_4x4,get_luma_13_4x4,get_luma_23_4x4,get_luma_33_4x4}
};

MC_PREDICTION_LUMA_FUNC AddLumaBlock[7][16] = {
	{add_block_00_16x16,add_luma_10_16x16,add_luma_20_16x16,add_luma_30_16x16,add_luma_01_16x16,add_luma_11_16x16,
	 add_luma_21_16x16,add_luma_31_16x16,add_luma_02_16x16,add_luma_12_16x16,add_luma_22_16x16,add_luma_32_16x16,
	 add_luma_03_16x16,add_luma_13_16x16,add_luma_23_16x16,add_luma_33_16x16},
	{add_block_00_16x8,add_luma_10_16x8,add_luma_20_16x8,add_luma_30_16x8,add_luma_01_16x8,add_luma_11_16x8,
	 add_luma_21_16x8,add_luma_31_16x8,add_luma_02_16x8,add_luma_12_16x8,add_luma_22_16x8,add_luma_32_16x8,
	 add_luma_03_16x8,add_luma_13_16x8,add_luma_23_16x8,add_luma_33_16x8},
	{add_block_00_8x16,add_luma_10_8x16,add_luma_20_8x16,add_luma_30_8x16,add_luma_01_8x16,add_luma_11_8x16,
	 add_luma_21_8x16,add_luma_31_8x16,add_luma_02_8x16,add_luma_12_8x16,add_luma_22_8x16,add_luma_32_8x16,
	 add_luma_03_8x16,add_luma_13_8x16,add_luma_23_8x16,add_luma_33_8x16},
	{add_block_00_8x8,add_luma_10_8x8,add_luma_20_8x8,add_luma_30_8x8,add_luma_01_8x8,add_luma_11_8x8,
	 add_luma_21_8x8,add_luma_31_8x8,add_luma_02_8x8,add_luma_12_8x8,add_luma_22_8x8,add_luma_32_8x8,
	 add_luma_03_8x8,add_luma_13_8x8,add_luma_23_8x8,add_luma_33_8x8},
	{add_block_00_8x4,add_luma_10_8x4,add_luma_20_8x4,add_luma_30_8x4,add_luma_01_8x4,add_luma_11_8x4,
	 add_luma_21_8x4,add_luma_31_8x4,add_luma_02_8x4,add_luma_12_8x4,add_luma_22_8x4,add_luma_32_8x4,
	 add_luma_03_8x4,add_luma_13_8x4,add_luma_23_8x4,add_luma_33_8x4},
	{add_block_00_4x8,add_luma_10_4x8,add_luma_20_4x8,add_luma_30_4x8,add_luma_01_4x8,add_luma_11_4x8,
	 add_luma_21_4x8,add_luma_31_4x8,add_luma_02_4x8,add_luma_12_4x8,add_luma_22_4x8,add_luma_32_4x8,
	 add_luma_03_4x8,add_luma_13_4x8,add_luma_23_4x8,add_luma_33_4x8},
	{add_block_00_4x4,add_luma_10_4x4,add_luma_20_4x4,add_luma_30_4x4,add_luma_01_4x4,add_luma_11_4x4,
	 add_luma_21_4x4,add_luma_31_4x4,add_luma_02_4x4,add_luma_12_4x4,add_luma_22_4x4,add_luma_32_4x4,
	 add_luma_03_4x4,add_luma_13_4x4,add_luma_23_4x4,add_luma_33_4x4}
};
#endif
#if defined(VOSSSE3)
MC_PREDICTION_CHROMA_FUNC GetChromaBlockSSSE3[7][4] = {
	{get_block_00_8x8_chroma_ssse3,get_chroma_0X_8x8_ssse3,get_chroma_X0_8x8_ssse3,get_chroma_XX_8x8_ssse3},
	{get_block_00_8x4_chroma_ssse3,get_chroma_0X_8x4_ssse3,get_chroma_X0_8x4_ssse3,get_chroma_XX_8x4_ssse3},
	{get_block_00_4x8_chroma_ssse3,get_chroma_0X_4x8_ssse3,get_chroma_X0_4x8_ssse3,get_chroma_XX_4x8_ssse3},
	{get_block_00_4x4_chroma_ssse3,get_chroma_0X_4x4_ssse3,get_chroma_X0_4x4_ssse3,get_chroma_XX_4x4_ssse3},
	{get_block_00_4x2,get_chroma_0X_4x2,get_chroma_X0_4x2,get_chroma_XX_4x2},
	{get_block_00_2x4,get_chroma_0X_2x4,get_chroma_X0_2x4,get_chroma_XX_2x4},
	{get_block_00_2x2,get_chroma_0X_2x2,get_chroma_X0_2x2,get_chroma_XX_2x2}
};

MC_PREDICTION_CHROMA_FUNC AddChromaBlockSSSE3[7][4] = {
	{add_block_00_8x8_chroma_ssse3,add_chroma_0X_8x8_ssse3,add_chroma_X0_8x8_ssse3,add_chroma_XX_8x8_ssse3},
	{add_block_00_8x4_chroma_ssse3,add_chroma_0X_8x4_ssse3,add_chroma_X0_8x4_ssse3,add_chroma_XX_8x4_ssse3},
	{add_block_00_4x8_chroma_ssse3,add_chroma_0X_4x8_ssse3,add_chroma_X0_4x8_ssse3,add_chroma_XX_4x8_ssse3},
	{add_block_00_4x4_chroma_ssse3,add_chroma_0X_4x4_ssse3,add_chroma_X0_4x4_ssse3,add_chroma_XX_4x4_ssse3},
	{add_block_00_4x2,add_chroma_0X_4x2,add_chroma_X0_4x2,add_chroma_XX_4x2},
	{add_block_00_2x4,add_chroma_0X_2x4,add_chroma_X0_2x4,add_chroma_XX_2x4},
	{add_block_00_2x2,add_chroma_0X_2x2,add_chroma_X0_2x2,add_chroma_XX_2x2}
};

MC_PREDICTION_CHROMA_FUNC GetChromaBlockSSE2[7][4] = {
	{get_block_00_8x8_chroma_sse2,get_chroma_0X_8x8_sse2,get_chroma_X0_8x8_sse2,get_chroma_XX_8x8_sse2},
	{get_block_00_8x4_chroma_sse2,get_chroma_0X_8x4_sse2,get_chroma_X0_8x4_sse2,get_chroma_XX_8x4_sse2},
	{get_block_00_4x8_chroma_sse2,get_chroma_0X_4x8_sse2,get_chroma_X0_4x8_sse2,get_chroma_XX_4x8_sse2},
	{get_block_00_4x4_chroma_sse2,get_chroma_0X_4x4_sse2,get_chroma_X0_4x4_sse2,get_chroma_XX_4x4_sse2},
	{get_block_00_4x2,get_chroma_0X_4x2,get_chroma_X0_4x2,get_chroma_XX_4x2},
	{get_block_00_2x4,get_chroma_0X_2x4,get_chroma_X0_2x4,get_chroma_XX_2x4},
	{get_block_00_2x2,get_chroma_0X_2x2,get_chroma_X0_2x2,get_chroma_XX_2x2}
};

MC_PREDICTION_CHROMA_FUNC AddChromaBlockSSE2[7][4] = {
	{add_block_00_8x8_chroma_sse2,add_chroma_0X_8x8_sse2,add_chroma_X0_8x8_sse2,add_chroma_XX_8x8_sse2},
	{add_block_00_8x4_chroma_sse2,add_chroma_0X_8x4_sse2,add_chroma_X0_8x4_sse2,add_chroma_XX_8x4_sse2},
	{add_block_00_4x8_chroma_sse2,add_chroma_0X_4x8_sse2,add_chroma_X0_4x8_sse2,add_chroma_XX_4x8_sse2},
	{add_block_00_4x4_chroma_sse2,add_chroma_0X_4x4_sse2,add_chroma_X0_4x4_sse2,add_chroma_XX_4x4_sse2},
	{add_block_00_4x2,add_chroma_0X_4x2,add_chroma_X0_4x2,add_chroma_XX_4x2},
	{add_block_00_2x4,add_chroma_0X_2x4,add_chroma_X0_2x4,add_chroma_XX_2x4},
	{add_block_00_2x2,add_chroma_0X_2x2,add_chroma_X0_2x2,add_chroma_XX_2x2}
};
#else
MC_PREDICTION_CHROMA_FUNC GetChromaBlock[7][4] = {
	{get_block_00_8x8_chroma,get_chroma_0X_8x8,get_chroma_X0_8x8,get_chroma_XX_8x8},
	{get_block_00_8x4_chroma,get_chroma_0X_8x4,get_chroma_X0_8x4,get_chroma_XX_8x4},
	{get_block_00_4x8_chroma,get_chroma_0X_4x8,get_chroma_X0_4x8,get_chroma_XX_4x8},
	{get_block_00_4x4_chroma,get_chroma_0X_4x4,get_chroma_X0_4x4,get_chroma_XX_4x4},
	{get_block_00_4x2,get_chroma_0X_4x2,get_chroma_X0_4x2,get_chroma_XX_4x2},
	{get_block_00_2x4,get_chroma_0X_2x4,get_chroma_X0_2x4,get_chroma_XX_2x4},
	{get_block_00_2x2,get_chroma_0X_2x2,get_chroma_X0_2x2,get_chroma_XX_2x2}
};

MC_PREDICTION_CHROMA_FUNC AddChromaBlock[7][4] = {
	{add_block_00_8x8_chroma,add_chroma_0X_8x8,add_chroma_X0_8x8,add_chroma_XX_8x8},
	{add_block_00_8x4_chroma,add_chroma_0X_8x4,add_chroma_X0_8x4,add_chroma_XX_8x4},
	{add_block_00_4x8_chroma,add_chroma_0X_4x8,add_chroma_X0_4x8,add_chroma_XX_4x8},
	{add_block_00_4x4_chroma,add_chroma_0X_4x4,add_chroma_X0_4x4,add_chroma_XX_4x4},
	{add_block_00_4x2,add_chroma_0X_4x2,add_chroma_X0_4x2,add_chroma_XX_4x2},
	{add_block_00_2x4,add_chroma_0X_2x4,add_chroma_X0_2x4,add_chroma_XX_2x4},
	{add_block_00_2x2,add_chroma_0X_2x2,add_chroma_X0_2x2,add_chroma_XX_2x2}
};
#endif

MC_BI_FUNC GetBIBlock[10] = {
	bi_prediction_16x16,bi_prediction_16x8,bi_prediction_8x16,bi_prediction_8x8,bi_prediction_8x4,
	bi_prediction_4x8,bi_prediction_4x4,bi_prediction_4x2,bi_prediction_2x4,bi_prediction_2x2
};

MC_OFFSET_FUNC GetOffsetBlock[10] = {
	offset_mc_prediction_16x16,offset_mc_prediction_16x8,offset_mc_prediction_8x16,offset_mc_prediction_8x8,
	offset_mc_prediction_8x4,offset_mc_prediction_4x8,offset_mc_prediction_4x4,offset_mc_prediction_4x2,
	offset_mc_prediction_2x4,offset_mc_prediction_2x2
};

#if defined(VOSSSE3)
MC_WEIGHTED_FUNC GetWeightedBlockSSSE3[10] = {
	weighted_mc_prediction_16x16_ssse3,weighted_mc_prediction_16x8_ssse3,
	weighted_mc_prediction_8x16_ssse3,weighted_mc_prediction_8x8_ssse3,
	weighted_mc_prediction_8x4_ssse3,weighted_mc_prediction_4x8_ssse3,
	weighted_mc_prediction_4x4_ssse3,weighted_mc_prediction_4x2,
	weighted_mc_prediction_2x4,weighted_mc_prediction_2x2
};

MC_BIW_FUNC GetBIWBlockSSSE3[10] = {
	weighted_bi_prediction_16x16_ssse3,weighted_bi_prediction_16x8_ssse3,
	weighted_bi_prediction_8x16_ssse3,weighted_bi_prediction_8x8_ssse3,
	weighted_bi_prediction_8x4_ssse3,	weighted_bi_prediction_4x8_ssse3,
	weighted_bi_prediction_4x4_ssse3,
	weighted_bi_prediction_4x2,weighted_bi_prediction_2x4,weighted_bi_prediction_2x2
};

MC_WEIGHTED_FUNC GetWeightedBlockSSE2[10] = {
	weighted_mc_prediction_16x16_sse2,weighted_mc_prediction_16x8_sse2,
	weighted_mc_prediction_8x16_sse2,weighted_mc_prediction_8x8_sse2,
	weighted_mc_prediction_8x4_sse2,weighted_mc_prediction_4x8_sse2,
	weighted_mc_prediction_4x4_sse2,weighted_mc_prediction_4x2,
	weighted_mc_prediction_2x4,weighted_mc_prediction_2x2
};

MC_BIW_FUNC GetBIWBlockSSE2[10] = {
	weighted_bi_prediction_16x16_sse2,weighted_bi_prediction_16x8_sse2,
	weighted_bi_prediction_8x16_sse2,weighted_bi_prediction_8x8_sse2,
	weighted_bi_prediction_8x4_sse2,	weighted_bi_prediction_4x8_sse2,
	weighted_bi_prediction_4x4_sse2,
	weighted_bi_prediction_4x2,weighted_bi_prediction_2x4,weighted_bi_prediction_2x2
};

#else
MC_WEIGHTED_FUNC GetWeightedBlock[10] = {
	weighted_mc_prediction_16x16,weighted_mc_prediction_16x8,weighted_mc_prediction_8x16,weighted_mc_prediction_8x8,
	weighted_mc_prediction_8x4,weighted_mc_prediction_4x8,weighted_mc_prediction_4x4,weighted_mc_prediction_4x2,
	weighted_mc_prediction_2x4,weighted_mc_prediction_2x2
};

MC_BIW_FUNC GetBIWBlock[10] = {
	weighted_bi_prediction_16x16,weighted_bi_prediction_16x8,weighted_bi_prediction_8x16,weighted_bi_prediction_8x8,weighted_bi_prediction_8x4,
	weighted_bi_prediction_4x8,weighted_bi_prediction_4x4,weighted_bi_prediction_4x2,weighted_bi_prediction_2x4,weighted_bi_prediction_2x2
};
#endif

#if defined(VOSSSE3)

#if defined(_MAC_OS) || defined (_LINUX_ANDROID)
#include <xmmintrin.h>
		#ifndef _cpuid_number_
			#if !defined(__PIC__) || !defined(__i386__)
				#define __cpuid_count(index, number, a0, a1, a2, a3)        \
					__asm__ ("cpuid\n\t"                    \
					: "=a" (a0), "=b" (a1), "=c" (a2), "=d" (a3)    \
					: "0" (index), "2" (number))

				#define __cpuid(index, a0, a1, a2, a3)            \
					__asm__ ("cpuid\n\t"                    \
					: "=a" (a0), "=b" (a1), "=c" (a2), "=d" (a3)    \
					: "0" (index))
			#else
				#if __GNUC__ < 3
					#define __cpuid_count(index, count, a0, a1, a2, a3)        \
						__asm__ ("xchgl\t%%ebx, %1\n\t"            \
						"cpuid\n\t"                    \
						"xchgl\t%%ebx, %1\n\t"            \
						: "=a" (a0), "=r" (a1), "=c" (a2), "=d" (a3)    \
						: "0" (index), "2" (number))

					#define __cpuid(index, a0, a1, a2, a3)            \
						__asm__ ("xchgl\t%%ebx, %1\n\t"            \
						"cpuid\n\t"                    \
						"xchgl\t%%ebx, %1\n\t"            \
						: "=a" (a0), "=r" (a1), "=c" (a2), "=d" (a3)    \
						: "0" (index))
				#else
					#define __cpuid_count(index, number, a0, a1, a2, a3)        \
						__asm__ ("xchg{l}\t{%%}ebx, %1\n\t"            \
						"cpuid\n\t"                    \
						"xchg{l}\t{%%}ebx, %1\n\t"            \
						: "=a" (a0), "=r" (a1), "=c" (a2), "=d" (a3)    \
						: "0" (index), "2" (number))

					#define __cpuid(index, a0, a1, a2, a3)            \
						__asm__ ("xchg{l}\t{%%}ebx, %1\n\t"            \
						"cpuid\n\t"                    \
						"xchg{l}\t{%%}ebx, %1\n\t"            \
						: "=a" (a0), "=r" (a1), "=c" (a2), "=d" (a3)    \
						: "0" (index))
				#endif
			#endif
		#endif    // #ifndef _cpuid_number_
#endif

void perform_mc_init(void)
{
     int CPUInfo[4];
     VO_S32 is_ssse3;

#if defined (_MAC_OS) || defined (_LINUX_ANDROID)
	 __cpuid(1 , CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#else
	 __cpuid(CPUInfo,  1);
#endif

     is_ssse3 = !!(CPUInfo[2] & 0x200);
     if(is_ssse3)
     {
          memcpy(GetLumaBlock, GetLumaBlockSSSE3, sizeof(GetLumaBlock));
	   memcpy(AddLumaBlock, AddLumaBlockSSSE3, sizeof(AddLumaBlock));
	   memcpy(GetChromaBlock, GetChromaBlockSSSE3, sizeof(GetChromaBlock));
	   memcpy(AddChromaBlock, AddChromaBlockSSSE3, sizeof(AddChromaBlock));
	   memcpy(GetWeightedBlock, GetWeightedBlockSSSE3, sizeof(GetWeightedBlock));
	   memcpy(GetBIWBlock, GetBIWBlockSSSE3, sizeof(GetBIWBlock));
     }
     else
     {
          memcpy(GetLumaBlock, GetLumaBlockSSE2, sizeof(GetLumaBlock));
	   memcpy(AddLumaBlock, AddLumaBlockSSE2, sizeof(AddLumaBlock));
	   memcpy(GetChromaBlock, GetChromaBlockSSE2, sizeof(GetChromaBlock));
	   memcpy(AddChromaBlock, AddChromaBlockSSE2, sizeof(AddChromaBlock));
	   memcpy(GetWeightedBlock, GetWeightedBlockSSE2, sizeof(GetWeightedBlock));
	   memcpy(GetBIWBlock, GetBIWBlockSSE2, sizeof(GetBIWBlock));     
     }
}
#endif


#endif


