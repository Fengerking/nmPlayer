#include "CLiveSrcBase.h"
#include "cmnMemory.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
CLiveSrcBase::CLiveSrcBase()
{
	m_cChannel = 0;
	m_nCurrentChannel = -1;
	m_nDefaultChannel = -1;
	m_pParser = NULL;
	m_fRecording = NULL;
	m_bEnableRecvData = VO_FALSE;
	m_bStreamInfoReady = VO_FALSE;
	m_nVideoType = VO_VIDEO_CodingUnused;
	m_nAudioType = VO_AUDIO_CodingUnused;
	m_bFirstKeyFSend = VO_FALSE;

	m_TotalEsgInfo.pEsgInfo = NULL;
	m_TotalEsgInfo.tEngine	= VO_LIVESRC_FT_FILE;

	SetStatusClosed();
}

CLiveSrcBase::~CLiveSrcBase()
{

}


VO_U32 CLiveSrcBase::Open (VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	Close();
	
	m_pUserData = m_pOrigUserData = pUserData;
	m_fStatus	= fStatus;
	m_fSendData = fSendData;

	VO_U32 rc = LoadConfig();
	
	m_bEnableRecvData		= VO_FALSE;
	m_bFirstKeyFSend = VO_FALSE;

	rc = doInitParser();

	rc = doOpenDevice();
	if (rc == VO_ERR_LIVESRC_OK)
		SetStatusOpened();

	return rc;
}

VO_U32 CLiveSrcBase::Close ()
{
	m_bStreamInfoReady	= VO_FALSE;
	m_bEnableRecvData	= VO_FALSE;

	if (IsStatusStarted())
		Stop();
	int rc = doCloseDevice();
	if (rc == 0)
		SetStatusClosed();

	//SaveConfig();
	doUninitParser();
	
	m_nVideoType = VO_VIDEO_CodingUnused;
	m_nAudioType = VO_AUDIO_CodingUnused;
//	m_bRecvFirstVideoFrame = VO_FALSE;

	return rc;
}

VO_U32 CLiveSrcBase::Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}

VO_U32 CLiveSrcBase::doUninitParser()
{
	if (m_pParser)
	{
		m_pParser->Close();
		delete m_pParser;
		m_pParser = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}


VO_U32	CLiveSrcBase::Cancel (VO_U32 nID)
{
	if (nID == VO_LIVESRC_TASK_SCAN)
	{
		EnableScan(VO_FALSE);
		return VO_ERR_LIVESRC_OK;
	}
	return VO_ERR_LIVESRC_FAIL;
}


VO_U32	CLiveSrcBase::Start ()
{
	VO_U32 rc = doStartRecieve();
	if (rc == VO_ERR_LIVESRC_OK)
		SetStatusStarted();

	return rc;
}

VO_U32	CLiveSrcBase::Pause ()
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}

VO_U32	CLiveSrcBase::Stop()
{
	VO_U32 rc = doStopRecieve();
	if (rc == VO_ERR_LIVESRC_OK)
		SetStatusStopped();

	if (m_fRecording)
	{
		cmnFileClose(m_fRecording);
		m_fRecording = NULL;
	}

	return rc;
}

VO_U32 CLiveSrcBase::SendBuffer(VOMP_BUFFERTYPE * pBuffer) 
{
	VO_U32 rc = VO_ERR_LIVESRC_NOIMPLEMENT;
	
	if(!m_bEnableRecvData)
		return VO_ERR_LIVESRC_WRONG_STATUS;
		
	VO_PARSER_INPUT_BUFFER buf;
	memset(&buf, 0, sizeof(VO_PARSER_INPUT_BUFFER));
	
	buf.pBuf		= pBuffer->pBuffer;
	buf.nBufLen		= pBuffer->nSize;
	
	// Jim: here maybe need check flag conversion
	buf.nFlag		= pBuffer->nFlag;
	
	rc = OnRecvData(&buf);
	
	return rc;
}

VO_U32 CLiveSrcBase::doStartRecieve()
{
	m_bEnableRecvData = VO_TRUE;
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcBase::doStopRecieve()
{
	m_bEnableRecvData = VO_FALSE;
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcBase::SetDataSource(VO_PTR pSource, VO_U32 nFlag)
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}

VO_U32 CLiveSrcBase::Seek(VO_U64 nTimeStamp)
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}


VO_U32	CLiveSrcBase::GetChannel (VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	*pCount			= m_cChannel;
	*ppChannelInfo	= m_aChannelInfo;
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcBase::SetChannel(VO_S32 nChannelID)
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}

VO_U32 CLiveSrcBase::GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** pESGInfo)
{
	return VO_ERR_LIVESRC_NOIMPLEMENT;
}

