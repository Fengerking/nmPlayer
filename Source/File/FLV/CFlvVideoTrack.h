#pragma once
#include "CFlvTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CFlvVideoTrack :
	public CFlvTrack
{
public:
	CFlvVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, PFlvVideoInfo pVideoInfo, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CFlvVideoTrack();

public:
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32 GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
protected:
	//video track must implement them!!
	virtual VO_U32		GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64		GetFilePosByFileIndex(VO_U32 dwFileIndex);

public:
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetFrameTime(VO_U32* pdwFrameTime);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);

	virtual VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);


	virtual VO_U32 GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);

	virtual VO_U8			IsCannotGenerateIndex(CvoFileDataParser* pDataParser);
	VO_BOOL		IsTrackBufferHasEnoughFrame();

protected:
	PFlvVideoInfo		m_pVideoInfo;
	VO_U64		m_uFilePos;
	VO_U32		m_dwFileIndex;
	VO_U32		m_uThumbnailCnt;
};


#ifdef _VONAMESPACE
}
#endif
