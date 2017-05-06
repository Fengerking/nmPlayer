#include "CMKVFileReader.h"
#include "CMKVAudioTrack_RealSpecialize.h"
#include "CMKVVideoTrack_RealSpecialize.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "fVideoHeadDataInfo.h"
#include "fCodec.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

//#define _SUPPORT_DIVX_

//VO_U32 CMKVFileReader::m_MaxFrameCnt = MAX_CACHE_FRAME_COUNT;

static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMKVFileReader* pReader = (CMKVFileReader*)pUser;
	MKVFrame* pSample = (MKVFrame*)pParam;

	return pReader->OnPacket( pSample );
}

static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CMKVTrack* pTrack = (CMKVTrack*)pUser;
	return pTrack->OnBlockWithFilePos(*(VO_U64*)pParam);
}

static VO_BOOL GOnGetNotify(VO_PTR pUser, VO_U8 StreamNumber , DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam )
{
	CMKVFileReader* pReader = (CMKVFileReader*)pUser;
	
	return pReader->Notify( StreamNumber , NotifyID , wParam , lParam );
}


CMKVFileReader::CMKVFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB):CBaseStreamFileReader( pFileOp , pMemOp, pLibOP, pDrmCB)
,m_HeadAnaylze( &m_chunk , pMemOp )
,m_DataParser( &m_chunk , pMemOp )
,m_pVideoTrackEntry( NULL )
,m_pAudioTrackEntry( NULL )
,m_pVideoTrack( NULL )
,m_pAudioTrack( NULL )
,m_Duration( 0 )
,m_is_init(VO_FALSE)
,m_MaxFrameCnt(MAX_CACHE_FRAME_COUNT)
{
	m_pFileDataParser = &m_DataParser;
}

CMKVFileReader::~CMKVFileReader(void)
{
	Close();
}

VO_U32 CMKVFileReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	

//	VO_BYTE *tmp = new VO_BYTE[1024]; 
	VOLOGI( "+CMKVFileReader::Load %d" , voOS_GetSysTime() );
	Close();
	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
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
	

	VOLOGI( "FileSize : %llu" , m_ullFileSize );

	m_HeadAnaylze.SetFileSize(m_ullFileSize);

	if(!m_HeadAnaylze.ReadFromFile())
		return VO_ERR_SOURCE_OPENFAIL;

	if( m_HeadAnaylze.GetSegmentCount() != 1 )	//currently we do not support multi-segment mkv file
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
/*
	if (!(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		m_pAudioTrackEntry = m_HeadAnaylze.GetBestAudioTrackInfo( 0 );
	}
	m_pVideoTrackEntry = m_HeadAnaylze.GetBestVideoTrackInfo( 0 );
	
	if( !m_pAudioTrackEntry && !m_pVideoTrackEntry )
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
	*/

	m_pSegmentInfo = m_HeadAnaylze.GetSegmentInfo( 0 );
	
	m_Duration = (VO_U32)(m_pSegmentInfo->Duration * m_pSegmentInfo->TimecodeScale / 1000000);

	if(VO_ERR_SOURCE_OK != InitTracks( nSourceOpenFlags )){
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
	}

#ifdef _SUPPORT_DIVX_
	if(!m_pVideoTrack){
		VO_U32 uFourcc = 0;
		if(VO_ERR_SOURCE_OK == m_pVideoTrack->GetCodecCC(&uFourcc)){
			if(fCodecIsDivX(uFourcc)){
				VOLOGE("mkv don't support divx");
				return VO_ERR_SOURCE_FORMATUNSUPPORT;
			}
		}
	}
#endif

