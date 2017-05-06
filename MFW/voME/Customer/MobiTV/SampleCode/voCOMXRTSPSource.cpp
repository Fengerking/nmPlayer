	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPSource.cpp

	Contains:	voCOMXRTSPSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXRTSPSource.h"
#include "voCOMXRTSPAudioPort.h"
#include "voCOMXRTSPVideoPort.h"

#include "CRTSPSource.h"

#include "voLog.h"

typedef	VO_PTR (VO_API * VOFILESOURCEOPEN) (VO_FILE_SOURCE * pSource);
typedef	VO_S64 (VO_API * VOFILESOURCESEEK) (VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);

voCOMXRTSPSource::voCOMXRTSPSource(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pAudioPort (NULL)
	, m_pVideoPort (NULL)
	, m_pClockPort (NULL)
	, m_nURISize (0)
	, m_pURI (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_bForceClose (OMX_FALSE)
	, m_nSeekMode (OMX_TIME_SeekModeAccurate)
	, m_llStartPos (0)
	, m_pSource (NULL)
	, m_nAudioTrack (-1)
	, m_nVideoTrack (-1)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.FileSource");

	VOLOGF ("Name %s.", m_pName);
}

voCOMXRTSPSource::~voCOMXRTSPSource()
{
	VOLOGF ("Name %s.", m_pName);

	m_bForceClose = OMX_TRUE;;

	CloseFile ();

	if (m_pURI != NULL)
		voOMXMemFree (m_pURI);
}

OMX_ERRORTYPE voCOMXRTSPSource::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name %s.The Index is 0X%08X", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nParamIndex)
	{
	case OMX_IndexParamContentURI:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)pComponentParameterStructure;
			if (pContent->nSize < m_nURISize)
				return OMX_ErrorBadParameter;

			voOMXMemCopy (pContent->contentURI, m_pURI, m_nURISize);
		}
		break;

	case OMX_IndexParamNumAvailableStreams:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			errType = ((voCOMXRTSPOutputPort *)m_ppPorts[pU32Type->nPortIndex])->GetStreams (&pU32Type->nU32);
		}
		break;

	case OMX_IndexParamActiveStream:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			errType = ((voCOMXRTSPOutputPort *)m_ppPorts[pU32Type->nPortIndex])->SetStream (pU32Type->nU32);
		}
		break;

	default:
		errType = voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXRTSPSource::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name %s.The index is %d", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamContentURI:
		{
			OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pContent, pContent->nSize);
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pURI != NULL)
				voOMXMemFree (m_pURI);

			m_nURISize = pContent->nSize - 8;
			m_pURI = (OMX_S8*)voOMXMemAlloc (m_nURISize);
			if (m_pURI == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pURI, pContent->contentURI, m_nURISize);

