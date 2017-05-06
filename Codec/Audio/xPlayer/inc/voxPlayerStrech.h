/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

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
* @file voxPlayerStrech.h
*
* xPlayer strech functions header file
*
* @author  Huaping Liu
* @date    2013-04-26
************************************************************************/

#ifndef __VOXPLAYERSTRECH_H__
#define __VOXPLAYERSTRECH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voType.h"
#include "voRingBuffer.h"

typedef struct  
{
	VO_S32			nChs;
	VO_S32			sampleReq;
	VO_S16			*pMidBuffer;                                                     /* Process temp buffer */
	VO_S16			*pRefMidBuffer;                                           /* Scale Process temp buffer */
	VO_S16			*pRefMidBufTemp;
	VO_S32			overlapLength;
	VO_S32			seekLength;
	VO_S32			nSeekWinLens;
	VO_S32			overlapDividerBits;
	VO_S32			slopingDivider;
	VO_S32			sampleRate;
	VO_S32			sequenceMs;
	VO_S32			seekWindowMs;
	VO_S32			overlapMs;
	VO_BOOL		bAutoSeqFlag;
	VO_BOOL		bAutoSeekFlag;
	VO_BOOL		bFastMode;

	float				nominalSkip;
	float				skipFract;
	float				tempo;
	float				tempo_temp;
	voRingBuffer *pInBuf;
	voRingBuffer *pOutBuf;
	VO_MEM_OPERATOR      *pvoMemop;
}voxPlayerStrech;


void tsmSetTempo(voxPlayerStrech *pTsm, float newTempo);
void tsmProcess(voxPlayerStrech *pTsm);
void tsmClear(voxPlayerStrech *pTsm);
void tsmSetParameters(voxPlayerStrech *pTsm,
								   VO_S32  aSampleRate, 
					               VO_S32  aSequenceMS, 
					               VO_S32  aSeekWindowMS, 
					               VO_S32   aOverlapMS);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VOXPLAYERSTRECH_H__