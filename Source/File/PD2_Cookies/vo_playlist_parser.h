#pragma once
#ifndef __voPlaylist_Parser_H__
#define __voPlaylist_Parser_H__

#include "voString.h"
#include "voMem.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_playlist_parser
{
public:
	vo_playlist_parser(void);
	virtual ~vo_playlist_parser(void);

	virtual VO_BOOL parse() = 0;

	virtual VO_BOOL read_line( VO_PBYTE ptr_buffer); 

	virtual VO_VOID set_source(VO_PBYTE ptr_buffer);

	virtual VO_BOOL get_url(VO_S32 index, VO_PCHAR strurl) = 0;

private:
	VO_S32 getcur_pos()
	{
		return m_curpos;
	}

	VO_VOID setcur_pos(VO_S32 pos)
	{
		m_curpos = pos;
	}

private:
	//record the current pos of readline op in the buffer
	VO_S32 m_curpos;

	VO_PBYTE m_ptr_buffer;
	VO_S32 m_buflen;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __voPlaylist_Parser_H__
