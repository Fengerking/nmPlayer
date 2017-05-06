	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseComp.cpp

	Contains:	CBaseComp class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <string.h>
#include <voOMX_Index.h>

#include "voOMXOSFun.h"
#include "CBaseComp.h"
#include "CFileSource.h"

#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CBaseComp"
#endif
#include "voLog.h"
#include "voinfo.h"

//#pragma warning (disable : 4996)

CBaseComp::CBaseComp(VO_CODEC_INIT_USERDATA * pUserData)
	: m_pComponent (NULL)
	, m_sState (OMX_StateInvalid)
	, m_bSendMsg (OMX_FALSE)
	, m_pInputPort (NULL)
	, m_pOutputPort (NULL)
	, m_pHeadData (NULL)
	, m_nHeadSize (0)
	, m_bHeadDone (VO_FALSE)
	, m_nCoding (0)
	, m_nVideoWidth(640)
	, m_nVideoHeight(480)
	, m_nColorType (VO_COLOR_YUV_PLANAR420)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nSampleBits (16)
	, m_pBufferList(new CWrapBufferList())
	, m_pInputBuffer(NULL)
	, m_bResetOutputPort (OMX_FALSE)
	, m_pMsgThread (NULL)
	, m_bThumbnailMode(VO_FALSE)
	, m_pSource(NULL)
	, m_nAudioTrack(-1)
	, m_nVideoTrack(-1)
	, mpSurface(NULL)
{
	m_CallBack.EventHandler = voOMXEventHandler;
	m_CallBack.EmptyBufferDone = voOMXEmptyBufferDone;
	m_CallBack.FillBufferDone = voOMXFillBufferDone;

	strcpy (m_szCoreFile, "");
	strcpy (m_szCompName, "");

	m_pMsgThread = new voCBaseThread ("vomp Message");
	if (m_pMsgThread != NULL)
		m_pMsgThread->Start (NULL);

	if (pUserData)
		m_pSource = (CFileSource*)pUserData->reserved1;

	if(m_pSource)
	{
		VO_SOURCE_INFO	filInfo;
		m_pSource->GetSourceInfo (&filInfo);
		VO_U32	i = 0;
		VO_SOURCE_TRACKINFO	trkInfo;
		for (i = 0; i < filInfo.Tracks; i++)
		{
			m_pSource->GetTrackInfo (i, &trkInfo);
			if (trkInfo.Type == VO_SOURCE_TT_AUDIO && m_nAudioTrack == -1)
				m_nAudioTrack = i;
			else if (trkInfo.Type == VO_SOURCE_TT_VIDEO && m_nVideoTrack == -1)
				m_nVideoTrack = i;
		}		
	}
}

CBaseComp::~CBaseComp()
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pComponent != NULL)
	{
		m_bSendMsg = OMX_TRUE;
		errType = SendCommand (OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if (m_pInputPort != NULL)
			m_pInputPort->FreeBuffer ();
		if (m_pOutputPort != NULL)
			m_pOutputPort->FreeBuffer ();
		if (WaitCommandDone () == VO_ERR_NONE)
			errType = GetState();
	}

	if (m_pInputPort != NULL)
		delete m_pInputPort;

	if (m_pOutputPort != NULL)
		delete m_pOutputPort;

	if (m_pHeadData != NULL)
		delete []m_pHeadData;

	if (m_pBufferList)
		delete m_pBufferList;
	m_pBufferList = NULL;

	if (m_pMsgThread != NULL)
	{
		m_pMsgThread->Stop ();
		delete m_pMsgThread;
	}

	VOLOGI ("Free Core %s, Comp %s", m_szCoreFile, m_szCompName);
}