/*
			COMP_MESSAGE_TASK * pTask = NULL;
			pTask = (COMP_MESSAGE_TASK *) voOMXMemAlloc (sizeof (COMP_MESSAGE_TASK));
			if (pTask == NULL)
				return OMX_ErrorInsufficientResources;

			pTask->uType = TASK_LOAD_FILE;
			pTask->uParam = 0;
			pTask->pData = 0;

			m_tqMessage.Add (pTask);
			m_tsMessage.Up ();
*/
			errType = LoadFile ((OMX_PTR) m_pURI, 1);
		}
		break;

	case OMX_VO_IndexSourceHandle:
			errType = LoadFile ((OMX_PTR) pComponentParameterStructure, 2);
		break;

	case OMX_VO_IndexSourceID:
		errType = LoadFile ((OMX_PTR) pComponentParameterStructure, 3);
		break;

	case OMX_VO_IndexSourceOffset:
		m_nOffset = *(OMX_S64 *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourceLength:
		m_nLength = *(OMX_S64 *)pComponentParameterStructure;
		break;

	default:
		errType = voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXRTSPSource::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name %s. The Index is %d", m_pName, nIndex);

	switch (nIndex)
	{
	case OMX_IndexConfigTimeSeekMode:
		{
			OMX_TIME_CONFIG_SEEKMODETYPE * pSeekMode = (OMX_TIME_CONFIG_SEEKMODETYPE *)pComponentConfigStructure;

			pSeekMode->eType = m_nSeekMode;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSource::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE voCOMXRTSPSource::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name %s. The Index is %d", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nIndex)
	{
	case OMX_IndexConfigTimeSeekMode:
		{
			OMX_TIME_CONFIG_SEEKMODETYPE * pSeekMode = (OMX_TIME_CONFIG_SEEKMODETYPE *)pComponentConfigStructure;

			m_nSeekMode = pSeekMode->eType;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimePosition:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			if (pTime->nPortIndex != 0)
				return OMX_ErrorNone;

			if (m_sState <= OMX_StateIdle)
				m_llStartPos = pTime->nTimestamp;
	
			VO_S64 nNewPos = pTime->nTimestamp;
			VOLOGI ("Before Set Pos. The pos is %d", (int)nNewPos);
			if (m_nVideoTrack >= 0 && m_pVideoPort->IsEnable ())
			{
				errType = m_pVideoPort->SetTrackPos (&nNewPos);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("m_pVideoPort->SetTrackPos wass failed. 0X%08X", errType);

					if (m_pClockPort != NULL)
						m_pClockPort->UpdateMediaTime (pTime->nTimestamp);

					return errType;
				}
			}

			if (m_nAudioTrack >= 0 && m_pAudioPort->IsEnable ())
			{
				errType = m_pAudioPort->SetTrackPos (&nNewPos);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("m_pAudioPort->SetTrackPos wass failed. 0X%08X", errType);

					if (m_pClockPort != NULL)
						m_pClockPort->UpdateMediaTime (pTime->nTimestamp);

					return errType;
				}
			}

			if (m_pClockPort != NULL)
				m_pClockPort->UpdateMediaTime (nNewPos);

			VOLOGI ("After Set Pos. The pos is %d", (int)nNewPos);

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXRTSPSource::SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_COMMANDTYPE Cmd,
											OMX_IN  OMX_U32 nParam1,
											OMX_IN  OMX_PTR pCmdData)
{
	VOLOGF ("Name: %s", m_pName);

	if (Cmd == OMX_CommandStateSet)
	{
		if (nParam1 == OMX_StateLoaded && m_sState == OMX_StateLoaded)
		{
			if (m_pSource != NULL)
			{
				ForceCloseSource ();
			}
		}
	}

	return voCOMXCompSource::SendCommand (hComponent, Cmd, nParam1, pCmdData);
}

OMX_ERRORTYPE voCOMXRTSPSource::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name %s.", m_pName);

	if (m_pSource != NULL)
	{
		if (newState == OMX_StateExecuting && (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute))
		{
			m_llStartPos = 0;
			m_pSource->Start ();
		}
		else if (newState == OMX_StatePause && (m_sTrans == COMP_TRANSSTATE_ExecuteToPause))
		{
			m_pSource->Pause ();
		}
		else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		{
			m_pSource->Stop ();
		}
		
		OMX_ERRORTYPE errType = OMX_ErrorNone;

		OMX_S64 nNewPos = 0;
		if (m_pVideoPort != NULL)
			errType = m_pVideoPort->SetTrackPos (&nNewPos);

		nNewPos = 0;
		if (m_pAudioPort != NULL)
			errType = m_pAudioPort->SetTrackPos (&nNewPos);
	}

	return voCOMXCompSource::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXRTSPSource::LoadFile (OMX_PTR pSource, OMX_U32 nType)
{
	VOLOGF ("Name %s.", m_pName);

	CloseFile ();

	voCOMXAutoLock lock (&m_tmLoadSource);

	m_bForceClose = OMX_FALSE;

	VO_U32 uFileFlag = VO_FILE_TYPE_NAME;
	if ((nType & 0X0F) == 1)
		uFileFlag = VO_FILE_TYPE_NAME;
	else if ((nType & 0X0F) == 2)
		uFileFlag = VO_FILE_TYPE_HANDLE;
	else
		uFileFlag = VO_FILE_TYPE_ID;

	m_pSource = new CRTSPSource (NULL, 0, NULL, NULL);
	if (m_pSource == NULL)
		return OMX_ErrorInsufficientResources;

	m_pSource->SetCallBack ((OMX_PTR)voomxFileSourceStreamCallBack, (OMX_PTR)this);

	VO_U32 nRC = m_pSource->LoadSource (pSource, uFileFlag, m_nOffset, m_nLength);
	if (m_bForceClose)
		return OMX_ErrorUndefined;

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_pSource->LoadSource was failed. 0X%08X", nRC);
		return OMX_ErrorUndefined;
	}

	VO_SOURCE_INFO	filInfo;
	m_pSource->GetSourceInfo (&filInfo);
	if (filInfo.Tracks == 0)
	{
		VOLOGE ("m_pSource->GetSourceInfo was failed. 0X%08X", nRC);
		return OMX_ErrorUndefined;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1)
		{
			m_nAudioTrack = i;
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)  && m_nVideoTrack == -1)
		{
			VO_VIDEO_FORMAT fmtVideo;
			nRC = m_pSource->GetVideoFormat (i, &fmtVideo);
			if (nRC == VO_ERR_NONE && (fmtVideo.Width == 0 || fmtVideo.Height == 0))
			{
				VOLOGE ("m_pSource->GetVideoFormat was failed. 0X%08X", nRC);
				return OMX_ErrorUndefined;
			}

			m_nVideoTrack = i;
		}
	}

	if (m_nAudioTrack < 0 && m_nVideoTrack < 0)
	{
		VOLOGE ("There was not video or audio data in file.");
		return OMX_ErrorUndefined;
	}

	if (m_nAudioTrack >= 0 && m_nVideoTrack >= 0)
		m_uPorts = 3;
	else
		m_uPorts = 2;
	//m_ppPorts = new voCOMXBasePort*[m_uPorts];
	m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
	if (m_ppPorts == NULL)
		return OMX_ErrorInsufficientResources;

	OMX_S32 nIndex = 0;
	if (m_nAudioTrack >= 0)
	{
		m_ppPorts[nIndex] = new voCOMXRTSPAudioPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pAudioPort = (voCOMXRTSPAudioPort *) m_ppPorts[nIndex];
		m_pAudioPort->SetTrack (m_pSource, m_nAudioTrack);

		m_portParam[OMX_PortDomainAudio].nPorts = 1;
		m_portParam[OMX_PortDomainAudio].nStartPortNumber = nIndex;
		nIndex++;
	}

	if (m_nVideoTrack >= 0)
	{
		m_ppPorts[nIndex] = new voCOMXRTSPVideoPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pVideoPort = (voCOMXRTSPVideoPort *) m_ppPorts[nIndex];
		m_pVideoPort->SetTrack (m_pSource, m_nVideoTrack);

		m_portParam[OMX_PortDomainVideo].nPorts = 1;
		m_portParam[OMX_PortDomainVideo].nStartPortNumber = nIndex;
		nIndex++;
	}

	m_ppPorts[nIndex] = new voCOMXPortClock (this, nIndex, OMX_DirInput);
	m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
	m_pClockPort = (voCOMXPortClock *) m_ppPorts[nIndex];
	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = nIndex;

	if (m_pVideoPort != NULL)
	{
		m_pVideoPort->SetClockPort (m_pClockPort);
		m_pVideoPort->SetLoadMutex (&m_tmLoadSource);
	}

	if (m_nAudioTrack >= 0)
		m_pClockPort->SetClockType(OMX_TIME_RefClockAudio);

	if (m_pAudioPort != NULL)
	{
		m_pAudioPort->SetVideoPort (m_pVideoPort);
		m_pAudioPort->SetLoadMutex (&m_tmLoadSource);
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXRTSPSource::CloseFile (void)
{
	VOLOGF ("Name %s.", m_pName);

	if (m_pSource != NULL)
	{
		VOLOGI ();
		m_pSource->CloseSource ();

		voCOMXAutoLock lock (&m_tmLoadSource);
		if (m_pAudioPort != NULL)
			m_pAudioPort->SetTrack (NULL, -1);
		if (m_pVideoPort != NULL)
			m_pVideoPort->SetTrack (NULL, -1);


		delete m_pSource;
		m_pSource = NULL;

		m_nAudioTrack = -1;
		m_nVideoTrack = -1;
	}

	voCOMXAutoLock lock (&m_tmLoadSource);

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

OMX_ERRORTYPE voCOMXRTSPSource::ForceCloseSource (void)
{
	VOLOGF ("Name %s.", m_pName);

	if (m_pSource != NULL)
	{
		VOLOGI ();

		m_pSource->CloseSource ();

		voCOMXAutoLock lock (&m_tmLoadSource);
		if (m_pAudioPort != NULL)
			m_pAudioPort->SetTrack (NULL, -1);
		if (m_pVideoPort != NULL)
			m_pVideoPort->SetTrack (NULL, -1);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPSource::FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGR ("Name %s.", m_pName);

	return voCOMXCompSource::FillThisBuffer (hComponent, pBuffer);
}

int voCOMXRTSPSource::voomxFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	voCOMXRTSPSource * pSource = (voCOMXRTSPSource *)userData;

	VOLOGF ("Name %s.", pSource->m_pName);

	if (EventCode == VO_STREAM_BUFFERSTART)
	{
		if (pSource->m_pClockPort != NULL)
			pSource->m_pClockPort->StartWallClock (0);

		if (pSource->m_pAudioPort != NULL)
			pSource->m_pAudioPort->SetBuffering (OMX_TRUE);
		if (pSource->m_pVideoPort != NULL)
			pSource->m_pVideoPort->SetBuffering (OMX_TRUE);

		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_BufferStart, 0, 0, NULL);
	}
	else if (EventCode == VO_STREAM_BUFFERSTATUS)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_Bufferstatus, *EventParam1, 0, NULL);
	}
	else if (EventCode == VO_STREAM_BUFFERSTOP)
	{
//		if (pSource->m_pClockPort != NULL)
//			pSource->m_pClockPort->StartWallClock (1);

		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_BufferStop, 100, 0, NULL);
	}
	else if (EventCode == VO_STREAM_DOWNLODPOS)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_DownloadPos, *EventParam1, 0, NULL);
	}
	else if (EventCode == VO_STREAM_ERROR)
	{
		VOLOGI ("Stream Error: %d", EventCode);
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_StreamError, EventCode, 0, NULL);
	}

	return 0;
}
