
#ifndef __VOSUBTITLEFUNC_H__

#define __VOSUBTITLEFUNC_H__

#include "voYYDef_Common.h"
#include "voSubtitleType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

bool isequal_subtitleinfo( const voSubtitleInfo* ptr_a , const voSubtitleInfo* ptr_b );

/*
voSubtitleInfo* create_subtitleinfo( const voSubtitleInfo* ptr_info );

void destroy_subtitleinfo( const voSubtitleInfo* ptr_info );
*/
	
#ifdef _VONAMESPACE
}
#endif

#endif //__VOSUBTITLEFUNC_H__