//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{		
		PrepareTracks();

		m_DataParser.Init(VO_NULL);
		if(!m_DataParser.SetStartFilePos(m_pSegmentInfo->ClusterPos, VO_TRUE))
			return VO_ERR_SOURCE_END;

		if( m_pVideoTrackEntry )
			m_DataParser.SetVideoID( m_pVideoTrackEntry->TrackNumber );

		m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
		m_DataParser.SetOnNotifyCallback( this , GOnGetNotify );
		m_DataParser.SetSegmentInfo( m_pSegmentInfo );
		if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
		{
			m_pVideoTrack->SetParseForSelf( VO_TRUE );
			m_DataParser.SetFlagForThumbnail(VO_TRUE);
		}
		
		m_is_init = VO_TRUE;
		if (!(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)){
			FileGenerateIndex();
		}
		m_is_init = VO_FALSE;

		InitHeadData();
		SelectDefaultTracks();
		
	}

	if( m_pVideoTrackEntry && strstr( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4" ) 
		&& strcmp( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4/ISO/AVC" ) != 0 )///<H264 should be excluded
		//		&& strcmp( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4/ISO/" ) != 0 )///<until now ,i think it make no sence,so remove
	{
		if( !m_pVideoTrackEntry->pCodecPrivate)
		{
			//modefied by Aiven :issue 10340, refer to the code of mkvinfo to generate the FPS
			VO_CODECBUFFER codec_buf;
			MemSet((VO_PTR)&codec_buf, 0x0, sizeof(VO_CODECBUFFER));
/*
			float nFPS = 0;
			if(m_pVideoTrackEntry->DefaultDuration > 0)
				nFPS = (1000000000/ (float)VO_U64(m_pVideoTrackEntry->DefaultDuration));			
*/
			//if codec_buf->Buffer is NULL, the header size will be set into codec_buf->Length and function will return ignore other input
			//First, We will get the codec_buf.Length.
			GenerateMPEG4Header(&codec_buf, 0, 0, 0);

			if(codec_buf.Length > 0)
			{
				codec_buf.Buffer = (VO_PBYTE)MemAlloc(codec_buf.Length);
				MemSet((VO_PTR)codec_buf.Buffer, 0x0, codec_buf.Length);
				
				//Second,We will get the buffer.
				GenerateMPEG4Header(&codec_buf, m_pVideoTrackEntry->PixelWidth, m_pVideoTrackEntry->PixelHeight, m_pVideoTrackEntry->DefaultDuration);				
				m_pVideoTrackEntry->CodecPrivateSize = codec_buf.Length;
				m_pVideoTrackEntry->pCodecPrivate = (VO_S8 *)MemAlloc( codec_buf.Length);
				MemCopy(m_pVideoTrackEntry->pCodecPrivate,codec_buf.Buffer,codec_buf.Length);
				MemFree(codec_buf.Buffer);
			}


		}

		/*VO_U32 bitrate;
		if( !m_pVideoTrack->GetBitrate( &bitrate ) )
		{
			if( bitrate > 10 * 1024 * 1024 )
				return VO_ERR_SOURCE_FORMATUNSUPPORT;
		}*/
	}

// 	if( ( m_pVideoTrackEntry->ContentCompAlgo != -1 && m_pVideoTrackEntry->ContentCompAlgo != 3 ) ||
// 		( m_pAudioTrackEntry->ContentCompAlgo != -1 && m_pAudioTrackEntry->ContentCompAlgo != 3 ) )
// 		return VO_ERR_SOURCE_FORMATUNSUPPORT;

	VOLOGI( "-CMKVFileReader::Load %d" , voOS_GetSysTime() );

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVFileReader::Close()
{
	UnprepareTracks();

	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
	
	return CBaseStreamFileReader::Close();
}

VO_U32 CMKVFileReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	PvoLIST_ENTRY pEntry = NULL;
	VO_U32 nTrackSize = 0;
	VO_BOOL vflag = VO_TRUE;
	VO_BOOL afag = VO_TRUE;

	//First, Get the track numbers.
	pEntry = m_pSegmentInfo->TracksTrackEntryListHeader.Flink;
	while( pEntry != &(m_pSegmentInfo->TracksTrackEntryListHeader))
	{
		TracksTrackEntryStruct * pTmTrackEntry = NULL;
		pTmTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );
	
		if(pTmTrackEntry->TrackType == Video_Type){
			nTrackSize++;
		}
		else if(pTmTrackEntry->TrackType == Audio_Type){
			if(!(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)){					
				nTrackSize++;
			}
		}
		else{
		}
		
		pEntry = pEntry->Flink;
	}
	TracksCreate(nTrackSize);

	//Second, create the track.
	int index = 0;
	pEntry = m_pSegmentInfo->TracksTrackEntryListHeader.Flink;	
	while( pEntry != &(m_pSegmentInfo->TracksTrackEntryListHeader))
	{
		TracksTrackEntryStruct * pTmTrackEntry = NULL;
		pTmTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

		CMKVTrack* pTrack = NULL;
		if(pTmTrackEntry->TrackType == Video_Type){
			if(VO_ERR_SOURCE_OK !=InitVideoTrack(&pTrack, pTmTrackEntry, nSourceOpenFlags)){
				pEntry = pEntry->Flink;				
				continue;
			}

			TracksAdd((CBaseTrack*)pTrack);
			if(vflag){
				vflag = VO_FALSE;
				m_pVideoTrack = (CMKVVideoTrack*)pTrack;
				m_pVideoTrackEntry = pTmTrackEntry;
			}
			index++;
		}
		else if(pTmTrackEntry->TrackType == Audio_Type){
			if(!(nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)){
				if(VO_ERR_SOURCE_OK !=InitAudioTrack(&pTrack, pTmTrackEntry)){
					pEntry = pEntry->Flink;					
					continue;
				}

				TracksAdd((CBaseTrack*)pTrack);
				if(afag){
					afag = VO_FALSE;
					m_pAudioTrack = (CMKVAudioTrack*)pTrack;	
					m_pAudioTrackEntry = pTmTrackEntry;
				}
				index++;		
			}
		}
		
		pEntry = pEntry->Flink;
	}

	if( !m_pAudioTrackEntry && !m_pVideoTrackEntry )
		return VO_ERR_SOURCE_FORMATUNSUPPORT;

	return CBaseStreamFileReader::InitTracks(nSourceOpenFlags);
}

VO_U32 CMKVFileReader::InitAudioTrack(CMKVTrack** pTrack, TracksTrackEntryStruct * pTrackEntry)
{
	VO_U32	nResult = VO_ERR_SOURCE_OK;
	CMKVAudioTrack * pAudioTrack = NULL;

	if(!pTrackEntry){
		return VO_ERR_SOURCE_ERRORDATA;
	}

#if 0
	//not support DTS,
	if(!MemCompare( pTrackEntry->str_CodecID , (VO_PTR)"A_DTS" , 5 )){
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
	}
#endif

	VO_U16 wInterleaveBlockSize = 0;

	if( MemCompare( pTrackEntry->str_CodecID , (VO_PTR)"A_REAL" , 6 ) == 0 )
	{
		pAudioTrack = new CMKVAudioTrack_RealSpecialize( (VO_U8)pTrackEntry->TrackNumber , m_Duration , pTrackEntry , this , m_pMemOp );
		wInterleaveBlockSize = ((CMKVAudioTrack_RealSpecialize*)pAudioTrack)->GetInterleaveBlockSize();
	}
	else
	{
		pAudioTrack = new CMKVAudioTrack( (VO_U8)pTrackEntry->TrackNumber , m_Duration , pTrackEntry , this , m_pMemOp );
	}
	pAudioTrack->SetGeneralSegmentInfo( m_pSegmentInfo );
	pAudioTrack->SetBufferTime(m_MaxFrameCnt);
	
	memcpy(pAudioTrack->m_strLanguage, pTrackEntry->str_Language, sizeof(pTrackEntry->str_Language));
	if(wInterleaveBlockSize)
		pAudioTrack->SetGlobalBufferExtSize(wInterleaveBlockSize);
	else
	{
		VO_U32 dwMaxSampleSize = 0;
		pAudioTrack->GetMaxSampleSize(&dwMaxSampleSize);
		pAudioTrack->SetGlobalBufferExtSize(dwMaxSampleSize * 2);
	}

//	m_ppTracks[0] = m_pAudioTrack;

	*pTrack = (CMKVTrack*)pAudioTrack;

	return nResult;
}

VO_U32 CMKVFileReader::InitVideoTrack(CMKVTrack** pTrack, TracksTrackEntryStruct * pTrackEntry, VO_U32 nSourceOpenFlags)
{
	VO_U32	nResult = VO_ERR_SOURCE_OK;
	CMKVVideoTrack * pVideoTrack = NULL;

	if(!pTrackEntry){
		return VO_ERR_SOURCE_ERRORDATA;
	}

	if (MemCompare( pTrackEntry->str_CodecID , (VO_PTR)"V_MPEG4/ISO/AVC" , strlen( "V_MPEG4/ISO/AVC" ) ) == 0)
	{
		VO_U32 nTmpSize = pTrackEntry->PixelHeight*pTrackEntry->PixelWidth;
		if (nTmpSize*MAX_CACHE_FRAME_COUNT > 1920*1080*6)
		{
			m_MaxFrameCnt = 1920*1080*6/nTmpSize;
		}
	}
	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		VOLOGI("m_MaxFrameCnt = 5");
		if (m_MaxFrameCnt > 1)
		{
			m_MaxFrameCnt = 1;
		}
	}

	if( MemCompare( pTrackEntry->str_CodecID , (VO_PTR)"V_REAL" , 6 ) == 0 )
	{
		pVideoTrack = new CMKVVideoTrack_RealSpecialize( (VO_U8)pTrackEntry->TrackNumber , m_Duration , pTrackEntry , this , m_pMemOp );
	}
	else
	{
		pVideoTrack = new CMKVVideoTrack( (VO_U8)pTrackEntry->TrackNumber , m_Duration , pTrackEntry , this , m_pMemOp );
	}

	pVideoTrack->SetGeneralSegmentInfo( m_pSegmentInfo );

	pVideoTrack->SetBufferTime(m_MaxFrameCnt);
	VO_U32 dwMaxSampleSize = 0;
	pVideoTrack->GetMaxSampleSize(&dwMaxSampleSize);
	pVideoTrack->SetGlobalBufferExtSize(dwMaxSampleSize * 2);	
	memcpy(pVideoTrack->m_strLanguage, pTrackEntry->str_Language, sizeof(pTrackEntry->str_Language));
	if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
//		m_ppTracks[0] = m_pVideoTrack;
		pVideoTrack->SetFlagForThumbnail(VO_TRUE);
	}
