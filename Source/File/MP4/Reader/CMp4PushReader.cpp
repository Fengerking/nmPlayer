#include "CMp4PushReader.h"
#ifdef _SUPPORT_PARSER_METADATA
#include "CMP4MetaData.h"
#endif	//_SUPPORT_PARSER_METADATA

#include "voLog.h"
#include "voLiveSource.h"
#include "SegmentTrack.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const double TIME_UNIT = 1.0;	//all time multiply this value is MS


CMp4PushReader::CMp4PushReader()
	: CBaseReader(NULL, NULL, NULL, NULL)
	, m_pReader(VO_NULL)
	, m_pStream(VO_NULL)
	, m_pMovie(VO_NULL)
	,m_trackInfo(NULL)
	,m_nDefaultSize_Video(0)
	,m_nDefaultDuration_Video(0)
	,m_nDefaultSize_Audio(0)
	,m_nDefaultDuration_Audio(0)
	,m_u32TimeScale(1)
	,m_pBuf_x(NULL)
	,m_bBufNew(VO_FALSE)
{

}

CMp4PushReader::~CMp4PushReader()
{
	Close();
}

VO_U32 CMp4PushReader::Open(PARSERPROC pProc, VO_VOID* pUserData)
{
	//m_bProcDownloadEnd = VO_FALSE;
	m_pProc		= pProc;
	m_pUserData	= pUserData;
	return VO_ERR_NONE;
}
VO_U32 CMp4PushReader::CreateStream(VO_U32 nFlag,VO_PBYTE pBuf, VO_U32 nSize)
{	
	m_pBuf_x = pBuf;
	m_bBufNew = VO_FALSE;
	VO_U32 size = nSize;
	if(nFlag == VO_PARSER_FLAG_STREAM_DASHHEAD)
	{

		if(pBuf[4] == 'f'&&pBuf[5] == 't'&&pBuf[6] == 'y'&&pBuf[7] == 'p')
		{	
			VO_U32 ftypSize = pBuf[0]<<24 | pBuf[1]<<16 | pBuf[2]<<9 |pBuf[3];
			if(ftypSize >24)
			{
				pBuf[20] = 'd';
				pBuf[21] = 'a';
				pBuf[22] = 's';
				pBuf[23] = 'h';
			}
			else
			{
				pBuf[8] = 'd';
				pBuf[9] = 'a';
				pBuf[10] = 's';
				pBuf[11] = 'h';
			}
		}
		else
		{
			//for Hamonic, no ftyp box
			size +=16;
			m_pBuf_x = new VO_BYTE[size+1];
			m_pBuf_x[0] = 0x00;
			m_pBuf_x[1] = 0x00;
			m_pBuf_x[2] = 0x00;
			m_pBuf_x[3] = 0x10;
			m_pBuf_x[4] = 0x66;
			m_pBuf_x[5] = 0x74;
			m_pBuf_x[6] = 0x79;
			m_pBuf_x[7] = 0x70;
			m_pBuf_x[8] = 0x64;
			m_pBuf_x[9] = 0x61;
			m_pBuf_x[10] = 0x73;
			m_pBuf_x[11] = 0x68;
			m_pBuf_x[12] = 0x00;
			m_pBuf_x[13] = 0x00;
			m_pBuf_x[14] = 0x00;
			m_pBuf_x[15] = 0x00;
			memcpy(m_pBuf_x+ 16, pBuf,nSize);
			m_bBufNew =VO_TRUE;
		}
	}

	m_pStream = new CMp4PushFileStream(NULL);
	if(!m_pStream)
		return VO_ERR_OUTOF_MEMORY;
	if(!m_pStream->Open(m_pBuf_x, size))
		return VO_ERR_SOURCE_OPENFAIL;
	m_ullFileSize = m_pStream->Size();
	m_pReader = new ReaderMSB(m_pStream, m_ullFileSize);
	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	m_u32TimeScale = 0;
	m_u64Starttime = 0;
	m_u64Starttime = *(VO_S64*)pBuffer->pReserved;
	m_u32StreamID = pBuffer->nStreamID;
	VO_U32 rc = VO_ERR_SOURCE_OK;

	Close();
	
	CreateStream(pBuffer->nFlag, pBuffer->pBuf, pBuffer->nBufLen);

	
	if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHHEAD)
	{
		VOLOGR("+Parser Head.")
		rc = ParserHead(pBuffer->pBuf,pBuffer->nBufLen);	
		VOLOGR("-Parser Head.")
	}
	else if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_DASHDATA)
	{
		VOLOGR("+Parser data.")
		rc = ParserData(pBuffer->pBuf,pBuffer->nBufLen);
		VOLOGR("-Parser data.")
	}
	
	return rc;
}

