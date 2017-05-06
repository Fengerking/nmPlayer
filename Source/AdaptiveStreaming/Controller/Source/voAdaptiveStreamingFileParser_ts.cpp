#include "voAdaptiveStreamingFileParser_ts.h"
#include "voTSParser.h"
#include "voLog.h"
#include "voToolUtility.h"
#include "voProgramInfo.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingFileParser_ts"
#endif



typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);

voAdaptiveStreamingFileParser_ts::voAdaptiveStreamingFileParser_ts( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser( pCallback )
,m_isFormatChange(VO_FALSE)
,m_bTsRollback_audio(VO_FALSE)
,m_bTsRollback_video(VO_FALSE)
,m_bTsRollback_subtitle(VO_FALSE)
,m_mediatype( -1 )
,m_isSampleCallbacked(VO_FALSE)
,m_ChunkCount(0)
,m_pID3Mgr(NULL)
{
	memset( &m_Api , 0 , sizeof( VO_PARSER_API ) );

	if( pLibOp )
		SetLibOperator( ( VO_LIB_OPERATOR* )pLibOp );
	vostrcpy( m_szDllFile , _T("voTsParser") );
	vostrcpy( m_szAPIName , _T("voGetParserAPI") );

}

voAdaptiveStreamingFileParser_ts::~voAdaptiveStreamingFileParser_ts()
{
	FreeParser();
	VOLOGI("+ ~voAdaptiveStreamingFileParser_ts");
	if(m_pID3Mgr)
		delete m_pID3Mgr;
	m_pID3Mgr = NULL;
	VOLOGI("- ~voAdaptiveStreamingFileParser_ts");
}

VO_U32 voAdaptiveStreamingFileParser_ts::LoadParser()
{

	if( LoadLib(NULL) == 0 )
		return VO_RET_SOURCE2_FAIL;
	VO_U32 ret = 0;
	pvoGetParserAPI pAPI = (pvoGetParserAPI) m_pAPIEntry;
	pAPI( &m_Api );

	if( m_Api.Open )
	{
		VO_PARSER_INIT_INFO info;
		info.pProc = ParserProcEntry;
		info.pUserData = this;
		info.pMemOP = NULL;
		info.strWorkPath = m_pWorkPath;
		ret = m_Api.Open( &m_hModule , &info );
	}
	else
	{
		ret = VO_RET_SOURCE2_FAIL;
	}
	ResetTrackInfo();

	return ret;
}

VO_U32 voAdaptiveStreamingFileParser_ts::FreeParser()
{
	if( m_Api.Open && m_hModule )
	{
		m_Api.Close( m_hModule );
		m_hModule = 0;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingFileParser_ts::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	m_pCurBuffer = pBuffer;

	VO_PARSER_INPUT_BUFFER input;
	memset( &input , 0 , sizeof( VO_PARSER_INPUT_BUFFER ) );
	input.nBufLen = pBuffer->uSize;
	input.nStreamID = 0;
	input.pBuf = pBuffer->pBuffer;

	if( m_isFormatChange )
	{
		input.nFlag = VO_PARSER_FLAG_STREAM_CHANGED;
		m_isFormatChange = VO_FALSE;
	}

	if( pBuffer->bIsEnd )
		m_ChunkCount++;

	if( !m_isSampleCallbacked && m_ChunkCount >= 10 )
	{
		VODS_VOLOGW( " It should be VO_PID_DO_TS_FORCE_PLAY ");
		m_Api.SetParam( m_hModule , VO_PID_DO_TS_FORCE_PLAY , 0 );
	}

	m_Api.Process( m_hModule , &input );

	return VO_RET_SOURCE2_OK;
}
#if 1
//FILE *ff =NULL;
void voAdaptiveStreamingFileParser_ts::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	voAdaptiveStreamingFileParser_ts * ptr_parser = (voAdaptiveStreamingFileParser_ts *)pData->pUserData;
	VO_U32 uStreamID = pData->nStreamID;

	VOLOGR(" Has ParserProc. type:%d. uStreamID:%d ", pData->nType, uStreamID );
	VO_BOOL bSample = VO_FALSE;
	VO_SOURCE2_TRACK_TYPE ttype = VO_SOURCE2_TT_MAX;
	VO_MTV_FRAME_BUFFER * ptr_buffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

	voTrackInfoGroup *pInfoGroup = NULL;
	VO_BOOL *pNew = NULL;
	VO_U64 *pLastTimestamp = NULL;
	VO_BOOL *pbRollback = NULL;
	switch ( pData->nType )
	{
	case VO_PARSER_OT_AUDIO:
		{
			ttype = VO_SOURCE2_TT_AUDIO;
			pLastTimestamp = &m_nLastAudioTimestamp;
			pbRollback = &m_bTsRollback_audio;
			pNew = &m_new_audio;

			ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;
			pInfoGroup = &ptr_parser->m_audioTrackG;
			m_isSampleCallbacked = VO_TRUE;
			bSample = VO_TRUE;
		}
		break;
	case VO_PARSER_OT_VIDEO:
		{
			ttype = VO_SOURCE2_TT_VIDEO;
			pLastTimestamp = &m_nLastVideoTimestamp;
			pbRollback = &m_bTsRollback_video;
			pNew = &ptr_parser->m_new_video;

			ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;
			pInfoGroup = &m_videoTrackG;

			if( ptr_parser->m_new_video )
			{
				if( ptr_buffer->nFrameType != 0 )
					return;
			}

			m_isSampleCallbacked = VO_TRUE;

			bSample = VO_TRUE;			
		}
		break;
	case VO_PARSER_OT_TEXT:
		{
			ttype = VO_SOURCE2_TT_SUBTITLE;
			pLastTimestamp = &m_nLastSubtitleTimestamp;
			pbRollback = &m_bTsRollback_subtitle;
			pNew = &ptr_parser->m_new_subtitle;

			ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;
			pInfoGroup = &m_subtitleTrackG;

			m_isSampleCallbacked = VO_TRUE;

			bSample = VO_TRUE;			
		}
		break;
	case VO_PARSER_OT_STREAMINFO:
		{
			VO_PARSER_STREAMINFO * ptr_info = ( VO_PARSER_STREAMINFO * )pData->pOutputData;
			VO_SOURCE2_TRACK_TYPE type = VO_SOURCE2_TT_MAX;
			switch( ptr_info->eMediaType )
			{
			case VO_PARSER_MEDIA_TYPE_EX_TEXT:
				type = VO_SOURCE2_TT_SUBTITLE;
				break;
			case VO_PARSER_MEDIA_TYPE_EX_AUDIO:
				type = VO_SOURCE2_TT_AUDIO;
				break;
			case VO_PARSER_MEDIA_TYPE_EX_VIDEO:
				type = VO_SOURCE2_TT_VIDEO;
				break;
			default:
				return;
			}
			ptr_parser->CreateMediaInfo( type, ptr_info, uStreamID );
				
			bSample = VO_FALSE;

		}
		break;
	case VO_PARSER_OT_PRIVATE_DATA:
		{
#if 0
			VO_SOURCE2_CUSTOMERTAG_TIMEDTAG tag;
			tag.ullTimeStamp = m_nLastAudioTimestamp;
			tag.pData = ptr_buffer->pData;
			tag.uSize = ptr_buffer->nSize;
			tag.pReserved = NULL;
			tag.uFlag = VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_ID3TAG;

			ptr_parser->SendMediaData( VO_SOURCE2_TT_CUSTOMTIMEDTAG , &tag, uStreamID );

			bSample = VO_FALSE;
#else
			VOLOGI("VO_PARSER_OT_PRIVATE_DATA,nStartTime:%lld",ptr_buffer->nStartTime);
 			if( ptr_parser->m_pID3Mgr == NULL)
 			{
 				ptr_parser->m_pID3Mgr = new voAdaptiveStreamingFileParser_ID3( ptr_parser->m_pSampleCallback, NULL );
 				ptr_parser->m_pID3Mgr->LoadParser();
 			}
 			ptr_parser->m_pID3Mgr->SetASTrackID( ptr_parser->m_uASTrackID );
 			ptr_parser->m_pID3Mgr->SetFPTrackID( uStreamID );
 
			ptr_parser->m_pID3Mgr->SetFileFormat(m_ff);

 			VO_ADAPTIVESTREAMPARSER_BUFFER Buffer;
 			memset(&Buffer, 0x00, sizeof(VO_ADAPTIVESTREAMPARSER_BUFFER));
 			Buffer.ullStartTime = ptr_buffer->nStartTime;
 			Buffer.pBuffer	= ptr_buffer->pData;
 			Buffer.uSize	= ptr_buffer->nSize;
 			ptr_parser->m_pID3Mgr->Process( &Buffer);
			bSample = VO_FALSE;
#endif
		}
		break;
	case VO_PARSER_OT_TS_TIMESTAMP_ROLLBACK:
		{
			//ptr_parser->m_bTsRollback_audio = ptr_parser->m_bTsRollback_video = ptr_parser->m_bTsRollback_subtitle = VO_TRUE;
			VOLOGW(" TS's Timestamp rollback.");

			bSample = VO_FALSE;

		}
		break;
	default:
		bSample = VO_FALSE;
		break;
	}
	TryToSendTrackInfo( ttype, ptr_buffer->nStartTime, uStreamID );

	if( bSample )
	{			
		VOLOGR( "SampleData-Type:%d, (TimeStamp:%lld,OriginalStamp:%lld) Size:%d", pData->nType
			, ptr_buffer->nStartTime, ptr_buffer->nStartTime - ptr_parser->m_offset, ptr_buffer->nSize );

 		if( pbRollback && pLastTimestamp)
		{
			*pbRollback = CheckRollback(uStreamID, ptr_buffer->nStartTime);
			*pLastTimestamp = ptr_buffer->nStartTime;
		}
		ptr_parser->SendMediaData( ttype , ptr_buffer, uStreamID );
	}
	
}
#else

void voAdaptiveStreamingFileParser_ts::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	voAdaptiveStreamingFileParser_ts * ptr_parser = (voAdaptiveStreamingFileParser_ts *)pData->pUserData;
	VOLOGR(" Has ParserProc. type:%x", pData->nType );

	VO_BOOL bSendAgain = VO_FALSE;
	VO_SOURCE2_TRACK_TYPE ttype ;
	VO_MTV_FRAME_BUFFER * ptr_buffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;
	VO_U32 uStreamID = pData->nStreamID;

	switch ( pData->nType )
	{
	case VO_PARSER_OT_AUDIO:
		{
			ttype = VO_SOURCE2_TT_AUDIO;

			m_nLastAudioTimestamp = ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;

			if ( ptr_parser->m_new_audio )
			{
				ptr_parser->SendMediaInfo( ttype , ptr_buffer->nStartTime > 0 ? ptr_buffer->nStartTime - 1 : 0 , uStreamID);
				ptr_parser->m_new_audio = VO_FALSE;
			}


			m_isSampleCallbacked = VO_TRUE;

			bSendAgain = VO_TRUE;
		}
		break;
	case VO_PARSER_OT_VIDEO:
		{
			ttype = VO_SOURCE2_TT_VIDEO;

			ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;

			if( ptr_parser->m_new_video )
			{
				if( ptr_buffer->nFrameType != 0 )
					return;
			}

			if( ptr_parser->m_new_video )
			{
				ptr_parser->SendMediaInfo( ttype , ptr_buffer->nStartTime > 0 ? ptr_buffer->nStartTime - 1 : 0, uStreamID );
				ptr_parser->m_new_video = VO_FALSE;
			}


			m_isSampleCallbacked = VO_TRUE;

			bSendAgain = VO_TRUE;			
		}
		break;
	case VO_PARSER_OT_TEXT:
		{
			ttype = VO_SOURCE2_TT_SUBTITLE;

			ptr_buffer->nStartTime = ptr_buffer->nStartTime + ptr_parser->m_offset;

			if( ptr_parser->m_new_subtitle )
			{
				if( ptr_buffer->nFrameType != 0 )
					return;
			}

			if( ptr_parser->m_new_subtitle )
			{
				ptr_parser->SendMediaInfo( ttype , ptr_buffer->nStartTime > 0 ? ptr_buffer->nStartTime - 1 : 0, uStreamID );
				ptr_parser->m_new_subtitle = VO_FALSE;
			}


			m_isSampleCallbacked = VO_TRUE;

			bSendAgain = VO_TRUE;			
		}
		break;
	case VO_PARSER_OT_STREAMINFO:
		{
			VO_PARSER_STREAMINFO * ptr_info = ( VO_PARSER_STREAMINFO * )pData->pOutputData;

			if( ptr_info->nVideoExtraSize )
			{
				ptr_parser->CreateMediaInfo( VO_SOURCE2_TT_VIDEO , ptr_info, uStreamID );
			}

			if( ptr_info->nAudioExtraSize )
			{
				ptr_parser->CreateMediaInfo( VO_SOURCE2_TT_AUDIO , ptr_info, uStreamID );
			}

			if( ptr_info->eMediaType == VO_PARSER_MEDIA_TYPE_EX_TEXT )
			{
				ptr_parser->CreateMediaInfo( VO_SOURCE2_TT_SUBTITLE, ptr_info, uStreamID );
			}
			bSendAgain = VO_FALSE;

		}
		break;
	case VO_PARSER_OT_PRIVATE_DATA:
		{
			VO_SOURCE2_CUSTOMERTAG_TIMEDTAG tag;
			tag.ullTimeStamp = m_nLastAudioTimestamp;
			tag.pData = ptr_buffer->pData;
			tag.uSize = ptr_buffer->nSize;
			tag.pReserved = NULL;
			tag.uFlag = VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_ID3TAG;

			ptr_parser->SendMediaData( VO_SOURCE2_TT_CUSTOMTIMEDTAG , &tag, uStreamID );

			bSendAgain = VO_FALSE;

		}
		break;
	case VO_PARSER_OT_TS_TIMESTAMP_ROLLBACK:
		{
			ptr_parser->m_bTsRollback_audio = ptr_parser->m_bTsRollback_video = VO_TRUE;
			VOLOGW(" TS's Timestamp rollback.");

			bSendAgain = VO_FALSE;

		}
		break;
	default:
		bSendAgain = VO_FALSE;
		break;
	}

	if( bSendAgain )
	{
		VOLOGR( "SampleData-Type:%d, (TimeStamp:%lld,OriginalStamp:%lld) Size:%d", pData->nType
			, ptr_buffer->nStartTime, ptr_buffer->nStartTime - ptr_parser->m_offset, ptr_buffer->nSize );

		ptr_parser->SendMediaData( ttype , ptr_buffer, uStreamID );
	}

}
#endif

