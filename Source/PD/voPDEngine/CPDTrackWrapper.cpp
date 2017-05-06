#include <voPDPort.h>
#include <tchar.h>
#include <stdio.h>

#include "CPDTrackWrapper.h"
#include "CPDSessionWrapper.h"
#include "macro.h"

extern int g_nLog;
DWORD	CPDTrackWrapper::GetSampleByIndex(VOSAMPLEINFO* pSampleInfo){
	long rc=m_pSource->TrackGetSampleByIndex(m_hTrack,pSampleInfo);
	if(rc==0&&pSampleInfo->pBuffer)//get actuall frame
	{
		m_nCurTime		  =pSampleInfo->uTime;
		m_nSampleIndex=pSampleInfo->uIndex;
	}
	return rc;
}
DWORD	CPDTrackWrapper::GetSampleByTime(VOSAMPLEINFO* pSampleInfo)
{
	long rc=m_pSource->TrackGetSampleByTime(m_hTrack,pSampleInfo);
	return rc;
}
DWORD	CPDTrackWrapper::GetInfo(VOTRACKINFO* pTrackInfo)
{
	long rc=m_pSource->TrackGetInfo(m_hTrack,pTrackInfo);
	if(rc==0)
	{
		//m_avCodec = pTrackInfo->uCodec;
		//m_nMaxSampleSize = pTrackInfo->uMaxSampleSize;
		//m_nSampleCount = pTrackInfo->uSampleCount;

		m_nStartTime = 0;
		//m_nStopTime = pTrackInfo->uDuration;
	}
	return rc;
}
CPDTrackWrapper::CPDTrackWrapper(CPDSessionWrapper* pSource, HVOFILETRACK hTrack, bool isVideo,IVOMutex* pCritSec)
	: m_pSource(pSource)
	, m_hTrack(hTrack)
	, m_bVideo(isVideo)
	//, m_nMaxSampleSize(0)
	, m_nSampleIndex(0)
	//, m_nSampleCount(0)
	, m_nStartTime(0)
	//, m_nStopTime(0)
	, m_nCurTime(0)
	//, m_bEndOfStream(false)
	//, m_pExtData(NULL)
	//, m_nExtSize(0)
	//, m_dwThreadID(0)
	, m_pCritSec(pCritSec)
	//, m_bFirstSample(true)
{
	memset(&m_infoSample, 0, sizeof(VOSAMPLEINFO));
}

CPDTrackWrapper::~CPDTrackWrapper()
{
	//SAFE_DELETE_ARRAY(m_pExtData);
}

DWORD CPDTrackWrapper::GetParam(LONG nID, LONG* plValue)
{
	return m_pSource->TrackGetParam(m_hTrack, nID, plValue);
}




int			CPDTrackWrapper::	SetPos(long startTime)
{

	VOSAMPLEINFO	infoSample;
	memset(&infoSample, 0, sizeof(VOSAMPLEINFO));
	if(!m_bVideo)
	{
		startTime-=200;
		if (startTime<0)
		{
			startTime=0;
		}
	}
	infoSample.uTime = startTime;
	m_pSource->TrackGetSampleByTime(m_hTrack, &infoSample);
	int nSampleIndex = infoSample.uIndex;

	if(m_bVideo)
	{
		if((infoSample.uSize & 0X80000000) ==0)
			nSampleIndex = m_pSource->TrackGetNextKeySample(m_hTrack, infoSample.uIndex, -1);
		if (nSampleIndex < 0)
			nSampleIndex = 0;
	}
	m_nSampleIndex	= nSampleIndex;
	m_nCurTime=m_nStartTime			= startTime;
	voLog(LL_TRACE,"seek.txt","video=%d,idx=%d,start=%d\n",m_bVideo,m_nSampleIndex,m_nStartTime);
	return 0;
}
int CPDTrackWrapper::GetCurrPlayFilePos()
{
	return m_pSource->TrackGetFilePosByIndex(m_hTrack, m_nSampleIndex);
}