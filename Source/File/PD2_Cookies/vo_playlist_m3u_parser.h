#pragma once
#include "vo_playlist_parser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_playlist_m3u_parser :
	public vo_playlist_parser
{
public:
	vo_playlist_m3u_parser(void);
	virtual ~vo_playlist_m3u_parser(void);

	virtual VO_BOOL parse();
	virtual VO_BOOL get_url(VO_S32 index, VO_PCHAR strurl);

private:
	VO_CHAR m_url[2048];
};

#ifdef _VONAMESPACE
}
#endif