OMX_ERRORTYPE CBaseComp::SetCoreFile (OMX_STRING pCoreFile)
{
	strcpy (m_szCoreFile, pCoreFile);

#ifdef _WIN32
	strcat (m_szCoreFile, ".dll");
#elif defined LINUX
	strcat (m_szCoreFile, ".so");
#endif // _WIN32

	VOLOGI ("File %s", m_szCoreFile);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::SetCompName (OMX_STRING pCompName)
{
	strcpy (m_szCompName, pCompName);

	if (!strcmp (m_szCompName, "OMX.qcom.video.decoder.avc"))
		m_nColorType = VO_COLOR_YUV_420_PACK_2;
	else if (!strcmp (m_szCompName, "OMX.qcom.video.decoder.mpeg4"))
		m_nColorType = VO_COLOR_YUV_420_PACK_2;
	else if (!strcmp (m_szCompName, "OMX.qcom.video.decoder.h263"))
		m_nColorType = VO_COLOR_YUV_420_PACK_2;
	else if (!strcmp (m_szCompName, "OMX.TI.DUCATI1.VIDEO.DECODER"))
		m_nColorType = VO_COLOR_YUV_420_PACK;
	else
		m_nColorType = VO_COLOR_YUV_PLANAR420;

	VOLOGI ("component: %s, colortype: %d", m_szCompName, m_nColorType);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::Start (void)
{
	voCOMXAutoLock lock (&m_muxStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pComponent == NULL)
		Load2Idle();

	Load2Executing();
	VOINFO("start over....");
	return errType;
}

OMX_ERRORTYPE CBaseComp::Pause (void)
{
	voCOMXAutoLock lock (&m_muxStatus);
	VOLOGI ("MAAA Pause");
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pComponent == NULL)
	{
		VOLOGE ("There was no component created!");
		return OMX_ErrorInvalidState;
	}

	OMX_STATETYPE	state;
	errType = GetState(&state);
	VOINFO("state: %d", state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StatePause)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StatePause, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateExecuting return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateExecuting return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();
	VOINFO("state: %d", m_sState);
	if (m_sState != OMX_StatePause)
	{
		VOLOGE ("The current state %d was not OMX_StateExecuting!", state);
		return OMX_ErrorInvalidState;
	}

	return errType;
}

OMX_ERRORTYPE CBaseComp::Stop (void)
{
	voCOMXAutoLock lock (&m_muxStatus);
	VOLOGI ("MAAA Stop");

	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	OMX_STATETYPE	state;
	errType = GetState(&state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateIdle && state != OMX_StatePause && state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StateIdle)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateIdle return error %08X!", errType);
		return errType;
	}

	if (m_pInputPort != NULL)
		m_pInputPort->Stop ();
	if (m_pOutputPort != NULL)
		m_pOutputPort->Stop ();

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateIdle return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();
	if (m_sState != OMX_StateIdle)
	{
		VOLOGE ("The current state %d was not OMX_StateIdle!", state);
		return OMX_ErrorInvalidState;
	}

	return errType;
}

OMX_ERRORTYPE CBaseComp::Flush (void)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;
	m_bSendMsg = OMX_TRUE;
	errType = SendCommand(OMX_CommandFlush, OMX_ALL, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandFlush return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandFlush return error!");
		return OMX_ErrorInvalidState;
	}

	//VOLOGI ("Input Full %d, Free %d, Output Full %d, Free %d", m_pInputPort->GetFullNum (), m_pInputPort->GetEmptyNum (), m_pOutputPort->GetFullNum (), m_pOutputPort->GetEmptyNum ());
	return errType;
}

int CBaseComp::onNotifyEvent (int nMsg, int nV1, int nV2)
{
	voCOMXAutoLock lock (&m_muxStatus);

	switch (nMsg)
	{
		case VOCOMP_EVENT_RESETPORT:
			{
				ResetPort(nV1);
			}
			break;

		case VOCOMP_EVENT_RESETCROP:
			{
				VOINFO("gonna reset crop............................");
				ResetCrop();
			}
			break;

		default:
			break;
	}

	return 0;
}

int CBaseComp::postCommandEvent(int nMsg, int nV1, int nV2, int nDelayTime)
{
	if (m_pMsgThread == NULL)
		return -1;

	voCBaseEventItem * pEvent = m_pMsgThread->GetEventByType ((int)(VOCOMP_EVENT_COMMAND));
	if (pEvent == NULL)
		pEvent = new CBaseCompEvent (this, &CBaseComp::onNotifyEvent, VOCOMP_EVENT_COMMAND, nMsg, nV1, nV2);
	else
		pEvent->SetEventMsg (nMsg, nV1, nV2);

	m_pMsgThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

VO_U32 CBaseComp::SetVideoInputData(VO_CODECBUFFER * pInput)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pInputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	//currently, if the first sample get from source is sequence header, should skip it;
	if(pInput->Length<128)
		return VO_ERR_NONE;
	
	if(m_bThumbnailMode)
		m_pInputBuffer = pInput;
	else
	{
		if(m_pBufferList == NULL)
			m_pBufferList = new CWrapBufferList();
		m_pBufferList->addBuffer(pInput);
	}	
	
	return VO_ERR_NONE;
}

