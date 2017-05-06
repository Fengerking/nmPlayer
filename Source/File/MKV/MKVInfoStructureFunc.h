#pragma once

#include "MKVInfoStructure.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

inline void InitializeCuesCuePointStruct( CuesCuePointStruct * pObj )
{
	InitializeListHead( &( pObj->TrackPositionsHeader ) );
}

inline void UnInitializeCuesCuePointStruct( CuesCuePointStruct * pObj )
{
	voLIST_ENTRY * pEntry = pObj->TrackPositionsHeader.Flink;
	for( ; pEntry != &(pObj->TrackPositionsHeader) ; )
	{
		CuesCuePointTrackPositionsStruct * p = GET_OBJECTPOINTER( pEntry , CuesCuePointTrackPositionsStruct , List );
		pEntry = pEntry->Flink;
		delete p;
	}
}

inline void InitializeMKVSegmentStruct( MKVSegmentStruct * pObj )
{
	InitializeListHead( &(pObj->CuesCuePointEntryListHeader) );
	InitializeListHead( &(pObj->SeekHeadElementEntryListHeader) );
	InitializeListHead( &(pObj->TracksTrackEntryListHeader) );
}

inline void UnInitializeMKVSegmentStruct( MKVSegmentStruct * pObj )
{
	voLIST_ENTRY * pEntry = pObj->SeekHeadElementEntryListHeader.Flink;
	for( ; pEntry != &(pObj->SeekHeadElementEntryListHeader) ; )
	{
		SeekHeadElementStruct * p = GET_OBJECTPOINTER( pEntry , SeekHeadElementStruct , List );
		pEntry = pEntry->Flink;
		delete p;
	}

	pEntry = pObj->TracksTrackEntryListHeader.Flink;
	for( ; pEntry != &(pObj->TracksTrackEntryListHeader) ; )
	{
		TracksTrackEntryStruct * p = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );
		pEntry = pEntry->Flink;
		delete p;
	}

	pEntry = pObj->CuesCuePointEntryListHeader.Flink;
	for( ; pEntry != &(pObj->CuesCuePointEntryListHeader) ; )
	{
		CuesCuePointStruct * p = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
		UnInitializeCuesCuePointStruct( p );

		pEntry = pEntry->Flink;
		delete p;
	}
}

inline void InitializeMKVInfo( MKVInfo * pObj )
{
	InitializeListHead( &(pObj->MKVSegmentListHeader) );
}

inline void UnInitializeMKVInfo( MKVInfo * pObj )
{
	voLIST_ENTRY * pEntry = pObj->MKVSegmentListHeader.Flink;
	for( ; pEntry != &(pObj->MKVSegmentListHeader) ; )
	{
		MKVSegmentStruct * p = GET_OBJECTPOINTER( pEntry , MKVSegmentStruct , List );
		UnInitializeMKVSegmentStruct( p );

		pEntry = pEntry->Flink;
		delete p;
	}
}

#ifdef _VONAMESPACE
}
#endif
