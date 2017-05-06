#include "CTsTrack.h"
#include "CStream.h"
#include "CMTVStreamFileIndex.h"
#include "CBaseStreamFileReader.h"
#include "fCC.h"
#include "CTsFileDataParser.h"
#include "CDumper.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CTsTrack::CTsTrack(VO_TRACKTYPE nType, VO_U32 btStreamNum, VO_U32 dwDuration, CMTVReader* pReader, VO_MEM_OPERATOR* pMemOp, CStream* pStream)
: CBaseStreamFileTrack(nType, GetStreamNumByType(nType), dwDuration, pReader, pMemOp)
, m_pReader(pReader)
, m_pStream(pStream)
, m_nNoTimeStampCount(0)
, m_nLastTimeStamp(0)
, m_StramPID(btStreamNum)
, m_TmpStampEntryListHead(VO_NULL)
, m_TmpStampEntryListTail(VO_NULL)
, m_ValidTimeStamp(-1)
, m_wUnvalidTimeStampEntry(0)

{
    VO_CHAR*   pLanguage=NULL;
    m_ulTrackMode = TS_TRACK_MODE_NORMAL;
    m_ulBufferState = 0;
    m_ulCurIFrameCount = 0;
	if(pStream != NULL)
	{
		pLanguage = pStream->GetLanguageDesc();
		memcpy(m_strLanguage, pLanguage, strlen(pLanguage));
	}
	CreateDumpByStreamId(m_StramPID);
#ifdef _STEP_EVENT_
	m_StepEvent.Signal();
#endif
}

CTsTrack::~CTsTrack(void)
{
    if(m_ulTrackMode == TS_TRACK_MODE_THUMBNAIL)
    {
        DeleteAllThumbnail();
    }

	CloseAllFileDump();
	CBaseTrack::Unprepare();
}

VO_U32 CTsTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if (pdwMaxSampleSize)
	{
		if(m_nType == VOTT_AUDIO)
		{
			VO_U32 nSampleRate = 0,nChannels = 0;
			if (m_pStream)
			{
				m_pStream->GetAudioSampleRate(&nSampleRate);
				m_pStream->GetAudioChannels(&nChannels);
			}
			*pdwMaxSampleSize = VO_MAX( nSampleRate * nChannels * 2 , 65536 );
		}
		else
			*pdwMaxSampleSize = m_pStream->GetMaxFrameSize();
	}
	
	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsTrack::GetCodecCC(VO_U32* pCC)
{
	VO_U32 codec;

	GetCodec(&codec);

	VOLOGE("the codec %d", codec);
	if (VO_AUDIO_CodingAAC == codec)
	{
		//*pCC = 0xA106;

		// tag: 20100416
		int type;
		m_pStream->GetAACFrameType(&type);

		if (type == VOAAC_ADTS)
		{
			*pCC = AudioFlag_ADTS_AAC;
		}
		else
			*pCC = 0xA106;

		//end
	}
	else if (VO_AUDIO_CodingMP3 == codec)
	{
		*pCC = AudioFlag_MP3;
	}
	else if (VO_VIDEO_CodingH264 == codec)
	{
		*pCC = FOURCC_H264;
	}
	else if (VO_VIDEO_CodingMPEG2 == codec)
	{
		*pCC = FOURCC_MPG2;
	}
	else if (VO_VIDEO_CodingMPEG4 == codec)
	{
		*pCC = FOURCC_MPG4;
	}

	VOLOGE("the codec:%d", *pCC);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Time = m_dwDuration;
	return m_pStream->GetHeadData(pHeadData);
}

VO_U32 CTsTrack::GetCodec(VO_U32* pCC)
{
	*pCC = m_pStream->GetCodec();
	return VO_ERR_SOURCE_OK;
}



VO_U32 CTsTrack::GetBitrate(VO_U32* pdwBitrate)
{
	return VO_ERR_SOURCE_OK;
}

/*
VO_BOOL	CTsTrack::InitGlobalBuffer(VO_U32 dwBufferTime, VO_U32 dwExtSize)
{
	if(m_pGlobeBuffer)
		delete m_pGlobeBuffer;

	m_pGlobeBuffer = new CGBuffer(m_pMemOp);
	if(!m_pGlobeBuffer)
		return VO_FALSE;

	VO_U32 dwMaxSampleSize = 0;
	GetMaxSampleSize(&dwMaxSampleSize);

	VO_U32 dwBitrate = 0;
	GetBitrate(&dwBitrate);

	if(!m_pGlobeBuffer->Init(VO_MAX(dwBitrate, dwMaxSampleSize) * dwBufferTime + dwExtSize))
		return VO_FALSE;

	m_dwGBExtSize = dwExtSize;
	m_dwBufferTime = dwBufferTime;

	return VO_TRUE;
}
*/

