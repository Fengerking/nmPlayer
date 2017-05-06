
#include "voAdaptiveStreamingFileParser.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define  GetGroup( type, pInfoGrop ) \
{\
	switch( type )\
	{\
	case VO_SOURCE2_TT_AUDIO:\
		{\
			pInfoGrop = &m_audioTrackG;\
		}\
		break;\
	case VO_SOURCE2_TT_VIDEO:\
		{\
			pInfoGrop = &m_videoTrackG;\
		}\
		break;\
	case VO_SOURCE2_TT_SUBTITLE:\
		{\
			pInfoGrop = &m_subtitleTrackG;\
		}\
		break;\
	case VO_SOURCE2_TT_MAX:\
		break;\
	default:\
		return VO_RET_SOURCE2_OK;\
	}\
}

voAdaptiveStreamingFileParser::voAdaptiveStreamingFileParser( VO_SOURCE2_SAMPLECALLBACK * pCallback )
:m_pSampleCallback(pCallback)
,m_hModule(0)
,m_offset(0)
,m_audio_headdata_size(0)
,m_video_headdata_size(0)
,m_subtitle_headdata_size(0)
,m_bShallSendTrackInfoList(VO_TRUE)
,m_new_audio(VO_TRUE)
,m_new_video(VO_TRUE)
,m_new_subtitle(VO_TRUE)
,m_bTrackReset(VO_FALSE)
,m_uSampleMarkFlag(0)
,m_nLastSubtitleTimestamp(0)
,m_nLastAudioTimestamp(0)
,m_nLastVideoTimestamp(0)
,m_bIsAudioDrmHeadData(VO_FALSE)
,m_bIsVideoDrmHeadData(VO_FALSE)
,m_pDRM(NULL)
{
	ResetTrackInfo();
}

 voAdaptiveStreamingFileParser::~voAdaptiveStreamingFileParser()
{
	if( m_audio_trackinfo.pHeadData )
		delete []m_audio_trackinfo.pHeadData;

	if( m_video_trackinfo.pHeadData )
		delete []m_video_trackinfo.pHeadData;

	if( m_subtitle_trackinfo.pHeadData )
		delete []m_subtitle_trackinfo.pHeadData;
#ifdef _cc_info
	if(m_video_trackinfo.pVideoClosedCaptionDescData)
		delete []m_video_trackinfo.pVideoClosedCaptionDescData;
#endif
	ResetTrackInfo();
	m_audioTrackG.Reset();
	m_videoTrackG.Reset();
	m_subtitleTrackG.Reset();
	m_timestampRollbackInfo.Reset();

	m_audio_headdata_size = 0;
	m_video_headdata_size = 0;
	m_subtitle_headdata_size = 0;
	
	//SendMediaInfo( VO_SOURCE2_TT_MAX , 0 , 0, 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDEND );

	
}

 void voAdaptiveStreamingFileParser::ParserProcEntry(VO_PARSER_OUTPUT_BUFFER* pData)
 {
	voAdaptiveStreamingFileParser *pParser = (voAdaptiveStreamingFileParser*) pData->pUserData;
	pParser->ParserProc( pData );
 }


 void voAdaptiveStreamingFileParser::ResetTrackInfo()
 {
	 memset( &m_audio_trackinfo , 0 , sizeof( _TRACK_INFO ) );
	 memset( &m_video_trackinfo , 0 , sizeof( _TRACK_INFO ) );
	 memset( &m_subtitle_trackinfo , 0 , sizeof( _TRACK_INFO ) );

	 m_videoTrackG.Reset();
	 m_audioTrackG.Reset();
	 m_subtitleTrackG.Reset();
	 m_timestampRollbackInfo.Reset();
 }

 VO_VOID voAdaptiveStreamingFileParser::SendTrackInfoList(VO_U64 uTimestamp)
 {

	 if( m_bShallSendTrackInfoList )
	 {	 
		 m_uSampleMarkFlag = VO_DATASOURCE_MARKFLAG_ADD;
		 VODS_VOLOGI("+ SendTrackInfoList.");
		 SendMediaInfo( VO_SOURCE2_TT_MAX , 0 , 0, 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDBEGIN );
		 int counts = 0;
		 _TRACK_INFO *pTrackInfo = NULL;
		 pTrackInfo = m_subtitleTrackG.GetNextTrackInfo( pTrackInfo, VO_TRUE );
		 while( pTrackInfo )
		 {
			 counts ++;
			 SendMediaInfo( VO_SOURCE2_TRACK_TYPE (pTrackInfo->uTrackType) , 0 , pTrackInfo->uFPTrackID , 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD );
			 pTrackInfo = m_subtitleTrackG.GetNextTrackInfo( pTrackInfo, VO_TRUE );		
		 }

		 pTrackInfo = m_audioTrackG.GetNextTrackInfo( pTrackInfo,VO_TRUE );
		 while( pTrackInfo )
		 {
			 counts ++;

			 SendMediaInfo( VO_SOURCE2_TRACK_TYPE (pTrackInfo->uTrackType) , 0 , pTrackInfo->uFPTrackID , 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD );
			 pTrackInfo = m_audioTrackG.GetNextTrackInfo( pTrackInfo,VO_TRUE );
		 }

		 pTrackInfo = m_videoTrackG.GetNextTrackInfo( pTrackInfo,VO_TRUE );
		 while( pTrackInfo )
		 {
			 counts ++;

			 SendMediaInfo( VO_SOURCE2_TRACK_TYPE (pTrackInfo->uTrackType) , 0 , pTrackInfo->uFPTrackID , 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD );
			 pTrackInfo = m_videoTrackG.GetNextTrackInfo( pTrackInfo,VO_TRUE );
		 }

		 SendMediaInfo( VO_SOURCE2_TT_MAX , 0 , 0, 0, VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDEND );
		 
		 VODS_VOLOGI("- SendTrackInfoList. Counts:%d", counts);
	 }
	 m_bShallSendTrackInfoList = VO_FALSE;
 }

