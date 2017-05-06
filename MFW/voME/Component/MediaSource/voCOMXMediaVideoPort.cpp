	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaVideoPort.cpp

	Contains:	voCOMXMediaVideoPort class file

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

#include "voCOMXMediaVideoPort.h"
#include "voCOMXPortClock.h"
#include "voCOMXBaseComponent.h"

#define LOG_TAG "voCOMXMediaVideoPort"
#include "voLog.h"

voCOMXMediaVideoPort::voCOMXMediaVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXMediaOutputPort (pParent, nIndex)
	, m_pClockPort (NULL)
	, m_nReadSamples (0)
{
	strcpy (m_pObjName, __FILE__);
	m_sType.eDomain = OMX_PortDomainVideo;

	m_fmtVideo.Width = 320;
	m_fmtVideo.Height = 240;
	m_fmtVideo.Type = VO_VIDEO_FRAME_NULL;

	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

voCOMXMediaVideoPort::~voCOMXMediaVideoPort(void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

OMX_ERRORTYPE voCOMXMediaVideoPort::SetTrackInfo (OMX_S32 nTrackIndex, VO_SOURCE_TRACKINFO *pTrackInfo)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = voCOMXMediaOutputPort::SetTrackInfo (nTrackIndex, pTrackInfo);
	if (errType != OMX_ErrorNone)
		return errType;

	//m_pSource->GetVideoFormat (m_nTrackIndex, &m_fmtVideo);
	//if (m_fmtVideo.Width == 0 || m_fmtVideo.Height == 0)
	//	return OMX_ErrorUndefined;

	OMX_U32	nBufferSize = 409600;
	//VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	//if (nRC == VO_ERR_NONE)
	//	m_sType.nBufferSize = nBufferSize + 10240;
	//else
		m_sType.nBufferSize = nBufferSize;

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

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXMediaVideoPort::SetClockPort (voCOMXPortClock * pClock)
{
	m_pClockPort = pClock;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXMediaVideoPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
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

	return voCOMXMediaOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXMediaVideoPort::Flush (void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = voCOMXMediaOutputPort::Flush ();

	m_nReadSamples = 0;

	return errType;
}

OMX_ERRORTYPE voCOMXMediaVideoPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	// Dump log info
	if (m_nLogFrameSize > 0 && m_pLogFrameSpeed != NULL)
	{
		if (m_nLogFrameIndex < m_nLogFrameSize)
		{
			m_pLogFrameSpeed[m_nLogFrameIndex] = voOMXOS_GetSysTime ();
		}
	}

	if (m_nReadSamples > 0)
	{
		OMX_TICKS	mediaTime = 0;
		if (m_pClockPort != NULL)
			m_pClockPort->GetMediaTime (&mediaTime);

		if (mediaTime > 0)
			m_Sample.Time = mediaTime;

		VOLOGR ("Playing Time: %d", (int)mediaTime);
	}


	VO_U32	nRC = 0;
	nRC = m_pSampleStore->GetSample(&m_Sample);
	if (nRC == VO_ERR_NONE)
	{
		if (m_nFirstPos == -1)
			m_nFirstPos = m_Sample.Time;

		m_nCurrentPos = m_Sample.Time;

		if (m_sType.nBufferCountActual == 1)
			pBuffer->pBuffer = m_Sample.Buffer;
		else
			memcpy (pBuffer->pBuffer, m_Sample.Buffer, m_Sample.Size & 0X7FFFFFFF);
		pBuffer->nFilledLen = m_Sample.Size & 0X7FFFFFFF;
		pBuffer->nTimeStamp = m_Sample.Time;
		m_nReadSamples++;

		if (m_hDumpFile != NULL)
		{
			cmnFileWrite (m_hDumpFile, pBuffer->pBuffer, pBuffer->nFilledLen);
//			cmnFileFlush (0, m_hDumpFile);
		}

		if (m_nLogFrameSize > 0 && m_pLogMediaTime != NULL)
		{
			if (m_nLogFrameIndex < m_nLogFrameSize)
			{
				m_pLogMediaTime[m_nLogFrameIndex] = pBuffer->nTimeStamp;
				m_pLogSystemTime[m_nLogFrameIndex] = voOMXOS_GetSysTime ();
				m_pLogFrameSize[m_nLogFrameIndex] = m_Sample.Size;
				m_pLogFrameSpeed[m_nLogFrameIndex] = voOMXOS_GetSysTime () - m_pLogFrameSpeed[m_nLogFrameIndex];
			}
			m_nLogFrameIndex++;
		}

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

		VOLOGR ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_NEEDRETRY", m_pParent->GetName (), m_sType.nPortIndex);

		return OMX_ErrorNone;
	}
	else
	{
		VOLOGE ("Comp %s, Index %d.Get track data failed. 0X%08X", m_pParent->GetName (), m_sType.nPortIndex, nRC);
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}
