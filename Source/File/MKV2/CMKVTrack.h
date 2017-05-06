#pragma once
#include "CBaseTrack.h"
#include "MKVInfoStructure.h"
#include "CMKVAnalyze.h"
#define read_pointer2(p, l)\
{\
	MemCopy(p, pSrc, l);\
	pSrc += l;\
}

#define skip2(l)\
{\
	pSrc += l;\
}

#define read_byte2(b)\
{\
	read_pointer2(&b, 1);\
}

#define read_word2(w)\
{\
	tp = (VO_PBYTE)&w;\
	read_pointer2(tp + 1, 1);\
	read_pointer2(tp, 1);\
}

#define read_dword2(dw)\
{\
	tp = (VO_PBYTE)&dw;\
	read_pointer2(tp + 3, 1);\
	read_pointer2(tp + 2, 1);\
	read_pointer2(tp + 1, 1);\
	read_pointer2(tp, 1);\
}

#define read_fcc2(dw)\
{\
	read_pointer2(&dw, 4);\
}

#define STRCODECID2FOURCC(codec, fcc) if(MemCompare(m_pTrackInfo->str_CodecID , (VO_PTR)codec , strlen(codec)) == 0) {\
	*pCC = fcc;\
	return VO_ERR_SOURCE_OK;}
/*

class CMKVFileReader;

struct VO_SOURCE_BACKSAMPLE{
	VO_SOURCE_SAMPLE*	Sample;
	VO_U64				FilePos;
};
*/

struct VO_SOURCE_SAMPLE_STRUCT{
	VO_SOURCE_SAMPLE*	Sample;
	VO_BOOL				InUse;
};

class SampleList
{
public:
	SampleList();
	~SampleList();

	VO_BOOL	AddSample(VO_SOURCE_SAMPLE* pSample);
	VO_BOOL RemoveSample(VO_SOURCE_SAMPLE* pSample);

	VO_BOOL	IsSpaceLeft(VO_U64 nSpace = 0);	
	VO_BOOL IsReachTheMaxFrameNumber();		
	VO_BOOL IsNeedToStop();	
	
/*
	VO_U32	GetBackSampleCount();
	VO_U32	MoveBackSampleToSampleList(VO_PTR pFileIO);
*/

	VO_BOOL CalcTheTimestamp(VO_SOURCE_SAMPLE_STRUCT* pSampleStruct);
	VO_BOOL InsertSampleAndSortbyTime(VO_SOURCE_SAMPLE_STRUCT* pSampleStruct);

	VO_U32	GetSampleSize();
	VO_U32	GetSampleByTime(VO_S64 Time, VO_SOURCE_SAMPLE* pSample);
	VO_U32	GetSampleByIndex(VO_U32 Index, VO_SOURCE_SAMPLE* pSample);
	VO_U32	GetSampleCount();
	VO_VOID	Flush();

private:	
	VO_U64		m_nMaxSampleSize;
	VO_U32		m_nMaxSampleNumber;
	list_T<VO_SOURCE_SAMPLE_STRUCT *>		m_pSampleList;		
	VO_U32		m_nUnusedSampleNumber;
//	list_T<VO_SOURCE_BACKSAMPLE *>	m_pBackSampleList;	
};


class CMKVTrack : public CBaseTrack
{
public:
	CMKVTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pTrackInfo, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMKVTrack();

	VO_BOOL			SetDataParser(Parser* pParser);
	VO_BOOL 		RemoveDataParser();
	
	virtual VO_U32 GetBitrate(VO_U32* pdwBitrate){ *pdwBitrate = 0; return 0; }
//	virtual VO_U32 Prepare();
	VO_U32			GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample){return 0;}

	virtual VO_VOID 	Flush();
	VO_U32 			SetPosN(VO_S64* pPos);
	VO_U32			SetPosK(VO_S64* pPos){return 0;}
	VO_U32			GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);
	VO_U32 			Start();
	VO_U32			MoveTo(VO_S64 llTimeStamp);
	virtual VO_VOID 	SetParseForSelf(VO_BOOL bValue) {m_bParseForSelf = bValue;}
	virtual VO_BOOL     GetParseForSelf() {return m_bParseForSelf;}
	VO_BOOL 		IsNeedToCreateNewDataParser();
	VO_BOOL SetNotifyFuc(VO_PTR pOnNotifyUser, pOnNotify OnNotify);
	VO_VOID PrintMKVTrackSample(VO_SOURCE_SAMPLE* pSample);
	VO_VOID PrintMKVTrackBuffer(VO_SOURCE_SAMPLE* pSample);
	//VO_U32 SetPosN(VO_S64* pPos);
/*
	VO_BOOL OnReceivedData( MKVFrame * pFrame );
	VO_VOID SetGeneralSegmentInfo( MKVSegmentStruct * pSegInfo ){ m_pSegInfo = pSegInfo; };

	VO_BOOL OnBlockWithFilePos( VO_U64 timestamp );
	
	VO_VOID Notify( DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam );
	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame ) = 0;
	virtual VO_VOID NewClusterStarted() = 0;
	virtual VO_VOID NewBlockStarted() = 0;
	virtual VO_VOID FileReadEnd() = 0;							//use a constant value to fill the timecode,perhaps the last frame timecode is the duration
	virtual VO_VOID NewBlockGroupStarted() = 0;
	virtual VO_VOID NewReferenceBlockArrived() = 0;
	virtual VO_VOID NewClusterTimeCodeArrived( VO_S64 timecode ) = 0;

	virtual VO_VOID CheckCacheAndTransferTempData();
	virtual VO_VOID SetFlagForThumbnail(VO_BOOL beThumbnail){m_IsGetThumbNail = beThumbnail;}
	virtual VO_VOID SetIsAboutToSeek(){};
*/


protected:
	static VO_BOOL SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );
	VO_BOOL		TrackGenerateIndex();
	VO_BOOL CanGenerateMoreSample(VO_U64 nSpace = 0);
	

protected:
	TracksTrackEntryStruct * m_pTrackInfo;
	SampleList*	m_pSampleList;
	VO_BOOL 	m_bParseForSelf;

	//use for debug
	VO_PTR		m_hMKV;
	VO_PTR		m_hMKVBuffer;
public:
	Parser* m_pParser;
	NotifyStruct	m_nIsNeedNewParserNotify;	

/*
	MKVSegmentStruct * m_pSegInfo;

	MKVMediaSampleIndexEntry * m_pTempFrameIndexListHead;	//it points to cache frame list head
	MKVMediaSampleIndexEntry * m_pTempFrameIndexListTail;	//it points to cache frame list tail

	MKVMediaSampleIndexEntry * m_pTempFrameLastGroupPointer; //it points to the last frame node before the latest group

	VO_U32 m_BlocksInCache;
	VO_U32 m_FramesAvailable;	//if a frame is available ,it means it has already got a cluster timecode
	VO_BOOL m_IsFileReadEnd;
	VO_BOOL m_IsGetThumbNail;

//count means how many frames you want to calculate, return value is how much frames have been calculated
//if count is 0xffffffff it means we should calculate to the end of the temp list
	virtual VO_U32 CalculateTimeCode( VO_U32 count );

	VO_VOID TransferTempData( VO_U32 count );

	inline MKVMediaSampleIndexEntry * FindLastEntryWithoutClusterTimeCode()
	{
		MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;

		while( pEntry )
		{
			if( pEntry->cluster_timecode == -1 )
				break;

			pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;
		}

		return pEntry;
	}
*/
};