VO_VOID voAdaptiveStreamingFileParser::MarkSubtitleFlag(_SAMPLE *pSample, VO_U32 uReserve, VO_BOOL bMark, VO_U32 uValue)
{
	if(uReserve == 1 && bMark)
	{
		int type = uValue;
		pSample->pReserve1 = (VO_VOID*)type;
	}
	if(uReserve == 2 && bMark)
	{
		int type = uValue;
		pSample->pReserve2 = (VO_VOID*)type;
	}
}

VO_U32 voAdaptiveStreamingFileParser::SendMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_U64 timestamp, VO_U32 uStreamID, VO_U32 uFlag, VO_U32 uMarkOP )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	voTrackInfoGroup *pInfoGrop = NULL;

	GetGroup( type, pInfoGrop );

	_SAMPLE sample;
	memset( &sample , 0 , sizeof( _SAMPLE ) );

	sample.uMarkOP = uMarkOP;
	sample.uMarkFlag |= m_uSampleMarkFlag;
	sample.uFlag |= uFlag;
	sample.uFlag |= VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
	if(type == VO_SOURCE2_TT_AUDIO && m_bIsAudioDrmHeadData)
	{
		//if((sample.uMarkFlag & VO_DATASOURCE_MARKFLAG_ADD) && (sample.uMarkOP & VO_DATASOURCE_MARKOP_BASE))
			sample.uFlag |= VO_SOURCE2_FLAG_SAMPLE_DRMHEADER;
	}
	else if(type == VO_SOURCE2_TT_VIDEO && m_bIsVideoDrmHeadData)
	{
		//if((sample.uMarkFlag & VO_DATASOURCE_MARKFLAG_ADD) && (sample.uMarkOP & VO_DATASOURCE_MARKOP_BASE))
			sample.uFlag |= VO_SOURCE2_FLAG_SAMPLE_DRMHEADER;
	}
#if 1
	if( type == VO_SOURCE2_TT_SUBTITLE && ( m_ff == FileFormat_TS || m_ff == FileFormat_SMTH))
	{
		MarkSubtitleFlag(&sample, 2, VO_TRUE, __INTERNALSUBTITLE);
	}
#else
	VO_U32 type2 = 0;
	if( type == VO_SOURCE2_TT_SUBTITLE && ( m_ff == FileFormat_TS || m_ff == FileFormat_SMTH))
	{
		VOLOGI("__INTERNALSUBTITLE");
		type2 = __INTERNALSUBTITLE;
		sample.pReserve2 = (VO_VOID*)type2;
	}
#endif
	if( pInfoGrop )
	{
		_TRACK_INFO *pTrackInfo = pInfoGrop->GetTrackInfoByFPTrackID(uStreamID);
		if( pTrackInfo )
		{
			VOLOGI("ASTrackID:%d, FPTrackID:%d, UUID:%d", pTrackInfo->uASTrackID, pTrackInfo->uFPTrackID, pTrackInfo->uOutSideTrackID);
			sample.pFlagData = pTrackInfo;
		}
	}
	sample.uTime = timestamp;

	sample.uFPTrackID = uStreamID;
	sample.uASTrackID = m_uASTrackID;
	VODS_VOLOGI("ASTrackID:%d, FPTrackID:%d, Flag:%d, MarkOP:%d,MarkFlag:%d"
		, sample.uASTrackID, sample.uFPTrackID, sample.uFlag, sample.uMarkOP, sample.uMarkFlag );
	ret = m_pSampleCallback->SendData( m_pSampleCallback->pUserData , type , &sample );

#if 1
	if( uMarkOP == VO_DATASOURCE_MARKOP_BASE && pInfoGrop && pInfoGrop->GetNewByFPTrackID( uStreamID ) == 1 )
	{
		pInfoGrop->SetNewByFPTrackID( uStreamID, 0 );
	}
