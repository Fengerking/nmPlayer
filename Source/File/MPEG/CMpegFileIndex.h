#pragma once
#include "CBaseStreamFileIndex.h"
#include "MpegFileDataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMpegFileIndex
	: public CBaseStreamFileIndex
{
public:
	CMpegFileIndex(CMemPool* pMemPool);
	virtual ~CMpegFileIndex();

	VO_U64		GetEntryDuration();
	VO_U32		GetFrameInfo(VO_S64 *pNearKeyFramTs,VO_S64 * pMaxTs,VO_S64 * pMinTs,VO_S64 llTimeStamp,VO_BOOL bVideo);
	VO_U32		GetEntryByCount(VO_U32 uCount, PBaseStreamMediaSampleIndexEntry* pEntry);
public:
	
};
#ifdef _VONAMESPACE
}
#endif
