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
#include "voOMXOSFun.h"
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voCOMXTIVideoSink.h"
#include "voOMXThread.h"
#include "voLog.h"
#include "voinfo.h"

#define VO_VIDEOSINK_MAXDROPFRAME 10
voCOMXTIVideoSink::voCOMXTIVideoSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSink (pComponent)
	, m_pClockPort (NULL)
	, m_pOverlayPort (NULL)
	, m_pview(NULL)
	, m_pVideoRender(NULL)
	, m_pheldbuf(NULL)
	, m_nRendFrames (0)
	, m_nRenderSpeed (0)
	, m_nsleepcnt(0)
	, m_nTotalTime(0)
	, m_nlastsystime(0)
	, m_lllastframets(0)
	, m_naveragedura(0)
	, m_nDroppedFrame (0)
	, m_bDropVideo (VO_TRUE)
	, m_bDecodedSize(VO_FALSE)
	, m_llLastFrameTime (0)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, VOCMP_TIOMAP4_HWVIDEORENDER_NAME);

	m_videoFormat.eColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;//OMX_COLOR_FormatCbYCrY;
}

voCOMXTIVideoSink::~voCOMXTIVideoSink(void)
{
	if (m_pVideoRender != NULL) {
		delete m_pVideoRender;
		m_pVideoRender = NULL;

		if (m_pInput) 
			((voCOMXTIOverlayPort*)m_pInput)->m_pBufferAllocator = NULL;
	}

	m_pheldbuf = NULL;
}

OMX_ERRORTYPE voCOMXTIVideoSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
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
				pVideoFormat->eColorFormat = m_videoFormat.eColorFormat;
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
	case OMX_VO_IndexVideoUpsideDown:
		{
			if (m_pVideoRender == NULL)
				CreateRender();

			m_pVideoRender->setUpsideDown(*((bool*)pComponentParameterStructure));
			VOLOGI("%d : this is VP6 upside-down video, so, set this info to sink component.", *((bool*)pComponentParameterStructure));
		}
		break;

	case OMX_IndexParamForceUseThisBufferNumber: 
		{
				if (m_pVideoRender == NULL)
					CreateRender();

				if (m_pVideoRender)
					m_pVideoRender->setForceBufNum(*(unsigned int*)pComponentParameterStructure);

				VOINFO("accept the forcible buffer number: %d", *(unsigned int*)pComponentParameterStructure);
		}
		break;

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

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			OMX_U32 oldw = m_videoType.nFrameWidth;
			OMX_U32 oldh = m_videoType.nFrameHeight;

			voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

			OMX_BOOL recreated = OMX_FALSE;
			if (pPortType->nBufferCountActual != pType->nBufferCountActual ||
					oldw != m_videoType.nFrameWidth || oldh != m_videoType.nFrameHeight)	
			{
				recreated = OMX_TRUE;
				m_pInput->FreeBuffer(0, NULL);  
				pPortType->nBufferCountActual = pType->nBufferCountActual;
			}

			VOINFO("recreated: %d, video: %d x %d, buffer count: %d", recreated, m_videoType.nFrameWidth, m_videoType.nFrameHeight, pPortType->nBufferCountActual);
			if (m_videoType.nFrameWidth * m_videoType.nFrameHeight > 0) {

				if (m_bDecodedSize == VO_FALSE)
					pPortType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

				if (m_pVideoRender == NULL)
					CreateRender();

				if (m_pVideoRender) {
					m_pVideoRender->setForceBufNum(pPortType->nBufferCountActual);
					m_pVideoRender->setVideoInfo(m_videoType.nFrameWidth, m_videoType.nFrameHeight, m_videoFormat.eColorFormat);
				}

				if (recreated == OMX_FALSE)
					((voCOMXTIOverlayPort*)m_pInput)->RequestBuffer(0);  
				else if (recreated == OMX_TRUE)
					((voCOMXTIOverlayPort*)m_pInput)->RequestBuffer(1);  
			}
#if defined __VODBG__
			VOINFO("actual buffer count: %d", pPortType->nBufferCountActual);
			m_nemptystart = voOMXOS_GetSysTime();