CBaseTrack* CMp4PushReader::GetTrackByIndex(VO_U32 nIndex)
{
	CBaseTrack* track = CBaseReader::GetTrackByIndex(nIndex);
	if(track && track->IsInUsed())
		return track;
	
	return VO_NULL;
}
int a = 0;
int v = 0;
VO_U32 CMp4PushReader::ParserData(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	
	SegmentsBox sb;
	if(sb.Init(m_pReader, -2) != MP4RRC_OK)
		return VO_ERR_SOURCE_OPENFAIL;

	VO_U32 type = (m_u32StreamID == 1? VO_PARSER_OT_AUDIO: VO_PARSER_OT_VIDEO);
	
	int count = sb.GetSampleCount();
	int index = 0;
	uint32 curSize =0 ;
	unsigned char *pBuffer = NULL;
	uint64 sssize = 0;
	unsigned char *oneChunkBuffer = NULL;
	uint32 oneChunkSize = 0;
	uint64 ts = 0;
	bool bUseDefaultSize = false;
	bool bUseDefaultDuration = false;
	ABSPOS pos = 0;
	int i = 0;
	uint32 *defaultDuration;
	uint32 *defaultSize;
	if(type == VO_PARSER_OT_AUDIO)
	{
		defaultDuration = &m_nDefaultDuration_Audio;
		defaultSize = &m_nDefaultSize_Audio;
	}
	else
	{
		defaultDuration = &m_nDefaultDuration_Video;
		defaultSize = &m_nDefaultSize_Video;
	}
	if(*defaultSize) 
		sb.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_SIZE, defaultSize);
	if(*defaultDuration)
		sb.SetParam(VO_PID_FRAGMENT_DEFAULT_SAMPLE_DURATION, defaultDuration);

	while(count -- )
	{
		
		pos = sb.GetSampleAddress(index);
		uint32 nSize = sb.GetSampleSize(index);
		uint64 timeScale = sb.GetTimeScale();

		if(curSize < nSize) 
		{
			SAFE_DELETE(pBuffer);
			curSize = nSize;
			pBuffer = new unsigned char[curSize];
		}
		sssize += nSize;

		m_pReader->SetPosition(pos);
	
		m_pStream->Read(pBuffer, nSize);
		
		VO_SOURCE_SAMPLE pSample;
		if(type == VO_PARSER_OT_AUDIO && sb.IsNeedGather21Chunk())
		{
			pSample.Buffer = pBuffer;
			pSample.Size = nSize;
			pSample.Time = sb.m_nBaseMediaDecodetime;
		}
		else
		{
			pSample.Buffer = pBuffer;
			pSample.Size = nSize;
			pSample.Time = sb.GetSampleTime2(index, 1); 

			VOLOGR("I-IsVideo(%d) Key:%d, Time:%lld,Size:%d",!m_u32StreamID, (pSample.Size&0x80000000)>>31 , pSample.Time, pSample.Size &0x7fffffff);
			pSample.Duration = (VO_U32)sb.GetDurationByIndex(index);
			ts += sb.GetDurationByIndex(index);
			if(sb.m_nBaseMediaDecodetime == 0)
				 pSample.Time += m_u64Starttime;
		}
		if(type == VO_PARSER_OT_VIDEO && index == 0)
			pSample.Size |= 0x80000000;
		
		
		VOLOGR("IsVideo(%d) Key:%d, Time:%lld,StartTime:%lld Size:%d TimeScale:%d",!m_u32StreamID, (pSample.Size&0x80000000)>>31 , pSample.Time - m_u64Starttime, m_u64Starttime,pSample.Size &0x7fffffff, timeScale);
		/*if(timeScale >0)
			pSample.Time = VO_S64(pSample.Time / (1.0 * timeScale/1000));*/
		IssueParseResult(type, &pSample, &timeScale);
		index++;

		i++;
	}
	if(type == VO_PARSER_OT_AUDIO)
	{
		VOLOGR("Audio(%d):baseDecodeTime:%lld. SampleCounts:%d ChunkDuration:%lld",a++, sb.m_nBaseMediaDecodetime,sb.GetSampleCount(),ts );
	}
	else if(type == VO_PARSER_OT_VIDEO)
	{
		VOLOGR("Video(%d):baseDecodeTime:%lld. SampleCounts:%d ChunkDuration:%lld",v++, sb.m_nBaseMediaDecodetime,sb.GetSampleCount(),ts);
	}

	SAFE_DELETE(pBuffer);
	SAFE_DELETE(oneChunkBuffer);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4PushReader::ParserHead(VO_PBYTE pBuf,VO_U64 bufsize)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;
	

	m_pMovie = new Movie;

	m_pMovie->Open(m_pReader,VO_TRUE);
	
	VO_U32 nFlag =0;
	rc = InitTracks(nFlag);

	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	//SelectDefaultTracks();
