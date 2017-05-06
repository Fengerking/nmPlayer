#include "CMpegReader.h"
#include "CMpegTrack.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMpegTrack::CMpegTrack(VO_TRACKTYPE nType, VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader* pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID)
	: CBaseStreamFileTrack(nType, btStreamID, dwDuration, pReader, pMemOp)
	, m_pReader(pReader)
	, m_dwTimeOffset(dwTimeOffset)
	, m_dwBitrate(-1)
	, m_bSetExtData(VO_FALSE)
	, m_pActPESBuffer(VO_NULL)
	, m_TmpStampEntryListHead(VO_NULL)
	, m_TmpStampEntryListTail(VO_NULL)
	, m_ValidTimeStamp(-1)
	, m_wUnvalidTimeStampEntry(0)
	, m_btSubStreamNum(btSubStreamID)
{
}

CMpegTrack::~CMpegTrack()
{
	SAFE_MEM_FREE(m_pActPESBuffer);
}

VO_U32 CMpegTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = 0;
	pHeadData->Buffer = VO_NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = (0xFFFFFFFF == m_dwBitrate) ? m_pReader->GetBitrate() : m_dwBitrate;

	return VO_ERR_SOURCE_OK;
}

VO_VOID	CMpegTrack::Flush()
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
VO_VOID CMpegTrack::Flush2()
{
	if( m_pStreamFileIndex )
		m_pStreamFileIndex->RemoveAll();

	if(m_pGlobeBuffer)
		m_pGlobeBuffer->Flush();
}
VO_BOOL CMpegTrack::CheckPESBuffer()
{
	if(!m_pActPESBuffer)
	{
		m_pActPESBuffer = (VO_PBYTE)MemAlloc(VO_MAXU16);
		if(!m_pActPESBuffer)
			return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL	CMpegTrack::CalculateTimeStamp(PBaseStreamMediaSampleIndexEntry pNewEntry)
{
	if(!pNewEntry)
		return VO_FALSE;
	///<debug
	if(0xFFFFFFFF == pNewEntry->time_stamp || (m_ValidTimeStamp == pNewEntry->time_stamp))
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
			VO_S64 dwTimeStep = (pNewEntry->time_stamp - m_ValidTimeStamp) / (m_wUnvalidTimeStampEntry + 1);
			PBaseStreamMediaSampleIndexEntry pTmpEntry = m_TmpStampEntryListHead;
			for(VO_U32 i = 1; pTmpEntry && (i <= m_wUnvalidTimeStampEntry); i++)
			{
				pTmpEntry->time_stamp = m_ValidTimeStamp + i * dwTimeStep;
				pTmpEntry = pTmpEntry->next;
			}
		}

		m_ValidTimeStamp = pNewEntry->time_stamp;
		m_wUnvalidTimeStampEntry = 0;
		return VO_TRUE;
	}
}
VO_BOOL CMpegTrack::PushTmpSampleToStreamFile(PBaseStreamMediaSampleIndexEntry pNewEntry)
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

VO_U32 CMpegTrack::Init(VO_U32 nSourceOpenFlags)
{
	CMpegDataParser * pParser = (CMpegDataParser *)m_pStreamFileReader->GetFileDataParserPtr();
	if (pParser->GetParserMode() == VO_MPEG_PARSER_MODE_THUMBNAIL)
	{
		SetBufferTime(10);
	}
	else
		SetBufferTime(5);
	VO_U32 dwMaxSampleSize = 0;
	GetMaxSampleSize(&dwMaxSampleSize);
	SetGlobalBufferExtSize(dwMaxSampleSize);
	return CBaseStreamFileTrack::Init(nSourceOpenFlags);
}

VO_U8 CMpegTrack::IsCannotGenerateIndex(CvoFileDataParser* pDataParser)
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
		if(m_pReader->IsTrackBufferFull() || m_pReader->IsTrackBufferHasEnoughFrame())
		{	
			return 1;
		}
	}

	return 0;
}

VO_U64  CMpegTrack::GetTrackBufDuration()
{
	if (!m_pStreamFileIndex || (m_pStreamFileIndex->GetCurrEndTime() < m_pStreamFileIndex->GetCurrStartTime()))
	{
		return 0;
	}
//	VOLOGI("Time1==%d,Time2==%d",m_pStreamFileIndex->GetCurrEndTime(),m_pStreamFileIndex->GetCurrStartTime());
	return m_pStreamFileIndex->GetCurrEndTime() - m_pStreamFileIndex->GetCurrStartTime();
}

VO_U64	CMpegTrack::GetPreciseTrackBufDuration()
{
	CMpegFileIndex *pIndex = (CMpegFileIndex *)m_pStreamFileIndex;
	if (!m_pStreamFileIndex )
	{
		return 0;
	}
	VOLOGE("Duration==ll%d",pIndex->GetEntryDuration());
	return pIndex->GetEntryDuration();
}

VO_U32 CMpegTrack::Prepare()
{
	m_pStreamFileIndex = new CMpegFileIndex(m_pStreamFileReader->GetMemPoolPtr());
	InitGlobalBuffer();

	return CBaseTrack::Prepare();
}

VO_U32	CMpegTrack::GetFrameInfoInIndex(MPEGFrameInfo *info,VO_S64 llTimeStamp)
{
	CMpegFileIndex *pIndex = (CMpegFileIndex *)m_pStreamFileIndex;
	if (!m_pStreamFileIndex || !info)
	{
		return VO_ERR_SOURCE_ERRORDATA;
	}
	return pIndex->GetFrameInfo(&(info->nNearKeyFramTimeStamp),&(info->nMaxTimeStamp),&(info->nMinTimeStamp),llTimeStamp,(m_nType == VOTT_VIDEO) ? VO_TRUE : VO_FALSE);
}

VO_S64 CMpegTrack::CalcTrackTs(VO_S64 llTs)
{
	return (llTs == -1)?llTs:(llTs >= m_dwTimeOffset)?(llTs - m_dwTimeOffset):llTs;
}