VO_U32 CTsTrack::Init(VO_U32 nSourceOpenFlags)
{
	// 20100610
// 	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
// 		InitGlobalBuffer(10, 1024*128 + ((0 + 0x3) & ~0x3));

    if((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) == VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)
    {
        m_ulTrackMode = TS_TRACK_MODE_THUMBNAIL;
        m_ulIFrameCountPerTime = 1;
		m_ulIFrameCurIndex = 0;
        VOLOGI("Set the Thumbnail mode!");
    }


	SetBufferTime(20);
	VO_U32 dwMaxSampleSize = 0;
	GetMaxSampleSize(&dwMaxSampleSize);;
	CTsFileDataParser*pDataParser = (CTsFileDataParser*)m_pStreamFileReader->GetFileDataParserPtr();
	VO_U16 nPacketLen = 0;
	if (pDataParser)
	{
		nPacketLen = pDataParser->GetPacketLen();
	}
	else
		nPacketLen = 188;

	SetGlobalBufferExtSize(dwMaxSampleSize+TS_STREAM_PACKET_COUNT*nPacketLen);
//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	InitGlobalBuffer();

//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	m_pStreamFileIndex = new CMTVStreamFileIndex(m_pStreamFileReader->GetMemPoolPtr());

	CBaseTrack::Prepare();

	return CBaseTrack::Init(nSourceOpenFlags);
}

VO_U32 CTsTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	VOAUDIOFORMAT format;
	
	if(VO_ERR_NONE == m_pStream->GetAudioFormat(&format))
	{
		pAudioFormat->Channels		= format.channels;
		pAudioFormat->SampleBits	= format.sample_bits;
		pAudioFormat->SampleRate	= format.sample_rate;
	}
	else
		return VO_ERR_SOURCE_FORMATUNSUPPORT;


	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	VOVIDEOFORMAT fmt;
	
	if(VO_ERR_NONE == m_pStream->GetVideoFormat(&fmt))
	{
		VOLOGE("video format:width:%d,height:%d", fmt.width, fmt.height);
		pVideoFormat->Width		= fmt.width;
		pVideoFormat->Height	= fmt.height;
	}
	else
		return VO_ERR_SOURCE_FORMATUNSUPPORT;

	return VO_ERR_SOURCE_OK;
}

//GetSample and SetPos implement of normal mode!!
VO_U32 CTsTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	// 	if (m_bVideo)
	// 		VOLOGI("GetSampleN Time:%u", pSample->Time);

	//
	//pSample->Time = 0;
	//

	
	VO_BOOL bFrameDrop = VO_FALSE;
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;

    if(m_nType == VOTT_TEXT)
    {
		VO_U32   ulFirstTime = m_pStreamFileIndex->GetCurrStartTime();
		if(m_pStreamFileIndex->GetCurrEntryCount() == 0 || ulFirstTime > pSample->Time)
        {
			return VO_ERR_SOURCE_NEEDRETRY;
		}
    }

	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, pSample->Time, &pGet, &bFrameDrop))
		return VO_ERR_SOURCE_END;

	if(bFrameDrop)
		pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	// 	if(m_bVideo)
	// 		VOLOGI("Found Time:%u", pSample->Time);

	VO_U32 dwMaxSampleSize = VO_MAXU32;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		return VO_ERR_SOURCE_OK;
	}

	pSample->Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
		pSample->Buffer = m_pSampleData;

	DumpFrame(pSample);
#if 0
	if (VOTT_VIDEO == m_nType)
		CDumper::DumpVideoData(pSample->Buffer, pSample->Size);
	else
		CDumper::DumpAudioData(pSample->Buffer, pSample->Size);