//	rc = PrepareTracks();	
	if(m_nTracks>1)
		m_nTracks = 1;
	for(int i=0; i< m_nTracks; i++)
	{
		CreateTrackInfo(i);
// 		if(m_trackInfo->Type == VO_SOURCE_TT_AUDIO)
// 		{
// 			m_nDefaultDuration_Audio = m_pMovie->GetDefaultDuration();
// 			m_nDefaultSize_Audio = m_pMovie->GetDefaultSize();
// 			VOLOGI("Default Audio Size %d",m_nDefaultSize_Audio);
// 		}
// 		if(m_trackInfo->Type == VO_SOURCE_TT_VIDEO)
// 		{
// 			m_nDefaultDuration_Video = m_pMovie->GetDefaultDuration();
// 			m_nDefaultSize_Video = m_pMovie->GetDefaultSize();
// 		}

		IssueParseResult(VO_PARSER_OT_TRACKINFO, m_trackInfo, &m_u32TimeScale);

		SAFE_DELETE(m_trackInfo);
	}

	Close();

	return rc;
}
VO_VOID	CMp4PushReader::IssueParseResult(VO_U32 nType, VO_VOID* pValue, VO_VOID* pReserved /*= NULL*/)
{	
	VOLOGR("+ IssueParserResult. Type:0x%08x", nType);
	VO_PARSER_OUTPUT_BUFFER output;
	output.nStreamID = m_u32StreamID;
	output.nType = nType;
	output.pOutputData = (VO_VOID*)pValue;
	output.pReserved = pReserved;
	output.pUserData = m_pUserData;

	m_pProc(&output);
	VOLOGR("- IssueParserResult. ");
}

VO_U32 CMp4PushReader::CreateTrackInfo(int id)
{
	SAFE_DELETE(m_trackInfo);
	//SAFE_DELETE(m_trackInfo.);
	VO_CODECBUFFER codecbuffer;
	CMp4Track2* pNewTrack = (CMp4Track2*)m_ppTracks[id];
	pNewTrack->GetHeadData(&codecbuffer);
	m_u32TimeScale = VO_U32(pNewTrack->GetTimeScale());
	if(m_u32TimeScale == 0)
		m_u32TimeScale =1;
// 	if(codecbuffer.Length <=0)
// 		return VO_ERR_SOURCE_ERRORDATA;

	m_trackInfo = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + codecbuffer.Length ];

	VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_trackInfo;
	memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

	trackinfoex->Type = pNewTrack->GetType()== VOTT_AUDIO?VO_SOURCE_TT_AUDIO: (pNewTrack->GetType() == VOTT_VIDEO?VO_SOURCE_TT_VIDEO:VO_SOURCE_TT_MAX);
	pNewTrack->GetCodec(&trackinfoex->Codec);
	if(trackinfoex->Type == VO_SOURCE_TT_AUDIO)
	{
		pNewTrack->GetAudioFormat(&trackinfoex->audio_info);
	}
	else if(trackinfoex->Type == VO_SOURCE_TT_VIDEO)
	{
		pNewTrack->GetParameter(VO_PID_VIDEO_FORMAT, &trackinfoex->video_info);
	}
	trackinfoex->HeadData = trackinfoex->Padding;
	memcpy(trackinfoex->HeadData, codecbuffer.Buffer, codecbuffer.Length);
	trackinfoex->HeadSize = codecbuffer.Length;

	return VO_ERR_SOURCE_OK;
}
VO_U32 CMp4PushReader::Close()
{
	VOLOGR("+Close");
	UnprepareTracks();
	UninitTracks();

	VOLOGR("-1");
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
	VOLOGR("-12");

	if(m_pMovie)
		m_pMovie->Close();


	SAFE_DELETE(m_pReader);
	SAFE_DELETE(m_pStream);
	SAFE_DELETE(m_pMovie);
	if(m_bBufNew)
	{
		delete []m_pBuf_x;
		m_pBuf_x = NULL;
		m_bBufNew = VO_FALSE;
	}
	VOLOGR("-Close");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4PushReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_S32 nTracks = m_pMovie->GetTrackCount();
	if(nTracks > 0)
	{
		Track** ppTracks = NEW_OBJS(Track*, nTracks);
		if(ppTracks)
		{
			if(VO_ERR_SOURCE_OK == TracksCreate(nTracks))
			{
				MP4RRC rc = MP4RRC_OK;
				for(VO_S32 i = 0; i < nTracks; i++)
				{
					ppTracks[i] = VO_NULL;
					rc = m_pMovie->OpenTrack(i, ppTracks[i]);

					if(MP4RRC_OK == rc)
					{
						CMp4Track2* pNewTrack = new CMp4Track2(ppTracks[i], m_pReader, m_pMemOp);
						pNewTrack->SetIsStreaming(VO_TRUE);
						if(!pNewTrack)
							break;

						TracksAdd(pNewTrack);
					}
					else
					{
						if (ppTracks[i])
						{
							ppTracks[i]->Close();

							delete ppTracks[i];
							ppTracks[i] = VO_NULL;
						}
					}
				}
			}

			MemFree(ppTracks);
		}
	}

	return CBaseReader::InitTracks(nSourceOpenFlags);
}

