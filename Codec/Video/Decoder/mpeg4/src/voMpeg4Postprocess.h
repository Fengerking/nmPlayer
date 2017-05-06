/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/


#ifndef _POSTPROCESSING_H_
#define _POSTPROCESSING_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"

/* Filtering thresholds */
#if ENABLE_DEBLOCK

#define THR1 2
#define THR2 6


#define DERING_STRENGTH		2


void
image_postproc(POSTPROC *tbls, VO_IMGYUV * img, VO_S32 ExWidth,
				VO_U32 *quant_map, VO_S32 nMBWidth, VO_S32 nMBHeight, VO_S32 mb_stride,
				VO_S32 brightness, VO_S32 pf_mode);

void deblock8x8_h(POSTPROC *tbls, VO_U8 *img, VO_S32 stride, VO_S32 quant, VO_S32 dering);
void deblock8x8_v(POSTPROC *tbls, VO_U8 *img, VO_S32 stride, VO_S32 quant, VO_S32 dering);

void init_deblock(POSTPROC *tbls);

#endif
#endif
