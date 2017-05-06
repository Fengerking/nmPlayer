#pragma once

#include "CBaseStreamFileTrack.h"
#include "CGFileChunk.h"
#include "CFlvParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CFlvReader;
class CFlvTrack : public CBaseStreamFileTrack
{
	friend class CFlvReader;
public:
	CFlvTrack(VO_TRACKTYPE nType,  VO_U8 btStreamNum, VO_U32 dwDuration, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CFlvTrack();

public:
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);

protected:
	virtual VO_BOOL		Init(VO_U32 nSourceOpenFlags, VO_U32 dwExtSize);

	virtual VO_BOOL		AddSample(CGFileChunk* pFileChunk, VO_BOOL bKeyFrame, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_U64 pos);

protected:
	CFlvReader*			m_pReader;
	VO_S32				m_lMaxSampleSize;
};


#ifdef _VONAMESPACE
}
#endif
