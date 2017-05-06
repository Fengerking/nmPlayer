#pragma once
#include "CBaseAnalyseData.h"

class CAnalyseQCELP :	public CBaseAnalyseData
{
public:
	CAnalyseQCELP(void);
	virtual ~CAnalyseQCELP(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
};
