/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef __MPEG2_MB_H__
#define __MPEG2_MB_H__
#include "voMpeg2Decoder.h"

#define IDCTSCAN_ZIGZAG		0
#define IDCTSCAN_ALT_HORI	1
#define IDCTSCAN_ALT_VERT	2

#define SAT(Value) (VO_U8)(Value < 0 ? 0: (Value > 255 ? 255: Value))
#define SAT_16(Value) Value <<= Value >> 16; Value |= (Value << 23) >> 31;


typedef VO_VOID (IDCT_BLOCK)(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
typedef IDCT_BLOCK *IDCT_BLOCK_PTR;
typedef VO_VOID (IDCT_CONST8X8)(VO_S32 v, VO_U8 * dst, VO_S32 dest_stride, VO_U8 * src, VO_S32 src_stride);
typedef IDCT_CONST8X8 *IDCT_CONST8X8_PTR;


extern VO_VOID  IdctBlock4x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock4x8Two_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock8x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock1x1_C(VO_S32 v, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock8x4_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock4x4Two_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
extern VO_VOID  IdctBlock4x4_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);


extern VO_VOID  Bit16_IDCT_Block4x4_Block8x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
extern VO_VOID  Bit16_IDCT_Block4x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
extern VO_VOID  Bit16_IDCT_Block8x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
extern VO_VOID  Bit16_IDCT_Const8x8_c(VO_S32 v, VO_S16 * Dst, VO_S32 DstPitch, VO_U8 * Src, VO_S32 SrcPitch);
extern VO_VOID  Bit16_Sat_Add_c(VO_S16 *buf, VO_U8 *src, VO_U8 *dst, VO_S32 buf_stride,VO_S32 src_stride, VO_S32 dst_stride);

#if defined(VOWMMX)
//VO_VOID STDCALL wmmx_transb4x8(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
//VO_VOID STDCALL wmmx_transb4x8_2(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
//VO_VOID STDCALL wmmx_transb8x8(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
//VO_VOID STDCALL wmmx_transc8x8(VO_S32 v,VO_U8 * Dst,VO_S32 DstStride,VO_U8 * src, VO_S32 src_stride);

//#define IDCT_Block4x8 arm_transb4x8
//#define IDCT_Block4x8_2 arm_transb4x8_2
//#define IDCT_Block8x8 arm_transb8x8
//#define IDCT_Const8x8 wmmx_transc8x8
VO_VOID STDCALL ArmIdctA(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctB(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
//VO_VOID STDCALL ArmIdctC(VO_S32 v, VO_U8 * dest, VO_S32 dest_stride, VO_U8 * src, VO_S32 src_stride);
VO_VOID STDCALL WmmxIdctC(VO_S32 v, VO_U8 * dest, VO_S32 dest_stride, VO_U8 * src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctD(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctE(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctF(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctG(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);//huwei 20090904 IDCT4x4

#define IDCT_Block4x8 ArmIdctA
#define IDCT_Block8x8 ArmIdctB
//#define IDCT_Const8x8 ArmIdctC
#define IDCT_Const8x8 WmmxIdctC
#define IDCT_Block4x8_2 ArmIdctD
#define IDCT_Block8x4 ArmIdctE
#define IDCT_Block4x4_2 ArmIdctF
#define IDCT_Block4x4 ArmIdctG

#define Bit16_IDCT_Block4x4_Block8x8 Bit16_IDCT_Block4x4_Block8x8_c
#define Bit16_IDCT_Block4x8 Bit16_IDCT_Block4x8_c
#define Bit16_IDCT_Block8x8 Bit16_IDCT_Block8x8_c
#define Bit16_IDCT_Const8x8 Bit16_IDCT_Const8x8_c
#define Bit16_Sat_Add  Bit16_Sat_Add_c
#elif defined(VOARAMV4) || defined(VOARMV6)

VO_VOID STDCALL Arm11IdctA(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL Arm11IdctB(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL Arm11IdctC(VO_S32 v, VO_U8 * dest, VO_S32 dest_stride, VO_U8 * src, VO_S32 src_stride);
VO_VOID STDCALL Arm11IdctD(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctD(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctE(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL ArmIdctF(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);

VO_VOID STDCALL Bit16ArmIdctA(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
VO_VOID STDCALL Bit16ArmIdctB(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
VO_VOID STDCALL Bit16ArmIdctG(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride);
VO_VOID STDCALL Bit16ArmIdctC(VO_S32 v, VO_S16 * Dst, VO_S32 DstPitch, VO_U8 * Src, VO_S32 SrcPitch);
VO_VOID STDCALL Bit16Armv6SatAdd(VO_S16 *buf, VO_U8 *src, VO_U8 *dst, VO_S32 buf_stride,VO_S32 src_stride, VO_S32 dst_stride);

#define IDCT_Block4x4 Arm11IdctA
#define IDCT_Block8x8 Arm11IdctB
#define IDCT_Const8x8 Arm11IdctC
#define IDCT_Block4x8 Arm11IdctD
#define IDCT_Block4x8_2 ArmIdctD
#define IDCT_Block8x4 ArmIdctE
#define IDCT_Block4x4_2 ArmIdctF

#define Bit16_IDCT_Block4x4_Block8x8 Bit16_IDCT_Block4x4_Block8x8_c
#define Bit16_IDCT_Block4x8 Bit16_IDCT_Block4x8_c
#define Bit16_IDCT_Block8x8 Bit16_IDCT_Block8x8_c
#define Bit16_IDCT_Const8x8 Bit16_IDCT_Const8x8_c
#define Bit16_Sat_Add  Bit16_Sat_Add_c
#elif defined(VOARMv7)
VO_VOID STDCALL Armv7IdctA(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL Armv7IdctB(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL Armv7IdctC(VO_S32 v, VO_U8 * Dst, VO_S32 dest_stride, VO_U8 * src, VO_S32 src_stride);
VO_VOID STDCALL Armv7IdctD(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);		
VO_VOID STDCALL Armv7IdctE(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);
VO_VOID STDCALL Armv7IdctF(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride);

VO_VOID STDCALL Bit16Armv7IdctA(VO_S16 *Block, VO_S16 *Dest, int DestStride, const VO_U8 *Src, int SrcStride);
VO_VOID STDCALL Bit16Armv7IdctB(VO_S16 *Block, VO_S16 *Dest, int DestStride, const VO_U8 *Src, int SrcStride);
VO_VOID STDCALL Bit16Armv7IdctC(int v, VO_S16 * Dst, int DstPitch, VO_U8 * Src, int SrcStride);
VO_VOID STDCALL Bit16Armv7SatAdd(VO_S16 *buf, VO_U8 *src, VO_U8 *dst, VO_S32 buf_stride,VO_S32 src_stride, VO_S32 dst_stride);

#define IDCT_Block4x8 Armv7IdctA
#define IDCT_Block8x8 Armv7IdctB
#define IDCT_Const8x8 Armv7IdctC
#define IDCT_Block4x8_2 Armv7IdctD
#define IDCT_Block8x4 Armv7IdctE
#define IDCT_Block4x4_2 Armv7IdctF
#define IDCT_Block4x4 Armv7IdctA

#define Bit16_IDCT_Block4x8 Bit16Armv7IdctA
#define Bit16_IDCT_Block8x8 Bit16Armv7IdctB
#define Bit16_IDCT_Const8x8 Bit16Armv7IdctC
#define Bit16_IDCT_Block4x4_Block8x8 Bit16Armv7IdctA
#define Bit16_Sat_Add  Bit16Armv7SatAdd
#else

#define IDCT_Block4x8 IdctBlock4x8_C
#define IDCT_Block8x8 IdctBlock8x8_C
#define IDCT_Const8x8 IdctBlock1x1_C
#define IDCT_Block4x8_2 IdctBlock4x8Two_C 

#define IDCT_Block8x4 IdctBlock8x4_C
#define IDCT_Block4x4_2 IdctBlock4x4Two_C
#define IDCT_Block4x4 IdctBlock4x4_C



#define Bit16_IDCT_Block4x4_Block8x8 Bit16_IDCT_Block4x4_Block8x8_c
#define Bit16_IDCT_Block4x8 Bit16_IDCT_Block4x8_c
#define Bit16_IDCT_Block8x8 Bit16_IDCT_Block8x8_c
#define Bit16_IDCT_Const8x8 Bit16_IDCT_Const8x8_c
#define Bit16_Sat_Add  Bit16_Sat_Add_c

#endif

VO_VOID MotionComp(MpegDecode *dec, Mp2DecLocal *mp2_dec_local, int field_based,
				int isMBPat, int dmv_flage);

#endif//__MPEG2_MB_H__