	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXZoom2VideoSink.cpp

	Contains:	voCOMXZoom2VideoSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voOMXOSFun.h"
#include "voOMXBase.h"
#include "voOMXMemory.h"

#include "voCOMXZoom2VideoSink.h"
#include "cmnMemory.h"
#include "vomeAPI.h"

#include "voLogoData.h"

#define OMX_COLOR_Format_QcomHW		0X7FA30C00

#include "voLog.h"

voCOMXZoom2VideoSink::voCOMXZoom2VideoSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSink (pComponent)
	, m_pClockPort (NULL)
	, m_pVideoRender (NULL)
	, m_nRendFrames (0)
	, m_nRenderSpeed (0)
	, m_nDroppedFrame (VO_TRUE)
	, m_nPrevSystemTime (0)
	, m_bThread (OMX_TRUE)
	, m_nSysTime (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Sink");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_cbRenderBuffer.pCallBack = NULL;
	m_cbRenderBuffer.pUserData = NULL;

	m_videoFormat.eColorFormat = OMX_COLOR_FormatYUV420Planar;

	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open (_T("vomeplay.cfg"));
	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"BufferThread", 1) == 0)
		m_bThread = OMX_FALSE;

	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpLog", 0) > 0)
	{
		m_nPfmFrameSize = 10240;
		m_pPfmFrameTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCodecThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCompnThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmMediaTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmSystemTime = new OMX_U32[m_nPfmFrameSize];
	}

	m_pCfgCodec = new CBaseConfig ();
	m_pCfgCodec->Open (_T("vommcodec.cfg"));

	VOLOGF ();
}