VO_U32 CMp4PushReader::GetFileHeadSize(VO_U32* pdwFileHeadSize)
{
	*pdwFileHeadSize = (VO_U32)m_pMovie->GetHeaderSize();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4PushReader::GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!pParam)
		return VO_ERR_INVALID_ARG;

	VO_S64 llMediaTime = VO_MAXU32;
	VO_S64 llTmp = VO_MAXU32;
	VO_U32 dwTrackDuration = 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(!m_ppTracks[i] && !m_ppTracks[i]->IsInUsed())
			continue;

		dwTrackDuration = m_ppTracks[i]->GetDuration();

		VO_U32 rc = ((CMp4Track2*)m_ppTracks[i])->GetMediaTimeByFilePos(pParam->llFilePos, &llTmp, (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_TRUE : VO_FALSE);
		//if reach this file position, the track is end
		//we will use the file duration value
		if(VO_ERR_SOURCE_OK != rc || llTmp >= dwTrackDuration)
			llTmp =VO_S64(m_pMovie->GetDuration() * TIME_UNIT);

		//we will get the min value of the tracks
		if(llMediaTime > llTmp)
			llMediaTime = llTmp;
	}

	pParam->llMediaTime = llMediaTime;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4PushReader::GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!pParam)
		return VO_ERR_INVALID_ARG;

	VO_S64 llFilePos = (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_MAXU32 : 0;
	VO_S64 llTmp = (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_MAXU32 : 0;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(!m_ppTracks[i] && !m_ppTracks[i]->IsInUsed())
			continue;

		((CMp4Track2*)m_ppTracks[i])->GetFilePosByMediaTime(pParam->llMediaTime, &llTmp, (pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START) ? VO_TRUE : VO_FALSE);
		if(pParam->nFlag & VO_FILE_MEDIATIMEFILEPOS_FLAG_START)
		{
			if(llFilePos > llTmp)
				llFilePos = llTmp;
		}
		else
		{
			if(llFilePos < llTmp)
				llFilePos = llTmp;
		}
	}

	pParam->llFilePos = llFilePos;

	return VO_ERR_SOURCE_OK;
}

#ifdef _SUPPORT_PARSER_METADATA
VO_U32 CMp4PushReader::InitMetaDataParser()
{
	m_pMetaDataParser = new CMP4MetaData(m_pFileOp, m_pMemOp);
	if(!m_pMetaDataParser)
		return VO_ERR_OUTOF_MEMORY;

	MetaDataInitInfo initInfo;
	initInfo.eType = VO_METADATA_iTunes;
	initInfo.ullFilePositions[0] = VO_MAXU64;
	initInfo.ullFilePositions[1] = VO_MAXU64;
	initInfo.ullFileSize = 0;
	initInfo.dwReserved = 0;
	initInfo.hFile = m_pStream->GetFileHandle();

	return m_pMetaDataParser->Load(&initInfo);
}

#endif //_SUPPORT_PARSER_METADATA