#endif

	if(pGet->IsKeyFrame())
	{
		//VOLOGI("++++++++++++++ key frame, ts : %u ++++++++++++++++++++++", pGet->time_stamp);
		pSample->Size |= 0x80000000;
	}

	// 	static VO_U32 size = 0;
	// 	static VO_U32 count = 0;
	// 	size += pSample->Size;
	// 	VOLOGI("output buffer total count:%d, total len : %d", ++count, size);
	// 	DumpData(pSample->Buffer, pSample->Size);

	//VOLOGI("output %s sample: %02x %02x %02x %02x", m_bVideo?"VIDEO":"AUDIO", pSample->Buffer[0], pSample->Buffer[1], pSample->Buffer[2], pSample->Buffer[3]);


	m_pStreamFileIndex->RemoveInclude(pGet);

	TrackGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsTrack::SetPosN(VO_S64* pPos)
{
//	return CBaseStreamFileTrack::SetPosN(pPos);
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);

	if(-1 == nRes || 1 == nRes)
	{
		VO_BOOL bParseEnd = VO_FALSE;

		while((-1 == nRes || 1 == nRes)&& !bParseEnd)	//retry!!
		{
			SetParseForSelf(VO_TRUE);
			//modified by yyang 20091215
			//if(m_pGlobeBuffer)
			//m_pGlobeBuffer->Flush();
			if (1 == nRes)
			{
				if(m_pGlobeBuffer)
					m_pGlobeBuffer->RemoveTo2(pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size);
			}
			m_pStreamFileIndex->RemoveAll();

			if(!TrackGenerateIndex())
				bParseEnd = VO_TRUE;

			nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
			//	if(-1 == nRes)
			//		return VO_ERR_SOURCE_END;
		}

		if(0 != nRes)
			return VO_ERR_SOURCE_END;

		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
		TrackGenerateIndex();
	}
	else	//ok
	{
		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
		m_pStreamFileReader->FileGenerateIndex();
	}

	return VO_ERR_SOURCE_OK;
}

//GetSample and SetPos implement of key frame mode!!
VO_U32 CTsTrack::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	CDumper::WriteLog((char *)"GetSampleK Time:%u", pSample->Time);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CTsTrack::SetPosK(VO_S64* pPos)
{
	CDumper::WriteLog((char *)"set posk: %d", *pPos);

	return VO_ERR_SOURCE_OK;
}

VO_VOID	CTsTrack::Flush()
{
	m_wUnvalidTimeStampEntry = 0;
	m_ValidTimeStamp = -1;
	PBaseStreamMediaSampleIndexEntry pTmpEntry = m_TmpStampEntryListHead;
	while (pTmpEntry)
	{
		m_pStreamFileIndex->DeleteEntry(pTmpEntry);
		pTmpEntry = pTmpEntry->next;
	}
	m_TmpStampEntryListHead = m_TmpStampEntryListTail = VO_NULL;
	return CBaseStreamFileTrack::Flush();
}

//int video_frame_count = 0;

VO_BOOL	CTsTrack::CalculateTimeStamp(PBaseStreamMediaSampleIndexEntry pNewEntry)
{
	if(!pNewEntry)
		return VO_FALSE;
	///<debug
	if(m_ValidTimeStamp == pNewEntry->time_stamp)
	{
		if (m_ValidTimeStamp == -1)
		{
			m_pStreamFileIndex->DeleteEntry(pNewEntry);
			return VO_FALSE;
		}
		m_wUnvalidTimeStampEntry++;
		if( m_TmpStampEntryListHead == VO_NULL)
		{
			m_TmpStampEntryListHead = m_TmpStampEntryListTail = pNewEntry;
		}
		else
		{
			m_TmpStampEntryListTail->next = pNewEntry;
			m_TmpStampEntryListTail = pNewEntry;
		}
		return VO_FALSE;
	}
	else
	{
		if(m_wUnvalidTimeStampEntry > 0)
		{
			VO_U32 dwTimeStep = 0;
			VO_U32 dwFlag = 0;
			if(pNewEntry->time_stamp - m_ValidTimeStamp)
			{
			    dwTimeStep = ((pNewEntry->time_stamp - m_ValidTimeStamp)) / (m_wUnvalidTimeStampEntry + 1);
				dwFlag = 1;
			}
			else
			{
				dwTimeStep = ((m_ValidTimeStamp -pNewEntry->time_stamp)) / (m_wUnvalidTimeStampEntry + 1);
				dwFlag = 0;
			}

			PBaseStreamMediaSampleIndexEntry pTmpEntry = m_TmpStampEntryListHead;
			for(VO_U32 i = 1; pTmpEntry && (i <= m_wUnvalidTimeStampEntry); i++)
			{
				if(dwFlag == 1)
				{
				    pTmpEntry->time_stamp = (VO_U32)m_ValidTimeStamp + i * dwTimeStep;
				}
				else
				{
					pTmpEntry->time_stamp = (VO_U32)m_ValidTimeStamp - i * dwTimeStep;
				}
				pTmpEntry = pTmpEntry->next;
			}
		}

		m_ValidTimeStamp = pNewEntry->time_stamp;
		m_wUnvalidTimeStampEntry = 0;
		return VO_TRUE;
	}
}

