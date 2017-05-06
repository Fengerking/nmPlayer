	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileSource.cpp

	Contains:	voCOMXFileSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voCOMXFileSource.h"
#include "voCOMXFileAudioPort.h"
#include "voCOMXFileVideoPort.h"
#include "CFileFormatCheck.h"
#include "CRTSPSource.h"
#include "CHTTPPDSource.h"
#include "voLiveSource.h"
#include "voWidevine.h"

#define LOG_TAG "voCOMXFileSource"
#include "voLog.h"

typedef	VO_PTR (VO_API * VOFILESOURCEOPEN) (VO_FILE_SOURCE * pSource);
typedef	VO_S64 (VO_API * VOFILESOURCESEEK) (VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);

extern VO_TCHAR * g_pvoOneWorkingPath;

#ifdef _HTC
static char* g_szHeadersParam_Header				= "x-htc-";
static char* g_szHeadersParam_Mimetype				= "x-htc-mimetype";
static char* g_szHeadersParam_RTSPProxyIP			= "x-htc-rtspproxyip";
static char* g_szHeadersParam_RTSPProxyPort			= "x-htc-rtspproxyport";
static char* g_szHeadersParam_MinUDPPort			= "x-htc-minudpport";
static char* g_szHeadersParam_MaxUDPPort			= "x-htc-maxudpport";
static char* g_szHeadersParam_RTSPTimeout			= "x-htc-rtsptimeout";
static char* g_szHeadersParam_RTPTimeout			= "x-htc-rtptimeout";
static char* g_szHeadersParam_RTCPReportInterval	= "x-htc-rtcpreportinterval";
static char* g_szHeadersParam_BufferTime			= "x-htc-buffertime";
static char* g_szHeadersParam_RTSPSeekTo			= "x-htc-rtspseekto";
static char* g_szHeadersParam_HTTPProxyIP			= "x-htc-httpproxyip";
static char* g_szHeadersParam_HTTPProxyPort			= "x-htc-httpproxyport";
#else	// _HTC
static char* g_szHeadersParam_Header				= (char*)"x-von-";
static char* g_szHeadersParam_Mimetype				= (char*)"x-von-mimetype";
static char* g_szHeadersParam_RTSPProxyIP			= (char*)"x-von-rtspproxyip";
static char* g_szHeadersParam_RTSPProxyPort			= (char*)"x-von-rtspproxyport";
static char* g_szHeadersParam_MinUDPPort			= (char*)"x-von-minudpport";
static char* g_szHeadersParam_MaxUDPPort			= (char*)"x-von-maxudpport";
static char* g_szHeadersParam_RTSPTimeout			= (char*)"x-von-rtsptimeout";
static char* g_szHeadersParam_RTPTimeout			= (char*)"x-von-rtptimeout";
static char* g_szHeadersParam_RTCPReportInterval	= (char*)"x-von-rtcpreportinterval";
static char* g_szHeadersParam_BufferTime			= (char*)"x-von-buffertime";
static char* g_szHeadersParam_RTSPSeekTo			= (char*)"x-von-rtspseekto";
static char* g_szHeadersParam_HTTPProxyIP			= (char*)"x-von-httpproxyip";
static char* g_szHeadersParam_HTTPProxyPort			= (char*)"x-von-httpproxyport";
#endif	// _HTC

