	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcHLS.h

	Contains:	CLiveSrcHLS header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-11-01		JBF			Create file

*******************************************************************************/
#ifndef __CLiveSrcISSPlus_H__
#define __CLiveSrcISSPlus_H__

#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"

#include "vo_smooth_streaming.h"
#include "CLiveSrcBase.h"

#ifdef _IOS
#ifdef _ISS_SOURCE_
using namespace _ISS;
#endif
#endif

#define LEON_ADD_2
#define  VO_PID_LIVESRC_CALLBACK 0x00001230
class CLiveSrcISSPlus:public CLiveSrcBase
{
public:
	CLiveSrcISSPlus(void);
	virtual ~CLiveSrcISSPlus(void);

	virtual	VO_U32	Init(VO_LIVESRC_CALLBACK *pCallback);
	virtual	VO_U32	UnInit();
	virtual	VO_U32 	GetParam(int nParamID, VO_PTR pValue);
	virtual	VO_U32 	SetParam(int nParamID, VO_PTR pValue);
	VO_U32 HeaderParser (VO_PBYTE pData, VO_U32 uSize);
	VO_U32 ChunkParser (VO_SOURCE_TRACKTYPE trackType, VO_PBYTE pData, VO_U32 uSize);
	VO_U32 GetStreamCounts ( VO_U32 *pStreamCounts);
	VO_U32 GetStreamInfo (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo);
	VO_U32 GetDRMInfo (VO_SOURCE2_DRM_INFO **ppDRMInfo);
	VO_U32 GetPlayList(FragmentItem **ppPlayList);
protected:
	vo_smooth_streaming *	m_pISS;
};

#endif //__CLiveSrcISSPlus_H__
