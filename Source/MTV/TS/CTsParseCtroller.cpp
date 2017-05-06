
#include "CTsParseCtroller.h"
#include "voLog.h"
#include "framechk.h"
#include "CStream.h"
#include "voOSFunc.h"
#include "CBaseTrack.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CTsParseCtroller::CTsParseCtroller(void)
: CvoBaseMemOpr(VO_NULL)
, m_pParser(VO_NULL)
, m_bSelectProgram(VO_FALSE)
, m_nParseType(PARSE_ALL)
{
	m_nStreamCount = 0;
	for (VO_U16 n=0; n<MAX_STREAM_COUNT; n++)
	{
		m_pStream[n] = VO_NULL;
	}

	m_pBufferForPCM = new uint8[32768];
	m_ulPcmBufferMaxLen = 32768;
	memset(m_pBufferForPCM, 0, 32768);
	m_ulPcmBufferLen = 0;
	m_ullTimeForPCM = 0;
	
	m_ulMPEGFrameCount = 0;
	m_ullLastTimeStamp = 0;
    m_ulOpenFlag = 0;
    m_bVideoTrackReady = VO_FALSE;
}

CTsParseCtroller::~CTsParseCtroller(void)
{
	if(m_pBufferForPCM != NULL)
	{
		delete[] m_pBufferForPCM;
		m_ulPcmBufferLen = 0;
	}
	Close();
}


