/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/************************************************************************
* @file h265dec_deblock.h
*
* H.265 decoder deblock filtering header file
*
* @author  Huaping Liu
* @date    2012-12-10
************************************************************************/

#ifndef __H265DEC_DEBLOCK_H__
#define __H265DEC_DEBLOCK_H__

#include "h265dec_config.h"
#include "h265dec_porting.h"
#include "h265dec_utils.h"

/* < K0220: Use picture-based chroma QP offsets in deblocking filter.> */ 
//#define ABS(X)    abs(X)
//(((X)>0)?(X):-(X)) 


// typedef struct{
// 	VO_BOOL  bInternalEdge;         /* indicates internal edge */
// 	VO_BOOL  bLeftEdge;             /* indicated left edge */
// 	VO_BOOL  bTopEdge;              /* indicated top edge */
// }voLFCUParam;



//ARM Optimization function define
#if DEBLOCK_ASM_ENABLED 
// void voChromaInFilterASM_H(VO_U8  *pCbSrc,
// 			  VO_U8  *pCrSrc,
// 			  VO_S32 iStride,
// 			  VO_U8  tc
// 	                  );
// 
// void voChromaInFilterASM_V(VO_U8  *pCbSrc,
// 			  VO_U8  *pCrSrc,
// 			  VO_S32 iStride,
// 			  VO_U8  tc
// 	                  );
// 
// void voLumaInFilterASM_H(VO_U8   *piSrc,
// 			 VO_S32  iStride,
// 			 VO_U8   tc,
// 			 VO_S32  nIdx
// 			);
// 
// 
// void voLumaInFilterASM_V(VO_U8   *piSrc,
// 			 VO_S32  iStride,
// 			 VO_U8   tc,
// 			 VO_S32  nIdx
// 			);
// 
// void voPelLumaWeakASM_V(VO_U8   *piSrc,
// 						VO_S32  iStride,
// 						VO_U8   tc,
// 						VO_BOOL bFilterSecondP,
// 						VO_BOOL bFilterSecondQ,
// 						VO_S32  nFlag
// 						);
// 
// void voPelLumaWeakASM_H(VO_U8   *piSrc,
// 						VO_S32  iOffset,
// 						VO_U8   tc,
// 						VO_BOOL bFilterSecondP,
// 						VO_BOOL bFilterSecondQ,
// 						VO_S32  nFlag
// 						);


void voLumaFilterHor_ASM(VO_U8* src, VO_U32 stride, VO_S32 iBeta, VO_S32 iTc);
void voLumaFilterVer_ASM(VO_U8* src, VO_U32 stride, VO_S32 iBeta, VO_S32 iTc);
void voChromaFilterHor_ASM(VO_U8* src, VO_U32 stride, VO_S32 iTc);
void voChromaFilterVer_ASM(VO_U8* src, VO_U32 stride, VO_S32 iTc);
void voChromaFilterCross_ASM(VO_U8* src, VO_U32 stride, VO_U32 ptc);
void voLumaFilterCross_ASM(VO_U8* src, VO_U32 stride, VO_U32 pbeta, VO_U32 ptc);


#endif
// APIs functions 

struct H265_DEC_SLICE;
#if USE_LCU_SYNC
// VO_S32 voDeblockingFilterLCUStep(struct H265_DEC_SLICE * const pSlice, const VO_U32 nRow);
//VO_S32 voDeblockingFilterLCUStepNew(struct H265_DEC_SLICE * const pSlice, const VO_U32 nRow);
VO_S32 voDeblockingFilterLCUStepNew(struct H265_DEC_SLICE * const pSlice, const VO_U32 nRow);
#else
/* H265 deblock LCU line process */
// VO_S32 voDeblockingFilterInRow(void* pSlice, VO_U32 nRow);
//VO_S32 voDeblockingFilterInRowNew(void* pSlice, VO_U32 nRow);

VO_S32 voDeblockingFilterInRowNew(void* pSlice, VO_U32 nRow);

#endif


struct H265_DEC;
/* Parpare for getting bs */
//VO_VOID voParpareParm(struct H265_DEC *p_dec, void *pGst, VO_U32 nRow, VO_U32 nCol, VO_S32 entry);

/* Get CU Edge Flag */
void voSetEdgefilterCU(void *pGst, 
							 VO_U32  BaseUnits, 
							 VO_U32 x4b,
							 VO_U32 y4b, VO_S32 entry);

/* Get PU Edge Flag */
void voSetEdgefilterPU( void* pSlice, VO_U32 BaseUnits, VO_U32 x4b, VO_U32 y4b, VO_U32 uiDepth);
void voSetEdgefilterTUVer(void *pGst, VO_U32 BaseUnits, VO_U32 nIndex);
void voSetEdgefilterTUHor(void *pGst, VO_U32 BaseUnits, VO_U32 nIndex);

/* Get CU and TU Edge Flag */
/*void voSetEdgefilterTU(void *pSlice,  
							 VO_U32  uiWidthInBaseUnits, 
							 VO_U32 x4b,
							 VO_U32 y4b,
							 VO_U32 nV,
							 VO_U32 nH);*/

#endif //__H265DEC_DEBLOCK_H__

