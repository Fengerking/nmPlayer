/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MOTION_H_
#define _MOTION_H_

#include "../../voMpegProtect.h"

extern const VO_U32 round_tab_76[16];
extern const VO_U32 round_tab_79[4];

bool MotionEstimation(ENCHND * const enc_hnd);

VO_VOID MacroBlockMotionCompensation(const Mpeg4Frame * const ref,
					                        Mpeg4Frame * const cur,
					                        const MACROBLOCK * const mb,
					                        VO_S16 * const dct_codes,
					                        VO_U32 stride,
					                        VO_U32 rounding);


#endif							/* _MOTION_H_ */
