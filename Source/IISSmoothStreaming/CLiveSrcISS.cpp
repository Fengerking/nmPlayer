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

#include "CLiveSrcISS.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "voLiveSource.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CLiveSrcISS::CLiveSrcISS(void)
	: CLiveSrcBase ()
	, m_pISS (NULL)
{
	memset (&m_sVideoSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	memset (&m_sAudioSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	m_sAudioSample.nTrackID = 1;

}

CLiveSrcISS::~CLiveSrcISS(void)
{
	if (m_pISS != NULL)
	{
		m_pISS->close ();
		delete m_pISS;
		m_pISS = NULL;
	}
}


_vosdk_int CLiveSrcISS::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	Close ();

	m_pUserData = m_pOrigUserData = pUserData;
	m_fStatus	= fStatus;
	m_fSendData = fSendData;

	m_pISS = new vo_smooth_streaming ();
	m_pISS->set_statuscallback (this, StatusOutData);
	m_pISS->set_datacallback (this, SendOutData);
	//m_pISS->set_eventcallback (this, NotifyEvent);

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::Close()
{
	if (m_pISS != NULL)
	{
		m_pISS->close ();
		delete m_pISS;
		m_pISS = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::Start()
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pISS->run ();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::Pause()
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pISS->pause ();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::Stop()
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pISS->close();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::SetDataSource(VO_PTR pSource, _vosdk_int nFlag)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	VO_BOOL bRC = m_pISS->SetDataSource ((VO_CHAR *)pSource, (VO_BOOL)nFlag);

	return bRC == VO_TRUE ? VO_ERR_LIVESRC_OK : VO_ERR_LIVESRC_NOIMPLEMENT;
}

_vosdk_int CLiveSrcISS::Seek(_vosdk_int64 nTimeStamp)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pISS->set_pos (VO_S32(nTimeStamp));

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::GetParam(_vosdk_int nParamID, VO_PTR pValue)
{
	if (m_pISS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	switch(nParamID)
	{
	case VO_PID_LIVESRC_SOURCE_TYPE:
		{
			if (m_pISS->is_live ())
				*(int *)pValue = 1;
			else
				*(int *)pValue = 0;
		}
		break;
	case VO_PID_LIVESRC_DURATION:
		*(int *)pValue = m_pISS->GetDuration ();
		break;
	case VO_PID_LIVESRC_TRACKNUMBER:
		*(int *)pValue = m_pISS->GetTrackNumber ();
		break;
	default:
		return CLiveSrcBase::GetParam (nParamID, pValue);
	}

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcISS::SetParam(_vosdk_int nParamID, VO_PTR pValue)
{
	VOLOGI("in SetParam");
	_vosdk_int nRc = VO_ERR_LIVESRC_OK;
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

VO_VOID	CLiveSrcISS::SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample)
{
	CLiveSrcISS * pISS = (CLiveSrcISS *)pUserData;

	pISS->m_fSendData (pISS->m_pUserData, VO_U16(ptr_sample->nTrackID ), ptr_sample );
}

VO_VOID	CLiveSrcISS::StatusOutData (VO_VOID * pUserData,_vosdk_int id, VO_VOID * pBuffer)
{
	CLiveSrcISS * pISS = (CLiveSrcISS *)pUserData;

	pISS->m_fStatus (pISS->m_pUserData, id, (_vosdk_int)pBuffer, 0 );
}