VO_U32 CBaseComp::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pOutputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32	nRC = VO_ERR_NONE;


	if (!m_bHeadDone && m_pHeadData != NULL)
	{
		VO_CODECBUFFER headBuff;
		headBuff.Buffer = m_pHeadData;
		headBuff.Length = m_nHeadSize;
		headBuff.Time = 0;

		nRC = m_pInputPort->SetVideoInputData (&headBuff, NULL, 0, VO_TRUE);
		m_bHeadDone = VO_TRUE;
	}
	m_semBufferFilled.Reset();
	VO_CODECBUFFER* pInputBuffer = NULL;
	VO_BOOL bSetInput = VO_FALSE;

	if(m_bThumbnailMode)
		pInputBuffer = m_pInputBuffer;
	else
		pInputBuffer = m_pBufferList->getBuffer();

	if (m_pInputPort->GetEmptyNum () > 0 && pInputBuffer != NULL)
	{	
		nRC = m_pInputPort->SetVideoInputData (pInputBuffer, NULL, 0, VO_FALSE);
		if(nRC == VO_ERR_NONE)
		{
			pInputBuffer = NULL;
			if(m_bThumbnailMode)
				m_pInputBuffer = NULL;
			else
				m_pBufferList->removeBuffer();
			bSetInput = VO_TRUE;
		}		
	}
	m_pOutputPort->FillOutputBuffer();
	// for thumbnail, should wait for decoding finish
	if(m_bThumbnailMode&&bSetInput)
	{
		m_semBufferFilled.Down();
	}
	nRC = m_pOutputPort->GetVideoOutputData (pOutBuffer, pOutInfo);

	if (nRC == VO_ERR_RETRY)
	{
		if (pInputBuffer == NULL)
			return VO_ERR_INPUT_BUFFER_SMALL;

		pOutInfo->Flag = VO_VIDEO_OUTPUT_MORE;
	}

	if (pInputBuffer != NULL)
		pOutInfo->Flag = VO_VIDEO_OUTPUT_MORE;
	else
		pOutInfo->Flag = 0;
	
	return nRC;
}

VO_U32 CBaseComp::SetAudioInputData(VO_CODECBUFFER * pInput)
{
	if (m_pInputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	if(m_pBufferList == NULL)
		m_pBufferList = new CWrapBufferList();
	m_pBufferList->addBuffer(pInput);

	return VO_ERR_NONE;
}

VO_U32 CBaseComp::GetAudioOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pOutputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32	nRC = VO_ERR_NONE;
	VO_U32	nTryTimes = 0;

	VO_CODECBUFFER* pInputBuffer = m_pBufferList->getBuffer();

	while (VO_TRUE)
	{
		nRC = m_pOutputPort->GetAudioOutputData (pOutBuffer, pOutInfo);
		if (nRC == VO_ERR_NONE)
			break;

		if (pInputBuffer != NULL)
		{
			nRC = m_pInputPort->SetAudioInputData (pInputBuffer);

			if (nRC == VO_ERR_NONE)
			{
				m_pBufferList->removeBuffer();
				pInputBuffer = NULL;
			}
		}

		nRC = m_pOutputPort->GetAudioOutputData (pOutBuffer, pOutInfo);
		if (nRC == VO_ERR_NONE)
			break;

		if (pInputBuffer == NULL)
			break;

		voOMXOS_Sleep (2);
		nTryTimes++;
		if (nTryTimes >= 50)
			break;
	}

	return nRC;
}