VO_U32 voAdaptiveStreamingFileParser_ts::SendMediaData( VO_SOURCE2_TRACK_TYPE type , VO_PTR pBuffer, VO_U32 uStreamID )
{
	/*Custom timed tag has its own structer*/
	if( type == VO_SOURCE2_TT_CUSTOMTIMEDTAG )
	{
		m_pSampleCallback->SendData( m_pSampleCallback->pUserData , type , pBuffer );
	}
	else
	{
		_SAMPLE sample;
		memset( &sample , 0 , sizeof( _SAMPLE ) );
		VO_MTV_FRAME_BUFFER *pBuf = (VO_MTV_FRAME_BUFFER *) pBuffer;
		sample.pBuffer = pBuf->pData;
		sample.uSize = pBuf->nSize;
		sample.uTime = pBuf->nStartTime;
		
#ifdef _new_programinfo
		sample.uFPTrackID = uStreamID;
		sample.uASTrackID = m_uASTrackID;
#endif
		if( pBuf->nFrameType == 0 )
			sample.uFlag = sample.uFlag | VO_SOURCE2_FLAG_SAMPLE_KEYFRAME;

		VO_BOOL *bTSRollback = NULL;
		VO_BOOL bTmp = VO_FALSE;
		if( type == VO_SOURCE2_TT_VIDEO )
			bTSRollback = &m_bTsRollback_video;
		else if( type == VO_SOURCE2_TT_AUDIO )
			bTSRollback = &m_bTsRollback_audio;
		else if( type == VO_SOURCE2_TT_SUBTITLE )
		{
			int type2 = __INTERNALSUBTITLE;
			bTSRollback = &m_bTsRollback_subtitle;
			sample.pReserve2 = (VO_VOID*)type2;
		}
		if( bTSRollback && *bTSRollback  )
		{
			sample.uFlag |= VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET ; 
			*bTSRollback = VO_FALSE;
		}

		m_pSampleCallback->SendData( m_pSampleCallback->pUserData , type , &sample );
	}

	return VO_RET_SOURCE2_OK;
}
#if 1
VO_U32 voAdaptiveStreamingFileParser_ts::CreateMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_PARSER_STREAMINFO * pStreamInfo,VO_U32 uStreamID )
{
	_TRACK_INFO trackinfo;
	memset( &trackinfo, 0x00, sizeof(_TRACK_INFO) );
	VO_U32 uCodec = 0;
	VO_U32 uExtraSize = 0;
	VO_PTR pExtraData = NULL;
	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREAUDIO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREVIDEO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			uCodec = pStreamInfo->nAudioCodecType;
			uExtraSize = pStreamInfo->nAudioExtraSize;
			pExtraData = pStreamInfo->pAudioExtraData;

			strcpy( trackinfo.sAudioInfo.chLanguage, pStreamInfo->AudioFormat.strLanguage );
			trackinfo.sAudioInfo.sFormat.Channels = pStreamInfo->AudioFormat.channels;
			trackinfo.sAudioInfo.sFormat.SampleBits = pStreamInfo->AudioFormat.sample_bits;
			trackinfo.sAudioInfo.sFormat.SampleRate = pStreamInfo->AudioFormat.sample_rate;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREVIDEO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREAUDIO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			uCodec = pStreamInfo->nVideoCodecType;
			uExtraSize = pStreamInfo->nVideoExtraSize;
			pExtraData = pStreamInfo->pVideoExtraData;
	
			trackinfo.sVideoInfo.sFormat.Height = pStreamInfo->VideoFormat.height;
			trackinfo.sVideoInfo.sFormat.Width = pStreamInfo->VideoFormat.width;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			uCodec = pStreamInfo->nSubTitleCodecType;
			uExtraSize = pStreamInfo->nSubTitleExtraSize;
			pExtraData = pStreamInfo->pSubTitleExtraData;

			strcpy( trackinfo.sSubtitleInfo.chLanguage, pStreamInfo->SubTitleFormat.strLanguage );
		}
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}

	trackinfo.uCodec = uCodec;
	if( trackinfo.pHeadData )
	{
		delete []trackinfo.pHeadData;
		trackinfo.pHeadData = 0;
	}
	if( !trackinfo.pHeadData )
	{
		trackinfo.pHeadData = new VO_BYTE[uExtraSize];
	}
	memcpy( trackinfo.pHeadData , pExtraData , uExtraSize );
	trackinfo.uHeadSize = uExtraSize;
