#include "CMKVParser.h"
#include "CMKVAnalyze.h"
#include "EBML.h"
#include "MKVID.h"
#include "voLog.h"
//#include "voOSFunc.h"
#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif


CMKVHeadParser::CMKVHeadParser( CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp ):CvoFileHeaderParser( pFileChunk , pMemOp )
,m_analyze( pFileChunk , pMemOp )
{
	m_analyze.InitializeMKVInfo( &m_Info );
}

CMKVHeadParser::~CMKVHeadParser()
{
	m_analyze.UnInitializeMKVInfo( &m_Info );
}

VO_BOOL CMKVHeadParser::ReadFromFile()
{

	m_analyze.AnalyzeMKV( (VO_S64)m_filesize , &m_Info );

	MKVSegmentStruct * ptr_seg = GetSegmentInfo( 0 );

	//Addedb by Aiven, dump the MKV HeadData for debug. 
	//Open this function only if you want to debug.
//	m_analyze.PrintMKVSegmentStruct(ptr_seg);
	if( ptr_seg && (VO_S32)ptr_seg->Duration == 0 )
	{
		GetDuration();
	}

	return VO_TRUE;
}

VO_VOID CMKVHeadParser::SetFileSize( VO_U64 filesize )
{
	m_filesize = filesize;
}


VO_U32 CMKVHeadParser::GetSegmentCount()
{
	return GetListLength( &(m_Info.MKVSegmentListHeader) );
}

PvoLIST_ENTRY CMKVHeadParser::GetSeekHeadList( VO_U32 segmentindex )
{
	PvoLIST_ENTRY pEntry = GetListEntryByIndex( &(m_Info.MKVSegmentListHeader) , segmentindex );

	if( pEntry == NULL )
		return NULL;

	MKVSegmentStruct * pSeg = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );

	return &(pSeg->SeekHeadElementEntryListHeader);
}

TracksTrackEntryStruct * CMKVHeadParser::GetBestAudioTrackInfo( VO_U32 segmentindex )
{
	PvoLIST_ENTRY pEntry = GetListEntryByIndex( &(m_Info.MKVSegmentListHeader) , segmentindex );

	if( pEntry == NULL )
		return NULL;

	MKVSegmentStruct * pSeg = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );

	pEntry = pSeg->TracksTrackEntryListHeader.Flink;

	TracksTrackEntryStruct * pTempSelect = NULL;

	while( pEntry != &(pSeg->TracksTrackEntryListHeader) )
	{
		TracksTrackEntryStruct * pTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

		if( pTrackEntry->TrackType == 2 )// 1 means audio
		{
			if( pTempSelect == NULL )
			{
				if( pTrackEntry->IsEnabled )
				{
					pTempSelect = pTrackEntry;

					if( pTempSelect->IsDefault || pTempSelect->IsForced )
						return pTempSelect;
				}
			}
			else
			{
				if( pTrackEntry->IsDefault || pTrackEntry->IsForced )
				{
					pTempSelect = pTrackEntry;
					return pTempSelect;
				}
			}
		}

		pEntry = pEntry->Flink;
	}

	return pTempSelect;
}

TracksTrackEntryStruct * CMKVHeadParser::GetBestVideoTrackInfo( VO_U32 segmentindex )
{
	PvoLIST_ENTRY pEntry = GetListEntryByIndex( &(m_Info.MKVSegmentListHeader) , segmentindex );

	if( pEntry == NULL )
		return NULL;

	MKVSegmentStruct * pSeg = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );

	pEntry = pSeg->TracksTrackEntryListHeader.Flink;

	TracksTrackEntryStruct * pTempSelect = NULL;

	while( pEntry != &(pSeg->TracksTrackEntryListHeader) )
	{
		TracksTrackEntryStruct * pTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

		if( pTrackEntry->TrackType == 1 )// 1 means video
		{
			if( pTempSelect == NULL )
			{
				if( pTrackEntry->IsEnabled )
				{
					pTempSelect = pTrackEntry;

					if( pTempSelect->IsDefault || pTempSelect->IsForced )
						return pTempSelect;
				}
			}
			else
			{
				if( pTrackEntry->IsDefault || pTrackEntry->IsForced )
				{
					pTempSelect = pTrackEntry;
					return pTempSelect;
				}
			}
		}

		pEntry = pEntry->Flink;
	}

	return pTempSelect;
}

PvoLIST_ENTRY CMKVHeadParser::GetCuesList( VO_U32 segmentindex )
{
	PvoLIST_ENTRY pEntry = GetListEntryByIndex( &(m_Info.MKVSegmentListHeader) , segmentindex );

	if( pEntry == NULL )
		return NULL;

	MKVSegmentStruct * pSeg = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );

	return &(pSeg->CuesCuePointEntryListHeader);
}

MKVSegmentStruct * CMKVHeadParser::GetSegmentInfo( VO_U32 segmentindex )
{
	PvoLIST_ENTRY pEntry = GetListEntryByIndex( &(m_Info.MKVSegmentListHeader) , segmentindex );

	if( pEntry == NULL )
		return NULL;

	MKVSegmentStruct * pSeg = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );

	return pSeg;
}

VO_VOID CMKVHeadParser::GetDuration()
{
	MKVSegmentStruct * ptr_seg = GetSegmentInfo( 0 );

	ptr_seg->Duration = GetLastClusterTime( ptr_seg );
}

