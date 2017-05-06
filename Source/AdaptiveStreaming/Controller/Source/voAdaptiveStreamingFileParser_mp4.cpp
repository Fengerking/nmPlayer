#include "voAdaptiveStreamingFileParser_mp4.h"
#include "fAudioHeadDataInfo.h"
#include "voLog.h"
#include "voSubtitleParser.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingFileParser_mp4"
#endif

typedef VO_S32 ( VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);

#define ADIFHEADER_CHECK(h)	 ((h)[0] == 'A' && (h)[1] == 'D' && (h)[2] == 'I' && (h)[3] == 'F')


voAdaptiveStreamingFileParser_mp4::voAdaptiveStreamingFileParser_mp4( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser( pCallback )
,m_mediatype( -1 )
,m_ptr_convertaudiobuffer(0)
,m_convertaudiosize(0)
,m_ptrCurBuffer(0)
,m_VideoTimescale(1)
,m_AudioTimescale(1)
,m_SubtitleTimescale(1)
{
	VOLOGR("Load mp4");
	memset( &m_Api , 0 , sizeof( VO_PARSER_API ) );

	if( pLibOp )
		SetLibOperator( ( VO_LIB_OPERATOR* )pLibOp );

	vostrcpy( m_szDllFile , _T("voMP4FR") );
	vostrcpy( m_szAPIName , _T("voGetMp4StreamReaderAPI") );
}

voAdaptiveStreamingFileParser_mp4::~voAdaptiveStreamingFileParser_mp4()
{
	FreeParser();

	if( m_ptr_convertaudiobuffer )
		delete []m_ptr_convertaudiobuffer;
	m_ptr_convertaudiobuffer = NULL;

}

VO_U32 voAdaptiveStreamingFileParser_mp4::LoadParser()
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

VO_U32 voAdaptiveStreamingFileParser_mp4::FreeParser()
{
	if( m_Api.Open && m_hModule )
	{
		VOLOGI("Free Parser");

		m_Api.Close( m_hModule );
		m_hModule = 0;
	}

	return VO_RET_SOURCE2_OK;
}


VO_U32 voAdaptiveStreamingFileParser_mp4::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	VO_PARSER_INPUT_BUFFER input;
	memset( &input , 0 , sizeof( VO_PARSER_INPUT_BUFFER ) );

	if( pBuffer->Type == VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA )
	{
		input.nFlag = VO_PARSER_FLAG_STREAM_DASHHEAD;

		m_new_audio = m_new_video = m_new_subtitle = VO_TRUE;
		m_mediatype = -1;
	}
	else if(pBuffer->Type == VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX)
	{
		input.nFlag = VO_PARSER_FLAG_STREAM_DASHINDEX;
	}
	else if(pBuffer->Type == VO_SOURCE2_ADAPTIVESTREAMING_INITDATA)
	{
		input.nFlag = VO_PARSER_FLAG_STREAM_DASHINITDATA;
	}
	else
		input.nFlag = VO_PARSER_FLAG_STREAM_DASHDATA;

	input.nBufLen = pBuffer->uSize;

	switch( pBuffer->Type )
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		input.nStreamID = 1;
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		input.nStreamID = 0;
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		input.nStreamID = 2;
		break;
	}

	input.pBuf = pBuffer->pBuffer;
	VO_S64 starttime = pBuffer->ullStartTime;
	input.pReserved = &starttime;

	m_ptrCurBuffer = pBuffer;

	return m_Api.Process( m_hModule , &input );
}

// VO_VOID voAdaptiveStreamingFileParser_mp4::SetFormatChange()
// {
// 	;
// }

VO_SOURCE2_MEDIATYPE voAdaptiveStreamingFileParser_mp4::GetMeidaType()
{
	return ( VO_SOURCE2_MEDIATYPE )m_mediatype;
}

