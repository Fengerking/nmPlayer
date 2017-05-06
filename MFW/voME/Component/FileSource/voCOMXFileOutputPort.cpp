	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileOutputPort.cpp

	Contains:	voCOMXFileOutputPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"
#include "voOMXFile.h"
#include "voCOMXBaseComponent.h"
#include "voCOMXFileOutputPort.h"

#define LOG_TAG "voCOMXFileOutputPort"
#include "voLog.h"

voCOMXFileOutputPort::voCOMXFileOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXPortSource (pParent, nIndex)
	, m_pSource (NULL)
	, m_pLoadMutex (NULL)
	, m_pVideoPort (NULL)
	, m_nTrackIndex (-1)
	, m_nFourCC (0)
	, m_nSeekMode (OMX_TIME_SeekModeAccurate)
	
	, m_nFirstPos (0)
	, m_nStartPos (0)
	, m_nCurrentPos (0)
	, m_bEOS (OMX_FALSE)
	, m_bDecOnly (OMX_FALSE)
	, m_bHadBuffering (OMX_FALSE)
	, m_nSeekPos (0)
	, m_pConfig (NULL)
	, m_hDumpFile (NULL)
	, m_nLogLevel (0)
	, m_nLogSysStartTime (0)
	, m_nLogSysCurrentTime (0)
	, m_nLogFrameIndex (0)
	, m_nLogFrameSize (0)
	, m_pLogMediaTime (NULL)
	, m_pLogSystemTime (NULL)
	, m_pLogFrameSize (NULL)
	, m_pLogFrameSpeed (NULL)
	, m_hDataSourceFile (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	voOMXMemSet (&m_Sample, 0, sizeof (VO_SOURCE_SAMPLE));
}

voCOMXFileOutputPort::~voCOMXFileOutputPort(void)
{
	if (m_hDumpFile != NULL)
		voOMXFileClose (m_hDumpFile);
	if (m_pLogMediaTime != NULL)
	{
		delete []m_pLogMediaTime;
		delete []m_pLogSystemTime;
		delete []m_pLogFrameSize;
		delete []m_pLogFrameSpeed;
	}
}

