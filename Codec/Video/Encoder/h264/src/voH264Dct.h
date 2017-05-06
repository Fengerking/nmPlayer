/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef AVC_DCT_H
#define AVC_DCT_H
#include "voH264EncID.h"

#if defined(VOARMV7)

extern void Sub8x8Dct_ARMV7( VO_S16 *dct, VO_U8 *src, VO_U8 *dst , VO_S32 dst_stride);
extern void Sub16x16Dct_ARMV7( VO_S16 *dct, VO_U8 *src, VO_U8 *dst, VO_S32 dst_stride);
extern void Sub8x8DctDc_ARMV7( VO_S16 dct[4], VO_U8 *src, VO_U8 *dst);
extern void Add8x8Idct_ARMV7( VO_U8 *p_dst, VO_S16 *dct );
extern void Add16x16Idct_ARMV7( VO_U8 *p_dst, VO_S16 *dct );
extern void Add8x8IdctDc_ARMV7( VO_U8 *p_dst, VO_S16 dct[4] );
extern void Add16x16IdctDc_ARMV7( VO_U8 *p_dst, VO_S16 dct[16] );
extern void Zigzag4x4_ARMV7( VO_S16 level[16], VO_S16 dct[16] );
extern void Dct4x4DC_ARMV7( VO_S16* d );
extern void Idct4x4Dc_ARMV7( VO_S16* d );
extern void Sub4x4Dct_ARMV7( VO_S16 *dct, VO_U8 *src, VO_U8 *dst , VO_S32 dst_stride);
extern void Add4x4Idct_ARMV7( VO_U8 *p_dst, VO_S16* dct );

#define Sub4x4Dct		Sub4x4Dct_ARMV7
#define Sub8x8Dct		Sub8x8Dct_ARMV7
#define Sub16x16Dct		Sub16x16Dct_ARMV7
#define Add4x4Idct		Add4x4Idct_ARMV7
#define Add8x8Idct		Add8x8Idct_ARMV7
#define Add16x16Idct	Add16x16Idct_ARMV7
#define Dct4x4DC		Dct4x4DC_ARMV7
#define Idct4x4Dc		Idct4x4Dc_ARMV7
#define Sub8x8DctDc		Sub8x8DctDc_ARMV7
#define Add8x8IdctDc	Add8x8IdctDc_ARMV7
#define Add16x16IdctDc	Add16x16IdctDc_ARMV7
#define Zigzag4x4		Zigzag4x4_ARMV7

//#define Sub4x4Dct		Sub4x4Dct_C
//#define Sub8x8Dct		Sub8x8Dct_C
//#define Sub16x16Dct		Sub16x16Dct_C
//#define Add4x4Idct		Add4x4Idct_C
//#define Add8x8Idct		Add8x8Idct_C
//#define Add16x16Idct	Add16x16Idct_C
//#define Dct4x4DC		Dct4x4DC_C
//#define Idct4x4Dc		Idct4x4Dc_C
//#define Sub8x8DctDc		Sub8x8DctDc_C
//#define Add8x8IdctDc	Add8x8IdctDc_C
//#define Add16x16IdctDc	Add16x16IdctDc_C
//#define Zigzag4x4		Zigzag4x4_C

#else
#define Sub4x4Dct		Sub4x4Dct_C
#define Sub8x8Dct		Sub8x8Dct_C
#define Sub16x16Dct		Sub16x16Dct_C
#define Add4x4Idct		Add4x4Idct_C
#define Add8x8Idct		Add8x8Idct_C
#define Add16x16Idct	Add16x16Idct_C
#define Dct4x4DC		Dct4x4DC_C
#define Idct4x4Dc		Idct4x4Dc_C
#define Sub8x8DctDc		Sub8x8DctDc_C
#define Add8x8IdctDc	Add8x8IdctDc_C
#define Add16x16IdctDc	Add16x16IdctDc_C
#define Zigzag4x4		Zigzag4x4_C
#endif

void Zigzag2x2Dc( VO_S16 level[4], VO_S16 dct[4] );
void IdctDQ2x2Dc( VO_S16 dct_DC[4], VO_S16 *dct16, VO_S32 *dequant_mf, VO_S32 nQP );
void IdctDQ2x2Dc4( VO_S16 out[4], VO_S16 dct_DC[4], VO_S32 *dequant_mf, VO_S32 nQP );
void Dct2x2Dc ( VO_S16 d[4], VO_S16 *dct16 );
void Dct2x2DcOnly( VO_S16 d[4] );
void Sub8x8Dct_C( VO_S16 *dct, VO_U8 *src, VO_U8 *dst , VO_S32 dst_stride);
void Sub16x16Dct_C( VO_S16 *dct, VO_U8 *src, VO_U8 *dst, VO_S32 dst_stride);
void Sub8x8DctDc_C( VO_S16 dct[4], VO_U8 *src, VO_U8 *dst);
void Add8x8Idct_C( VO_U8 *p_dst, VO_S16 *dct );
void Add16x16Idct_C( VO_U8 *p_dst, VO_S16 *dct );
void Add8x8IdctDc_C( VO_U8 *p_dst, VO_S16 dct[4] );
void Add16x16IdctDc_C( VO_U8 *p_dst, VO_S16 dct[16] );
void Zigzag4x4_C( VO_S16 level[16], VO_S16 dct[16] );
void Zigzag2x2Dc( VO_S16 level[4], VO_S16 dct[4] );
void Dct4x4DC_C( VO_S16* d );
void Idct4x4Dc_C( VO_S16* d );
#endif
