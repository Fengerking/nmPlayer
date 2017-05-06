#pragma once
#include "CBaseAnalyseData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAnalyseMPEG4 :
	public CBaseAnalyseData
{
public:
	CAnalyseMPEG4(void);
	virtual ~CAnalyseMPEG4(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen);
protected:

	//VO_BOOL		GetVideoResolution(VO_BYTE *pData , VO_U32 nDataLen);
};

#ifdef _VONAMESPACE
}
#endif
