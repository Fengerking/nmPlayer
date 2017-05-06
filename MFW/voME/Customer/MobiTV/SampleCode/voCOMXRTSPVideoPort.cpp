	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPVideoPort.cpp

	Contains:	voCOMXRTSPVideoPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif
#include <voOMX_Index.h>

#include <voOMXBase.h>
#include <voOMXOSFun.h>
#include <voOMXMemory.h>

#include "voCOMXRTSPSource.h"

#include "voCOMXRTSPVideoPort.h"
#include "voCOMXPortClock.h"
#include "voCOMXBaseComponent.h"

#define LOG_TAG "voCOMXRTSPVideoPort"
#include "voLog.h"

voCOMXRTSPVideoPort::voCOMXRTSPVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXRTSPOutputPort (pParent, nIndex)
	, m_pClockPort (NULL)
	, m_nReadSamples (0)
	, m_nReadResult (VO_ERR_NOT_IMPLEMENT)
	, m_nSeekPos (0)
{
	strcpy (m_pObjName, __FILE__);
	m_sType.eDomain = OMX_PortDomainVideo;

	m_fmtVideo.Width = 320;
	m_fmtVideo.Height = 240;
	m_fmtVideo.Type = VO_VIDEO_FRAME_NULL;

	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

voCOMXRTSPVideoPort::~voCOMXRTSPVideoPort(void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = voCOMXRTSPOutputPort::SetTrack (pSource, nTrackIndex);
	if (errType != OMX_ErrorNone)
		return errType;

	m_pSource->GetVideoFormat (m_nTrackIndex, &m_fmtVideo);
	if (m_fmtVideo.Width == 0 || m_fmtVideo.Height == 0)
		return OMX_ErrorUndefined;

	OMX_U32	nBufferSize = 409600;
	VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	if (nRC == VO_ERR_NONE)
		m_sType.nBufferSize = nBufferSize + 10240;
	else
		m_sType.nBufferSize = 409600;

	m_sType.nBufferCountActual = 1;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	m_sType.format.video.pNativeRender = NULL;
	m_sType.format.video.bFlagErrorConcealment = OMX_FALSE;

	m_sType.format.video.nFrameWidth = m_fmtVideo.Width;
	m_sType.format.video.nFrameHeight =m_fmtVideo.Height;
	m_sType.format.video.nStride = m_sType.format.video.nFrameWidth;
	m_sType.format.video.nSliceHeight = 16;
	m_sType.format.video.nBitrate = 0;
	m_sType.format.video.xFramerate = (25 << 16) + 1;
	m_sType.format.video.eColorFormat = OMX_COLOR_FormatUnused;
	m_sType.format.video.pNativeWindow = NULL;

	memcpy (m_pMIMEType, &m_nFourCC, 4);
	m_sType.format.video.cMIMEType = m_pMIMEType;

	if (m_trkInfo.Codec == VO_VIDEO_CodingMPEG4)
	{
		strcpy (m_pMIMEType, "MPV4");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingH263)
	{
		strcpy (m_pMIMEType, "H263");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingS263)
	{
		strcpy (m_pMIMEType, "S263");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingS263;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingH264)
	{
		strcpy (m_pMIMEType, "H264");
		//strcpy (m_pMIMEType, "0CVA");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingMPEG2)
	{
		strcpy (m_pMIMEType, "MPV2");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingVP6)
	{
		strcpy (m_pMIMEType, "VP6");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP6;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingMJPEG)
	{
		strcpy (m_pMIMEType, "MJPG");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMJPEG;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingRV)
	{
		strcpy (m_pMIMEType, "RV");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingRV;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingDIVX)
	{
		strcpy (m_pMIMEType, "DIV3");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingDIV3;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingWMV)
	{
		strcpy (m_pMIMEType, "WMV");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingWMV;

		VO_U32 nRC = m_pSource->GetTrackParam (nTrackIndex, VO_PID_SOURCE_BITMAPINFOHEADER, &m_trkInfo.HeadData);
		if (nRC == VO_ERR_NONE)
		{
			VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)m_trkInfo.HeadData;
			m_trkInfo.HeadSize = pBmpInfo->biSize;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::SetClockPort (voCOMXPortClock * pClock)
{
	m_pClockPort = pClock;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	errType = errType;
	switch (nIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pParam;
			if (pVideoFormat->nIndex > 0)
				return OMX_ErrorNoMore;

			pVideoFormat->eCompressionFormat = m_sType.format.video.eCompressionFormat;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXRTSPOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::SetTrackPos (OMX_S64 * pPos)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	if (m_pSource == NULL)
		return OMX_ErrorUndefined;

	VO_S64 nNewPos =  *pPos;
	m_nSeekPos = nNewPos;

	VO_U32 nRC = 0;
	nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);
	if (nRC == VO_ERR_SOURCE_END)
	{
		m_Sample.Time = nNewPos;
		m_nStartPos = nNewPos;
		m_nCurrentPos = nNewPos;

		return OMX_ErrorNone;
	}
	else if (nRC != VO_ERR_NONE)
	{
		VO_S64 nStartPos = 0;
		nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nStartPos);

		if (nRC != VO_ERR_NONE)
		{

			VOLOGE ("m_pSource->SetTrackPos was failed. 0X%08X", nRC);
			return OMX_ErrorNotImplemented;
		}
	}

	m_Sample.Time = nNewPos;
	m_nStartPos = nNewPos;
	m_nCurrentPos = nNewPos;

	m_bEOS = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::Flush (void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = voCOMXRTSPOutputPort::Flush ();

	m_nReadSamples = 0;
	m_nReadResult = VO_ERR_NOT_IMPLEMENT;

	return errType;
}

OMX_ERRORTYPE voCOMXRTSPVideoPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;
	pBuffer->nFlags = 0;

	voCOMXAutoLock lock (m_pLoadMutex);
	if (m_pSource == NULL)
		return OMX_ErrorInvalidState;

	if (m_nReadSamples > 0 && m_nReadResult == VO_ERR_NONE)
	{
		OMX_TICKS	mediaTime = 0;
		if (m_pClockPort != NULL)
			m_pClockPort->GetMediaTime (&mediaTime);

		if (mediaTime > 0)
			m_Sample.Time = mediaTime;

		VOLOGR ("Playing Time: %d", (int)mediaTime);
	}

	VO_U32	nRC = 0;

	nRC = m_pSource->GetTrackData (m_nTrackIndex, &m_Sample);

//	VOLOGI ("Size %d, Time: %d", m_Sample.Size, (int)m_Sample.Time);

	m_nReadResult = nRC;
	if (nRC == VO_ERR_NONE)
	{
		m_nCurrentPos = m_Sample.Time;

		if (m_sType.nBufferCountActual == 1)
			pBuffer->pBuffer = m_Sample.Buffer;
		else
			memcpy (pBuffer->pBuffer, m_Sample.Buffer, m_Sample.Size & 0X7FFFFFFF);
		pBuffer->nFilledLen = m_Sample.Size & 0X7FFFFFFF;
		pBuffer->nTimeStamp = m_Sample.Time;
		if (pBuffer->nTimeStamp < m_nSeekPos)
		{
			pBuffer->nFlags = OMX_BUFFERFLAG_DECODEONLY;
			m_bDecOnly = OMX_TRUE;
		}
		else
		{
			m_nSeekPos = 0;
			m_bDecOnly = OMX_FALSE;
		}
		if (m_bHadBuffering)
		{
			m_bHadBuffering = OMX_FALSE;
			pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		}
		m_nReadSamples++;

		VOLOGR ("Comp %s, Index %d. The buffer size %d, time %d", m_pParent->GetName (), m_sType.nPortIndex, pBuffer->nFilledLen, (int)pBuffer->nTimeStamp);
	}
	else if (nRC == VO_ERR_SOURCE_END)
	{
		pBuffer->nFilledLen = 0;
		pBuffer->nFlags = OMX_BUFFERFLAG_EOS;

		VOLOGI ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_END", m_pParent->GetName (), m_sType.nPortIndex);
	}
	else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		// VOLOGI ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_NEEDRETRY", m_pParent->GetName (), m_sType.nPortIndex);

		return OMX_ErrorNone;
	}
	else
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGE ("Comp %s, Index %d.Get track data failed. 0X%08X", m_pParent->GetName (), m_sType.nPortIndex, nRC);

		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}