voCOMXFileSource::voCOMXFileSource(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pAudioPort (NULL)
	, m_pVideoPort (NULL)
	, m_pClockPort (NULL)
	, m_nURISize (0)
	, m_pURI (NULL)
	, m_pPipe (NULL)
	, m_pDrm (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_bForceClose (OMX_FALSE)
	, m_pHeaders(NULL)
	, m_pRawHeaders(NULL)
	, m_pTmpHeaders(NULL)
	, m_nSeekMode (OMX_TIME_SeekModeAccurate)
	, m_llStartPos (0)
	, m_pSource (NULL)
	, m_nAudioTrack (-1)			   
	, m_nVideoTrack (-1)
	, m_nWVC1Dec(1)
	, m_pFileOP (NULL)
	, m_bBufferThread (OMX_TRUE)
	, m_ffSource (VO_FILE_FFUNKNOWN)
	, m_ffExternalSet (VO_FILE_FFUNKNOWN)
	, m_LiveSrcType(VO_LIVESRC_FT_MAX)
	, m_llTmpRefTime(0)
	, m_bBlockUnknownCodec (OMX_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.FileSource");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_pCfgSource = new CBaseConfig ();
	m_pCfgSource->Open ((TCHAR*)_T("vomeplay.cfg"));

	if (m_pCfgSource->GetItemValue (m_pName, (char*)"BufferThread", 1) == 0)
		m_bBufferThread = OMX_FALSE;

	m_nDumpRuntimeLog = m_pCfgSource->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

	if (m_pCfgSource->GetItemValue ((char*)"vomeMF", (char*)"SeekMode", 0) == 1)
		m_nSeekMode = OMX_TIME_SeekModeFast;

	VO_TCHAR szCfgFile[256];
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vommcodec.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vommcodec.cfg"));
	}
	m_pCfgCodec = new CBaseConfig ();
	m_pCfgCodec->Open (szCfgFile);
}

voCOMXFileSource::~voCOMXFileSource()
{
	voCOMXAutoLock lockClose (&m_tmForceClose);
	m_bForceClose = OMX_TRUE;

	CloseFile ();

	if(m_pURI)
		voOMXMemFree(m_pURI);
	if(m_pHeaders)
		voOMXMemFree(m_pHeaders);
	if(m_pRawHeaders)
		voOMXMemFree(m_pRawHeaders);
	if(m_pTmpHeaders)
		voOMXMemFree(m_pTmpHeaders);

	delete m_pCfgSource;
	delete m_pCfgCodec;

	if(m_pIVOWideVineDRM)
		m_pIVOWideVineDRM->closeDecryptSession();

//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXFileSource::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
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

			errType = ((voCOMXFileOutputPort *)m_ppPorts[pU32Type->nPortIndex])->GetStreams (&pU32Type->nU32);
		}
		break;

	case OMX_IndexParamActiveStream:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			errType = ((voCOMXFileOutputPort *)m_ppPorts[pU32Type->nPortIndex])->SetStream (pU32Type->nU32);
		}
		break;

	case OMX_VO_IndexSourceNearKeyFrame:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_VO_SOURCE_NEARKEYFRAME* pNearKeyframe = (OMX_VO_SOURCE_NEARKEYFRAME*)pComponentParameterStructure;
			VO_SOURCE_NEARKEYFRAME nearKeyframe;
			nearKeyframe.Time = pNearKeyframe->Time;
			nearKeyframe.PreviousKeyframeTime = 0x7fffffffffffffffll;
			nearKeyframe.NextKeyframeTime = 0x7fffffffffffffffll;
			VO_U32 nRC = m_pSource->GetNearKeyframe(&nearKeyframe);
			if(nRC != VO_ERR_SOURCE_OK)
				return OMX_ErrorNotImplemented;

			pNearKeyframe->PreviousKeyframeTime = nearKeyframe.PreviousKeyframeTime;
			pNearKeyframe->NextKeyframeTime = nearKeyframe.NextKeyframeTime;

			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexInterlacedVideoCheck:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			OMX_BOOL bInterlace;
			VO_U32 nRC = m_pSource->GetTrackParam(m_nVideoTrack, VO_PID_SOURCE_ISINTERLACE, &bInterlace);
			if(VO_ERR_NONE != nRC)
				return OMX_ErrorNotImplemented;

			*((OMX_BOOL*)pComponentParameterStructure) = bInterlace;
		}
		break;

	case OMX_VO_IndexFileFormat:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;
			
			*((VO_FILE_FORMAT*)pComponentParameterStructure) = m_ffSource;
		}
		break;

	case OMX_VO_IndexSeekablePosition:
		{
			if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;

			VO_S64 nSeekablePos = -1;
			VO_U32 nRC = m_pSource->GetSourceParam(VO_PID_SOURCE_SEEKABLETIME, &nSeekablePos);
			if(VO_ERR_NOT_IMPLEMENT == nRC)
				return OMX_ErrorNotImplemented;
			else if(VO_ERR_NONE == nRC)
			{
				*((OMX_S64 *)pComponentParameterStructure) = nSeekablePos;

				return OMX_ErrorNone;
			}
			else
				return OMX_ErrorUndefined;
		}
		break;
	
	case OMX_VO_IndexTrackCount:
		VO_SOURCE_INFO m_sourceInfo;
		if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;
		m_pSource->GetSourceInfo((VO_PTR)&m_sourceInfo);
		VOLOGI("clip track is %d",m_sourceInfo.Tracks);
		*((OMX_S32*)pComponentParameterStructure) = m_sourceInfo.Tracks;
		break;

	case OMX_VO_IndexTrackInfo:
		VOME_TRACKINFO *trackInfo;
		VO_SOURCE_TRACKINFO	indexTrkInfo;
		if (m_pSource == NULL)
				return OMX_ErrorNotImplemented;
		trackInfo = (VOME_TRACKINFO *)pComponentParameterStructure;
		m_pSource->GetTrackInfo (trackInfo->nIndex, &indexTrkInfo);
		if(indexTrkInfo.Type == VO_SOURCE_TT_VIDEO)
			trackInfo->nType = VOME_SOURCE_VIDEO;
		else if(indexTrkInfo.Type == VO_SOURCE_TT_AUDIO)
			trackInfo->nType = VOME_SOURCE_AUDIO;
		else if(indexTrkInfo.Type == VO_SOURCE_TT_SUBTITLE)
			trackInfo->nType = VOME_SOURCE_TEXT;
		VOLOGI("nIndex is %d,nType is %d",trackInfo->nIndex,trackInfo->nType);
		break;

	default:
		errType = voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXFileSource::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nIndex)
	{
	case OMX_IndexParamContentURI:
		{
			OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pContent, pContent->nSize);
// 			if (errType != OMX_ErrorNone)
// 				return errType;

			if (m_pURI != NULL)
				voOMXMemFree (m_pURI);

			m_nURISize = pContent->nSize - 8;
			m_pURI = (OMX_S8*)voOMXMemAlloc (m_nURISize);

			if (m_pURI == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pURI, pContent->contentURI, m_nURISize);

			errType = LoadFile ((OMX_PTR) m_pURI, 1);
		}
		break;

	case OMX_VO_IndexSourceHandle:
		errType = LoadFile ((OMX_PTR) pComponentParameterStructure, 2);
		break;

	case OMX_VO_IndexSourceID:
		errType = LoadFile ((OMX_PTR) pComponentParameterStructure, 3);
		break;

	case OMX_VO_IndexWVC1DecType:
		m_nWVC1Dec = *(OMX_U32 *)pComponentParameterStructure;
		if(m_pSource)
			m_pSource->SetWVC1Decoder(m_nWVC1Dec);
		break;

	case OMX_VO_IndexSourceOffset:
		m_nOffset = *(OMX_S64 *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourceLength:
		m_nLength = *(OMX_S64 *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourcePipe:
		m_pPipe = (OMX_VO_FILE_OPERATOR *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourceDrm:
		m_pDrm = (OMX_VO_SOURCEDRM_CALLBACK *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourceHeaders:
		{
			VOLOGI ("OMX_VO_IndexSourceHeaders %s", (OMX_STRING)pComponentParameterStructure);

			if(m_pHeaders)
				voOMXMemFree(m_pHeaders);

			m_pHeaders = (OMX_STRING)voOMXMemAlloc(strlen((OMX_STRING)pComponentParameterStructure) + 1);
			if(m_pHeaders)
				strcpy(m_pHeaders , (OMX_STRING)pComponentParameterStructure);

			if(m_pRawHeaders)
				voOMXMemFree(m_pRawHeaders);
			m_pRawHeaders = (OMX_STRING)voOMXMemAlloc(strlen((OMX_STRING)pComponentParameterStructure) + 1);

			GetRawHeaders();
		}
		break;

	case OMX_VO_IndexSourceFileFormat:
		m_ffExternalSet = *(VO_FILE_FORMAT *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexLiveStreamingType:
		m_LiveSrcType = *(VO_LIVESRC_FORMATTYPE*)pComponentParameterStructure;
		break;

	case OMX_VO_IndexBlockUnknownCodec:
		m_bBlockUnknownCodec = *((OMX_BOOL *)pComponentParameterStructure);
		break;

	case OMX_VO_IndexAddRTSPField:
		if(m_pSource)
			m_pSource->SetSourceParam(VOID_STREAMING_ADD_RTSP_FIELD, pComponentParameterStructure);

		break;

	case OMX_VO_IndexSetAudioTrack:
		VOME_TRACKSET* trackSet;
		VO_SOURCE_SELTRACK sSelTrack;
		trackSet = (VOME_TRACKSET*)pComponentParameterStructure;
		if(m_pAudioPort != NULL && m_pSource != NULL)
		{
			errType = m_pAudioPort->SetTrack(m_pSource,trackSet->nIndex);
			if(errType != OMX_ErrorNone)
				return errType;
			
			// reset select tracks
			sSelTrack.bInUsed = VO_TRUE;
			sSelTrack.nIndex = trackSet->nIndex;
			sSelTrack.llTs = (VO_S64)trackSet->nTimeStamp;
			VOLOGI("Ferry Audio entering!index is %d,time is %d",trackSet->nIndex,trackSet->nTimeStamp);
			m_pSource->SetSourceParam(VO_PID_SOURCE_SELTRACK, &sSelTrack);
		}
		break;

	case OMX_VO_IndexSetVideoTrack:
		break;
	default:
		errType = voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXFileSource::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
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


OMX_ERRORTYPE voCOMXFileSource::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimeSeekMode:
		{
			OMX_TIME_CONFIG_SEEKMODETYPE * pSeekMode = (OMX_TIME_CONFIG_SEEKMODETYPE *)pComponentConfigStructure;

			m_nSeekMode = pSeekMode->eType;
			if (m_pAudioPort != NULL)
				m_pAudioPort->SetSeekMode (m_nSeekMode);
			if (m_pVideoPort != NULL)
				m_pVideoPort->SetSeekMode (m_nSeekMode);

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimePosition:
		{
			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;
			if (pTime->nPortIndex != 0)
				return OMX_ErrorNone;

			if (m_sState <= OMX_StateIdle)
				m_llStartPos = pTime->nTimestamp;

			// now only PD will handle this parameter for async seek
			//VO_U32 nRC = 
				m_pSource->SetSourceParam(VO_PID_SOURCE_SEEKMODE, &m_nSeekMode);

/*			we need move it to framework so that flush can be canceled, East 20111025
			VO_S64 nSeekablePos = -1;
			nRC = m_pSource->GetSourceParam(VO_PID_SOURCE_SEEKABLETIME, &nSeekablePos);
			if(VO_ERR_NOT_IMPLEMENT != nRC)	// just Push Play implement this Parameter
			{
				if(nRC != VO_ERR_NONE)	// no index or other error
				{
					VOLOGW ("VO_PID_SOURCE_SEEKABLETIME nRC 0x%08X", nRC);
					return OMX_ErrorNone;
				}

				if(nSeekablePos < pTime->nTimestamp)	// download can't support this position
				{
					VOLOGW ("Seekable Position %d, Seek Position %d", (VO_S32)nSeekablePos, (VO_S32)pTime->nTimestamp);
					return OMX_ErrorNone;
				}
			}
			else
			{
				VOLOGR ("not Push Play");
			}*/

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			VO_S64 nNewPos = pTime->nTimestamp;
			if (m_nVideoTrack >= 0 && m_pVideoPort->IsEnable ())
			{
				errType = m_pVideoPort->SetTrackPos (&nNewPos);

				m_llTmpRefTime = nNewPos;

				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("m_pVideoPort->SetTrackPos was failed. 0X%08X", errType);

					if (m_pClockPort != NULL)
						m_pClockPort->UpdateMediaTime (pTime->nTimestamp);

					return errType;
				}
			}

			if (m_nSeekMode == OMX_TIME_SeekModeAccurate)
				nNewPos = pTime->nTimestamp;

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

			pTime->nTimestamp = nNewPos;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXFileSource::SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_COMMANDTYPE Cmd,
											OMX_IN  OMX_U32 nParam1,
											OMX_IN  OMX_PTR pCmdData)
{
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

OMX_ERRORTYPE voCOMXFileSource::SetNewState (OMX_STATETYPE newState)
{
	// check the seek mode
	if (newState == OMX_StateIdle && m_nSeekMode == OMX_TIME_SeekModeAccurate)
	{
		if (m_pVideoPort != NULL && !m_pVideoPort->IsTunnel ())
		{
			m_nSeekMode = OMX_TIME_SeekModeFast;
			m_pVideoPort->SetSeekMode (m_nSeekMode);
		}
	}

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
		
		if (m_llStartPos == 0 && newState == OMX_StateIdle && m_ffSource != VO_FILE_FFSTREAMING_HTTPPD && m_ffSource != VO_FILE_FFSTREAMING_RTSP && m_ffSource != VO_FILE_FFAPPLICATION_SDP)
		{
			OMX_ERRORTYPE errType = OMX_ErrorNone;

			OMX_S64 nNewPos = 0;
			if (m_pVideoPort != NULL)
				errType = m_pVideoPort->SetTrackPos (&nNewPos);

			nNewPos = 0;
			if (m_pAudioPort != NULL)
				errType = m_pAudioPort->SetTrackPos (&nNewPos);
		}
	}

	return voCOMXCompSource::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXFileSource::LoadFile (OMX_PTR pSource, OMX_U32 nType)
{
	if (m_pFileOP == NULL)
	{
		if (m_pIFileBasedDrm != NULL)
		{
			m_pFileOP = &m_FileOP;
			m_pFileOP->Open = FileBasedDrm_Open;
			m_pFileOP->Read = FileBasedDrm_Read;
			m_pFileOP->Write = FileBasedDrm_Write;
			m_pFileOP->Seek = FileBasedDrm_Seek;
			m_pFileOP->Flush = FileBasedDrm_Flush;
			m_pFileOP->Size = FileBasedDrm_Size;
			m_pFileOP->Close = FileBasedDrm_Close;
		}
		else if (m_pPipe == NULL)
		{
			cmnFileFillPointer ();
			m_pFileOP = &g_fileOP;
		}
		else
		{
			m_pFileOP = &m_FileOP;
			m_pFileOP->Open = (VOFILESOURCEOPEN) m_pPipe->Open;
			m_pFileOP->Read = m_pPipe->Read;
			m_pFileOP->Write = m_pPipe->Write;
			m_pFileOP->Seek = (VOFILESOURCESEEK) m_pPipe->Seek;
			m_pFileOP->Flush = m_pPipe->Flush;
			m_pFileOP->Size = m_pPipe->Size;
			m_pFileOP->Close = m_pPipe->Close;
		}
	}

	CloseFile ();

	voCOMXAutoLock lock (&m_tmLoadSource);

	m_bForceClose = OMX_FALSE;

	m_ffSource = m_ffExternalSet;

	if (m_ffSource == VO_FILE_FFUNKNOWN)
	{
		OMX_S8 szMimetype[128];
		if(GetValueFromHeaders(g_szHeadersParam_Mimetype, (OMX_STRING)szMimetype) == OMX_TRUE)
		{
			if(!strcmp((OMX_STRING)szMimetype, "application/sdp"))
				m_ffSource = VO_FILE_FFAPPLICATION_SDP;
		}
	}

	if (m_ffSource == VO_FILE_FFUNKNOWN && (nType & 0X0F) == 1)
	{
		m_ffSource = CFileFormatCheck::GetFileFormat((VO_PTCHAR)pSource, FLAG_CHECK_SDP);
		if(m_ffSource == VO_FILE_FFUNKNOWN)
		{
			VO_TCHAR szURL[1024*2];
			vostrcpy (szURL, (VO_PTCHAR)pSource);
			//vostrupr (szURL);

			if (!vostrncmp (szURL, _T("RTSP://"), 6) || !vostrncmp (szURL, _T("rtsp://"), 6) || 
				!vostrncmp (szURL, _T("MMS://"), 5) || !vostrncmp (szURL, _T("mms://"), 5))
				m_ffSource = VO_FILE_FFSTREAMING_RTSP;
			else if (!vostrncmp (szURL, _T("HTTP://"), 6) || !vostrncmp (szURL, _T("http://"), 6))
			{
				VO_TCHAR* p = vostrstr(szURL, _T(".sdp"));
				if(!p)
					p = vostrstr(szURL, _T(".SDP"));

				if(p && ((p + 4 - szURL) == (int)vostrlen(szURL) || p[4] == _T('?')))
					m_ffSource = VO_FILE_FFSTREAMING_RTSP;
				else
					m_ffSource = VO_FILE_FFSTREAMING_HTTPPD;
			}
		}
	}

	VO_U32 uFileFlag = VO_FILE_TYPE_NAME;
	if ((nType & 0X0F) == 1)
		uFileFlag = VO_FILE_TYPE_NAME;
	else if ((nType & 0X0F) == 2)
		uFileFlag = VO_FILE_TYPE_HANDLE;
	else
		uFileFlag = VO_FILE_TYPE_ID;

	if(m_ffSource == VO_FILE_FFUNKNOWN)
	{
		VO_FILE_SOURCE filSource;
		memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
		filSource.nMode = VO_FILE_READ_ONLY;
		filSource.pSource = pSource;
		filSource.nFlag = uFileFlag;
		filSource.nOffset = m_nOffset;
		filSource.nLength = m_nLength;
		filSource.nReserve = (VO_U32)m_pIFileBasedDrm;	//it will be used by FileBasedDRM API

		CFileFormatCheck filCheck(m_pFileOP, m_pMemOP);
		m_ffSource = filCheck.GetFileFormat(&filSource, ~FLAG_CHECK_AUDIOREADER2);

		if(m_ffSource == VO_FILE_FFUNKNOWN && uFileFlag == VO_FILE_TYPE_NAME)
			m_ffSource = CFileFormatCheck::GetFileFormat((VO_PTCHAR)pSource, FLAG_CHECK_EXTNAME);
	}

	VO_SOURCEDRM_CALLBACK cbSourceDrm;
	if(m_pPlayReadyAPI||m_pIVOWideVineDRM)
	{
		VOLOGI ("Framework Set APIs directly, so we should implement callback in source component.");
		cbSourceDrm.fCallback = voOMXSourceDrmCallBack;
		cbSourceDrm.pUserData = this;
	}
	else if(m_pDrm)
	{
		cbSourceDrm.fCallback = (VOSOURCEDRMCALLBACK)m_pDrm->fCallback;
		cbSourceDrm.pUserData = m_pDrm->pUserData;
	}
	else
	{
		cbSourceDrm.fCallback = NULL;
		cbSourceDrm.pUserData = NULL;
	}

	if (m_ffSource == VO_FILE_FFSTREAMING_RTSP || m_ffSource == VO_FILE_FFAPPLICATION_SDP)
		m_pSource = new CRTSPSource (NULL, m_ffSource, m_pMemOP, m_pFileOP, (m_pPlayReadyAPI ||m_pIVOWideVineDRM|| m_pDrm) ? &cbSourceDrm : NULL);
	else if (m_ffSource == VO_FILE_FFSTREAMING_HTTPPD)
		m_pSource = new CHTTPPDSource (NULL, m_ffSource, m_pMemOP, m_pFileOP, (m_pPlayReadyAPI ||m_pIVOWideVineDRM|| m_pDrm) ? &cbSourceDrm : NULL);
	else
		m_pSource = new CFileSource (NULL, m_ffSource, m_pMemOP, m_pFileOP, (m_pPlayReadyAPI ||m_pIVOWideVineDRM|| m_pDrm) ? &cbSourceDrm : NULL);
	if (m_pSource == NULL)
		return OMX_ErrorInsufficientResources;

	VO_U32 nDumpLevel = 0;
	if (m_pCfgSource != NULL)
		nDumpLevel = m_pCfgSource->GetItemValue (m_pName, (char*)"DumpLogLevel", 0);
	m_pSource->SetConfig (m_pCfgCodec);
	m_pSource->SetDumpLogLevel (nDumpLevel);
	m_pSource->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
	m_pSource->SetWorkPath ((VO_TCHAR *)m_pWorkPath);
	m_pSource->SetCallBack ((OMX_PTR)voomxFileSourceStreamCallBack, (OMX_PTR)this);
	m_pSource->SetWVC1Decoder(m_nWVC1Dec);
	if(m_pIFileBasedDrm)
		m_pSource->SetIFileBasedDrm(m_pIFileBasedDrm);
	
	if (m_ffSource == VO_FILE_FFSTREAMING_RTSP || m_ffSource == VO_FILE_FFAPPLICATION_SDP)
	{
		VOStreamInitParam* pInitParam = ((CRTSPSource*)m_pSource)->GetStreamInitParam();

		if(m_pRawHeaders)
		{
			if(!pInitParam->bUseExtraFields)
			{
				pInitParam->bUseExtraFields = 1;
				pInitParam->szExtraFields = m_pRawHeaders;
			}
			else
			{
				if(m_pTmpHeaders)
					voOMXMemFree(m_pTmpHeaders);

				m_pTmpHeaders = (OMX_STRING)voOMXMemAlloc(strlen(pInitParam->szExtraFields) + strlen(m_pRawHeaders) + 1);
				if(m_pTmpHeaders)
				{
					strcpy(m_pTmpHeaders, pInitParam->szExtraFields);
					strcat(m_pTmpHeaders, m_pRawHeaders);

					pInitParam->szExtraFields = m_pTmpHeaders;
				}
			}
		}

		OMX_S8 szValue[260];
		if(GetValueFromHeaders(g_szHeadersParam_RTSPProxyIP, (OMX_STRING)szValue) == OMX_TRUE)
		{
			pInitParam->bUseRTSPProxy = 1;
			strcpy(pInitParam->szProxyIP, (OMX_STRING)szValue);

			if(GetValueFromHeaders(g_szHeadersParam_RTSPProxyPort, (OMX_STRING)szValue) == OMX_TRUE)
			{
				strcat(pInitParam->szProxyIP, ":");
				strcat(pInitParam->szProxyIP, (OMX_STRING)szValue);
			}
			else
				pInitParam->bUseRTSPProxy = 0;
		}
		if(GetValueFromHeaders(g_szHeadersParam_MinUDPPort, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nMinUdpPort = atoi((OMX_STRING)szValue);
		if(GetValueFromHeaders(g_szHeadersParam_MaxUDPPort, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nMaxUdpPort = atoi((OMX_STRING)szValue);
		if(GetValueFromHeaders(g_szHeadersParam_RTSPTimeout, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nRTSPTimeout = atoi((OMX_STRING)szValue) / 1000;
		if(GetValueFromHeaders(g_szHeadersParam_RTPTimeout, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nRTPTimeOut = atoi((OMX_STRING)szValue) / 1000;
		if(GetValueFromHeaders(g_szHeadersParam_RTCPReportInterval, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nRTCPInterval = atoi((OMX_STRING)szValue);
		if(GetValueFromHeaders(g_szHeadersParam_BufferTime, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nBufferTime = atoi((OMX_STRING)szValue);

		if(GetValueFromHeaders(g_szHeadersParam_RTSPSeekTo, (OMX_STRING)szValue) == OMX_TRUE)
			((CRTSPSource*)m_pSource)->SetInitPlayPos(atoi((OMX_STRING)szValue));
	}
	else if (m_ffSource == VO_FILE_FFSTREAMING_HTTPPD)
	{
		m_pSource->SetLiveSrcType(m_LiveSrcType);
		VOPDInitParam* pInitParam = ((CHTTPPDSource*)m_pSource)->GetPDInitParam();

		if(m_pRawHeaders)
			pInitParam->ptr_cookie = m_pRawHeaders;

		VO_TCHAR szValue[260];
		if(GetValueFromHeaders(g_szHeadersParam_HTTPProxyIP, (OMX_STRING)szValue) == OMX_TRUE)
		{
			vostrcpy(pInitParam->mProxyName, (PTCHAR)szValue);

			if(GetValueFromHeaders(g_szHeadersParam_HTTPProxyPort, (OMX_STRING)szValue) == OMX_TRUE)
			{
				vostrcat(pInitParam->mProxyName, _T(":"));
				vostrcat(pInitParam->mProxyName, (PTCHAR)szValue);
			}
			else
				memset(pInitParam->mProxyName, 0, sizeof(pInitParam->mProxyName));
		}

		if(GetValueFromHeaders(g_szHeadersParam_BufferTime, (OMX_STRING)szValue) == OMX_TRUE)
			pInitParam->nBufferTime = atoi((OMX_STRING)szValue);
	}

	if ((m_ffSource == VO_FILE_FFAUDIO_AAC || m_ffSource == VO_FILE_FFAUDIO_MP3  || m_ffSource == VO_FILE_FFUNKNOWN) && m_pCfgSource != NULL)
	{
		int nScanLevel = m_pCfgSource->GetItemValue (m_pName, (char*)"ScanFileLevel", 0);
		if (nScanLevel == 100)
			uFileFlag = uFileFlag | VO_SOURCE_OPENPARAM_FLAG_EXACTSCAN;
	}
	
	if(m_pIFileBasedDrm)
	{
		// since our file operator function APIs just has Size, and it is used for current actual size
		// so for OMA DRM, we need tell Parser complete file size
		OMX_VO_DRM_FILEBASED_IO	sIO;
		memset(&sIO, 0, sizeof(sIO));
		OMX_S32 nFD = -1;
		int nRet = -1;
		if ((nType & 0X0F) == 1)	// URL string
			nRet = m_pIFileBasedDrm->Open(&sIO, (const char*)pSource);
		else if ((nType & 0X0F) == 2)	// Handle
			;
		else	// FD
		{
#ifdef _LINUX
			nFD = dup((int)pSource);
			nRet = m_pIFileBasedDrm->Open(&sIO, nFD);
#endif	// _LINUX
		}

		if(nRet != 0)
		{
			VOLOGE ("IVOFileBasedDrm Open fail %d, nType 0x%08X", nRet, (int)nType);
			return OMX_ErrorUndefined;
		}

		OMX_S32 nCompleteFileSize = m_pIFileBasedDrm->Size(&sIO);
		m_pIFileBasedDrm->Close(&sIO);
#ifdef _LINUX
		if(-1 != nFD)
			close(nFD);
#endif	// _LINUX

		VOLOGI ("DRM file complete file size %d, Length input %d", (int)nCompleteFileSize, (int)m_nLength);
		// modify m_nLength so that notify file source
		m_nLength = nCompleteFileSize;
	}

	if(m_pIVOWideVineDRM)
	{
		VO_S32 nRC = Widevine_DRM_ERROR_BASE;

		if((nType & 0X0F)== 1)//url
		{
			VOLOGI("file url=%s", (const char*)pSource);
			nRC = m_pIVOWideVineDRM->openDecryptSession((const char*)pSource);
		}
		else if ((nType & 0X0F) == 2)	// Handle
			;
		else	// FD
		{
#ifdef _LINUX
			VOLOGI("nType=%d, pSource=%d", (int)nType,(int)pSource);
			nRC = m_pIVOWideVineDRM->openDecryptSession(dup((int)pSource), m_nOffset, m_nLength);
#endif	// _LINUX			
		}

		if(nRC!= Widevine_DRM_NO_ERROR)
		{
			VOLOGE("Widevine DRM Open DecryptSession fail: %d", (int)nRC);
			if(m_pCallBack)
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, OMX_EventError, nRC, 0, NULL);

			return OMX_ErrorNotImplemented;
		}
	}

	VO_U32 nRC = m_pSource->LoadSource (pSource, uFileFlag, m_nOffset, m_nLength);

	voCOMXAutoLock lockClose (&m_tmForceClose);
	if (m_bForceClose)
		return OMX_ErrorUndefined;

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_pSource->LoadSource was failed. 0X%08X", (int)nRC);

		if(VO_ERR_SOURCE_FORMATUNSUPPORT == nRC)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_FormatUnsupported, 0, 0, NULL);
		else if(VO_ERR_SOURCE_CODECUNSUPPORT == nRC)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecUnsupported, 0, 0, NULL);
		else if(VO_ERR_SOURCE_NEEDPROXY == nRC)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_NeedProxy, 0, 0, NULL);

		return OMX_ErrorUndefined;
	}

	if(VO_FILE_FFAUDIO_OGG == m_pSource->GetFormat())
	{
		VO_BOOL bNeedLoop = VO_FALSE;
		m_pSource->GetSourceParam (VO_PID_SOURCE_NEEDLOOPPLAY, &bNeedLoop);
		if(bNeedLoop)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_NeedLoopPlay, 0, 0, NULL);
	}

	VO_SOURCE_INFO	filInfo;
	memset (&filInfo, 0, sizeof (VO_SOURCE_INFO));
	m_pSource->GetSourceInfo (&filInfo);
	if (filInfo.Tracks == 0)
	{
		VOLOGE ("m_pSource->GetSourceInfo was failed. 0X%08X", (int)nRC);
		return OMX_ErrorUndefined;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < filInfo.Tracks; i++)
	{
		memset (&trkInfo, 0, sizeof (VO_SOURCE_TRACKINFO));
		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1)
		{
			if(m_bBlockUnknownCodec && VO_AUDIO_CodingUnused == trkInfo.Codec)
			{
				VOLOGW ("Unsupport Audio Track %d, Not create port for it", (int)i);
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_UnknownCodecPortBlocked, 0, 0, NULL);
				continue;
			}

			m_nAudioTrack = i;

			VO_U32 nFourcc = 0;
			m_pSource->GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourcc);
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_MediaFourcc, 0, nFourcc, NULL);
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)  && m_nVideoTrack == -1)
		{
			if(m_bBlockUnknownCodec && VO_VIDEO_CodingUnused == trkInfo.Codec)
			{
				VOLOGW ("Unsupport Video Track %d, Not create port for it", (int)i);
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_UnknownCodecPortBlocked, 1, 0, NULL);
				continue;
			}

 			VO_BOOL bInterlace = VO_FALSE;
 			nRC = m_pSource->GetTrackParam(i, VO_PID_SOURCE_ISINTERLACE, &bInterlace);
			// source component can't decide if play it or not, framework will judge OMX_VO_IndexInterlacedVideoCheck and decoder type then decide block it or not
// 			if(VO_ERR_NONE == nRC && bInterlace == VO_TRUE)
// 			{
// 				VOLOGW ("Unsupported interlaced video, notify Framework level OMX_VO_Event_VideoUnsupported, eventValue = %d", OMX_VO_Event_VideoUnsupported);
// 				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_VideoUnsupported, 0, 0, NULL);
// 			}

			VO_VIDEO_FORMAT fmtVideo;
			memset (&fmtVideo, 0, sizeof (VO_VIDEO_FORMAT));
			nRC = m_pSource->GetVideoFormat (i, &fmtVideo);
			if (nRC == VO_ERR_NONE && (fmtVideo.Width < 0 || fmtVideo.Height < 0))
			{
				VOLOGE ("m_pSource->GetVideoFormat was failed. 0X%08X", (int)nRC);
				return OMX_ErrorUndefined;
			}

			m_nVideoTrack = i;

			VO_U32 nFourcc = 0;
			nRC = m_pSource->GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourcc);
			// added by gtxia for deal with STE H264 Decoder can not support interlaced clip then I fake nFourcc			
			if(trkInfo.Codec == VO_VIDEO_CodingH264 && bInterlace)
			memcpy((char*)&nFourcc, (char*)"AVCI", 4);
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_MediaFourcc, 1, nFourcc, NULL);
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
		m_ppPorts[nIndex] = new voCOMXFileAudioPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pAudioPort = (voCOMXFileAudioPort *) m_ppPorts[nIndex];
		m_pAudioPort->SetSourceConfig (m_pCfgSource);
		m_pAudioPort->SetTrack (m_pSource, m_nAudioTrack);
		m_pAudioPort->SetSeekMode (m_nSeekMode);

		m_portParam[OMX_PortDomainAudio].nPorts = 1;
		m_portParam[OMX_PortDomainAudio].nStartPortNumber = nIndex;
		nIndex++;
	}

	if (m_nVideoTrack >= 0)
	{
		m_ppPorts[nIndex] = new voCOMXFileVideoPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pVideoPort = (voCOMXFileVideoPort *) m_ppPorts[nIndex];
		m_pVideoPort->SetSourceConfig (m_pCfgSource);
		m_pVideoPort->SetTrack (m_pSource, m_nVideoTrack);
		m_pVideoPort->SetSeekMode (m_nSeekMode);

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

	if(m_pAudioPort)
	{
		if(m_pVideoPort)	//audio and video
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_MediaContentType, 3, 0, NULL);
		else				//audio only
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_MediaContentType, 1, 0, NULL);
	}
	else
	{
		if(m_pVideoPort)	//video only
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_MediaContentType, 2, 0, NULL);
	}

	VO_BOOL bSeekable = VO_TRUE;
	nRC = m_pSource->GetSourceParam(VO_PID_SOURCE_SEEKABLE, &bSeekable);
	if(VO_ERR_NONE == nRC && bSeekable == VO_FALSE)
		m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_NotSeekable, 0, 0, NULL);

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXFileSource::CloseFile (void)
{
	if (m_pSource != NULL)
	{
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

OMX_ERRORTYPE voCOMXFileSource::ForceCloseSource (void)
{
	{
		voCOMXAutoLock lockClose (&m_tmForceClose);
		m_bForceClose = OMX_TRUE;

		if (m_pSource != NULL)
			m_pSource->CloseSource ();
	}

	voCOMXAutoLock lock (&m_tmLoadSource);
	if (m_pAudioPort != NULL)
		m_pAudioPort->SetTrack (NULL, -1);
	if (m_pVideoPort != NULL)
		m_pVideoPort->SetTrack (NULL, -1);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileSource::QuickReturnBuffer()
{
	if (m_pSource != NULL)
	{
		VO_QUICKRESPONSE_COMMAND cmd = VO_QUICKRESPONSE_RETURNBUFFER;
		m_pSource->SetSourceParam(VOID_STREAMING_QUICKRESPONSE_COMMAND, &cmd);
	}

	return OMX_ErrorNone;
}

OMX_U32	voCOMXFileSource::MessageHandle (COMP_MESSAGE_TYPE * pTask)
{
	if (pTask == NULL)
		return 0;

	if (pTask->uType == TASK_LOAD_FILE)
	{
		return LoadFile ((OMX_PTR) m_pURI, 1);
	}

	return voCOMXCompSource::MessageHandle (pTask);
}

OMX_U32 voCOMXFileSource::DumpPerformanceInfo (OMX_STRING pFile)
{
	if (m_pAudioPort != NULL)
		m_pAudioPort->DumpLog ();
	if (m_pVideoPort != NULL)
		m_pVideoPort->DumpLog ();
	return 0;
}

OMX_ERRORTYPE voCOMXFileSource::FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
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

int voCOMXFileSource::voomxFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	voCOMXFileSource * pSource = (voCOMXFileSource *)userData;

	if (EventCode == VO_STREAM_BUFFERSTART)
	{
		if (pSource->m_pClockPort != NULL)
		{
			pSource->m_pClockPort->GetMediaTime(&pSource->m_llTmpRefTime);
			pSource->m_pClockPort->StartWallClock (0);
		}

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
		// video sink can not break when WaitForRenderTime, so we should start clock here for video only media, East 20110528
		if (pSource->m_pClockPort != NULL)
		{
			OMX_COMPONENTTYPE * pComp = pSource->m_pClockPort->GetTunnelComp ();
			if (pComp != NULL)
			{
				OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkType;
				pComp->GetConfig (pComp, OMX_IndexConfigTimeActiveRefClock, &clkType);
				if (clkType.eClock == OMX_TIME_RefClockVideo)
				{
					pSource->m_pClockPort->UpdateMediaTime(pSource->m_llTmpRefTime);
					pSource->m_pClockPort->StartWallClock (1);
				}
			}
		}

		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData,(OMX_EVENTTYPE)OMX_VO_Event_BufferStop, 100, 0, NULL);
	}
	else if (EventCode == VO_STREAM_DOWNLODPOS)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_DownloadPos, *EventParam1, 0, NULL);
	}
	else if (EventCode == VO_STREAM_ERROR)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_StreamError, *EventParam1, 0, NULL);
	}
	else if (EventCode == VO_STREAM_RUNTIMEERROR)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_StreamError, *EventParam1, 1, NULL);
	}
	else if (EventCode == VO_STREAM_PACKETLOST)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_PacketLost, 0, 0, NULL);
	}
	else if (EventCode == VO_STREAM_BITRATETHROUGHPUT)
	{
		VOStreamBitrateThroughput* pBitrateThroughput = (VOStreamBitrateThroughput*)EventParam1;
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_StreamBitrateThroughput, pBitrateThroughput->nBitrate, pBitrateThroughput->nThroughput, NULL);
	}
	else if (EventCode == VO_STREAM_RTCP_RR)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_StreamRtcpRR, (OMX_U32)EventParam1, 0, NULL);
	}
	else if (EventCode == VO_STREAM_SEEKFINALPOS)
	{
		pSource->m_llTmpRefTime = *((VO_S64 *)EventParam1);
		VOLOGI ("VO_STREAM_SEEKFINALPOS %d", (int)pSource->m_llTmpRefTime);

		if (pSource->m_pClockPort)
			pSource->m_pClockPort->UpdateMediaTime (pSource->m_llTmpRefTime);
	}
	else if (EventCode >= VO_STREAM_RTSP_SESSION_START && EventCode <= VO_STREAM_RTSP_SESSION_TEARDOWN)
	{
		pSource->m_pCallBack->EventHandler (pSource->m_pComponent, pSource->m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_RTSPSessionEvent, EventCode + VO_STREAM_VALUEOFFSET, (OMX_U32)EventParam1, NULL);
	}

	return 0;
}

