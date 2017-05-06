#pragma once

#include "CMKVTrack.h"
#include "MKVInfoStructure.h"
#include "CMKVParser.h"
#ifdef _RAW_DUMP_
#include "CDumper.h"
#endif
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CMKVVideoTrack : public CMKVTrack
{
public:
	CMKVVideoTrack( VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pVideoInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVVideoTrack();

	VO_BOOL CheckHeadData(VO_PBYTE pHeadData, VO_U32 nSize, VO_U32 nCodec);
	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	VO_U32 GetCodecCC(VO_U32* pCC);

	VO_U32 GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);

	VO_VOID Flush();

	//VO_U32 SetPosN(VO_S64* pPos);

	VO_U32 GetSampleN(VO_SOURCE_SAMPLE* pSample);

	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame );
	VO_VOID NewClusterStarted();
	VO_VOID NewBlockStarted();
	VO_VOID FileReadEnd();
	VO_VOID NewClusterTimeCodeArrived( VO_S64 timecode );
	VO_VOID NewBlockGroupStarted();
	VO_VOID NewReferenceBlockArrived();
	VO_U32	GetFrameTime(VO_U32* pdwFrameTime);
	VO_U32	GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader);
	VO_U8	IsCannotGenerateIndex(CvoFileDataParser* pDataParser);
	VO_BOOL	IsTrackBufferHasEnoughFrame();
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);
	
	virtual VO_VOID CheckCacheAndTransferTempData();
protected:
	VO_VOID RerangeFrameTimeCode( MKVMediaSampleIndexEntry * pEntry );
	VO_U32 GetFirstFrame(VO_SOURCE_SAMPLE* pSample);
	VO_U32 GetMKVProfileLevel_Video(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel, VO_VIDEO_CODINGTYPE nCodecType);
	VO_U32 GetMKVVideoProfileLevel(VO_VIDEO_PROFILELEVEL* pInfo);
//	VO_BOOL ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize,VO_CODECBUFFER* pcodecbuf);
	virtual VO_U32 GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);

private:
	VO_PBYTE m_pHeadData;
	VO_U32 m_HeadSize;
	
	VO_S64 m_lastrelativetimecode;

    VO_BOOL m_bcontain_undetermined_keyframe;
	VO_U32  m_nExistThumnnailCnt;
	VO_BOOL m_bIsFirsttime;
#ifdef _RAW_DUMP_
	CDumper	m_Dumper;
#endif
};

#ifdef _VONAMESPACE
}
#endif