double	CMKVHeadParser::GetLastClusterTime( MKVSegmentStruct * ptr_seg )
{
	m_pFileChunk->FLocate( ptr_seg->ClusterPos );

	VO_U64 last_clusterpos = ptr_seg->ClusterPos;
	VO_BOOL find = VO_FALSE;

	do 
	{
		while( 1 )
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			IDAndSizeInfo info;

			if( ReadIDAndSize( &info ) == VO_FALSE )
			{
				find = VO_TRUE;
				break;
			}

			if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) == 0 )
			{
				last_clusterpos = pos;
			}

			if( !m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size ))
			{
				MemFree( info.pID );
				break;
			}

			MemFree( info.pID );
		}
	} while ( !find );

	m_pFileChunk->FLocate( last_clusterpos , VO_TRUE );

	VO_U64 CurrentTimeCode = 0;
	VO_U64 maxreftimecode = 0;
	while( 1 )
	{

		IDAndSizeInfo info;

		if( ReadIDAndSize( &info ) == VO_FALSE )
		{
			break;
		}

		if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) == 0 )
		{
			;
		}
		else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , info.s_ID ) == 0 )
		{
			if (info.size <= 4 )
			{
				Read_uint( (VO_S32)info.size , (VO_S64*)&CurrentTimeCode );
				CurrentTimeCode = (VO_S64)(ptr_seg->TimecodeScale / 1000000. * CurrentTimeCode);
			}
			else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
			{
				info.size = 0;
				CurrentTimeCode = 0;
			}
		}
		else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , info.s_ID ) == 0 )
		{
		}
		else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info.s_ID ) == 0 )
		{
			VO_U64 end = m_pFileChunk->FGetFilePos() + info.size;
			VO_U64 rtime = GetRelativeTime( ptr_seg->TimecodeScale );

			if( rtime > maxreftimecode )
				maxreftimecode = rtime;

			if( !m_pFileChunk->FLocate( end ) )
			{
				MemFree( info.pID );
				break;
			}
		}
		else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , info.s_ID ) == 0 )
		{
			VO_U64 end = m_pFileChunk->FGetFilePos() + info.size;
			VO_U64 rtime = GetRelativeTime( ptr_seg->TimecodeScale );

			if( rtime > maxreftimecode )
				maxreftimecode = rtime;

			if( !m_pFileChunk->FLocate( end ) )
			{
				MemFree( info.pID );
				break;
			}
		}
		else
		{
			if( !m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size ))
			{
				MemFree( info.pID );
				break;
			}
		}

		MemFree( info.pID );
	}

	return (double)(CurrentTimeCode + maxreftimecode);
}

VO_BOOL CMKVHeadParser::Read_uint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)MemAlloc(readsize);
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	for( VO_S32 i = 0 ; i < readsize ; i++ )
	{
		pPos[i] = pData[ readsize - i - 1 ];
	}

	MemFree(pData);

	return VO_TRUE;
}

VO_BOOL CMKVHeadParser::ReadIDAndSize( IDAndSizeInfo * pInfo )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_S64 result;
	VO_U8 retval;
	VO_BOOL retread;

	//first read id
	retread = m_pFileChunk->FRead( &sizeFlag , 1 );
	if( retread == VO_FALSE )
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
	if( retread == VO_FALSE )
	{
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

	return VO_TRUE;

}

VO_U64 CMKVHeadParser::GetRelativeTime( VO_S64 timescale )
{
	VO_S64 block_timecode;
	Read_vint( &block_timecode );

	Read_sint( 2 , &block_timecode );

	block_timecode = (VO_S64)(timescale / 1000000. * block_timecode);

	return block_timecode;
}

VO_BOOL CMKVHeadParser::Read_vint( VO_S64 * pResult )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_U8 retval;

	m_pFileChunk->FRead( &sizeFlag , 1 );

	retval = TranslateVINT( &sizeFlag , &size , pResult );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)MemAlloc(size);
		pByte[0] = sizeFlag;

		m_pFileChunk->FRead( pByte + 1 , size - 1 );

		TranslateVINT( pByte , &size , pResult );

		MemFree(pByte);
	}

	return VO_TRUE;
}

VO_BOOL CMKVHeadParser::Read_sint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)MemAlloc(readsize);
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	if( pData[0] & 0x80 )
	{
		for( VO_U32 i = 0 ; i < sizeof( VO_S64 ) ; i++ )
		{
			if( i < (VO_U32)readsize )
				pPos[i] = pData[ readsize - i - 1 ];
			else
				pPos[i] = 0xff;
		}
	}
	else
	{
		for( VO_S32 i = 0 ; i < readsize ; i++ )
		{
			if( i < readsize )
				pPos[i] = pData[ readsize - i - 1 ];
		}
	}

	MemFree(pData);

	return VO_TRUE;
}


CMKVDataParser::CMKVDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
: CvoFileDataParser(pFileChunk, pMemOp)
,m_CurrentTimeCode(-1)
,m_bIsInBlock( VO_FALSE )
,m_framecount(0)
,m_currentframeindex( 0 )
,m_currentblocksize(0)
,m_block_timecode(0)
,m_block_tracknumber(0)
,m_simpleblock_kf_way(VO_FALSE)
,m_groupblock_kf_way(VO_FALSE)
,m_pSegInfo(0)
,m_nCurCluserEndPos(0)
,m_IsGetThumbNail(VO_FALSE)
{
	MemSet( m_framesizearray , 0 , 256 * sizeof( VO_U32 ) );
}

CMKVDataParser::~CMKVDataParser()
{
	;
}


VO_VOID CMKVDataParser::Init(VO_PTR pParam)
{
	;
}