voCOMXZoom2VideoSink::~voCOMXZoom2VideoSink(void)
{
	VOLOGF ();

	if (m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	delete m_pCfgComponent;
	delete m_pCfgCodec;

//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex == 0)
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
				errType = OMX_ErrorNone;
			}
		}
		break;

	default:
		errType = voCOMXCompSink::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXZoom2VideoSink::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam (pComp, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = pType->nBufferCountActual;

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

			m_nYUVWidth = (m_videoType.nFrameWidth + 15) & ~0XF;
			m_nYUVHeight = (m_videoType.nFrameHeight + 15) & ~0XF;
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex == 0)
			{
				m_videoFormat.eColorFormat = pVideoFormat->eColorFormat;

				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420Planar)
					return OMX_ErrorNone;

				// QCOM HW Video Decoder Coor Type
				if (pVideoFormat->eColorFormat == OMX_COLOR_Format_QcomHW)
					return OMX_ErrorNone;

				errType = OMX_ErrorPortsNotCompatible;
			}
		}
		break;

	case OMX_VO_IndexRenderBufferCallBack:
		{
			OMX_VO_CHECKRENDERBUFFERTYPE * pRenderType = (OMX_VO_CHECKRENDERBUFFERTYPE *)pComponentParameterStructure;
			m_cbRenderBuffer.pCallBack = pRenderType->pCallBack;
			m_cbRenderBuffer.pUserData = pRenderType->pUserData;
		}
		break;

	default:
		errType = voCOMXCompSink::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nIndex);

	switch (nIndex)
	{
	case OMX_VO_IndexConfigDisplayArea:
		{
			OMX_VO_DISPLAYAREATYPE * pDisp = (OMX_VO_DISPLAYAREATYPE *)pComponentConfigStructure;

			m_hView = pDisp->hView;

			m_rcDisp.left = pDisp->nX;
			m_rcDisp.top = pDisp->nY;
			m_rcDisp.right = pDisp->nX + pDisp->nWidth;
			m_rcDisp.bottom = pDisp->nY + pDisp->nHeight;

			if (m_pVideoRender != NULL)
				m_pVideoRender->SetDispRect (m_hView, &m_rcDisp);

			m_nRendFrames = 0;
			m_nRenderSpeed = 0;
			m_nDroppedFrame = VO_TRUE;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSink::SetConfig (hComponent, nIndex, pComponentConfigStructure);

}

OMX_ERRORTYPE voCOMXZoom2VideoSink::CreatePorts (void)
{
	VOLOGF ();

	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		m_pInput = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);

		m_pBufferQueue = m_pInput->GetBufferQueue ();
		m_pSemaphore = m_pInput->GetBufferSem ();

		m_ppPorts[1] = new voCOMXPortClock (this, 1, OMX_DirInput);
		m_pClockPort = (voCOMXPortClock *)m_ppPorts[1];
		m_pClockPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pClockPort->SetClockType (OMX_TIME_RefClockVideo);

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::CreateRender (void)
{
	VOLOGF ();

	if (m_pVideoRender == NULL)
	{
		m_pVideoRender = new CVideoRender (NULL, m_hView, m_pMemOP);
		m_pVideoRender->SetConfig (m_pCfgCodec);

		if (m_cbRenderBuffer.pCallBack != NULL)
			m_pVideoRender->SetCallBack (videosinkVideoRenderProc, this);

		VO_VIDEO_FORMAT	fmtVideo;
		fmtVideo.Width = m_videoType.nFrameWidth;
		fmtVideo.Height = m_videoType.nFrameHeight;

		OMX_U32 nRC = m_pVideoRender->Init (&fmtVideo);
		nRC = nRC;
		m_pVideoRender->SetDispRect (m_hView, &m_rcDisp);

		if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYUV420Planar)
		{
			m_pVideoRender->SetVideoInfo (fmtVideo.Width, fmtVideo.Height, VO_COLOR_YUV_PLANAR420);
		}
		else if (m_videoFormat.eColorFormat == OMX_COLOR_Format_QcomHW)
		{
			m_pVideoRender->SetVideoInfo (fmtVideo.Width, fmtVideo.Height, VO_COLOR_YUV_420_PACK_2);
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. The new state is %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_pVideoRender == NULL)
			CreateRender ();

		if (!m_bThread)
			return OMX_ErrorNone;
	}

	m_nRendFrames = 0;

	return voCOMXCompSink::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::Flush (OMX_U32	nPort)
{
	VOLOGF ();

	OMX_ERRORTYPE errType = voCOMXCompSink::Flush (nPort);

	m_nRendFrames = 0;

	return errType;
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												   OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGF ();

	if (m_bThread)
	{
		return voCOMXCompSink::EmptyThisBuffer (hComponent, pBuffer);
	}
	else
	{
		OMX_ERRORTYPE	errType = OMX_ErrorNone;

		errType = FillBuffer (pBuffer);

		if (m_pInput->IsTunnel ())
		{
			pBuffer->nOutputPortIndex = m_pInput->GetTunnelPort ();
			pBuffer->nInputPortIndex = 0;
			errType = m_pInput->GetTunnelComp()->FillThisBuffer (m_pInput->GetTunnelComp (), pBuffer);
		}
		else
		{
			errType = m_pCallBack->EmptyBufferDone (m_pComponent, m_pAppData, pBuffer);
		}

		if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		{
			if (m_pCallBack != NULL)
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL);
			}
		}

		return errType;
	}
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pBuffer->nFilledLen, (int)pBuffer->nTimeStamp);

	voCOMXAutoLock lokc (&m_tmStatus);

	if (m_nSysTime == 0)
		m_nSysTime = voOMXOS_GetSysTime ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	m_pRenderBuffer = pBuffer;
	if (pBuffer->nFilledLen > 0)
	{
		OMX_BOOL	bDrapped = OMX_FALSE;
		if (pBuffer->nFilledLen == sizeof (VO_VIDEO_BUFFER))
		{
			VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pBuffer->pBuffer;
			if ((pVideoBuffer->Stride[0] & 0XFFFF0000) > 0)
			{
				VO_U32 nWidth = (pVideoBuffer->Stride[0] & 0XFFFF0000) >> 16;
				VO_U32 nHeight = (pVideoBuffer->Stride[1] & 0XFFFF0000) >> 16;
				if (m_pVideoRender != NULL)
					m_pVideoRender->SetVideoInfo (nWidth, nHeight, VO_COLOR_YUV_PLANAR420);

				pVideoBuffer->Stride[0] = pVideoBuffer->Stride[0] & 0XFFFF;
				pVideoBuffer->Stride[1] = pVideoBuffer->Stride[1] & 0XFFFF;
			}

			memcpy (&m_videoBuffer, pBuffer->pBuffer, sizeof (VO_VIDEO_BUFFER));
		}
		else
		{
			if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYUV420Planar)
			{
				m_videoBuffer.Buffer[0] = pBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = pBuffer->pBuffer + m_videoType.nFrameWidth * m_videoType.nFrameHeight;
				m_videoBuffer.Buffer[2] = pBuffer->pBuffer + m_videoType.nFrameWidth * m_videoType.nFrameHeight * 5 / 4;
				m_videoBuffer.Stride[0] = m_videoType.nFrameWidth;
				m_videoBuffer.Stride[1] = m_videoType.nFrameWidth / 2;
				m_videoBuffer.Stride[2] = m_videoType.nFrameWidth / 2;
				m_videoBuffer.ColorType = VO_COLOR_YUV_PLANAR420;

			}
			else if (m_videoFormat.eColorFormat == OMX_COLOR_Format_QcomHW)
			{
				m_videoBuffer.Buffer[0] = pBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = pBuffer->pBuffer + m_nYUVWidth * m_nYUVHeight;
				m_videoBuffer.Stride[0] = m_nYUVWidth;
				m_videoBuffer.Stride[1] = m_nYUVWidth;
				m_videoBuffer.ColorType = VO_COLOR_YUV_420_PACK_2;

				m_videoBuffer.Buffer[2] = (VO_PBYTE)pBuffer->pPlatformPrivate;
			}
		}

		OMX_TICKS	mediaTime = 0;
		if (m_pClockPort != NULL && m_nRendFrames > 0)
		{
			m_pClockPort->GetMediaTime (&mediaTime);

			while (mediaTime + 30 < pBuffer->nTimeStamp)
			{
				voOMXOS_Sleep (2);
				m_pClockPort->GetMediaTime (&mediaTime);

				VOLOGR ("Clock Time %d", (int)mediaTime);

				if (m_sTrans == COMP_TRANSSTATE_ExecuteToPause || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle ||
					m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
				{
					return OMX_ErrorNone;
				}
				if (m_sTrans == COMP_TRANSSTATE_None && m_sState != OMX_StateExecuting)
				{
					return OMX_ErrorNone;
				}
				if (m_pInput->IsFlush ())
				{
					return OMX_ErrorNone;
				}
			}
		}

		// detect the video render speed.
		if (m_nRendFrames == 0)
		{
			RenderVideo (&m_videoBuffer, pBuffer->nTimeStamp, VO_FALSE);
		}
		else if (m_nRendFrames > 1 && m_nRendFrames < 12)
		{
			VO_U32 nRenderTime = voOMXOS_GetSysTime ();

			RenderVideo (&m_videoBuffer, pBuffer->nTimeStamp, VO_FALSE);

			m_nRenderSpeed = m_nRenderSpeed + (voOMXOS_GetSysTime () - nRenderTime);
		}
		else
		{
			if (m_nRenderSpeed > 100)
			{
				if (mediaTime > pBuffer->nTimeStamp + 100)
				{
					if (m_nDroppedFrame)
					{
						RenderVideo (&m_videoBuffer, pBuffer->nTimeStamp, VO_FALSE);
						m_nDroppedFrame = VO_FALSE;
					}
					else
					{
						m_nDroppedFrame = VO_TRUE;
						bDrapped = OMX_TRUE;
					}
				}
				else
				{
					RenderVideo (&m_videoBuffer, pBuffer->nTimeStamp, VO_FALSE);
					m_nDroppedFrame = VO_FALSE;
				}
			}
			else
			{
				RenderVideo (&m_videoBuffer, pBuffer->nTimeStamp, VO_FALSE);
			}
		}

		if (m_pClockPort != NULL && m_nRendFrames == 0)
		{
			m_pClockPort->UpdateMediaTime (pBuffer->nTimeStamp);
//			if (!bDrapped)
//				m_pClockPort->RequestMediaTime (pBuffer->nTimeStamp, 0);
		}
		m_nRendFrames++;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXZoom2VideoSink::RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL);
		m_pPfmCompnThreadTime[m_nPfmFrameIndex] = m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		m_pPfmMediaTime[m_nPfmFrameIndex] = (VO_U32) nStart;
		m_pPfmSystemTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pVideoRender == NULL)
		CreateRender ();

	if (m_pVideoRender != NULL)
	{
		m_pVideoRender->Render (pVideoBuffer, nStart, bWait);
		VOLOGR ("Render Video Buffer!");
	}

	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime () - m_pPfmFrameTime[m_nPfmFrameIndex];
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL) - m_pPfmCodecThreadTime[m_nPfmFrameIndex];
	}
	m_nPfmFrameIndex++;

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXZoom2VideoSink::InitPortType (void)
{
	VOLOGF ();

	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 0;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	return OMX_ErrorNone;
}

