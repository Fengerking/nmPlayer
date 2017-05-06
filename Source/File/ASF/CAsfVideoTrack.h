#pragma once
#include "CAsfTrack.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAsfVideoTrack :
	public CAsfTrack
{
public:
	CAsfVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfVideoTrack();

	virtual	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32			GetFrameTime(VO_U32* pdwFrameTime);
	virtual VO_U32			GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader);
	virtual VO_U32			GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime) {return m_pReader->GetNearKeyFrame(llTime, pllPreviousKeyframeTime, pllNextKeyframeTime);}

	virtual VO_U32			GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	//video track must implement them!!
	virtual VO_U32			GetSampleN(VO_SOURCE_SAMPLE* pSample);

	virtual VO_U32			GetSampleK(VO_SOURCE_SAMPLE* pSample);

	virtual VO_U32			GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64			GetFilePosByFileIndex(VO_U32 dwFileIndex);

	VO_U32					GetNextKeyFrameSize();
	virtual VO_U32          GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
private:
	VO_U32 m_iPosMaxKeyFrame;
	VO_U32 m_iSizeMaxKeyFrame;	//For thumbnail
	VO_U32 m_uThumbnailCnt;
};

#ifdef _VONAMESPACE
}
#endif