VO_BOOL CMKVDataParser::StepB()
{
	if( m_bIsInBlock == VO_TRUE )
	{
		ReadBlock();
	}
	else
	{
	//	static VO_U64 nCluserEndPos = 0;
		while( 1 )
		{
			IDAndSizeInfo info;

			if( ReadIDAndSize( &info ) == VO_FALSE )
			{
				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , End , NULL , NULL );
				return VO_FALSE;
			}
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			//Added by Aiven, if the filepos is out of the range of Segment,return false.
			if( pos > (VO_U64)m_pSegInfo->SegmentEndPos)
			{
				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , End , NULL , NULL );
				MemFree( info.pID );
				return VO_FALSE;
			}

			
			if(pos+info.size > (VO_U64)m_pSegInfo->SegmentEndPos){
				info.size = m_pSegInfo->SegmentEndPos - (VO_S64)pos;
			}

			if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) == 0 )
			{
				m_nCurCluserEndPos = m_pFileChunk->FGetFilePos() + info.size;

				//Added by Aiven, if the filepos is out of the range of Segment,return false.
				if( m_nCurCluserEndPos > (VO_U64)m_pSegInfo->SegmentEndPos)
				{
					m_OnNotify( m_pOnNotifyUser , m_btBlockStream , End , NULL , NULL );
					MemFree( info.pID );
					return VO_FALSE;
				}
				
				m_CurrentTimeCode = -1;
				m_bIsKeyFrame = VO_FALSE;
				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewCluser , NULL , NULL );
				MemFree( info.pID );
				continue;
			}
			if (m_nCurCluserEndPos == 0)
			{		
				m_pFileChunk->FLocate(m_pFileChunk->FGetFilePos()+info.size);
				MemFree( info.pID );
				continue;
			}
			if (m_pFileChunk->FGetFilePos() + info.size > m_nCurCluserEndPos)
			{
				m_pFileChunk->FLocate(m_nCurCluserEndPos);
				m_nCurCluserEndPos = 0;
				MemFree( info.pID );
				continue;
			}
			if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , info.s_ID ) == 0 )
			{
				if (info.size <= 4)
				{
					Read_uint( (VO_S32)info.size , &m_CurrentTimeCode );
					m_CurrentTimeCode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * m_CurrentTimeCode);
					m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewClusterTimeCode , &m_CurrentTimeCode , NULL );
				}
				else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
				{
					m_pFileChunk->FLocate(m_pFileChunk->FGetFilePos()+info.size);
					m_CurrentTimeCode = 0;
				}
			}
			else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , info.s_ID ) == 0 )
			{

				if(pos+info.size > m_nCurCluserEndPos){
					VO_U64 temppos = m_pFileChunk->FGetFilePos();
					info.size = m_nCurCluserEndPos - temppos;
				}

				//do nothing
				m_simpleblock_kf_way = VO_FALSE;
				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewBlockGroup , NULL , NULL );
				m_bIsKeyFrame = VO_TRUE;
				if (m_IsGetThumbNail)
				{
					VO_U32 BlockGroupPos = (VO_U32)m_pFileChunk->FGetFilePos();
					VO_U32 CurGroupFilePos = 0;
					VO_BOOL beRefBlock = VO_FALSE;
					IDAndSizeInfo info_Group;
					info_Group.pID = VO_NULL;
					while (1)
					{
						if (info_Group.pID)
						{
							MemFree(info_Group.pID);
							info_Group.pID = VO_NULL;
						}
						if(!ReadIDAndSize(&info_Group))
						{
							break;
						}
						CurGroupFilePos = (VO_U32)m_pFileChunk->FGetFilePos();
						if (CurGroupFilePos + info_Group.size - BlockGroupPos > info.size)
						{
							break;
						}
						if (MemCompare( info_Group.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_REFERENCEBLOCK , info_Group.s_ID ) == 0 )
						{
							m_pFileChunk->FLocate( BlockGroupPos + info.size );
							beRefBlock = VO_TRUE;
							break;
						}
						else if (MemCompare( info_Group.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info_Group.s_ID ) == 0 )
						{
							VO_S64 tmp = 0,tmp1 = 0;
							Read_vint( &tmp );
							Read_sint( 2 , &tmp1 );
							VO_U8 flag;
							m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );
							if (tmp != m_VideoID || (m_groupblock_kf_way && !(flag &0x80)))
							{
								m_pFileChunk->FLocate(BlockGroupPos + info.size );
								beRefBlock = VO_TRUE;
								break;
							}
							else
								m_pFileChunk->FLocate(CurGroupFilePos + info_Group.size );	

						}
						else
						{
							if ((VO_S64)(m_pFileChunk->FGetFilePos() + info_Group.size) >= (VO_S64)(info.size + BlockGroupPos))
							{
								break;
							}
							else
								m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info_Group.size );		
						}
					}
					if (info_Group.pID)
					{
						MemFree(info_Group.pID);
					}
					if (!beRefBlock)
					{
						m_pFileChunk->FLocate(BlockGroupPos);
					}
					
				}
			}
			else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info.s_ID ) == 0 )
			{

				//m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewBlock , NULL , NULL );
				m_currentblocksize = info.size;
			//	m_simpleblock_kf_way = VO_FALSE;
				ReadBlock();
				MemFree( info.pID );
				break;
			}
			else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , info.s_ID ) == 0 )
			{
				if(pos+info.size > m_nCurCluserEndPos){
					VO_U64 temppos = m_pFileChunk->FGetFilePos();
					info.size = m_nCurCluserEndPos - temppos;
				}

				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , SimpleBlock , NULL , NULL );
				m_simpleblock_kf_way = VO_TRUE;
				if(m_IsGetThumbNail)
				{
					VO_S64 tmp = 0;
					VO_U32 SimBlockPos = (VO_U32)m_pFileChunk->FGetFilePos();
					Read_vint( &tmp );
					if (tmp != m_VideoID)
					{
						m_pFileChunk->FLocate(SimBlockPos + info.size);
						continue;
					}
					Read_sint( 2 , &tmp);
					VO_U8 flag;
					m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );
					if (!(flag & 0x80))
					{
						m_pFileChunk->FLocate(SimBlockPos + info.size);
						continue;
					}
					else
					{
						m_pFileChunk->FLocate(SimBlockPos);
					}
				}
				m_currentblocksize = info.size;
				m_bIsKeyFrame = VO_FALSE;
				ReadBlock();
				MemFree( info.pID );
				break;
			}
			else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_REFERENCEBLOCK , info.s_ID ) == 0 )
			{
				m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewReferenceBlock , NULL , NULL );
				m_bIsKeyFrame = VO_FALSE;
				m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );
			}
			else
			{
				m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );
			}

			MemFree( info.pID );
		}
	}

	return VO_TRUE;
}

