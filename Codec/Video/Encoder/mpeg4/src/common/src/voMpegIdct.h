/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __IDCT_H__
#define __IDCT_H__

#include "voType.h"
#define idct_t VO_S16
//#define SAT(Value) ( (Value) < 0 ? 0: ( (Value) > 255 ? (VO_U8)255: (VO_U8)(Value)))
#define SAT(Value) (VO_U8)(Value < 0 ? 0: (Value > 255 ? 255: Value))


#if  defined(VOARMV4) || defined(VOWMMX)
VO_VOID extern IdctBlock1x1_ARMV4(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock4x4_ARMV4(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock4x8_ARMV4(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock8x8_ARMV4(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );

#define IDCT_Block1x1 IdctBlock1x1_ARMV4
#define IDCT_Block4x4 IdctBlock4x4_ARMV4
#define IDCT_Block4x8 IdctBlock4x8_ARMV4
#define IDCT_Block8x8 IdctBlock8x8_ARMV4

#elif defined(VOARMV6)
VO_VOID extern IdctBlock1x1_ARMV6(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock4x4_ARMV6(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock4x8_ARMV6(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock8x8_ARMV6(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );

#define IDCT_Block1x1 IdctBlock1x1_ARMV6
#define IDCT_Block4x4 IdctBlock4x4_ARMV6
#define IDCT_Block4x8 IdctBlock4x8_ARMV6
#define IDCT_Block8x8 IdctBlock8x8_ARMV6

#elif defined(VOARMV7)
VO_VOID extern IdctBlock4x8_ARMV7(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock8x8_ARMV7(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID extern IdctBlock1x1_ARMV7(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );

#define IDCT_Block1x1 IdctBlock1x1_ARMV7 //MPEG4DEC_VO_Armv7IdctC
#define IDCT_Block4x4 IdctBlock4x8_ARMV7 //MPEG4DEC_VO_Armv7IdctA	//not bit-complaint with C
#define IDCT_Block4x8 IdctBlock4x8_ARMV7 //MPEG4DEC_VO_Armv7IdctA	//not bit-complaint with C( shoule be Arm11IdctD,but it is same with MPEG4DEC_VO_Armv7IdctA)
#define IDCT_Block8x8 IdctBlock8x8_ARMV7 //MPEG4DEC_VO_Armv7IdctB	//not bit-complaint with C

#else
VO_VOID IdctBlock1x1_C(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID IdctBlock4x4_C(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID IdctBlock4x8_C(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );
VO_VOID IdctBlock8x8_C(idct_t *Block, VO_U8 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride );

#define IDCT_Block1x1 IdctBlock1x1_C
#define IDCT_Block4x4 IdctBlock4x4_C
#define IDCT_Block4x8 IdctBlock4x8_C
#define IDCT_Block8x8 IdctBlock8x8_C



#endif
#endif//__IDCT_H__