#endif
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
				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar 
					|| pVideoFormat->eColorFormat == 0x7F000001 /*OMX_TI_COLOR_FormatYUV420PackedSemiPlanar_Sequential_TopBottom*/) {

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
	switch (nIndex)
	{
	case OMX_VO_IndexConfigDisplayArea:
		{
			OMX_VO_DISPLAYAREATYPE * pDisp = (OMX_VO_DISPLAYAREATYPE *)pComponentConfigStructure;

			m_pview = pDisp->hView;

			if (m_pVideoRender == NULL) 
				CreateRender();

			if (m_pVideoRender)
				m_pVideoRender->SetView((void*)pDisp->hView);

			m_nsleepcnt = 0;
			m_nRendFrames = 0;
			m_nRenderSpeed = 0;
			m_naveragedura = 0;
			m_nDroppedFrame = 0; 
			m_bDropVideo = VO_FALSE;
			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexTIHWDecodedDimension:
		{
			OMX_VO_TIHWDecodedDimension* pinfo = (OMX_VO_TIHWDecodedDimension*)pComponentConfigStructure;
			if (pinfo == NULL) 
				return OMX_ErrorPortsNotCompatible;
			
			VOLOGI("accept decoded dimension: w x h = %d x %d, rotate: %d", pinfo->nDecodedWidth, pinfo->nDecodedHeight, pinfo->nRotationDegree);
			if (pinfo->nDecodedWidth * pinfo->nDecodedHeight > 0) {
				m_bDecodedSize = VO_TRUE;
				OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
				m_pInput->GetPortType (&pPortType);
				pPortType->nBufferSize = pinfo->nDecodedWidth * pinfo->nDecodedHeight * 2;

				if (m_pVideoRender == NULL)
					CreateRender();

				if (m_pVideoRender)
					m_pVideoRender->setDimensionInfo(pinfo->nDecodedWidth, pinfo->nDecodedHeight, pinfo->nRotationDegree);
			}
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

	//m_nRendFrames = 0;
	//m_llLastFrameTime = 0;
	
	VOINFO("flushing .....................");
	//m_nRenderSpeed = 0;

	if (m_pheldbuf) {
		((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
		OMX_BUFFERHEADERTYPE* pb = m_pheldbuf;
		VOINFO("return the held buffer[%d]: %p, pBuffer: %p", ((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pb), pb, pb->pBuffer);
		m_pInput->ReturnBuffer(pb);
		m_pheldbuf = NULL;
	}

	m_nsleepcnt = 0;
	m_naveragedura = 0;
	OMX_ERRORTYPE errType = voCOMXBaseComponent::Flush(nPort);

	VOINFO("flushed over.....................");
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
			m_llLastFrameTime = pRenderBuffer->nTimeStamp;
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
			return OMX_ErrorNone;
		}
	}

	if (pRenderBuffer->nTimeStamp < m_llSeekTime)
	{
		VOINFO("dropping frames. seektime: %d, ts: %d", m_llSeekTime, pRenderBuffer->nTimeStamp);
		pRenderBuffer->nFilledLen = 0;
		pRenderBuffer->nTimeStamp = 0;
		((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
		return OMX_ErrorNone;
	}

	if (m_videoFormat.eColorFormat != OMX_COLOR_FormatYUV420Planar) { // for HW

		if (m_pheldbuf != NULL) {

			OMX_BUFFERHEADERTYPE *pcb = pRenderBuffer;
			if (pcb->nTimeStamp < m_pheldbuf->nTimeStamp) {
				OMX_TICKS curts = pcb->nTimeStamp;
				pcb->nTimeStamp = m_pheldbuf->nTimeStamp;
				m_pheldbuf->nTimeStamp = curts;
				VOINFO("exchanging timestamp! previous = %d, current = %d", pcb->nTimeStamp, curts);
			}

			pRenderBuffer = m_pheldbuf;
			m_pheldbuf = pcb;

		} else {
			m_pheldbuf = pRenderBuffer;
			VOINFO("hold buffer[%d]: %p, pBuffer: %p", ((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pBuffer), pBuffer, pBuffer->pBuffer);
			return OMX_ErrorNone;
		}
	} else {   // for SW
		if (m_llLastFrameTime > 0)
		{
			if (pRenderBuffer->nTimeStamp < m_llLastFrameTime && pRenderBuffer->nTimeStamp + 30000 > m_llLastFrameTime)
			{
				VOINFO("The last frame time is %d, %d, %d. Drop this frame!", (int)m_llLastFrameTime, (int)pRenderBuffer->nTimeStamp, (int)(pRenderBuffer->nTimeStamp - m_llLastFrameTime));
				m_llLastFrameTime = pRenderBuffer->nTimeStamp;

				pRenderBuffer->nFilledLen = 0;
				pRenderBuffer->nTimeStamp = 0;
				((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
				return OMX_ErrorNone;
			}
		}
	}

#if defined __VODBG1__
	VOINFO("lasttime: %lld, buffer time: %d", m_llLastFrameTime, pRenderBuffer->nTimeStamp);
#endif
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
		VOINFO("--------------------clock start at: %d----------------------", pRenderBuffer->nTimeStamp);
	}

	OMX_TICKS mediaTime = 0;
	if(m_pClockPort != NULL)
		m_pClockPort->GetMediaTime (&mediaTime);

	// detect the video render speed.
	int rt = 0;
	if (m_nRendFrames == 0)
	{
		m_nlastsystime = m_nTotalTime = voOMXOS_GetSysTime();
		m_lllastframets = 0;
		rt = RenderBuffer (pRenderBuffer);
	}
	else if (m_nRendFrames > 0 && m_nRendFrames < 11)
	{
		VO_U32 nRenderTime = voOMXOS_GetSysTime ();

		rt = RenderBuffer (pRenderBuffer);
		m_nRenderSpeed += (voOMXOS_GetSysTime() - nRenderTime);
		if (m_nRendFrames == 11)
			VOINFO("Render Speed is: %d", m_nRenderSpeed);

		m_naveragedura = pRenderBuffer->nTimeStamp / 10;
	}
	else
	{
		if (m_bDropVideo && m_videoFormat.eColorFormat == OMX_COLOR_FormatYUV420Planar)
		{
			VO_S32 nDrop = (mediaTime - pRenderBuffer->nTimeStamp) / 100;
			if (nDrop > VO_VIDEOSINK_MAXDROPFRAME)
				nDrop = VO_VIDEOSINK_MAXDROPFRAME;

			if (nDrop <= m_nDroppedFrame)
			{
				rt = RenderBuffer (pRenderBuffer);
				m_nDroppedFrame = 0;
			}
			else
			{
				m_nDroppedFrame++; 
				VOINFO("dropped: %d, mediatime %d, timestamp %d, systemtime %d", m_nDroppedFrame, (OMX_U32)mediaTime, (OMX_U32)pRenderBuffer->nTimeStamp, voOMXOS_GetSysTime());
				((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
			}
		}
		else
		{
			rt = RenderBuffer (pRenderBuffer);
		}
	}

	//OMX_TICKS dist = mediaTime - pRenderBuffer->nTimeStamp;
	//VOINFO("timestamp audio: %lld, video: %lld, dist: %lld", mediaTime, pRenderBuffer->nTimeStamp, dist);
	//VOINFO("frame: %d, timestamp: %lld, rt: %d", m_nRendFrames, pRenderBuffer->nTimeStamp, rt);
	if (pRenderBuffer->nTimeStamp * mediaTime)
		WaitForRenderTime(pRenderBuffer->nTimeStamp + 30, NULL);
	m_nRendFrames++;

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

		while (mediaTime < nTime)
		{
			//VOINFO("systime: %d, mt: %lld, ts: %lld", voOMXOS_GetSysTime(), mediaTime, nTime);
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
	if (m_pVideoRender == NULL) {
		m_pVideoRender = new voTIVideoRender;
		
		if (m_pInput) 
			((voCOMXTIOverlayPort*)m_pInput)->m_pBufferAllocator = m_pVideoRender;
	}

	return OMX_ErrorNone;
}

int voCOMXTIVideoSink::RenderBuffer(OMX_BUFFERHEADERTYPE * pBuffer) {

#if defined __VODBG__
	OMX_TICKS curframets = 0; 
#endif

	int curstart = 0; 
	int rendertime = 0;

	if (pBuffer->nFilledLen > 0 && m_pVideoRender != NULL) {

		curstart = voOMXOS_GetSysTime();

		m_pVideoRender->Render(pBuffer->pBuffer + pBuffer->nOffset, pBuffer->nFilledLen);
	
		rendertime = voOMXOS_GetSysTime() - curstart;
		if (m_naveragedura < 17) {
			VOINFO("framecount: %d", m_nRendFrames);
			if (m_nsleepcnt < 1) {
				voOMXOS_Sleep(20);
				m_nsleepcnt++;
			} else if (m_nsleepcnt == 1) {
				m_nsleepcnt = 0;
			}
		}

#if defined __VODBG__ 
		curframets = pBuffer->nTimeStamp;
#endif
	}

#if defined __VODBG__ 
	int cursystime = voOMXOS_GetSysTime();
	rendertime = cursystime - curstart;
	OMX_TICKS mediaTime = 0;
	if(m_pClockPort != NULL)
		m_pClockPort->GetMediaTime (&mediaTime);
	VOINFO("frame: %d, mediatime: %lld, timestamp: %lld, frame interval: %lld, system time: %d, system interval: %d, render time: %d", 
					m_nRendFrames, mediaTime, curframets, curframets-m_lllastframets, cursystime, cursystime - m_nlastsystime, rendertime);
	m_lllastframets = curframets;
	m_nlastsystime = cursystime;
#endif
	return rendertime;
}

OMX_U32	voCOMXTIVideoSink::BufferHandle (void) {

	if (!m_pInput->IsFlush())
	{
		if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
			m_tsState.Down ();
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	m_pSemaphore->Down ();

	OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *) m_pBufferQueue->Remove ();
	//VOINFO("input buffer[%d]: %p, pBuffer: %p, filledlen: %d",((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pBuffer), pBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
	if (pBuffer != NULL)
	{
		if (m_sState <= OMX_StateIdle)
		{
			VOINFO("return the buffer[%d]: %p, pBuffer: %p", ((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pBuffer), pBuffer, pBuffer->pBuffer);
			m_pInput->ReturnBuffer (pBuffer);
			return OMX_ErrorInvalidState;
		}

		if (m_sTrans != COMP_TRANSSTATE_None)
		{
			while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute) {
				VOINFO("m_nTrans: %d", m_sTrans);
				voOMXOS_Sleep (2);
			}
		}


#if defined __VODBG1__
		int nstart = voOMXOS_GetSysTime();
#endif

		if (IsRunning() && !m_pInput->IsFlush() && pBuffer->nFilledLen > 0) {

#if defined __VODBG1__
			VOINFO(" >>>>>>>>>>>>>>>>>>>>>>> FillBuffer: %p, buffer: %p, at: %d", pBuffer, pBuffer->pBuffer, voOMXOS_GetSysTime()-m_nemptystart);
#endif
			errType = FillBuffer(pBuffer);
		} else {
			VOINFO("directly return buffer[%d]: %p, pBuffer: %p, filledlen: %d", ((voCOMXTIOverlayPort*)m_pInput)->GetBufferIndex(pBuffer), pBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
			((voCOMXTIOverlayPort*)m_pInput)->SetFrameDropped();
		}

		if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) && !m_bEOS)
		{
			m_nTotalTime = voOMXOS_GetSysTime() - m_nTotalTime;
			VOLOGI ("frames: %d, time: %d, fps: %0.3f", m_nRendFrames, m_nTotalTime, (float)m_nRendFrames*1000/m_nTotalTime);
	
			m_bEOS = OMX_TRUE;
			m_llPlayTime = 0;
			if (m_pCallBack != NULL)
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL); 
		}

		m_pInput->ReturnBuffer(pBuffer);
#if defined __VODBG1__
		VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<< header %p, buffer: %p, usedtime: %d", pBuffer, pBuffer->pBuffer, voOMXOS_GetSysTime() - nstart);
#endif
	}

	return errType;
}

#if defined __VODBG__

OMX_ERRORTYPE voCOMXTIVideoSink::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer) {

	VO_U32	last = m_nemptystart;
	m_nemptystart = voOMXOS_GetSysTime();
	//VOINFO(">>>>>>>>>>>>>>>>>>>>>>>>>>>> header: %p, buffer: %p, begin at: %d, last: %d, interval: %d", pBuffer, pBuffer->pBuffer, m_nemptystart, last, m_nemptystart-last);
	VOINFO("header: %p, buffer: %p, filledlen: %d", pBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
	return voCOMXBaseComponent::EmptyThisBuffer(hComponent, pBuffer);
}
#endif


OMX_ERRORTYPE voCOMXTIVideoSink::InitPortType (void)
{
	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;


	pPortType->nBufferCountActual = 22;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 0;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 16;

	pPortType->format.video.cMIMEType = "YUV422";
	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;
	//m_pInput->SetPortType(pPortType);

	OMX_PARAM_BUFFERSUPPLIERTYPE supType;

	m_pInput->GetSupplier (&supType);
	supType.eBufferSupplier = OMX_BufferSupplyInput;
	m_pInput->SetSupplier (&supType);

	return OMX_ErrorNone;
}

