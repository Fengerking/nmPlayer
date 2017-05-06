#include "vo_playlist_parser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_playlist_parser::vo_playlist_parser(void)
:m_curpos(0)
,m_ptr_buffer(0)
,m_buflen(0)
{
}

vo_playlist_parser::~vo_playlist_parser(void)
{
}

VO_VOID vo_playlist_parser::set_source(VO_PBYTE ptr_buffer)
{
	m_ptr_buffer = ptr_buffer;
	m_buflen = strlen((const VO_PCHAR)m_ptr_buffer);
	VOLOGI( "%s" , (char *)m_ptr_buffer);
}

VO_BOOL vo_playlist_parser::read_line( VO_PBYTE ptr_buffer)
{
	VOLOGI( "+readline buf len: %ld" , m_buflen );

	VO_S32 startpos = getcur_pos();
	VO_S32 i = startpos;
	
	VOLOGI( "current pos: %ld" , startpos );

	while( 1 )
	{	
		//we should check memory boundary first
		//if the start pos is already end ,return immediately
		if(i >= m_buflen)
		{
			break;
		}

		if( m_ptr_buffer[i] == '\r' || m_ptr_buffer[i] == '\n' )
		{
			break;
		}
		i++;
	}
	

	//skip \r or \n after it
	setcur_pos(i+1);

	strncpy((VO_PCHAR) ptr_buffer , (const VO_PCHAR)m_ptr_buffer+startpos, i - startpos);

	VOLOGI("--readline  strncpy: %s, size : %ld" , ptr_buffer , i - startpos);

	//indicate the caller is eof
	if(i >= m_buflen)
	{
		return VO_FALSE;
	}

	return VO_TRUE;

}