void voAdaptiveStreamingFileParser_mp4::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	voAdaptiveStreamingFileParser_mp4 * ptr_obj = ( voAdaptiveStreamingFileParser_mp4 * )pData->pUserData;
	VO_SOURCE2_TRACK_TYPE type = VO_SOURCE2_TT_MAX;
	VO_U32 uStreamID = pData->nStreamID;
	switch( pData->nType )
	{
	case VO_PARSER_OT_TRACKINFO:
		{
			if( ptr_obj->m_ptrCurBuffer->ullTimeScale == 1 )
			{
				VO_U32 * ptr = (VO_U32*)pData->pReserved;
				if((( VO_LIVESRC_TRACK_INFOEX * )pData->pOutputData)->Type == VO_SOURCE_TT_AUDIO)
				{
					if( ptr )
					{
						ptr_obj->m_AudioTimescale = *ptr;
					}
				}
				else if((( VO_LIVESRC_TRACK_INFOEX * )pData->pOutputData)->Type == VO_SOURCE_TT_VIDEO)
				{
					if( ptr )
					{
						ptr_obj->m_VideoTimescale = *ptr;
					}
				}
				else if((( VO_LIVESRC_TRACK_INFOEX * )pData->pOutputData)->Type == VO_SOURCE_TT_SUBTITLE)
				{
					if( ptr )
					{
						ptr_obj->m_SubtitleTimescale = *ptr;
					}
				}
				
			}
			VO_LIVESRC_TRACK_INFOEX *pTrackinfo = ( VO_LIVESRC_TRACK_INFOEX * )pData->pOutputData;
			VOLOGR("TrackID:%d HeadSize:%d,HeadData:%p ",uStreamID, pTrackinfo->HeadSize,pTrackinfo->HeadData );

			ptr_obj->CreateTrackInfo( ( VO_LIVESRC_TRACK_INFOEX * )pData->pOutputData, uStreamID );
			/*let thread check this formatchange flag*/
			ptr_obj->m_pSampleCallback->SendData( m_pSampleCallback->pUserData , VO_SOURCE2_TT_TRACKINFO , NULL );

		}
		return;
	case VO_PARSER_OT_VIDEO:
		type = VO_SOURCE2_TT_VIDEO;
		break;
	case VO_PARSER_OT_AUDIO:
		type = VO_SOURCE2_TT_AUDIO;
		break;
	case VO_PARSER_OT_TEXT:
		type = VO_SOURCE2_TT_SUBTITLE;
		break;
	case VO_PARSER_OT_SEGMENTINDEX_INFO:
		type = VO_SOURCE2_TT_SEGMENTINDEX;
		m_pSampleCallback->SendData( m_pSampleCallback->pUserData , type , pData->pOutputData);
		return;
	case VO_PARSER_OT_INITDATA_INFO:
		type = VO_SOURCE2_TT_INITDATA;
		m_pSampleCallback->SendData(m_pSampleCallback->pUserData, type, pData->pOutputData);
		return;
	}

	ptr_obj->SendSample( type , ( VO_SOURCE_SAMPLE * )pData->pOutputData, uStreamID );
}

VO_VOID voAdaptiveStreamingFileParser_mp4::CreateTrackInfo( VO_LIVESRC_TRACK_INFOEX * ptr_info, VO_U32 uSteamID )
{
	VODS_VOLOGI("%d new TrackInof.", ptr_info->Type );
	_TRACK_INFO * ptr_trackinfo = 0;
	switch( ptr_info->Type )
	{
	case VO_SOURCE_TT_VIDEO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREVIDEO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREAUDIO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_trackinfo = &m_video_trackinfo;
			ptr_trackinfo->uTrackType = VO_SOURCE2_TT_VIDEO;
			memcpy( ptr_trackinfo->strFourCC , "AVC1" , 4 );		
		}
		break;
	case VO_SOURCE_TT_AUDIO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREAUDIO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREVIDEO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_trackinfo = &m_audio_trackinfo;
			ptr_trackinfo->uTrackType = VO_SOURCE2_TT_AUDIO;
			ptr_trackinfo->sAudioInfo.sFormat.Channels = ptr_info->audio_info.Channels;
			ptr_trackinfo->sAudioInfo.sFormat.SampleBits = ptr_info->audio_info.SampleBits;
			ptr_trackinfo->sAudioInfo.sFormat.SampleRate = ptr_info->audio_info.SampleRate;

		}
		break;
	case VO_SOURCE_TT_SUBTITLE:
		{
			m_mediatype == VO_SOURCE2_MT_SUBTITLE;
			ptr_trackinfo = &m_subtitle_trackinfo;
			ptr_trackinfo->uTrackType = VO_SOURCE2_TT_SUBTITLE;
			memcpy(ptr_trackinfo->strFourCC,"TTML",4);
		}
		break;
	}

	if( !ptr_trackinfo )
		return;

	ptr_trackinfo->uCodec = ptr_info->Codec;

	if( ptr_trackinfo->pHeadData )
		delete []ptr_trackinfo->pHeadData;

	ptr_trackinfo->pHeadData = 0;

	if( ptr_info->HeadSize > 0 )
	{
		ptr_trackinfo->uHeadSize = ptr_info->HeadSize;
		ptr_trackinfo->pHeadData = new VO_BYTE[ ptr_info->HeadSize ];
		memcpy( ptr_trackinfo->pHeadData , ptr_info->HeadData , ptr_info->HeadSize );
	}