OMX_ERRORTYPE voCOMXFileOutputPort::SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex)
{
	m_pSource = pSource;
	m_nTrackIndex = nTrackIndex;

	if (pSource == NULL)
		return OMX_ErrorInvalidState;

	m_pSource->GetTrackInfo (m_nTrackIndex, &m_trkInfo);
	m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_CODECCC, &m_nFourCC);

	if (m_pConfig != NULL)
	{
		OMX_STRING	pFileName = NULL;
		OMX_U32		nDumpLog = 0;
		if (m_trkInfo.Type == VO_SOURCE_TT_VIDEO || m_trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)
		{
			pFileName = m_pConfig->GetItemText (m_pParent->GetName (), (char *)"DumpVideoData");
			nDumpLog = m_pConfig->GetItemValue (m_pParent->GetName (), (char *)"DumpVideoInfo", 0);
		}
		else if (m_trkInfo.Type == VO_SOURCE_TT_AUDIO || m_trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO)
		{
			pFileName = m_pConfig->GetItemText (m_pParent->GetName (), (char *)"DumpAudioData");
			nDumpLog = m_pConfig->GetItemValue (m_pParent->GetName (), (char *)"DumpAudioInfo", 0);
		}
		if (pFileName != NULL)
		{
#ifdef _WIN32
			TCHAR szFileName[256];
			memset (szFileName, 0, sizeof (szFileName));
			MultiByteToWideChar (CP_ACP, 0, pFileName, -1, szFileName, sizeof (szFileName));
			m_hDumpFile = voOMXFileOpen ((OMX_STRING)szFileName, VOOMX_FILE_READ_WRITE);
#else
			m_hDumpFile = voOMXFileOpen (pFileName, VOOMX_FILE_READ_WRITE);
#endif // _WIN32
		}

		if (nDumpLog > 0 && m_pLogMediaTime == NULL)
		{
			m_nLogFrameSize = 10240;
			m_nLogFrameIndex = 0;
			m_pLogMediaTime = new OMX_U32[m_nLogFrameSize];
			m_pLogSystemTime = new OMX_U32[m_nLogFrameSize];
			m_pLogFrameSize = new OMX_U32[m_nLogFrameSize];
			m_pLogFrameSpeed = new OMX_U32[m_nLogFrameSize];
		}
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXFileOutputPort::GetStreams (OMX_U32 * pStreams)
{
	*pStreams = 1;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileOutputPort::SetStream (OMX_U32 nStream)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileOutputPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *) pParam;
			if (pExtData->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			if (m_trkInfo.HeadSize > pExtData->nDataSize)
				return OMX_ErrorBadParameter;

			if (m_trkInfo.HeadSize  == 0)
			{
				pExtData->eType = OMX_ExtraDataNone;
				pExtData->nDataSize = 0;
			}
			else
			{
				voOMXMemCopy (pExtData->data, m_trkInfo.HeadData, m_trkInfo.HeadSize);
				pExtData->nDataSize = m_trkInfo.HeadSize;
				pExtData->eType = OMX_ExtraDataQuantization;
			}

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetParameter (nIndex, pParam);
}


OMX_ERRORTYPE voCOMXFileOutputPort::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	switch (nIndex)
	{
	case OMX_VO_IndexConfigTimeDuration:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			pTime->nTimestamp = m_trkInfo.Duration;

			if(m_pSource->GetSourceType() == VO_SOURCE_TYPE_RTSP)
			{
				if(m_trkInfo.Duration == 0)
					pTime->nTimestamp = -1;
			}

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimePosition:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			pTime->nTimestamp = m_nCurrentPos - m_nFirstPos;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXFileOutputPort::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			VO_S64 nNewPos =  m_nStartPos + pTime->nTimestamp;
			m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);

			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexOutputConfigData:
		{
			OMX_VO_OUTPUTCONFIGDATATYPE * pOutputConfigData = (OMX_VO_OUTPUTCONFIGDATATYPE *)pConfig;
			if (pOutputConfigData->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorPortsNotCompatible;

			if (VO_ERR_NONE != m_pSource->SetOutputConfigData(m_nTrackIndex, (OMX_TRUE == pOutputConfigData->bOutputConfigData) ? VO_TRUE : VO_FALSE))
				return OMX_ErrorUndefined;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::SetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXFileOutputPort::SetTrackPos (OMX_S64 * pPos)
{
	if (m_pSource == NULL)
		return OMX_ErrorUndefined;

	VO_S64 nNewPos =  *pPos;
	if (m_nFirstPos != -1)
		 nNewPos =  m_nFirstPos + *pPos;

	VO_U32 nRC = 0;
	nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);
	if (nRC == VO_ERR_SOURCE_END)
	{
		m_Sample.Time = nNewPos;
		m_nStartPos = nNewPos;
		m_nCurrentPos = nNewPos - m_nFirstPos;

		//East 2009/12/25
		m_bEOS = OMX_FALSE;

		return OMX_ErrorNone;
	}
	else if (nRC != VO_ERR_NONE)
	{
		m_nSeekPos = *pPos;

		VO_S64 nStartPos = 0;
		nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nStartPos);
			VOLOGE ("m_pSource->SetTrackPos was failed. 0X%08X", (int)nRC);

		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("m_pSource->SetTrackPos was failed. 0X%08X", (int)nRC);
			return OMX_ErrorNotImplemented;
		}
	}

	m_Sample.Time = nNewPos;
	m_nStartPos = nNewPos;

	m_nCurrentPos = nNewPos - m_nFirstPos;

	*pPos = nNewPos - m_nFirstPos;
	if ((*pPos) < 0)
		*pPos = 0;

	m_bEOS = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileOutputPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	if (m_pConfig != NULL && m_pConfig->GetItemValue (m_pParent->GetName (), (char *)"BufferThread", 1) == 0)
	{
		if (m_tsState.Waiting ())
			m_tsState.Up ();

		if (!IsEnable ())
			return OMX_ErrorNone;

		OMX_ERRORTYPE	errType = OMX_ErrorNone;
		if (sNew == OMX_StateIdle && sTrans == COMP_TRANSSTATE_LoadedToIdle)
		{
			if (IsTunnel () && IsSupplier ())
			{
				errType = AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("AllocTunnelBuffer was failed. 0X%08X", errType);
					return errType;
				}
			}
			else
			{
				m_tsAlloc.Down ();
			}
			return errType;
		}
		else if (sNew == OMX_StateExecuting && (sTrans == COMP_TRANSSTATE_IdleToExecute || sTrans == COMP_TRANSSTATE_PauseToExecute))
		{
			if (IsTunnel () && IsSupplier ())
			{
				OMX_STATETYPE	nState = OMX_StateLoaded;
				OMX_U32			nTryTimes = 0;
				m_hTunnelComp->GetState (m_hTunnelComp, &nState);

				while (nState != OMX_StateExecuting && nState != OMX_StatePause)
				{
					voOMXOS_Sleep (5);
					m_hTunnelComp->GetState (m_hTunnelComp, &nState);

					nTryTimes++;
					if (nTryTimes > 100)
					{
						VOLOGE ("Time out when switch to run status");
						break;
					}
				}

				for (OMX_U32 i = 0; i < m_tqBuffer.Count (); i++)
					FillBufferOne ((OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ());
			}

			return OMX_ErrorNone;
		}
	}

	return voCOMXPortSource::SetNewCompState (sNew, sTrans);
}

