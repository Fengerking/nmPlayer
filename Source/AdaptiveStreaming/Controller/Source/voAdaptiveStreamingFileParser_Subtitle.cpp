
#include "voAdaptiveStreamingFileParser_Subtitle.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define BUFFERINCREASESIZE 1024

voAdaptiveStreamingFileParser_Subtitle::voAdaptiveStreamingFileParser_Subtitle( VO_SOURCE2_SAMPLECALLBACK * pCallback, VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser_unknow(pCallback)
,m_ptr_buffer( 0 )
,m_buffer_size( BUFFERINCREASESIZE )
,m_buffer_usedsize( 0 )
,m_subtitleCodingType(VO_SUBTITLE_CodingUnused)
{
	m_ptr_buffer = new VO_BYTE[m_buffer_size];
}

 voAdaptiveStreamingFileParser_Subtitle::~voAdaptiveStreamingFileParser_Subtitle()
{
	delete []m_ptr_buffer;
}



VO_U32 voAdaptiveStreamingFileParser_Subtitle::LoadParser()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingFileParser_Subtitle::FreeParser()
{
	return VO_RET_SOURCE2_OK;
}
VO_VOID voAdaptiveStreamingFileParser_Subtitle::SetFileFormat( FileFormat ff )
{
	m_ff = ff; 
	switch(m_ff)
	{
	case FileFormat_WEBVTT:
		m_subtitleCodingType = VO_SUBTITLE_CodingWEBVTT;
		break;
	case FileFormat_TTML:
		m_subtitleCodingType = VO_SUBTITLE_CodingTTML;
		break;
	default:
		m_subtitleCodingType = VO_SUBTITLE_CodingUnused;
		break;
	}
}


void voAdaptiveStreamingFileParser_Subtitle::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	VO_SOURCE2_TRACK_TYPE type = VO_SOURCE2_TT_SUBTITLE;
	SendSample( type, (VO_SOURCE_SAMPLE *)pData->pOutputData );
}

VO_U32 voAdaptiveStreamingFileParser_Subtitle::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	if( !m_ptr_buffer )
		return VO_RET_SOURCE2_FAIL;

	VO_U32 endbuffersize = m_buffer_usedsize + pBuffer->uSize;

	if( endbuffersize > m_buffer_size )
	{
		VO_U32 newsize = ( endbuffersize / BUFFERINCREASESIZE + 4 ) * BUFFERINCREASESIZE;

		VO_PBYTE ptr_buffer = new VO_BYTE[newsize];

		if( !ptr_buffer )
			return VO_RET_SOURCE2_FAIL;

		memcpy( ptr_buffer , m_ptr_buffer , m_buffer_usedsize );

		delete []m_ptr_buffer;

		m_ptr_buffer = ptr_buffer;
		m_buffer_size = newsize;
	}

	memcpy( m_ptr_buffer + m_buffer_usedsize , pBuffer->pBuffer , pBuffer->uSize );
	m_buffer_usedsize = endbuffersize;


	if( pBuffer->bIsEnd )
	{
		pBuffer->pBuffer = m_ptr_buffer;
		pBuffer->uSize = m_buffer_usedsize;
		VO_U32 ret = voAdaptiveStreamingFileParser_unknow::Process( pBuffer );

		m_buffer_usedsize = 0;
		return ret;
	}
	else
		return VO_RET_SOURCE2_OK;
}

VO_VOID voAdaptiveStreamingFileParser_Subtitle::SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_PTR pBuffer )
{
	VO_SOURCE_SAMPLE *pSample = (VO_SOURCE_SAMPLE *)pBuffer;

	TryToSendTrackInfo( VO_SOURCE2_TT_SUBTITLE, pSample->Time, 0 );

	_SAMPLE sample2;
	memset( &sample2, 0x00, sizeof( _SAMPLE ) );
	sample2.pBuffer = pSample->Buffer;
	sample2.uTime = pSample->Time;
	sample2.uSize = pSample->Size;
	sample2.uASTrackID = m_uASTrackID;
	VOLOGR( "SampleData-Type:%d, TimeStamp:%lld, Size:%d", nOutputType, sample2.uTime, sample2.uSize );
	//if unknown, subtitle is set to webvtt
	if( nOutputType == VO_SOURCE2_TT_SUBTITLE )
	{
		int type = 0;
		type = m_subtitleCodingType;
		sample2.pReserve1 = (VO_VOID*)type;
// 		int type2 = __INTERNALSUBTITLE;
// 		sample2.pReserve2 = (VO_VOID*)type2;
		VOLOGI( "Subtitle:TimeStamp[%lld],Size[%d]", sample2.uTime, sample2.uSize );
	}

	m_pSampleCallback->SendData( m_pSampleCallback->pUserData , nOutputType , &sample2 );
}

VO_U32 voAdaptiveStreamingFileParser_Subtitle::CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo)
{
	m_subtitle_trackinfo.uASTrackID = m_uASTrackID;
	m_subtitle_trackinfo.uCodec = m_subtitleCodingType;
	m_subtitle_trackinfo.uTrackType = VO_SOURCE2_TT_SUBTITLE;
	VO_CHAR c[8] = {0};
	if( m_subtitleCodingType == VO_SUBTITLE_CodingWEBVTT )
		strcpy(c, "WEBVTT" );
	else if( m_subtitleCodingType == VO_SUBTITLE_CodingTTML )
		strcpy(c, "TTML" );

	memcpy ( m_subtitle_trackinfo.strFourCC, c, strlen(c) );
	m_new_subtitle = VO_TRUE;
	StoreTrackInfo( type, &m_subtitle_trackinfo );
	return VO_RET_SOURCE2_OK;
}	