OMX_BOOL voCOMXFileSource::GetLineFromHeaders(OMX_STRING pKey, OMX_STRING pLine)
{
	if(m_pHeaders == NULL)
		return OMX_FALSE;

	char* ps = m_pHeaders;
	char* pe = NULL;
	char* p1 = NULL;
	while((pe = strstr(ps, "\r\n")) != NULL)
	{
		p1 = strstr(ps, pKey);
		if(p1 == ps && *(p1 + strlen(pKey)) == ':')
		{
			strncpy(pLine, ps, pe + 2 - ps);	//contain "\r\n"
			pLine[pe + 2 - ps] = '\0';
			return OMX_TRUE;
		}

		ps = pe + 2;
	}

	return OMX_FALSE;
}

OMX_BOOL voCOMXFileSource::GetValueFromHeaders(OMX_STRING pKey, OMX_STRING pValue)
{
	if(m_pHeaders == NULL)
		return OMX_FALSE;

	char* ps = m_pHeaders;
	char* pe = NULL;
	char* p1 = NULL;
	while((pe = strstr(ps, "\r\n")) != NULL)
	{
		p1 = strstr(ps, pKey);
		if(p1 == ps && *(p1 + strlen(pKey)) == ':')
		{
			strncpy(pValue, p1 + strlen(pKey) + 2, pe - p1 - strlen(pKey) - 2);	//no include "\r\n"
			pValue[pe - p1 - strlen(pKey) - 2] = '\0';
			if(strstr(pKey, "proxyip") != NULL)
				return CheckProxyIP(pValue);
			else if(strstr(pKey, "proxyport") != NULL)
				return CheckProxyPort(pValue);

			return OMX_TRUE;
		}

		ps = pe + 2;
	}

	return OMX_FALSE;
}

