#include "CBaseStreamFileTrack.h"
#include "CBaseStreamFileReader.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseStreamFileTrack::CBaseStreamFileTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseTrack(nType, btStreamNum, dwDuration, pMemOp)
	, m_pStreamFileReader(pReader)
	, m_pStreamFileIndex(VO_NULL)
	, m_pGlobeBuffer(VO_NULL)
	, m_dwBufferTime(5)
	, m_dwGBExtSize(0x10000)
	, m_pDataParser(VO_NULL)
	, m_bParseForSelf(VO_FALSE)
	, m_dwFileIndexInFastPlay(0)
	, m_ullCurrParseFilePos(-1)
{
}

CBaseStreamFileTrack::~CBaseStreamFileTrack()
{
	SAFE_DELETE(m_pGlobeBuffer);
	SAFE_DELETE(m_pDataParser);
	SAFE_DELETE(m_pStreamFileIndex);
}

VO_U32 CBaseStreamFileTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bFrameDropped = VO_FALSE;
	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, pSample->Time, &pGet, &bFrameDropped))
		return VO_ERR_SOURCE_END;

	if(bFrameDropped)
		pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	VO_U32 dwMaxSampleSize = VO_MAXU32;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		m_pGlobeBuffer->RemoveTo(pGet->pos_in_buffer & 0x7FFFFFFF);
	}
	else
	{
		pSample->Size = pGet->size;
		if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
			pSample->Buffer = m_pSampleData;

		if(pGet->IsKeyFrame())
			pSample->Size |= 0x80000000;
	}

	m_pStreamFileIndex->RemoveInclude(pGet);
	TrackGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseStreamFileTrack::SetPosN(VO_S64* pPos)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
	if(-1 == nRes)
		return VO_ERR_SOURCE_END;
	else if(1 == nRes)	//retry
	{
		VO_BOOL bParseEnd = VO_FALSE;
		while(1 == nRes && !bParseEnd)	//retry!!
		{
			//modified by yyang 20091215
			//if(m_pGlobeBuffer)
			//m_pGlobeBuffer->Flush();
			if(m_pGlobeBuffer)
				m_pGlobeBuffer->RemoveTo2(pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size);
			m_pStreamFileIndex->RemoveAll();

			if(!TrackGenerateIndex())
				bParseEnd = VO_TRUE;

			nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
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

VO_U32 CBaseStreamFileTrack::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	if(0xffffffff == m_dwFileIndexInFastPlay)
		return VO_ERR_SOURCE_END;

	VO_U64 ullFilePos = GetFilePosByFileIndex(m_dwFileIndexInFastPlay);
	if(0xffffffffffffffffll == ullFilePos)
		return VO_ERR_SOURCE_END;

	Flush();

	//do parse until get key frame entry!!
	CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
	if(!pDataParser->SetStartFilePos(ullFilePos))
		return VO_ERR_SOURCE_END;

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bRet = VO_FALSE;
	while(!bRet)
	{
		if(!pDataParser->Step())
			return VO_ERR_SOURCE_END;

		bRet = m_pStreamFileIndex->GetKeyFrameEntry(&pGet);
	}

	//sample ready!!
	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	VO_U32 dwMaxSampleSize = 0;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		return VO_ERR_SOURCE_OK;
	}

	pSample->Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
		pSample->Buffer = m_pSampleData;

	pSample->Size |= 0x80000000;

	//not forget update state!!
	if(VO_SOURCE_PM_FF == m_PlayMode)
		m_dwFileIndexInFastPlay++;
	else
		m_dwFileIndexInFastPlay--;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseStreamFileTrack::SetPosK(VO_S64* pPos)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32 dwFileIndex = GetFileIndexByTime((VO_SOURCE_PM_FF == m_PlayMode) ? VO_TRUE : VO_FALSE, *pPos);

	if( 0xffffffff == dwFileIndex)
		return VO_ERR_SOURCE_PLAYMODEUNSUPPORT;

	m_dwFileIndexInFastPlay = dwFileIndex;
	return VO_ERR_SOURCE_OK;
}

