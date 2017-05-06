	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXQcomVideoDec.cpp

	Contains:	voCOMXQcomVideoDec class file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18	East		Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXQcomVideoDec.h"

#define LOG_TAG "voCOMXQcomVideoDec"
#include "voLog.h"

#ifdef _LINUX_ANDROID
VideoHeap::VideoHeap(int fd, size_t size, void *base)
{
	// dup file descriptor, map once, use pmem
	init(dup(fd), base, size, 0, "/dev/pmem_adsp");
}
VideoHeap::~VideoHeap()
{
}
#endif	//_LINUX_ANDROID

voCOMXQcomVideoDec::voCOMXQcomVideoDec(OMX_COMPONENTTYPE* pComponent)
	: voCOMXBaseComponent(pComponent)
	, m_pInput (NULL)
	, m_pOutput (NULL)
	, m_pInputBuffer (NULL)
	, m_pOutputBuffer (NULL)
	, m_pInputQueue (NULL)
	, m_pInputSem (NULL)
	, m_pOutputQueue (NULL)
	, m_pOutputSem (NULL)
	, m_nOutBuffTime (0)
	, m_nOutBuffSize (0)
	, m_bInputEOS (OMX_FALSE)
	, m_bResetBuffer (OMX_FALSE)
	, m_bFlushed (OMX_FALSE)
	, m_nCoding(OMX_VIDEO_CodingAutoDetect)
	, m_nExtSize(0)
	, m_pExtData(NULL)
	, m_pVideoDec(NULL)
{
	strcpy(m_pObjName, __FILE__);
	strcpy(m_pName, "OMX.VOQCOM.Video.Decoder.XXX");

	memset(&m_videoType, 0, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));
}

voCOMXQcomVideoDec::~voCOMXQcomVideoDec()
{
	EndBufferHandleThread();

	ResetPortBuffer();

	if(m_pExtData != NULL)
	{
		voOMXMemFree(m_pExtData);
		m_pExtData = NULL;
	}

	DestroyVideoDec();
}

