
#include "ShareLib.h"
#include "voLog.h"
#include "voOSFunc.h"

#include "ShareLib_IO.h"
#include "ShareLib_Gen.h"
#include "ShareLib_AS.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif





CShareLib::CShareLib()
:m_pShareLib_Gen(NULL)
,m_pShareLib_IO(NULL)
{
	m_pShareLib_IO = new CShareLib_IO;
	m_pShareLib_Gen = new CShareLib_Gen;
	m_pShareLib_AS = new CShareLib_AS;
}

CShareLib::~CShareLib()
{
	delete m_pShareLib_IO;
	delete m_pShareLib_Gen;
	delete m_pShareLib_AS;
}

VO_U32 CShareLib::SetParameter( VO_U32 uID, VO_PTR pParam)
{	
	return SetAndGet( uID, pParam, OPTION_SET, NULL );
}
VO_U32 CShareLib::GetParameter( VO_U32 uID, VO_PTR pParam, COPY_FUNC pFunc)
{
	return SetAndGet( uID, pParam, OPTION_GET, pFunc );
}

VO_U32 CShareLib::SetAndGet(VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc )
{

	VO_U32 ret = VO_RET_SOURCE2_OK;

	if( uID & VODS_PID_SHARELIB_GENERAL_BASE == VODS_PID_SHARELIB_GENERAL_BASE )
		m_pShareLib_Gen->DoParameter( uID, pParam, nOption, pFunc );

	if( uID & VODS_PID_SHARELIB_IO_BASE == VODS_PID_SHARELIB_IO_BASE )
		m_pShareLib_IO->DoParameter( uID, pParam, nOption, pFunc );

	if( uID & VODS_PID_SHARELIB_AS_BASE == VODS_PID_SHARELIB_AS_BASE )
		m_pShareLib_AS->DoParameter( uID, pParam, nOption, pFunc );

	return ret;
}