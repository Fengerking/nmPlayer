#pragma once
#include "CBaseAnalyseData.h"

class CAnalyseAAC :	public CBaseAnalyseData
{
public:
	CAnalyseAAC(void);
	virtual ~CAnalyseAAC(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_U32 TrimSample(VO_BYTE **ppData , VO_U32 &nDataLen);
};