#ifdef _new_programinfo
	ptr_trackinfo->uASTrackID = m_uASTrackID;
	ptr_trackinfo->uFPTrackID = uSteamID;
#endif
	SetDrmHeadData(VO_SOURCE2_TRACK_TYPE(ptr_trackinfo->uTrackType), VO_FALSE);
	if( m_pDRM && m_pDRM->hHandle )
	{
		VO_U32 nDestSize = 0;
		VO_U32 nRet = VO_RET_SOURCE2_OK;
		nRet = m_pDRM->DataProcess_FR( m_pDRM->hHandle, NULL, ptr_trackinfo->uHeadSize, NULL, &nDestSize, VO_DRM2DATATYPE_SEQUENCEHEADER,VO_DRM2_DATAINFO_UNKNOWN,NULL );
		if(nRet == VO_ERR_DRM2_OUTPUT_BUF_SMALL)
		{
			VO_PBYTE pNewHeadData =  new VO_BYTE[nDestSize];
			VO_PBYTE pNewOutHeadData = NULL;
			memset(pNewHeadData,0,nDestSize);
			if(ptr_trackinfo->pHeadData)
			{
				memcpy(pNewHeadData,ptr_trackinfo->pHeadData,ptr_trackinfo->uHeadSize);
				delete []ptr_trackinfo->pHeadData;
				ptr_trackinfo->pHeadData = NULL;
				nRet = m_pDRM->DataProcess_FR( m_pDRM->hHandle , pNewHeadData,ptr_trackinfo->uHeadSize,  &pNewOutHeadData, &nDestSize, VO_DRM2DATATYPE_SEQUENCEHEADER,VO_DRM2_DATAINFO_UNKNOWN,NULL );
				if(nRet == VO_RET_SOURCE2_OK)
				{
					ptr_trackinfo->uHeadSize = nDestSize;
					ptr_trackinfo->pHeadData = pNewOutHeadData;
					SetDrmHeadData(VO_SOURCE2_TRACK_TYPE(ptr_trackinfo->uTrackType), VO_TRUE);	
				}
			}
		}
	}
	StoreTrackInfo( VO_SOURCE2_TRACK_TYPE(ptr_trackinfo->uTrackType), ptr_trackinfo );

	if( ptr_trackinfo->pHeadData )
	{
		delete []ptr_trackinfo->pHeadData;
		ptr_trackinfo->pHeadData = NULL;
	}
	VOLOGR("headSize:%d,pHeadData:%p, HeadSize:%d,HeadData:%p ",ptr_trackinfo->uHeadSize, ptr_trackinfo->pHeadData, ptr_info->HeadSize,ptr_info->HeadData );

}

