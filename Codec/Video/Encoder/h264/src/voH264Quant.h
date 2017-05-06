/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264_QUANT_H_
#define _VOH264_QUANT_H_
#include "voH264EncGlobal.h"

#if defined(VOARMV7)

extern VO_S32 CoeffLast4_ARMV7( VO_S16 *l );
extern VO_S32 CoeffLast15_ARMV7( VO_S16 *l );
extern VO_S32 CoeffLast16_ARMV7( VO_S16 *l );
extern VO_S32 Quant4x4_ARMV7( VO_S16 *dct, VO_U16 inv[16], VO_U16 offset[16] );
extern VO_S32 Quant4x4Dc_ARMV7( VO_S16 *dct, VO_S32 inv, VO_S32 offset );
extern void Dequant4x4_ARMV7( VO_S16 *dct, VO_S32 *dequant_mf, VO_S32 nQP );
extern void Dequant4x4Dc_ARMV7( VO_S16* dct, VO_S32 *dequant_mf, VO_S32 nQP );
extern VO_S32 Quant2x2Dc_ARMV7( VO_S16 *dct, VO_S32 inv, VO_S32 offset );

#define	Quant4x4			Quant4x4_ARMV7
#define	Quant4x4Dc			Quant4x4Dc_ARMV7
#define	Quant2x2Dc			Quant2x2Dc_ARMV7
#define	Dequant4x4			Dequant4x4_ARMV7
#define	Dequant4x4Dc		Dequant4x4Dc_ARMV7

#else		

#define	Quant4x4			Quant4x4_C
#define	Quant4x4Dc			Quant4x4Dc_C
#define	Quant2x2Dc			Quant2x2Dc_C
#define	Dequant4x4			Dequant4x4_C
#define	Dequant4x4Dc		Dequant4x4Dc_C

#endif

extern VO_S32  InitCQM( H264ENC *pEncGlobal );
extern void DeleteCQM4x4( H264ENC *pEncGlobal );
extern VO_S32  NoneResDis( VO_S16 *dct, VO_S32 nMax );
VO_S32 Quant4x4_C( VO_S16 *dct, VO_U16 inv[16], VO_U16 offset[16] );
VO_S32 Quant4x4Dc_C( VO_S16 *dct, VO_S32 inv, VO_S32 offset );
void Dequant4x4_C( VO_S16 *dct, VO_S32 *dequant_mf, VO_S32 nQP );
void Dequant4x4Dc_C( VO_S16* dct, VO_S32 *dequant_mf, VO_S32 nQP );
VO_S32 Quant2x2Dc_C( VO_S16 *dct, VO_S32 inv, VO_S32 offset );

#endif//_VOH264_QUANT_H_
