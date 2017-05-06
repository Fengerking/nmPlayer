#ifndef __Sharelib_AS_H__
#define __Sharelib_AS_H__

#include "ShareLib.h"
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CShareLib_AS:public CShareLib
{
public:
	CShareLib_AS();
	virtual ~CShareLib_AS();

protected:
	VO_U32 DoParameter( VO_U32 uID, VO_PTR pParam, PARAMETER_OPTION nOption, COPY_FUNC pFunc  );

private:
	VO_SOURCE2_PROGRAM_TYPE m_ProgramType;
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE m_StreamingType;
};


#ifdef _VONAMESPACE
}
#endif

#endif