VO_U32 CTsParseCtroller::Open(PARSERPROC pProc, VO_VOID* pUserData)
{
	CDumper::StartLogOutput();

	Close();

	m_pProc		= pProc;
	m_pUserData	= pUserData;

	// tag: 20100427
	// tag: 20101019 lin
	m_nParseType = PARSE_PLAYBACK;
	//m_nParseType = PARSE_PLAYBACK2;
	//end

	m_pParser	= new CTSParser(m_nParseType);
    m_pParser->SetOpenFlag(m_ulOpenFlag);
	m_pParser->SetListener(this);
	
	// tag: 20100420
	//m_pParser->Reset();
	// end

	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParseCtroller::Close()
{
	CDumper::WriteLog((char *)"Close CTsParseCtroller");

	if (m_pParser)
	{
		m_pParser->WaitParseFinish();
		delete m_pParser;
		m_pParser = VO_NULL;
	}

	m_pProc		= VO_NULL;
	m_pUserData	= VO_NULL;

	m_nStreamCount = 0;
	for (VO_U16 n=0; n<MAX_STREAM_COUNT; n++)
	{
		if (m_pStream[n])
		{
			delete m_pStream[n];
			m_pStream[n] = VO_NULL;
		}
	}

	m_bParsedProgramInfo = VO_FALSE;
	m_nTimeStampOffset = -1;

	m_ulPcmBufferLen = 0;

	return VO_ERR_PARSER_OK;
}

VO_VOID	CTsParseCtroller::OnStreamChanged()
{
	if (m_pParser)
	{
		m_pParser->WaitParseFinish();
		delete m_pParser;
		m_pParser = VO_NULL;
	}

	m_nStreamCount = 0;
	for (VO_U16 n=0; n<MAX_STREAM_COUNT; n++)
	{
		if (m_pStream[n])
		{
			delete m_pStream[n];
			m_pStream[n] = VO_NULL;
		}
	}
	m_bParsedProgramInfo = VO_FALSE;

	// open it again
	
	m_ulPcmBufferLen = 0;

	m_nParseType = PARSE_PLAYBACK;
	m_pParser	= new CTSParser(m_nParseType);
	m_pParser->SetOpenFlag(m_ulOpenFlag);
	m_pParser->SetListener(this);

	m_nVideoTimeStampOffset = -1;
	m_nAudioTimeStampOffset = -1;
	m_nTimeStampOffset = -1;
}

VO_U32 CTsParseCtroller::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	// just for LIVE stream dumper
	CDumper::DumpRawData(pBuffer->pBuf, pBuffer->nBufLen);

// 	VO_U32 t = voOS_GetSysTime();

	// tag: 20100609
	//voCAutoLock lock(&m_Mutex);

	if (m_pParser)
	{
		if (pBuffer->nFlag & VO_PARSER_FLAG_STREAM_CHANGED)
		{
			// reset
			CDumper::WriteLog((char *)"Stream changed!!!!");
			OnStreamChanged();

// 			CDumper::CloseAllDumpFile();
// 			m_pStream[0]->SetStreamID(257);
// 			m_pStream[1]->SetStreamID(258);
		}

		if(pBuffer->nFlag & VO_PARSER_FLAG_STREAM_RESET_ALL)
		{
			CDumper::WriteLog((char *)"Stream changed!!!!");
			OnStreamChanged();
			VOLOGE("Reset the tsparser!");
		}

		int processed = 0;
		m_pParser->Process(pBuffer->nStreamID, pBuffer->pBuf, pBuffer->nBufLen, &processed);
	}

// 	t = voOS_GetSysTime() - t;
// 	CDumper::WriteLog("Process use time = %d, size = %d", t, pBuffer->nBufLen);

	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParseCtroller::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (VO_PID_PARSER_SELECT_PROGRAM == uID)
	{
		SelectProgram(*(VO_U32*)pParam);
		return VO_ERR_PARSER_OK;
	}
	else if (VO_PID_PARSE_SET_PARSER_TYPE == uID)
	{
		SetParseType(*(VO_U32*)pParam);
	}
	else if(VO_PID_DO_TS_FORCE_PLAY == uID)
	{
        VOLOGI("do the TsParser Force Play!");
		m_pParser->DoForcePlayAction();
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CTsParseCtroller::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if(_CHECK_TS_PLAYBACK_READY == uID)
	{
		int *pResult = (int *)pParam;
		*pResult = m_pParser->IsPlaybackReady();
		return VO_ERR_PARSER_FAIL;
	}
	else
	{
	    return VO_ERR_PARSER_FAIL;
	}
}

VO_U32 CTsParseCtroller::SelectProgram(VO_U32 uProgramID)
{
	if(m_pParser)
	{
		m_bSelectProgram	= VO_TRUE;
		m_nStreamID			= uProgramID;
		VOSTREAMPARSERETURNCODE ret = m_pParser->SelectProgram(uProgramID);
		return VORC_STREAMPARSE_OK==ret?VO_ERR_PARSER_OK:VO_ERR_PARSER_FAIL;
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CTsParseCtroller::SetParseType(VO_U32 nType)
{
	m_nParseType = nType;

	// tag: 20100423
#ifdef CHANGE_PARSE_TYPE
	if (m_pParser)
	{
		m_pParser->SetParserType(nType);
	}
#endif
	//end
	
	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParseCtroller::ResetState()
{
	for (VO_U8 n=0; n<m_nStreamCount; n++)
	{
		if (m_pStream[n])
		{
			m_pStream[n]->Reset();
		}
	}

	if (m_pParser)
	{
		m_pParser->Reset2();
	}

	return VO_ERR_PARSER_OK;
}

VO_BOOL CTsParseCtroller::IsProgramInfoParsed()
{
	return m_bParsedProgramInfo;
}

VO_U8 CTsParseCtroller::GetStreamCount()
{
	return (VO_U8)m_nStreamCount;
}

CStream* CTsParseCtroller::GetStreamByIdx(VO_U32 nIdx)
{
	if(nIdx > (VO_U32)(m_nStreamCount-1))
		return VO_NULL;

	return m_pStream[nIdx];
}

CStream* CTsParseCtroller::GetStream(VO_U32 nStreamID)
{
	for (VO_U16 n=0; n<m_nStreamCount; n++)
	{
		if (m_pStream[n])
		{
			if(m_pStream[n]->GetStreamID() == nStreamID)
				return m_pStream[n];
		}
	}

	return VO_NULL;
}




/***************************************
interface from VOSTREAMPARSELISTENER
****************************************/

void CTsParseCtroller::OnProgramIndex(VOSTREAMPARSEPROGRAMINDEX* pProgramIndex)
{
	CDumper::WriteLog((char *)"OnProgramIndex");

	// needn't process it
}

void CTsParseCtroller::OnNewProgram(VOSTREAMPARSEPROGRAMINFO* pProgramInfo)
{
	CDumper::WriteLog((char *)"OnNewProgram");

	m_bParsedProgramInfo = VO_TRUE;

	// need issue it, app need select a program in callback function
	m_bSelectProgram = VO_FALSE;
	//IssueParseResult(VO_PARSER_OT_STREAMINFO, pProgramInfo);
	IssueParseResult(VO_PARSER_OT_TS_PROGRAM_INFO, pProgramInfo);

	// app need select a program, but if app doesn't do that, we should set default program.
	if (!m_bSelectProgram)
	{
		SelectProgram(pProgramInfo->id);
	}
}

void CTsParseCtroller::OnNewProgramEnd()
{
}

void CTsParseCtroller::OnNewStream(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CDumper::WriteLog((char *)"OnNewStream");

	m_pStream[m_nStreamCount] = new CStream(pStreamInfo);
    if(m_pStream[m_nStreamCount] == NULL || m_pStream[m_nStreamCount]->GetFrameBuf() == NULL)
	{
        VOLOGE("lack of memory! may be serious wrong happen!");
		if(m_pStream[m_nStreamCount] != NULL)
		{
			delete m_pStream[m_nStreamCount];
		}
		return;
	}

	m_nStreamCount++;

	VO_PARSER_STREAMINFO info;
	MemSet(&info, 0, sizeof(VO_PARSER_STREAMINFO));

	//for (VO_U16 n=0; n<m_nStreamCount; n++)
	{
		CStream* pStream = m_pStream[m_nStreamCount-1];

		if (pStream)
		{
			VO_CODECBUFFER head;
			pStream->GetHeadData(&head);

			if (pStream->IsVideo())
			{
				CDumper::DumpVideoHeadData(head.Buffer, head.Length);
			//	info.beVideo = VO_TRUE;
				info.nVideoCodecType	= pStream->GetCodec();
				info.pVideoExtraData	= head.Buffer;
				info.nVideoExtraSize	= (VO_U16)head.Length;
				info.VideoFormat.height = pStreamInfo->video.height;
				info.VideoFormat.width = pStreamInfo->video.width;
				info.ulStreamId = pStreamInfo->id;
				info.eMediaType = VO_PARSER_MEDIA_TYPE_EX_VIDEO;
				info.pVideoClosedCaptionDescData = pStreamInfo->pClosedCaptionDescData;
				info.nVideoClosedCaptionDescDataLen = (VO_U16)pStreamInfo->iClosedCaptionDescLen;

				m_bVideoTrackReady = VO_TRUE;
			}
			else if(pStream->GetTrackType() == VOTT_AUDIO)
			{
			//	info.beVideo = VO_FALSE;
				info.nAudioCodecType	= pStream->GetCodec();
				info.pAudioExtraData	= head.Buffer;
				info.nAudioExtraSize	= (VO_U16)head.Length;
				info.AudioFormat.channels = pStreamInfo->audio.channels;
				info.AudioFormat.sample_rate = pStreamInfo->audio.sample_rate;
				info.AudioFormat.sample_bits = pStreamInfo->audio.sample_bits;
				memcpy(info.AudioFormat.strLanguage, pStreamInfo->audio.audio_language, strlen(pStreamInfo->audio.audio_language));
				info.ulStreamId = pStreamInfo->id;
				info.eMediaType = VO_PARSER_MEDIA_TYPE_EX_AUDIO;
			}
			else if(pStream->GetTrackType() == VOTT_TEXT)
			{
				info.nSubTitleCodecType	= pStream->GetCodec();
				info.pSubTitleExtraData	= head.Buffer;
				info.nSubTitleExtraSize	= (VO_U16)head.Length;
				memcpy(info.SubTitleFormat.strLanguage, pStreamInfo->subtitle.subtitle_language, strlen(pStreamInfo->subtitle.subtitle_language));
				info.ulStreamId = pStreamInfo->id;
				info.eMediaType = VO_PARSER_MEDIA_TYPE_EX_TEXT;
			}
			else if(pStream->GetTrackType() == VOTT_METADATA)
			{
				info.ulStreamId = pStreamInfo->id;
				info.eMediaType = VO_PARSER_MEDIA_TYPE_EX_PRIVATE_DATA;
			}
			IssueParseResult(VO_PARSER_OT_STREAMINFO, &info);
		}
	}
}
void CTsParseCtroller::OnStreamChanged(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CDumper::WriteLog((char *)"OnStreamChanged");
}

void CTsParseCtroller::OnNewStreamEnd()
{
	CDumper::WriteLog((char *)"OnNewStreamEnd");

// 	VO_PARSER_STREAMINFO info;
// 
// 	for (VO_U16 n=0; n<m_nStreamCount; n++)
// 	{
// 		CStream* pStream = m_pStream[n];
// 
// 		if (pStream)
// 		{
// 			VO_CODECBUFFER head;
// 			pStream->GetHeadData(&head);
// 
// 			if (pStream->IsVideo())
// 			{
// 				info.nVideoCodecType	= pStream->GetCodec();
// 				info.pVideoExtraData	= head.Buffer;
// 				info.nVideoExtraSize	= head.Length;
// 			}
// 			else
// 			{
// 				info.nAudioCodecType	= pStream->GetCodec();
// 				info.pAudioExtraData	= head.Buffer;
// 				info.nAudioExtraSize	= head.Length;
// 			}
// 		}
// 	}
// 
// 	IssueParseResult(VO_PARSER_OT_STREAMINFO, &info);
}

VO_BYTE* CTsParseCtroller::GetFrameBuf(int nStreamId)
{
	CStream* pStream = GetStream(nStreamId);
	if (pStream)
	{
		return pStream->GetFrameBuf();
	}

	return NULL;
}

VO_U32 CTsParseCtroller::GetBufferSize(int nStreamId)
{
	CStream* pStream = GetStream(nStreamId);
	if (pStream)
	{
		return pStream->GetBufferSize();
	}

	return 0;
}


VO_U32 CTsParseCtroller::SetNewBufSize(int nStreamId, VO_U32  ulNewBufferSize)
{
	CStream* pStream = GetStream(nStreamId);
	if (pStream)
	{
        pStream->SetNewFrameSize(ulNewBufferSize);
	}

	return 0;
}

VO_U32 CTsParseCtroller::SetNewTimeStamp(int nStreamId, VO_S64  illTimeStamp)
{
	CStream* pStream = GetStream(nStreamId);
	if (pStream)
	{
		pStream->SetNewTimeStamp(illTimeStamp);
	}

	return 0;
}

void    CTsParseCtroller::SetEITCallbackFun(void*  pFunc)
{
	if(m_pParser != NULL)
	{
		m_pParser->SetEITCallbackFun(pFunc);
	}
}


void CTsParseCtroller::OnNewFrame(int nStreamId, VOSTREAMPARSEFRAMEINFO* pFrameInfo)
{
	//VOLOGI("OnNewFrame: %s", GetStream(nStreamId)->IsVideo()?"video":"audio");

	CStream* pStream = GetStream(nStreamId);

	if (pStream)
	{

		IssueFrame(pStream);		
		pStream->OnNewFrame(pFrameInfo);
		if (-1 != pStream->GetTimeStamp())
		{
			SetTimeStampOffset(pStream->GetTimeStamp());
		}
	}
}

VO_VOID CTsParseCtroller::IssueFrame(CStream* pStream)
{
    VO_U32   ulTrackMediaType = 0;
	if(!pStream)
		return;

	int sizes[128] = {0};
	int count = 0;
	VO_U64 time_stamp = 0;

	VO_BYTE* pOutBuf = VO_NULL;
	
	if(pStream->OnFrameEnd(sizes, &count, &pOutBuf, &time_stamp))
	{
		VO_BYTE* buf_addr = pOutBuf;
		VO_MTV_FRAME_BUFFER		frame_buf;
		VO_PARSER_OUTPUT_BUFFER output_buf;


        if(pStream->IsVideo() && pStream->GetCodec() == VO_VIDEO_CodingMPEG2)
		{
			if( (m_ulMPEGFrameCount > 0) && (time_stamp == 0xFFFFFFFFFFFFFFFELL))
			{
				time_stamp = m_ullLastTimeStamp + 25; //add 25 ms
			}

			m_ulMPEGFrameCount++;
			m_ullLastTimeStamp = time_stamp;
		}


		if(count >= MAX_SPLIT_FRAME_COUNT_IN_PES)
		{
			VOLOGE("something error! the data maybe dirty");
			return;
		}
		if (m_nVideoTimeStampOffset==-1 && pStream->IsVideo())
		{
			m_nVideoTimeStampOffset = time_stamp;
			CDumper::WriteLog((char *)"++++++Video first frame ts = %llu++++++", time_stamp);
		}
		else if(m_nAudioTimeStampOffset==-1 && !pStream->IsVideo())
		{
			// tag: 20100609
// 			if(time_stamp == 0)
// 				return;
			// end

			m_nAudioTimeStampOffset = time_stamp;

			CDumper::WriteLog((char *)"++++++Audio first frame ts = %llu++++++", time_stamp);
		}


		for (int n=0; n<count; n++)
		{
			MemSet(&output_buf, 0, sizeof(VO_PARSER_OUTPUT_BUFFER));
			
			ulTrackMediaType = pStream->GetTrackType();
			switch(ulTrackMediaType)
			{
			    case VOTT_VIDEO:
			    {
                    output_buf.nType = VO_PARSER_OT_VIDEO;
					break;
				}
				case VOTT_AUDIO:
				{
					output_buf.nType = VO_PARSER_OT_AUDIO;
					break;
				}
				case VOTT_TEXT:
				{
					output_buf.nType = VO_PARSER_OT_TEXT;
					break;
				}
				case VOTT_METADATA:
				{
					output_buf.nType = VO_PARSER_OT_PRIVATE_DATA;
					break;
				}
				default:
				{
                    VOLOGI("unknown Media Type!");
					return;
				}
			}

			output_buf.nStreamID	= pStream->GetStreamID();
			output_buf.pOutputData	= &frame_buf;

			MemSet(&frame_buf, 0, sizeof(VO_MTV_FRAME_BUFFER));
			frame_buf.nCodecType	= pStream->GetCodec();

#ifdef _USE_RELATIVE_TIME
			frame_buf.nStartTime	= time_stamp;
#else
			frame_buf.nStartTime	= CheckTimestamp(time_stamp, pStream->IsVideo()?VO_TRUE:VO_FALSE);
#endif


		    frame_buf.nFrameType	= pStream->IsSync(buf_addr, sizes[n])?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_P;
		    frame_buf.nPos			= VO_MTV_FRAME_POS_WHOLE;
		    frame_buf.nSize			= sizes[n];
			frame_buf.pData			= buf_addr;

#if 1

			if (frame_buf.nFrameType == VO_VIDEO_FRAME_I && pStream->IsVideo())
			{
				//CDumper::WriteLog("I frame detected... ts = %08llu, size = %d", frame_buf.nStartTime, frame_buf.nSize);
			}

			//if (pStream->GetStreamID() == 258)		// apple file
			if (!pStream->IsVideo())	// brazil adts file
			{
				//CDumper::WriteLog("Audio data len = %d -> %x %x %x %x", sizes[n], buf_addr[0], buf_addr[1], buf_addr[2], buf_addr[3]);
			}
			else
			{
				//CDumper::WriteLog("Video data len = %d -> %x %x %x %x", sizes[n], buf_addr[0], buf_addr[1], buf_addr[2], buf_addr[3]);
			}
#endif
			m_nStreamID = pStream->GetStreamID();


			if((!pStream->IsVideo()) && pStream->GetCodec() == VO_AUDIO_CodingPCM)
			{
				if((m_ulPcmBufferLen+sizes[n]) < 32678)
				{
					memcpy(m_pBufferForPCM+m_ulPcmBufferLen, buf_addr, sizes[n]);
					m_ulPcmBufferLen += sizes[n];
				}

				if((m_ulPcmBufferLen > 8192) && 
					(m_ullTimeForPCM == 0))
				{
					m_ullTimeForPCM = frame_buf.nStartTime;
				}


				if((m_ulPcmBufferLen>16384) && (m_ulPcmBufferLen<32678))
				{
					frame_buf.nFrameType	= pStream->IsSync(buf_addr, sizes[n])?VO_VIDEO_FRAME_I:VO_VIDEO_FRAME_P;
					frame_buf.nPos			= VO_MTV_FRAME_POS_WHOLE;
					frame_buf.nSize			= m_ulPcmBufferLen;
					frame_buf.pData			= m_pBufferForPCM;
					frame_buf.nStartTime    = m_ullTimeForPCM;
					m_ulPcmBufferLen = 0;
					m_ullTimeForPCM = 0;
					IssueParseResult(output_buf.nType, &frame_buf);
				}
			}
			else
			{
				IssueParseResult(output_buf.nType, &frame_buf);
			}

			buf_addr += sizes[n];

			//CDumper::WriteLog("%s ts = %u", pStream->IsVideo()?"video":"audio", time_stamp);
		}
	}
	else
	{
		// discard samples
		// tag: 20100609
		//CDumper::WriteLog("discard frame...");
	}

	// 20100617
	//pStream->Reset();
}

VO_U64	CTsParseCtroller::CheckTimestamp(VO_U64 time, VO_BOOL bVideo)
{
	VO_U64 t = time;

	if (bVideo)
		t = m_nVideoTimeStampOffset;
	else
		t = m_nAudioTimeStampOffset;

	if(time < t)
		return 0;

	return time - t;
}

void CTsParseCtroller::OnFrameData(int nStreamId, void* pData, int nSize)
{
	//VOLOGI("OnFrameData");

	CStream* pStream = GetStream(nStreamId);

	if (pStream)
	{
		pStream->OnFrameData(pData, nSize);
	}
}

void CTsParseCtroller::OnFrameEnd(int nStreamId)
{
	CDumper::WriteLog((char *)"OnFrameEnd");

}

void CTsParseCtroller::OnFrameError(int nStreamId, int nErrorCode)
{
	CDumper::WriteLog((char *)"OnFrameError");

}

void  CTsParseCtroller::SetOpenFlag(VO_U32 ulOpenFlag)
{
    m_ulOpenFlag = ulOpenFlag;
}

VO_BOOL  CTsParseCtroller::IsVideoTrackReady()
{
	return m_bVideoTrackReady;
}