VO_U32 CLiveSrcBase::GetParam(VO_U32 nParamID, VO_PTR pValue)
{
	if(doGetParam(nParamID, pValue) == VO_ERR_LIVESRC_OK)
		return VO_ERR_LIVESRC_OK;

	switch(nParamID)
	{
	case VO_PID_LIVESRC_SIGNAL:
		{
			VO_U32 signal		= GetSignalStrength();
			*((VO_U32*)pValue)	= signal;
			return VO_ERR_LIVESRC_OK;
		}
		break;
	case VO_PID_LIVESRC_TRACKINFO:
		{
			// to do...
		}
		break;
	case VO_PID_LIVESRC_CHDATASAVEPATH:
		{
		
		}
		break;
	default:
		break;
	}

	return VO_ERR_LIVESRC_NOIMPLEMENT;
}


VO_U32 CLiveSrcBase::SetParam(VO_U32 nParamID, VO_PTR pValue)
{
	if(doSetParam(nParamID, pValue) == VO_ERR_LIVESRC_OK)
		return VO_ERR_LIVESRC_OK;

	switch(nParamID)
	{
	case VO_PID_LIVESRC_SIGNAL:
		break;
	case VO_PID_LIVESRC_TRACKINFO:
		break;
	case VO_PID_LIVESRC_STARTRECORD:
		{
			RecordStart((TCHAR*)pValue);
			return VO_ERR_LIVESRC_OK;
		}
		break;
	case VO_PID_LIVESRC_ENDRECORD:
		{
			RecordEnd();
			return VO_ERR_LIVESRC_OK;
		}
		break;
	case VO_PID_LIVESRC_CHDATASAVEPATH:
		{
			// to do...
		}
		break;
	default:
		break;
	}

	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_U32 CLiveSrcBase::doOpenDevice()
{
	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_U32 CLiveSrcBase::doCloseDevice()
{
	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_U32 CLiveSrcBase::doInitParser()
{
	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_U32 CLiveSrcBase::doGetParam(VO_U32 nParamID, VO_PTR pValue)
{
	return VO_ERR_LIVESRC_INVALIDARG;
}
	
VO_U32 CLiveSrcBase::doSetParam(VO_U32 nParamID, VO_PTR pValue)
{
	return VO_ERR_LIVESRC_INVALIDARG;
}


///////////////////////////////////////////////////////////////////////////////////


VO_U32 CLiveSrcBase::FindChannel(VO_LIVESRC_FORMATTYPE type)
{
	CChannelInfo* p = m_aChannelInfo;
	for ( VO_U32 i = 0; i < m_cChannel; i++)
	{
		if (p->nType == type)
			return i;
		p++;
	}
	return VO_ERR_LIVESRC_FAIL;
}


VO_VOID CLiveSrcBase::SendStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	m_fStatus(m_pUserData, nID, nParam1, nParam2);
}


VO_VOID CLiveSrcBase::SendData(VO_U32 nOutputType, VO_PBYTE pData, VO_U32 nDataLen, VO_U64 llTimeStamp, VO_U32 nCodecType, VO_U32 nStreamId, VO_BOOL bKeyFrame)
{
	if(nOutputType == VO_LIVESRC_OUTPUT_AUDIO || 
		nOutputType == VO_LIVESRC_OUTPUT_VIDEO)
	{
		VO_SOURCE_SAMPLE sample;
		sample.Buffer	= pData;
		sample.Duration	= 0;
		sample.Time		= llTimeStamp;

		//if(bKeyFrame)
			//nDataLen |= 0x80000000;

		VO_LIVESRC_SAMPLE av_sample;
		cmnMemSet(0, &av_sample, 0, sizeof(VO_LIVESRC_SAMPLE));
		av_sample.nCodecType		= nCodecType;
		av_sample.nTrackID			= nStreamId;
		av_sample.Sample.Buffer		= pData;
		av_sample.Sample.Duration	= 0;
		av_sample.Sample.Time		= llTimeStamp;
		av_sample.Sample.Size		= nDataLen;
		
		if((VO_LIVESRC_OUTPUT_VIDEO == nOutputType) && (nCodecType != m_nVideoType))
		{
			//m_bRecvFirstVideoFrame = VO_TRUE;
			m_nVideoType = nCodecType;
			av_sample.Sample.Flag		|= VOMP_FLAG_BUFFER_NEW_FORMAT;
		}
		else if((VO_LIVESRC_OUTPUT_AUDIO == nOutputType) && (nCodecType != m_nAudioType))
		{
			//m_bRecvFirstVideoFrame = VO_TRUE;
			m_nAudioType = nCodecType;
			av_sample.Sample.Flag		|= VOMP_FLAG_BUFFER_NEW_FORMAT;
		}

		m_fSendData(m_pUserData, (VO_U16)nOutputType, &av_sample);
	}
	else if(nOutputType == VO_LIVESRC_OUTPUT_DATABLOCK)
	{
		VO_LIVESRC_DATA_SERVICE data_service;
		data_service.pData		= pData;
		data_service.nDataLen	= nDataLen;
		data_service.nReserved	= 0;
		m_fSendData(m_pUserData, (VO_U16)nOutputType, &data_service);
	}
	else if(nOutputType == VO_LIVESRC_OUTPUT_TRACKINFO)
	{
		m_fSendData(m_pUserData, (VO_U16)nOutputType, pData);
	}
}

VO_VOID CLiveSrcBase::SendScanProgress(VO_U32 nParam1, VO_U32 nParam2)
{
	SendStatus(VO_LIVESRC_STATUS_SCAN_PROCESS, nParam1, nParam2);
}

VO_VOID CLiveSrcBase::SendChannelStopped(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType)
{
	SendStatus(VO_LIVESRC_STATUS_CHANNEL_STOP, oldType, newType);
}

VO_VOID CLiveSrcBase::SendChannelStarted(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType)
{
	SendStatus(VO_LIVESRC_STATUS_CHANNEL_START, oldType, newType);
}

VO_VOID CLiveSrcBase::SendProgramChanged(VO_U32 oldProgramID, VO_U32 newProgramID)
{
	SendStatus(VO_LIVESRC_STATUS_CODEC_CHANGED, oldProgramID, newProgramID);
}

VO_U16	CLiveSrcBase::GetChannelNum (void)
{
	return (VO_U16)m_cChannel;
}



VO_U32 CLiveSrcBase::RecordStart (VO_PTCHAR pPath)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)pPath;
	fileSource.nMode = VO_FILE_WRITE_ONLY;
	m_fRecording = cmnFileOpen(&fileSource);
	if (m_fRecording)
		return VO_ERR_LIVESRC_OK;
	else 
		return VO_ERR_LIVESRC_FAIL;
}

VO_U32  CLiveSrcBase::RecordWrite(VO_PBYTE pData, VO_U32 nDataLen)
{
	if (!m_fRecording)
		return VO_ERR_LIVESRC_FAIL;

	VO_U32 dwWrited = cmnFileWrite (m_fRecording, pData, nDataLen);

	return dwWrited==nDataLen?VO_ERR_LIVESRC_OK:VO_ERR_LIVESRC_FAIL;
}

VO_U32 CLiveSrcBase::OnRecvData(VO_PARSER_INPUT_BUFFER* pRecvedData)
{
	VO_U32 rc = VO_ERR_LIVESRC_FAIL;
	
	RecordWrite(pRecvedData->pBuf, pRecvedData->nBufLen);

	if (m_pParser)
	{
		rc = m_pParser->Process(pRecvedData);
	}
	
	return rc;
}

VO_U32 CLiveSrcBase::RecordEnd ()
{
	if (!m_fRecording)
		return VO_ERR_LIVESRC_FAIL;
	cmnFileClose(m_fRecording);
	m_fRecording = NULL;
	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcBase::GetSignalStrength () 
{ 
	return 0; 
}

void OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	((CLiveSrcBase*)(pData->pUserData))->OnParsedData(pData);
}

VO_BOOL CLiveSrcBase::doOnParsedData(VO_PARSER_OUTPUT_BUFFER * pData)
{
	return VO_FALSE;
}

void CLiveSrcBase::OnParsedData(VO_PARSER_OUTPUT_BUFFER* pData)
{
	// give subclass a changce to process it at first, return it if subclass processed
	if (doOnParsedData(pData))
	{
		return;
	}

	switch(pData->nType)
	{
	case VO_PARSER_OT_STREAMINFO :
		{
			VO_PARSER_STREAMINFO* pInfo = (VO_PARSER_STREAMINFO*)pData->pOutputData;

			VO_LIVESRC_TRACK_INFO	track_info;
			cmnMemSet(0, &track_info, 0, sizeof(VO_LIVESRC_TRACK_INFO));

			if (pInfo->pVideoExtraData && pInfo->nVideoExtraSize>0)
			{
				track_info.nTrackCount++;
			}
			if (pInfo->pAudioExtraData && pInfo->nAudioExtraSize>0)
			{
				track_info.nTrackCount++;
			}

			if(track_info.nTrackCount == 0)
				return;
			
			VO_SOURCE_TRACKINFO*	pTrack		= new VO_SOURCE_TRACKINFO[2];
			VO_U32*					pTrackID	= new VO_U32[2];

			// NOTE: need to refactor to support multiple tracks
			VO_U32 track_id = 0;
			if (pInfo->pVideoExtraData && pInfo->nVideoExtraSize>0)
			{
				VOLOGI("Video codec %d", pInfo->nVideoCodecType);
				pTrackID[0]			= track_id++;

				pTrack[0].Codec		= pInfo->nVideoCodecType;
				pTrack[0].HeadData	= (VO_BYTE*)pInfo->pVideoExtraData;
				pTrack[0].HeadSize	= pInfo->nVideoExtraSize;
				pTrack[0].Type		= VO_SOURCE_TT_VIDEO;
				pTrack[0].Duration	= 0;
				pTrack[0].Start		= 0;
			}
			if (pInfo->pAudioExtraData && pInfo->nAudioExtraSize>0)
			{
				VOLOGI("Audio codec %d", pInfo->nAudioCodecType);
				pTrackID[1]			= track_id++;

				pTrack[1].Codec		= pInfo->nAudioCodecType;
				pTrack[1].HeadData	= (VO_BYTE*)pInfo->pAudioExtraData;
				pTrack[1].HeadSize	= pInfo->nAudioExtraSize;
				pTrack[1].Type		= VO_SOURCE_TT_AUDIO;
				pTrack[1].Duration	= 0;
				pTrack[1].Start		= 0;
			}

			track_info.pInfo	= pTrack;
			track_info.pTrackID	= pTrackID;
			
			//SendData(VO_LIVESRC_OUTPUT_TRACKINFO, (VO_PBYTE)&track_info,sizeof(VO_LIVESRC_TRACK_INFO), 0 , 0, 0,VO_FALSE);

			if(pInfo->pVideoExtraData && pInfo->nVideoExtraSize>0)
			{
				VO_MTV_FRAME_BUFFER buffer;
				memset(&buffer, 0, sizeof(VO_MTV_FRAME_BUFFER));
				buffer.pData = (VO_PBYTE)pInfo->pVideoExtraData;
				buffer.nSize = pInfo->nVideoExtraSize;
				
				SendData(VO_LIVESRC_OUTPUT_VIDEO, buffer.pData, buffer.nSize, 0, pInfo->nVideoCodecType, 1, buffer.nFrameType==0?VO_TRUE:VO_FALSE);
			}
			
			if(pInfo->pAudioExtraData && pInfo->nAudioExtraSize>0)
			{
				VO_MTV_FRAME_BUFFER buffer;
				memset(&buffer, 0, sizeof(VO_MTV_FRAME_BUFFER));
				buffer.pData = (VO_PBYTE)pInfo->pAudioExtraData;
				buffer.nSize = pInfo->nAudioExtraSize;
				
				//SendData(VO_LIVESRC_OUTPUT_AUDIO, buffer.pData, buffer.nSize, 0, pInfo->nAudioCodecType, 1, buffer.nFrameType==0?VO_TRUE:VO_FALSE);
			}
			
			//VO_MTV_FRAME_BUFFER * pFrame	= (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
			//SendData(VO_LIVESRC_OUTPUT_VIDEO, pFrame->pData, pFrame->nSize, pFrame->nStartTime, pFrame->nCodecType, 1, pFrame->nFrameType==0?VO_TRUE:VO_FALSE);
			
			delete []pTrackID;
			delete []pTrack;

			m_bStreamInfoReady = VO_TRUE;
		}
		break;
	case VO_PARSER_OT_AUDIO :
		{
			if(m_bStreamInfoReady)
			{
				VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
				SendData(VO_LIVESRC_OUTPUT_AUDIO, pFrame->pData, pFrame->nSize, pFrame->nStartTime, pFrame->nCodecType, 0, pFrame->nFrameType==0?VO_TRUE:VO_FALSE);
			}
		}
		break;
	case VO_PARSER_OT_VIDEO :
		{
			if(m_bStreamInfoReady)
			{
				VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
				
				if (VO_FALSE == m_bFirstKeyFSend) {
					
					if ((NULL != pFrame) && (0 == pFrame->nFrameType)) {
						m_bFirstKeyFSend = VO_TRUE;
					}
				}
				
				if (VO_TRUE == m_bFirstKeyFSend) {
					m_bFirstKeyFSend = VO_TRUE;
					
					SendData(VO_LIVESRC_OUTPUT_VIDEO, pFrame->pData, pFrame->nSize, pFrame->nStartTime, pFrame->nCodecType, 1, pFrame->nFrameType==0?VO_TRUE:VO_FALSE);
				
#if 0
					// test code
					static VO_U64 last_keyframe = 0;
					if(pFrame->nFrameType == 0)
					{
						if(last_keyframe != 0)
							VOLOGI("Video keyframe interval = %d", pFrame->nStartTime - last_keyframe);
						last_keyframe = pFrame->nStartTime;
					}
					// test code
#endif
				}
			}
		}
		break;
	default:
		{
			//VOLOGI("A parser callback not process!!!");
		}
		break;
	}
}

VO_VOID CLiveSrcBase::NotifyStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	if (m_fStatus)
	{
		m_fStatus(m_pUserData, nID, nParam1, nParam2);
	}
}



