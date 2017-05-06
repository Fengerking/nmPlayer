
#include "ShareLib_Gen.h"
#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CShareLib_Gen::CShareLib_Gen()
:m_pLibOp(NULL)
,m_pLogOp(NULL)
{

}

CShareLib_Gen::~CShareLib_Gen()
{

}


VO_U32 CShareLib_Gen::DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( uID)
	{
	case VODS_PID_SHARELIB_GENERAL_LIBOP:
		_GETSET_PTR(m_pLibOp, pParam, nOption );
		break;
	case VODS_PID_SHARELIB_GENERAL_LOGOP:
		_GETSET_PTR(m_pLogOp, pParam, nOption );
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	return ret;
}