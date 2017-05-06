#pragma once
#include "CRealTrack.h"
#include "voRealVideo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct rv_segment_struct
{
	VO_BOOL		bIsValid;		//always VO_TRUE
	VO_U32		ulOffset;
} rv_segment;
typedef struct rv_frame_struct
{
	VO_U32		ulDataLen;
	VO_PBYTE	pData;
	VO_U32		ulTimestamp;
	VO_U16		usSequenceNum;	//always 0
	VO_U16		usFlags;
	VO_BOOL		bLastPacket;	//always VO_FALSE
	VO_U32		ulNumSegments;
	rv_segment*	pSegment;
} rv_frame;
class CRealVideoTrack :
	public CRealTrack
{
public:
	CRealVideoTrack(PRealTrackInfo pTrackInfo, CRealReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealVideoTrack();

public:
	virtual VO_U32			GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	//video track must implement them!!
	virtual VO_U32			GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64			GetFilePosByFileIndex(VO_U32 dwFileIndex);

	virtual VO_VOID			Flush();

public:
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodec(VO_U32* pCodec);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32			GetFrameTime(VO_U32* pdwFrameTime);
	virtual VO_U32			GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);

	virtual VO_BOOL			SetGlobalBufferExtSize(VO_U32 dwExtSize);
	VO_S32					GetThumbnailScanCount(){ return m_ThumbnallScanCount;}

protected:
	virtual	VO_BOOL			ParseExtData(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize);

	virtual VO_BOOL			OnPacket(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp);

	inline VO_BOOL			ProcessMultiFrame(CGFileChunk* pFileChunk, VO_U8 btFlags);
	inline VO_BOOL			AddPayloadInfo(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U8 btPayloadNum, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_BOOL bLastPayload = VO_TRUE);

	inline VO_VOID			AddUnvalidTimeStampEntryList(VO_U32 dwRefTime);
	inline VO_VOID			AddToUnvalidTimeStampEntryList(PBaseStreamMediaSampleIndexEntry pEntry);

protected:
	VO_U32					m_dwLastTimeStamp;
	VO_BOOL					m_bFirstFrame;
	VO_PBYTE				m_pTempBufHeader;
	VO_U32					m_dwPayloadIndex;
	VO_U32					m_dwPayloadOffset;
	VO_S32					m_nMaxEncoderFrameSize;

	PBaseStreamMediaSampleIndexEntry	m_pTempEntry;

	PBaseStreamMediaSampleIndexEntry	m_pUnvalidTimeStampEntries;
	PBaseStreamMediaSampleIndexEntry	m_pLastUnvalidTimeStampEntry;
	VO_U16								m_wUnvalidTimeStampEntries;

	rv_frame				m_TmpFrame;
};

#ifdef _VONAMESPACE
}
#endif