#ifdef _cc_info
        if(trackinfo.pVideoClosedCaptionDescData)
       	{
       		delete []trackinfo.pVideoClosedCaptionDescData;
		trackinfo.pVideoClosedCaptionDescData = NULL;
        }
	if(!trackinfo.pVideoClosedCaptionDescData && (pStreamInfo->nVideoClosedCaptionDescDataLen > 0) && pStreamInfo->pVideoClosedCaptionDescData)
	{
		trackinfo.pVideoClosedCaptionDescData = new VO_BYTE[pStreamInfo->nVideoClosedCaptionDescDataLen];
		memcpy(trackinfo.pVideoClosedCaptionDescData, pStreamInfo->pVideoClosedCaptionDescData, pStreamInfo->nVideoClosedCaptionDescDataLen);
		trackinfo.nVideoClosedCaptionDescDataLen = pStreamInfo->nVideoClosedCaptionDescDataLen;
	}
#endif
	trackinfo.uTrackType = type;
	trackinfo.uASTrackID = m_uASTrackID;
#ifdef _new_programinfo
	trackinfo.uFPTrackID = uStreamID;
#endif
	StoreTrackInfo(type, &trackinfo );

	if( trackinfo.pHeadData )
	{
		delete []trackinfo.pHeadData;
		trackinfo.pHeadData = NULL;
	}
