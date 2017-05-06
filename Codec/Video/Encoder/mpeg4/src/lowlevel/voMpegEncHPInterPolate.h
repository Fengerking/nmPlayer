/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _INTERPOLATE_H_
#define _INTERPOLATE_H_
#include "../voMpegProtect.h"
#include "voType.h" 

typedef VO_VOID (INPLACE16_INTERPOLATE_HALFPEL)(VO_U8 * const dst_h,
											                            VO_U8 * const dst_v,
											                            VO_U8 * const dst_hv,
											                            const VO_U8 * const src,
											                            const VO_S32 dst_stride,
											                            const VO_S32 src_stride,
											                            const VO_U32 rounding);

typedef INPLACE16_INTERPOLATE_HALFPEL *INPLACE16_INTERPOLATE_HALFPEL_PTR;

extern INPLACE16_INTERPOLATE_HALFPEL_PTR Inplace16InterpolateHP[2];

#if defined (VOWMMX)
INPLACE16_INTERPOLATE_HALFPEL inplace16_interpolate_halfpel_wmmx0;
INPLACE16_INTERPOLATE_HALFPEL inplace16_interpolate_halfpel_wmmx1;
#elif (defined (VOARMV4) || defined(VOARMV6))
INPLACE16_INTERPOLATE_HALFPEL Inplace16InterpolateHP_ARMV4;
#elif defined(VOARMV7) 
INPLACE16_INTERPOLATE_HALFPEL Inplace16InterpolateHP_ARMV7;
#else
INPLACE16_INTERPOLATE_HALFPEL Inplace16InterpolateHP_C;
#endif


#endif //_INTERPOLATE8X8_H_
