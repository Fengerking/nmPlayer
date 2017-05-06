#include "CMKVFileReader.h"
#include "CMKVAudioTrack_RealSpecialize.h"
#include "CMKVVideoTrack_RealSpecialize.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "fVideoHeadDataInfo.h"


CTrackCenter::CTrackCenter()
{
	;
}
CTrackCenter::~CTrackCenter()
{
	Flush();
}

VO_BOOL CTrackCenter::RegistNewTrack(CBaseTrack* pTrack)
{
	VO_BOOL	bResult = VO_FALSE;
	
	if(NULL == pTrack)
		return bResult;
	
	m_nTrackList.push_back(pTrack);

	return bResult;
}

VO_VOID CTrackCenter::UnRegistTrack(CBaseTrack* pTrack)
{
	list_T<CBaseTrack *>::iterator iter;
	for(iter=m_nTrackList.begin(); iter!=m_nTrackList.end(); ++iter)
	{
		CBaseTrack* pBaseTrack = (CBaseTrack*)(*iter);
		if(pBaseTrack == pTrack){
			m_nTrackList.remove(iter);
			break;
		}
		
	}
}

VO_VOID CTrackCenter::Flush()
{
	list_T<CBaseTrack *>::iterator iter;
	for(iter=m_nTrackList.begin(); iter!=m_nTrackList.end(); ++iter)
	{
		delete *iter;
	}
	m_nTrackList.clear();

}

VO_VOID	CTrackCenter::InitTracks()
{
	;
}	

VO_VOID	CTrackCenter::UnInitTracks()
{
	;
}

VO_U32 CTrackCenter::GetTrackNumbers()
{
	return m_nTrackList.size();
}

CBaseTrack* CTrackCenter::GetTrackbyTrackNumber(VO_U32 TrackNumber)
{
	CBaseTrack* pBaseTrack = VO_NULL;

	list_T<CBaseTrack *>::iterator iter;
	for(iter=m_nTrackList.begin(); iter!=m_nTrackList.end(); ++iter)
	{
		if(pBaseTrack->GetStreamNum() == TrackNumber){
			pBaseTrack = (CBaseTrack*)(*iter);			
			break;
		}
		
	}

	return pBaseTrack;

}
CBaseTrack* CTrackCenter::GetTrackbyIndex(VO_U32 Index)
{
	CBaseTrack* pBaseTrack = VO_NULL;
	VO_U32 count = 0;

	list_T<CBaseTrack *>::iterator iter;
	for(iter=m_nTrackList.begin(); iter!=m_nTrackList.end(); ++iter)
	{
		if(count == Index){
			pBaseTrack = (CBaseTrack*)(*iter);			
			break;
		}
		count++;
	}

	return pBaseTrack;
}

CMKVFileReader::CMKVFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
:CBaseReader( pFileOp , pMemOp, pLibOP, pDrmCB)
//,m_HeadAnaylze( &m_chunk , pMemOp )
//,m_DataParser( &m_chunk , pMemOp )
//,m_pVideoTrackEntry( NULL )
//,m_pAudioTrackEntry( NULL )
//,m_pVideoTrack( NULL )
//,m_pAudioTrack( NULL )
//,m_Duration( 0 )
//,m_is_init(VO_FALSE)
{
	m_pAnalyze = new CMKVAnalyze( &m_chunk);
	m_pCTrackCenter = new CTrackCenter();
	
	m_pAnalyze->RegistNotify(SEGMENT, (VO_PTR)this, SegmentComplete);
	m_pAnalyze->RegistNotify(TRACK, (VO_PTR)this, TracksComplete);

//	m_pFileDataParser = &m_DataParser;
}

CMKVFileReader::~CMKVFileReader(void)
{
	Close();

	if(m_pAnalyze){
		delete m_pAnalyze;
	}

	if(m_pCTrackCenter){
		delete m_pCTrackCenter;
	}
	
/*
	if(m_pSegmentInfo){
		delete m_pSegmentInfo;
	}
*/	
}

