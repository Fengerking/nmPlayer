
#include <tchar.h>
#include <stdio.h>

#include "CPDTrackWrapper2.h"
#include "CPDSessionWrapper2.h"
#include "macro.h"

extern int g_nLog;
CPDTrackWrapper2::CPDTrackWrapper2(CPDSessionWrapper2* pSource, int index)
	: m_pSource(pSource)
	,m_headData(NULL)
	,m_index(index)
	,m_curTime(0)
	,m_isVideo(false)
{
}

CPDTrackWrapper2::~CPDTrackWrapper2()
{
	VO_SAFE_DELETE(m_headData);
}
void	CPDTrackWrapper2::Init()
{
	VO_SOURCE_TRACKINFO * pTrackInfo=&m_trackInfo;
	VO_SOURCE_READAPI	* reader=m_pSource->GetReaderAPI();
	reader->GetTrackInfo(m_pSource->GerReaderHandler(),m_index,pTrackInfo);
	if(pTrackInfo->Type==VO_SOURCE_TT_VIDEO)
		m_isVideo = true;
}
VO_U32	CPDTrackWrapper2::GetTrackInfo (VO_SOURCE_TRACKINFO * pTrackInfo)
{
	memcpy(pTrackInfo,&m_trackInfo,sizeof(m_trackInfo));
	return 0;
}

