
#include "ShareLib_IO.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif




CShareLib_IO::CShareLib_IO()
:m_pIO(NULL)
,m_pIOHttpCB(NULL)
{

}

CShareLib_IO::~CShareLib_IO()
{
	
}


VO_U32 CShareLib_IO::DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( uID)
	{
		/*io*/
	case VODS_PID_SHARELIB_IO_PTR:
		_GETSET_PTR( m_pIO, pParam, nOption );
		break;
	case VODS_PID_SHARELIB_IO_HTTPHCB:
		_GETSET_PTR( m_pIOHttpCB, pParam, nOption  );
		break;
	case VODS_PID_SHARELIB_IO_VERIFICATION:
		_GETSET_STRUCT( &m_IOVerification, pParam, nOption, pFunc  );
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	return ret;
}