VO_BOOL CMKVDataParser::ReadBlock()
{
	if( m_bIsInBlock == VO_TRUE )
	{
		m_currentframeindex++;

		MKVFrame frame;
		MemSet( &frame , 0 , sizeof( MKVFrame ) );

		frame.bIsKeyFrame = (VO_FALSE || m_bIsKeyFrame)?VO_TRUE:VO_FALSE;// if it is lacing it should not be a key frame( it is an assumption )

		frame.RelativeTimeCode = m_block_timecode;

		frame.TimeCode = m_CurrentTimeCode;

		frame.pFileChunk = m_pFileChunk;
		frame.framesize = m_framesizearray[m_currentframeindex];
		frame.TrackNumber = (VO_U8)m_block_tracknumber;
		frame.duration = 1;
		frame.filepos = m_pFileChunk->FGetFilePos();

		//onblock
		VO_S64 timestamp = frame.TimeCode+frame.RelativeTimeCode;
		if( (m_btBlockStream == m_block_tracknumber) && ( !m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData , &timestamp ) ) )
		{
			VO_S32 size = 0;

			for(;m_currentframeindex < m_framecount ; m_currentframeindex++ )
				size = size + m_framesizearray[m_currentframeindex];

			m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + size );

			if( m_currentframeindex == m_framecount)
			{
				m_bIsInBlock = VO_FALSE;
				m_framecount = 0;
				m_currentframeindex = 0;
				m_currentblocksize = 0;
				m_block_timecode = 0;
				m_block_tracknumber = 0;
			}

			return VO_TRUE;
		}
		//

		//callback
		if( CBRT_FALSE == m_fODCallback( m_pODUser, m_pODUserData, &frame ) )
			m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + frame.framesize );
		//

		if( m_currentframeindex == m_framecount - 1 )
		{
			m_bIsInBlock = VO_FALSE;
			m_framecount = 0;
			m_currentframeindex = 0;
			m_currentblocksize = 0;
			m_block_timecode = 0;
			m_block_tracknumber = 0;
		}
	}
	else
	{
		VO_U64 BlockEndPos = m_pFileChunk->FGetFilePos() + m_currentblocksize;

		Read_vint( &m_block_tracknumber );

		m_OnNotify( m_pOnNotifyUser , m_btBlockStream , NewBlock , &m_block_tracknumber , NULL );

		Read_sint( 2 , &m_block_timecode );

		m_block_timecode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * m_block_timecode);

		VO_U8 flag;
		m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

		switch( flag & 0x6 )
		{
		case 0:// no lacing
			{
				MKVFrame frame;
				MemSet( &frame , 0 , sizeof( MKVFrame ) );

				if( m_simpleblock_kf_way || m_groupblock_kf_way)
				{
					if( flag & 0x80 )
					{
						frame.bIsKeyFrame = VO_TRUE;
					}
					else
						frame.bIsKeyFrame = VO_FALSE;
				}
				else
				{
					if( flag & 0x80 )
					{
						frame.bIsKeyFrame = ( VO_TRUE || m_bIsKeyFrame )?VO_TRUE:VO_FALSE;
						m_groupblock_kf_way = VO_TRUE;
					//	m_simpleblock_kf_way = VO_TRUE;
					}
					else
						frame.bIsKeyFrame = ( VO_FALSE || m_bIsKeyFrame )?VO_TRUE:VO_FALSE;
				}

				frame.RelativeTimeCode = m_block_timecode;

				frame.TimeCode = m_CurrentTimeCode;

				frame.pFileChunk = m_pFileChunk;
				frame.framesize = BlockEndPos - m_pFileChunk->FGetFilePos();
				frame.TrackNumber = (VO_U8)m_block_tracknumber;
				frame.duration = 1;

				frame.filepos = m_pFileChunk->FGetFilePos();

				//onblock
				if( (m_btBlockStream == m_block_tracknumber) )
				{
					VO_S64 timestamp = frame.TimeCode+frame.RelativeTimeCode;
					if( !m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData , &timestamp ) )
					{
						m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + frame.framesize );
						return VO_TRUE;
					}
				}
				//

				//callback
				if( CBRT_FALSE == m_fODCallback( m_pODUser, m_pODUserData, &frame ) )
					m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + frame.framesize );
				//
			}
			break;
		default:
			{
				m_pFileChunk->FRead( &m_framecount , sizeof( VO_U8 ) );
				m_framecount++;

				MemSet( m_framesizearray , 0 ,sizeof(VO_U32)*m_framecount );

				if( (flag & 0x6) == 0x2 )//xiph lacing
				{
					VO_U64 totalsize, sumsize = 0;
					for( VO_U8 i = 0 ; i < m_framecount - 1 ; i++ )
					{
						VO_U8 value;
						do
						{
							m_pFileChunk->FRead( &value , sizeof( VO_U8 ) );
							m_framesizearray[i] = m_framesizearray[i] + value;
						} while ( value == 0xff );

						sumsize = sumsize + m_framesizearray[i];
					}

					totalsize = BlockEndPos - m_pFileChunk->FGetFilePos();

					m_framesizearray[ m_framecount - 1 ] = (VO_U32)(totalsize - sumsize);
				}
				else if( (flag & 0x6) == 0x4 )//fix size lacing
				{
					VO_U64 leftsize = BlockEndPos - m_pFileChunk->FGetFilePos();
					VO_U32 average = (VO_U32)(leftsize / m_framecount);

					for( VO_U8 i = 0 ; i < m_framecount ; i++ )
					{
						m_framesizearray[i] = average;
					}
				}
				else if( (flag & 0x6) == 0x6 )//ebml lacing
				{
					VO_S64 result;
					VO_U64 totalsize, sumsize;

					Read_vint( &result );
					m_framesizearray[0] = (VO_U32)result;

					sumsize = result;

					for( VO_U8 i = 1 ; i < m_framecount - 1 ; i++ )
					{
						Read_svint( &result );
						m_framesizearray[i] = (VO_U32)(result + m_framesizearray[i-1]);

						sumsize = sumsize + m_framesizearray[i];
					}

					totalsize = BlockEndPos - m_pFileChunk->FGetFilePos();

					m_framesizearray[ m_framecount - 1 ] = (VO_U32)(totalsize - sumsize);
				}

				MKVFrame frame;
				MemSet( &frame , 0 , sizeof( MKVFrame ) );

				if( flag & 0x80 )
					frame.bIsKeyFrame = ( VO_TRUE || m_bIsKeyFrame )?VO_TRUE:VO_FALSE;
				else
					frame.bIsKeyFrame = ( VO_FALSE || m_bIsKeyFrame )?VO_TRUE:VO_FALSE;

				frame.RelativeTimeCode = m_block_timecode;

				frame.TimeCode = m_CurrentTimeCode;

				frame.pFileChunk = m_pFileChunk;
				frame.framesize = m_framesizearray[0];
				frame.TrackNumber = (VO_U8)m_block_tracknumber;
				frame.duration = 1;

				frame.filepos = m_pFileChunk->FGetFilePos();

				//onblock
				if( (m_btBlockStream == m_block_tracknumber) )
				{
					VO_S64 timestamp = frame.TimeCode+frame.RelativeTimeCode;
					if( !m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData , &timestamp ) )
					{
						m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + frame.framesize );
						if( 0 < m_framecount - 1 )
						{
							m_bIsInBlock = VO_TRUE;
							m_currentframeindex = 0;
						}
						return VO_TRUE;
					}
				}
				//

				//callback
				if( CBRT_FALSE == m_fODCallback( m_pODUser, m_pODUserData, &frame ) )
					m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + frame.framesize );
				//

				if( 0 < m_framecount - 1 )
				{
					m_bIsInBlock = VO_TRUE;
					m_currentframeindex = 0;
				}
			}
			break;
		}
	}

	return VO_TRUE;
}