VO_BOOL CTsTrack::PushTmpSampleToStreamFile(PBaseStreamMediaSampleIndexEntry pNewEntry)
{
	///<due to m_pStreamFileIndex->Add designed too simple,so i have to deconstruct the temporal list
	///<and add the sample one by one
	PBaseStreamMediaSampleIndexEntry pTmpEntry = m_TmpStampEntryListHead;
	while (pTmpEntry)
	{
		m_pStreamFileIndex->Add(pTmpEntry);
		pTmpEntry = pTmpEntry->next;
	}
	m_TmpStampEntryListHead = m_TmpStampEntryListTail =VO_NULL;
	m_pStreamFileIndex->Add(pNewEntry);
	return VO_TRUE;
}
VO_U32 CTsTrack::AddFrame(VO_MTV_FRAME_BUFFER* pBuf)
{
	//__try

	if(m_ulTrackMode == TS_TRACK_MODE_THUMBNAIL)
	{
		if(m_nType != VOTT_VIDEO)
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}

		if(pBuf->nFrameType != VO_VIDEO_FRAME_I)
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}
		
		VOLOGI("Got %d I frames", m_ulCurIFrameCount);
		CacheFrameForThumbnail(pBuf);
		return VO_ERR_SOURCE_OK;
	}


    if(m_bInUsed == VO_FALSE)
	{
		while(!m_pGlobeBuffer->HasIdleBuffer(m_dwGBExtSize+pBuf->nSize))
		{
			RemoveSampleUnUsed();
		}
	}

	{
		VO_PBYTE addr = NULL;

		VO_U32 dwMaxSampleSize = VO_MAXU32;
		GetMaxSampleSize(&dwMaxSampleSize);
		if (pBuf->nSize > dwMaxSampleSize )
		{
			pBuf->nSize+=0;
		}
		VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&addr, pBuf->nSize);

		if (!addr)
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}

		m_pGlobeBuffer->DirectWrite2(pBuf->pData, addr, 0, pBuf->nSize);

		// adjust time stamp

		PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry((VO_U32)(pBuf->nStartTime), dwPosInBuffer, pBuf->nSize);

		if(!pNew)
			return VO_ERR_SOURCE_ERRORDATA;

#if 0 // test code
		if (IsVideo())
		{
			video_frame_count++;
			CDumper::WriteLog("Video I Frame, TS    = %u", pBuf->nStartTime);
			if (pBuf->nFrameType == VO_VIDEO_FRAME_I)
			{
				//CDumper::WriteLog("Video I Frame, TS = %d, index = %d", pBuf->nStartTime, video_frame_count);
				CDumper::WriteLog("Video I Frame, Index = %u\r\n", video_frame_count);
			}
		}
#endif
		
		
		pNew->SetKeyFrame((pBuf->nFrameType==VO_VIDEO_FRAME_I)?VO_TRUE:VO_FALSE);
		if (CalculateTimeStamp(pNew))
		{
			PushTmpSampleToStreamFile(pNew);
		}
		
///<remove by dolby,this handling may destroy mempool and cause crash,i will change the handling for this situation that just one timestamo for mutil frames
#if 0

		if (!m_pLastValidEntry || pBuf->nStartTime>m_nLastTimeStamp)
		{
			if (m_nNoTimeStampCount > 0)
			{
				VO_U32 dwTimeStep = (pNew->time_stamp - m_pLastValidEntry->time_stamp) / (m_nNoTimeStampCount+1);

				// 			if(!IsVideo())
				// 				CDumper::WriteLog("m_nNoTimeStampCount = %d, average time = %d", m_nNoTimeStampCount, dwTimeStep);

				PBaseStreamMediaSampleIndexEntry pTmpEntry = m_pLastValidEntry->next;
				for(VO_U16 i = 1; i <= m_nNoTimeStampCount; i++)
				{
					if(pTmpEntry)
					{
						pTmpEntry->time_stamp = m_pLastValidEntry->time_stamp + i * dwTimeStep;
						pTmpEntry = pTmpEntry->next;
					}
				}
			}

			m_nNoTimeStampCount = 0;
			m_pLastValidEntry	= pNew;
		}
		else
			m_nNoTimeStampCount++;

		m_nLastTimeStamp = pBuf->nStartTime;

