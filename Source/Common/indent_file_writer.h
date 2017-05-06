
#ifndef __INDENT_FILE_WRITER_H__

#define __INDENT_FILE_WRITER_H__

#include "voYYDef_SrcCmn.h"
#include "voType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class indent_file_writer
{
public:
	indent_file_writer( const VO_CHAR * str_filename );
	~indent_file_writer(void);

	void add_indent();
	void release_indent();

	void write_string( const VO_CHAR * str );

private:
	int m_indent;
#ifdef WIN32
	FILE * m_fp;
#endif
};
#ifdef _VONAMESPACE
}
#endif
#endif
