/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegEncHPInterPolate.h"
 
INPLACE16_INTERPOLATE_HALFPEL_PTR Inplace16InterpolateHP[2] = {
#ifdef VOWMMX
	inplace16_interpolate_halfpel_wmmx0,
	inplace16_interpolate_halfpel_wmmx1
#elif(defined(VOARMV4) || defined (VOARMV6))
	Inplace16InterpolateHP_ARMV4,
	Inplace16InterpolateHP_ARMV4
#elif defined(VOARMV7) 
	Inplace16InterpolateHP_ARMV7,
	Inplace16InterpolateHP_ARMV7
#else
	Inplace16InterpolateHP_C,
	Inplace16InterpolateHP_C
#endif
};

__inline VO_VOID Inplace16InterpolateHP_C (VO_U8 * const dst_h,
								                        VO_U8 * const dst_v,
								                        VO_U8 * const dst_hv,
								                        const VO_U8 * const src,
								                        const VO_S32 dst_stride,	//24
								                        const VO_S32 src_stride,
								                        const VO_U32 rounding)
{
    VO_S32 i, j, dst_pos = 0, src_pos = 0;
    const VO_U32 k = 1 - rounding;
    for(i = 0; i < 17; i++){
        for(j = 0; j < 17; j++){
            dst_pos = dst_stride*i+j;
            src_pos = src_stride*i+j;
            dst_h[dst_pos] = (VO_U8)((src[src_pos-1] + src[src_pos] + k)>>1);
            dst_v[dst_pos] = (VO_U8)((src[src_pos-src_stride] + src[src_pos] + k)>>1);
            dst_hv[dst_pos] = (VO_U8)((src[src_pos-1] + src[src_pos-1-src_stride] + src[src_pos-src_stride] + src[src_pos] +1 + k)>>2);
        }
    }
}
