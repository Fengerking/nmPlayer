/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __MCOMP_H
#define __MCOMP_H
#include "voMpeg4DecGlobal.h"


#define EDGE 32
#define MAXBUF 5

typedef	void (COPYBLOCK)(VO_U8*,VO_U8*,VO_S32,VO_S32,VO_S32);
typedef	void (*addblock)(VO_U8*,VO_U8*,VO_S32,VO_S32,VO_S32);

typedef COPYBLOCK *COPYBLOCK_PTR; 


extern VO_VOID AvgBlockb_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock16x16_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock8x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlock_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
// extern VO_VOID MCMpegBlockRd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockRdHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlockAdd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);


// extern VO_VOID MCMpegBlockAddRd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
// extern VO_VOID MCMpegBlockAddRdHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
// extern VO_VOID MCMpegBlockAddRdVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
// extern VO_VOID MCMpegBlockAddRdHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);


#if defined(VOWMMX)
void WmmxInter8x8(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
void WmmxInter8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
void WmmxInter8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void WmmxInter8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void WmmxInter8x8HRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void WmmxInter8x8VRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void WmmxInter8x8HVRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void wmmx_addblk(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void wmmx_addblkh(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount); 
void wmmx_addblkv(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void wmmx_addblkhv(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
#define CopyBlock16x16 CCopy16X16
#define CopyBlock8x8   CCopy8X8
#elif defined(VOARMV4)
void Armv4Inter8x8(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
void Armv4Inter8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter8x8HRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter8x8VRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter8x8HVRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter2_8x8 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter2_8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter2_8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Inter2_8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Copy16x16(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Armv4Copy8x8(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 Armv4Copy16x16
#define CopyBlock8x8   Armv4Copy8x8
#elif defined(VOARMV6)
void Arm11Inter8x8(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
void Arm11Inter8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter8x8HRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter8x8VRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter8x8HVRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter2_8x8 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter2_8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter2_8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Inter2_8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Copy16x16(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void Arm11Copy8x8(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 Arm11Copy16x16
#define CopyBlock8x8   Arm11Copy8x8
#elif defined(VOARMV7)
void MPEG4DEC_VO_Armv7Inter8x8(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount); 
void MPEG4DEC_VO_Armv7Inter8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter8x8HRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter8x8VRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter8x8HVRD(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride, VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter2_8x8 (VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter2_8x8H(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter2_8x8V(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Inter2_8x8HV(VO_U8 * Src, VO_U8 * Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Copy16x16(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);
void MPEG4DEC_VO_Armv7Copy8x8(VO_U8 *Src, VO_U8 *Dst, VO_S32 SrcStride,VO_S32 DstStride, VO_S32 LoopCount);

#define CopyBlock16x16 MPEG4DEC_VO_Armv7Copy16x16
#define CopyBlock8x8   MPEG4DEC_VO_Armv7Copy8x8
#else
#define CopyBlock16x16 CopyBlock16x16_C
#define CopyBlock8x8   CopyBlock8x8_C
#endif

extern const COPYBLOCK_PTR AllCopyBlock[8];
extern const addblock  AllAddBlock[8];
//extern COPYBLOCK_PTR CopyBlock16x16, CopyBlock8x8;
#endif //__MCOMP_H

