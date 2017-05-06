#pragma once
#include "CBaseStreamFileIndex.h"

class CMTVStreamFileIndex : public CBaseStreamFileIndex
{
public:
	CMTVStreamFileIndex(CMemPool* pMemPool);
	virtual ~CMTVStreamFileIndex(void);

public:
	virtual VO_BOOL		GetEntry(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry);
	virtual VO_S32		GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry);

};
