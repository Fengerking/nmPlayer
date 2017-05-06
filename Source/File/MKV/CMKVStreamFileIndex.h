#pragma once
#include "CBaseStreamFileIndex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct MKVMediaSampleIndexEntry:public BaseStreamMediaSampleIndexEntry
{
	MKVMediaSampleIndexEntry( VO_S64 Cluster_TimeCode , VO_S64 Relative_TimeCode , VO_U32 _pos_in_buffer, VO_U32 _size )
		//:BaseStreamMediaSampleIndexEntry( Cluster_TimeCode + Relative_TimeCode , _pos_in_buffer , _size )
		:cluster_timecode( Cluster_TimeCode )
		,relative_timecode( Relative_TimeCode )
		,pre(NULL)
	{
		time_stamp = (VO_U32)(Cluster_TimeCode + Relative_TimeCode);
		pos_in_buffer = _pos_in_buffer;
		size = _size;
		if( Cluster_TimeCode != -1 )
			time_stamp = (VO_U32)(cluster_timecode + relative_timecode);
	}

	void set_clustertimecode( VO_S64 timecode )
	{
		cluster_timecode = timecode;
		time_stamp = (VO_U32)(cluster_timecode + relative_timecode);
	}

	void set_relativetimecode( VO_S64 timecode )
	{
		relative_timecode = timecode;

		if( cluster_timecode != -1 )
			time_stamp = (VO_U32)(cluster_timecode + relative_timecode);
	}

	void add_relativetimecode( VO_S64 timecodeplus )
	{
		relative_timecode += timecodeplus;

		if( cluster_timecode != -1 )
			time_stamp = (VO_U32)(cluster_timecode + relative_timecode);
	}

	VO_S64 cluster_timecode;
	VO_S64 relative_timecode;
	VO_U64 filepos;

	BaseStreamMediaSampleIndexEntry * pre;
};

class CMKVStreamFileIndex :
	public CBaseStreamFileIndex
{
public:
	CMKVStreamFileIndex(CMemPool* pMemPool);
	~CMKVStreamFileIndex(void);

	VO_S32 GetEntryByFilePos(VO_BOOL bVideo, VO_U64 filepos, PBaseStreamMediaSampleIndexEntry* ppEntry);
	VO_U64 GetCurrStartFilePos();
	VO_U64 GetCurrEndFilePos();

	VO_U32 GetCurrStartBufferPos();
	VO_U32 GetCurrEndBufferPos();
	VO_U32 GetKeyFrameCnt();
	virtual VO_S32 GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry);
	VO_S32 PrintIndex();

public:
	virtual MKVMediaSampleIndexEntry*	NewEntry( VO_S64 Cluster_TimeCode , VO_S64 Relative_TimeCode , VO_U32 dwPosInBuffer, VO_U32 dwSize );
	virtual VO_VOID								DeleteEntry(MKVMediaSampleIndexEntry* pEntry);
};

#ifdef _VONAMESPACE
}
#endif
