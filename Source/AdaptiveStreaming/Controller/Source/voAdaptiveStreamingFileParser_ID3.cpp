#include "voAdaptiveStreamingFileParser_ID3.h"
#include "voLog.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingFileParser_ID3"
#endif


voAdaptiveStreamingFileParser_ID3::voAdaptiveStreamingFileParser_ID3( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser( pCallback )
,m_mediatype( -1 )
,m_bStop(VO_FALSE)
{
	m_ff = FileFormat_ID3;
}

voAdaptiveStreamingFileParser_ID3::~voAdaptiveStreamingFileParser_ID3()
{
	m_bStop = VO_TRUE;
	ResetSampleList();
	FreeParser();
}

VO_U32 voAdaptiveStreamingFileParser_ID3::LoadParser()
{

	VOLOGR("Load ID3 Parser");


	VO_SOURCE2_SAMPLECALLBACK   sample_callback;
	sample_callback.pUserData = this;
	sample_callback.SendData = OnSample;
	VO_U32 ret = m_ID3Parser.Init(NULL, 0, &sample_callback);

	ResetTrackInfo();
	return ret;
}

VO_U32 voAdaptiveStreamingFileParser_ID3::FreeParser()
{
	m_ID3Parser.Uninit();
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingFileParser_ID3::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{

	NotifyCustomerTag(pBuffer);

//	m_ID3Parser.Reset();
	m_nLastSubtitleTimestamp = pBuffer->ullStartTime;
	VOLOGI("+ ReceiveData. TimeStamp:%lld ", m_nLastSubtitleTimestamp);
	VO_U32 ret = m_ID3Parser.ReceiveData( pBuffer->pBuffer, pBuffer->uSize );

	vo_singlelink_list< _SAMPLE* >::iterator iter = m_SampleList.begin();
	vo_singlelink_list< _SAMPLE* >::iterator itere = m_SampleList.end();
	while( iter != itere )
	{	
		_SAMPLE *pSample = (*iter);
		TryToSendTrackInfo( VO_SOURCE2_TT_SUBTITLE, m_nLastSubtitleTimestamp, pSample->uFPTrackID );
		m_pSampleCallback->SendData( m_pSampleCallback->pUserData , VO_SOURCE2_TT_SUBTITLE ,pSample);
		iter++;
	}
	ResetSampleList();
	VOLOGI("- ReceiveData ");
	return ret;
}

VO_U32 voAdaptiveStreamingFileParser_ID3::NotifyCustomerTag( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{	
	VO_SOURCE2_CUSTOMERTAG_TIMEDTAG tag;
	memset(&tag, 0x00, sizeof(VO_SOURCE2_CUSTOMERTAG_TIMEDTAG));
	tag.ullTimeStamp = pBuffer->ullStartTime;
	tag.pData = pBuffer->pBuffer;
	tag.uSize = pBuffer->uSize;
	tag.pReserved = NULL;
	tag.uFlag = VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_ID3TAG;
	
	if( m_pSampleCallback && m_pSampleCallback->SendData )
	{
		VO_U32 ret = m_pSampleCallback->SendData( m_pSampleCallback->pUserData , VO_SOURCE2_TT_CUSTOMTIMEDTAG , &tag );
		return ret;
	}
	return VO_RET_SOURCE2_OK;
}

VO_S32 voAdaptiveStreamingFileParser_ID3::OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	//return VO_RET_SOURCE2_OK;

	voAdaptiveStreamingFileParser_ID3 * ptr_obj = (voAdaptiveStreamingFileParser_ID3 *)pUserData;

	if( ptr_obj->m_bStop )
		return 0;
	_SAMPLE    sample = {0}; 
	memset( &sample, 0x00, sizeof( _SAMPLE ) );
	sample.uFPTrackID = ptr_obj->m_uFPTrackID;
	sample.uASTrackID = ptr_obj->m_uASTrackID;


	if( nOutputType != VO_SOURCE2_TT_HINT )
		return VO_RET_SOURCE2_OK;

	VO_SOURCE2_TRACK_TYPE  nType = VO_SOURCE2_TT_MAX;
	ID3Frame* pID3Frame = (ID3Frame*)pData;
	if(pID3Frame != NULL)
	{
		switch(pID3Frame->nSubHeadFlag)
		{
		case SUBHEAD_ID3_TXXX:
			{
				TxxxStruct *pTXXX = (TxxxStruct*)pID3Frame->pSubStruct;
				VO_CHAR * ptr_str = new VO_CHAR[pTXXX->nValueLength + 2];
				memset( ptr_str , 0 , pTXXX->nValueLength + 2 );
				memcpy( ptr_str , pTXXX->pValue , pTXXX->nValueLength );

				if( !strstr( ptr_str , "xmlns:smpte" ) )
				{
					delete []ptr_str;
					return VO_RET_SOURCE2_OK;
				}

				delete []ptr_str;

				ptr_obj->m_subtitleCodingType = VO_SUBTITLE_CodingSMPTETT;
				nType = VO_SOURCE2_TT_SUBTITLE;
				
				sample.pBuffer = pTXXX->pValue;
				sample.uSize = pTXXX->nValueLength;
				sample.uTime = ptr_obj->m_nLastSubtitleTimestamp ++;


				_TRACK_INFO trackInfo;
				memset(&trackInfo, 0x00, sizeof(_TRACK_INFO));

				//Refer URL: http://tools.ietf.org/html/draft-smpte-id3-http-live-streaming-00
				VO_CHAR ch1[255] = {0};
				VO_CHAR ch2[255] = {0};
				VO_CHAR *pDescription = (VO_CHAR*)pTXXX->pDescription;
				if( pDescription )
					sscanf( (VO_CHAR*)pDescription, "%[^,],%s",ch1,ch2);
				VO_CHAR ch11[255] = {0};
				VO_CHAR ch12[255] = {0};
				VO_CHAR ch21[255] = {0};
				VO_CHAR ch22[255] = {0};

				if(ch1)
					sscanf( (VO_CHAR*)ch1, "%[^:]:%s",ch11,ch12);
				if(ch2)
					sscanf( (VO_CHAR*)ch2, "%[^:]:%s",ch21,ch22);
				VO_CHAR cTrack[255] = {0};
				VO_CHAR cLang[255] = {0};
				if( ch11 && strcmp(ch11,"Track") == 0)
					strcpy(cTrack, ch12);
				if( ch11 && strcmp(ch11,"Lang") == 0)
					strcpy(cLang, ch12);
				if( ch21 && strcmp(ch21,"Track") == 0)
					strcpy(cTrack, ch22);
				if( ch21 && strcmp(ch21,"Lang") == 0)
					strcpy(cLang, ch22);

				for(int i = 0;i < strlen(cTrack); i ++)
					sample.uFPTrackID += cTrack[i];
				if( ptr_obj->m_subtitleTrackG.GetTrackInfoByFPTrackID( sample.uFPTrackID ))
				{
					ptr_obj->m_new_subtitle = VO_FALSE;
					ptr_obj->m_bShallSendTrackInfoList = VO_FALSE;
				}
				else
				{
					ptr_obj->m_new_subtitle = VO_TRUE;
					ptr_obj->m_bShallSendTrackInfoList = VO_TRUE;
				}
				VOLOGI("ID3 Description:%s(%d). Lang:%s, uASTrackID:%d, uFPTrackID:%d"
					, pTXXX->pDescription, ptr_obj->m_new_subtitle,	cLang, sample.uASTrackID, sample.uFPTrackID );
				strcpy(trackInfo.sSubtitleInfo.chLanguage, cLang);
				
				if(ptr_obj->m_new_subtitle)
				{
					trackInfo.uASTrackID = sample.uASTrackID;
					trackInfo.uFPTrackID = sample.uFPTrackID ;

					ptr_obj->CreateTrackInfo2(nType, &trackInfo);
				}
#if 1
				ptr_obj->MarkSubtitleFlag(&sample, 1, VO_TRUE, VO_SUBTITLE_CodingSMPTETT);
				ptr_obj->MarkSubtitleFlag(&sample, 2, VO_TRUE, __INTERNALSUBTITLE);
#else
				int type = 0;
				type = VO_SUBTITLE_CodingSMPTETT;
				sample.pReserve1 = (VO_VOID*)type;
				int type2 = __INTERNALSUBTITLE;
				sample.pReserve2 = (VO_VOID*)type2;
#endif
			}
			break;
		default:
			return VO_RET_SOURCE2_OK;
		}
	}
	else
		return VO_RET_SOURCE2_OK;//endof if(pID3Frame != NULL)

	_SAMPLE *pSample = CreateSampleOP_T(&sample, ptr_obj->m_alloc );
	ptr_obj->m_SampleList.push_back(pSample);

	return VO_RET_SOURCE2_OK;
}


VO_SOURCE2_MEDIATYPE voAdaptiveStreamingFileParser_ID3::GetMeidaType()
{
	return ( VO_SOURCE2_MEDIATYPE )m_mediatype;
}

VO_U32 voAdaptiveStreamingFileParser_ID3::CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo)
{
	VODS_VOLOGI("%d new TrackInof.", type );
	_TRACK_INFO * ptr_trackinfo = pStreamInfo;
	switch( type )
	{
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ptr_trackinfo->uCodec = m_subtitleCodingType;
			ptr_trackinfo->uTrackType = VO_SOURCE2_TT_SUBTITLE;
			VO_CHAR c[8] = {0};
			if( m_subtitleCodingType == VO_SUBTITLE_CodingSMPTETT )
				strcpy(c, "SMPTE" );
			memcpy ( ptr_trackinfo->strFourCC, c, strlen(c) );
		
			VO_U32 ret = StoreTrackInfo( VO_SOURCE2_TT_SUBTITLE, ptr_trackinfo );
			m_new_subtitle = VO_TRUE;
		}	
		break;
	}
	return VO_RET_SOURCE2_OK;
 
}

VO_VOID voAdaptiveStreamingFileParser_ID3::ResetSampleList()
{
	vo_singlelink_list< _SAMPLE* >::iterator iter = m_SampleList.begin();
	vo_singlelink_list< _SAMPLE* >::iterator itere = m_SampleList.end();

	while( iter != itere )
	{
		DestroySampleOP_T( (*iter), m_alloc);
		iter++;
	}

	m_SampleList.reset();
}