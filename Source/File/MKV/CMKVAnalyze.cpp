#include "CMKVAnalyze.h"
#include "MKVID.h"
#include "EBML.h"
#include "MKVInfoStructureFunc.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "cmnFile.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif


CMKVAnalyze::CMKVAnalyze( CGFileChunk* m_pFileChunk, VO_MEM_OPERATOR* pMemOp ):CvoBaseMemOpr(pMemOp),m_pFileChunk( m_pFileChunk )
,m_lastfilepos(0)
{
	;
}

VO_VOID CMKVAnalyze::AnalyzeMKV( VO_S64 filesize , MKVInfo * pInfo )
{
	m_pFileChunk->FLocate( 0 );
	ReadSection( &CMKVAnalyze::ProcessMainSection , filesize , pInfo );
}


void CMKVAnalyze::ReadSection( pProcessSection ProcessSection , VO_S64 SectionSize , VO_VOID * pParam )
{
	VO_S64 CurFilePos , SectionEndFilePos;

	CurFilePos = m_pFileChunk->FGetFilePos();

	SectionEndFilePos = CurFilePos + SectionSize;

	while ( 1 )
	{
		IDAndSizeInfo info;

		m_lastfilepos = m_pFileChunk->FGetFilePos();

		if( !ReadIDAndSize( &info ) )
			break;
		CurFilePos = m_pFileChunk->FGetFilePos();
		
		if(info.size > SectionEndFilePos - CurFilePos)
			info.size = SectionEndFilePos - CurFilePos;

		if( !(this->*ProcessSection)( &info , pParam ) )
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			m_pFileChunk->FLocate( pos + info.size );
		}

		MemFree(info.pID);

		CurFilePos = m_pFileChunk->FGetFilePos();

		if( CurFilePos >= SectionEndFilePos )
			break;
	}
}

