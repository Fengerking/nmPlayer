/**
 *  h265dec_inter.h - HEVC inter block prediction header definitions. This header file
 *  defines the motion compensation interpolation filters methods correspond with 
 *  different PUs and weighted prediction algorithm including luma and chroma(YUV420 format).
 *  Created by RaymanLee.
 *
 *  Copyright (c) 2012 VisualOn, Inc, Confidential and Proprietary.
 *
 */
#ifndef __H265DEC_INTER_H__
#define __H265DEC_INTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "h265dec_config.h"
#include "h265dec_mem.h"
#include "h265dec_ID.h"

#define NTAPS_LUMA        8 ///< Number of taps for luma
#define NTAPS_CHROMA      4 ///< Number of taps for chroma
#define IF_INTERNAL_PREC 14 ///< Number of bits for internal precision
#define IF_FILTER_PREC    6 ///< Log2 of sum of filter taps
#define IF_INTERNAL_OFFS (1<<(IF_INTERNAL_PREC-1)) ///< Offset used internally



typedef struct {
//   // Explicit weighted prediction parameters parsed in slice header,
//   // or Implicit weighted prediction parameters (8 bits depth values).
//   VO_S32          wp_present_flag;
//   VO_U32 wp_log2_weight_denom;
//   VO_S32          wp_weight;
//   VO_S32          wp_offset;

  // Weighted prediction scaling values built from above parameters (bit-depth scaled):
  VO_S32         w, o, offset, shift, round;
} WP_SCALING_PARAM;

#if MC_ASM_ENABLED
extern VO_VOID CopyBlock_4x4_neon(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride);
extern VO_VOID CopyBlock_8x4_neon(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride);
extern VO_VOID CopyBlock_8x8_neon(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride);
extern VO_VOID CopyBlock_16x16_neon(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride);

extern VO_VOID MC_InterLuma_4x4_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );
extern VO_VOID MC_InterLuma_8x8_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );

extern VO_VOID MC_InterChroma_4x4_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );
extern VO_VOID MC_InterChroma_8x8_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );

extern VO_VOID MC_InterLumaBi_4x4_neon(VO_U8* ref, VO_S32 refStride, VO_S16* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );
extern VO_VOID MC_InterLumaBi_8x8_neon(VO_U8* ref, VO_S32 refStride, VO_S16* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );

extern VO_VOID MC_InterChromaBi_4x4_neon(VO_U8* ref, VO_S32 refStride, VO_S16* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );
extern VO_VOID MC_InterChromaBi_8x8_neon(VO_U8* ref, VO_S32 refStride, VO_S16* dst, VO_S32 dstStride, VO_S32 xFrac, VO_S32 yFrac );

extern VO_VOID AddAverageBi_4x4_neon(VO_S16* pred0, VO_S32 predStride0, VO_S16* pred1, VO_S32 predStride1, VO_U8* dst, VO_S32 dstStride);
extern VO_VOID AddAverageBi_8x8_neon(VO_S16* pred0, VO_S32 predStride0, VO_S16* pred1, VO_S32 predStride1, VO_U8* dst, VO_S32 dstStride);

VO_VOID MC_InterLuma_neon( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac 
  );

VO_VOID MC_InterChroma_neon( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac 
  );

VO_VOID MC_InterLumaBi_neon( 
  VO_U8* ref0, 
  VO_S32 refStride0,
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height,
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1
  );

VO_VOID MC_InterChromaBi_neon( 
  VO_U8* ref0, 
  VO_S32 refStride0,
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height,
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1
  );

VO_VOID CopyBlock_neon(
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height
  );

#endif

void MC_InterLuma( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac 
  );

void MC_InterChroma( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac 
  );

void MC_InterLumaBi( 
  VO_U8* ref0, 
  VO_S32 refStride0,
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height,
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1
  );

void MC_InterChromaBi( 
  VO_U8* ref0, 
  VO_S32 refStride0,
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height,
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1
  );

void MC_InterLumaWeighted( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac, 
  const WP_SCALING_PARAM* wp 
  );

void MC_InterChromaWeighted( 
  VO_U8* ref, 
  VO_S32 refStride, 
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac, 
  VO_S32 yFrac, 
  const WP_SCALING_PARAM* wp 
  );

void MC_InterLumaWeightedBi( 
  VO_U8* ref0, 
  VO_S32 refStride0, 
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1,
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  );

void MC_InterChromaWeightedBi( 
  VO_U8* ref0, 
  VO_S32 refStride0, 
  VO_U8* ref1, 
  VO_S32 refStride1,
  VO_U8* dst, 
  VO_S32 dstStride, 
  VO_S32 width, 
  VO_S32 height, 
  VO_S32 xFrac0, 
  VO_S32 yFrac0,
  VO_S32 xFrac1, 
  VO_S32 yFrac1,
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  );

#if MC_ASM_ENABLED
#define ALIGN(ptr, bytePos) \
        (ptr + ( ((bytePos - (int)ptr) & (bytePos - 1)) / sizeof(*ptr) ))

typedef  void (*MC_FUNC)(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
typedef  void (*MC_BI_FUNC)(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
typedef  void (*MC_CHROMA_FUNC)(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
typedef  void (*MC_CHROMA_BI_FUNC)(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);


extern void VO_put_MC_InterLuma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H2V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H3V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H0V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H0V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H2V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H3V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H1V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H2V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H3V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H1V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H2V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_put_MC_InterLuma_H3V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);

extern void VO_avg_MC_InterLuma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H2V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H3V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H0V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H0V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H2V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H3V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H1V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H2V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H3V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H1V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H2V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_MC_InterLuma_H3V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);

extern void VO_avg_nornd_MC_InterLuma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H2V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H3V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H0V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H0V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H2V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H3V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H1V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H2V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H3V2_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H1V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H2V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
extern void VO_avg_nornd_MC_InterLuma_H3V3_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32);
        
extern void VO_put_MC_InterChroma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);        
extern void VO_put_MC_InterChroma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32); 
extern void VO_put_MC_InterChroma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);        
extern void VO_put_MC_InterChroma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);        

extern void VO_avg_MC_InterChroma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_MC_InterChroma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_MC_InterChroma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_MC_InterChroma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);

extern void VO_avg_nornd_MC_InterChroma_H0V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_nornd_MC_InterChroma_H0V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_nornd_MC_InterChroma_H1V0_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
extern void VO_avg_nornd_MC_InterChroma_H1V1_neon(VO_U8*, VO_S32, VO_U8*, VO_S32, VO_S32, VO_S32, VO_S16*, VO_S32, VO_S32, VO_S32);
#endif

#ifdef __cplusplus
}
#endif

#endif // __H265DEC_INTER_H__