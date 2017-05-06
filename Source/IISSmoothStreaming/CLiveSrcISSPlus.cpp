	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcHLS.cpp

	Contains:	CLiveSrcHLS class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-11-01		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "CLiveSrcISSPlus.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "voLiveSource.h"



CLiveSrcISSPlus::CLiveSrcISSPlus(void)
:CLiveSrcBase()
,m_pISS (NULL)
{

}

CLiveSrcISSPlus::~CLiveSrcISSPlus(void)
{
	if (m_pISS != NULL)
	{
		m_pISS->close ();
		delete m_pISS;
		m_pISS = NULL;
	}
}


VO_U32 CLiveSrcISSPlus::Init(VO_LIVESRC_CALLBACK *pCallback)
{
	Close ();

	m_pISS = new vo_smooth_streaming ();
	m_pISS->SetLiveSrcCallbackPlus(pCallback);
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcISSPlus::UnInit()
{
	if (m_pISS != NULL)
	{
		m_pISS->close ();
		delete m_pISS;
		m_pISS = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}


VO_U32 CLiveSrcISSPlus::GetParam(int nParamID, VO_PTR pValue)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	if (nParamID == VO_PID_LIVESRC_SOURCE_TYPE)
	{
		if (m_pISS->is_live ())
			*(int *)pValue = 1;
		else
			*(int *)pValue = 0;
	}
	else if (nParamID == VO_PID_LIVESRC_DURATION)
	{
		*(int *)pValue = m_pISS->GetDuration ();
	}
	else if(nParamID == VO_PID_LIVESRC_TRACKNUMBER)
		*(int *)pValue = m_pISS->GetTrackNumber ();
	
	return CLiveSrcBase::GetParam (nParamID, pValue);

}

VO_U32 CLiveSrcISSPlus::SetParam(int nParamID, VO_PTR pValue)
{
	VOLOGI("in SetParam");
	int nRc = VO_ERR_LIVESRC_OK;
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	/*
	if (nParamID == VO_PID_LIVESRC_HLS_DRM)
		m_pISS->set_DRM (pValue);
	*/
	switch(nParamID)
	{
	case VO_PID_LIVESRC_VIDEODELAY:
		m_pISS->set_VideoDelayTime (pValue);
		break;
	case VO_PID_LIVESRC_POSITION:
		VOLOGI("SetParam %x, %d",nParamID,*(VO_S32*)pValue)
		nRc = m_pISS->set_pos(*(VO_S32*)pValue);
		break;
	case VO_PID_LIVESRC_DRMCALLBACK:
		m_pISS->SetDrmCallBack(pValue);
		break;
	case VO_PID_LIVESRC_LIBOP:
		m_pISS->SetLibOp((VO_LIB_OPERATOR*)pValue);
		break;
	}
	if(nRc == VO_ERR_LIVESRC_OK)
		return nRc;

	return CLiveSrcBase::SetParam (nParamID, pValue);

}



VO_U32 CLiveSrcISSPlus::HeaderParser (VO_PBYTE pData, VO_U32 uSize)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
	
	return m_pISS->HeaderParser(pData, uSize);
}
VO_U32 CLiveSrcISSPlus::ChunkParser (VO_SOURCE_TRACKTYPE trackType, VO_PBYTE pData, VO_U32 uSize)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
	return m_pISS->ChunkParser(trackType, pData, uSize);
}

VO_U32 CLiveSrcISSPlus::GetStreamCounts ( VO_U32 *pStreamCounts)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
	return m_pISS->GetStreamCounts( pStreamCounts);
}
VO_U32 CLiveSrcISSPlus::GetStreamInfo (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	return m_pISS->GetStreamInfo(nStreamID, ppStreamInfo);
}
VO_U32 CLiveSrcISSPlus::GetDRMInfo (VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
	return  m_pISS->GetDRMInfo(ppDRMInfo);
}
VO_U32 CLiveSrcISSPlus::GetPlayList(FragmentItem **ppPlayList)
{
	 return m_pISS->GetPlayList(ppPlayList);
	 
}