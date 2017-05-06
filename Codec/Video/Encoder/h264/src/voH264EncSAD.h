/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264ENC_SAD_H_
#define _VOH264ENC_SAD_H_

#include "voH264EncID.h"

typedef VO_S32 (*SadFunc)(VO_U8 *cur_mb, VO_U8 *ref_mb,VO_U32 cur_stride,
                           VO_U32 ref_stride,VO_U32 best_sad);

extern SadFunc SadBlock[2];
extern	VO_S32 Var28x8_C( VO_U8 *pix1, VO_U8 *pix2, VO_S32 i_stride1, VO_S32 i_stride2, VO_S32 *ssd );

#if defined(VOARMV7)

extern VO_S32 sad_16x16_ARMV7( VO_U8 *ptr_cur_mb,VO_U8 * ptr_ref_mb,VO_U32 cur_stride,
				  VO_U32 ref_stride,VO_U32 best_sad);
extern VO_S32 sad_8x8_ARMV7( VO_U8 *ptr_cur_mb,VO_U8 * ptr_ref_mb,VO_U32 cur_stride,
				  VO_U32 ref_stride,VO_U32 best_sad);
extern VO_S32 ssd_8x8_ARMV7( VO_U8 *src1,VO_U8 *src2,VO_S32 stride1, VO_S32 stride2);
extern VO_S32 Var2_8x8_ARMV7( VO_U8 *pix1, VO_U8 *pix2, VO_S32 i_stride1, VO_S32 i_stride2, VO_S32 *ssd );
#define	sad_16x16	sad_16x16_ARMV7
#define	sad_8x8		sad_8x8_ARMV7
#define	ssd_8x8		ssd_8x8_ARMV7
#define	Var2_8x8	Var2_8x8_ARMV7

//#define	sad_16x16	Sad16x16_C
//#define	sad_8x8		Sad8x8_C
//#define	ssd_8x8		SSD8x8_C
//#define	Var2_8x8	Var28x8_C

#else
extern VO_S32 SSD8x8_C( VO_U8 *src1,VO_U8 *src2,VO_S32 stride1, VO_S32 stride2);
extern VO_S32 Sad16x16_C( VO_U8 *ptr_cur_mb,VO_U8 * ptr_ref_mb,VO_U32 cur_stride,
				  VO_U32 ref_stride,VO_U32 best_sad);
extern VO_S32 Sad8x8_C(VO_U8 *ptr_cur_mb,VO_U8 *ptr_ref_mb,VO_U32 cur_stride,
				VO_U32 ref_stride,VO_U32 best_sad);

#define	sad_16x16	Sad16x16_C
#define	sad_8x8		Sad8x8_C
#define	ssd_8x8		SSD8x8_C
#define	Var2_8x8	Var28x8_C


#endif

#endif// _VOH264ENC_SAD_H_
