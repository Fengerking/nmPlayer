#pragma once
#include "CBaseStreamFileTrack.h"
#include "CMKVParser.h"
#include "MKVInfoStructure.h"
#include "CMKVStreamFileIndex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


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

class CMKVFileReader;

class CMKVTrack : public CBaseStreamFileTrack
{
	friend class CMKVFileReader;
public:
	CMKVTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pTrackInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMKVTrack();

	virtual VO_U32 GetBitrate(VO_U32* pdwBitrate){ *pdwBitrate = 0; return 0; }
	virtual VO_U32 Prepare();
	virtual VO_VOID Flush();
	//VO_U32 SetPosN(VO_S64* pPos);

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
	VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);
	VO_U32 SetPosN(VO_S64* pPos, VO_BOOL bflag = VO_FALSE);
	VO_BOOL GetCurrentTimeStamp(VO_U32* timestamp);
	VO_VOID SetPosHasBeenParsed();	
	VO_BOOL PrintIndex();

protected:
	TracksTrackEntryStruct * m_pTrackInfo;
	MKVSegmentStruct * m_pSegInfo;

	MKVMediaSampleIndexEntry * m_pTempFrameIndexListHead;	//it points to cache frame list head
	MKVMediaSampleIndexEntry * m_pTempFrameIndexListTail;	//it points to cache frame list tail

	MKVMediaSampleIndexEntry * m_pTempFrameLastGroupPointer; //it points to the last frame node before the latest group

	VO_U32 m_BlocksInCache;
	VO_U32 m_FramesAvailable;	//if a frame is available ,it means it has already got a cluster timecode
	VO_BOOL m_IsFileReadEnd;
	VO_BOOL m_IsGetThumbNail;

	VO_U32	m_nPosHasBeenParsed;
	

//count means how many frames you want to calculate, return value is how much frames have been calculated
//if count is 0xffffffff it means we should calculate to the end of the temp list
	virtual VO_U32 CalculateTimeCode( VO_U32 count );

	VO_VOID TransferTempData( VO_U32 count );
	VO_BOOL CanReveiveDataFromParser();

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

};

#ifdef _VONAMESPACE
}
#endif