VO_BOOL CMKVDataParser::Read_vint( VO_S64 * pResult )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_U8 retval;

	m_pFileChunk->FRead( &sizeFlag , 1 );

	retval = TranslateVINT( &sizeFlag , &size , pResult );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)MemAlloc(size);
		pByte[0] = sizeFlag;

		m_pFileChunk->FRead( pByte + 1 , size - 1 );

		TranslateVINT( pByte , &size , pResult );

		MemFree(pByte);
	}

	return VO_TRUE;
}

VO_BOOL CMKVDataParser::Read_svint( VO_S64 * pResult )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_U8 retval;

	m_pFileChunk->FRead( &sizeFlag , 1 );

	retval = TranslateVSINT( &sizeFlag , &size , pResult );

	if( retval )
	{
		VO_BYTE * pByte = (VO_BYTE*)MemAlloc(size);
		pByte[0] = sizeFlag;

		m_pFileChunk->FRead( pByte + 1 , size - 1 );

		TranslateVSINT( pByte , &size , pResult );

		MemFree(pByte);
	}

	return VO_TRUE;
}

VO_BOOL CMKVDataParser::ReadIDAndSize( IDAndSizeInfo * pInfo )
{
	VO_BYTE sizeFlag;
	VO_U8 size = 1;
	VO_S64 result;
	VO_U8 retval;
	VO_BOOL retread;

	//first read id

	VO_S32 readtimes = 0;

	while( readtimes < 1024 * 1024 )
	{
		retread = m_pFileChunk->FRead( &sizeFlag , 1 );

		if( retread == VO_FALSE )
			return VO_FALSE;

		readtimes++;

		if( sizeFlag != 0 && sizeFlag != 0xff )
			break;
	}

	if( sizeFlag == 0 || sizeFlag == 0xff )
		return VO_FALSE;

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
	if( retread == VO_FALSE )
	{
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

	if( pInfo->s_ID == 2 && pInfo->pID[0] == 0 && pInfo->pID[1] == 0 && pInfo->size == 0 )
	{
		MemFree(pInfo->pID);
		pInfo->pID = NULL;
		return VO_FALSE;
	}

	return VO_TRUE;

}

VO_BOOL CMKVDataParser::Read_uint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)MemAlloc(readsize);
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	for( VO_S32 i = 0 ; i < readsize ; i++ )
	{
		pPos[i] = pData[ readsize - i - 1 ];
	}

	MemFree(pData);

	return VO_TRUE;
}

VO_BOOL CMKVDataParser::Read_sint( VO_S32 readsize , VO_S64 * pResult )
{
	VO_PBYTE pData = (VO_BYTE*)MemAlloc(readsize);
	VO_PBYTE pPos = ( VO_PBYTE )pResult;
	*pResult = 0;

	m_pFileChunk->FRead( pData , readsize );

	if( pData[0] & 0x80 )
	{
		for( VO_U32 i = 0 ; i < sizeof( VO_S64 ) ; i++ )
		{
			if( i < (VO_U32)readsize )
				pPos[i] = pData[ readsize - i - 1 ];
			else
				pPos[i] = 0xff;
		}
	}
	else
	{
		for( VO_S32 i = 0 ; i < readsize ; i++ )
		{
			if( i < readsize )
				pPos[i] = pData[ readsize - i - 1 ];
		}
	}

	MemFree(pData);

	return VO_TRUE;
}

VO_U32 CMKVDataParser::MoveTo( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	VO_S64 filepos;

	if( IsListEmpty( &(pSegInfo->CuesCuePointEntryListHeader) ) )
	{
		//I have to guess a place
		filepos = GuessPositionByTime( llTimeStamp , pSegInfo );//55564561;
		//filepos = GetPositionByTravelCluster( llTimeStamp , pSegInfo );
	}
	else
	{
		//use cues info to get a right place
		filepos = GetPositionWithCuesInfoByTime( llTimeStamp , pSegInfo );
	}
	//

	if (filepos >= 0)
		SetStartFilePos(filepos);	
	else
		SetParseEnd(VO_TRUE);

	return VO_ERR_SOURCE_OK;
}

VO_S64 CMKVDataParser::GuessPositionByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	double duration = pSegInfo->Duration * pSegInfo->TimecodeScale / 1000000.;
//	VO_U64 guessfilepos = (VO_U64)( (((llTimeStamp - 5000)<0)?0:(llTimeStamp - 5000)) / duration ) * ( pSegInfo->SegmentEndPos - pSegInfo->ClusterPos );
	double temp = ((llTimeStamp - 5000)<0)?0:((double)llTimeStamp - 5000.);
	double percent =   temp / duration;
	VO_U64 guessfilepos = percent * ( pSegInfo->SupposeSegmentEndPos - pSegInfo->ClusterPos );
	VO_S64 filesize = m_pFileChunk->FGetFileSize();

	VOLOGI("GuessPositionByTime1--guessfilepos=%llu, filesize=%llu",guessfilepos,filesize);

	if(guessfilepos >= filesize){
		return -1;
	}

	if( guessfilepos <= (VO_U64)pSegInfo->ClusterPos )
		return GetPositionPrecisely( llTimeStamp , pSegInfo->ClusterPos );

	VO_U64 startpos = guessfilepos;
	VO_U64 endpos = pSegInfo->SegmentEndPos;

	VO_S64 filepos;