VO_S32 voCOMXZoom2VideoSink::videosinkVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart)
{
	voCOMXZoom2VideoSink * pVideoSink = (voCOMXZoom2VideoSink *)pUserData;

	return pVideoSink->videosinkVideoRender (pVideoBuffer, pVideoFormat, nStart);
}

VO_S32 voCOMXZoom2VideoSink::videosinkVideoRender (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart)
{
	VOLOGR ();

	if (pVideoBuffer == NULL)
	{
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)pVideoFormat);
		return VO_ERR_FINISH;
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	if (m_cbRenderBuffer.pCallBack != NULL)
	{
		m_cbVideoBuffer.Buffer[0] = pVideoBuffer->Buffer[0];
		m_cbVideoBuffer.Buffer[1] = pVideoBuffer->Buffer[1];
		m_cbVideoBuffer.Buffer[2] = pVideoBuffer->Buffer[2];
		m_cbVideoBuffer.Stride[0] = pVideoBuffer->Stride[0];
		m_cbVideoBuffer.Stride[1] = pVideoBuffer->Stride[1];
		m_cbVideoBuffer.Stride[2] = pVideoBuffer->Stride[2];

		m_cbVideoBuffer.Width = pVideoFormat->Width;
		m_cbVideoBuffer.Height = pVideoFormat->Height;
		m_cbVideoBuffer.Time = m_pRenderBuffer->nTimeStamp;

		if (pVideoBuffer->ColorType == VO_COLOR_RGB565_PACKED)
			m_cbVideoBuffer.Color = OMX_COLOR_Format16bitRGB565;
		else
			m_cbVideoBuffer.Color = m_videoFormat.eColorFormat;

		if (m_cbVideoBuffer.Buffer[0] == NULL)
			return VO_ERR_FINISH;
		if (m_cbVideoBuffer.Stride[0] == 0)
			return VO_ERR_FINISH;

		errType = m_cbRenderBuffer.pCallBack (this, m_cbRenderBuffer.pUserData, m_pRenderBuffer, (OMX_INDEXTYPE)OMX_VO_IndexVideoBufferType, &m_cbVideoBuffer);

		if (errType == OMX_ErrorNone)
			return VO_ERR_FINISH;
		else
			return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_FINISH;
}
