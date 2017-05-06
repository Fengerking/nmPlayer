
#include "CCmmbTrack.h"
#include "fMacros.h"
#include "fCC.h"
#include "voLog.h"
#include "CCmmbFileDataParser.h"
#include "CMTVStreamFileIndex.h"
#include "CDumper.h"
#include "CAnalyseH264.h"

CCmmbTrack::CCmmbTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CMTVReader* pReader, VO_MEM_OPERATOR* pMemOp)
: CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
, m_pReader(pReader)
, m_pHeadData(VO_NULL)
{
}

CCmmbTrack::~CCmmbTrack(void)
{
	if (m_pHeadData)
	{
		MemFree(m_pHeadData);
		m_pHeadData = VO_NULL;
	}

	CBaseTrack::Unprepare();
}


VO_U32 CCmmbTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	// tag: 20100616
	//*pdwMaxSampleSize = 128 * 1024;

	if (m_nType == VOTT_VIDEO)
	{
		VO_VIDEO_FORMAT fmt;
		GetVideoFormat(&fmt);
		*pdwMaxSampleSize = (fmt.Width*fmt.Height) >> 1;
	}
	else
	{
		VO_AUDIO_FORMAT fmt;
		GetAudioFormat(&fmt);
		*pdwMaxSampleSize = 1024;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::GetCodecCC(VO_U32* pCC)
{
// 	CMMB_FILE_INFO* pInfo = ((CCmmbFileDataParser*)m_pDataParser)->GetFileInfo();
// 	
// 	if (pInfo->nDuration > 0)
// 	{
// 		if (m_bVideo)
// 		{
// 			*pCC = FOURCC_H264;
// 		}
// 		else
// 		{
// 			if(pInfo->nAudioCodec == VO_AUDIO_CodingAAC)
// 				*pCC = AudioFlag_AAC;
// 			else if (pInfo->nAudioCodec == VO_AUDIO_CodingDRA)
// 			{
// 				// here has issue
// 				*pCC = AudioFlag_AAC;
// 			}
// 
// 		}
// 	}
// 	else
	{
		if (m_nType == VOTT_VIDEO)
		{
			*pCC = FOURCC_H264;
		}
		else
		{
			*pCC = 0xA106;// raw data
			//*pCC = 0xFF;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	if (m_nType == VOTT_VIDEO)
	{
		pHeadData->Buffer	= VO_NULL;
		pHeadData->Length	= 0;
		pHeadData->Time		= m_dwDuration;
	}
	else
	{
		if (!m_pHeadData)
			m_pHeadData = (VO_BYTE*)MemAlloc(2);

		m_pHeadData[0] = 0x13;
		m_pHeadData[1] = 0x10;

		pHeadData->Buffer	= m_pHeadData;
		pHeadData->Length	= 2;
		pHeadData->Time		= m_dwDuration;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::GetCodec(VO_U32* pCC)
{
	if (m_nType == VOTT_VIDEO)
	{
		*pCC = VO_VIDEO_CodingH264;
	}
	else
		*pCC = VO_AUDIO_CodingAAC;


	return VO_ERR_SOURCE_OK;
}



VO_U32 CCmmbTrack::GetBitrate(VO_U32* pdwBitrate)
{
	VO_U32 n = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::Init(VO_U32 nSourceOpenFlags)
{

	//dwExtSize = 0;

	// 20100610
// 	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
// 		InitGlobalBuffer(10, 1024*128 + ((dwExtSize + 0x3) & ~0x3));

	//SetBufferTime(10);
	SetGlobalBufferExtSize(1024*128);

	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
		InitGlobalBuffer();

	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
		m_pStreamFileIndex = new CMTVStreamFileIndex(m_pStreamFileReader->GetMemPoolPtr());

	CBaseTrack::Prepare();

	return CBaseTrack::Init(nSourceOpenFlags);
}


//GetSample and SetPos implement of normal mode!!
VO_U32 CCmmbTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bFrameDropped = VO_FALSE;
	if(!m_pStreamFileIndex->GetEntry(m_nType==VOTT_VIDEO?VO_TRUE:VO_FALSE, pSample->Time, &pGet, &bFrameDropped))
	{
// 		TrackGenerateIndex();
// 		CDumper::WriteLog("GetSampleN error.");
// 		if(!m_pStreamFileIndex->GetEntry(m_nType==VOTT_VIDEO?VO_TRUE:VO_FALSE, pSample->Time, &pGet, &bFrameDropped))
			return VO_ERR_SOURCE_END;
	}
	if(bFrameDropped)
		pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

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

	if(m_nType==VOTT_AUDIO)
	{
		//CDumper::DumpAudioData(pSample->Buffer, pSample->Size);
		//CDumper::PrintMemory(pSample->Buffer, 6);
	}
	else
	{
		//CDumper::DumpVideoData(pSample->Buffer, pSample->Size);
		//CDumper::PrintMemory(pSample->Buffer, 6);
	}
		

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

VO_U32 CCmmbTrack::SetPosN(VO_S64* pPos)
{
	CDumper::WriteLog("%s track set posn: %llu", (m_nType == VOTT_VIDEO)?"Video":"Audio", *pPos);
	return CBaseStreamFileTrack::SetPosN(pPos);


	m_pReader->OnSetPos(pPos);

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime(m_nType==VOTT_VIDEO?VO_TRUE:VO_FALSE, *pPos, &pGet);
 	
	if(-1 == nRes)
	{
		return VO_ERR_SOURCE_END;
	}
	else if(1 == nRes)	//retry
	{

		VO_BOOL bParseEnd = VO_FALSE;
		while(1 == nRes && !bParseEnd)	//retry!!
		{
			m_pStreamFileIndex->RemoveAll();
			if(m_pGlobeBuffer)
				m_pGlobeBuffer->Flush();

			if(!TrackGenerateIndex())
				bParseEnd = VO_TRUE;

			nRes = m_pStreamFileIndex->GetEntryByTime(m_nType == VOTT_VIDEO?VO_TRUE:VO_FALSE, *pPos, &pGet);
			if(-1 == nRes)
				return VO_ERR_SOURCE_END;
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
VO_U32 CCmmbTrack::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	VOLOGI("GetSampleK Time:%u", pSample->Time);

// 	if (m_pReader)
// 	{
// 		if (m_bVideo)
// 		{
// 			CBaseScanner* pScanner = m_pReader->GetScanner();
// 
// 			if (pScanner)
// 			{
// 				pScanner->GetSampleK(pSample);
// 			}
// 			else
// 				return VO_ERR_SOURCE_OPENFAIL;
// 		}
// 	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::SetPosK(VO_S64* pPos)
{
	VOLOGI("set posk: %d", *pPos);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = 2;
	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = 24000;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	pVideoFormat->Width = 320;
	pVideoFormat->Height = 240;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CCmmbTrack::OnData(CMMB_FRAME_BUFFER* pBuf)
{
	VO_PBYTE addr = NULL;
	VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&addr, pBuf->nDataLen);

	if (!addr)
	{
		return VO_ERR_SOURCE_ERRORDATA;
	}

	VO_U64 ts	= pBuf->nTimeStamp;
	ts			= (ts*1000)/22500;

	// test code
	if (m_nType == VOTT_AUDIO)
	{
		//CDumper::DumpAudioData(pBuf->pData, pBuf->nDataLen);
	}
	else
	{
		//CDumper::WriteLog("Recv %s frame, ts = %llu, size = %d...", (m_nType == VOTT_VIDEO)?"video":"audio", ts, pBuf->nDataLen);
		//CDumper::DumpVideoData(pBuf->pData, pBuf->nDataLen);
	}
	// end


	m_pGlobeBuffer->DirectWrite2(pBuf->pData, addr, 0, pBuf->nDataLen);
	PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry(ts, dwPosInBuffer, pBuf->nDataLen);
	
	if(!pNew)
		return VO_ERR_SOURCE_ERRORDATA;

	pNew->SetKeyFrame(pBuf->bKeyFrame);
	m_pStreamFileIndex->Add(pNew);

	return VO_ERR_SOURCE_OK;
}

