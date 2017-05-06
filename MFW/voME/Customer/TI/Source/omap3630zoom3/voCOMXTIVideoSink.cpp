	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTIVideoSink.cpp

	Contains:	voCOMXTIVideoSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#define LOG_TAG "voCOMXTIVideoSink"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voOMXOSFun.h"
#include "voOMXBase.h"
#include "voOMXMemory.h"

#include "voCOMXTIVideoSink.h"
#include "voOMXThread.h"
#include "voLog.h"
#include "voinfo.h"

voCOMXTIVideoSink::voCOMXTIVideoSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSink (pComponent)
	, m_pClockPort (NULL)
	, m_pOverlayPort (NULL)
	, m_pview(NULL)
	, m_pVideoRender(NULL)
	, m_nRendFrames (0)
	, m_nRenderSpeed (0)
	, m_nTotalTime(0)
	, m_nlastsystime(0)
	, m_lllastframets(0)
	, m_bDroppedFrame (VO_TRUE)
	, m_bDropVideo (VO_TRUE)
	, m_bRenderThreadCreated(VO_FALSE)
	, m_llLastFrameTime (0)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VOTI.Video.Render");

	m_videoFormat.eColorFormat = OMX_COLOR_FormatCbYCrY;

	VOLOGF ();
}

voCOMXTIVideoSink::~voCOMXTIVideoSink(void)
{
	VOLOGF();

	if (m_pVideoRender != NULL) {
		delete m_pVideoRender;
		m_pVideoRender = NULL;

		if (m_pInput) 
			((voCOMXTIOverlayPort*)m_pInput)->m_pBufferAllocator = NULL;
	}
}

OMX_ERRORTYPE voCOMXTIVideoSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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
				pVideoFormat->eColorFormat = OMX_COLOR_FormatCbYCrY;
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


