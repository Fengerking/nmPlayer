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

#include "CLiveSrcDASH.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "voLiveSource.h"


//#pragma warning (disable : 4996)


CLiveSrcDASH::CLiveSrcDASH(void)
	: CLiveSrcBase ()
	, m_pDash (NULL)
{
	memset (&m_sVideoSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	memset (&m_sAudioSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	m_sAudioSample.nTrackID = 1;

}

CLiveSrcDASH::~CLiveSrcDASH(void)
{
	if (m_pDash != NULL)
	{
		m_pDash->close ();
		delete m_pDash;
		m_pDash = NULL;
	}
}


_vosdk_int CLiveSrcDASH::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	Close ();

	m_pUserData = m_pOrigUserData = pUserData;
	m_fStatus	= fStatus;
	m_fSendData = fSendData;

	m_pDash = new vo_mpd_streaming();
    m_pDash->set_statuscallback (this, StatusOutData);
	m_pDash->set_datacallback (this, SendOutData);
	//m_pDash->set_eventcallback (this, NotifyEvent);

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::Close()
{
	if (m_pDash != NULL)
	{
		m_pDash->close ();
		delete m_pDash;
		m_pDash = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::Start()
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pDash->run ();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::Pause()
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pDash->pause ();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::Stop()
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pDash->close();

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::SetDataSource(VO_PTR pSource, _vosdk_int nFlag)
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	VO_BOOL bRC = m_pDash->SetDataSource ((VO_CHAR *)pSource, (VO_BOOL)nFlag);

	return bRC == VO_TRUE ? VO_ERR_LIVESRC_OK : VO_ERR_LIVESRC_NOIMPLEMENT;
}

_vosdk_int CLiveSrcDASH::Seek(_vosdk_int64 nTimeStamp)
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
    VOLOGE( "CLiveSrcDASH1 set_pos %lld ",nTimeStamp );
	m_pDash->set_pos (nTimeStamp);
	VOLOGE( "CLiveSrcDASH2 set_pos %lld ",nTimeStamp );

	return VO_ERR_LIVESRC_OK;
}

_vosdk_int CLiveSrcDASH::GetParam(_vosdk_int nParamID, VO_PTR pValue)
{
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;
	
	_vosdk_int nRc = VO_ERR_LIVESRC_WRONG_STATUS;
	switch(nParamID)
	{
	case VO_PID_LIVESRC_SOURCE_TYPE:
		{
			if (m_pDash->is_live ())
				*(int *)pValue = 1;
			else
				*(int *)pValue = 0;
			
			nRc = VO_ERR_NONE;
			break;
		}
	case VO_PID_LIVESRC_DURATION:
		{
			*(int *)pValue = m_pDash->GetDuration ();
			nRc = VO_ERR_NONE;
			break;
		}
	case VO_PID_LIVESRC_TRACKNUMBER:
		{
			*(int *)pValue = m_pDash->GetTrackNumber ();
			nRc = VO_ERR_NONE;
			break;
		}
	case VO_PID_LIVESRC_SAMPLE_DATA:
		{
			int type = 1;
			*(VO_LIVESRC_SAMPLE *)pValue = m_pDash->GetParam(type);
			nRc = VO_ERR_NONE;
			break;
		}
	default:
		return CLiveSrcBase::GetParam (nParamID, pValue);
	}
	
	return nRc;

}

_vosdk_int CLiveSrcDASH::SetParam(_vosdk_int nParamID, VO_PTR pValue)
{
	VOLOGI("in SetParam");
	_vosdk_int nRc = VO_ERR_LIVESRC_OK;
	if (m_pDash == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	/*
	if (nParamID == VO_PID_LIVESRC_HLS_DRM)
		m_pDash->set_DRM (pValue);
	*/
	switch(nParamID)
	{
	case VO_PID_LIVESRC_VIDEODELAY:
		m_pDash->set_VideoDelayTime (pValue);
		break;
	case VO_PID_LIVESRC_POSITION:
		VOLOGI("SetParam %x, %d",nParamID,*(VO_S32*)pValue)
		nRc = m_pDash->set_pos(*(VO_S32*)pValue);
		break;
	case VO_PID_LIVESRC_DRMCALLBACK:
		m_pDash->SetDrmCallBack(pValue);
		break;
	case VO_PID_LIVESRC_LIBOP:
		m_pDash->SetLibOp((VO_LIB_OPERATOR*)pValue);
		break;
	}
	if(nRc == VO_ERR_LIVESRC_OK)
		return nRc;

	return CLiveSrcBase::SetParam (nParamID, pValue);

}

VO_VOID	CLiveSrcDASH::SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample)
{
	CLiveSrcDASH * PDASH = (CLiveSrcDASH *)pUserData;

	PDASH->m_fSendData (PDASH->m_pUserData, ptr_sample->nTrackID , ptr_sample );
}

VO_VOID	CLiveSrcDASH::StatusOutData (VO_VOID * pUserData,_vosdk_int id, VO_VOID * pBuffer)
{
	CLiveSrcDASH * PDASH = (CLiveSrcDASH *)pUserData;

	PDASH->m_fStatus (PDASH->m_pUserData, id, (_vosdk_int)pBuffer, 0 );
}
