	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIComp.cpp

	Contains:	CTIComp class file

	Written by:	David1
	Time: 2011/11/29

*******************************************************************************/

#include <string.h>
#include <voOMX_Index.h>

#include "voOMXOSFun.h"
#include "CTIComp.h"
#include "CTIPort.h"
#include "CFileSource.h"

#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CTIComp"
#endif
#include "voLog.h"

//#pragma warning (disable : 4996)

CTIComp::CTIComp(VO_CODEC_INIT_USERDATA * pUserData)
	: CBaseComp(pUserData)
	, m_pListener(NULL)
{
	if (pUserData) 
		m_pListener = (VOMP_LISTENERINFO*)pUserData->reserved2;
}

CTIComp::~CTIComp()
{}

OMX_ERRORTYPE CTIComp::SetCoreFile (OMX_STRING pCoreFile)
{
	strcpy (m_szCoreFile, "libOMX_Core.so");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE CTIComp::SetCompName (OMX_STRING pCompName)
{
	strcpy(m_szCompName, "OMX.TI.DUCATI1.VIDEO.DECODER");
	m_nColorType = VO_COLOR_YUV_PLANAR420_NV12;   //
	return OMX_ErrorNone;
}

OMX_ERRORTYPE CTIComp::EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	return m_pInputPort->EmptyBufferDone (pBuffer);
}

OMX_ERRORTYPE CTIComp::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	m_semBufferFilled.Up();
	return m_pOutputPort->FillBufferDone (pBuffer);
}

OMX_ERRORTYPE CTIComp::ResetPort (int nIndex)
{
	VOLOGI ("The port is %d, m_pOutputPort %08X", nIndex, (int)m_pOutputPort);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (nIndex != 1 || m_pOutputPort == NULL)
		return errType;

	m_bSendMsg = OMX_TRUE;
	((CTIPort*)m_pOutputPort)->ResetCrop();

	errType = SendCommand (OMX_CommandPortDisable, nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE ("SendCommand OMX_CommandPortDisable failed 0X%08X.", errType);

	VOLOGI ("Free buffer");

	errType = m_pOutputPort->FreeBuffer ();
	if (errType != OMX_ErrorNone)
		VOLOGE ("FreeBuffer failed 0X%08X.", errType);

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandPortDisable return error!");
		return OMX_ErrorInvalidState;
	}


	m_bSendMsg = OMX_TRUE;

	VOLOGI ("Enable port");

	errType = SendCommand (OMX_CommandPortEnable, nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE ("SendCommand OMX_CommandPortEnable failed 0X%08X.", errType);

	VOLOGI ("Alloc buffer");

	errType = m_pOutputPort->AllocBuffer ();
	if (errType != OMX_ErrorNone)
		VOLOGE ("AllocBuffer failed 0X%08X.", errType);

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandPortEnable return error!");
		return OMX_ErrorInvalidState;
	}

	errType = m_pOutputPort->StartBuffer ();
	if (errType != OMX_ErrorNone)
		VOLOGE ("StartBuffer failed 0X%08X.", errType);

	m_pOutputPort->GetVideoSize (m_nVideoWidth, m_nVideoHeight);
	m_bResetOutputPort = OMX_TRUE;

	VOLOGI ("Reset port done %08X", errType);
	return errType;
}


