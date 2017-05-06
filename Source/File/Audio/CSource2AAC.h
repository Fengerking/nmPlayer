	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSource2AAC.cpp

	Contains:	Implement the AAC Source2 Interface

	Written by:	Aiven

	Change History (most recent first):
	2012-04-11		Aiven			Create file

*******************************************************************************/

#ifndef __CSource2AAC_H__
#define __CSource2AAC_H__

#include "voSource2.h"
#include "voString.h"
//#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"

#include "CAacParser.h"
#include "voCMutex.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

//static	voCMutex			m_AutoLock;	
class CSource2AAC 
{
public:
	CSource2AAC();
	~CSource2AAC();

	VO_U32 Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback, VO_TCHAR *  pWorkingPath);
	VO_U32 Uninit();
	VO_U32 Open();
	VO_U32 Close();
	VO_U32 Start();
	VO_U32 Pause();
	VO_U32 Stop();
	VO_U32 Seek(VO_U64* pTimeStamp);
	VO_U32 GetDuration(VO_U64 * pDuration);
	VO_U32 GetSample(VO_U32 nOutPutType , VO_PTR pSample);
	VO_U32 GetStreamCount(VO_U32 *pStreamCount);
	VO_U32 GetStreamInfo(VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo);
	VO_U32 GetCurSelTrackInfo(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo );
	VO_U32 SelectTrack(VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID );
	VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);
	VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& buffer);	
	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
protected:
	
private:
	CAacParser*	m_pAacParser;
	VO_SOURCE2_SAMPLECALLBACK * m_pCallback;
	VO_PTR				m_hCheck;
	
};

#ifdef _VONAMESPACE
}
#endif
#endif //__CSource2AAC_H__