VO_U32 CMp4PushReader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch ( uID )
	{
	case VO_PID_SOURCE_GETSEEKTIMEBYPOS:
		GetSeekTimeByPos((VO_FILE_MEDIATIMEFILEPOS*)pParam);
		return 0;
	/*case VO_PID_FILE_HEADDATAINFO:
		{
			headerdata_info * ptr_info = (headerdata_info *)pParam;

			if( ptr_info->ptr_array )
			{
				ptr_info->ptr_array[0].physical_pos = m_pMovie->GetHeaderPos();
				ptr_info->ptr_array[0].size = m_pMovie->GetHeaderSize();
			}

			ptr_info->arraysize = 1;

			return 0;
		}
		break;

	case VO_PID_FILE_GETAVSTARTPOS: //Jason 10/21/2010
		{
			av_startpos* avsp = (av_startpos*) pParam;
			avsp->video_startpos = avsp->audio_startpos = -1;
			for( VO_S32 i = 0 ; i < m_nTracks ; i++ )
			{
				CMp4Track2* track = (CMp4Track2*) m_ppTracks[i];
				if( track && track->IsInUsed() )
				{
					if (track->GetType() == VOTT_VIDEO)
						avsp->video_startpos = track->GetDataStartPos();
					else if (track->GetType() == VOTT_AUDIO)
						avsp->audio_startpos = track->GetDataStartPos();
				}
			}
			return 0;
		}
		break;

	case VO_PID_FILE_GETVIDEOMATRIX: //Jason 4/21/2011
		{
			video_matrix* vm = (video_matrix*) pParam;
			memset(vm, 0, sizeof(video_matrix));
			m_pMovie->GetMatrix(vm->global);
			for( VO_S32 i = 0 ; i < m_nTracks ; i++ )
			{
				CMp4Track2* track = (CMp4Track2*) m_ppTracks[i];
				if( track && track->IsInUsed() )
				{
					if (track->GetType() == VOTT_VIDEO)
						track->GetMatrix(vm->track);
				}
			}
			int c1 = vm->global[3] >> 16;
			int c2 = vm->track[3] >> 16;
			int c3 = vm->global[4] >> 16;
			int c4 = vm->track[4] >> 16;

			if((c1 | c2) == 1)
				vm->rotation = 270;
			else if((c1 | c2) == -1)
				vm->rotation = 90;
			else if((c3 | c4) == -1)
				vm->rotation = 180;
			else
				vm->rotation = 0;
			return 0;
		}
		break;
	case VO_PID_SOURCE_DATE:
		{
			char * date = (char*)pParam;
			char *time = m_pMovie->GetCreateTime();
			int size = strlen(time);
			memcpy(date, time,size);
			date[strlen(time)] = '\0';
			//VOLOGR("GetSourceDate: %s",date);
		}
		break;*/
	case VO_PID_SOURCE_NUM_TRACKS:
		{
			int *num = (int*)pParam;
			*num = m_pMovie->GetTrackCount();
			//VOLOGR("GetTrackNum: %d", *num);
		}
		break;
	default:
		return CBaseReader::GetParameter( uID , pParam );
	}
	return 0;
}

VO_U32 CMp4PushReader::GetSeekPosByTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	VO_S64 mediatime = pParam->llMediaTime;

	for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
	{
		if( m_ppTracks[i] && m_ppTracks[i]->IsInUsed() )
		{
			VO_S64 time = mediatime;
			if( 0 == m_ppTracks[i]->SetPos( &time ) )
			{
				if( time < mediatime )
					mediatime = time;
			}
		}
	}

	pParam->llMediaTime = mediatime;
	return GetFilePosByMediaTime( pParam );
}

VO_U32 CMp4PushReader::GetSeekTimeByPos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	VO_S64 mediatime = 0x7fffffffffffffffll;

	for( VO_U32 i = 0 ; i < m_nTracks ; i++ )
	{
		if( m_ppTracks[i] && m_ppTracks[i]->IsInUsed() )
		{
			VO_S64 time =0;
			time = m_ppTracks[i]->GetRealDurationByFilePos(pParam->llFilePos );
			if( time < mediatime )
					mediatime = time;
		}
	}

	pParam->llMediaTime = mediatime;
	return VO_ERR_NONE;
}


VO_U32 CMp4PushReader::SetParam(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRc= VO_ERR_NONE;
	switch(uID)
	{
	case VOSTREAMREADER_PID_DRM_CALLBACK:
		{
		}
		break;
	case VOSTREAMREADER_PID_DRM_INFO:
		{
		}
		break;
	default:
		VOLOGR("unknow ID");
		nRc = VO_ERR_NOT_IMPLEMENT;
		break;
	}

	return nRc;
}

VO_U32 CMp4PushReader::GetParam(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 nRc = VO_ERR_NONE;
	return nRc;
}