#include "voAdaptiveStreamingFileParser_smth.h"
#include "fAudioHeadDataInfo.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingFileParser_smth"
#endif



voAdaptiveStreamingFileParser_smth::voAdaptiveStreamingFileParser_smth( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser_mp4( pCallback,pLibOp )
{
	vostrcpy( m_szDllFile , _T("voSmthParser") );
	vostrcpy( m_szAPIName , _T("voGetSMTHParserAPI") );
}

voAdaptiveStreamingFileParser_smth::~voAdaptiveStreamingFileParser_smth()
{
}

VO_U32 voAdaptiveStreamingFileParser_smth::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	m_SubtitleTimescale = m_VideoTimescale = m_AudioTimescale = (VO_U32)pBuffer->ullTimeScale;
	return voAdaptiveStreamingFileParser_mp4::Process(pBuffer);
}

void voAdaptiveStreamingFileParser_smth::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	voAdaptiveStreamingFileParser_smth * ptr_obj = ( voAdaptiveStreamingFileParser_smth * )pData->pUserData;
	VO_SOURCE2_TRACK_TYPE type = VO_SOURCE2_TT_MAX;
	voTrackInfoGroup *pInfoGroup = NULL;

	switch( pData->nType )
	{
	case VO_PARSER_OT_VIDEO:
		type = VO_SOURCE2_TT_VIDEO;
		pInfoGroup = &m_videoTrackG;
		break;
	case VO_PARSER_OT_AUDIO:
		type = VO_SOURCE2_TT_AUDIO;
		pInfoGroup = &m_audioTrackG;
		break;
	case VO_PARSER_OT_TEXT:
		type = VO_SOURCE2_TT_SUBTITLE;
		pInfoGroup = &m_subtitleTrackG;
		break;
	}

	if(pInfoGroup)
	{
		_TRACK_INFO *pTrackInfo = NULL;
		pTrackInfo = pInfoGroup->GetNextTrackInfo(pTrackInfo);
		if(pTrackInfo)
			pTrackInfo->uFPTrackID = pData->nStreamID;
	}

	ptr_obj->SendSample( type , ( VO_SOURCE_SAMPLE * )pData->pOutputData, pData->nStreamID );
}


VO_U32 voAdaptiveStreamingFileParser_smth::CreateTrackInfo2( VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pTrackInfo )
{
	_TRACK_INFO *ptr_info = 0;
	VO_U32 *pHeadSize = 0;
	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREAUDIO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREVIDEO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_info = &m_audio_trackinfo;
			pHeadSize = &m_audio_headdata_size;

			ptr_info->uTrackType = VO_SOURCE2_TT_AUDIO;

			ptr_info->sAudioInfo.sFormat.Channels = pTrackInfo->sAudioInfo.sFormat.Channels;
			ptr_info->sAudioInfo.sFormat.SampleBits = pTrackInfo->sAudioInfo.sFormat.SampleBits;
			ptr_info->sAudioInfo.sFormat.SampleRate = pTrackInfo->sAudioInfo.sFormat.SampleRate;

			m_new_audio = VO_TRUE;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			if( m_mediatype == -1 )
				m_mediatype = VO_SOURCE2_MT_PUREVIDEO;
			else if( m_mediatype == VO_SOURCE2_MT_PUREAUDIO )
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;

			ptr_info = &m_video_trackinfo;
			pHeadSize = &m_video_headdata_size;
			m_new_video = VO_TRUE;

		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ptr_info = &m_subtitle_trackinfo;
			pHeadSize = &m_subtitle_headdata_size;
			m_new_subtitle = VO_TRUE;
		}
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}

	ptr_info->uCodec = pTrackInfo->uCodec;
	if( ptr_info->pHeadData && *pHeadSize < pTrackInfo->uHeadSize )
	{
		delete []ptr_info->pHeadData;
		ptr_info->pHeadData = 0;
		*pHeadSize = 0;
	}
	if( !ptr_info->pHeadData )
	{
		ptr_info->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize];
		*pHeadSize = pTrackInfo->uHeadSize;
	}
	memcpy( ptr_info->pHeadData , pTrackInfo->pHeadData , pTrackInfo->uHeadSize );
	ptr_info->uHeadSize = pTrackInfo->uHeadSize;
	
	memcpy( ptr_info->strFourCC, pTrackInfo->strFourCC, sizeof( pTrackInfo->strFourCC) );

	ptr_info->uTrackType = type;
#ifdef _new_programinfo
	ptr_info->uASTrackID = m_uASTrackID;
	ptr_info->uFPTrackID = pTrackInfo->uFPTrackID;
#endif

	//StoreTrackInfo( type, ptr_info );
	return VO_RET_SOURCE2_OK;
}