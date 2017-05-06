#ifndef __Sharelib_Gen_H__
#define __Sharelib_Gen_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#include "voLog.h"
#include "ShareLib.h"
#include "voSource2.h"
class CShareLib_Gen:public CShareLib
{
public:
	CShareLib_Gen();
	virtual ~CShareLib_Gen();

protected:
	VO_U32 DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  );

private:
	/*LIB op*/
	VO_SOURCE2_LIB_FUNC		* m_pLibOp;
	/*Log op*/
	VO_LOG_PRINT_CB			* m_pLogOp;

};


#ifdef _VONAMESPACE
}
#endif

#endif