VO_BOOL CMKVFileReader::SegmentComplete(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{
	if(!wParam)
		return VO_FALSE;
	
	if(!pUser){
		return VO_FALSE;
	}

	VOLOGI("SegmentComplete");

	CMKVFileReader* pReader = (CMKVFileReader*)pUser;
	
	pReader->m_pSegmentInfo = (MKVSegmentStruct*)wParam;

	return VO_TRUE;
}

VO_BOOL CMKVFileReader::TracksComplete(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{

	if(!wParam)
		return VO_FALSE;
	
	if(!pUser){
		return VO_FALSE;
	}
	VOLOGI("TracksComplete");

	CMKVFileReader* pReader = (CMKVFileReader*)pUser;

	TracksTrackEntryStruct* pAudioTrack = NULL;
	TracksTrackEntryStruct* pVideoTrack = NULL;
	
	pVideoTrack = pReader->GetBestTrackInfo(Video_Type, (list_T<TracksTrackEntryStruct *>*)(wParam) );
	pAudioTrack = pReader->GetBestTrackInfo(Audio_Type, (list_T<TracksTrackEntryStruct *>*)(wParam) );

	pReader->InitTracks(pAudioTrack, pVideoTrack );
/*
//	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
//		pReader->SelectDefaultTracks();
//		pReader->PrepareTracks();
		m_DataParser.Init(VO_NULL);
		if(!m_DataParser.SetStartFilePos(m_pSegmentInfo->ClusterPos, VO_TRUE))
			return VO_ERR_SOURCE_END;

		if( VideoTrackEntry )
			m_DataParser.SetVideoID( VideoTrackEntry->TrackNumber );

		m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
		m_DataParser.SetOnNotifyCallback( this , GOnGetNotify );
		m_DataParser.SetSegmentInfo( m_pSegmentInfo );
		if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
			m_DataParser.SetFlagForThumbnail(VO_TRUE);
//		m_is_init = VO_TRUE;
//		pReader->FileGenerateIndex();
//		m_is_init = VO_FALSE;
	}
*/
	if( pVideoTrack && strstr( (char *)pVideoTrack->str_CodecID , "V_MPEG4" ) 
		&& strcmp( (char *)pVideoTrack->str_CodecID , "V_MPEG4/ISO/AVC" ) != 0 )///<H264 should be excluded
		//		&& strcmp( (char *)m_pVideoTrackEntry->str_CodecID , "V_MPEG4/ISO/" ) != 0 )///<until now ,i think it make no sence,so remove
	{
		if( !pVideoTrack->pCodecPrivate)
		{
			//modefied by Aiven :issue 10340, refer to the code of mkvinfo to generate the FPS
			VO_CODECBUFFER codec_buf;
			memset((VO_PTR)&codec_buf, 0x0, sizeof(VO_CODECBUFFER));

			float nFPS = 0;

			if(pVideoTrack->DefaultDuration > 0)
				nFPS = (1000000000/ (float)VO_U64(pVideoTrack->DefaultDuration));			

			//if codec_buf->Buffer is NULL, the header size will be set into codec_buf->Length and function will return ignore other input
			//First, We will get the codec_buf.Length.
			GenerateMPEG4Header(&codec_buf, 0, 0, 0);

			if(codec_buf.Length > 0)
			{
				codec_buf.Buffer = (VO_PBYTE)new char[codec_buf.Length];
				memset((VO_PTR)codec_buf.Buffer, 0x0, codec_buf.Length);
				
				//Second,We will get the buffer.
				GenerateMPEG4Header(&codec_buf, pVideoTrack->PixelWidth, pVideoTrack->PixelHeight, (VO_U32)(nFPS+0.5)); 			
				pVideoTrack->CodecPrivateSize = codec_buf.Length;
				pVideoTrack->pCodecPrivate = (VO_S8 *)new VO_S8[codec_buf.Length];
				memcpy(pVideoTrack->pCodecPrivate,codec_buf.Buffer,codec_buf.Length);
				delete(codec_buf.Buffer);
			}


		}

		/*VO_U32 bitrate;
		if( !m_pVideoTrack->GetBitrate( &bitrate ) )
		{
			if( bitrate > 10 * 1024 * 1024 )
				return VO_ERR_SOURCE_FORMATUNSUPPORT;
		}*/
	}

//	if( ( m_pVideoTrackEntry->ContentCompAlgo != -1 && m_pVideoTrackEntry->ContentCompAlgo != 3 ) ||
//		( m_pAudioTrackEntry->ContentCompAlgo != -1 && m_pAudioTrackEntry->ContentCompAlgo != 3 ) )
//		return VO_ERR_SOURCE_FORMATUNSUPPORT;


	return VO_TRUE;
}


VO_U32 CMKVFileReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	
	m_nSourceOpenFlags = nSourceOpenFlags;
//	VO_BYTE *tmp = new VO_BYTE[1024]; 
	VOLOGI( "+CMKVFileReader::Load %d" , voOS_GetSysTime() );
	Close();

	VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	VOLOGI( "FileSize : %lld" , m_ullFileSize );

	m_pAnalyze->AnalyzeFile( (VO_S64)m_ullFileSize );
	InitalizeTracks();
	VOLOGI( "-CMKVFileReader::Load %d" , voOS_GetSysTime() );

	return VO_ERR_SOURCE_OK;
}

VO_VOID CMKVFileReader::InitalizeTracks()
{
	VO_U32 Duration = (VO_U32)(m_pSegmentInfo->Duration * m_pSegmentInfo->TimecodeScale / 1000000);
	VO_U32 length = m_pCTrackCenter->GetTrackNumbers();
	
	for(VO_U32 index=0; index<length; index++ ){		
		
		CBaseTrack* pTrack = GetTrackByIndex(index);
		if(pTrack){
			pTrack->SetDuration(Duration);
			pTrack->SetInUsed(VO_TRUE);
			((CMKVTrack*)pTrack)->SetNotifyFuc((VO_PTR)this, NeedCreateNewDataParser);
			((CMKVTrack*)pTrack)->Start();
		}
	}
}

VO_U32 CMKVFileReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	if(!pSourceInfo)
		return VO_ERR_INVALID_ARG;
	
	VO_U32 Duration = (VO_U32)(m_pSegmentInfo->Duration * m_pSegmentInfo->TimecodeScale / 1000000);

	pSourceInfo->Duration = Duration;
	pSourceInfo->Tracks = m_pCTrackCenter->GetTrackNumbers();

	return VO_ERR_SOURCE_OK;
}


