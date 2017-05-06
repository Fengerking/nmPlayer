/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __IDCT_H__
#define __IDCT_H__
#include "voMpeg4DecGlobal.h"

#define SAT(Value) (VO_U8)(Value < 0 ? 0: (Value > 255 ? 255: Value))

#if  defined(VOARMV4) || defined(VOWMMX)
void STDCALL ArmIdctA(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL ArmIdctB(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL ArmIdctC(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL ArmIdctD(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
#define IDCTBlock4x4 ArmIdctA	//Arm11IdctA
#define IDCTBlock8x8 ArmIdctB
#define IDCT_Block1x1 ArmIdctC
#define IDCTBlock4x8 ArmIdctD	//Arm11IdctD

#elif defined(VOARMV6)
void STDCALL Arm11IdctA(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctB(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctC(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctD(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
#define IDCTBlock4x4 Arm11IdctA	//Arm11IdctA
#define IDCTBlock8x8 Arm11IdctB
#define IDCT_Block1x1 Arm11IdctC
#define IDCTBlock4x8 Arm11IdctD	//Arm11IdctD
#elif defined(VOARMV7)
// #if 1
#ifdef _IOS
void STDCALL Arm11IdctA(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctB(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctC(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL Arm11IdctD(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
#define IDCTBlock4x4 Arm11IdctA	//Arm11IdctA
#define IDCTBlock8x8 Arm11IdctB
#define IDCT_Block1x1 Arm11IdctC
#define IDCTBlock4x8 Arm11IdctD	//Arm11IdctD
#else
void STDCALL MPEG4DEC_VO_Armv7IdctA(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL MPEG4DEC_VO_Armv7IdctB(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL MPEG4DEC_VO_Armv7IdctC(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
void STDCALL MPEG4DEC_VO_Armv7IdctD(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
#define IDCTBlock4x4 MPEG4DEC_VO_Armv7IdctA	//Arm11IdctA
#define IDCTBlock8x8 MPEG4DEC_VO_Armv7IdctB
#define IDCT_Block1x1 MPEG4DEC_VO_Armv7IdctC
#define IDCTBlock4x8 MPEG4DEC_VO_Armv7IdctA	//Arm11IdctD
#endif

#else

VO_VOID  IdctBlock1x1_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID  IdctBlock4x4_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID  IdctBlock4x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID  IdctBlock8x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);

#define IDCT_Block1x1 IdctBlock1x1_C
#define IDCTBlock4x4 IdctBlock4x4_C
#define IDCTBlock4x8 IdctBlock4x8_C
#define IDCTBlock8x8 IdctBlock8x8_C

#endif
#endif//__IDCT_H__