#ifdef _cc_info
	if(trackinfo.pVideoClosedCaptionDescData)
	{
		delete []trackinfo.pVideoClosedCaptionDescData;
		trackinfo.pVideoClosedCaptionDescData = NULL;
	}
#endif

	return VO_RET_SOURCE2_OK;
}
#else

VO_U32 voAdaptiveStreamingFileParser_ts::CreateMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_PARSER_STREAMINFO * pStreamInfo,VO_U32 uStreamID )
{
	_TRACK_INFO *ptr_info = 0;

	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREAUDIO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREVIDEO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_info = &m_audio_trackinfo;

			ptr_info->uCodec = pStreamInfo->nAudioCodecType;

			if( ptr_info->pHeadData && m_audio_headdata_size < pStreamInfo->nAudioExtraSize )
			{
				delete []ptr_info->pHeadData;
				ptr_info->pHeadData = 0;
				m_audio_headdata_size = 0;
			}

			if( !ptr_info->pHeadData )
			{
				ptr_info->pHeadData = new VO_BYTE[pStreamInfo->nAudioExtraSize];
				m_audio_headdata_size = pStreamInfo->nAudioExtraSize;
			}

			memcpy( ptr_info->pHeadData , pStreamInfo->pAudioExtraData , pStreamInfo->nAudioExtraSize );
			ptr_info->uHeadSize = pStreamInfo->nAudioExtraSize;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREVIDEO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREAUDIO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_info = &m_video_trackinfo;

			ptr_info->uCodec = pStreamInfo->nVideoCodecType;

			if( ptr_info->pHeadData && m_video_headdata_size < pStreamInfo->nVideoExtraSize )
			{
				delete []ptr_info->pHeadData;
				ptr_info->pHeadData = 0;
				m_video_headdata_size = 0;
			}

			if( !ptr_info->pHeadData )
			{
				ptr_info->pHeadData = new VO_BYTE[pStreamInfo->nVideoExtraSize];
				m_video_headdata_size = pStreamInfo->nVideoExtraSize;
			}

			memcpy( ptr_info->pHeadData , pStreamInfo->pVideoExtraData , pStreamInfo->nVideoExtraSize );
			ptr_info->uHeadSize = pStreamInfo->nVideoExtraSize;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ptr_info = &m_subtitle_trackinfo;

			ptr_info->uCodec = pStreamInfo->nSubTitleCodecType;

			if( ptr_info->pHeadData && m_subtitle_headdata_size < pStreamInfo->nSubTitleExtraSize )
			{
				delete []ptr_info->pHeadData;
				ptr_info->pHeadData = 0;
				m_subtitle_headdata_size = 0;
			}

			if( !ptr_info->pHeadData )
			{
				ptr_info->pHeadData = new VO_BYTE[pStreamInfo->nSubTitleExtraSize];
				m_subtitle_headdata_size = pStreamInfo->nSubTitleExtraSize;
			}

			memcpy( ptr_info->pHeadData , pStreamInfo->pSubTitleExtraData , pStreamInfo->nSubTitleExtraSize );
			ptr_info->uHeadSize = pStreamInfo->nSubTitleExtraSize;
		}
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}

	ptr_info->uTrackType = type;
