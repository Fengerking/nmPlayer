
#include "voAdaptiveStreamingFileParser_unknow.h"
#include "voLog.h"
#include "voSubtitleParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voAdaptiveStreamingFileParser_unknow::voAdaptiveStreamingFileParser_unknow( VO_SOURCE2_SAMPLECALLBACK * pCallback, VO_SOURCE2_LIB_FUNC * pLibOp )
:voAdaptiveStreamingFileParser(pCallback)
,m_mediatype(-1)
{

}

 voAdaptiveStreamingFileParser_unknow::~voAdaptiveStreamingFileParser_unknow()
{
}



VO_U32 voAdaptiveStreamingFileParser_unknow::LoadParser()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingFileParser_unknow::FreeParser()
{
	return VO_RET_SOURCE2_OK;
}

VO_SOURCE2_MEDIATYPE voAdaptiveStreamingFileParser_unknow::GetMeidaType()
{
	return ( VO_SOURCE2_MEDIATYPE )m_mediatype;
}

void voAdaptiveStreamingFileParser_unknow::ParserProc( VO_PARSER_OUTPUT_BUFFER* pData )
{
	VO_SOURCE2_TRACK_TYPE type = VO_SOURCE2_TT_MAX;
	switch( pData->nType )
	{
	case VO_PARSER_OT_TEXT:
		type = VO_SOURCE2_TT_SUBTITLE;
		break;
	default:
		type = VO_SOURCE2_TT_MAX;
		break;
	}
	SendSample( type, (VO_SOURCE_SAMPLE *)pData->pOutputData );
}

VO_U32 voAdaptiveStreamingFileParser_unknow::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	VO_PARSER_OUTPUT_BUFFER buffer;
	memset( &buffer, 0x00, sizeof( VO_PARSER_OUTPUT_BUFFER ) );
	switch( pBuffer->Type )
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		buffer.nType = VO_PARSER_OT_TEXT;
		break;
	default:
		buffer.nType = VO_PARSER_OT_BASE;
		break;
	}
	VO_SOURCE_SAMPLE sample;
	memset( &sample, 0x00, sizeof( VO_SOURCE_SAMPLE ) );
	buffer.nStreamID = 0;
	buffer.pUserData = this;
	buffer.pOutputData = &sample;
	sample.Buffer = pBuffer->pBuffer;
	sample.Time = pBuffer->ullStartTime / ( pBuffer->ullTimeScale / 1000. );
	sample.Size = pBuffer->uSize;
	
	ParserProc( &buffer );
	return VO_RET_SOURCE2_OK;
}
// VO_VOID voAdaptiveStreamingFileParser_unknow::SetFormatChange()
// {
// 	;
// }

VO_VOID voAdaptiveStreamingFileParser_unknow::SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_PTR pBuffer )
{
	VO_SOURCE_SAMPLE *pSample = (VO_SOURCE_SAMPLE *)pBuffer;
	_SAMPLE sample2;
	memset( &sample2, 0x00, sizeof( _SAMPLE ) );
	sample2.pBuffer = pSample->Buffer;
	sample2.uTime = pSample->Time;
	sample2.uSize = pSample->Size;
	VOLOGR( "SampleData-Type:%d, TimeStamp:%lld, Size:%d", nOutputType, sample2.uTime, sample2.uSize );
	//if unknown, subtitle is set to webvtt
	if( nOutputType == VO_SOURCE2_TT_SUBTITLE )
	{
		int type = 0;
		type = SUBTITLE_PARSER_TYPE_WEBVTT;
		sample2.pReserve1 = (VO_VOID*)type;
		VOLOGI( "Subtitle:TimeStamp[%lld],Size[%d]", sample2.uTime, sample2.uSize );
	}

	m_pSampleCallback->SendData( m_pSampleCallback->pUserData , nOutputType , &sample2 );
}