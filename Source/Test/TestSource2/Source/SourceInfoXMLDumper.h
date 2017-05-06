
#ifndef __SOURCEINFOXMLDUMPER_H__

#define __SOURCEINFOXMLDUMPER_H__

#include "ISource2Interface.h"
#include "xml_writer.h"

class SourceInfoXMLDumper
{
public:
	SourceInfoXMLDumper( const VO_CHAR * dump_xml , const VO_CHAR * filename , ISource2Interface * ptr_obj );
	~SourceInfoXMLDumper(void);

	void Dump();

protected:
	void DumpProgram( VO_SOURCE2_PROGRAM_INFO * ptr_info );
	void DumpStream( VO_SOURCE2_STREAM_INFO * ptr_info );
	void DumpTrack( VO_SOURCE2_TRACK_INFO * ptr_info );

protected:
	VO_CHAR m_filename[2048];
	ISource2Interface * m_ptr_source;

	xml_writer * m_ptr_writer;
};

#endif
