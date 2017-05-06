#pragma once
#include <voPDPort.h>
#include "voPD.h"
#include "VOUtility.h"
#define	DSTIME	10000

class CPDSessionWrapper2;
class CPDTrackWrapper2:MEM_MANAGER
{
	VO_PBYTE										m_headData;
	VO_SOURCE_TRACKINFO			m_trackInfo;
	int															m_index;
	int															m_curTime;
	CPDSessionWrapper2*			m_pSource;
	bool										m_isVideo;
public:
	CPDTrackWrapper2(CPDSessionWrapper2* pSource, int index);
	virtual ~CPDTrackWrapper2();
	void	Init();
	VO_U32	GetTrackInfo (VO_SOURCE_TRACKINFO * pTrackInfo);
	void		SetCurTime(int time){m_curTime=time;}
	int			GetCurTime(){return m_curTime;}
	bool		IsVideo(){return m_isVideo;}
};
