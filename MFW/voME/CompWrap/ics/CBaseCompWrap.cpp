	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseCompWrap.cpp

	Contains:	CBaseCompWrap class file

	Written by:	Bangfei Jin

	Change History(most recent first):
	2009-06-25		JBF			Create file

*******************************************************************************/
#include <string.h>
#include "voString.h"

#include "voOMXWrap.h"
#include "CBaseCompWrap.h"
#include "CTIComp.h"
#include "voLog.h"

CBaseCompWrap::CBaseCompWrap(VO_COMPWRAP_CODECTYPE nCodecType, VO_U32 nCoding,VO_CODEC_INIT_USERDATA * pUserData)
	: m_pCompWrap(NULL)
{
	m_pCompWrap = new CTIComp(pUserData);
	m_pCompWrap->SetCodecCoding(nCodecType, nCoding);
}

CBaseCompWrap::~CBaseCompWrap()
{
	if (m_pCompWrap != NULL)
		delete m_pCompWrap;
}

VO_U32 CBaseCompWrap::vdSetInputData(VO_CODECBUFFER * pInput)
{
	return m_pCompWrap->SetVideoInputData(pInput);;
}

VO_U32 CBaseCompWrap::vdGetOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	return m_pCompWrap->GetVideoOutputData(pOutBuffer, pOutInfo);;
}

VO_U32 CBaseCompWrap::adSetInputData(VO_CODECBUFFER * pInput)
{
	return m_pCompWrap->SetVideoInputData(pInput);;
}

VO_U32 CBaseCompWrap::adGetOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	return m_pCompWrap->GetAudioOutputData(pOutBuffer, pOutInfo);;
}

VO_U32 CBaseCompWrap::vrSetInputData(VO_VIDEO_BUFFER * pInput)
{
	return m_pCompWrap->SetVideoRenderData(pInput);
}

VO_U32 CBaseCompWrap::vrSetVideoInfo(VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	if (m_pCompWrap != NULL)
		return m_pCompWrap->SetVideoSize(nWidth , nHeight);

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseCompWrap::SetParam(VO_S32 uParamID, VO_PTR pData)
{
	VO_U32 nRC = VO_ERR_NONE;
	if (m_pCompWrap == NULL)
		return VO_ERR_WRONG_STATUS;

	switch (uParamID)
	{
	case VO_PID_COMMON_CoreFile:
		nRC = m_pCompWrap->SetCoreFile((OMX_STRING) pData);
		break;

	case VO_PID_COMMON_CompName:
		nRC = m_pCompWrap->SetCompName((OMX_STRING) pData);
		break;

	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER * pBuffer =(VO_CODECBUFFER *)pData;
			nRC = m_pCompWrap->SetHeadData(pBuffer->Buffer, pBuffer->Length);
		}
		break;

	case VO_PID_COMMON_FLUSH:
		m_pCompWrap->Flush();
		break;

	case VO_PID_COMMON_START:
		m_pCompWrap->Start();
		break;

	case VO_PID_COMMON_PAUSE:
		m_pCompWrap->Pause();
		break;

	case VO_PID_COMMON_STOP:
		m_pCompWrap->Stop();
		break;

	case VO_PID_VIDEO_FORMAT:
		{
			VO_VIDEO_FORMAT * pFormat =(VO_VIDEO_FORMAT *)pData;
			nRC = m_pCompWrap->SetVideoSize(pFormat->Width, pFormat->Height);
		}
		break;

	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT * pFormat =(VO_AUDIO_FORMAT *)pData;
			nRC = m_pCompWrap->SetAudioFormat(pFormat->SampleRate, pFormat->Channels, pFormat->SampleBits);
		}
		break;
	case VO_PID_OMXWRAP_VRSURFACE:
		{
			m_pCompWrap->SetVideoRenderSurface(pData);
		}
		break;

	case VO_PID_VIDEO_THUMBNAIL_MODE:
		{
			VO_BOOL bThumbMode = *(VO_BOOL*)pData;
			m_pCompWrap->SetThumbnailMode(bThumbMode);
			VOLOGE("SetThumbMode %d", bThumbMode);
		}
		break;

	case VOMP_PID_DRAW_VIEW:
		{
			m_pCompWrap->SetVideoRenderSurface(pData);
		}

		break;
	
	default:
		break;
	}

	return nRC;
}


VO_U32 CBaseCompWrap::GetParam(VO_S32 uParamID, VO_PTR pData)
{
	if (m_pCompWrap == NULL)
		return VO_ERR_WRONG_STATUS;

	switch (uParamID)
	{
	case VO_PID_VIDEO_DIMENSION: 
		m_pCompWrap->GetDecParam(VO_PID_VIDEO_DIMENSION, pData);
		break;

	default:
		break;
	}

	return VO_ERR_WRONG_PARAM_ID;
}

