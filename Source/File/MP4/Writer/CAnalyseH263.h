#pragma once
#include "CBaseAnalyseData.h"

class CAnalyseH263 :	public CBaseAnalyseData
{
public:
	CAnalyseH263(void);
	virtual ~CAnalyseH263(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen);
};
