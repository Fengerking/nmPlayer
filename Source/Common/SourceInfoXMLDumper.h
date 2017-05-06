
#ifndef __SOURCEINFOXMLDUMPER_H__

#define __SOURCEINFOXMLDUMPER_H__

#include "voYYDef_SrcCmn.h"
#include "ISource2Interface.h"
#include "xml_writer.h"
#include "voDSType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class SourceInfoXMLDumper
{
public:
	SourceInfoXMLDumper( const VO_CHAR * dump_xml , const VO_CHAR * filename ,const VO_CHAR *xmlName, ISource2Interface * ptr_obj );
	~SourceInfoXMLDumper(void);

	void Dump();
	void DumpProgram( _PROGRAM_INFO * ptr_info );
	void DumpProgram( VO_SOURCE2_PROGRAM_INFO * ptr_info );
	void DumpStream( VO_SOURCE2_STREAM_INFO * ptr_info );
	void DumpTrack( VO_SOURCE2_TRACK_INFO * ptr_info );
protected:
	void DumpStream( _STREAM_INFO * ptr_info );
	void DumpTrack( _TRACK_INFO * ptr_info );

protected:
	VO_CHAR m_filename[2048];
	ISource2Interface * m_ptr_source;

	xml_writer * m_ptr_writer;
};
#ifdef _VONAMESPACE
}
#endif
#endif
