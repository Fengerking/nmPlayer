#pragma once

#include "voIndex.h"
#include "voMem.h "

#define VO_INDEX_FILTER_SOURCE			_VO_INDEX_FILTER | 0x10000
#define VO_INDEX_FILTER_VIDEODECODER	_VO_INDEX_FILTER | 0x20000
#define VO_INDEX_FILTER_VIDEORENDER		_VO_INDEX_FILTER | 0x30000
#define VO_INDEX_FILTER_AUDIODECODER	_VO_INDEX_FILTER | 0x40000
#define VO_INDEX_FILTER_FILEWRITER		_VO_INDEX_FILTER | 0x50000


class CMemoryOpt
{
public:
	CMemoryOpt(VO_MEM_OPERATOR *pMemOpt , VO_S32 uID);
	virtual ~CMemoryOpt(void);

	void SetMemOpt(VO_MEM_OPERATOR *pMemOpt){mpMemOpt = pMemOpt;}

	void * Alloc(VO_U32 nSize);
	VO_U32 Free(VO_PTR pBuff);
	VO_U32 Set (VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize);
	VO_U32 Copy(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize);
	VO_U32 Check(VO_PTR pBuffer, VO_U32 uSize);
	VO_S32 Compare(VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize);
	VO_U32 Move(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize);
protected:
	VO_MEM_OPERATOR *	mpMemOpt;
	VO_S32				mnID;
};
