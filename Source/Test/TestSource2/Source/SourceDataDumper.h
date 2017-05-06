
#ifndef __SOURCEDATADUMPER_H__

#define __SOURCEDATADUMPER_H__

#include "ISource2Interface.h"
#include "voType.h"
#include "xml_writer.h"
#include "vo_doublelink_list.hpp"

struct SAMPLE_ELEMENT
{
	VO_U32 flag;
	VO_U64 timestamp;
	VO_U32 size;
	VO_U64 filepos;
};

class SourceDataDumper
{
public:
	SourceDataDumper( const VO_CHAR * str_dumpdir , const VO_CHAR * filename , ISource2Interface * ptr_source );
	~SourceDataDumper(void);

	void Dump();

protected:
	VO_U64 dump_data( FILE * fp , vo_doublelink_list< SAMPLE_ELEMENT * >& list , VO_SOURCE2_TRACK_TYPE track_type );

protected:
	ISource2Interface * m_ptr_source;
	xml_writer * m_ptr_writer;
	vo_doublelink_list< SAMPLE_ELEMENT * > m_videosamplelist;
	vo_doublelink_list< SAMPLE_ELEMENT * > m_audiosamplelist;

	FILE * m_audio_fp;
	FILE * m_video_fp;

	VO_CHAR m_filename[2048];

	bool m_is_video_eos;
	bool m_is_audio_eos;
};

#endif