VO_U32 CMKVFileReader::Close()
{
/*
	UnprepareTracks();

	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}
*/
	return CBaseReader::Close();
}

VO_U32 CMKVFileReader::InitTracks(TracksTrackEntryStruct* pAudioTrackEntry, TracksTrackEntryStruct* pVideoTrackEntry)
{
	CMKVVideoTrack * VideoTrack = NULL;
	CMKVAudioTrack * AudioTrack = NULL;
	VO_U32	MaxFrameCnt = MAX_CACHE_FRAME_COUNT;
	VO_U32 Duration = 0;//(VO_U32)(m_pSegmentInfo->Duration * m_pSegmentInfo->TimecodeScale / 1000000);
/*
	if((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
	{
		TracksCreate( 1 );
		m_nTracks = 1;
	}
	else
	{
		TracksCreate( 2 );
		m_nTracks = 2;
	}
*/	
	TracksTrackEntryStruct* AudioTrackEntry = NULL;
	TracksTrackEntryStruct* VideoTrackEntry = NULL;

	
	AudioTrackEntry = pAudioTrackEntry;
	VideoTrackEntry = pVideoTrackEntry;

	/*		
	
	if (VideoTrackEntry)
	{
		if (MemCompare( VideoTrackEntry->str_CodecID , (VO_PTR)"V_MPEG4/ISO/AVC" , strlen( "V_MPEG4/ISO/AVC" ) ) == 0)
		{
			VO_U32 nTmpSize = VideoTrackEntry->PixelHeight*VideoTrackEntry->PixelWidth;
			if (nTmpSize*MAX_CACHE_FRAME_COUNT > 1920*1080*6)
			{
				MaxFrameCnt = 1920*1080*6/nTmpSize;
			}
		}
		if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
		{
			VOLOGI("MaxFrameCnt = 5");
			if (MaxFrameCnt > 1)
			{
				MaxFrameCnt = 1;
			}
		}
	}
	*/		

	if( AudioTrackEntry )
	{
		VO_U16 wInterleaveBlockSize = 0;
		if( MemCompare( AudioTrackEntry->str_CodecID , (VO_PTR)"A_REAL" , 6 ) == 0 )
		{
			AudioTrack = new CMKVAudioTrack_RealSpecialize( (VO_U8)AudioTrackEntry->TrackNumber , Duration , AudioTrackEntry , m_pMemOp );
			wInterleaveBlockSize = ((CMKVAudioTrack_RealSpecialize*)AudioTrack)->GetInterleaveBlockSize();
		}
		else
		{
			AudioTrack = new CMKVAudioTrack( (VO_U8)AudioTrackEntry->TrackNumber , Duration , AudioTrackEntry , m_pMemOp );
		}

		AudioTrack->SetDataParser((Parser*)m_pAnalyze);
		m_pCTrackCenter->RegistNewTrack((CBaseTrack *)AudioTrack);

/*
		AudioTrack->SetGeneralSegmentInfo( m_pSegmentInfo );
		AudioTrack->SetBufferTime(MaxFrameCnt);
		if(wInterleaveBlockSize)
			AudioTrack->SetGlobalBufferExtSize(wInterleaveBlockSize);
		else
		{
			VO_U32 dwMaxSampleSize = 0;
			AudioTrack->GetMaxSampleSize(&dwMaxSampleSize);
			AudioTrack->SetGlobalBufferExtSize(dwMaxSampleSize * 2);
		}
*/
//		m_ppTracks[0] = AudioTrack;
	}

	if( VideoTrackEntry )
	{
		if( MemCompare( VideoTrackEntry->str_CodecID , (VO_PTR)"V_REAL" , 6 ) == 0 )
		{
			VideoTrack = new CMKVVideoTrack_RealSpecialize( (VO_U8)VideoTrackEntry->TrackNumber , Duration , VideoTrackEntry , m_pMemOp );
		}
		else
		{
			VideoTrack = new CMKVVideoTrack( (VO_U8)VideoTrackEntry->TrackNumber , Duration , VideoTrackEntry , m_pMemOp );
		}

		VideoTrack->SetDataParser((Parser*)m_pAnalyze);
		m_pCTrackCenter->RegistNewTrack((CBaseTrack *)VideoTrack);
/*
		VideoTrack->SetGeneralSegmentInfo( m_pSegmentInfo );

		VideoTrack->SetBufferTime(MaxFrameCnt);
		VO_U32 dwMaxSampleSize = 0;
		VideoTrack->GetMaxSampleSize(&dwMaxSampleSize);
		VideoTrack->SetGlobalBufferExtSize(dwMaxSampleSize * 2);

		if ((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL))
		{
			m_ppTracks[0] = VideoTrack;
//			VideoTrack->SetFlagForThumbnail(VO_TRUE);
		}
		else
			m_ppTracks[1] = VideoTrack;
*/					
	}

//	((CMKVTrack*)m_ppTracks[0])->TrackGenerateIndex();

	return VO_ERR_SOURCE_OK;
}