VO_U8 CBaseStreamFileTrack::IsCannotGenerateIndex(CvoFileDataParser* pDataParser)
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

VO_BOOL CBaseStreamFileTrack::TrackGenerateIndexB(CvoFileDataParser* pDataParser)
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

VO_BOOL CBaseStreamFileTrack::TrackGenerateIndex()
{
	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();
	VO_BOOL bNoEnd = TrackGenerateIndexB(pDataParser);
	if(m_pStreamFileIndex->IsNull() && bNoEnd)	//track buffer null but file parse not end, so need new data parser to parse
	{
		VOLOGI("tracks force to use each data parser!!");

		if(!m_pDataParser)
			m_pStreamFileReader->OnNewDataParser(this, &m_pDataParser);

		bNoEnd = TrackGenerateIndexB(m_pDataParser);
	}

	return bNoEnd;
}

VO_BOOL CBaseStreamFileTrack::SetBufferTime(VO_U32 dwBufferTime)
{
	m_dwBufferTime = dwBufferTime;

	return VO_TRUE;
}

VO_BOOL CBaseStreamFileTrack::SetGlobalBufferExtSize(VO_U32 dwExtSize)
{
	m_dwGBExtSize = dwExtSize;

	return VO_TRUE;
}

VO_BOOL CBaseStreamFileTrack::InitGlobalBuffer()
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

	return m_pGlobeBuffer->Init(VO_MAX(dwBitrate, dwMaxSampleSize) * m_dwBufferTime + m_dwGBExtSize);
}

VO_U32 CBaseStreamFileTrack::Prepare()
{
	m_pStreamFileIndex = new CBaseStreamFileIndex(m_pStreamFileReader->GetMemPoolPtr());
	InitGlobalBuffer();

	return CBaseTrack::Prepare();
}

VO_U32 CBaseStreamFileTrack::Unprepare()
{
	Flush();

	SAFE_DELETE(m_pGlobeBuffer);
	SAFE_DELETE(m_pStreamFileIndex);

	return CBaseTrack::Unprepare();
}

VO_VOID CBaseStreamFileTrack::Flush()
{
	m_ullCurrParseFilePos = -1;

	SAFE_DELETE(m_pDataParser);
	m_bParseForSelf = VO_FALSE;

	if( m_pStreamFileIndex )
		m_pStreamFileIndex->RemoveAll();

	if(m_pGlobeBuffer)
		m_pGlobeBuffer->Flush();
}

VO_U32 CBaseStreamFileTrack::GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	if(!m_pStreamFileIndex->GetKeyFrameEntry(&pGet))
		return VO_ERR_SOURCE_NEEDRETRY;

	pSample->Size = pGet->size;
	pSample->Size |= 0x80000000;

	pSample->Time = pGet->time_stamp;
	pSample->Duration = 1;

	return VO_ERR_SOURCE_OK;
}

VO_VOID CBaseStreamFileTrack::SetNewDataParser(CvoFileDataParser* pNewDataParser)
{
	SAFE_DELETE(m_pDataParser);
	m_pDataParser = pNewDataParser;
}

VO_BOOL CBaseStreamFileTrack::OnBlock(VO_U32 dwTimeStamp)
{
	//30% is the limit value!!
	if(dwTimeStamp < m_pStreamFileIndex->GetCurrStartTime() + m_dwBufferTime * 300 || dwTimeStamp > m_pStreamFileIndex->GetCurrEndTime())
		return VO_TRUE;

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	if(0 != m_pStreamFileIndex->GetEntryByTime(VO_FALSE, dwTimeStamp, &pGet))
		return VO_TRUE;

	VOLOGI("tracks can use one data parser again!!");

	//record current data parser file position instead of remove media sample
	//East 2009/12/07
//	m_pGlobeBuffer->RemoveFrom(pGet->pos_in_buffer & 0x7FFFFFFF);
//	m_pStreamFileIndex->RemoveFrom(pGet);
	m_ullCurrParseFilePos = m_pDataParser->GetCurrParseFilePos();

	SAFE_DELETE(m_pDataParser);
	m_pStreamFileReader->OnDelDataParser(this);

	return VO_FALSE;
}
