/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _ENCODER_SAD_H_
#define _ENCODER_SAD_H_

#include "../voMpegProtect.h"
#include "voType.h"

extern VO_U32 Sad16x16_C(const VO_U8 * const cur,
                                    const VO_U8 * const ref,
                                    const VO_U32 cur_stride,
                                    const VO_U32 ref_stride,
                                    const VO_U32 best_sad);
extern VO_U32 Sad16x16_ARMV5(const VO_U8 * const cur,
                                    const VO_U8 * const ref,
                                    const VO_U32 cur_stride,
                                    const VO_U32 ref_stride,
                                    const VO_U32 best_sad);
extern VO_U32 Sad16x16_ARMV6(const VO_U8 * const cur,
                                    const VO_U8 * const ref,
                                    const VO_U32 cur_stride,
                                    const VO_U32 ref_stride,
                                    const VO_U32 best_sad);
extern VO_U32 Sad16x16_ARMV7(const VO_U8 * const cur,
                                    const VO_U8 * const ref,
                                    const VO_U32 cur_stride,
                                    const VO_U32 ref_stride,
                                    const VO_U32 best_sad);

extern VO_U32 Sad8x8_C(const VO_U8 * const cur,
                                  const VO_U8 * const ref,
                                  const VO_U32 cur_stride,
                                  const VO_U32 ref_stride);
extern VO_U32 Sad8x8_ARMV4(const VO_U8 * const cur,
                                  const VO_U8 * const ref,
                                  const VO_U32 cur_stride,
                                  const VO_U32 ref_stride);
extern VO_U32 Sad8x8_ARMV6(const VO_U8 * const cur,
                                  const VO_U8 * const ref,
                                  const VO_U32 cur_stride,
                                  const VO_U32 ref_stride);
extern VO_U32 Sad8x8_ARMV7(const VO_U8 * const cur,
                                  const VO_U8 * const ref,
                                  const VO_U32 cur_stride,
                                  const VO_U32 ref_stride);

extern VO_U32 Dev16x16_C(const VO_U8 * const cur, const VO_U32 stride);
extern VO_U32 Dev16x16_ARMV6(const VO_U8 * const cur, const VO_U32 stride);
extern VO_U32 Dev16x16_ARMV7(const VO_U8 * const cur, const VO_U32 stride);

extern VO_U32 Sad16x16FourMv_C( VO_U8 * cur,
                                                VO_U8 *  ref,
                                               const VO_U32 cur_stride, 
                                               const VO_U32 ref_stride,
                                               VO_U32 *sad8);

VO_VOID HalfpelRefineSad16x16(VO_U8 *cur,
                                    VO_U8 *ref, 
                                    const VO_S32 ref_stride, 
                                    const VO_S32 rounding, 
                                    VO_U32* tmp_sad8);

#if defined (VOWMMX)
#define Sad16     sad16_wmmx
#define Sad8      sad8_wmmx
#define Dev16     dev16_wmmx
#define Sad16_4MV  sad16v_wmmx
#define Sad16v2  sad16v_wmmx2

#elif defined (VOARMV4)
#define Sad16        Sad16x16_ARMV5
#define Sad8          Sad8x8_ARMV4
#define Dev16         Dev16x16_C
#define Sad16_4MV Sad16x16FourMv_C
#elif defined (VOARMV6)
#define Sad16         Sad16x16_ARMV6
#define Sad8          Sad8x8_ARMV6
#define Dev16         Dev16x16_ARMV6
#define Sad16_4MV Sad16x16FourMv_C
#elif defined (VOARMV7)
#define Sad16         Sad16x16_ARMV7
#define Sad8          Sad8x8_ARMV7
#define Dev16         Dev16x16_ARMV7
#define Sad16_4MV  Sad16x16FourMv_C
#else
#define Sad16        Sad16x16_C
#define Sad8          Sad8x8_C
#define Dev16         Dev16x16_C
#define Sad16_4MV  Sad16x16FourMv_C
#endif

#endif/* _ENCODER_SAD_H_ */
