/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/
#ifndef _VOH264_MC_H_
#define _VOH264_MC_H_

#include "voH264EncID.h"


/* Do the MC
 * XXX: Only width = 4, 8 or 16 are valid
 * width == 4 -> height == 4 or 8
 * width == 8 -> height == 4 or 8 or 16
 * width == 16-> height == 8 or 16
 * */
//edit by Really Yang 1210
//typedef void (*MC_COPY_FUNC)( VO_U8 *dst, VO_S32, VO_U8 *src, VO_S32, VO_S32 i_height );
typedef void (*MC_COPY_FUNC)( VO_U8 *src, VO_S32, VO_U8 *dst, VO_S32);

//end of edit


extern void MCLuma( VO_U8 *dst, VO_S32 dst_stride,
                     VO_U8 *src, VO_S32 src_stride,
                     VO_S32 width, VO_S32 height);

extern void MCChroma8x8_c( VO_U8 *dst, VO_S32 i_dst_stride,
                       VO_U8 *src, VO_S32 i_src_stride,
                       VO_S32 dmvx, VO_S32 dmvy );

void Hpel16x16SearchFilter_HV_C( VO_U8 *dsth, VO_U8 *dstv, VO_U8 *src,VO_S32 stride );

void HpelFilter16X16_C_C(VO_U8 *dst, VO_S32 dst_stride,
                     					VO_U8 *src, VO_S32 src_stride,
                     					VO_S16 *buf );

void HpelFilter16X16_V_C(VO_U8 *dst, VO_S32 dst_stride,
                     			VO_U8 *src, VO_S32 src_stride
                     					);

void HpelFilter16X16_H_C(VO_U8 *dst, VO_S32 dst_stride,
                     			VO_U8 *src, VO_S32 src_stride
                     					);

#if defined(VOARMV7)
extern void MCChroma8x8_ARMV7( VO_U8 *dst, VO_S32 i_dst_stride,
                       VO_U8 *src, VO_S32 i_src_stride,
                       VO_S32 dmvx, VO_S32 dmvy);
#endif

extern const MC_COPY_FUNC CopyBlock[2];

#endif//_VOH264_MC_H_
