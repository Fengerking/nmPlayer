#pragma once

#include "CMKVTrack.h"
#include "MKVInfoStructure.h"


class CMKVVideoTrack : public CMKVTrack
{
public:
	CMKVVideoTrack( VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pVideoInfo , VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVVideoTrack();

	VO_BOOL CheckHeadData(VO_PBYTE pHeadData, VO_U32 nSize, VO_U32 nCodec);
	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	VO_U32 GetCodecCC(VO_U32* pCC);

	VO_U32 GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);

	VO_VOID Flush();

	//VO_U32 SetPosN(VO_S64* pPos);
/*

	VO_U32 GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame );
	VO_VOID NewClusterStarted();
	VO_VOID NewBlockStarted();
	VO_VOID FileReadEnd();
	VO_VOID NewClusterTimeCodeArrived( VO_S64 timecode );
	VO_VOID NewBlockGroupStarted();
	VO_VOID NewReferenceBlockArrived();
*/	
	VO_U32	GetFrameTime(VO_U32* pdwFrameTime);
	VO_U32	GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader);
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);
	
protected:
//	VO_VOID RerangeFrameTimeCode( MKVMediaSampleIndexEntry * pEntry );
	VO_U32 GetFirstFrame(VO_SOURCE_SAMPLE* pSample);
	VO_U32 GetMKVProfileLevel_Video(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel, VO_VIDEO_CODINGTYPE nCodecType);
	VO_U32 GetMKVVideoProfileLevel(VO_VIDEO_PROFILELEVEL* pInfo);
	static VO_BOOL SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );

private:
	VO_PBYTE m_pHeadData;
	VO_U32 m_HeadSize;
	VO_S64 m_lastrelativetimecode;

    VO_BOOL m_bcontain_undetermined_keyframe;
	VO_U32  m_nExistThumnnailCnt;
};
