
#include "CMKVTrack.h"
#include "CMKVFileReader.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define MAX_READ_BUFFER_RETRY_COUNT 3 ///< can be changed due to the peformance,3 for pentech project

CMKVTrack::CMKVTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pTrackInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp)
: CBaseStreamFileTrack( nType , btStreamNum , dwDuration , pReader , pMemOp )
,m_pTrackInfo( pTrackInfo )
,m_pTempFrameIndexListHead(0)
,m_pTempFrameIndexListTail(0)
,m_pTempFrameLastGroupPointer(0)
,m_BlocksInCache(0)
,m_FramesAvailable(0)
,m_IsFileReadEnd(VO_FALSE)
,m_IsGetThumbNail(VO_FALSE)
,m_nPosHasBeenParsed(0)
{
	m_bInUsed = VO_TRUE;
}

CMKVTrack::~CMKVTrack()
{
	;
}

VO_BOOL CMKVTrack::OnReceivedData( MKVFrame * pFrame )
{
	if(!CanReveiveDataFromParser()){
		return VO_FALSE;
	}

	if(!IsInUsed())
		return VO_FALSE;

	if(!m_pGlobeBuffer->HasIdleBuffer((VO_U32)pFrame->framesize))	//buffer lack
		return VO_FALSE;

	VOLOGI("pFrame->framesize=%llu",pFrame->framesize&0x7fffffff);
	NewFrameArrived( pFrame );
	if (m_nType == VOTT_AUDIO || m_IsGetThumbNail)
	{
		CheckCacheAndTransferTempData();
	}

	return VO_TRUE;
}

VO_VOID CMKVTrack::Notify( DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam )
{
	if(!CanReveiveDataFromParser()){
		return;
	}

	switch ( NotifyID )
	{
	case NewBlock:
		{
			VO_S64 *tracknumber = (VO_S64*)wParam;

			if( m_pTrackInfo->TrackNumber == *tracknumber )
				NewBlockStarted();
		}
		break;
	case NewCluser:
		{
			NewClusterStarted();
		}
		break;
	case NewClusterTimeCode:
		{
			VO_S64 *timecode = (VO_S64*)wParam;
			NewClusterTimeCodeArrived( *timecode );
			CheckCacheAndTransferTempData();
		}
		break;
	case End:
		{
			m_IsFileReadEnd = VO_TRUE;
			FileReadEnd();
			CheckCacheAndTransferTempData();
		}
		break;
	case NewBlockGroup:
		{
			CheckCacheAndTransferTempData();
			NewBlockGroupStarted();
		}
		break;
	case NewReferenceBlock:
		{
			NewReferenceBlockArrived();
		}
		break;
	case SimpleBlock:
		{
			CheckCacheAndTransferTempData();
		}
		break;
	}
}

VO_VOID CMKVTrack::CheckCacheAndTransferTempData()
{
	int number_to_transfer = 0;

	if (m_IsGetThumbNail)
	{
		number_to_transfer = m_FramesAvailable;
	}
	else
	{
		if( m_IsFileReadEnd == VO_TRUE )
		{
			number_to_transfer = CalculateTimeCode( 0xffffffff );
		}
		else if(m_FramesAvailable >= 2)
		{
			number_to_transfer = CalculateTimeCode( m_FramesAvailable );
		}
	}
	TransferTempData( number_to_transfer );
}

