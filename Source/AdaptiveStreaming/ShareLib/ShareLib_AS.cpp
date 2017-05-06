
#include "ShareLib_AS.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



CShareLib_AS::CShareLib_AS()
:m_ProgramType(VO_SOURCE2_STREAM_TYPE_UNKNOWN)
,m_StreamingType(VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN)
{

}

CShareLib_AS::~CShareLib_AS()
{

}


VO_U32 CShareLib_AS::DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( uID)
	{
	case VODS_PID_SHARELIB_AS_PROGRAMTYPE:
		_GETSET_PTR( &m_ProgramType, pParam, nOption );
		break;
	case VODS_PID_SHARELIB_AS_STREAMTYPE:
		_GETSET_PTR( &m_ProgramType, pParam, nOption );
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	return ret;
}