//	VO_U32 	time = voOS_GetSysTime();
	while( 1 )
	{
		filepos = FindCluster( llTimeStamp , startpos , endpos );

		if( filepos >= 0 )
		{
			if( filepos < pSegInfo->ClusterPos )
			{
				filepos = pSegInfo->ClusterPos;
			}
			break;
		}
		else if(filepos == -2)
		{
			VO_S64 pos = (VO_U64)(startpos - ( 5000 / duration ) * ( pSegInfo->SegmentEndPos - pSegInfo->ClusterPos ));

			if( pos < pSegInfo->ClusterPos )
			{
				filepos = pSegInfo->ClusterPos;
				break;
			}
			else
			{
				startpos = pos;
			}
		}
		else
		{
			return -1;
		}
	}

//	time = voOS_GetSysTime() - time;
//	VOLOGI("GuessPositionByTime2--pos=%llu, llTimeStamp=%lld, timecode=%llu",filepos,llTimeStamp, time);
	VO_U64 pos = GetPositionPrecisely( llTimeStamp , filepos );
//	VOLOGI("GuessPositionByTime3--pos=%llu, llTimeStamp=%lld, timecode=%llu",pos,llTimeStamp, voOS_GetSysTime() - time);

	return pos;
}

VO_U64 CMKVDataParser::GetPositionByTravelCluster( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	return GetPositionPrecisely( llTimeStamp , pSegInfo->ClusterPos );
}

VO_S64 CMKVDataParser::FindCluster( VO_S64 llTimeStamp , VO_U64 startpos , VO_U64 endpos )
{
//	VO_U64 findpos = -1;

	m_pFileChunk->FLocate( startpos );
	VO_U32 dwInterSizeCluster = 4*1024*1024;

	VO_BYTE Buffer[1024];
	VO_U32 dwRead;
	VO_U32 offset = 0;
	VO_U32 count = 0;

	while( 1 )
	{
		if (count++ >= dwInterSizeCluster/1024)
		{
			return -1;
		}
		
		if( m_pFileChunk->FRead3( Buffer + offset , 1024 - offset , &dwRead ) )
		{
			for ( VO_U32 i = 0 ; i < dwRead + offset - 3 ; i++ )
			{
				if( MemCompare( Buffer + i , (VO_PTR)MKVID_SEGMENT_CLUSTER , 4 ) == 0 )
				{
					VO_U64 filepos = m_pFileChunk->FGetFilePos() - dwRead - offset + i;
					//check if it is a cluster
					if( VO_TRUE == CheckIsCluster( filepos , endpos ) )
					{
						//get cluster time to compare
//						VO_U64 timecode = GetClusterFirstKeyFrameTimeCodeDirectly( filepos );
						VO_U64 timecode = GetClusterByTimeCodeDirectly( filepos );

						if( timecode <= (VO_U64)llTimeStamp )
							return filepos;
						else
							return -2;
					}
				}
			}

			offset = 3;
			Buffer[0] = Buffer[dwRead-3];
			Buffer[1] = Buffer[dwRead-2];
			Buffer[2] = Buffer[dwRead-1];
		}
		else
		{
			return -1;
		}
	}
}

VO_BOOL CMKVDataParser::CheckIsCluster( VO_U64 filepos , VO_U64 endpos )
{
	VO_U64 filepos_backup = m_pFileChunk->FGetFilePos();

	m_pFileChunk->FLocate( filepos );

	IDAndSizeInfo info;

	if( ReadIDAndSize( &info ) == VO_FALSE )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return VO_FALSE;
	}

	if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) != 0 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		MemFree( info.pID );
		return VO_FALSE;
	}

	if( m_pFileChunk->FGetFilePos() + info.size > endpos - 5 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		MemFree( info.pID );
		return VO_FALSE;
	}

	m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );

	MemFree( info.pID );

	if( ReadIDAndSize( &info ) == VO_FALSE )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return VO_FALSE;
	}

	if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , info.s_ID ) != 0 )
	{
		m_pFileChunk->FLocate( filepos_backup );
		MemFree( info.pID );
		return VO_FALSE;
	}

	MemFree( info.pID );

	m_pFileChunk->FLocate( filepos_backup );
	return VO_TRUE;
}

VO_U64 CMKVDataParser::GetClusterByTimeCodeDirectly( VO_U64 filepos )
{
	VO_U64 filepos_backup = m_pFileChunk->FGetFilePos();
	VO_U64 CurFilePos = filepos;
	VO_U64 EndFilePos;
	VO_S64 clustertimecode = 0x7fffffffffffffffLL;

	m_pFileChunk->FLocate( filepos );

	IDAndSizeInfo info;
	if( !ReadIDAndSize( &info ) )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return 0xffffffffffffffffLL;
	}

	MemFree( info.pID );

	EndFilePos = CurFilePos + info.size;

//	VO_U32 time = voOS_GetSysTime();
	while ( 1 )
	{
		IDAndSizeInfo subinfo;

		if( !ReadIDAndSize( &subinfo ) )
			break;

		if( MemCompare( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , subinfo.s_ID ) == 0 )
		{
			MemFree(subinfo.pID);
			if (subinfo.size <= 4 )
			{
				Read_uint( (VO_U32)subinfo.size , &clustertimecode );
				clustertimecode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * clustertimecode);
				break;
			}
			else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
			{
				break;
			}
		}
		else
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			m_pFileChunk->FLocate( pos + subinfo.size );
		}

		MemFree(subinfo.pID);

		CurFilePos = m_pFileChunk->FGetFilePos();

		if( CurFilePos >= EndFilePos )
			break;
	}

	m_pFileChunk->FLocate( filepos_backup );

