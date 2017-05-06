/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseTask.cpp

Contains:	CBaseTask class file

Written by:	   Yu Wei 

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#include "CBaseTask.h"

#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"


#pragma warning (disable : 4996)


#define LOG_TAG "CBaseTask"

CBaseTask::CBaseTask()
: m_pMark(NULL)
, m_bVideoFinished(VO_FALSE)
, m_bAudioFinished(VO_FALSE)
{	

}

CBaseTask::~CBaseTask ()
{
	
}

void CBaseTask::SetMark(VO_PTR in_pMark)
{
	m_pMark = (VOEDITMARK*)in_pMark;
}

VO_U32 CBaseTask::CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo)
{
	return 0;
}

VO_BOOL  CBaseTask::CheckFinished()
{
	return (m_bAudioFinished == VO_TRUE && m_bVideoFinished == VO_TRUE) ? VO_TRUE : VO_FALSE; 
}
