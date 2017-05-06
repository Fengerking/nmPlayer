
#ifndef __FORTEST_H__

#define __FORTEST_H__

#include "voYYDef_filcmn.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


	VO_VOID output( const char * str , VO_S32 id );
	VO_VOID output( const char * str , VO_S32 id ,int logflag );

#ifdef _VONAMESPACE
}
#endif /* _VONAMESPACE */

#endif