//	else
//		m_ppTracks[1] = m_pVideoTrack;

	*pTrack = (CMKVTrack*)pVideoTrack;
	
	return nResult;
}

VO_U8 CMKVFileReader::OnPacket( MKVFrame* pSample )
{
	if( m_is_init )
	{
		if( m_pVideoTrack && m_pVideoTrackEntry && pSample->TrackNumber == m_pVideoTrack->GetStreamNum() && !m_pVideoTrackEntry->pCodecPrivate )
		{
			if( strstr( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4" )
				&& strcmp( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4/ISO/AVC" ) != 0 )///<H264 should be excluded
		//		&& strcmp( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4/ISO/" ) != 0 )///<until now ,i think it make no sence,so remove
			{
				ParseMPEG4SequenceHeader( pSample , m_pVideoTrackEntry );
			}
		}
	}

	if( m_pVideoTrack && pSample->TrackNumber == m_pVideoTrack->GetStreamNum() )
	{
		//pSample->pFileChunk->FSkip( 1 );
		//pSample->framesize -= 1;
		if( !m_pVideoTrack->OnReceivedData( pSample ) )
			return CBRT_FALSE;
	}
	else if( m_pAudioTrack && pSample->TrackNumber == m_pAudioTrack->GetStreamNum() )
	{
		if( !m_pAudioTrack->OnReceivedData( pSample ) )
			return CBRT_FALSE;
	}
	else
	{
		return CBRT_FALSE;
	}

	return CBRT_CONTINUABLENOTNEEDSKIP;
}

VO_BOOL CMKVFileReader::Notify( VO_U8 StreamNumber , DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam )
{
	if( StreamNumber == 0xff )
	{
		if( m_pAudioTrack )
			m_pAudioTrack->Notify( NotifyID , wParam , lParam );
		if( m_pVideoTrack )
			m_pVideoTrack->Notify( NotifyID , wParam , lParam );
	}
	else
	{
		if( m_pAudioTrack && StreamNumber != m_pAudioTrack->GetStreamNum() )
			m_pAudioTrack->Notify( NotifyID , wParam , lParam );

		if( m_pVideoTrack && StreamNumber != m_pVideoTrack->GetStreamNum() )
			m_pVideoTrack->Notify( NotifyID , wParam , lParam );
	}

	return VO_TRUE;
}

VO_VOID CMKVFileReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CMKVDataParser* pNewDataParser = new CMKVDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	pNewDataParser->Init(VO_NULL);
	pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
	pNewDataParser->SetBlockStream(GetOtherTrackInUsed(pRqstTrack)->GetStreamNum());
	pNewDataParser->SetOnNotifyCallback( this , GOnGetNotify );
	pNewDataParser->SetSegmentInfo( m_pSegmentInfo );
	pNewDataParser->CopyParametersFromOtherParser( m_DataParser );

	m_DataParser.SetBlockStream(pRqstTrack->GetStreamNum());
	m_DataParser.SetOnBlockCallback(pRqstTrack, VO_NULL, GOnBlock);

	if( m_pVideoTrack )
		m_pVideoTrack->SetParseForSelf( VO_TRUE );

	if( m_pAudioTrack )
		m_pAudioTrack->SetParseForSelf( VO_TRUE );

	*ppDataParser = pNewDataParser;
}

VO_U32 CMKVFileReader::MoveTo(VO_S64 llTimeStamp)
{
	Flush();
	
	((CMKVDataParser *)m_pFileDataParser)->MoveTo( llTimeStamp , m_pSegmentInfo );

	if( m_pVideoTrack )
		m_pVideoTrack->SetIsAboutToSeek();

	if( m_pAudioTrack )
		m_pAudioTrack->SetIsAboutToSeek();

	FileGenerateIndex();

	return CBaseReader::MoveTo(llTimeStamp);
}

VO_U32 CMKVFileReader::MultiTrackMoveTo(CMKVTrack* pTrack, VO_S64 llTimeStamp)
{
	VO_U32 nResult = VO_ERR_SOURCE_OK;

	VOLOGI("MultiTrackMoveTo---llTimeStamp=%lld", llTimeStamp);
	
	((CMKVDataParser *)m_pFileDataParser)->Reset();
	m_pFileDataParser->SetParseEnd(VO_FALSE);
	m_pFileDataParser->SetBlockStream(0xFF);
	m_pFileDataParser->SetOnBlockCallback(VO_NULL, VO_NULL, VO_NULL);

	m_chunk.SetShareFileHandle(VO_FALSE);
	pTrack->Flush();
	
	((CMKVDataParser *)m_pFileDataParser)->MoveTo( llTimeStamp , m_pSegmentInfo );

	if( m_pVideoTrack )
		m_pVideoTrack->SetIsAboutToSeek();

	if( m_pAudioTrack )
		m_pAudioTrack->SetIsAboutToSeek();

	pTrack->TrackGenerateIndex();

	VO_U32 timestamp = 0;
	VO_S64 targettime = llTimeStamp;

	pTrack->SetPosN(&targettime, VO_TRUE);
	pTrack->GetCurrentTimeStamp(&timestamp);
	VOLOGI("MultiTrackMoveTo---targettime = %lld, timestamp=%lu", targettime, timestamp);
/*
	m_pAudioTrack->SetPosN(&targettime);
	m_pAudioTrack->GetCurrentTimeStamp(&timestamp);
	VOLOGI("MultiTrackMoveTo---audio---targettime = %lld, timestamp=%lu", targettime, timestamp);

	timestamp = 0;
	targettime = llTimeStamp;
	
	m_pVideoTrack->SetPosN(&targettime, VO_TRUE);
	m_pVideoTrack->GetCurrentTimeStamp(&timestamp);	
	VOLOGI("MultiTrackMoveTo---video---targettime = %lld, timestamp=%lu", targettime, timestamp);
*/
	return nResult;
}

VO_VOID CMKVFileReader::Flush()
{
	((CMKVDataParser *)m_pFileDataParser)->Reset();

	return CBaseStreamFileReader::Flush();
}

VO_VOID CMKVFileReader::ParseMPEG4SequenceHeader( MKVFrame * pSample , TracksTrackEntryStruct * pVideoTrackEntry )
{
	VO_U64 pos_stor = pSample->pFileChunk->FGetFilePos();

	VO_BYTE * ptr_buffer = (VO_BYTE *)MemAlloc( (VO_U32)pSample->framesize );

	if( !ptr_buffer )
		return;

	pSample->pFileChunk->FRead( ptr_buffer , (VO_U32)pSample->framesize );

	pSample->pFileChunk->FLocate( pos_stor );

	VO_BYTE sub1[4] = { 0x0 , 0x0 , 0x1, 0x20 };

	VO_BYTE * ptr_substart = memsub( ptr_buffer , (VO_U32)pSample->framesize , sub1 , sizeof( sub1 ) );

	if( !ptr_substart )
	{
		MemFree( ptr_buffer );
		return;
	}

	VO_BYTE sub2[4] = { 0x0 , 0x0 , 0x1 , 0xb6 };
 	VO_BYTE * ptr_subend = memsub( ptr_substart + sizeof(sub1) , (VO_U32)( pSample->framesize - ( ptr_substart + sizeof( sub1 ) - ptr_buffer ) ) , sub2 , sizeof( sub2 ) );

	pVideoTrackEntry->CodecPrivateSize = ptr_subend - ptr_substart;
	if(pVideoTrackEntry->pCodecPrivate){	
		MemFree( pVideoTrackEntry->pCodecPrivate );
	}
	pVideoTrackEntry->pCodecPrivate = (VO_S8 *)MemAlloc( pVideoTrackEntry->CodecPrivateSize );

	MemCopy( pVideoTrackEntry->pCodecPrivate , ptr_substart , pVideoTrackEntry->CodecPrivateSize );	
	MemFree( ptr_buffer );
}

VO_BYTE * CMKVFileReader::memsub( VO_BYTE * ptr_buffer , VO_S32 buffer_size , VO_BYTE * ptr_sub , VO_S32 sub_size )
{
	VO_BYTE * ptr_end = ptr_buffer + buffer_size;

	while( ptr_buffer < ptr_end - sub_size + 1 )
	{
		if( ptr_buffer[0] == ptr_sub[0] )
		{
			VO_S32 i = 1;
			for(  ; i < sub_size ; i++ )
			{
				if( ptr_buffer[i] != ptr_sub[i] )
					break;
			}

			if( i == sub_size )
				return ptr_buffer;
		}

		ptr_buffer++;
	}

	return NULL;
}

VO_BOOL CMKVFileReader::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return ((CMKVDataParser *)m_pFileDataParser)->GetThumbNailBuffer(ppBuffer, nSize, nPos);
}

VO_U32 CMKVFileReader::SelectStream(VO_U32 StreamNumber, VO_U64 llTs)
{
	VO_U32 nResult = VO_ERR_SOURCE_OK;
	TracksTrackEntryStruct * pTrackEntry = NULL;
	PvoLIST_ENTRY pEntry = NULL;
	VO_U32	timestamp= (VO_U32)llTs;

	//First, find the trackentry by streamnumber
	pEntry = m_pSegmentInfo->TracksTrackEntryListHeader.Flink;
	while( pEntry != &(m_pSegmentInfo->TracksTrackEntryListHeader))
	{
		TracksTrackEntryStruct * pTmTrackEntry = NULL;
		pTmTrackEntry = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

		if(pTmTrackEntry->TrackNumber == StreamNumber){
			pTrackEntry = pTmTrackEntry;
			break;
		}		
		
		pEntry = pEntry->Flink;
	}

	if(!pTrackEntry){
		return VO_ERR_SOURCE_ERRORDATA;
	}

	//Second, Get the timestamp for seeking after create the new track.
	VOLOGI("SelectStream---StreamNumber=%lu, TrackType =%lu",StreamNumber, pTrackEntry->TrackType);
	switch(pTrackEntry->TrackType)
	{
	case Video_Type:		
		nResult = VO_ERR_SOURCE_ERRORDATA;
		break;
	case Audio_Type:
		{
			CMKVTrack* pTrack = NULL;
			pTrack = (CMKVTrack*)m_pVideoTrack ? (CMKVTrack*)m_pVideoTrack : (CMKVTrack*)m_pAudioTrack;
			//set the parser_pos to skip the data the track has parsed.

			if(m_pAudioTrack->m_btStreamNum == StreamNumber){
				return VO_ERR_SOURCE_OK;
			}
			
			if(pTrack->GetType() != VOTT_AUDIO){
				pTrack->SetPosHasBeenParsed();
			}
/*			
			if(VO_TRUE != pTrack->GetCurrentTimeStamp(&timestamp)){
				return VO_ERR_SOURCE_ERRORDATA;
			}
*/			
		}
	
		break;
	default:
		break;
	}

	//Third,  switch to the track we wanted.
	switch(pTrackEntry->TrackType)
	{
	case Video_Type:		
		nResult = VO_ERR_SOURCE_ERRORDATA;
		break;
	case Audio_Type:		
		if(m_pAudioTrack){
			m_pAudioTrack->SetInUsed(VO_FALSE);
		}
		m_pAudioTrack = (CMKVAudioTrack*)GetTrackByStreamNum(pTrackEntry->TrackNumber);
		m_pAudioTrackEntry = pTrackEntry;
		break;
	default:
		break;
	}

	if(VO_ERR_SOURCE_OK != nResult){
		return nResult;
	}

	m_pVideoTrack->PrintIndex();


	//Fourth, seek to the position by time.
	switch(pTrackEntry->TrackType)
	{
	case Video_Type:		
		nResult = VO_ERR_SOURCE_ERRORDATA;
		break;
	case Audio_Type:
		nResult = MultiTrackMoveTo(m_pAudioTrack, timestamp);
		break;
	default:
		break;
	}

	m_pAudioTrack->PrintIndex();
	m_pVideoTrack->PrintIndex();
	
	return nResult;
}

VO_U32 CMKVFileReader::SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs)
{
	VO_U32	nResult = VO_ERR_SOURCE_OK;

	nResult = CBaseReader::SetSelectTrack(nIndex, bInUsed, 0);
	VOLOGI("SetSelectTrack---index=%lu",nIndex);
	
	if(VO_ERR_SOURCE_OK != nResult){
		return VO_ERR_SOURCE_ERRORDATA;
	}

	if(bInUsed){
		nResult = SelectStream(((CMKVTrack*)m_ppTracks[nIndex])->m_btStreamNum, llTs);
	}

	return nResult;
}

VO_U32 CMKVFileReader::InitHeadData()
{
	VO_U32 index = 0;
	CBaseTrack* pTrack = GetTrackByIndex(index);
	VO_CODECBUFFER HeadData;
	while(pTrack){

		if(pTrack->IsInUsed()){
			pTrack->GetHeadData(&HeadData);
			pTrack->SetInUsed(VO_FALSE);
		}
		
		index++;
		pTrack = GetTrackByIndex(index);

	}

	return VO_ERR_SOURCE_OK;
}