//	VOLOGI("GetClusterByTimeCodeDirectly---filepos=%llu, clustertimecode=%lld, timecode=%llu",filepos, clustertimecode,voOS_GetSysTime() -time);

	return clustertimecode;
}



VO_U64 CMKVDataParser::GetClusterFirstKeyFrameTimeCodeDirectly( VO_U64 filepos )
{
	VO_U64 filepos_backup = m_pFileChunk->FGetFilePos();
	VO_U64 CurFilePos = filepos;
	VO_U64 EndFilePos;
	VO_S64 clustertimecode = 0;
	VO_S64 mostsmallrelativetimecode = 0x7fffffffffffffffLL;

	m_pFileChunk->FLocate( filepos );

	IDAndSizeInfo info;
	if( !ReadIDAndSize( &info ) )
	{
		m_pFileChunk->FLocate( filepos_backup );
		return 0xffffffffffffffffLL;
	}

	MemFree( info.pID );

	EndFilePos = CurFilePos + info.size;

	while ( 1 )
	{
		IDAndSizeInfo subinfo;

		if( !ReadIDAndSize( &subinfo ) )
			break;

		if( MemCompare( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_TIMECODE , subinfo.s_ID ) == 0 )
		{
			if (subinfo.size <= 4 )
			{
				Read_uint( (VO_U32)subinfo.size , &clustertimecode );
				clustertimecode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * clustertimecode);
			}
			else///<TIMECODE should be uint,so if it > 4,skip info.size bytes and the reset the value
			{
				break;
			}
		}
		else if( MemCompare( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP , subinfo.s_ID ) == 0 )
		{
			VO_U64 endpos = m_pFileChunk->FGetFilePos() + subinfo.size;

			VO_S64 time = GetFirstKeyFrameRelativeTime( m_pFileChunk->FGetFilePos() , subinfo.size );

			if( time != 0x7fffffffffffffffLL )
			{
				if( time < mostsmallrelativetimecode )
					mostsmallrelativetimecode = time;
			}

			m_pFileChunk->FLocate(endpos);
		}
		else if( MemCompare( subinfo.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_SIMPLEBLOCK , subinfo.s_ID ) == 0 )
		{
			VO_U64 endpos = m_pFileChunk->FGetFilePos() + subinfo.size;

			VO_S64 id;
			Read_vint( &id );

			VO_S64 relativetimecode;
			Read_sint( 2 , &relativetimecode );
			relativetimecode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * relativetimecode);

			VO_U8 flag;
			m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

			if( (flag & 0x80) && id == m_VideoID )
			{
				if( relativetimecode < mostsmallrelativetimecode )
					mostsmallrelativetimecode = relativetimecode;
			}

			m_pFileChunk->FLocate( endpos );
		}
		else
		{
			VO_U64 pos = m_pFileChunk->FGetFilePos();
			m_pFileChunk->FLocate( pos + subinfo.size );
		}

		MemFree(subinfo.pID);

		CurFilePos = m_pFileChunk->FGetFilePos();

		if( CurFilePos >= EndFilePos )
			break;
	}

	m_pFileChunk->FLocate( filepos_backup );

	if( mostsmallrelativetimecode == 0x7fffffffffffffffLL )
		return 0xffffffffffffffffLL;
	else
		return mostsmallrelativetimecode + clustertimecode;
}

VO_S64 CMKVDataParser::GetFirstKeyFrameRelativeTime( VO_U64 filepos , VO_U64 blockgroupsize )
{
	VO_U64 filepos_backup = filepos;
	VO_U64 endpos = filepos + blockgroupsize;

	VO_BOOL isKeyFrame = VO_TRUE;
	VO_S64 submostsmallframerelativecode = 0x7fffffffffffffffLL;

	m_pFileChunk->FLocate( filepos );

	while( 1 )
	{
		IDAndSizeInfo info;
		if(!ReadIDAndSize( &info ))
		{
			break;
		}
		if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_BLOCK , info.s_ID ) == 0 )
		{
			VO_U64 subendpos = m_pFileChunk->FGetFilePos() + info.size;
			VO_S64 id;
			Read_vint( &id );

			VO_S64 relativetimecode;
			Read_sint( 2 , &relativetimecode );
			relativetimecode = (VO_S64)(m_pSegInfo->TimecodeScale / 1000000. * relativetimecode);

			if( m_simpleblock_kf_way || m_groupblock_kf_way)
			{
				VO_U8 flag;
				m_pFileChunk->FRead( &flag , sizeof( VO_U8 ) );

				if( (flag & 0x80) && id == m_VideoID )
				{
					if( relativetimecode < submostsmallframerelativecode )
						submostsmallframerelativecode = relativetimecode;
				}
			}
			else
			{
				if( (id == m_VideoID) && (relativetimecode < submostsmallframerelativecode) )
					submostsmallframerelativecode = relativetimecode;

				if( id != m_VideoID )
					isKeyFrame = VO_FALSE;
			}

			m_pFileChunk->FLocate( subendpos );
		}
		else if( MemCompare( info.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER_BLOCKGROUP_REFERENCEBLOCK , info.s_ID ) == 0 )
		{
			isKeyFrame = VO_FALSE;
			MemFree( info.pID );
			break;
		}
		else
		{
			m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );
		}

		MemFree( info.pID );

		if( m_pFileChunk->FGetFilePos() >= endpos )
			break;
	}

	m_pFileChunk->FLocate( filepos_backup );

	if( isKeyFrame )
		return submostsmallframerelativecode;
	else
		return 0x7fffffffffffffffLL;
}
VO_U64 CMKVDataParser::GetPositionWithCuesInfoByCount(const MKVSegmentStruct * pSegInfo )
{
	VO_U64 pos = 0xFFFFFFFFFFFFFFFFULL;

	voLIST_ENTRY * pEntry = pSegInfo->CuesCuePointEntryListHeader.Flink;
	VO_U64 nCurPos = m_pFileChunk->FGetFilePos();
	CuesCuePointStruct * pCue;
	CuesCuePointTrackPositionsStruct * pPointer = VO_NULL;
	while (pEntry != &(pSegInfo->CuesCuePointEntryListHeader) )
	{
		pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
		pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
		if (pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos >= (VO_S64)nCurPos)
		{
			pos = pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
			break;
		}
		pEntry = pEntry->Flink;
	}
	return pos;
	
}
VO_U64 CMKVDataParser::GetPositionWithCuesInfoByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo )
{
	voLIST_ENTRY * pEntry = pSegInfo->CuesCuePointEntryListHeader.Flink;
	VO_S64 MaxCueTime = 0;

	VO_S64 TempTimeStamp = ((llTimeStamp - 5000) <=0)?0:(llTimeStamp - 5000);

	while( pEntry != &(pSegInfo->CuesCuePointEntryListHeader) )
	{
		CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );

		if( pCue->CueTime * pSegInfo->TimecodeScale / 1000000. >= TempTimeStamp )
		{
			if (pCue->CueTime * pSegInfo->TimecodeScale / 1000000. == TempTimeStamp)
			{
				CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
				return pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
			}
			break;
		}

		MaxCueTime = (VO_S64)(pCue->CueTime* pSegInfo->TimecodeScale / 1000000.);

		pEntry = pEntry->Flink;
	}
