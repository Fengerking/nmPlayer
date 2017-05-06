#pragma once
#include "CBaseAnalyseData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAnalysePCM :	public CBaseAnalyseData
{
public:
	CAnalysePCM();
	virtual ~CAnalysePCM();

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
};
#ifdef _VONAMESPACE
}
#endif