VO_BOOL CMKVAnalyze::ProcessMainSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVInfo * pMKVInfo = ( MKVInfo * )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)(VO_PTR)MKVID_SEGMENT , pInfo->s_ID ) == 0 )
	{
		MKVSegmentStruct * pSegInfo = (MKVSegmentStruct*)MemAlloc( sizeof(MKVSegmentStruct) );
		MemSet( pSegInfo , 0 , sizeof( MKVSegmentStruct ) );
		InitializeMKVSegmentStruct( pSegInfo );

		/*************Get the currect segment info**********************/
		IDAndSizeInfo info;
		m_pFileChunk->FLocate(m_lastfilepos);
		if( !ReadIDAndSize( &info ) )
			return VO_FALSE;

		pSegInfo->SupposeSegmentEndPos = pSegInfo->SegmentBeginPos + info.size;
		MemFree( info.pID );
		/**********************************************************/
		
		pSegInfo->SegmentBeginPos = m_pFileChunk->FGetFilePos();
		pSegInfo->SegmentEndPos = pSegInfo->SegmentBeginPos + pInfo->size;

		ReadSection( &CMKVAnalyze::ProcessSegmentSection , pInfo->size , pSegInfo );

		InsertTailList( &(pMKVInfo->MKVSegmentListHeader) , &(pSegInfo->List) );

		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL CMKVAnalyze::ProcessSegmentSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD , pInfo->s_ID ) == 0 )
	{
		VOLOGI( "+CMKVAnalyze::ProcessSeekHeadSection %lu" , voOS_GetSysTime() );
		ReadSection( &CMKVAnalyze::ProcessSeekHeadSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessSeekHeadSection %lu" , voOS_GetSysTime() );
	}
	else if(  MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessSegmentInfoSection %lu" , voOS_GetSysTime() );
		ReadSection( &CMKVAnalyze::ProcessSegmentInfoSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessSegmentInfoSection %lu" , voOS_GetSysTime() );
	}
	else if(  MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessTracksSection %lu" , voOS_GetSysTime() );
		ReadSection( &CMKVAnalyze::ProcessTracksSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessTracksSection %lu" , voOS_GetSysTime() );
	}
	else if(  MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::ProcessCuesSection %lu" , voOS_GetSysTime() );
		ReadSection( &CMKVAnalyze::ProcessCuesSection , pInfo->size , pSegInfo );
		VOLOGI( "-CMKVAnalyze::ProcessCuesSection %lu" , voOS_GetSysTime() );
	}
	else if(  MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , pInfo->s_ID ) == 0  )
	{
		VOLOGI( "+CMKVAnalyze::MKVID_SEGMENT_CLUSTER %lu" , voOS_GetSysTime() );
		pSegInfo->ClusterPos = m_lastfilepos;

		PvoLIST_ENTRY pEntry = pSegInfo->SeekHeadElementEntryListHeader.Flink;
		for( ; pEntry != &(pSegInfo->SeekHeadElementEntryListHeader) ; pEntry = pEntry->Flink )
		{
			SeekHeadElementStruct * pElement = GET_OBJECTPOINTER( pEntry , SeekHeadElementStruct , List );

			if( IsListEmpty(&(pSegInfo->CuesCuePointEntryListHeader)) && pElement->SeekID == Cues )
			{
				if(m_pFileChunk->FLocate( pElement->SeekPos + pSegInfo->SegmentBeginPos ))
					return VO_TRUE;
				continue;
			}
			else if( IsListEmpty(&(pSegInfo->SeekHeadElementEntryListHeader)) && pElement->SeekID == SeekHead )
			{
				if(m_pFileChunk->FLocate( pElement->SeekPos + pSegInfo->SegmentBeginPos ))
					return VO_TRUE;
				continue;
			}
			else if( IsListEmpty(&(pSegInfo->TracksTrackEntryListHeader)) && pElement->SeekID == Tracks )
			{
				if(m_pFileChunk->FLocate( pElement->SeekPos + pSegInfo->SegmentBeginPos ))
					return VO_TRUE;
				continue;
			}
		}

		m_pFileChunk->FLocate( pSegInfo->SegmentEndPos );

		VOLOGI( "-CMKVAnalyze::MKVID_SEGMENT_CLUSTER %lu" , voOS_GetSysTime() );

	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessSeekHeadSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK , pInfo->s_ID ) == 0 )
	{
		SeekHeadElementStruct * pElement = ( SeekHeadElementStruct*)MemAlloc( sizeof(SeekHeadElementStruct) );
		MemSet( pElement , 0 , sizeof( SeekHeadElementStruct ) );

		ReadSection( &CMKVAnalyze::ProcessSeekHeadSeekSection , pInfo->size , pElement );

		InsertASCSortedList<SeekHeadElementStruct>( &(pSegInfo->SeekHeadElementEntryListHeader) , &(pElement->List) , SeekHeadElementStructComp() );
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessSeekHeadSeekSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	SeekHeadElementStruct * pElement = ( SeekHeadElementStruct * )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK_SEEKID , pInfo->s_ID ) == 0 )
	{
		VO_PBYTE pID = (VO_PBYTE)MemAlloc( (VO_U32)(pInfo->size * sizeof( VO_BYTE )) );
		MemSet( pID , 0 , (VO_U32)(pInfo->size * sizeof( VO_BYTE )) );

		m_pFileChunk->FRead( pID , (VO_U32)pInfo->size );

		if( MemCompare( pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO , 4 ) == 0 )
		{
			pElement->SeekID = SegmentInfo;
		}
		else if( MemCompare( pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD , 4 ) == 0 )
		{
			pElement->SeekID = SeekHead;
		}
		else if( MemCompare( pID , (VO_PTR)MKVID_SEGMENT_TRACKS , 4 ) == 0 )
		{
			pElement->SeekID = Tracks;
		}
		else if( MemCompare( pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , 4 ) == 0 )
		{
			pElement->SeekID = Cluster;
		}
		else if( MemCompare( pID , (VO_PTR)MKVID_SEGMENT_CUEDATA , 4 ) == 0 )
		{
			pElement->SeekID = Cues;
		}

		MemFree(pID);
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEEKHEAD_SEEK_SEEKPOSITION , pInfo->s_ID ) == 0 )
	{
		VO_S64 pos;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &pos );

		pElement->SeekPos = pos;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessSegmentInfoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO_DURATION , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size == 4 )
		{
			float duration;

			m_pFileChunk->FRead( &duration , sizeof( float ) );
			BigEndianLittleEndianExchange( &duration , sizeof(float) );

			pSegInfo->Duration = duration;
		}
		else if( pInfo->size == 8 )
		{
			double duration;

			m_pFileChunk->FRead( &duration , sizeof( double ) );
			BigEndianLittleEndianExchange( &duration , sizeof(double) );

			pSegInfo->Duration = duration;
		}
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_SEGMENTINFO_TIMECODESCALE , pInfo->s_ID ) == 0 )
	{
		VO_S64 codescale = 0;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &codescale );

		pSegInfo->TimecodeScale = codescale;
	}
	else
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessTracksSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY , pInfo->s_ID ) == 0 )
	{
		TracksTrackEntryStruct * pElement = (TracksTrackEntryStruct*)MemAlloc( sizeof(TracksTrackEntryStruct) );
		MemSet( pElement , 0 , sizeof( TracksTrackEntryStruct ) );
		InitializeTracksTrackEntryStruct( pElement );

		ReadSection( &CMKVAnalyze::ProcessTrackEntrySection , pInfo->size , pElement );

		InsertTailList( &(pSegInfo->TracksTrackEntryListHeader) , &(pElement->List) );
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessTrackEntrySection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKNUMBER , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracknum;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &tracknum );

		pElement->TrackNumber = (VO_U32)tracknum;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKTYPE , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracktype;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &tracktype );

		pElement->TrackType = (VO_U32)tracktype;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGENABLED , pInfo->s_ID ) == 0 )
	{
		VO_S64 enable;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &enable );

		pElement->IsEnabled = (enable == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGDEFAULT , pInfo->s_ID ) == 0 )
	{
		VO_S64 defaultflag;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &defaultflag );

		pElement->IsDefault = (defaultflag == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_FLAGFORCED , pInfo->s_ID ) == 0 )
	{
		VO_S64 forced;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &forced );

		pElement->IsForced = (forced == 1) ? VO_TRUE : VO_FALSE;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_DEFAULTDURATION , pInfo->s_ID ) == 0 )
	{
		VO_S64 defaultduration;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &defaultduration );

		pElement->DefaultDuration = defaultduration;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_TRACKTIMECODESCALE , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size == 4 )
		{
			float duration;

			m_pFileChunk->FRead( &duration , sizeof(float) );
			BigEndianLittleEndianExchange( &duration , sizeof(float) );

			pElement->TrackTimecodeScale = duration;
		}
		else if( pInfo->size == 8 )
		{
			double duration;

			m_pFileChunk->FRead( &duration , sizeof( double ) );
			BigEndianLittleEndianExchange( &duration , sizeof(double) );

			pElement->TrackTimecodeScale = duration;
		}
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_LANGUAGE , pInfo->s_ID ) == 0 )
	{
		VO_U8 languae[10];
		MemSet( languae , 0 , 10 );

		m_pFileChunk->FRead( languae , (VO_U32)pInfo->size );
		memcpy( pElement->str_Language , languae , (VO_U32)pInfo->size );
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CODECID , pInfo->s_ID ) == 0 )
	{
		VO_U8 codecid[50];
		MemSet( codecid , 0 , 50 );

		m_pFileChunk->FRead( codecid , (VO_U32)pInfo->size );
		memcpy( pElement->str_CodecID , codecid , (VO_U32)pInfo->size );
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CODECPRIVATE , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size )
		{
			pElement->CodecPrivateSize = (VO_U32)pInfo->size;
			pElement->pCodecPrivate = ( VO_S8* )MemAlloc( (VO_U32)pInfo->size );

			m_pFileChunk->FRead( pElement->pCodecPrivate , (VO_U32)pInfo->size );
		}
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessTrackEntryVideoSection , pInfo->size , pElement );
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessTrackEntryAudioSection , pInfo->size , pElement );
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTENCODEINGS , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentEncodings , pInfo->size , pElement );
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessTrackEntryVideoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO_PIXELWIDTH , pInfo->s_ID ) == 0 )
	{
		VO_S64 width;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &width );

		pElement->PixelWidth = (VO_U32)width;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_VIDEO_PIXELHEIGHT , pInfo->s_ID ) == 0 )
	{
		VO_S64 height;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &height );

		pElement->PixelHeight = (VO_U32)height;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessTrackEntryAudioSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_SAMPLEFREQ , pInfo->s_ID ) == 0 )
	{
		if( (VO_S32)pInfo->size == 4 )
		{
			float samplefreq;

			m_pFileChunk->FRead( &samplefreq , sizeof( float ) );
			BigEndianLittleEndianExchange( &samplefreq , sizeof(float) );

			pElement->SampleFreq = samplefreq;
		}
		else if( (VO_S32)pInfo->size == 8 )
		{
			double samplefreq;

			m_pFileChunk->FRead( &samplefreq , sizeof( double ) );
			BigEndianLittleEndianExchange( &samplefreq , sizeof(double) );

			pElement->SampleFreq = samplefreq;
		}
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_CHANNELS , pInfo->s_ID ) == 0 )
	{
		VO_S64 channels;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &channels );

		pElement->Channels = (VO_U32)channels;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_AUDIO_BITDEPTH , pInfo->s_ID ) == 0 )
	{
		VO_S64 bitdepth;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &bitdepth );

		pElement->BitDepth = (VO_U32)bitdepth;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessCuesSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	MKVSegmentStruct * pSegInfo = (MKVSegmentStruct *)pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT , pInfo->s_ID ) == 0 )
	{
		CuesCuePointStruct * pElement = (CuesCuePointStruct*)MemAlloc( sizeof(CuesCuePointStruct) );
		MemSet( pElement , 0 , sizeof( CuesCuePointStruct ) );
		InitializeCuesCuePointStruct( pElement );

		ReadSection( &CMKVAnalyze::ProcessCuePointSection , (VO_U32)pInfo->size , pElement );

		InsertASCSortedList< CuesCuePointStruct >( &(pSegInfo->CuesCuePointEntryListHeader) , &(pElement->List) , CuesCuePointStructComp() );
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessCuePointSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	CuesCuePointStruct * pElement = ( CuesCuePointStruct * )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETIME , pInfo->s_ID ) == 0 )
	{
		VO_S64 cuetime;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &cuetime );

		pElement->CueTime = cuetime;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS, pInfo->s_ID ) == 0 )
	{
		CuesCuePointTrackPositionsStruct * pPosition = (CuesCuePointTrackPositionsStruct*)MemAlloc( sizeof(CuesCuePointTrackPositionsStruct) );
		MemSet( pPosition , 0 , sizeof( CuesCuePointTrackPositionsStruct ) );

		ReadSection( &CMKVAnalyze::ProcessCuePointTrackPositionsSection , (VO_U32)pInfo->size , pPosition );

		InsertTailList( &( pElement->TrackPositionsHeader ) , &(pPosition->List) );
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessCuePointTrackPositionsSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	CuesCuePointTrackPositionsStruct * pElement = ( CuesCuePointTrackPositionsStruct * )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUETRACK , pInfo->s_ID ) == 0 )
	{
		VO_S64 tracknumber;

		ReadSectionContent_uint( (VO_U32)pInfo->size , &tracknumber );

		pElement->CueTrack = (VO_U32)tracknumber;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUECLUSTERPOSITION , pInfo->s_ID ) == 0 )
	{
		VO_S64 pos;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &pos );

		pElement->CueClusterPosition = pos;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_CUEDATA_CUEPOINT_CUETRACKPOSITIONS_CUEBLOCKNUMBER , pInfo->s_ID ) == 0 )
	{
		VO_S64 blocknumber;

		ReadSectionContent_uint( (VO_S32)pInfo->size , &blocknumber );

		pElement->CueBlockNumber = (VO_U32)blocknumber;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	return VO_FALSE;
}