VO_U32 CBaseComp::SetVideoRenderData(VO_VIDEO_BUFFER * pInput)
{
	if(m_pInputPort != NULL)
	{
		return m_pInputPort->SetVideoRenderInput(pInput);;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseComp::SetVideoRenderSurface(VO_PTR pSuf)
{
	 mpSurface = pSuf;

	return SetSurfaceToVideoRender();
}

#define NVX_INDEX_CONFIG_EXTERNALOVERLAY "OMX.Nvidia.index.config.externaloverlay"
/** Holds data to allow the client to provide its own overlay surface. */
typedef struct NVX_CONFIG_EXTERNALOVERLAY
{
	OMX_U32 nSize;                      /**< Size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;           /**< NVX extensions specification version information */
	OMX_U32 nPortIndex;                 /**< Port that this struct applies to */
	OMX_U64 oOverlayPtr;                /**< Pointer to application overlay surface */
} NVX_CONFIG_EXTERNALOVERLAY;

VO_U32	CBaseComp::SetSurfaceToVideoRender()
{
	if(m_pComponent == NULL || mpSurface == NULL)
		return OMX_ErrorNotImplemented;

	OMX_INDEXTYPE index;
	NVX_CONFIG_EXTERNALOVERLAY externalOverlay;
	OMX_ERRORTYPE err;

	err = m_pComponent->GetExtensionIndex(m_pComponent , (char*)NVX_INDEX_CONFIG_EXTERNALOVERLAY , &index);
	if (OMX_ErrorNone != err)
		return err;

	SetHeader(&externalOverlay , sizeof(NVX_CONFIG_EXTERNALOVERLAY));
	if(mpSurface == 0)
		externalOverlay.oOverlayPtr = 0;
	else
		externalOverlay.oOverlayPtr = (OMX_U64)mpSurface;

	err = m_pComponent->SetConfig(m_pComponent , index , &externalOverlay);
	return err;
}


OMX_ERRORTYPE CBaseComp::SetCodecCoding (VO_COMPWRAP_CODECTYPE nCodecType,VO_U32 nCoding)
{
	voCOMXAutoLock lock (&m_muxStatus);

	m_nCodecType = nCodecType;
	m_nCoding = nCoding;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::SetVideoSize (VO_U32 nWidth, VO_U32 nHeight)
{
	voCOMXAutoLock lock (&m_muxStatus);

	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	VOINFO("MAAA SetVideoSize = %d %d " , (int)m_nVideoWidth , (int)m_nVideoHeight);

	if (m_nVideoWidth == 0)
		m_nVideoWidth = 320;
	if (m_nVideoHeight == 0)
		m_nVideoHeight = 240;

	return OMX_ErrorNone;
}
OMX_ERRORTYPE CBaseComp::SetAudioFormat (VO_U32 nSampleRate, VO_U32 nChannels, VO_U32 nSampleBits)
{
	voCOMXAutoLock lock (&m_muxStatus);

	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	m_nSampleBits = nSampleBits;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pHeadData != NULL)
		delete []m_pHeadData;
	m_pHeadData = NULL;

	m_nHeadSize = nHeadSize;
	if (m_nHeadSize > 0)
	{
		m_pHeadData = new VO_BYTE[m_nHeadSize];
		memcpy (m_pHeadData, pHeadData, m_nHeadSize);
	}

	m_bHeadDone = VO_FALSE;
	return OMX_ErrorNone;	
}

OMX_ERRORTYPE CBaseComp::Load2Idle(void)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::Load2Executing(void)
{
	if (m_pComponent == NULL)
	{
		VOLOGE ("There was no component created!");
		return OMX_ErrorInvalidState;
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	OMX_STATETYPE	state;
	errType = GetState(&state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateIdle && state != OMX_StatePause && state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StateExecuting)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateExecuting return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateExecuting return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();

	if (m_sState != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was not OMX_StateExecuting!", state);
		return OMX_ErrorInvalidState;
	}

	if (m_pInputPort != NULL)
		m_pInputPort->StartBuffer ();
	if (m_pOutputPort != NULL)
		m_pOutputPort->StartBuffer ();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::SetHeader(OMX_PTR pHeader, OMX_U32 nSize)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (pHeader == NULL)
		return OMX_ErrorBadParameter;

	memset (pHeader, 0, nSize);
	*((OMX_U32*)pHeader) = nSize;
	memcpy ((OMX_S8 *)pHeader + 4, &m_verMain, sizeof (OMX_VERSIONTYPE));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::CreatePorts (void)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pInputPort != NULL)
		return OMX_ErrorNone;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_PORT_PARAM_TYPE	typePort;

	OMX_U32	nPorts = 0;

	SetHeader(&typePort, sizeof(OMX_PORT_PARAM_TYPE));
	errType = GetParameter (OMX_IndexParamAudioInit, &typePort);
	if (errType == OMX_ErrorNone)
		nPorts += typePort.nPorts;

	errType = GetParameter (OMX_IndexParamVideoInit, &typePort);
	if (errType == OMX_ErrorNone)
		nPorts += typePort.nPorts;

	if(nPorts == 0)
	{
		for(VO_U32 ii = 0 ; ii < 16 ; ii++)
		{
			OMX_PARAM_PORTDEFINITIONTYPE portDef;
			SetHeader(&portDef , sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			portDef.nPortIndex = ii;

			errType = GetParameter(OMX_IndexParamPortDefinition, &portDef);
			if(errType != OMX_ErrorNone)
			{
				break;
			}

			switch (portDef.eDomain)
			{
			case OMX_PortDomainAudio: 
				nPorts ++;
				break;
			case OMX_PortDomainVideo:
				nPorts ++;
				break;
			case OMX_PortDomainImage:
				nPorts ++;
				break;
			case OMX_PortDomainOther:
				nPorts ++;
				break;
			default:
				break;
			}
		}
	}

	OMX_PARAM_PORTDEFINITIONTYPE portType;
	SetHeader (&portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));

	for (OMX_U32 i = 0; i < nPorts; i++)
	{
		portType.nPortIndex = i;
		errType = GetParameter (OMX_IndexParamPortDefinition, &portType);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("It could not get param OMX_IndexParamPortDefinition. return error %08X!", errType);
			return errType;
		}

		if (portType.eDir == OMX_DirInput && m_pInputPort == NULL)
			m_pInputPort = new CBasePort (this, i);
		else if (portType.eDir == OMX_DirOutput && m_pOutputPort == NULL )
			m_pOutputPort = new CBasePort (this, i);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBaseComp::SendCommand (OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData)
{
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->SendCommand (m_pComponent, nCmd, nParam, pData);
	return errType;
}

OMX_ERRORTYPE CBaseComp::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size)
{
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->GetParameter (m_pComponent, nIndex, pParam);
	return errType;
}

OMX_ERRORTYPE CBaseComp::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size)
{
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->SetParameter (m_pComponent, nIndex, pParam);
	return errType;
}

OMX_ERRORTYPE CBaseComp::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size)
{
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->GetConfig (m_pComponent, nIndex, pConfig);
	return errType;
}