VO_U32 CMKVTrack::CalculateTimeCode( VO_U32 count )
{
	if( !m_pTempFrameIndexListHead )
		return 0;

	MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;
	VO_U32 calculated = 0;

	MKVMediaSampleIndexEntry * pNext = ( MKVMediaSampleIndexEntry * )pEntry->next;

	while( pEntry )
	{
		VO_U32 offsetcount = 1;

		while( pNext && pNext->time_stamp == pEntry->time_stamp && ( calculated + offsetcount <= count ) )
		{
			pNext = ( MKVMediaSampleIndexEntry * )pNext->next;
			offsetcount++;
		}

		if( calculated + offsetcount >= count )
			break;

		VO_U32 average = 0;
		if( pNext )
			average = ( pNext->time_stamp - pEntry->time_stamp ) / offsetcount;

		for( VO_U32 i = 0 ; i < offsetcount ; i++ )
		{
			pEntry->time_stamp = pEntry->time_stamp + average * i;
			pEntry = ( MKVMediaSampleIndexEntry * )pEntry->next;
		}

		calculated = calculated + offsetcount;
		m_BlocksInCache--;

		if( pEntry )
			pNext = ( MKVMediaSampleIndexEntry * )pEntry->next;
	}

	return calculated;
}

VO_VOID CMKVTrack::TransferTempData( VO_U32 count )
{
	MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;

	for( VO_U32 i = 0 ; i < count ; i++ )
	{
		MKVMediaSampleIndexEntry * pTemp = pEntry;
		pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;

		if(pTemp->next)
			((MKVMediaSampleIndexEntry *)pTemp->next)->pre = NULL;
		pTemp->next = NULL;
		pTemp->pre = NULL;

		m_pStreamFileIndex->Add( pTemp );
	}

	m_pTempFrameIndexListHead = pEntry;

	if( pEntry == NULL )
		m_pTempFrameIndexListHead = m_pTempFrameIndexListTail = NULL;

	m_FramesAvailable -= count;
}

VO_U32 CMKVTrack::Prepare()
{
	m_pStreamFileIndex = new CMKVStreamFileIndex(m_pStreamFileReader->GetMemPoolPtr());
	InitGlobalBuffer();

	return CBaseTrack::Prepare();
}

VO_BOOL CMKVTrack::OnBlockWithFilePos( VO_U64 timestamp )
{
	return OnBlock( (VO_U32)timestamp );
}

VO_VOID CMKVTrack::Flush()
{
	m_IsFileReadEnd = VO_FALSE;
	m_FramesAvailable = 0;
	m_BlocksInCache = 0;
	m_pTempFrameLastGroupPointer = VO_NULL;

	MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;

	while( pEntry )
	{
		MKVMediaSampleIndexEntry * pTemp = pEntry;
		pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;

		((CMKVStreamFileIndex*)m_pStreamFileIndex)->DeleteEntry( pTemp );
	}

	m_pTempFrameIndexListHead = m_pTempFrameIndexListTail = VO_NULL;
	
	return CBaseStreamFileTrack::Flush();
}