VO_VOID voAdaptiveStreamingFileParser_mp4::SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_SOURCE_SAMPLE * ptr_sample, VO_U32 uStreamID )
{	
	if(nOutputType == VO_SOURCE2_TT_AUDIO)
	{
		ptr_sample->Time = ptr_sample->Time / ( m_AudioTimescale / 1000. ) ;
	}
	else if(nOutputType == VO_SOURCE2_TT_VIDEO)
	{
		ptr_sample->Time = ptr_sample->Time / ( m_VideoTimescale / 1000. ) ;
	}
	else if(nOutputType == VO_SOURCE2_TT_SUBTITLE)
	{
		ptr_sample->Time = ptr_sample->Time / ( m_SubtitleTimescale / 1000. ) ;
	}
	VOLOGR("Type:%d, Time:%lld, Size:0x%08x", nOutputType, ptr_sample->Time, ptr_sample->Size );
	
	VO_BOOL bSample = VO_FALSE;
	VO_BOOL *pNew = NULL;
	VO_U32 uFlag = 0;
	switch( nOutputType )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			pNew = & m_new_audio;
			bSample = VO_TRUE;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			pNew = & m_new_video;
			bSample = VO_TRUE;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{	
			
			pNew = & m_new_subtitle;
			bSample = VO_TRUE;
		}
		break;
		
	}

	TryToSendTrackInfo( nOutputType, ptr_sample->Time, uStreamID );
// 	if( bSample && pNew && *pNew )
// 	{
// 		SendTrackInfoList(ptr_sample->Time > 0 ? ptr_sample->Time - 2 : 0);
// 		SendMediaInfo( nOutputType, ptr_sample->Time > 0 ? ptr_sample->Time - 1 : 0, uStreamID, uFlag, 0 );
// 		*pNew = VO_FALSE;
// 	}


	if( nOutputType == VO_SOURCE2_TT_VIDEO || nOutputType == VO_SOURCE2_TT_AUDIO || nOutputType == VO_SOURCE2_TT_SUBTITLE )
	{
		_SAMPLE sample;
		memset( &sample , 0 , sizeof( _SAMPLE ) );
		sample.pBuffer = ptr_sample->Buffer;
		sample.uTime = ptr_sample->Time;
		sample.uFlag = ( ptr_sample->Size & 0x80000000 ) ? VO_SOURCE2_FLAG_SAMPLE_KEYFRAME : 0;
#ifdef _new_programinfo
		sample.uFPTrackID = uStreamID;
		sample.uASTrackID = m_uASTrackID;
#endif
		sample.uSize = ptr_sample->Size & 0x7fffffff;
		if( nOutputType == VO_SOURCE2_TT_AUDIO && m_audio_trackinfo.uCodec == VO_AUDIO_CodingAAC && !m_bIsAudioDrmHeadData )
			ConvertAudio( &sample );
		else if( nOutputType == VO_SOURCE2_TT_VIDEO && (m_video_trackinfo.uCodec == VO_VIDEO_CodingH264 || m_video_trackinfo.uCodec == VO_VIDEO_CodingH265) && !m_bIsVideoDrmHeadData)
			ConvertVideo( &sample );
		else if( nOutputType == VO_SOURCE2_TT_SUBTITLE && !strcmp( (VO_CHAR *)m_subtitle_trackinfo.strFourCC, "TTML"))
		{	
#if 1
			MarkSubtitleFlag(&sample, 1, VO_TRUE, VO_SUBTITLE_CodingSMPTETT);
			MarkSubtitleFlag(&sample, 2, VO_TRUE, __INTERNALSUBTITLE);
#else
			int type = 0;
			type = VO_SUBTITLE_CodingSMPTETT;//VO_SUBTITLE_CodingSSTTML;
			sample.pReserve1 = (VO_VOID*)type;
			VOLOGI("__INTERNALSUBTITLE");
			int type2 = 0;
			type2 = __INTERNALSUBTITLE;
			sample.pReserve2 = (VO_VOID*)type2;
#endif
			VOLOGI( "Subtitle:TimeStamp[%lld],Size[%d]", sample.uTime, sample.uSize );
		}
		VOLOGR( "SampleData-Type:%d, TimeStamp:%lld, Size:%d", nOutputType, sample.uTime, sample.uSize )
		m_pSampleCallback->SendData( m_pSampleCallback->pUserData , nOutputType , &sample );
	}
}

