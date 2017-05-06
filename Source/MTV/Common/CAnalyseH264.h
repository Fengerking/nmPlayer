#pragma once
#include "CBaseAnalyseData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAnalyseH264 :public CBaseAnalyseData
{
public:
	CAnalyseH264(void);
	virtual ~CAnalyseH264(void);

	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen);

private:
	int GetFrameFormat (VO_BYTE* buffer, int nBufSize);
	int GetFrameType (VO_BYTE* pInData, int nInSize);
	int GetFrameType2 (VO_BYTE* pInData, int nInSize);
};

#ifdef _VONAMESPACE
}
#endif