OMX_ERRORTYPE voCOMXQcomVideoDec::EmptyThisBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	m_bInputEOS = OMX_FALSE;

	return voCOMXBaseComponent::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXQcomVideoDec::FillThisBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	return voCOMXBaseComponent::FillThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXQcomVideoDec::GetParameter(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_INDEXTYPE nParamIndex, OMX_INOUT OMX_PTR pComponentParameterStructure)
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
				pVideoFormat->eCompressionFormat = m_nCoding;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatUnused;
			}
			else
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelCurrent:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXBaseComponent::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXQcomVideoDec::SetParameter(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_INDEXTYPE nIndex, OMX_IN OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorUnsupportedIndex;
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

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
				voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

				OMX_PARAM_PORTDEFINITIONTYPE * pOutType;
				m_pOutput->GetPortType (&pOutType);
				pOutType->format.video.nFrameWidth = m_videoType.nFrameWidth;
				pOutType->format.video.nFrameHeight = m_videoType.nFrameHeight;
				pOutType->format.video.nStride = m_videoType.nFrameWidth;
				pOutType->format.video.nBitrate = m_videoType.nBitrate;
				pOutType->format.video.xFramerate = m_videoType.xFramerate;

				pOutType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;
			}
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
				m_nCoding = pVideoFormat->eCompressionFormat;

				if (m_nCoding == OMX_VIDEO_CodingMPEG4)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingH263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VO_VIDEO_CodingDIV3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingAVC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingWMV)
					errType = OMX_ErrorNone;
				else
					errType = OMX_ErrorComponentNotFound;
			}
			else
				errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoMpeg4:
		{
			OMX_VIDEO_PARAM_MPEG4TYPE * pVideoFormat = (OMX_VIDEO_PARAM_MPEG4TYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_MPEG4TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			if (pVideoFormat->bGov == OMX_TRUE)
				return OMX_ErrorUndefined;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoWmv:
		{
			OMX_VIDEO_PARAM_WMVTYPE * pVideoFormat = (OMX_VIDEO_PARAM_WMVTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_WMVTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelQuerySupported:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PROFILELEVELTYPE));
			if (errType != OMX_ErrorNone)
				return errType;
			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNoMore;
		}
		break;

	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)pComponentParameterStructure;

			if (m_pExtData != NULL)
				voOMXMemFree (m_pExtData);
			m_pExtData = NULL;
			m_nExtSize = 0;

			if (pExtData->nDataSize > 0)
			{
				m_nExtSize = pExtData->nDataSize;
				m_pExtData = (OMX_S8*)voOMXMemAlloc (m_nExtSize);
				if (m_pExtData != NULL)
					voOMXMemCopy (m_pExtData, pExtData->data, m_nExtSize);
				else
					m_nExtSize = 0;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_nCoding = OMX_VIDEO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg4"))
				m_nCoding = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.h263"))
				m_nCoding = OMX_VIDEO_CodingH263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.divx"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingDIV3;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.avc"))
				m_nCoding = OMX_VIDEO_CodingAVC;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.vc1"))
				m_nCoding = OMX_VIDEO_CodingWMV;

			if (m_nCoding != OMX_VIDEO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;

			if(NULL == m_pInput)
				CreatePorts();

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
			pPortType->nBufferSize = 1024 * 64;

			m_pOutput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
		}
		break;

	default:
		errType = voCOMXBaseComponent::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::ComponentRoleEnum(OMX_IN OMX_HANDLETYPE hComponent, OMX_OUT OMX_U8* cRole, OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");
	if (nIndex == 0)
		strcpy ((char*)cRole, "video_decoder.mpeg4");
	else if (nIndex == 1)
		strcpy ((char*)cRole, "video_decoder.h263");
	else if (nIndex == 2)
		strcpy ((char*)cRole, "video_decoder.divx");
	else if (nIndex == 3)
		strcpy ((char*)cRole, "video_decoder.avc");
	else if (nIndex == 4)
		strcpy ((char*)cRole, "video_decoder.vc1");
	else 
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::CreatePorts()
{
	if(m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc(m_uPorts * sizeof(voCOMXBasePort*));
		if(m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;

		m_pInput = m_ppPorts[0] = new voCOMXBasePort(this, 0, OMX_DirInput);
		if(m_pInput == NULL)
			return OMX_ErrorInsufficientResources;

		m_pOutput = m_ppPorts[1] = new voCOMXBasePort(this, 1, OMX_DirOutput);
		if(m_pOutput == NULL)
			return OMX_ErrorInsufficientResources;

		m_pInput->SetCallbacks(m_pCallBack, m_pAppData);
		m_pOutput->SetCallbacks(m_pCallBack, m_pAppData);

		m_pInputQueue = m_pInput->GetBufferQueue();
		m_pInputSem = m_pInput->GetBufferSem();

		m_pOutputQueue = m_pOutput->GetBufferQueue();
		m_pOutputSem = m_pOutput->GetBufferSem();

		InitPortType();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::Flush(OMX_U32	nPort)
{
	if(m_pVideoDec != NULL)
		m_pVideoDec->Flush(ADSP_VDEC_PORTTYPE_ALL);

	ResetPortBuffer();

	voCOMXAutoLock lokc(&m_tmStatus);

	m_pInput->SetFlush(OMX_FALSE);
	m_pOutput->SetFlush(OMX_FALSE);

	m_bFlushed = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::DisablePort(OMX_U32 nPort)
{
	if(nPort == 1)
	{
		if(m_pInputSem->Waiting())
			m_pInputSem->Up();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::ResetPortBuffer()
{
	if(m_pInput == NULL || m_pOutput == NULL)
		return OMX_ErrorInvalidState;

	m_bResetBuffer = OMX_TRUE;

	OMX_U32 nInputNum = 0;
	OMX_PARAM_PORTDEFINITIONTYPE* psType = NULL;
	if(m_pInput->IsTunnel() && m_pInput->IsSupplier())
	{
		m_pInput->GetPortType(&psType);
		nInputNum = psType->nBufferCountActual;
	}

	OMX_U32 nOutputNum = 0;
	if(m_pOutput->IsTunnel() && m_pOutput->IsSupplier())
	{
		m_pOutput->GetPortType(&psType);
		nOutputNum = psType->nBufferCountActual;
	}

	OMX_U32 nTryTimes = 0;

	// try to wait the buffer reset
	while(m_pOutputQueue->Count () != nOutputNum || m_pOutputBuffer != NULL)
	{
		voOMXOS_Sleep(1);
		if(m_tsState.Waiting())
			m_tsState.Up();

		nTryTimes++;
		if(nTryTimes > 200)
			break;
	}

	// Force to rest the buffer
	nTryTimes = 0;
	while(m_pInputQueue->Count() != nInputNum || m_pInputBuffer != NULL || 
		m_pOutputQueue->Count() != nOutputNum || m_pOutputBuffer != NULL)
	{
		if(m_pOutputBuffer == NULL && (m_pInputBuffer != NULL  || m_pInputQueue->Count() != nInputNum))
		{
			if(m_pOutputSem->Waiting())
				m_pOutputSem->Up();
		}
		if((m_pOutputBuffer != NULL || m_pOutputQueue->Count() != nOutputNum) && m_pInputBuffer == NULL)
		{
			if(m_pInputSem->Waiting())
				m_pInputSem->Up();
		}

		if(m_hBuffThread == NULL)
			break;

		voOMXOS_Sleep(1);
		if (m_tsState.Waiting())
			m_tsState.Up();

		nTryTimes++;
		if(nTryTimes > 500)
		{
			VOLOGE("Name %s. Buffer status: Input: %d, %d, Output: %d, %d.", m_pName,
				(int)m_pInputQueue->Count(), (int)nInputNum, (int)m_pOutputQueue->Count(), (int)nOutputNum);

			break;
		}
	}

	m_pOutputSem->Reset();
	m_pInputSem->Reset();

	m_pInput->ResetBuffer(OMX_FALSE);
	m_pOutput->ResetBuffer(OMX_FALSE);

	m_bResetBuffer = OMX_FALSE;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::InitPortType()
{
	m_portParam[OMX_PortDomainVideo].nPorts = 2;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 20480;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingAutoDetect;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatUnused;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	m_pOutput->GetPortType (&pPortType);
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

OMX_U32 voCOMXQcomVideoDec::BufferHandle()
{
	while((m_pInput->IsSettingChange() || m_pOutput->IsSettingChange()) && m_sState > OMX_StateIdle)
		voOMXOS_Sleep(2);

	if (m_pInputBuffer == NULL)
	{
		m_pInputSem->Down();
		m_pInputBuffer = (OMX_BUFFERHEADERTYPE *) m_pInputQueue->Remove();
	}

	if (m_pOutputBuffer == NULL)
	{
		m_pOutputSem->Down();
		m_pOutputBuffer = (OMX_BUFFERHEADERTYPE *) m_pOutputQueue->Remove();
		if (m_pOutputBuffer != NULL)
		{
			m_pOutputBuffer->nFilledLen = 0;
			m_pOutputBuffer->nFlags = 0;
			m_pOutputBuffer->nTimeStamp = 0;
		}
	}

	if (m_sState <= OMX_StateIdle)
	{
		if (m_pInputBuffer != NULL)
		{
			m_pInput->ReturnBuffer (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}
		if (m_pOutputBuffer != NULL)
		{
			m_pOutput->ReturnBuffer (m_pOutputBuffer);
			m_pOutputBuffer = NULL;
		}

		return OMX_ErrorInvalidState;
	}

	if (m_pInputBuffer == NULL || m_pOutputBuffer == NULL)
	{
		if (!m_bResetBuffer)
		{
			VOLOGW ("Name %s. m_pInputBuffer = 0X%08X || m_pOutputBuffer = 0X%08X.State: %d, Trans %d",
				m_pName,  (int)m_pInputBuffer, (int)m_pOutputBuffer, m_sState, m_sTrans);
		}

		if (m_pInputBuffer != NULL)
		{
			m_pInput->ReturnBuffer (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}

		if (m_pOutputBuffer != NULL)
		{
			if (m_bInputEOS)
			{
				m_pOutputBuffer->nFilledLen = 0;
				m_pOutputBuffer->nFlags += OMX_BUFFERFLAG_EOS;

				voOMXOS_Sleep (10);
			}

			m_pOutput->ReturnBuffer (m_pOutputBuffer);
			m_pOutputBuffer = NULL;
		}

		return OMX_ErrorNotImplemented;
	}

	if (m_pInputBuffer->nFilledLen == 0 && (m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		return OMX_ErrorNone;
	}

	if (!m_pInput->IsFlush() && !m_pOutput->IsFlush())
	{
		if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
			m_tsState.Down ();
	}

	OMX_BOOL		bEmpty = OMX_FALSE;
	OMX_BOOL		bFill = OMX_FALSE;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	m_pOutputBuffer->nFilledLen = 0;
	m_pOutputBuffer->hMarkTargetComponent = m_pInputBuffer->hMarkTargetComponent;
	m_pOutputBuffer->nFlags = (m_pOutputBuffer->nFlags  | m_pInputBuffer->nFlags);
	m_pOutputBuffer->nTimeStamp = m_pInputBuffer->nTimeStamp;
	m_pOutputBuffer->pMarkData = m_pInputBuffer->pMarkData;

	if (m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS)
	{
		m_bInputEOS = OMX_TRUE;
		VOLOGI ("Name %s. OMX_BUFFERFLAG_EOS.", m_pName);
	}

	if ((m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS) && m_pInputBuffer->nFilledLen == 0)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		m_pOutputBuffer->nFilledLen  = m_nOutBuffSize;
		m_pOutputBuffer->nTimeStamp = m_nOutBuffTime;

		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;
		m_nOutBuffSize = 0;

		VOLOGI ("Name %s. OMX_BUFFERFLAG_EOS.", m_pName);

		return OMX_ErrorNone;
	}

	if (m_sTrans != COMP_TRANSSTATE_None)
	{
		while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
			voOMXOS_Sleep (2);
	}

	if (IsRunning () && !m_bResetBuffer)
	{
//		errType = FillBuffer (m_pInputBuffer, &bEmpty, m_pOutputBuffer, &bFill);
	}
	else
	{
		bEmpty = OMX_TRUE;
		bFill = OMX_TRUE;
	}

	m_bFlushed = OMX_FALSE;

	if (errType != OMX_ErrorNone)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;

		VOLOGW ("Name %s. FillBuffer was failed.", m_pName, errType);

		return errType;
	}

	if (bEmpty)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;
	}

	if (bFill)
	{
		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::SetNewState(OMX_STATETYPE newState)
{
	if(newState == OMX_StateIdle)
	{
		if(m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
		{
			if(m_pVideoDec == NULL)
				CreateVideoDec();
		}
		else
		{
			if(m_pVideoDec != NULL)
				m_pVideoDec->Flush(ADSP_VDEC_PORTTYPE_ALL);

			if(m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle)
			{
				ResetPortBuffer();

				return OMX_ErrorNone;
			}
		}
	}
	else if(newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		if(m_pVideoDec != NULL)
			m_pVideoDec->Flush(ADSP_VDEC_PORTTYPE_ALL);

		m_bInputEOS = OMX_FALSE;
	}
	else if(m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		EndBufferHandleThread();

		return OMX_ErrorNone;
	}

	return voCOMXBaseComponent::SetNewState(newState);
}

OMX_ERRORTYPE voCOMXQcomVideoDec::CreateVideoDec()
{
	m_pVideoDec = new CQcomVideoDec;
	if(m_pVideoDec == NULL)
		return OMX_ErrorInsufficientResources;

	VO_BOOL nRC = m_pVideoDec->Init(m_nCoding, (m_videoType.cMIMEType ? *((VO_U32 *)m_videoType.cMIMEType) : 0), 
		(VO_PBYTE)m_pExtData, m_nExtSize, m_videoType.nFrameWidth, m_videoType.nFrameHeight);

	if (nRC != VO_TRUE)
		return OMX_ErrorResourcesLost;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::DestroyVideoDec()
{
	if(m_pVideoDec)
	{
		m_pVideoDec->Uninit();

		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXQcomVideoDec::EndBufferHandleThread()
{
	m_bBuffThreadStop = OMX_TRUE;

	while(m_hBuffThread != NULL)
	{
		if(m_pInputSem->Waiting())
			m_pInputSem->Up();
		if (m_pOutputSem->Waiting())
			m_pOutputSem->Up();

		voOMXOS_Sleep(1);
	}

	return OMX_ErrorNone;
}