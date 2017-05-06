/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264_INTRAPREDICTION_H_
#define _VOH264_INTRAPREDICTION_H_

#include "voH264EncID.h"

typedef void (*PredictFunc)( VO_U8 *src, VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride  );

enum PREDMODE_INTRA_CHROMA
{
  INTRA_CHROMA_DC = 0,
  INTRA_CHROMA_H  = 1,
  INTRA_CHROMA_V  = 2,
  INTRA_CHROMA_P  = 3,
  INTRA_CHROMA_DC_LEFT = 4,
  INTRA_CHROMA_DC_TOP  = 5,
  INTRA_CHROMA_DC_128  = 6
};
static const VO_U8 intra_chroma_mode_index[7] =
{
  INTRA_CHROMA_DC, INTRA_CHROMA_H, INTRA_CHROMA_V, INTRA_CHROMA_P,
  INTRA_CHROMA_DC, INTRA_CHROMA_DC,INTRA_CHROMA_DC
};

enum PREDMODE_INTRA_LUMA
{
  INTRA_LUMA_V  = 0,
  INTRA_LUMA_H  = 1,
  INTRA_LUMA_DC = 2,
  INTRA_LUMA_P  = 3,
  INTRA_LUMA_DC_LEFT = 4,
  INTRA_LUMA_DC_TOP  = 5,
  INTRA_LUMA_DC_128  = 6,
};
static const VO_U8 intra_luma_mode_index[7] =
{
  INTRA_LUMA_V, INTRA_LUMA_H, INTRA_LUMA_DC, INTRA_LUMA_P,
  INTRA_LUMA_DC,INTRA_LUMA_DC,INTRA_LUMA_DC
};
static const VO_U8 intra_chroma_mode_bits[7] =
{
  1, 3, 3, 5, 1, 1, 1
};
static const VO_U8 intra_luma_mode_bits[7] =
{
  1, 3, 3, 5, 3, 3, 3
};

extern const PredictFunc  PredictIntraLuma16x16[7];
extern const PredictFunc  PredictIntraChroma8x8[7];

#if defined(VOARMV7)

extern	void PredIntraLuma16x16P_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraLuma16x16Dc_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraLuma16x16H_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraLuma16x16V_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraChroma8x8H_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraChroma8x8V_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
#define	PredIntraLuma16x16P			PredIntraLuma16x16P_ARMV7
#define	PredIntraLuma16x16Dc		PredIntraLuma16x16Dc_ARMV7
#define	PredIntraLuma16x16H			PredIntraLuma16x16H_ARMV7
#define	PredIntraLuma16x16V			PredIntraLuma16x16V_ARMV7
#define	PredIntraChroma8x8H			PredIntraChroma8x8H_ARMV7
#define	PredIntraChroma8x8V			PredIntraChroma8x8V_ARMV7

#else

#define	PredIntraLuma16x16P			PredIntraLuma16x16P_C
#define	PredIntraLuma16x16Dc		PredIntraLuma16x16Dc_C
#define	PredIntraLuma16x16H			PredIntraLuma16x16H_C
#define	PredIntraLuma16x16V			PredIntraLuma16x16V_C
#define	PredIntraChroma8x8H			PredIntraChroma8x8H_C
#define	PredIntraChroma8x8V			PredIntraChroma8x8V_C

#endif

#endif//_VOH264_INTRAPREDICTION_H_
