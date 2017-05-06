
#pragma once

#include "CGFileChunk.h"
#include "CvoBaseMemOpr.h"
#include "MKVInfoStructure.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


struct IDAndSizeInfo 
{
	VO_PBYTE	pID;
	VO_S32		s_ID;
	VO_S64		size;
};


class SeekHeadElementStructComp
{
public:
	VO_S64 operator()( SeekHeadElementStruct * p1 , SeekHeadElementStruct * p2 )
	{
		return p1->SeekPos - p2->SeekPos;
	}
};

class CuesCuePointStructComp
{
public:
	VO_S64 operator()( CuesCuePointStruct * p1 , CuesCuePointStruct * p2 )
	{
		return p1->CueTime - p2->CueTime;
	}
};



class CMKVAnalyze: public CvoBaseMemOpr
{
typedef VO_BOOL ( CMKVAnalyze:: * pProcessSection )( IDAndSizeInfo * pInfo , VO_VOID * pParam );

public:

	CMKVAnalyze( CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp );
	VO_VOID AnalyzeMKV( VO_S64 filesize , MKVInfo * pInfo );

	VO_BOOL ReadIDAndSize( IDAndSizeInfo * pInfo );

	//if processed this section please return VO_TRUE, if not please return VO_FALSE
	VO_BOOL ProcessMainSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessSegmentSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessSeekHeadSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessSeekHeadSeekSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessSegmentInfoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessTracksSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessTrackEntrySection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessTrackEntryVideoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessTrackEntryAudioSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessCuesSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessCuePointSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessCuePointTrackPositionsSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_BOOL ProcessContentEncodings( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessContentEncoding( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_BOOL ProcessContentCompression( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	//

	//Addedb by Aiven, dump the MKV HeadData for debug.
	VO_VOID PrintMKVSegmentStruct(MKVSegmentStruct* info);
	
	inline static void InitializeTracksTrackEntryStruct( TracksTrackEntryStruct * pObj )
	{
		pObj->IsDefault = VO_TRUE;
		pObj->IsEnabled = VO_TRUE;
		pObj->IsForced = VO_FALSE;
		pObj->TrackTimecodeScale = 1.0;
		strcpy( (char*)(pObj->str_Language) , "eng");

		pObj->SampleFreq = 8000.0;
		pObj->Channels = 1;

		pObj->ContentCompAlgo = -1;
	}

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
			MemFree(p);
		}
	}

	inline void InitializeMKVSegmentStruct( MKVSegmentStruct * pObj )
	{
		pObj->TimecodeScale = 1000000;

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
			MemFree(p);
		}

		pEntry = pObj->TracksTrackEntryListHeader.Flink;
		for( ; pEntry != &(pObj->TracksTrackEntryListHeader) ; )
		{
			TracksTrackEntryStruct * p = GET_OBJECTPOINTER( pEntry , TracksTrackEntryStruct , List );

			if( p->pCodecPrivate )
				MemFree( p->pCodecPrivate );

			if( p->ptr_ContentCompSettings )
				MemFree( p->ptr_ContentCompSettings );

			pEntry = pEntry->Flink;
			MemFree(p);
		}

		pEntry = pObj->CuesCuePointEntryListHeader.Flink;
		for( ; pEntry != &(pObj->CuesCuePointEntryListHeader) ; )
		{
			CuesCuePointStruct * p = GET_OBJECTPOINTER( pEntry , CuesCuePointStruct , List );
			UnInitializeCuesCuePointStruct( p );

			pEntry = pEntry->Flink;
			MemFree(p);
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
			MemFree(p);
		}
	}
	
private:
	inline VO_BOOL ReadSectionContent_uint( VO_S32 readsize , VO_S64 * pResult )
	{
		VO_PBYTE pData = (VO_BYTE*)MemAlloc(readsize);
		VO_PBYTE pPos = ( VO_PBYTE )pResult;
		*pResult = 0;

		m_pFileChunk->FRead( pData , readsize );

		for( VO_S32 i = 0 ; i < readsize ; i++ )
		{
			pPos[i] = pData[ readsize - i - 1 ];
		}

		MemFree(pData);

		return VO_TRUE;
	}

	inline VO_VOID BigEndianLittleEndianExchange( VO_VOID * pData , VO_S32 DataSize )
	{
		VO_PBYTE pBuffer = (VO_PBYTE)pData;

		for( VO_S32 i = 0 ; i < DataSize / 2 ; i++ )
		{
			VO_BYTE temp = pBuffer[i];
			pBuffer[i] = pBuffer[DataSize-i-1];
			pBuffer[DataSize-i-1] = temp;
		}
	}

	void ReadSection( pProcessSection ProcessSection , VO_S64 SectionSize , VO_VOID * pParam );

	CGFileChunk * m_pFileChunk;
	VO_U64        m_lastfilepos;
};

#ifdef _VONAMESPACE
}
#endif
