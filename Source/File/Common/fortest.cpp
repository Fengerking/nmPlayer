

#include "fortest.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_VOID output( const char * str , VO_S32 id )
{
	if(str != NULL)
		VOLOGR( "%s %d" , str , id );
}
VO_VOID output( const char * str , VO_S32 id ,int logflag)
{
	if(str != NULL)
	{
		switch (logflag)
		{
		case 0:
			VOLOGE( "%s %d" , str , id );
			break;
		case 1:
			VOLOGW( "%s %d" , str , id );
			break;
		case 2:
			VOLOGI( "%s %d" , str , id );
			break;
		case 5:
			VOLOGR( "%s %d" , str , id );
			break;
		default:
			VOLOGR( "%s %d" , str , id );
			break;
		}
	}
}
// End define
#ifdef _VONAMESPACE
}
#endif
 