#ifdef _new_programinfo
	ptr_info->uFPTrackID = uStreamID;
#endif
	return VO_RET_SOURCE2_OK;
}
#endif

#if 1
VO_U32 voAdaptiveStreamingFileParser_ts::SendMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_U64 timestamp, VO_U32 uStreamID, VO_U32 uFlag, VO_U32 uMarkOP )
{
	_TRACK_INFO *ptr_info = 0;

	VO_BOOL *pBTSRollback = NULL;

	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			pBTSRollback = &m_bTsRollback_audio;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			pBTSRollback = &m_bTsRollback_video;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			pBTSRollback = &m_bTsRollback_subtitle;
		}
		break;
	case VO_SOURCE2_TT_MAX:
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}
	
	uFlag = 0;
	if( pBTSRollback && *pBTSRollback  )
	{
		uFlag |= VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET ; 
		*pBTSRollback = VO_FALSE;
	}
	return voAdaptiveStreamingFileParser::SendMediaInfo( type, timestamp, uStreamID, uFlag, uMarkOP );
}
#else

VO_U32 voAdaptiveStreamingFileParser_ts::SendMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_U64 timestamp, VO_U32 uStreamID )
{
	_TRACK_INFO *ptr_info = 0;

	VO_BOOL *bTSRollback = NULL;
	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			ptr_info = &m_audio_trackinfo;
			bTSRollback = &m_bTsRollback_audio;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			ptr_info = &m_video_trackinfo;
			bTSRollback = &m_bTsRollback_video;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ptr_info = &m_subtitle_trackinfo;
			bTSRollback = &m_bTsRollback_subtitle;
		}
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}

	_SAMPLE sample;
	memset( &sample , 0 , sizeof( _SAMPLE ) );


	sample.uFlag = sample.uFlag | VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT; 
	if( bTSRollback && *bTSRollback  )
	{
		sample.uFlag |= VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET ; 
		*bTSRollback = VO_FALSE;
	}
	sample.pFlagData = ptr_info;
	sample.uTime = timestamp;
#ifdef _new_programinfo
	sample.uTrackID = uStreamID;
#endif
	return m_pSampleCallback->SendData( m_pSampleCallback->pUserData , type , &sample );
}
#endif

VO_SOURCE2_MEDIATYPE voAdaptiveStreamingFileParser_ts::GetMeidaType()
{
	return (VO_SOURCE2_MEDIATYPE)m_mediatype;
}

VO_VOID voAdaptiveStreamingFileParser_ts::SetFormatChange()
{
	m_isFormatChange = VO_TRUE; 
	m_mediatype = -1;
	voAdaptiveStreamingFileParser::SetFormatChange();
}

VO_VOID voAdaptiveStreamingFileParser_ts::OnChunkEnd()
{
	if(!m_isSampleCallbacked)
	{
		VOLOGI("when chunk end, still no sample output, VO_PID_DO_TS_FORCE_PLAY here");

		m_Api.SetParam(m_hModule, VO_PID_DO_TS_FORCE_PLAY, 0);
	}
}
