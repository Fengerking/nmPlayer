#pragma once
#include "CBaseAnalyseData.h"

class CAnalyseAMR :
	public CBaseAnalyseData
{
public:
	CAnalyseAMR(bool bNB = true);
	virtual ~CAnalyseAMR(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);

protected:
	bool	mbNB;
};