VO_BOOL CMKVAnalyze::ReadIDAndSize( IDAndSizeInfo * pInfo )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_S64 result;
	VO_U8 retval;
	VO_BOOL retread;

	//first read id
	retread = m_pFileChunk->FRead( &sizeFlag , 1 );
	if( retread == VO_FALSE || sizeFlag == 0 )
	{
		return VO_FALSE;
	}

	retval = TranslateVINT( &sizeFlag , &size , &result );

	pInfo->pID = (VO_BYTE*)MemAlloc(size);
	pInfo->s_ID = size;
	pInfo->pID[0] = sizeFlag;

	if( retval )
	{
		retread = m_pFileChunk->FRead( &(pInfo->pID[1]) , size - 1 );
		if( retread == VO_FALSE )
		{
			MemFree(pInfo->pID);
			pInfo->pID = NULL;
			return VO_FALSE;
		}
	}
	//

	//second read size
	retread = m_pFileChunk->FRead( &sizeFlag , 1 );
	if( retread == VO_FALSE || sizeFlag == 0xFF)
	{
		if (sizeFlag == 0xFF)
		{
			VOLOGE("MKV Error:not support undefined element size");
		}
		MemFree(pInfo->pID);
		pInfo->pID = NULL;
		return VO_FALSE;
	}

	size = 1;
	retval = TranslateVINT( &sizeFlag , &size , &result );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)MemAlloc(size);
		pByte[0] = sizeFlag;

		retread = m_pFileChunk->FRead( pByte + 1 , size - 1 );
		if( retread == VO_FALSE )
		{
			MemFree(pByte);
			MemFree(pInfo->pID);
			pInfo->pID = NULL;
			return VO_FALSE;
		}

		TranslateVINT( pByte , &size , &result );

		MemFree(pByte);
	}

	pInfo->size = result;
	//

	if( pInfo->s_ID == 2 && pInfo->pID[0] == 0 && pInfo->pID[1] == 0 && (VO_S32)pInfo->size == 0 )
	{
		MemFree(pInfo->pID);
		pInfo->pID = NULL;
		return VO_FALSE;
	}

	return VO_TRUE;

}

