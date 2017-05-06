#ifndef __Sharelib_H__
#define __Sharelib_H__

#include "voShareLibType.h"
#include "voLog.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CShareLib
{
public:
	CShareLib();
	virtual ~CShareLib();

	virtual VO_U32 SetParameter( VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetParameter( VO_U32 uID, VO_PTR pParam, COPY_FUNC pFunc);
	VO_U32 SetAndGet(VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  );
	virtual VO_U32 DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  ){return 0;}

private:
	CShareLib		*m_pShareLib_Gen;
	CShareLib		*m_pShareLib_IO;
	CShareLib		*m_pShareLib_AS;
};


#ifdef _VONAMESPACE
}
#endif

#endif