#endif

#if 1

		//if(!IsVideo())
		//CDumper::WriteLog("%s ts = %u", IsVideo()?"video":"audio", pNew->time_stamp);

#endif

#if 0
		if (t2 == t1)
		{
			m_nNoTimeStampCount++;
			return VO_ERR_SOURCE_OK;
		}

		int step = (t2 - t1) / m_nNoTimeStampCount;
		if (t2 < t1)
			step = 0;

		PBaseStreamMediaSampleIndexEntry entry = VO_NULL;
		m_pStreamFileIndex->GetEntry(IsVideo(), t1, &entry);

		PBaseStreamMediaSampleIndexEntry bak = entry;

		VO_U32 count = m_nNoTimeStampCount;
		VO_U64 t = t1;
		entry = entry->next;
		while (--m_nNoTimeStampCount && entry) //special for first sample
		{
			t += step;
			entry->time_stamp = t;
			entry = entry->next;
		}

		if (IsVideo())
		{
			CDumper::WriteLog("begin -----------------------------------------------------------------------------");

			VO_U64 last = 0;
			while(bak)
			{
				CDumper::WriteLog("%s ts = %I64d", IsVideo()?"video":"audio", bak->time_stamp);

				if (last == bak->time_stamp)
				{
					int n = 0;
				}

				last	= bak->time_stamp;
				bak		= bak->next;
			}
			CDumper::WriteLog("end   -----------------------------------------------------------------------------\r\n\r\n");
		}

		m_nNoTimeStampCount = 1; //reset the count
#endif
	}


	return VO_ERR_SOURCE_OK;
}

VO_BOOL CTsTrack::TrackGenerateIndex()
{
#ifdef _STEP_EVENT_
	m_StepEvent.Reset();
#endif

	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();

	VO_BOOL bNoEnd = TrackGenerateIndexC(pDataParser);

    if(m_nType == VOTT_TEXT)
    {
		return bNoEnd;
    }

	if(m_pStreamFileIndex->IsNull() && bNoEnd)	//track buffer null but file parse not end, so need new data parser to parse
	{
		VOLOGI("tracks force to use each data parser!!");

		if(m_ulBufferState == 0)
		{
			if(!m_pDataParser)
				m_pStreamFileReader->OnNewDataParser(this, &m_pDataParser);
		}

		bNoEnd = TrackGenerateIndexC(m_pDataParser);
	}

#ifdef _STEP_EVENT_
	m_StepEvent.Signal();
#endif

	return bNoEnd;
}

VO_BOOL CTsTrack::TrackGenerateIndexC(CvoFileDataParser* pDataParser)
{
	if(!pDataParser)
		return VO_FALSE;

	VO_U8 nRes = 0;
	while(!(nRes = IsCannotGenerateIndex(pDataParser)))
	{
		if(!pDataParser->Step())
			pDataParser->SetParseEnd(VO_TRUE);
	}

	return (nRes == 1) ? VO_TRUE : VO_FALSE;
}

VO_U8 CTsTrack::IsCannotGenerateIndex(CvoFileDataParser* pDataParser)
{
	if(pDataParser->IsParseEnd())
		return 2;

	if(m_bParseForSelf)
	{
		if(IsGlobalBufferFull())
			return 1;
	}
	else
	{
		if(m_pStreamFileReader->IsTrackBufferFull())
			return 1;
	}

	return 0;
}


VO_U32	CTsTrack::GetSampleUnUsed(VO_SOURCE_SAMPLE* pSample)
{
	return GetSampleN(pSample);
}

VO_VOID  CTsTrack::SetTrackLanguage(VO_CHAR*   planguage)
{
    return;
}



