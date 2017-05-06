#pragma once
#include "CvoBaseMemOpr.h"
#include "voString.h"
#include "AsfFileDataStruct.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

struct CAsfIndexEntries : public CvoBaseMemOpr
{
	CAsfIndexEntries(VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfIndexEntries() {}

	virtual VO_U64	GetPacketPosByTime(VO_S64 llTimeStamp) = 0;
	virtual VO_U64	GetPacketPosByIndex(VO_U32 dwIndex) = 0;

	virtual VO_U32	GetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_S64	GetPrevKeyFrameTime(VO_S64 llTimeStamp) = 0;
	virtual VO_S64	GetNextKeyFrameTime(VO_S64 llTimeStamp) = 0;

	virtual VO_U32	SetEntriesNum(VO_U32 dwEntries) = 0;

	VO_U64					m_ullTimeInterval;		//<MS>
	VO_U32					m_dwEntries;			//entry count
};

struct CAsfIndexObjectEntries : public CAsfIndexEntries
{
	CAsfIndexObjectEntries(VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfIndexObjectEntries();

	virtual VO_U64	GetPacketPosByTime(VO_S64 llTimeStamp);
	virtual VO_U64	GetPacketPosByIndex(VO_U32 dwIndex);

	virtual VO_S64	GetPrevKeyFrameTime(VO_S64 llTimeStamp);
	virtual VO_S64	GetNextKeyFrameTime(VO_S64 llTimeStamp);

	virtual VO_U32	SetEntriesNum(VO_U32 dwEntries);

	VO_U32*					m_pEntries;	
};

struct CAsfSampleIndexObjectEntries : public CAsfIndexEntries
{
	CAsfSampleIndexObjectEntries(VO_MEM_OPERATOR* pMemOp, VO_U32 dwPacketSize);
	virtual ~CAsfSampleIndexObjectEntries();

	virtual VO_U64	GetPacketPosByTime(VO_S64 llTimeStamp);
	virtual VO_U64	GetPacketPosByIndex(VO_U32 dwIndex);

	virtual VO_S64	GetPrevKeyFrameTime(VO_S64 llTimeStamp);
	virtual VO_S64	GetNextKeyFrameTime(VO_S64 llTimeStamp);

	virtual VO_U32	SetEntriesNum(VO_U32 dwEntries);

	PAsfIndexEntry			m_pEntries;				//entry content

	VO_U32					m_dwPacketSize;
};

#ifdef _VONAMESPACE
}
#endif