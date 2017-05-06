#include "CFlvReader.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CFlvReader* pReader = (CFlvReader*)pUser;
	PFlvDataParserSample pSample = (PFlvDataParserSample)pParam;
	return pReader->OnSample(pSample->pFileChunk, pSample->bVideo, pSample->btFlag, pSample->dwLen, pSample->dwTimeStamp, pSample->pos);
}

static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CFlvTrack* pTrack = (CFlvTrack*)pUser;
	return pTrack->OnBlock(*(VO_U32*)pParam);
}

CFlvReader::CFlvReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseStreamFileReader(pFileOp, pMemOp,pLibOP, pDrmCB)
	, m_HeaderParser(&m_chunk, pMemOp)
	, m_DataParser(&m_chunk, pMemOp)
	, m_pFlvAudioTrack(VO_NULL)
	, m_pFlvVideoTrack(VO_NULL)
	,m_bThumbnail(VO_FALSE)
{
	m_pFileDataParser = &m_DataParser;
}

CFlvReader::~CFlvReader()
{
	Close();
}

VO_U32 CFlvReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	//added by doncy for test 0609
	//nSourceOpenFlags |= VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL;
	//end
	Close();


	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		m_bThumbnail = VO_TRUE;
		m_HeaderParser.SetThumbNail(VO_TRUE);
		m_DataParser.SetThumbNailFromFR(VO_TRUE);

		pFileSource->nMode = VO_FILE_READ_ONLY;
		m_hFile = FileOpen(pFileSource);
		if(!m_hFile)
			return VO_ERR_SOURCE_OPENFAIL;

		m_ullFileSize = FileSize(m_hFile);

		if(!m_chunk.FCreate(m_hFile,0,256))
			return VO_ERR_SOURCE_OPENFAIL;
	}
	else
	{
		VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
		if(VO_ERR_SOURCE_OK != rc)
			return rc;
	}

	m_HeaderParser.set_physical_file_size( m_ullFileSize );
	if(!m_HeaderParser.ReadFromFile())
		return VO_ERR_SOURCE_OPENFAIL;

	VO_U32 dwFileDuration = m_HeaderParser.GetDuration();
	if(dwFileDuration <= 0)
		return VO_ERR_SOURCE_OPENFAIL;

	m_nTracks = m_HeaderParser.GetTrackCount();
	TracksCreate( m_nTracks );

	VO_S32 trackcount = 0;
	//create audio track
	if ((!m_bThumbnail)&&m_HeaderParser.IsAudioInfoValid())
	{
		m_pFlvAudioTrack = new CFlvAudioTrack(0x8, dwFileDuration, m_HeaderParser.GetAudioInfo(), this, m_pMemOp);
		m_ppTracks[trackcount] = m_pFlvAudioTrack;
		m_pFlvAudioTrack->SetInUsed( VO_TRUE );
		m_pFlvAudioTrack->Prepare();
		trackcount++;
	}

	//create video track
	if(m_HeaderParser.IsVideoInfoValid())
	{
		m_pFlvVideoTrack = new CFlvVideoTrack(0x9, dwFileDuration, m_HeaderParser.GetVideoInfo(), this, m_pMemOp);
		m_ppTracks[trackcount] = m_pFlvVideoTrack;
		m_pFlvVideoTrack->SetInUsed( VO_TRUE );
		m_pFlvVideoTrack->Prepare();
		trackcount++;
	}

	m_nTracks = trackcount;

	if(!InitTracks(nSourceOpenFlags, 0))
		return VO_ERR_SOURCE_OPENFAIL;

	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
		m_DataParser.Init(VO_NULL);

		m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
		if (!m_bThumbnail)
		{
			if(!m_DataParser.SetStartFilePos(m_HeaderParser.GetMediaDataFilePos()))
				return VO_ERR_SOURCE_END;
			FileGenerateIndex();
		}
		else
		{
			m_pFlvVideoTrack->SetParseForSelf(VO_TRUE);
		}

		//still no frame, then we should delete it!!
		if(m_pFlvAudioTrack && m_pFlvAudioTrack->m_pStreamFileIndex->IsNull())
		{
			TracksRemove(m_pFlvAudioTrack);
			SAFE_DELETE(m_pFlvAudioTrack);
		}
		if(m_pFlvVideoTrack && m_pFlvVideoTrack->m_pStreamFileIndex->IsNull()&&(!m_bThumbnail))
		{
			TracksRemove(m_pFlvVideoTrack);
			SAFE_DELETE(m_pFlvVideoTrack);
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvReader::Close()
{
	UnprepareTracks();
	UninitTracks();

	SAFE_DELETE(m_pFlvAudioTrack);
	SAFE_DELETE(m_pFlvVideoTrack);

	return CBaseReader::Close();
}

VO_U8 CFlvReader::OnSample(CGFileChunk* pFileChunk, VO_BOOL bVideo, VO_U8 btFlag, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_U64 pos)
{
	if(bVideo)
	{
		if(!m_pFlvVideoTrack)
			return CBRT_CONTINUABLEANDNEEDSKIP;

		if(!m_pFlvVideoTrack->AddSample(pFileChunk, (0x10 == (btFlag & 0xF0)) ? VO_TRUE : VO_FALSE, dwLen, dwTimeStamp, pos))
			return CBRT_FALSE;
	}
	else
	{
		if(!m_pFlvAudioTrack)
			return CBRT_CONTINUABLEANDNEEDSKIP;

		if(!m_pFlvAudioTrack->AddSample(pFileChunk, VO_FALSE, dwLen, dwTimeStamp, pos))
			return CBRT_FALSE;
	}

	return CBRT_CONTINUABLENOTNEEDSKIP;
}

VO_U32 CFlvReader::SetPlayMode(VO_SOURCE_PLAYMODE PlayMode)
{
	if((VO_SOURCE_PM_PLAY != PlayMode) && !m_HeaderParser.IndexIsValid())
		return VO_ERR_SOURCE_PLAYMODEUNSUPPORT;

	return CBaseReader::SetPlayMode(PlayMode);
}

VO_U32 CFlvReader::MoveTo(VO_S64 llTimeStamp)
{
	Flush();


	VO_U64 ullMediaDataFilePos = m_HeaderParser.GetMediaDataFilePos();
	VO_U64 ullFirstSamplePos = 0;
	if( m_HeaderParser.IndexIsValid() )
	{
		ullFirstSamplePos = m_HeaderParser.IndexGetFilePosByTime( 0 ) - 4;
	}
	VO_U64 ullCurrMediaDataFilePos = VO_MAX( ullMediaDataFilePos , ullFirstSamplePos );//ullMediaDataFilePos;
	if(llTimeStamp > 1000)
	{
		if(m_HeaderParser.IndexIsValid())	//file position not include PreviousTagSiz, so we should back forward 4 bytes
			ullCurrMediaDataFilePos = m_HeaderParser.IndexGetFilePosByTime(llTimeStamp) - 4;
		else
		{
			VO_U32 dwBitrate = GetFileBitrate();
			VO_U64 ullStartFilePos = ullMediaDataFilePos + (llTimeStamp / 1000) * dwBitrate;
			VO_U64 ullCurrMediaDataPos = VO_MAXU64;
			VO_U32 dwVideoMaxSampleSize = VO_MAXU32;
			if(m_pFlvVideoTrack)
				m_pFlvVideoTrack->GetMaxSampleSize(&dwVideoMaxSampleSize);
			while(!m_DataParser.GetFilePosByTime(ullStartFilePos, dwVideoMaxSampleSize, m_HeaderParser.GetDuration(), llTimeStamp, ullCurrMediaDataPos) && (VO_MAXU64 == ullCurrMediaDataPos))
			{
				if(ullStartFilePos < dwBitrate)
					break;

				ullStartFilePos -= dwBitrate;
			}
			ullCurrMediaDataFilePos = (VO_MAXU32 != ullCurrMediaDataPos) ? ullCurrMediaDataPos : ullMediaDataFilePos;
		}
	}

	if(!m_DataParser.SetStartFilePos(ullCurrMediaDataFilePos))
		return VO_ERR_SOURCE_END;

	FileGenerateIndex();

	return CBaseReader::MoveTo(llTimeStamp);
}

VO_U64 CFlvReader::FileIndexGetFilePosByIndex(VO_U32 dwFileIndex)
{
	return m_HeaderParser.IndexIsValid() ? m_HeaderParser.IndexGetFilePosByIndex(dwFileIndex) : -1;
}

VO_BOOL CFlvReader::InitTracks(VO_U32 nSourceOpenFlags, VO_U32 dwExtSize)
{
	if( VO_ERR_SOURCE_OK == CBaseReader::InitTracks(nSourceOpenFlags) )
		return VO_TRUE;
	else
		return VO_FALSE;
}

VO_U32 CFlvReader::FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	if(llTimeStamp == 0)
		return 0;

	return m_HeaderParser.IndexIsValid() ? m_HeaderParser.IndexGetIndexByTime(bForward, llTimeStamp) : -1;
}

