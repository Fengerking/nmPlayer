/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __VOMPEG4_GMC_H
#define __VOMPEG4_GMC_H

#include "voMpeg4DecGlobal.h"

typedef struct VO_VIDEO_GMC_DATA
{
	VO_S32 width; /* size * 16 */
	VO_S32 height;
	VO_S32 gmc_accuracy; /* {0,1,2,3}  =>   {1/2,1/4,1/8,1/16} pel */
	VO_S32 dU[2], dV[2], Uo, Vo, Uco, Vco;

	void (*GMCLuma)(VOCONST struct VO_VIDEO_GMC_DATA * VOCONST gmc,VO_U8 *dst, VOCONST VO_U8 *src,
		VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);
	void (*GMCChroma)  (VOCONST struct VO_VIDEO_GMC_DATA * VOCONST gmc,VO_U8 *uDst, VOCONST VO_U8 *uSrc,
		VO_U8 *vDst, VOCONST VO_U8 *vSrc,VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);
	void (*GMCGetAvgMV)(VOCONST struct VO_VIDEO_GMC_DATA * VOCONST gmc, VO_S32* mvx, VO_S32 *mvy,
		VO_S32 x, VO_S32 y, VO_S32 qpel);
} VO_VIDEO_GMC_DATA;


static INLINE VO_S32
GMCSanitize(VO_S32 value, VO_S32 quarterpel, VO_S32 fcode)
{
	VO_S32 length = 1 << (fcode+4);

	if (value < -length)
		return -length;
	else if (value >= length)
		return length-1;
	else 
		return value;
}


void GMCLuma_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
			   VO_U8 *dst, VOCONST VO_U8 *src,
			   VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);

void GMCChroma_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
				 VO_U8 *uDst, VOCONST VO_U8 *uSrc,
				 VO_U8 *vDst, VOCONST VO_U8 *vSrc,
				 VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);

void GMCGetAvgMV_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc, VO_S32* mvx, VO_S32 *mvy,
				   VO_S32 x, VO_S32 y, VO_S32 qpel);


void GMCLuma1Pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
				  VO_U8 *Dst, VOCONST VO_U8 *Src,
				  VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);

void GMCChroma1Pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
					VO_U8 *uDst, VOCONST VO_U8 *uSrc,
					VO_U8 *vDst, VOCONST VO_U8 *vSrc,
					VO_S32 dststride, VO_S32 srcstride, VO_S32 x, VO_S32 y, VO_S32 rounding);

void GMCGetAvgMV1pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc, VO_S32* mvx, VO_S32 *mvy,
					  VO_S32 x, VO_S32 y, VO_S32 qpel);


void IniGMCParam(VO_S32 warp_pt_num, VOCONST VO_S32 gmc_accuracy,VO_S32 *warp_point,
				 VOCONST VO_S32 nWidth, VOCONST VO_S32 nHeight,VO_VIDEO_GMC_DATA *VOCONST gmc);



#endif //__VOMPEG4_GMC_H