OMX_ERRORTYPE voCOMXTIVideoSink::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if (pComponentParameterStructure == NULL)
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

			if (m_videoType.nFrameHeight >= 1080) {
				VOINFO("1080p not supported");
				return OMX_ErrorNotImplemented;
			}

			pPortType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;
			if (m_videoType.nFrameWidth * m_videoType.nFrameHeight) {
				if (m_pVideoRender == NULL)
					CreateRender();

				if (m_pVideoRender)
					m_pVideoRender->setVideoInfo(m_videoType.nFrameWidth, m_videoType.nFrameHeight, m_videoFormat.eColorFormat);

				}
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			if (pVideoFormat->nPortIndex == 0)
			{
				m_videoFormat.eColorFormat = pVideoFormat->eColorFormat;

				OMX_PARAM_BUFFERSUPPLIERTYPE supType;
				m_pInput->GetSupplier (&supType);
				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatCbYCrY) {
					supType.eBufferSupplier = OMX_BufferSupplyInput;
					m_pInput->SetSupplier (&supType);
					return OMX_ErrorNone;
				} else if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420Planar) {
					supType.eBufferSupplier = OMX_BufferSupplyUnspecified;
					m_pInput->SetSupplier (&supType);
					return OMX_ErrorNone;
				}

				errType = OMX_ErrorPortsNotCompatible;
			}
		}
		break;

	case OMX_IndexParamCommonExtraQuantData:
			errType = OMX_ErrorPortsNotCompatible;
		break;

	default:
		errType = voCOMXCompSink::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXTIVideoSink::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nIndex);

	switch (nIndex)
	{
	case OMX_VO_IndexConfigDisplayArea:
		{
			OMX_VO_DISPLAYAREATYPE * pDisp = (OMX_VO_DISPLAYAREATYPE *)pComponentConfigStructure;

			m_pview = pDisp->hView;
			if (m_pVideoRender == NULL) 
				CreateRender();

			if (m_pVideoRender) {
				m_pVideoRender->SetView((void*)pDisp->hView);

				if (m_videoType.eColorFormat == OMX_COLOR_FormatYUV420Planar)
					m_pVideoRender->CreateOverlay(m_videoType.nFrameWidth, m_videoType.nFrameHeight);
			}

			m_nRendFrames = 0;
			m_nRenderSpeed = 0;
			m_bDroppedFrame = VO_TRUE; // true
			m_bDropVideo = VO_FALSE; // false 

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSink::SetConfig (hComponent, nIndex, pComponentConfigStructure);

}

OMX_ERRORTYPE voCOMXTIVideoSink::CreatePorts (void)
{
	VOLOGF ();

	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		m_pInput = m_ppPorts[0] = new voCOMXTIOverlayPort (this, 0, OMX_DirInput);
		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);
		m_pOverlayPort = (voCOMXTIOverlayPort *)m_pInput;

		m_pBufferQueue = m_pInput->GetBufferQueue ();
		m_pSemaphore = m_pInput->GetBufferSem ();

		m_ppPorts[1] = new voCOMXPortClock (this, 1, OMX_DirInput);
		m_pClockPort = (voCOMXPortClock *)m_ppPorts[1];
		m_pClockPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pClockPort->SetClockType (OMX_TIME_RefClockVideo);

		InitPortType();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXTIVideoSink::Flush (OMX_U32	nPort)
{
	VOLOGF ();

	m_nRendFrames = 0;
	m_llLastFrameTime = 0;
	m_nRenderSpeed = 0;

	//m_pVideoRender->Flush();
	OMX_ERRORTYPE errType = voCOMXBaseComponent::Flush(nPort);

	return errType;
}


OMX_ERRORTYPE voCOMXTIVideoSink::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	voCOMXAutoLock lokc (&m_tmStatus);

	OMX_BUFFERHEADERTYPE * pRenderBuffer = pBuffer;

	if (m_bSetPosInPause)
	{
		if (m_nRendFrames >= 1)
		{
			voOMXOS_Sleep (30);
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
			return OMX_ErrorNone;
		}
	}

	if (pRenderBuffer->nTimeStamp < m_llSeekTime)
	{
		pRenderBuffer->nFilledLen = 0;
		pRenderBuffer->nTimeStamp = 0;
		((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
		return OMX_ErrorNone;
	}

	if (m_llLastFrameTime > 0)
	{
		if (pRenderBuffer->nTimeStamp < m_llLastFrameTime &&  pRenderBuffer->nTimeStamp + 30000 > m_llLastFrameTime)
		{
			VOLOGW ("The last frame time is %d, %d, %d. Drop this frame!", (int)m_llLastFrameTime, (int)pRenderBuffer->nTimeStamp, (int)(pRenderBuffer->nTimeStamp - m_llLastFrameTime));
			m_llLastFrameTime = pRenderBuffer->nTimeStamp;

			pRenderBuffer->nFilledLen = 0;
			pRenderBuffer->nTimeStamp = 0;
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
			return OMX_ErrorNone;
		}
	}
	m_llLastFrameTime = pRenderBuffer->nTimeStamp;


	if (m_pClockPort != NULL && (m_nRendFrames == 0 || (pRenderBuffer->nFlags & OMX_BUFFERFLAG_STARTTIME)))
	{
		OMX_COMPONENTTYPE * pComp = m_pClockPort->GetTunnelComp ();
		if (pComp != NULL)
		{
			OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkType;

			pComp->GetConfig (pComp, OMX_IndexConfigTimeActiveRefClock, &clkType);
			if (clkType.eClock == OMX_TIME_RefClockVideo)
			{
				m_pClockPort->StartWallClock (1);
			}
		}
	}


	if (m_pClockPort != NULL && m_nRendFrames == 0) {
		m_nlastsystime = voOMXOS_GetSysTime ();
		m_pClockPort->UpdateMediaTime (pRenderBuffer->nTimeStamp);
		//VOLOGI("--------------------clock start at: %d----------------------", pRenderBuffer->nTimeStamp);
	}

	OMX_TICKS mediaTime = 0;
	if(m_pClockPort != NULL)
		m_pClockPort->GetMediaTime (&mediaTime);

	// detect the video render speed.
	if (m_nRendFrames == 0)
	{
		m_nTotalTime = voOMXOS_GetSysTime();
		m_nlastsystime = m_nTotalTime;
		m_lllastframets = 0;
		RenderBuffer (pRenderBuffer);
	}
	else if (m_nRendFrames > 0 && m_nRendFrames < 11)
	{
		VO_U32 nRenderTime = voOMXOS_GetSysTime ();

		RenderBuffer (pRenderBuffer);
		m_nRenderSpeed = m_nRenderSpeed + (voOMXOS_GetSysTime () - nRenderTime);
		if (m_nRendFrames == 11)
			VOLOGI ("Render Speed is: %d", m_nRenderSpeed);
	}
	else
	{
		if (m_bDropVideo)
		{
			if (mediaTime > 5000 && mediaTime > pRenderBuffer->nTimeStamp + 100)
			{
				if (m_bDroppedFrame)
				{
					RenderBuffer (pRenderBuffer);
					m_bDroppedFrame = VO_FALSE;
				}
				else
				{
					((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
					m_bDroppedFrame = VO_TRUE;
				}
			}
			else
			{
				RenderBuffer (pRenderBuffer);
				m_bDroppedFrame = VO_FALSE;
			}
		}
		else
		{
			RenderBuffer (pRenderBuffer);
		}
	}

	//OMX_TICKS dist = mediaTime - pRenderBuffer->nTimeStamp;
	//VOLOGI("timestamp audio: %lld, video: %lld, dist: %lld", mediaTime, pRenderBuffer->nTimeStamp, dist);
	//VOLOGI("frame: %d, timestamp: %lld", m_nRendFrames, pRenderBuffer->nTimeStamp);
	WaitForRenderTime(pRenderBuffer->nTimeStamp + 30, NULL);
	m_nRendFrames++;
	//VOLOGI("frame: %d", m_nRendFrames);

	m_llPlayTime = pRenderBuffer->nTimeStamp;
	m_nRenderFrames++;

	if (m_pCallBack != NULL)
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_VideoRenderFrames, m_nRendFrames, 0, NULL);

	pRenderBuffer->nFilledLen = 0;
	pRenderBuffer->nTimeStamp = 0;
	return OMX_ErrorNone;
}

OMX_BOOL voCOMXTIVideoSink::WaitForRenderTime (OMX_TICKS nTime, OMX_TICKS* pnCurrMediaTime)
{
	OMX_TICKS mediaTime = 0;
	if (m_pClockPort)
	{
		m_pClockPort->GetMediaTime (&mediaTime);

		//VOLOGI("systime: %s, mt: %lld, ts: %lld", __TIME__, mediaTime, nTime);
		while (mediaTime < nTime)
		{
			voOMXOS_Sleep(2);
			m_pClockPort->GetMediaTime(&mediaTime);

			// it is IDLE status
			if(mediaTime == 0)
			{
				OMX_COMPONENTTYPE* pComp = m_pClockPort->GetTunnelComp();
				if(pComp)
				{
					OMX_STATETYPE state = OMX_StateInvalid;
					pComp->GetState(pComp, &state);
					if(state != OMX_StateExecuting)
						return OMX_FALSE;
				}
			}

			// if the time stamp is wrong, sleep and break it.
			if(nTime > mediaTime + 30000 || mediaTime > nTime + 30000)
			{
				voOMXOS_Sleep(30);
				return OMX_FALSE;
			}

			if (m_sTrans == COMP_TRANSSTATE_ExecuteToPause || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle ||
				m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
				return OMX_FALSE;

			if (m_sTrans == COMP_TRANSSTATE_None && m_sState != OMX_StateExecuting)
				return OMX_FALSE;

			if (m_pInput->IsFlush())
				return OMX_FALSE;
		}
	}

	if(pnCurrMediaTime)
		*pnCurrMediaTime = mediaTime;

	return OMX_TRUE;
}


OMX_ERRORTYPE voCOMXTIVideoSink::CreateRender(void)
{
	VOLOGF ();

	if (m_pVideoRender == NULL) {
		m_pVideoRender = new voTIVideoRender;
		
		if (m_pInput) 
			((voCOMXTIOverlayPort*)m_pInput)->m_pBufferAllocator = m_pVideoRender;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voCOMXTIVideoSink::RenderBuffer(OMX_BUFFERHEADERTYPE * pBuffer) {

	OMX_TICKS curframets = 0, rts; 
	//VO_U32 start = voOMXOS_GetSysTime();
	if (pBuffer->nFilledLen > 0 && m_pVideoRender != NULL) {
		curframets = pBuffer->nTimeStamp;
		m_pClockPort->GetMediaTime (&rts);
		//VOLOGI("sys: %d, frame: %d, fts: %lld, rts: %lld", voOMXOS_GetSysTime(), m_nRendFrames, curframets, rts);
		m_pVideoRender->Render(pBuffer->pBuffer + pBuffer->nOffset);
	}

	//VOLOGI("thread: %d, frame: %d, timestamp: %lld, frame interval: %lld, system time: %d, system interval: %d, render time: %d", 
	//				pthread_self(), m_nRendFrames, curframets, curframets-m_lllastframets, cursystime, cursystime - m_nlastsystime, cursystime - curstart);
	//VOLOGI("frame: %d, timestamp: %lld, frame interval: %lld", m_nRendFrames, curframets, curframets-m_lllastframets);

	//m_lllastframets = curframets;
	return OMX_ErrorNone;
}

OMX_U32	voCOMXTIVideoSink::BufferHandle (void) {

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	m_pSemaphore->Down ();

	if (!m_pInput->IsFlush())
	{
		if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
			m_tsState.Down ();
	}

	OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *) m_pBufferQueue->Remove ();
	VOINFO("input buffer[%d]: %p, pBuffer: %p, filledlen: %d",((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pBuffer), pBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
	if (pBuffer != NULL)
	{
		if (m_sState <= OMX_StateIdle)
		{
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
			m_pInput->ReturnBuffer (pBuffer);
			return OMX_ErrorInvalidState;
		}

		if (m_sTrans != COMP_TRANSSTATE_None)
		{
			while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
				voOMXOS_Sleep (2);
		}

		if (IsRunning () && !m_pInput->IsFlush() && pBuffer->nFilledLen > 0)
			 errType = FillBuffer (pBuffer);
		else {
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
		}

		if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) && !m_bEOS) {

			m_nTotalTime = voOMXOS_GetSysTime() - m_nTotalTime;
			VOLOGI ("frames: %d, time: %d, fps: %0.3f", m_nRendFrames, m_nTotalTime, (float)m_nRendFrames*1000/m_nTotalTime);
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped(2);
	
			m_bEOS = OMX_TRUE;
			m_llPlayTime = 0;
			if (m_pCallBack != NULL)
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL); 
		}

		m_pInput->ReturnBuffer(pBuffer);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXTIVideoSink::InitPortType (void)
{
	VOLOGF ();

	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 6;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 0;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.video.cMIMEType = "YUV422";
	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatCbYCrY;//OMX_COLOR_FormatYUV420Planar;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;
	//m_pInput->SetPortType(pPortType);

	OMX_PARAM_BUFFERSUPPLIERTYPE supType;

	m_pInput->GetSupplier (&supType);
	supType.eBufferSupplier = OMX_BufferSupplyInput;
	m_pInput->SetSupplier (&supType);

	return OMX_ErrorNone;
}