VO_BOOL CMKVAnalyze::ProcessContentEncodings( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTENCODEING , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentEncoding , pInfo->size , pElement );
	}
	else
		return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessContentEncoding( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPRESSION , pInfo->s_ID ) == 0 )
	{
		ReadSection( &CMKVAnalyze::ProcessContentCompression , pInfo->size , pElement );
	}
	else
		return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CMKVAnalyze::ProcessContentCompression( IDAndSizeInfo * pInfo , VO_VOID * pParam )
{
	TracksTrackEntryStruct * pElement = ( TracksTrackEntryStruct* )pParam;

	if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPALGO , pInfo->s_ID ) == 0 )
	{
		VO_S64 algo;
		ReadSectionContent_uint( (VO_S32)pInfo->size , &algo );

		pElement->ContentCompAlgo = (VO_U32)algo;
	}
	else if( MemCompare( pInfo->pID , (VO_PTR)MKVID_SEGMENT_TRACKS_TRACKENTRY_CONTENTCOMPSETTINGS , pInfo->s_ID ) == 0 )
	{
		if( pInfo->size )
		{
			pElement->ContentCompSettingsSize = (VO_S32)pInfo->size;
			pElement->ptr_ContentCompSettings = (VO_PBYTE)MemAlloc( (VO_U32)pInfo->size );
			m_pFileChunk->FRead( pElement->ptr_ContentCompSettings , (VO_U32)pInfo->size );
		}
	}
	else
		return VO_FALSE;

	return VO_TRUE;
}

