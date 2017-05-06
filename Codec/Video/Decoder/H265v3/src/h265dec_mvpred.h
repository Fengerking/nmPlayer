 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_mvpred.h
    \brief    mv prediction
    \author   Renjie Yu
	\change
*/

#ifndef __VOH265DEC_MVPRED_H__
#define __VOH265DEC_MVPRED_H__

#include "h265_decoder.h"

VO_VOID GetInterMergeCandidates( H265_DEC_SLICE* p_slice,
	const VO_U32 xCb, 
	const VO_U32 yCb,
	const VO_U32 nCbS,                      
	const VO_U32 xPb, 
	const VO_U32 yPb,
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	const VO_U32 partIdx,
	const VO_S32 mrgCandIdx,
	PicMvField* pcMvFieldNeighbours
 );

VO_VOID FillMvpCand ( H265_DEC_SLICE * p_slice, 
	const RefPicList eRefPicList, 
	const VO_U32 xCb,
	const VO_U32 yCb,
	const VO_U32 nCbS,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	const VO_S32 iRefIdx, 
	const VO_U32 partIdx,
	VO_S32 * pInfo );

#endif//__VOH265DEC_MVPRED_H__