/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef _MP4_VLD_H_
#define _MP4_VLD_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"


extern VOCONST VO_U16 vld_intra[];
extern VOCONST VO_U16 vld_intra_aic[];
extern VOCONST VO_U16 vld_inter[];

extern VO_S32 Mpeg4GetBlockVld( VO_MPEG4_DEC* pDec, const VO_U16 *table, const VO_U8 *scan, VO_S32 len, idct_t* block, VO_S32 mb_type); 
extern VO_S32 Mpeg4GetBlockRvld( VO_MPEG4_DEC* pDec, const VO_U16 *table, const VO_U8 *scan, VO_S32 len, idct_t* block , VO_S32 mb_type); 
extern VO_S32 MpegGetBlockVld( VO_MPEG4_DEC* pDec, const VO_U16 *table, const VO_U8 *scan, VO_S32 len, idct_t* block , VO_S32 mb_type); 
extern VO_S32 H263GetBlockVld( VO_MPEG4_DEC* pDec, const VO_U16 *table, const VO_U8 *scan, VO_S32 len, idct_t* block, VO_S32 nQuant); 




#endif
