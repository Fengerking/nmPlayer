	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaSource.cpp

	Contains:	voCOMXMediaSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXMediaSource.h"
#include "voIMediaSource.h"
#include "voCOMXMediaAudioPort.h"
#include "voCOMXMediaVideoPort.h"
#include "voCOMXPortClock.h"

#include "cmnFile.h"
#include "cmnMemory.h"

#include "voLog.h"

//typedef	VO_PTR (VO_API * VOFILESOURCEOPEN) (VO_FILE_SOURCE * pSource);
//typedef	VO_S64 (VO_API * VOFILESOURCESEEK) (VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);

#define MAX_PORT_NUM    (32)

voCOMXMediaSource::voCOMXMediaSource(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pIMediaSource (NULL)
	, m_pAudioPort (NULL)
	, m_pVideoPort (NULL)
	, m_pClockPort (NULL)
	, m_pPipe (NULL)
	//, m_nOffset (0)
	//, m_nLength (0)
	, m_pFileOP (NULL)
	, m_nAudioTrack (-1)
	, m_nVideoTrack (-1)
	, m_bBufferThread (OMX_TRUE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.MediaSource");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_pCfgSource = new CBaseConfig ();
	m_pCfgSource->Open (_T("vomeplay.cfg"));

	if (m_pCfgSource->GetItemValue (m_pName, (char*)"BufferThread", 1) == 0)
		m_bBufferThread = OMX_FALSE;

	m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc(MAX_PORT_NUM * sizeof(voCOMXBasePort *));
	voOMXMemSet(m_ppPorts, 0, (MAX_PORT_NUM * sizeof(voCOMXBasePort *)));

	VOLOGF ("Name %s.", m_pName);
}

voCOMXMediaSource::~voCOMXMediaSource()
{
	VOLOGF ("Name %s.", m_pName);

	CloseSource();

	if (m_pIMediaSource != NULL)
	{
		voOMXMemFree (m_pIMediaSource);
		m_pIMediaSource = NULL;
	}

	delete m_pCfgSource;

//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXMediaSource::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name %s.The Index is 0X%08X", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nParamIndex)
	{
	case OMX_IndexParamNumAvailableStreams:
		{
			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			errType = ((voCOMXMediaOutputPort *)m_ppPorts[pU32Type->nPortIndex])->GetStreams (&pU32Type->nU32);
		}
		break;

	case OMX_IndexParamActiveStream:
		{
			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			errType = ((voCOMXMediaOutputPort *)m_ppPorts[pU32Type->nPortIndex])->SetStream (pU32Type->nU32);
		}
		break;

	case OMX_VO_IndexMediaSourceInterface:
		{
			if(m_pIMediaSource == NULL)
			{
				m_pIMediaSource = (VOOMX_IMEDIASOURCE *)voOMXMemAlloc(sizeof(VOOMX_IMEDIASOURCE));
				if(m_pIMediaSource == NULL)
					return OMX_ErrorInsufficientResources;

				m_pIMediaSource->NewTrack = ::APINewTrack;
				m_pIMediaSource->SendData = ::APISendData;
			}

			VOOMX_IMEDIASOURCE ** ppIMediaSource = (VOOMX_IMEDIASOURCE **)pComponentParameterStructure;
			*ppIMediaSource = m_pIMediaSource;
			return OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXMediaSource::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name %s.The index is %d", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_VO_IndexSourcePipe:
		m_pPipe = (OMX_VO_FILE_OPERATOR *)pComponentParameterStructure;
		break;

	default:
		errType = voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXMediaSource::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name %s. The Index is %d", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	//switch (nIndex)
	//{

	//default:
	//	break;
	//}

	return voCOMXCompSource::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXMediaSource::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name %s. The Index is %d", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	//switch (nIndex)
	//{

	//default:
	//	break;
	//}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXMediaSource::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name %s.", m_pName);

	return voCOMXCompSource::SetNewState (newState);
}

OMX_U32	voCOMXMediaSource::MessageHandle (COMP_MESSAGE_TYPE * pTask)
{
	if (pTask == NULL)
		return 0;

	return voCOMXCompSource::MessageHandle (pTask);
}

OMX_ERRORTYPE voCOMXMediaSource::CloseSource (void)
{
	VOLOGF ("Name %s.", m_pName);

	m_nAudioTrack = -1;
	m_nVideoTrack = -1;

	for (OMX_U32 i = 0; i < m_uPorts; i++)
	{
		if (m_ppPorts[i] != NULL)
		{
			m_ppPorts[i]->ResetBuffer (OMX_TRUE);
		}
	}

	ReleasePort ();

	return OMX_ErrorNone;
}

OMX_U32 voCOMXMediaSource::DumpPerformanceInfo (OMX_STRING pFile)
{
	if (m_pAudioPort != NULL)
		m_pAudioPort->DumpLog ();
	if (m_pVideoPort != NULL)
		m_pVideoPort->DumpLog ();

	return 0;
}

OMX_ERRORTYPE voCOMXMediaSource::FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGR ("Name %s.", m_pName);

	if (!m_bBufferThread)
	{
		voCOMXAutoLock lokc (&m_tmStatus);

		if (pBuffer == NULL)
			return OMX_ErrorBadParameter;

		OMX_U32 nPortIndex = pBuffer->nOutputPortIndex;

		OMX_ERRORTYPE errType = OMX_ErrorNone;

		if (m_ppPorts[nPortIndex] == m_pAudioPort)
			errType = m_pAudioPort->FillBufferOne(pBuffer);
		else if (m_ppPorts[nPortIndex] == m_pVideoPort)
			errType = m_pVideoPort->FillBufferOne(pBuffer);

		return errType;
	}

	return voCOMXCompSource::FillThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXMediaSource::NewTrack (OMX_S32 nIndex, VO_SOURCE_TRACKINFO *pTrackInfo)
{
	VOLOGF ("Name %s.", m_pName);

	if(m_pClockPort == NULL) //clock port
	{
		m_pClockPort = new voCOMXPortClock (this, m_uPorts, OMX_DirInput);
		m_pClockPort->SetCallbacks (m_pCallBack, m_pAppData);

		m_portParam[OMX_PortDomainOther].nPorts = 1;
		m_portParam[OMX_PortDomainOther].nStartPortNumber = m_uPorts;

		m_ppPorts[m_uPorts] = m_pClockPort;
		++m_uPorts;
	}

	if(pTrackInfo->Type == VO_SOURCE_TT_AUDIO && m_nAudioTrack == -1)
	{
		m_pAudioPort = new voCOMXMediaAudioPort (this, m_uPorts);
		m_pAudioPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pAudioPort->SetSourceConfig (m_pCfgSource);
		m_pAudioPort->SetTrackInfo (nIndex, pTrackInfo);
	
		m_portParam[OMX_PortDomainAudio].nPorts = 1;
		m_portParam[OMX_PortDomainAudio].nStartPortNumber = m_uPorts;

		m_ppPorts[m_uPorts] = m_pAudioPort;
		m_nAudioTrack = nIndex;
		++m_uPorts;
	}
	else if(pTrackInfo->Type == VO_SOURCE_TT_VIDEO && m_nVideoTrack == -1)
	{
		m_pVideoPort = new voCOMXMediaVideoPort (this, m_uPorts);
		m_pVideoPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pVideoPort->SetSourceConfig (m_pCfgSource);
		m_pVideoPort->SetTrackInfo (nIndex, pTrackInfo);

		m_pVideoPort->SetClockPort (m_pClockPort);

		m_portParam[OMX_PortDomainVideo].nPorts = 1;
		m_portParam[OMX_PortDomainVideo].nStartPortNumber = m_uPorts;

		m_ppPorts[m_uPorts] = m_pVideoPort;
		m_nVideoTrack = nIndex;
		++m_uPorts;
	}
	else if(pTrackInfo->Type == VO_SOURCE_TT_RICHMEDIA)
	{
		++m_uPorts;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXMediaSource::SendData (OMX_S32 nIndex, VO_SOURCE_SAMPLE *pSample)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXMediaOutputPort *pMediaOutputPort = NULL;
	for(int i=1; i<m_uPorts; ++i) //skip m_ppPorts[0] clock port
	{
		pMediaOutputPort = (voCOMXMediaOutputPort *)m_ppPorts[i];	
		if(nIndex == pMediaOutputPort->GetTrackIndex())
		{
			pMediaOutputPort->SendSample(pSample);
		}
	}

	return errType;
}
