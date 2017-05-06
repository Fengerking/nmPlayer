
#ifndef __INDENT_FILE_WRITER_H__

#define __INDENT_FILE_WRITER_H__

#include "voType.h"

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
	FILE * m_fp;
};

#endif