TracksTrackEntryStruct * CMKVFileReader::GetBestTrackInfo(TrackType tracktype,list_T<TracksTrackEntryStruct *>* pEntry )
{
	if( pEntry == NULL )
		return NULL;
		
	TracksTrackEntryStruct * pTempSelect = NULL;
	TracksTrackEntryStruct*  pTrackEntry;
	list_T<TracksTrackEntryStruct *>::iterator iter;
	
	for(iter=pEntry->begin(); iter!=pEntry->end(); ++iter)
	{
		pTrackEntry = (TracksTrackEntryStruct *)(*iter);
		
		if( pTrackEntry->TrackType == tracktype )// 1 means audio
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
	
	}

	return pTempSelect;
}

VO_U32 CMKVFileReader::MoveTo(VO_S64 llTimeStamp)
{	
	VO_U32 length = m_pCTrackCenter->GetTrackNumbers();
	
	for(VO_U32 index=0; index<length; index++ ){		
		
		CBaseTrack* pTrack = GetTrackByIndex(index);

		//clear the samplelist in track
		((CMKVTrack*)pTrack)->Flush();

		//move the fileparser pos if the track in SeekTrack
		if(index == GetSeekTrack()){
			((CMKVTrack*)pTrack)->MoveTo(llTimeStamp);
		}
	}
/*	
	for(VO_U32 i=0; i<m_nTracks; i++ ){
		
//		((CMKVTrack*)m_ppTracks[i])->Flush();
		if(m_ppTracks[i]){
			((CMKVTrack*)m_ppTracks[i])->MoveTo(llTimeStamp);
		}

//		((CMKVTrack*)m_ppTracks[i])->SetIsAboutToSeek();
	}
/*
	if( m_pVideoTrack )
		m_pVideoTrack->SetIsAboutToSeek();

	if( m_pAudioTrack )
		m_pAudioTrack->SetIsAboutToSeek();

	FileGenerateIndex();	
*/

	
	return CBaseReader::MoveTo(llTimeStamp);
}


CBaseTrack* CMKVFileReader::GetTrackByIndex(VO_U32 nIndex)
{
	return m_pCTrackCenter->GetTrackbyIndex(nIndex);
}


VO_BOOL CMKVFileReader::NeedCreateNewDataParser(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{
	CMKVFileReader* pReader = (CMKVFileReader*)pUser;
	
	VO_BOOL bResult = VO_FALSE;
	VO_U32 length = pReader->m_pCTrackCenter->GetTrackNumbers();

	for(VO_U32 index=0; index<length; index++ ){		
		CBaseTrack* pTrack = pReader->GetTrackByIndex(index);
		if(((CMKVTrack*)pTrack)->IsNeedToCreateNewDataParser()){
			bResult = VO_TRUE;
			break;
		}
	}

	return bResult;
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
