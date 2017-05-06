#ifndef __Sharelib_IO_H__
#define __Sharelib_IO_H__

#include "voSource2_IO.h"
#include "voLog.h"
#include "ShareLib.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CShareLib_IO:public CShareLib
{
public:
	CShareLib_IO();
	virtual ~CShareLib_IO();

protected:
	VO_U32 DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc );

private:
	/*IO*/
	VO_SOURCE2_IO_HTTPCALLBACK 		* m_pIOHttpCB;
	VO_SOURCE2_IO_API          		* m_pIO;
	VO_SOURCE2_VERIFICATIONINFO 	  m_IOVerification;

};


#ifdef _VONAMESPACE
}
#endif

#endif