///< i don't think "llTimeStamp < MaxCueTime" does make any sence,if don't get the pEntry with cue tiem more than llTimeStamp
	///<we should TravelCluster from the last cue point with MaxCueTime to end of segment 
#if 0
	if( pEntry == &(pSegInfo->CuesCuePointEntryListHeader)  && llTimeStamp < MaxCueTime)
	{
		VO_U64 ret= GetPositionByTravelCluster( llTimeStamp , pSegInfo );
		return ret;
	}
#else
	if( pEntry == &(pSegInfo->CuesCuePointEntryListHeader))
	{
		voLIST_ENTRY * pEntry = pSegInfo->CuesCuePointEntryListHeader.Blink;
		if (pEntry == &(pSegInfo->CuesCuePointEntryListHeader))
		{
			///<There is no list in Cue segment,but i don't think it make sence,just in case
			return GetPositionByTravelCluster( TempTimeStamp , pSegInfo );
		}
		CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
		if (MaxCueTime != pCue->CueTime* pSegInfo->TimecodeScale / 1000000.)
		{
			///< the cue segment is not ordered by cue time,there must be something wrong 
			return GetPositionByTravelCluster( TempTimeStamp , pSegInfo );
		}
		CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
		 
		VO_U64 ret= GetPositionPrecisely( TempTimeStamp , pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos);
		return ret;
	}
#endif
	if( pEntry->Blink != &(pSegInfo->CuesCuePointEntryListHeader) )
		pEntry = pEntry->Blink;
	else
		return GuessPositionByTime( llTimeStamp , pSegInfo );

	CuesCuePointStruct * pCue = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
	CuesCuePointTrackPositionsStruct * pPointer = GET_OBJECTPOINTER(pCue->TrackPositionsHeader.Flink , CuesCuePointTrackPositionsStruct , List );
	return pPointer->CueClusterPosition + pSegInfo->SegmentBeginPos;
}

VO_U64 CMKVDataParser::GetPositionPrecisely( VO_S64 llTimeStamp , VO_U64 posnear )
{
	VO_U64 clusterpre = posnear;
	VO_U64 timecode = 0;
	VO_U64 pos = posnear;

	m_pFileChunk->FLocate( posnear );

	IDAndSizeInfo info;
	if( !ReadIDAndSize( &info ) )
		return pos;

	MemFree( info.pID );

	m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + info.size );

	while( 1 )
	{
		VO_U64 backup = m_pFileChunk->FGetFilePos();

		IDAndSizeInfo infonext;
		if( !ReadIDAndSize( &infonext ) )
			return clusterpre;

		if( MemCompare( infonext.pID , (VO_PTR)MKVID_SEGMENT_CLUSTER , infonext.s_ID ) != 0 )
		{
			MemFree( infonext.pID );
			return clusterpre;
		}

		MemFree( infonext.pID );

//		timecode = GetClusterFirstKeyFrameTimeCodeDirectly( backup );
		timecode = GetClusterByTimeCodeDirectly(backup);
		if( timecode == 0xffffffffffffffffLL )
		{
			;
		}
		else if( timecode > llTimeStamp ){
			pos = clusterpre;
			break;
		}
		else if( timecode == llTimeStamp ){
			pos = backup;
			break;
		}
		else
			clusterpre = backup;

		m_pFileChunk->FLocate( m_pFileChunk->FGetFilePos() + infonext.size );
	}
	
	VOLOGI("GetPositionPrecisely--pos=%llu, llTimeStamp=%lld, timecode=%llu",pos,llTimeStamp, timecode);

	return pos;
}

VO_BOOL CMKVDataParser::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return m_pFileChunk->FRead2(*ppBuffer, nSize, nPos);
}

VO_VOID CMKVDataParser::Reset()
{
	m_bIsInBlock = VO_FALSE;
	m_framecount = 0;
	m_currentframeindex = 0;
	m_currentblocksize = 0;
	m_block_timecode = 0;
	m_block_tracknumber = 0;
	m_CurrentTimeCode = -1;
	m_nCurCluserEndPos = 0;
	m_simpleblock_kf_way = VO_FALSE;
}

VO_VOID CMKVDataParser::CopyParametersFromOtherParser( CMKVDataParser& other )
{
	m_bIsInBlock = other.m_bIsInBlock;
	m_framecount = other.m_framecount;
	m_currentframeindex = other.m_currentframeindex;
	m_currentblocksize = other.m_currentblocksize;
	m_block_timecode = other.m_block_timecode;
	m_block_tracknumber = other.m_block_tracknumber;
	m_CurrentTimeCode = other.m_CurrentTimeCode;
	m_nCurCluserEndPos = other.m_nCurCluserEndPos;
	m_bIsKeyFrame = other.m_bIsKeyFrame;
	m_simpleblock_kf_way = other.m_simpleblock_kf_way;
	m_groupblock_kf_way = other.m_groupblock_kf_way;
	m_VideoID = other.m_VideoID;
	MemCopy(m_framesizearray,other.m_framesizearray, 256 * sizeof( VO_U32 ));
}