VO_VOID CMKVAnalyze::PrintMKVSegmentStruct(MKVSegmentStruct* info)
{
	VO_PTR	m_hMKV = NULL;
	VO_U32 len = 0;
	char	common_info[256];
	len = sizeof(common_info);
	memset(common_info, 0x0, len);
	

	if (!m_hMKV)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_READ_WRITE;

#ifdef _WIN32
		fileSource.pSource = (VO_PTR)_T("G:\\Debug\\dumper_MKVSegmentStruct_Pc.txt");
#else
		fileSource.pSource = (VO_PTR)_T("/data/local/voOMXPlayer/dumper_MKVSegmentStruct_Dev.txt");
#endif
		m_hMKV = cmnFileOpen(&fileSource);	

		if(m_hMKV == NULL)
		{
			return;
		}

//		strcpy(common_info, "Idx		Real		TS			Size		Sync(0:I frame)\r\n");
//		cmnFileWrite(m_hVideoLogFile, common_info, strlen(common_info));

	}
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "Duration  %f\r\n", info->Duration);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "TimecodeScale  %lld\r\n", info->TimecodeScale);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "ClusterPos  %lld\r\n", info->ClusterPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "SegmentEndPos  %lld\r\n", info->SegmentEndPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
	
	memset(common_info, 0x0, len);
	sprintf(common_info, "SegmentBeginPos  %lld	\r\n", info->SegmentBeginPos);
	cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		
	PvoLIST_ENTRY pEntry = info->SeekHeadElementEntryListHeader.Flink;
	while(pEntry != &(info->SeekHeadElementEntryListHeader))
	{
		SeekHeadElementStruct * p = GET_OBJECTPOINTER( pEntry , SeekHeadElementStruct , List );
		memset(common_info, 0x0, len);
		sprintf(common_info, "SeekID  %d	, SeekPos  %lld\r\n", p->SeekID, p->SeekPos);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		pEntry = pEntry->Flink;
	}

	pEntry = info->TracksTrackEntryListHeader.Flink;
	while( pEntry != &(info->TracksTrackEntryListHeader))
	{
		TracksTrackEntryStruct * pTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

		memset(common_info, 0x0, len);
		sprintf(common_info, "TrackNumber  %lu	, TrackType  %lu   ,DefaultDuration  %lld	, TrackTimecodeScale  %f\r\n", pTrackEntry->TrackNumber, pTrackEntry->TrackType, pTrackEntry->DefaultDuration, pTrackEntry->TrackTimecodeScale);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));

		if(pTrackEntry->TrackType == 1)//video
		{
			memset(common_info, 0x0, len);		
			sprintf(common_info, "PixelWidth  %lu , PixelHeight  %lu	\r\n", pTrackEntry->PixelWidth, pTrackEntry->PixelHeight);		
			cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		}
		else if(pTrackEntry->TrackType == 2)//audio
		{
			memset(common_info, 0x0, len);				
			sprintf(common_info, "SampleFreq  %f , Channels  %lu	,BitDepth  %lu	\r\n", pTrackEntry->SampleFreq, pTrackEntry->Channels, pTrackEntry->BitDepth);		
			cmnFileWrite(m_hMKV, common_info, strlen(common_info));
		}


		pEntry = pEntry->Flink;
	}


	pEntry = info->CuesCuePointEntryListHeader.Flink;
	while( pEntry != &(info->CuesCuePointEntryListHeader))
	{
		CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );

		CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
		memset(common_info, 0x0, len);
		sprintf(common_info, "CueTime  %lld	, CueClusterPosition  %lld   ,CueBlockNumber  %lu	, CueTrack  %lu\r\n", pCue->CueTime, pPointer->CueClusterPosition, pPointer->CueBlockNumber,pPointer->CueTrack);
		cmnFileWrite(m_hMKV, common_info, strlen(common_info));

		pEntry = pEntry->Flink;
	}

	
	cmnFileClose(m_hMKV);
	m_hMKV = NULL;
}
