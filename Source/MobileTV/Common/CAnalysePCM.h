#pragma once
#include "CBaseAnalyseData.h"

class CAnalysePCM :	public CBaseAnalyseData
{
public:
	CAnalysePCM();
	virtual ~CAnalysePCM();

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
};