VO_U32  CTsTrack::RemoveSampleUnUsed()
{
	VO_BOOL bFrameDrop = VO_FALSE;
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
    VO_SOURCE_SAMPLE    varSample;
	VO_U32 dwMaxSampleSize = VO_MAXU32;

	memset(&varSample, 0, sizeof(VO_SOURCE_SAMPLE));

	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, varSample.Time, &pGet, &bFrameDrop))
	{
		return VO_ERR_SOURCE_END;
	}


	GetMaxSampleSize(&dwMaxSampleSize);
	if(varSample.Size > dwMaxSampleSize)
	{
		varSample.Size = 0;
		return VO_ERR_SOURCE_OK;
	}

	varSample.Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&(varSample.Buffer), m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
	{
		varSample.Buffer = m_pSampleData;
	}

	m_pStreamFileIndex->RemoveInclude(pGet);
	return 0;
}



VO_VOID CTsTrack::Wait()
{
#ifdef _STEP_EVENT_
	m_StepEvent.Wait(5000);
#endif
}
VO_U8	CTsTrack::GetStreamNumByType(VO_TRACKTYPE nStreamType)
{
	switch (nStreamType)
	{
	case VOTT_VIDEO:
		return TS_STREAM_NUMBERVIDEO;
	case VOTT_AUDIO:
		return TS_STREAM_NUMBERAUDIO;
	case VOTT_TEXT:
		return TS_STREAM_NUMBERSUBTITLE;
	default:
		return TS_STREAM_NUMBERMAX;
	}
}

VO_BOOL     CTsTrack::InitGlobalBuffer()
{
    uint32 iMaxSize = 8*1024*1024;
    uint32 iCalculateSize = 0;

    if(m_ulTrackMode == TS_TRACK_MODE_THUMBNAIL)
	{
		VOLOGI("Thumbnail mode, without global buffer!");
        memset(m_pFrameArrayForThumbnail, 0, sizeof(VO_MTV_FRAME_BUFFER*)*TS_MAX_I_FRAME_COUNT);
		
		if(m_pGlobeBuffer)
			delete m_pGlobeBuffer;

		m_pGlobeBuffer = new CGBuffer(m_pMemOp);
		if(!m_pGlobeBuffer)
			return VO_FALSE;

		VO_U32 dwMaxSampleSize = 0;
		GetMaxSampleSize(&dwMaxSampleSize);

		VO_U32 dwBitrate = 0;
		GetBitrate(&dwBitrate);

		//In Thumbnail Mode,set the buffer time 1 second 
		iCalculateSize = VO_MAX(dwBitrate, dwMaxSampleSize) * 1;
		if((iCalculateSize+m_dwGBExtSize) > iMaxSize)
		{
			iCalculateSize = iMaxSize;
		}
		else
		{
			iCalculateSize +=m_dwGBExtSize;
		}

		return m_pGlobeBuffer->Init(iCalculateSize);
	}
	else
	{
		//In normal playback mode
		if(m_pGlobeBuffer)
			delete m_pGlobeBuffer;

		m_pGlobeBuffer = new CGBuffer(m_pMemOp);
		if(!m_pGlobeBuffer)
			return VO_FALSE;

		VO_U32 dwMaxSampleSize = 0;
		GetMaxSampleSize(&dwMaxSampleSize);

		VO_U32 dwBitrate = 0;
		GetBitrate(&dwBitrate);

		iCalculateSize = VO_MAX(dwBitrate, dwMaxSampleSize) * m_dwBufferTime;
		if((iCalculateSize+m_dwGBExtSize) > iMaxSize)
		{
			iCalculateSize = iMaxSize;
		}
		else
		{
			iCalculateSize +=m_dwGBExtSize;
		}

		return m_pGlobeBuffer->Init(iCalculateSize);
    }
}


VO_VOID     CTsTrack::SetBufferState(VO_U32  ulBufferState)
{
    m_ulBufferState = ulBufferState;
}
VO_VOID	CTsTrack::InitAllFileDump()
{
#ifdef _DUMP_FRAME
    m_pFileData = NULL;
    m_pFileTimeStamp_Size = NULL;
#endif
}

VO_VOID	CTsTrack::CreateDumpByStreamId(VO_U32 ulPID)
{
#ifdef _DUMP_FRAME
	VO_U32   ulIndex = 0;
	VO_CHAR    strDumpPath[256] = {0};
	FILE*      pFile = NULL;
	VO_CHAR*   pStringValue = NULL;
	sprintf(strDumpPath, "C:\\Dump\\local_%d_data.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileData = pFile;
	}

	memset(strDumpPath, 0, 256);
	sprintf(strDumpPath, "C:\\Dump\\local_%d_timestamp_size.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileTimeStamp_Size = pFile;
		pStringValue = "TimeStamp    Size\n";
		fwrite(pStringValue, 1, strlen(pStringValue), pFile);
		fflush(pFile);
	}
#endif
}

