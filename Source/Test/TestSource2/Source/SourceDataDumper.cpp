#include "SourceDataDumper.h"

#define INVALID_TIMESTAMP 0xffffffffffffffffll

SourceDataDumper::SourceDataDumper( const VO_CHAR * str_dumpdir , const VO_CHAR * filename , ISource2Interface * ptr_source )
:m_ptr_source( ptr_source )
,m_is_video_eos(false)
,m_is_audio_eos(false)
{
	VO_CHAR xml_filepath[1024];
	memset( xml_filepath , 0 , sizeof( xml_filepath ) );
	strcpy( xml_filepath , str_dumpdir );
	strcat( xml_filepath , "sample.xml" );

	m_ptr_writer = new xml_writer( xml_filepath , XML_ENCODING_TYPE_UTF8 );

	VO_CHAR dump_filepath[1024];
	memset( dump_filepath , 0 , sizeof( dump_filepath ) );
	strcpy( dump_filepath , str_dumpdir );
	strcat( dump_filepath , "audio.dump" );

	m_audio_fp = fopen( dump_filepath , "wb+" );

	memset( dump_filepath , 0 , sizeof( dump_filepath ) );
	strcpy( dump_filepath , str_dumpdir );
	strcat( dump_filepath , "video.dump" );

	m_video_fp = fopen( dump_filepath , "wb+" );

	memset( m_filename , 0 , sizeof( m_filename ) );
	strcpy( m_filename , filename );
}

SourceDataDumper::~SourceDataDumper(void)
{
	if( m_ptr_writer )
		delete m_ptr_writer;

	if( m_audio_fp )
		fclose( m_audio_fp );

	if( m_video_fp )
		fclose( m_video_fp );
}

void SourceDataDumper::Dump()
{
	VO_U64 video_timestamp = INVALID_TIMESTAMP;
	VO_U64 audio_timestamp = INVALID_TIMESTAMP;

	while( 1 )
	{
		video_timestamp = dump_data( m_video_fp , m_videosamplelist , VO_SOURCE2_TT_VIDEO );

		audio_timestamp = dump_data( m_audio_fp , m_audiosamplelist , VO_SOURCE2_TT_AUDIO );

		if( video_timestamp != INVALID_TIMESTAMP && audio_timestamp != INVALID_TIMESTAMP )
		{
			if( video_timestamp < audio_timestamp )
			{
				while( video_timestamp < audio_timestamp )
				{
					video_timestamp = dump_data( m_video_fp , m_videosamplelist , VO_SOURCE2_TT_VIDEO );

					if( video_timestamp == INVALID_TIMESTAMP )
						break;
				}
			}
			else if( video_timestamp > audio_timestamp )
			{
				while( video_timestamp > audio_timestamp )
				{
					audio_timestamp = dump_data( m_audio_fp , m_audiosamplelist , VO_SOURCE2_TT_AUDIO );

					if( audio_timestamp == INVALID_TIMESTAMP )
						break;
				}
			}
		}

		if( m_is_audio_eos && m_is_video_eos )
			break;
	}

	m_ptr_writer->write_document_start( "SampleDataInfo" );
	m_ptr_writer->write_element_attribute( "URL" , m_filename );
	m_ptr_writer->write_element_attribute( "DumpMode" , "RAW" );

	m_ptr_writer->write_element_start( "Video" );
	m_ptr_writer->write_element_attribute( "Count" , m_videosamplelist.count() );

	vo_doublelink_list< SAMPLE_ELEMENT * >::iterator video_iter = m_videosamplelist.begin();
	vo_doublelink_list< SAMPLE_ELEMENT * >::iterator video_itere = m_videosamplelist.end();

	while( video_iter != video_itere )
	{
		m_ptr_writer->write_element_start( "Sample" );
		m_ptr_writer->write_element_attribute( "TimeStamp" , (*video_iter)->timestamp );
		m_ptr_writer->write_element_attribute( "Size" , (*video_iter)->size );
		m_ptr_writer->write_element_attribute( "Flag" , (*video_iter)->flag );
		m_ptr_writer->write_element_attribute( "DumpFilePos" , (*video_iter)->filepos );
		m_ptr_writer->write_element_end();

		delete *video_iter;
		video_iter++;
	}

	m_ptr_writer->write_element_end();

	m_ptr_writer->write_element_start( "Audio" );
	m_ptr_writer->write_element_attribute( "Count" , m_audiosamplelist.count() );

	vo_doublelink_list< SAMPLE_ELEMENT * >::iterator audio_iter = m_audiosamplelist.begin();
	vo_doublelink_list< SAMPLE_ELEMENT * >::iterator audio_itere = m_audiosamplelist.end();

	while( audio_iter != audio_itere )
	{
		m_ptr_writer->write_element_start( "Sample" );
		m_ptr_writer->write_element_attribute( "TimeStamp" , (*audio_iter)->timestamp );
		m_ptr_writer->write_element_attribute( "Size" , (*audio_iter)->size );
		m_ptr_writer->write_element_attribute( "Flag" , (*audio_iter)->flag );
		m_ptr_writer->write_element_attribute( "DumpFilePos" , (*audio_iter)->filepos );
		m_ptr_writer->write_element_end();

		delete *audio_iter;
		audio_iter++;
	}

	m_ptr_writer->write_element_end();

	m_ptr_writer->write_document_end();
}

VO_U64 SourceDataDumper::dump_data( FILE * fp , vo_doublelink_list< SAMPLE_ELEMENT * >& list , VO_SOURCE2_TRACK_TYPE track_type )
{
	VO_SOURCE2_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_SOURCE2_SAMPLE ) );

	VO_U32 ret = m_ptr_source->GetSample( track_type , &sample );

	if( ret != VO_RET_SOURCE2_OK )
	{
		if( ret == VO_RET_SOURCE2_END )
		{
			if( VO_SOURCE2_TT_AUDIO == track_type )
				m_is_audio_eos = true;
			else
				m_is_video_eos = true;
		}

		return INVALID_TIMESTAMP;
	}

	if( sample.uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE )
		return INVALID_TIMESTAMP;

	if( sample.uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS )
	{
		if( VO_SOURCE2_TT_AUDIO == track_type )
			m_is_audio_eos = true;
		else
			m_is_video_eos = true;

		return INVALID_TIMESTAMP;
	}

	SAMPLE_ELEMENT * ptr = new SAMPLE_ELEMENT;
	ptr->filepos = ftell( fp );
	ptr->flag = sample.uFlag;
	ptr->size = sample.uSize;
	ptr->timestamp = sample.uTime;

	list.push_back( ptr );

	fwrite( sample.pBuffer , sample.uSize , 1 , fp );

	return sample.uTime;
}
