#include "vo_playlist_m3u_parser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_playlist_m3u_parser::vo_playlist_m3u_parser(void)
{
	memset( m_url , 0 , sizeof( m_url ) );
}

vo_playlist_m3u_parser::~vo_playlist_m3u_parser(void)
{
}

//now only process basic m3u, not include Extended M3U
VO_BOOL vo_playlist_m3u_parser::parse()
{
	VO_CHAR str[2048];
	VO_BOOL suc = VO_FALSE;

	memset( str , 0 , 2048 );
	suc = read_line( (VO_PBYTE)str);

	VOLOGI( "the first line read_line is: %s", str);
	if(  ( memcmp( str , "#EXTM3U"  , strlen("#EXTM3U")) && memcmp( str , "http://" , strlen("http://")) ) ||  !suc)
	{
		return VO_FALSE;
	}
	else if( !memcmp( str , "http://" , strlen("http://")) )
	{
		strcpy( m_url , str );
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_playlist_m3u_parser::get_url(VO_S32 index, VO_PCHAR strurl)
{
	if( strlen( m_url ) )
	{
		strcpy( strurl , m_url);
		return VO_TRUE;
	}
	return VO_FALSE;
}