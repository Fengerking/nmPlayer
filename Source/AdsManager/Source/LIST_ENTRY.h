#pragma once


#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef struct _voLIST_ENTRY {
	struct _voLIST_ENTRY *Flink;
	struct _voLIST_ENTRY *Blink;
} voLIST_ENTRY, *PvoLIST_ENTRY;

#ifdef _MAC_OS
#define GET_OBJECTPOINTER(address, type, field) ((type *)( (unsigned long)(address) - (unsigned long)(&((type *)0)->field)))
#else
#define GET_OBJECTPOINTER(address, type, field) ((type *)( (VO_U32)(address) - (VO_U32)(&((type *)0)->field)))
#endif


inline void InitializeListHead( PvoLIST_ENTRY ListHead )
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

inline bool IsListEmpty( const voLIST_ENTRY * ListHead )
{
	return ListHead->Flink == ListHead;
}

inline bool RemoveEntryList( PvoLIST_ENTRY Entry )
{
	PvoLIST_ENTRY Blink;
	PvoLIST_ENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
	return Flink == Blink;
}

inline PvoLIST_ENTRY RemoveHeadList( PvoLIST_ENTRY ListHead )
{
	PvoLIST_ENTRY Flink;
	PvoLIST_ENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}



inline PvoLIST_ENTRY RemoveTailList( PvoLIST_ENTRY ListHead )
{
	PvoLIST_ENTRY Blink;
	PvoLIST_ENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}


inline void InsertTailList( PvoLIST_ENTRY ListHead, PvoLIST_ENTRY Entry )
{
	PvoLIST_ENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}


inline void InsertHeadList( PvoLIST_ENTRY ListHead, PvoLIST_ENTRY Entry )
{
	PvoLIST_ENTRY Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}

inline void AppendTailList( PvoLIST_ENTRY ListHead, PvoLIST_ENTRY ListToAppend )
{
	PvoLIST_ENTRY ListEnd = ListHead->Blink;

	ListHead->Blink->Flink = ListToAppend;
	ListHead->Blink = ListToAppend->Blink;
	ListToAppend->Blink->Flink = ListHead;
	ListToAppend->Blink = ListEnd;
}

inline VO_U32 GetListLength( PvoLIST_ENTRY ListHead )
{
	VO_U32 count = 0;

	PvoLIST_ENTRY pEntry = ListHead->Flink;

	while( pEntry != ListHead )
	{
		count++;
		pEntry = pEntry->Flink;
	}

	return count;
}

inline PvoLIST_ENTRY GetListEntryByIndex( PvoLIST_ENTRY ListHead , VO_U32 index/*counts from 0*/ )
{
	PvoLIST_ENTRY pEntry = ListHead->Flink;

	if( pEntry == ListHead )
		return NULL;

	for( VO_U32 i = 0 ; i < index ; i++ )
	{
		if( pEntry == ListHead )
			return NULL;

		pEntry = pEntry->Flink;
	}

	return pEntry;
}

template < class type , class Comp >
	inline void InsertASCSortedList( PvoLIST_ENTRY ListHead , PvoLIST_ENTRY Entry , Comp compare )
	{
		PvoLIST_ENTRY pEntry = ListHead->Blink;

		for( ; pEntry != ListHead ; pEntry = pEntry->Blink )
		{
			type * pObj = GET_OBJECTPOINTER( pEntry , type , List );
			type * pObjToInsert = GET_OBJECTPOINTER( Entry , type , List );

			if( compare( pObj , pObjToInsert ) < 0  )
			{
				Entry->Flink = pEntry->Flink;
				pEntry->Flink->Blink = Entry;
				Entry->Blink = pEntry;
				pEntry->Flink = Entry;

				break;
			}
		}

		if( pEntry == ListHead )
			InsertHeadList( ListHead , Entry );
	}

#ifdef _VONAMESPACE
}
#endif
