#include "SourceInfoXMLDumper.h"

SourceInfoXMLDumper::SourceInfoXMLDumper( const VO_CHAR * dump_xml , const VO_CHAR * filename , ISource2Interface * ptr_obj )
:m_ptr_source( ptr_obj )
{
	memset( m_filename , 0 , sizeof( m_filename ) );
	strcpy( m_filename , filename );

	VO_CHAR xml_path[2048];
	memset( xml_path , 0 , sizeof( xml_path ) );
	strcpy( xml_path , dump_xml );
	strcat( xml_path , "info.xml" );

	m_ptr_writer = new xml_writer( xml_path , XML_ENCODING_TYPE_UTF8 );
}

SourceInfoXMLDumper::~SourceInfoXMLDumper(void)
{
	if( m_ptr_writer )
		delete m_ptr_writer;
}

void SourceInfoXMLDumper::Dump()
{
	m_ptr_writer->write_document_start( "SourceInfoDump" );

	m_ptr_writer->write_element_attribute( "FileName" , m_filename );

	VO_U64 duration = 0;
	VO_U32 ret = m_ptr_source->GetDuration( &duration );

	if( ret == VO_RET_SOURCE2_OK )
		m_ptr_writer->write_element_attribute( "Duration" , duration );

	VO_U32 programcount = 0;
	ret = m_ptr_source->GetProgramCount( &programcount );

	if( ret == VO_RET_SOURCE2_OK )
	{
		m_ptr_writer->write_element_start( "ProgramCount" );
		m_ptr_writer->write_element_value( programcount );
		m_ptr_writer->write_element_end();
	}

	for( int i = 0 ; i < programcount ; i++ )
	{
		VO_SOURCE2_PROGRAM_INFO * ptr_info = 0;

		ret = m_ptr_source->GetProgramInfo( i , &ptr_info );

		if( ret == VO_RET_SOURCE2_OK )
		{
			DumpProgram( ptr_info );
		}
	}

	m_ptr_writer->write_document_end();
}

void SourceInfoXMLDumper::DumpProgram( VO_SOURCE2_PROGRAM_INFO * ptr_info )
{
	m_ptr_writer->write_element_start( "ProgramInfo" );
	m_ptr_writer->write_element_attribute( "ProgramID" , ptr_info->uProgramID );
	m_ptr_writer->write_element_attribute( "SelInfo" , ptr_info->uSelInfo );
	m_ptr_writer->write_element_attribute( "ProgramType" , (VO_U64)ptr_info->sProgramType );
	m_ptr_writer->write_element_attribute( "Name" , ptr_info->strProgramName );

	m_ptr_writer->write_element_start( "StreamCount" );
	m_ptr_writer->write_element_value( ptr_info->uStreamCount );
	m_ptr_writer->write_element_end();

	for( int i = 0 ; i < ptr_info->uStreamCount ; i++ )
	{
		DumpStream( ptr_info->ppStreamInfo[i] );
	}


	m_ptr_writer->write_element_end();
}

void SourceInfoXMLDumper::DumpStream( VO_SOURCE2_STREAM_INFO * ptr_info )
{
	m_ptr_writer->write_element_start( "StreamInfo" );
	m_ptr_writer->write_element_attribute( "StreamID" , ptr_info->uStreamID );
	m_ptr_writer->write_element_attribute( "SelInfo" , ptr_info->uSelInfo );
	m_ptr_writer->write_element_attribute( "Bitrate" , ptr_info->uBitrate );

	m_ptr_writer->write_element_start( "TrackCount" );
	m_ptr_writer->write_element_value( ptr_info->uTrackCount );
	m_ptr_writer->write_element_end();

	for( int i = 0 ; i < ptr_info->uTrackCount ; i++ )
	{
		DumpTrack( ptr_info->ppTrackInfo[i] );
	}

	m_ptr_writer->write_element_end();
}

void SourceInfoXMLDumper::DumpTrack( VO_SOURCE2_TRACK_INFO * ptr_info )
{
	m_ptr_writer->write_element_start( "TrackInfo" );
	m_ptr_writer->write_element_attribute( "TrackID" , ptr_info->uTrackID );
	m_ptr_writer->write_element_attribute( "SelInfo" , ptr_info->uSelInfo );
	m_ptr_writer->write_element_attribute( "TrackType" , ptr_info->uTrackType );
	m_ptr_writer->write_element_attribute( "Codec" , ptr_info->uCodec );
	m_ptr_writer->write_element_attribute( "FourCC" , (VO_CHAR *)ptr_info->strFourCC );
	m_ptr_writer->write_element_attribute( "Duration" , ptr_info->uDuration );

	m_ptr_writer->write_element_start( "ChunkCount" );
	m_ptr_writer->write_element_value( ptr_info->uChunkCounts );
	m_ptr_writer->write_element_end();

	m_ptr_writer->write_element_start( "Bitrate" );
	m_ptr_writer->write_element_value( ptr_info->uBitrate );
	m_ptr_writer->write_element_end();

	m_ptr_writer->write_element_start( "HeaderSize" );
	m_ptr_writer->write_element_value( ptr_info->uHeadSize );
	m_ptr_writer->write_element_end();

	if( ptr_info->uTrackType == VO_SOURCE2_TT_AUDIO )
	{
		m_ptr_writer->write_element_start( "AudioDescription" );
		m_ptr_writer->write_element_attribute( "Language" , ptr_info->sAudioInfo.chLanguage );
		m_ptr_writer->write_element_end();

		m_ptr_writer->write_element_start( "AudioFormat" );
		m_ptr_writer->write_element_attribute( "SampleRate" , ptr_info->sAudioInfo.sFormat.SampleRate );
		m_ptr_writer->write_element_attribute( "Channels" , ptr_info->sAudioInfo.sFormat.Channels );
		m_ptr_writer->write_element_attribute( "SampleBits" , ptr_info->sAudioInfo.sFormat.SampleBits );
		m_ptr_writer->write_element_end();
	}
	else if( ptr_info->uTrackType == VO_SOURCE2_TT_VIDEO )
	{
		m_ptr_writer->write_element_start( "VideoDescription" );
		m_ptr_writer->write_element_attribute( "Angle" , ptr_info->sVideoInfo.uAngle );
		m_ptr_writer->write_element_attribute( "Detail" , ptr_info->sVideoInfo.strVideoDesc );
		m_ptr_writer->write_element_end();

		m_ptr_writer->write_element_start( "VideoFormat" );
		m_ptr_writer->write_element_attribute( "Width" , ptr_info->sVideoInfo.sFormat.Width );
		m_ptr_writer->write_element_attribute( "Height" , ptr_info->sVideoInfo.sFormat.Height );
		m_ptr_writer->write_element_end();
	}
	else if( ptr_info->uTrackType == VO_SOURCE2_TT_SUBTITLE )
	{
		m_ptr_writer->write_element_start( "SubtitleDescription" );
		m_ptr_writer->write_element_attribute( "Language" , ptr_info->sSubtitleInfo.chLanguage );
		m_ptr_writer->write_element_attribute( "CodingType" , ptr_info->sSubtitleInfo.uCodingType);
		m_ptr_writer->write_element_end();
	}

	m_ptr_writer->write_element_end();
}