VO_VOID voAdaptiveStreamingFileParser_mp4::ConvertAudio( _SAMPLE * ptr_sample )
{
	VO_U32 len = ptr_sample->uSize + 7;

	if( len > m_convertaudiosize )
	{
		if( m_ptr_convertaudiobuffer )
			delete []m_ptr_convertaudiobuffer;

		m_ptr_convertaudiobuffer = 0;
		m_convertaudiosize = 0;
	}

	if( !m_ptr_convertaudiobuffer )
	{
		m_ptr_convertaudiobuffer = new VO_BYTE[ len + 1 ];
		m_convertaudiosize = len + 1;
	}


	VO_BOOL bGetAACHeadInfo = VO_FALSE;

	VO_AUDIO_HEADDATAINFO sHeadInfo_HeadData;
	memset(&sHeadInfo_HeadData, 0x00, sizeof(VO_AUDIO_HEADDATAINFO));

	_TRACK_INFO *pTrackInfo = NULL;
	pTrackInfo = m_audioTrackG.GetTrackInfoByFPTrackID( ptr_sample->uFPTrackID);
	if(pTrackInfo->uHeadSize != 0 && pTrackInfo->pHeadData)
	{
		VO_CODECBUFFER codebufIn;
		codebufIn.Buffer = pTrackInfo->pHeadData;
		codebufIn.Length = pTrackInfo->uHeadSize;
		if( 0 == voGetAACHeadInfo(&codebufIn, &sHeadInfo_HeadData))
			bGetAACHeadInfo = VO_TRUE;
	}

	VO_AUDIO_HEADDATAINFO *pInfo = NULL;
	VO_AUDIO_HEADDATAINFO sHeadInfo_TrackInfo;
	memset(&sHeadInfo_TrackInfo, 0x00, sizeof(VO_AUDIO_HEADDATAINFO));
	if( bGetAACHeadInfo )
		pInfo = &sHeadInfo_HeadData;
	else
	{
		sHeadInfo_TrackInfo.nChannels = m_audio_trackinfo.sAudioInfo.sFormat.Channels;
		sHeadInfo_TrackInfo.nSampleBits = m_audio_trackinfo.sAudioInfo.sFormat.SampleBits;
		sHeadInfo_TrackInfo.nSampleRate = m_audio_trackinfo.sAudioInfo.sFormat.SampleRate;
		pInfo = &sHeadInfo_TrackInfo;
	}

	VO_CODECBUFFER codebufIn, codebufOut;
	codebufIn.Buffer = ptr_sample->pBuffer;
	codebufIn.Length = ptr_sample->uSize;
	codebufOut.Buffer = m_ptr_convertaudiobuffer;
	codebufOut.Length = len;
	if( pInfo && voAACADTSHeadPack(&codebufIn,&codebufOut, pInfo ) == 0)
	{
		ptr_sample->pBuffer = codebufOut.Buffer;
		ptr_sample->uSize = codebufOut.Length;
	}

}

VO_VOID voAdaptiveStreamingFileParser_mp4::ConvertVideo( _SAMPLE * ptr_sample )
{

	VO_PBYTE pOutData = ptr_sample->pBuffer;
	VO_PBYTE pInData = ptr_sample->pBuffer;
	VO_U32 nInSize = ptr_sample->uSize;

	if (pOutData == NULL)
		return;

	VO_PBYTE pBuffer = pInData;
	VO_U32	 nFrameLen = 0;
	VO_U32 nNalLen = 4;
	VO_U32 nNalWord = 0X01000000;
	VO_U32 nVideoSize = 0;

	int i = 0;
	while (pBuffer - pInData + nNalLen < nInSize)
	{
		nFrameLen = *pBuffer++;
		for (i = 0; i < (int)nNalLen - 1; i++)
		{
			nFrameLen = nFrameLen << 8;
			nFrameLen += *pBuffer++;
		}

		if(nFrameLen > nInSize)
			return;

		if (nNalLen == 3 || nNalLen == 4)
		{
			memcpy ((pBuffer - nNalLen), &nNalWord, nNalLen);
		}
		else
		{
			memcpy (pOutData + nVideoSize, &nNalWord, 4);
			nVideoSize += 4;
			memcpy (pOutData + nVideoSize, pBuffer, nFrameLen);
			nVideoSize += nFrameLen;
		}

		pBuffer += nFrameLen;
	}

}
