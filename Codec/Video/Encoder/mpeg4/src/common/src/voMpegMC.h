/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __MCOMP_CC_H_
#define __MCOMP_CC_H_
#include "../../voMpegProtect.h"
#include "voType.h" 

#define MV_SUB(x, y) (x&1)+((y&1)<<1)

typedef	VO_VOID (*COPYBLOCK)(VO_U8*,VO_U8*,VO_S32,VO_S32,VO_S32);
typedef	VO_VOID (*ADDBLOCK)(VO_U8*,VO_U8*,VO_S32,VO_S32,VO_S32);
typedef  VO_VOID (*SUBBLOCK)(VO_U8*, VO_S16*, VO_U8*, const VO_S32);

#if defined VOWMMX
SUBBLOCK SubBlock_WMMX;
SUBBLOCK SubBlockHor_WMMX0;
SUBBLOCK SubBlockHor_WMMX1;
SUBBLOCK SubBlockVer_WMMX0;
SUBBLOCK SubBlockVer_WMMX1;
SUBBLOCK SubBlockHorVer_WMMX0;
SUBBLOCK SubBlockHorVer_WMMX1;
#elif defined(VOARMV6)
extern VO_VOID SubBlock_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHor_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVer_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVer_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorRound_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVerRound_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVerRound_ARMV6(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
#elif defined(VOARMV7)
extern VO_VOID SubBlock_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHor_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorRound_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVer_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVerRound_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVer_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVerRound_ARMV7(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
#else
extern VO_VOID SubBlock(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHor(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorRound(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVer(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockVerRound(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVer(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
extern VO_VOID SubBlockHorVerRound(VO_U8 *Src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch);
#endif

/*************************************************/

#if defined(VOWMMX)
VO_VOID WmmxInter8x8(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
VO_VOID WmmxInter8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
VO_VOID WmmxInter8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID WmmxInter8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID WmmxInter8x8HRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID WmmxInter8x8VRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID WmmxInter8x8HVRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID wmmx_addblk(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID wmmx_addblkh(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount); 
VO_VOID wmmx_addblkv(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID wmmx_addblkhv(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
#define CopyBlock16x16 CopyBlock16x16_C
#define CopyBlock8x8   CopyBlock8x8_C
#elif defined(VOARMV4)
extern VO_VOID MCMpegBlock_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockHor_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
extern VO_VOID MCMpegBlockVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockHorVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdHor_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdHorVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAdd_ARMV4 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddHor_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddHorVer_ARMV4(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID CopyBlock16x16_ARMV4(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID CopyBlock8x8_ARMV4(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 CopyBlock16x16_ARMV4
#define CopyBlock8x8    CopyBlock8x8_ARMV4
#elif defined(VOARMV6)
extern VO_VOID MCMpegBlock_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockHor_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
extern VO_VOID MCMpegBlockVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockHorVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdHor_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockRdHorVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAdd_ARMV6 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddHor_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID MCMpegBlockAddHorVer_ARMV6(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID CopyBlock16x16_ARMV6(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID CopyBlock8x8_ARMV6(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 CopyBlock16x16_ARMV6 
#define CopyBlock8x8   CopyBlock8x8_ARMV6
#elif defined(VOARMV7)
VO_VOID MCMpegBlock_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockHor_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
VO_VOID MCMpegBlockVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockHorVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockRdHor_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockRdVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockRdHorVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockAdd_ARMV7 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockAddHor_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockAddVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID MCMpegBlockAddHorVer_ARMV7(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID CopyBlock16x16_ARMV7(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
VO_VOID CopyBlock8x8_ARMV7(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 CopyBlock16x16_ARMV7
#define CopyBlock8x8   CopyBlock8x8_ARMV7
#else
extern VO_VOID MCMpegBlock_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlockRd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlockAdd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID CopyBlock16x16_C(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcStride,const VO_S32 DstStride, VO_S32 LoopCount);
extern VO_VOID CopyBlock8x8_C(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcStride,const VO_S32 DstStride, VO_S32 LoopCount);
#define CopyBlock16x16 CopyBlock16x16_C
#define CopyBlock8x8   CopyBlock8x8_C
#endif

extern const COPYBLOCK AllCopyBlock[2][4];
extern const SUBBLOCK   AllSubBlock[2][4];
extern const ADDBLOCK   AllAddBlock[4];

#endif //__MCOMP_H

