#pragma once

#include "filebase.h"

#define	DSTIME	10000

class CPDSessionWrapper;
class CPDTrackWrapper
{
public:
	CPDTrackWrapper(CPDSessionWrapper* pSource, HVOFILETRACK hTrack, bool isVideo,IVOMutex* pCritSec=NULL);
	virtual ~CPDTrackWrapper();

public:
	virtual bool				IsVideo() {return m_bVideo;}
	virtual DWORD		GetParam(LONG nID, LONG* plValue);
	virtual int					GetCurrPlayFilePos();
	
	int								SetPos(long startTime);
protected:
	CPDSessionWrapper*			m_pSource;
	HVOFILETRACK		m_hTrack;
	bool				m_bVideo;
	DWORD				m_nSampleIndex;
	int					m_nStartTime;
	int					m_nCurTime;
	VOSAMPLEINFO		m_infoSample;
	IVOMutex*			m_pCritSec;
public:
	int		GetCurTime(){return m_nCurTime;}
	HVOFILETRACK GetTrack(){return m_hTrack;}
	DWORD	GetInfo(VOTRACKINFO* pTrackInfo);
	DWORD	GetSampleByIndex(VOSAMPLEINFO* pSampleInfo);
	DWORD	GetSampleByTime(VOSAMPLEINFO* pSampleInfo);
};