#else
	if( pInfoGrop && pInfoGrop->GetNewByFPTrackID( uStreamID ) == 1 )
	{
		pInfoGrop->SetNewByFPTrackID( uStreamID, 0 );
	}
#endif

	return ret;
}



VO_U32 voAdaptiveStreamingFileParser::StoreTrackInfo( VO_SOURCE2_TRACK_TYPE type, _TRACK_INFO *pTrackInfo )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	voTrackInfoGroup *pInfoGrop = NULL;
	GetGroup( type, pInfoGrop );
	
	if( pInfoGrop && !pInfoGrop->GetTrackInfoByFPTrackID(pTrackInfo->uFPTrackID) )
	{
		pInfoGrop->AddTrackInfo( pTrackInfo );
	}
	else
		ret = VO_RET_SOURCE2_FAIL;

	m_timestampRollbackInfo.AddRollbackInfo( pTrackInfo->uFPTrackID );

	return ret;
}	

VO_VOID voAdaptiveStreamingFileParser::SetFormatChange()
{	
	m_audioTrackG.Reset();
	m_subtitleTrackG.Reset();
	m_videoTrackG.Reset();
	m_bShallSendTrackInfoList = VO_TRUE; 
	m_new_audio = m_new_video = m_new_subtitle = VO_TRUE; 
	m_uSampleMarkFlag = VO_DATASOURCE_MARKFLAG_ADD;
}


VO_VOID voAdaptiveStreamingFileParser::TryToSendTrackInfo(VO_SOURCE2_TRACK_TYPE uType, VO_U64 uTimeStamp, VO_U32 uTrackID )
{

#if 0
	VO_BOOL *pNew = NULL;
	switch(uType)
	{
	case VO_SOURCE2_TT_SUBTITLE:
		pNew = &m_new_subtitle;
		break;
	case VO_SOURCE2_TT_AUDIO:
		pNew = &m_new_audio;
		break;
	case VO_SOURCE2_TT_VIDEO:
		pNew = &m_new_video;
		break;
	default: 
		return;
	}
#else
	VO_BOOL *pNew = NULL;
	switch(uType)
	{
	case VO_SOURCE2_TT_SUBTITLE:
		m_new_subtitle = (VO_BOOL) m_subtitleTrackG.GetNewByFPTrackID(uTrackID);
		pNew = &m_new_subtitle;
		break;
	case VO_SOURCE2_TT_AUDIO:
		m_new_audio = (VO_BOOL) m_audioTrackG.GetNewByFPTrackID(uTrackID);
		pNew = &m_new_audio;
		break;
	case VO_SOURCE2_TT_VIDEO:
		m_new_video = (VO_BOOL) m_videoTrackG.GetNewByFPTrackID(uTrackID);
		pNew = &m_new_video;
		break;
	default: 
		return;
	}
#endif
	if( pNew && !( *pNew ) )
	{
		return;
	}

	VOLOGI("TrackReset:%d. Type:%s", m_bTrackReset, TrackType2String(uType));
	VO_U64 uTS = uTimeStamp > 0? uTimeStamp -1 :0;
	if( m_bTrackReset )
	{
		m_uSampleMarkFlag = VO_DATASOURCE_MARKFLAG_RESET;
		SendMediaInfo( uType , uTS, -1,0, 0 );
		m_bTrackReset = VO_FALSE;
	}
	m_uSampleMarkFlag = VO_DATASOURCE_MARKFLAG_ADD;

	SendTrackInfoList(uTimeStamp);
	m_uSampleMarkFlag = 0;
	SendMediaInfo( uType , uTS , uTrackID, 0,0);

	*pNew = VO_FALSE;
}

VO_VOID voAdaptiveStreamingFileParser::SetASTrackID( VO_U32 uASTrackID )
{ 
	m_uASTrackID = uASTrackID; 
	VODS_VOLOGI("SetASTrackID:%d", m_uASTrackID );
}
VO_VOID voAdaptiveStreamingFileParser::SetFPTrackID( VO_U32 uFPTrackID )
{ 
	m_uFPTrackID = uFPTrackID; 
	VODS_VOLOGI("SetFPTrackID:%d", m_uFPTrackID );
}
VO_BOOL voAdaptiveStreamingFileParser::CheckRollback(VO_U32 uFPTrackID, VO_U64 uTimestamp )
{
	return m_timestampRollbackInfo.CheckRollback(uFPTrackID, uTimestamp );
}

VO_VOID voAdaptiveStreamingFileParser::SetDrmHeadData(VO_SOURCE2_TRACK_TYPE type, VO_BOOL IsDrmHeadData)
{
	switch(type)
	{
		case VO_SOURCE2_TT_AUDIO:
			m_bIsAudioDrmHeadData = IsDrmHeadData;
			break;
		case VO_SOURCE2_TT_VIDEO:
			m_bIsVideoDrmHeadData = IsDrmHeadData;
			break;
		default:
			break;
	}
}