VO_VOID	CTsTrack::DumpFrame(VO_SOURCE_SAMPLE* pSample)
{
#ifdef _DUMP_FRAME
	FILE*   pFileDump = NULL;
	VO_U32   ulIndex = 0;
	VO_CHAR    strInfoDump[256] = {0};
	if(pSample == NULL)
	{
		return;
	}


	pFileDump = m_pFileData;
	if(pFileDump != NULL)
	{
		fwrite(pSample->Buffer, 1, pSample->Size, pFileDump);
		fflush(pFileDump);
	}

	memset(strInfoDump, 0, 256);
	pFileDump = m_pFileTimeStamp_Size;
	if(pFileDump != NULL)
	{
		sprintf(strInfoDump, "%d", (VO_U32)pSample->Time);
		fwrite(strInfoDump, 1, strlen(strInfoDump), pFileDump);
		memset(strInfoDump, 0, 256);
		sprintf(strInfoDump, "        %d\n", pSample->Size);
		fwrite(strInfoDump, 1, strlen(strInfoDump), pFileDump);
		fflush(pFileDump);
	}
#endif
}

VO_VOID	CTsTrack::CloseAllFileDump()
{
#ifdef _DUMP_FRAME
    if(m_pFileData != NULL)
    {
		fclose(m_pFileData);
    }
    
    if(m_pFileTimeStamp_Size != NULL)
    {
        fclose(m_pFileTimeStamp_Size);
	}
#endif
}


VO_VOID	CTsTrack::CacheFrameForThumbnail(VO_MTV_FRAME_BUFFER* pBuf)
{
    CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();
    VO_SOURCE_THUMBNAILINFO*   pBuffer = NULL;
    VO_BYTE*               pData = NULL;

    if(m_ulCurIFrameCount >= (TS_MAX_I_FRAME_COUNT-1))
	{
		return;
	}

	pBuffer = new VO_SOURCE_THUMBNAILINFO;
    if(pBuffer == NULL)
	{
		return;
	}

    memset(pBuffer, 0, sizeof(VO_SOURCE_THUMBNAILINFO));
    pBuffer->uSampleSize = pBuf->nSize;
	pBuffer->ullFilePos = pBuf->nPos;
    pBuffer->ullTimeStamp = pBuf->nStartTime;
	pBuffer->uPrivateData = m_ulCurIFrameCount;

    if(pDataParser != NULL)
    {
		pBuffer->ullFilePos = pDataParser->GetCurrParseFilePos();
    }


	pData = new VO_BYTE[pBuf->nSize];
	memcpy(pData, pBuf->pData, pBuf->nSize);
    pBuffer->pSampleData = pData;
	m_pFrameArrayForThumbnail[m_ulCurIFrameCount] = pBuffer;
	m_ulCurIFrameCount++;
	return;
}

VO_VOID	CTsTrack::TrackGenerateThumbnail()
{
	VO_U32   ulPreIFrameCount = m_ulCurIFrameCount;

	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();

	while(m_ulCurIFrameCount < (ulPreIFrameCount+2) && (pDataParser->IsParseEnd() != VO_TRUE) )
	{
		if(!pDataParser->Step())
			pDataParser->SetParseEnd(VO_TRUE);
	}

	return;
}

