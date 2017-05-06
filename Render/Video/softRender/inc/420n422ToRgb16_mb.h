
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2006				*
	*																		*
	************************************************************************/

#ifndef _420N422TORGB_MB__H_
#define _420N422TORGB_MB__H_
#include "ccConstant.h"

typedef void(CC_MB)(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, VO_S32 in_stride,
					VO_U8 *out_buf, VO_S32 out_stride, VO_S32 width, VO_S32 height,VO_S32 uin_stride, VO_S32 vin_stride);

typedef CC_MB *CC_MB_PTR;


CC_MB cc_mb_16x16_c;
CC_MB cc_mb_16x16_180_c;
CC_MB cc_mb_16x16_l90_c;
CC_MB cc_mb_16x16_r90_c;

CC_MB cc_mb_16x16_arm;
CC_MB cc_mb_16x16_180_arm;
CC_MB cc_mb_16x16_l90_arm;
CC_MB cc_mb_16x16_r90_arm;

CC_MB cc_mb;
CC_MB cc_mb_l90;
CC_MB cc_mb_r90;


#if defined(VOARMV4) || defined(VOARMV6) || defined(VOARMV7)
#define cc_mb_16x16			cc_mb_16x16_arm
#define cc_mb_16x16_180		cc_mb_16x16_180_arm
#define cc_mb_16x16_l90		cc_mb_16x16_l90_arm
#define cc_mb_16x16_r90		cc_mb_16x16_r90_arm
#else
#define cc_mb_16x16			cc_mb_16x16_c
#define cc_mb_16x16_180		cc_mb_16x16_180_c
#define cc_mb_16x16_l90		cc_mb_16x16_l90_c
#define cc_mb_16x16_r90		cc_mb_16x16_r90_c
#endif


#endif//_420N422TORGB_MB__H_