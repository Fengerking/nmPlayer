#pragma once
#include "CBaseAnalyseData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAnalyseAAC :	public CBaseAnalyseData
{
public:
	CAnalyseAAC(void);
	virtual ~CAnalyseAAC(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
};

#ifdef _VONAMESPACE
}
#endif