VO_VOID CFlvReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CFlvDataParser* pNewDataParser = new CFlvDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	pNewDataParser->Init(VO_NULL);
	pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
	pNewDataParser->SetBlockStream(GetOtherTrackInUsed(pRqstTrack)->GetStreamNum());

	m_DataParser.SetBlockStream(pRqstTrack->GetStreamNum());
	m_DataParser.SetOnBlockCallback(pRqstTrack, VO_NULL, GOnBlock);

	for(VO_U8 i = 0; i < 2; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_TRUE);
	}

	*ppDataParser = pNewDataParser;
}

VO_U32 CFlvReader::GetFileBitrate()
{
	VO_U64 ullFileSize = m_HeaderParser.GetFileSize();
	if(!ullFileSize)
		ullFileSize = m_ullFileSize;

	return static_cast<VO_U32>( (ullFileSize - m_HeaderParser.GetMediaDataFilePos()) * 1000 / m_HeaderParser.GetDuration() );
}

VO_U32 CFlvReader::GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if( !m_HeaderParser.IndexIsValid() )
		return VO_ERR_SOURCE_END;

	VO_S64 timestamp  = m_HeaderParser.IndexGetTimeByFilePos( pParam->llFilePos );

	if( timestamp == -1 )
		return VO_ERR_SOURCE_END;

	pParam->llMediaTime = timestamp;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvReader::GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if( !m_HeaderParser.IndexIsValid() )
		return VO_ERR_SOURCE_END;

	pParam->llFilePos = m_HeaderParser.IndexGetFilePosByTime( pParam->llMediaTime ) - 4;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvReader::GetFileHeadSize(VO_U32* pdwFileHeadSize)
{
	*pdwFileHeadSize = 0;//m_HeaderParser.GetHeaderSize();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvReader::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	if(!m_HeaderParser.IndexIsValid())
		return VO_ERR_NOT_IMPLEMENT;

	if(llTime < 0)
		return VO_ERR_INVALID_ARG;

	*pllPreviousKeyframeTime = m_HeaderParser.GetPrevKeyFrameTime(llTime);
	*pllNextKeyframeTime = m_HeaderParser.GetNextKeyFrameTime(llTime);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CFlvReader::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return ((CFlvDataParser *)m_pFileDataParser)->GetThumbNailBuffer(ppBuffer, nSize, nPos);
}
