#include "CFlvReader.h"
#include "CFlvTrack.h"
#include "fCodec.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CFlvTrack::CFlvTrack(VO_TRACKTYPE nType,  VO_U8 btStreamNum, VO_U32 dwDuration, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
	, m_pReader(pReader)
	, m_lMaxSampleSize(0)
{
}

CFlvTrack::~CFlvTrack()
{
}

VO_BOOL CFlvTrack::AddSample(CGFileChunk* pFileChunk, VO_BOOL bKeyFrame, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_U64 pos)
{
	//if current content has been parsed, only skipped
	//East 2009/12/07
	if(VO_MAXU64 != m_ullCurrParseFilePos)
	{
		if(pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
			return pFileChunk->FSkip(dwLen);
		else
			m_ullCurrParseFilePos = -1;
	}

	if(!m_pGlobeBuffer->HasIdleBuffer(dwLen))	//buffer lack
		return VO_FALSE;

	if (m_pReader->IsThumbnail())
	{
		PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry( dwTimeStamp, 0, dwLen );
		if(!pNew)
			return VO_FALSE;

		pNew->pos_in_buffer = pos;
		pNew->pos_in_buffer |=0x80000000;
		pFileChunk->FLocate(pos+dwLen);
		m_pStreamFileIndex->Add(pNew);
	}
	else
	{

		VO_U32 dwPosInBuffer = m_pGlobeBuffer->Add(pFileChunk, dwLen);
		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		//PBaseStreamMediaSampleIndexEntry pNew = new BaseStreamMediaSampleIndexEntry(dwTimeStamp, (bKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), dwLen);
		PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry( dwTimeStamp, (bKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), dwLen );
		if(!pNew)
			return VO_FALSE;

		m_pStreamFileIndex->Add(pNew);

	}
	return VO_TRUE;
}

VO_BOOL CFlvTrack::Init(VO_U32 nSourceOpenFlags, VO_U32 dwExtSize)
{
	//if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
		VO_U32 dwMaxSampleSize = 0;
		GetMaxSampleSize(&dwMaxSampleSize);

		//
		SetBufferTime( 5 );
		InitGlobalBuffer();
	}

	if( VO_ERR_SOURCE_OK == CBaseStreamFileTrack::Init(nSourceOpenFlags) )
		return VO_TRUE;
	else
		return VO_FALSE;
}

VO_U32 CFlvTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = 0;
	pHeadData->Buffer = VO_NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = m_lMaxSampleSize;

	return VO_ERR_SOURCE_OK;
}