VO_U32 CTIComp::SetVideoInputData(VO_CODECBUFFER * pInput)
{
	voCOMXAutoLock lock (&m_muxStatus);

	if (m_pInputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	//currently, if the first sample get from source is sequence header, should skip it;
	if(pInput->Length<128)
		return VO_ERR_NONE;
	
	if(m_pBufferList == NULL)
		m_pBufferList = new CWrapBufferList();

	m_pBufferList->addBuffer(pInput);

	return VO_ERR_NONE;
}

VO_U32 CTIComp::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
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

	VO_CODECBUFFER* pInputBuffer = NULL;
	pInputBuffer = m_pBufferList->getBuffer();

	if (m_pInputPort->GetEmptyNum() > 0 && pInputBuffer != NULL)
	{	
		nRC = m_pInputPort->SetVideoInputData (pInputBuffer, NULL, 0, VO_FALSE);
		if(nRC == VO_ERR_NONE)
		{
			pInputBuffer = NULL;
			m_pBufferList->removeBuffer();
		}		
	}

	m_pOutputPort->FillOutputBuffer();
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

OMX_ERRORTYPE CTIComp::CreatePorts (void)
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
			m_pInputPort = new CTIPort (this, i);
		else if (portType.eDir == OMX_DirOutput && m_pOutputPort == NULL )
			m_pOutputPort = new CTIPort (this, i);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CTIComp::CreateComponent(void)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;

	if(m_pComponent!= NULL)
		return OMX_ErrorNone;

	if (m_pCore != NULL)
		delete m_pCore;
	m_pCore = NULL;

	m_pCore = new COMXLoader ();
	m_pComponent = m_pCore->CreateComponent (m_szCoreFile, m_szCompName, this, &m_CallBack);

	if (m_pComponent == NULL)
	{
		VOLOGE ("It could not create the component!");
		return OMX_ErrorInvalidComponentName;
	}

	errType = m_pComponent->GetComponentVersion (m_pComponent, (OMX_STRING)m_szName, &m_verMain, &m_verSpec, &m_uuidType);	
	errType = m_pComponent->GetState (m_pComponent, &m_sState);

	errType = SetComponentRole();
	if(errType != OMX_ErrorNone)
	{
		VOLOGE("Set Component Role fails: 0X%8X", errType);
		return errType;
	}

	errType = CreatePorts ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not create the ports! Return %08X", errType);
		return errType;
	}

	if (m_pInputPort != NULL)
	{
		if (m_nCodecType == VO_COMPWRAP_CODEC_VideoDec)
		{
			errType = m_pInputPort->SetVideoInfo ((VO_VIDEO_CODINGTYPE)m_nCoding, m_nColorType, m_nVideoWidth, m_nVideoHeight);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not set input video info! Return %08X", errType);
				return errType;
			}
		}
		else if (m_nCodecType == VO_COMPWRAP_CODEC_AudioDec)
		{
			errType = m_pInputPort->SetAudioInfo ((VO_AUDIO_CODINGTYPE)m_nCoding, m_nSampleRate, m_nChannels, m_nSampleBits);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not set intput audio info! Return %08X", errType);
				return errType;
			}
		}
		else if(m_nCodecType == VO_COMPWRAP_RENDER_VIDEO)
		{
			VOLOGI("MAAA Input Port set video info : %d  %d " , (int)m_nVideoWidth , (int)m_nVideoHeight);
			errType = m_pInputPort->SetVideoInfo(VO_VIDEO_CodingUnused , m_nColorType , m_nVideoWidth , m_nVideoHeight);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not set input video info! Return %08X", errType);
				return errType;
			}
		}

	}

	if (m_pOutputPort != NULL)
	{
		if (m_nCodecType == VO_COMPWRAP_CODEC_VideoDec)
		{
			errType = m_pOutputPort->SetVideoInfo ((VO_VIDEO_CODINGTYPE)m_nCoding, m_nColorType, m_nVideoWidth, m_nVideoHeight);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not set output video info! Return %08X", errType);
				return errType;
			}
		}
		else if (m_nCodecType == VO_COMPWRAP_CODEC_AudioDec)
		{
			errType = m_pOutputPort->SetAudioInfo ((VO_AUDIO_CODINGTYPE)m_nCoding, m_nSampleRate, m_nChannels, m_nSampleBits);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("It could not set output audio info! Return %08X", errType);
				return errType;
			}
		}
	}

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_StateIdle return error %08X!", errType);
		return errType;
	}

	if (m_pInputPort != NULL)
		m_pInputPort->AllocBuffer();

	if (m_pOutputPort != NULL)
		m_pOutputPort->AllocBuffer();

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_StateIdle return error!");
		return OMX_ErrorInvalidState;
	}

	errType = m_pComponent->GetState (m_pComponent, &m_sState);
	if (m_sState != OMX_StateIdle)
	{
		VOLOGE ("The current state %d was not OMX_StateIdle!", m_sState);
		return OMX_ErrorInvalidState;
	}

	return errType;
}

OMX_ERRORTYPE CTIComp::Flush (void) {
	voCOMXAutoLock lock (&m_muxStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

#if 1
	if (m_pComponent == NULL)
		return OMX_ErrorInvalidState;

	m_bSendMsg = OMX_TRUE;

	errType = SendCommand(OMX_CommandFlush, OMX_ALL, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandFlush return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone() != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandFlush return error!");
		return OMX_ErrorInvalidState;
	}

	VOLOGI ("Input Full %d, Free %d, Output Full %d, Free %d", (int)m_pInputPort->GetFullNum (), (int)m_pInputPort->GetEmptyNum (), (int)m_pOutputPort->GetFullNum (), (int)m_pOutputPort->GetEmptyNum ());

	((CTIPort*)m_pOutputPort)->ReturnAllBuffers();
#endif

	return errType;
}