OMX_ERRORTYPE CBaseComp::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size)
{
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->SetConfig (m_pComponent, nIndex, pConfig);
	return errType;
}

VO_U32 CBaseComp::WaitCommandDone (void)
{
	VO_U32	nTryTimes = 0;

	while (m_bSendMsg)
	{
		voOMXOS_Sleep (2);

		nTryTimes++;
		if (nTryTimes > 2500)
			return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_NONE;
}

OMX_ERRORTYPE CBaseComp::SetComponentRole()
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(m_nCodecType == VO_COMPWRAP_CODEC_VideoDec)
	{
		char szVideoDecRole[128];
		
		if (m_nCoding == VO_VIDEO_CodingVP7)
			strcpy (szVideoDecRole, "video_decoder.vp7");
		else if (m_nCoding == VO_VIDEO_CodingMPEG4)
			strcpy (szVideoDecRole, "video_decoder.mpeg4");
		else if (m_nCoding == VO_VIDEO_CodingH264)
			strcpy (szVideoDecRole, "video_decoder.avc");
		else
		{
			VOLOGE("m_nCoding=0X%x", (unsigned int)m_nCoding);
			strcpy (szVideoDecRole, "video_decoder.unknown");
		}

		OMX_U8		szRole[OMX_MAX_STRINGNAME_SIZE];
		OMX_STRING	pRole = (OMX_STRING)szRole;
		OMX_U32		nIndex = 0;

		while (errType == OMX_ErrorNone && m_pComponent->ComponentRoleEnum != NULL)
		{
			memset (szRole, 0, OMX_MAX_STRINGNAME_SIZE);
			errType = m_pComponent->ComponentRoleEnum (m_pComponent, szRole, nIndex++);
			if (errType != OMX_ErrorNone)
				break;

			if(pRole == NULL|| strstr(pRole, "video_decoder") == NULL)
			{// some component may not implement this function and return NULL
				VOLOGI("ComponentRoleEnum not implemented");
				OMX_PARAM_COMPONENTROLETYPE rolType;

				SetHeader (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
				memcpy (rolType.cRole, szVideoDecRole, OMX_MAX_STRINGNAME_SIZE);
				errType= SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
				break;
			}
			else if (!strcmp (pRole, szVideoDecRole))
			{
				OMX_PARAM_COMPONENTROLETYPE rolType;

				SetHeader (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
				memcpy (rolType.cRole, szVideoDecRole, OMX_MAX_STRINGNAME_SIZE);
				errType= SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
				break;
			}			
		}
	}
	
	return errType;
}

OMX_ERRORTYPE	CBaseComp::GetState(OMX_STATETYPE *pst) {

	OMX_ERRORTYPE err = OMX_ErrorNone;
	if (pst != NULL)
		m_pComponent->GetState(m_pComponent, pst);
	else
		m_pComponent->GetState(m_pComponent, &m_sState);

	return err;
}

VO_U32 CBaseComp::GetSourceParam(VO_U32 paramID, VO_PTR pData)
{
	VO_U32 err= VO_ERR_NONE;
	switch(paramID)
	{
	case VO_PID_VIDEO_ParamVideoAvc:
		err = m_pSource->GetTrackParam(m_nVideoTrack, paramID, pData);
		break;
	default:
		err = VO_ERR_NOT_IMPLEMENT;
		break;
	}
	return err;
}

VO_U32 CBaseComp::GetDecParam(VO_U32 paramID, VO_PTR pData)
{
	if (pData == NULL) return VO_ERR_INVALID_ARG;

	VO_U32 err= VO_ERR_NONE;

	switch(paramID)
	{
	case VO_PID_VIDEO_DIMENSION: 
		{
			err = m_pOutputPort->GetVideoDecDimension(pData);
		}
		break;

	default:
		err = VO_ERR_NOT_IMPLEMENT;
		break;
	}

	return err;
}


OMX_ERRORTYPE CBaseComp::EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	CBasePort * pPort = (CBasePort *)pBuffer->pAppPrivate;

	return pPort->EmptyBufferDone (pBuffer);
}

OMX_ERRORTYPE CBaseComp::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	CBasePort * pPort = (CBasePort *)pBuffer->pAppPrivate;
	m_semBufferFilled.Up();
	return pPort->FillBufferDone (pBuffer);
}