VO_U32	CTsTrack::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbnail)
{
    VO_U32   ulPreIFrameCount = 0;
    VO_U32   ulIndex = 0;

	if(pThumbnail == NULL)
	{
		return VO_ERR_SOURCE_ERRORDATA;
	}


    ulPreIFrameCount = m_ulCurIFrameCount;
	VOLOGI("Current I Frame Count:%d, Current Output I Frame Count:%d, Flag:%d",m_ulCurIFrameCount, m_ulIFrameCurIndex, pThumbnail->uFlag);
	switch(pThumbnail->uFlag)
	{
        case VO_SOURCE_THUMBNAILMODE_INFOONLY:
		{
			VOLOGI("Get the %d I Frame!", m_ulIFrameCurIndex);
			if(m_ulIFrameCurIndex == 0)
			{
                TrackGenerateThumbnail();
				if(m_ulCurIFrameCount == 0)
				{
					VOLOGI("Can't get the IFrame!");
					return VO_ERR_SOURCE_ERRORDATA;
				}
				else
				{
					pThumbnail->ullTimeStamp = m_pFrameArrayForThumbnail[0]->ullTimeStamp;
					pThumbnail->uSampleSize = m_pFrameArrayForThumbnail[0]->uSampleSize;
					pThumbnail->pSampleData = m_pFrameArrayForThumbnail[0]->pSampleData;
					pThumbnail->ullFilePos = m_pFrameArrayForThumbnail[0]->ullFilePos;
					pThumbnail->uPrivateData = m_pFrameArrayForThumbnail[0]->uPrivateData;
					VOLOGI("Current I Frame Count:%d, Current Output I Frame Count:%d",m_ulCurIFrameCount, m_ulIFrameCurIndex);
					VOLOGI("The private data:%d, size:%d", m_pFrameArrayForThumbnail[0]->uPrivateData, m_pFrameArrayForThumbnail[0]->uSampleSize);
					m_ulIFrameCurIndex = 1;
				}
			}
			else
			{
				if(m_ulIFrameCurIndex <=(m_ulCurIFrameCount-1))
				{
					pThumbnail->ullTimeStamp = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->ullTimeStamp;
					pThumbnail->uSampleSize = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uSampleSize;
					pThumbnail->pSampleData = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->pSampleData;
					pThumbnail->ullFilePos = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->ullFilePos;
					pThumbnail->uPrivateData = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uPrivateData;

					VOLOGI("The private data:%d, size:%d", m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uPrivateData, m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uSampleSize);
					VOLOGI("Current I Frame Count:%d, Current Output I Frame Count:%d",m_ulCurIFrameCount, m_ulIFrameCurIndex);
					m_ulIFrameCurIndex++;
				}
				else
				{
					TrackGenerateThumbnail();
					if(ulPreIFrameCount == m_ulCurIFrameCount)
					{
						return VO_ERR_SOURCE_END;
					}
					pThumbnail->ullTimeStamp = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->ullTimeStamp;
					pThumbnail->uSampleSize = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uSampleSize;
					pThumbnail->pSampleData = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->pSampleData;
					pThumbnail->ullFilePos = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->ullFilePos;
					pThumbnail->uPrivateData = m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uPrivateData;

					VOLOGI("The private data:%d, size:%d", m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uPrivateData, m_pFrameArrayForThumbnail[m_ulIFrameCurIndex]->uSampleSize);
					VOLOGI("Current I Frame Count:%d, Current Output I Frame Count:%d",m_ulCurIFrameCount, m_ulIFrameCurIndex);

					m_ulIFrameCurIndex++;
				}
			}

			break;
		}
		case VO_SOURCE_THUMBNAILMODE_DATA:
		{
			VOLOGI("the Index of I Frame:%d", pThumbnail->uPrivateData);
            for(ulIndex=0; ulIndex<m_ulCurIFrameCount; ulIndex++)
			{
				if(m_pFrameArrayForThumbnail[ulIndex]->uPrivateData == pThumbnail->uPrivateData)
				{
					pThumbnail->ullTimeStamp = m_pFrameArrayForThumbnail[ulIndex]->ullTimeStamp;
					pThumbnail->uSampleSize = m_pFrameArrayForThumbnail[ulIndex]->uSampleSize;
					pThumbnail->pSampleData = m_pFrameArrayForThumbnail[ulIndex]->pSampleData;
					pThumbnail->ullFilePos = m_pFrameArrayForThumbnail[ulIndex]->ullFilePos;
					pThumbnail->uPrivateData = m_pFrameArrayForThumbnail[ulIndex]->uPrivateData;
					break;
				}
			}

			break;
		}

		default:
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}
	}

	return VO_ERR_SOURCE_OK;
}


VO_VOID	CTsTrack::DeleteAllThumbnail()
{
	VO_U32  ulIndex = 0;
	for(ulIndex=0; ulIndex<m_ulCurIFrameCount; ulIndex++)
	{
		if(m_pFrameArrayForThumbnail[ulIndex] != NULL )
		{
			if(m_pFrameArrayForThumbnail[ulIndex]->pSampleData != NULL)
			{
				delete[] m_pFrameArrayForThumbnail[ulIndex]->pSampleData;
				m_pFrameArrayForThumbnail[ulIndex]->pSampleData = NULL;
			}

            delete    (m_pFrameArrayForThumbnail[ulIndex]);
            m_pFrameArrayForThumbnail[ulIndex] = NULL;
		}
	}
}