OMX_ERRORTYPE voCOMXFileOutputPort::ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (m_bEOS)
	{
		if (IsTunnel () && IsSupplier ())
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}
	}

	if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		m_bEOS = OMX_TRUE;

	return voCOMXPortSource::ReturnBuffer (pBuffer);
}

OMX_ERRORTYPE voCOMXFileOutputPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	voCOMXAutoLock lock (m_pLoadMutex);
	if (m_pSource == NULL)
		return OMX_ErrorInvalidState;

	pBuffer->nFlags = 0;

	if (m_nLogFrameSize > 0 && m_pLogFrameSpeed != NULL)
	{
		if (m_nLogFrameIndex < m_nLogFrameSize)
		{
			m_pLogFrameSpeed[m_nLogFrameIndex] = voOMXOS_GetSysTime ();
		}
	}

	if (m_pSource->HasError () != 0)
	{
		// m_pCallBack->EventHandler (m_pParent->GetComponent (), m_pAppData, OMX_EventError, 0, 0, 0);
		return OMX_ErrorUndefined;
	}

	if(m_pParent->DumpRuntimeLog())
	{
		VOLOGI ("systime %d", (int)voOMXOS_GetSysTime());
	}
	VO_U32 nRC = m_pSource->GetTrackData (m_nTrackIndex, &m_Sample);
	if(m_pParent->DumpRuntimeLog())
	{
		VOLOGI ("nRC 0x%08X, sync frame 0x%08X, Size %d, Time %d, systime %d", (int)nRC, (int)(m_Sample.Size & 0x80000000), (int)(m_Sample.Size & 0x7fffffff), (int)m_Sample.Time, (int)voOMXOS_GetSysTime());
	}

	if (nRC == VO_ERR_NONE)
	{
	//	if (m_nFirstPos == -1)
	//		m_nFirstPos = m_Sample.Time;

		m_nCurrentPos = m_Sample.Time;

		pBuffer->nOffset = 0;
		pBuffer->nFilledLen = m_Sample.Size & 0X7FFFFFFF;

		if (m_sType.nBufferCountActual == 1)
			pBuffer->pBuffer = m_Sample.Buffer;
		else
			memcpy (pBuffer->pBuffer, m_Sample.Buffer, pBuffer->nFilledLen);
		pBuffer->nTimeStamp = m_Sample.Time;

		if (m_bHadBuffering)
		{
			m_bHadBuffering = OMX_FALSE;
			pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		}

		if (m_hDumpFile != NULL)
		{
			voOMXFileWrite (m_hDumpFile, pBuffer->pBuffer, pBuffer->nFilledLen);
//			voOMXFileFlush (0, m_hDumpFile);
		}

		if (m_nLogFrameSize > 0 && m_pLogMediaTime != NULL)
		{
			if (m_nLogFrameIndex < m_nLogFrameSize)
			{
				m_pLogMediaTime[m_nLogFrameIndex] = (OMX_U32)pBuffer->nTimeStamp;
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

		voOMXOS_Sleep (10);

		VOLOGI ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_END", m_pParent->GetName (), (int)m_sType.nPortIndex);
	}
	else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGR ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_NEEDRETRY", m_pParent->GetName (), m_sType.nPortIndex);
	}
	else
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGE ("Comp %s, Index %d.Get track data failed. 0X%08X", m_pParent->GetName (), (int)m_sType.nPortIndex, (int)nRC);
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}

void voCOMXFileOutputPort::SetSourceConfig (CBaseConfig * pConfig)
{
	m_pConfig = pConfig;

	if (m_pConfig != NULL)
		m_nLogLevel = m_pConfig->GetItemValue (m_pParent->GetName (), (char*)"ShowLogLevel", 0);
}