OMX_BOOL voCOMXFileSource::GetRawHeaders()
{
	if(!m_pHeaders || !m_pRawHeaders)
		return OMX_FALSE;

	//remove x-von-XXX\r\n from pHeaders
	strcpy(m_pRawHeaders, "");

	char* ps = m_pHeaders;
	char* pe = NULL;
	char* p1 = NULL;
	while((pe = strstr(ps, "\r\n")) != NULL)
	{
		p1 = strstr(ps, g_szHeadersParam_Header);
		if(p1 != ps)
			strncat(m_pRawHeaders, ps, pe + 2 - ps);

		ps = pe + 2;
	}

	return OMX_TRUE;
}

OMX_BOOL voCOMXFileSource::CheckProxyIP(OMX_STRING strIP)
{
	OMX_STRING pBeg, pEnd;
	OMX_U8 nIP[4] ={0};
	OMX_U32 i=0;
	pBeg =pEnd = strIP;

	// if proxy is host name, return true
	if(!(strIP[0]>='0'&&strIP[0]<='9'))
		return OMX_TRUE;

	while(*pEnd != '\0' )
	{
		if(*pEnd == '.')
		{
			*pEnd  = '\0';
			nIP[i++] = atoi(pBeg);
			*pEnd  = '.';
			pBeg = pEnd+1;
		}
		if(*(pEnd+1) == '\0')
		{
			nIP[i] = atoi(pBeg);
			break;
		}
		pEnd++;
	}
	VOLOGI("Proxy ip: %d:%d:%d:%d", nIP[0],nIP[1],nIP[2],nIP[3]);

	if(nIP[0] == 127)
		return OMX_FALSE;
	if((nIP[0]||nIP[1]||nIP[2]||nIP[3])==0)
		return OMX_FALSE;

	return OMX_TRUE;
}

OMX_BOOL voCOMXFileSource::CheckProxyPort(OMX_STRING strPort)
{
	OMX_S32 nPort = atoi(strPort);
	VOLOGI("Proxy port:%d", (int)nPort);
	if(nPort <= 0)
		return OMX_FALSE;
	return OMX_TRUE;
}
