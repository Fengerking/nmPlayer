#pragma once
#include "voYYDef_filcmn.h"
#include "CMemPool.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagBaseStreamMediaSampleIndexEntry
{
	VO_U32								time_stamp;		//time stamp, <MS>
	VO_U32								pos_in_buffer;	//buffer position, first bit is key frame flag(set mean key frame)
	VO_U32								size;
	tagBaseStreamMediaSampleIndexEntry*	next;			//the next media sample, if it is null, indicate end

	inline VO_VOID SetKeyFrame(VO_BOOL bKeyFrame)
	{
		if(bKeyFrame)
			pos_in_buffer |= 0x80000000;
		else
			pos_in_buffer &= 0x7FFFFFFF;
	}

	inline VO_BOOL IsKeyFrame()
	{
		return (pos_in_buffer & 0x80000000) ? VO_TRUE : VO_FALSE;
	}

	inline VO_U32 GetPosInBuf()
	{
		return (pos_in_buffer & 0x7FFFFFFF);
	}

	inline VO_VOID Add(VO_U32 _size)
	{
		size += _size;
	}
	inline VO_VOID Cut(VO_U32 _size)
	{
		size -= _size;
	}
} BaseStreamMediaSampleIndexEntry, *PBaseStreamMediaSampleIndexEntry;

class CBaseStreamFileIndex
	: public CvoBaseObject
{
public:
	CBaseStreamFileIndex(CMemPool* pMemPool);
	virtual ~CBaseStreamFileIndex();

public:
	virtual VO_VOID		Add(PBaseStreamMediaSampleIndexEntry pEntry);
	virtual VO_VOID		AddEntries(PBaseStreamMediaSampleIndexEntry pHead, PBaseStreamMediaSampleIndexEntry pTail);
	virtual VO_VOID		RemoveAll();
	virtual VO_VOID		RemoveUntil(PBaseStreamMediaSampleIndexEntry pEntry);
	virtual VO_VOID		RemoveInclude(PBaseStreamMediaSampleIndexEntry pEntry);
	virtual VO_VOID		RemoveFrom(PBaseStreamMediaSampleIndexEntry pEntry);

	virtual VO_BOOL		IsNull();

	virtual VO_BOOL		GetEntry(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry, VO_BOOL* pbFrameDropped);
	virtual VO_BOOL		GetTail(PBaseStreamMediaSampleIndexEntry*	pTail);

	/* return value description
	-1:	fail
	0:	ok
	1:	fail, need retry */
	virtual VO_S32		GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry);
	virtual VO_BOOL		GetKeyFrameEntry(PBaseStreamMediaSampleIndexEntry* ppEntry);
	virtual VO_U32		GetCurrStartTime() {return m_pHead ? m_pHead->time_stamp : 0;}
	virtual VO_U32		GetCurrEndTime() {return m_pTail ? m_pTail->time_stamp : 0;}
	virtual VO_U32		GetCurrEntryCount();

	virtual PBaseStreamMediaSampleIndexEntry	NewEntry(VO_U32 dwTimeStamp, VO_U32 dwPosInBuffer, VO_U32 dwSize);
	virtual VO_VOID								DeleteEntry(PBaseStreamMediaSampleIndexEntry pEntry);

protected:
	PBaseStreamMediaSampleIndexEntry	m_pHead;		//actual media sample index entry content
	PBaseStreamMediaSampleIndexEntry	m_pTail;		//use for add

	CMemPool*							m_pMemPool;
};

#ifdef _VONAMESPACE
}
#endif