VO_U32 CMKVTrack::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	*pllPreviousKeyframeTime = 0xffffffffffffffffll;
	*pllNextKeyframeTime = 0xffffffffffffffffll;

	if (m_nType != VOTT_VIDEO || m_pSegInfo == NULL)
	{
		return VO_ERR_NOT_IMPLEMENT;
	}
	
	if( !IsListEmpty( &(m_pSegInfo->CuesCuePointEntryListHeader) ) )
	{
		voLIST_ENTRY * pEntry = m_pSegInfo->CuesCuePointEntryListHeader.Flink;

		while( pEntry != &(m_pSegInfo->CuesCuePointEntryListHeader) )
		{
			CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );

			if( pCue->CueTime * m_pSegInfo->TimecodeScale / 1000000. >= llTime )
			{
				if( pCue->CueTime * m_pSegInfo->TimecodeScale / 1000000. > llTime )
				{
					break;
				}
				*pllPreviousKeyframeTime = llTime;
			}

			pEntry = pEntry->Flink;
		}

		if( pEntry != &(m_pSegInfo->CuesCuePointEntryListHeader)/*  && llTimeStamp < MaxCueTime*/)
		{
			CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
			*pllNextKeyframeTime = (VO_S64)(pCue->CueTime * m_pSegInfo->TimecodeScale / 1000000.);

			if ((*pllPreviousKeyframeTime == (VO_S64)0xffffffffffffffffll) && pEntry->Blink != &(m_pSegInfo->CuesCuePointEntryListHeader))
			{
				CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry->Blink , CuesCuePointStruct , List );
				*pllPreviousKeyframeTime = (VO_S64)(pCue->CueTime * m_pSegInfo->TimecodeScale / 1000000.);
			}
			return VO_ERR_NONE;

		}
		
		if(*pllPreviousKeyframeTime == (VO_S64)0xffffffffffffffffll)
		{
			return VO_ERR_FAILED;
		}
		else
		{
			return VO_ERR_NONE;
		}
		
	}
	return VO_ERR_NOT_IMPLEMENT;
}
//VO_U32 CMKVTrack::SetPosN(VO_S64* pPos)
//{
//	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
//	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//	if(-1 == nRes)
//		return VO_ERR_SOURCE_END;
//	else if(1 == nRes)	//retry
//	{
//		VO_BOOL bParseEnd = VO_FALSE;
//		while(1 == nRes && !bParseEnd)	//retry!!
//		{
//
//			m_pGlobeBuffer->RemoveFrom( pGet->pos_in_buffer & 0x7fffffff );
//			m_pStreamFileIndex->RemoveAll();
//
//			if(!TrackGenerateIndex())
//				bParseEnd = VO_TRUE;
//
//			nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//			if(-1 == nRes)
//				return VO_ERR_SOURCE_END;
//		}
//
//		if(0 != nRes)
//			return VO_ERR_SOURCE_END;
//
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//		TrackGenerateIndex();
//	}
//	else	//ok
//	{
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//		m_pStreamFileReader->FileGenerateIndex();
//	}
//
//	return VO_ERR_SOURCE_OK;
//}



VO_U32 CMKVTrack::SetPosN(VO_S64* pPos, VO_BOOL bflag)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = 0;
	if(bflag){
		nRes = m_pStreamFileIndex->GetEntryByTime(VO_FALSE, *pPos, &pGet);
	}else{
		nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
	}
	
	if(-1 == nRes)
	{
		return VO_ERR_SOURCE_END;
	}
	else if(1 == nRes)	//retry
	{
		VO_BOOL bParseEnd = VO_FALSE;
		VO_U32 nCntReTry = 0;///<not supposed to retry infinitely,it will be cost too much time slice once data error
		while(1 == nRes && !bParseEnd && nCntReTry < MAX_READ_BUFFER_RETRY_COUNT)	//retry!!
		{
			nCntReTry++;
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
		{
			return VO_ERR_SOURCE_END;
		}
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

VO_BOOL CMKVTrack::GetCurrentTimeStamp(VO_U32* timestamp)
{
	VO_BOOL bResult = VO_TRUE;
	PBaseStreamMediaSampleIndexEntry  pEntry;

	*timestamp = m_pStreamFileIndex->GetCurrStartTime();

	return bResult;
}

VO_VOID CMKVTrack:: SetPosHasBeenParsed()
{
	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();
	if (pDataParser)
	{
		m_nPosHasBeenParsed = pDataParser->GetCurrParseFilePos();
	}

}

VO_BOOL CMKVTrack:: CanReveiveDataFromParser()
{
	VO_BOOL bResult = VO_FALSE;
	VO_U32 pos = 0;

	//return vo_TRUE directly.
	if(0 == m_nPosHasBeenParsed){
		return VO_TRUE;
	}
	
	CvoFileDataParser* pDataParser = m_pDataParser ? m_pDataParser : m_pStreamFileReader->GetFileDataParserPtr();

	if (pDataParser){
		pos = pDataParser->GetCurrParseFilePos();
	}

	if(pos >= m_nPosHasBeenParsed){
		//set the pos to zero when parser reach the pos.
		m_nPosHasBeenParsed = 0;
		bResult = VO_TRUE;
	}

	return bResult;

}

VO_BOOL CMKVTrack::PrintIndex()
{
	VO_BOOL bResult = VO_TRUE;

	((CMKVStreamFileIndex*)m_pStreamFileIndex)->PrintIndex();

	return bResult;
}

