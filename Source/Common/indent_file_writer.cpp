#include "indent_file_writer.h"
#include "stdio.h"
#include "voString.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
indent_file_writer::indent_file_writer( const VO_CHAR * str_filename )
:m_indent(0)
{
#ifdef WIN32
	m_fp = fopen( str_filename , "wb+" );
#endif
}

indent_file_writer::~indent_file_writer(void)
{
#ifdef WIN32
	if( m_fp )
		fclose( m_fp );
#endif
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
	
#ifdef WIN32
	if( !m_fp )
		return;
	for( int i = 0 ; i < m_indent ; i++ )
		fwrite( "\t" , 1 , 1 , m_fp );

	fwrite( str , strlen( str ) , 1 , m_fp );
	fwrite( "\n" , 1 , 1 , m_fp );
#else
	VO_U32 len = m_indent + strlen( str ) + 10 ;
	VO_CHAR *log = new VO_CHAR[m_indent + strlen( str ) + 10 ];
	memset(log, 0x00, len);
	for( int i = 0 ; i < m_indent ; i++ )
	{
		log[i] = '\t';
	}
	strcpy( &log[m_indent], str );
	
	log[ m_indent + strlen(str) + 1] = '\n';
	VODS_VOLOGI("%s",log);

	delete []log;
#endif
}
