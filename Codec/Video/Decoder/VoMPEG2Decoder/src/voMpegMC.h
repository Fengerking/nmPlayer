/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __MCOMP_H
#define __MCOMP_H
#include "voMpegPort.h"


#define EDGE 32
#define MAXBUF 5

typedef	VO_VOID (COPYBLOCK)(VO_U8 *,VO_U8*, const VO_S32, const VO_S32, VO_S32);
typedef	VO_VOID (*addblock)(VO_U8*,VO_U8*, const VO_S32, const VO_S32, VO_S32);

typedef COPYBLOCK *COPYBLOCK_PTR; 


#if defined(VOWMMX)
VO_VOID wmmx_copyblk(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkh(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkhv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkhrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_copyblkhvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_addblk(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_addblkh(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_addblkv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_addblkhv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_CopyBlock16x16(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_CopyBlock8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

#define CopyBlock16x16 wmmx_CopyBlock16x16
#define CopyBlock8x8   wmmx_CopyBlock8x8
//#define CopyBlock16x16 CopyBlock16x16_C
//#define CopyBlock8x8   CopyBlock8x8_C
#elif defined(VOARMV4) || defined(VOARMV6)
VO_VOID ArmInter8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter8x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID ArmInter8x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter8x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkhrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkhvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_8x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_8x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_8x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);;
VO_VOID ArmCopy16x16(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmCopy8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

#define CopyBlock16x16 ArmCopy16x16
#define CopyBlock8x8   ArmCopy8x8
#elif defined(VOARMv7)
VO_VOID MPEG2DEC_VO_Armv7Inter8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter8x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID MPEG2DEC_VO_Armv7Inter8x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter8x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkhrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID arm_copyblkhvrd(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_8x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_8x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_8x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Copy16x16(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Copy8x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

#define CopyBlock16x16 MPEG2DEC_VO_Armv7Copy16x16
#define CopyBlock8x8   MPEG2DEC_VO_Armv7Copy8x8
#else

extern VO_VOID MCMpegBlock_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock16x16_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock8x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlockAdd_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAddHorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);


#define CopyBlock16x16 CopyBlock16x16_C
#define CopyBlock8x8   CopyBlock8x8_C
#endif

extern const COPYBLOCK_PTR AllCopyBlock[8];
extern const addblock  AllAddBlock[8];

extern VO_VOID MCMpegBlock4x8_01(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8_03(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8_11(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8_13(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlockAdd4x8_01(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8_03(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8_11(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8_13(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);


#if defined(VOWMMX)
VO_VOID wmmx4x8_copyblk(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_copyblkh(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_copyblkv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_copyblkhv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_addblk(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_addblkh(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_addblkv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx4x8_addblkhv(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID wmmx_CopyBlock8x16(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);
VO_VOID wmmx_CopyBlock4x8(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);

#define CopyBlock8x16 wmmx_CopyBlock8x16
#define CopyBlock4x8  wmmx_CopyBlock4x8
#elif defined(VOARMV4) || defined(VOARMV6)
VO_VOID ArmInter4x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter4x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID ArmInter4x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter4x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_4x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_4x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_4x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmInter2_4x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID ArmCopy8x16(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);
VO_VOID ArmCopy4x8(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);

#define CopyBlock8x16 ArmCopy8x16
#define CopyBlock4x8   ArmCopy4x8

#elif defined(VOARMv7)

#if 1
//VO_VOID MPEG2DEC_VO_Armv7Inter4x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter4x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
//VO_VOID MPEG2DEC_VO_Armv7Inter4x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter4x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
//VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Copy8x16(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Copy4x8(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

extern VO_VOID MCMpegBlock4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8Hor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8Ver_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8HorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

// AddBlock DstPitch=8
extern VO_VOID MCMpegBlockAdd4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8Hor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8Ver_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8HorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

#else
VO_VOID MPEG2DEC_VO_Armv7Inter4x8(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter4x8H(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height); 
VO_VOID MPEG2DEC_VO_Armv7Inter4x8V(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter4x8HV(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8H(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8V(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Inter2_4x8HV(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);;
VO_VOID MPEG2DEC_VO_Armv7Copy8x16(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID MPEG2DEC_VO_Armv7Copy4x8(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);

#endif

#define CopyBlock8x16 MPEG2DEC_VO_Armv7Copy8x16
#define CopyBlock4x8   MPEG2DEC_VO_Armv7Copy4x8
#else

extern VO_VOID MCMpegBlock4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8Hor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8Ver_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlock4x8HorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock8x16_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID CopyBlock4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

// AddBlock DstPitch=8
extern VO_VOID MCMpegBlockAdd4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8Hor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8Ver_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
extern VO_VOID MCMpegBlockAdd4x8HorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);

#define CopyBlock8x16 CopyBlock8x16_C
#define CopyBlock4x8   CopyBlock4x8_C
#endif


extern const COPYBLOCK_PTR NewAllCopyBlock4_8[8];
extern const addblock  NewAllAddBlock4x8[8];

#ifdef BFRAME_NOERR


#if defined(VOWMMX)
VO_VOID BArmInter8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID BArmInter8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmCopy16x8(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID BArmCopy8x4(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID ArmCopy8x4(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);

//#define CopyBlock8x4		ArmCopy8x4
//#define BCopyBlock16x8	BArmCopy16x8
//#define BCopyBlock8x4	BArmCopy8x4
#elif defined(VOARMV4) || defined(VOARMV6)
VO_VOID BArmInter8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID BArmInter8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmInter2_8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID BArmCopy16x8(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID BArmCopy8x4(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);
VO_VOID ArmCopy8x4(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, const VO_S32 height);

//#define CopyBlock8x4		ArmCopy8x4
//#define BCopyBlock16x8	BArmCopy16x8
//#define BCopyBlock8x4		BArmCopy8x4

#elif defined(VOARMv7)
VO_VOID MPEG2DEC_VO_BArmv7Inter8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height); 
VO_VOID MPEG2DEC_VO_BArmv7Inter8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter2_8x4(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter2_8x4H(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter2_8x4V(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
VO_VOID MPEG2DEC_VO_BArmv7Inter2_8x4HV(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height);
#else

extern VO_VOID BMCMpegBlockVer8x4_C(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);
extern VO_VOID BMCMpegBlockHorVer8x4_C(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);

// AddBlock DstPitch=8
extern VO_VOID BMCMpegBlockAddVer8x4_C(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);
extern VO_VOID BMCMpegBlockAddHorVer8x4_C(VO_U8 * Src, VO_U8 * Dst, const VO_S32 SrcPitch, const VO_S32 DstPitch, VO_S32 height);


#endif

extern const COPYBLOCK_PTR BAllCopyBlock8x4[8];
extern const addblock  BAllAddBlock8x4[8];

#endif //BFRAME_NOERR

//extern COPYBLOCK_PTR CopyBlock16x16, CopyBlock8x8;
#endif //__MCOMP_H
