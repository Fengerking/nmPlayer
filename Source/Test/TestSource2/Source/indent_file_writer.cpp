#include "indent_file_writer.h"
#include "stdio.h"
#include "voString.h"

indent_file_writer::indent_file_writer( const VO_CHAR * str_filename )
:m_indent(0)
{
	m_fp = fopen( str_filename , "wb+" );
}

indent_file_writer::~indent_file_writer(void)
{
	fclose( m_fp );
}

void indent_file_writer::add_indent()
{
	m_indent++;
}

void indent_file_writer::release_indent()
{
	m_indent--;
}

void indent_file_writer::write_string( const VO_CHAR * str )
{
	if( !m_fp )
		return;

	for( int i = 0 ; i < m_indent ; i++ )
		fwrite( "\t" , 1 , 1 , m_fp );

	fwrite( str , strlen( str ) , 1 , m_fp );
	fwrite( "\n" , 1 , 1 , m_fp );
}