void voCOMXFileOutputPort::DumpLog (void)
{
	if (m_nLogFrameIndex == 0 || m_pLogMediaTime == NULL)
		return;
/*
	VO_PTR hFile = NULL;
#ifdef _WIN32
	TCHAR szFile[256];
#ifndef _WIN32_WCE
	_tcscpy (szFile, _T("C:\\"));
#else
	_tcscpy (szFile, _T("\\"));
#endif // _WIN32_WCE
	TCHAR szName[128];
	memset (szName, 0, 128);
	MultiByteToWideChar (CP_ACP, 0, m_pParent->GetName (), -1, szName,  128);
	_tcscat (szFile, szName);
#elif defined _LINUX
	char szFile[256];
	strcpy (szFile, _T("/data/local/"));
	strcat (szFile, m_pParent->GetName ());
#endif // _WIN32
	if (m_trkInfo.Type == VO_SOURCE_TT_VIDEO || m_trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)
		vostrcat (szFile, _T("_Video.txt"));
	else
		vostrcat (szFile, _T("_Audio.txt"));

	VO_FILE_SOURCE filSource;
	filSource.pSource = szFile;
	filSource.nFlag = VO_SOURCE_OPENPARAM_FLAG_SOURCENAME;

	hFile = cmnFileOpen (0, &filSource, VO_FILE_WRITE_ONLY);
	if (hFile == NULL)
		return;

	if (m_nLogFrameIndex > m_nLogFrameSize)
		m_nLogFrameIndex = m_nLogFrameSize;

	char szLine[128];

	strcpy (szLine, "Index	Sys	Step	Media	Step	Speed	Size	Key	\r\n");
	cmnFileWrite (0, hFile, (OMX_U8 *)szLine, strlen (szLine));

	OMX_U32 nSysTime  = 0;
	char	szKey[32];

	for (VO_U32 i = 0; i < m_nLogFrameIndex; i++)
	{
		if (m_pLogFrameSize[i] & 0X80000000)
			strcpy (szKey, "Key");
		else
			strcpy (szKey, "0");
		nSysTime = m_pLogSystemTime[i] - m_pLogSystemTime[0];

		if (i == 0)
			sprintf (szLine, "%d,	%d	%d	%d	%d	%d	%d	%s\r\n",
					 (int)i, (int)nSysTime, 0,
							  (int)m_pLogMediaTime[i], 0, (int)m_pLogFrameSpeed[i],
					 (int)(m_pLogFrameSize[i] & 0X7FFFFFFF), szKey);
		else
			sprintf (szLine, "%d,	%d	%d	%d	%d	%d	%d	%s\r\n",
					 (int)i, (int)nSysTime, (int)(m_pLogSystemTime[i] - m_pLogSystemTime[i-1]),
					 (int)m_pLogMediaTime[i], (int)(m_pLogMediaTime[i] - m_pLogMediaTime[i-1]), (int)m_pLogFrameSpeed[i],
					 (int)m_pLogFrameSize[i] & 0X7FFFFFFF, szKey);
		cmnFileWrite (0, hFile, (OMX_U8 *)szLine, strlen (szLine));
	}

	cmnFileClose (0, hFile);
*/
	m_nLogFrameIndex = 0;
}

OMX_ERRORTYPE voCOMXFileOutputPort::FillBufferOne (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (IsTunnel () && IsSupplier ())
	{
		if (m_bEOS)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		OMX_TRANS_STATE transType = m_pParent->GetTransState ();
		OMX_STATETYPE	stateType = m_pParent->GetCompState ();

		if (m_sStatus == PORT_TRANS_ENA2DIS)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if (transType == COMP_TRANSSTATE_ExecuteToIdle ||
			transType == COMP_TRANSSTATE_PauseToIdle)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if ((stateType == OMX_StateIdle || IsFlush ()) &&
			(transType != COMP_TRANSSTATE_IdleToExecute && transType != COMP_TRANSSTATE_IdleToPause))
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}
	}

	errType = FillBuffer (pBuffer);

	if (IsTunnel ())
	{
		pBuffer->nInputPortIndex = m_nTunnelPort;
		pBuffer->nOutputPortIndex = m_sType.nPortIndex;

		errType = m_hTunnelComp->EmptyThisBuffer (m_hTunnelComp, pBuffer);
	}
	else
	{
		errType = m_pCallBack->FillBufferDone (m_pParent->GetComponent (), m_pAppData, pBuffer);
	}

	if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		m_bEOS = OMX_TRUE;

	return errType;
}