OMX_ERRORTYPE CBaseComp::voOMXEventHandler (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_EVENTTYPE eEvent,
									OMX_IN OMX_U32 nData1,
									OMX_IN OMX_U32 nData2,
									OMX_IN OMX_PTR pEventData)
{
//	VOLOGI ("pAppData %d", (int)pAppData);

	CBaseComp * pBox = (CBaseComp *)pAppData;

	return  pBox->EventHandler (eEvent, nData1, nData2, pEventData);
}

OMX_ERRORTYPE CBaseComp::voOMXEmptyBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
//	VOLOGI ("pAppData %d", (int)pAppData);

	CBaseComp * pBox = (CBaseComp *)pAppData;

	return  pBox->EmptyBufferDone (pBuffer);
}

OMX_ERRORTYPE CBaseComp::voOMXFillBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
//	VOLOGI ("pAppData %d", (int)pAppData);

	CBaseComp * pBox = (CBaseComp *)pAppData;

	return  pBox->FillBufferDone (pBuffer);
}

OMX_ERRORTYPE CBaseComp::EventHandler (OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	switch (eEvent)
	{
	case OMX_EventCmdComplete:
		{
			m_bSendMsg = OMX_FALSE;

			switch (nData1)
			{
			case OMX_CommandStateSet:
				break;

			case OMX_CommandFlush:
				break;

			case OMX_CommandPortDisable:
				VOINFO("port disabled.....");
				break;

			case OMX_CommandPortEnable:
				VOINFO("port enabled.....");
				break;

			case OMX_CommandMarkBuffer:
				break;

			default:
				break;
			}
		}
		break;

	case OMX_EventError:
		m_bSendMsg = OMX_FALSE;
		break;

	case OMX_EventMark:
		break;

	case OMX_EventPortSettingsChanged:
		{
			VOINFO("OMX_EventPortSettingsChanged, Data1 %d, Data2 0x%08x", (int)nData1, (int)nData2);
			if (nData2 == 0 || nData2 == OMX_IndexParamPortDefinition) 
			{
				postCommandEvent(VOCOMP_EVENT_RESETPORT, nData1, nData2);
			}
			else if (nData1 == kPortIndexOutput && (nData2 == OMX_IndexConfigCommonOutputCrop || nData2 == OMX_IndexConfigCommonScale)) 
			{
				if (nData2 == OMX_IndexConfigCommonOutputCrop) {

					VOINFO("format has notably changed....");
					postCommandEvent(VOCOMP_EVENT_RESETCROP, nData1, nData2);

				} else if (nData2 == OMX_IndexConfigCommonScale) {
					VOINFO("common scale...");
				} 
			}
		}
		break;

	case OMX_EventBufferFlag:
		break;

	case OMX_EventResourcesAcquired:
		break;

	case OMX_EventComponentResumed:
		break;

	case OMX_EventDynamicResourcesAvailable:
		break;

	case OMX_EventPortFormatDetected:
		break;

	default:
		break;
	}

	return OMX_ErrorNone;
}
