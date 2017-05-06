#ifndef __CMp4Track_H__
#define __CMp4Track_H__

#include "CBaseTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4Reader;
class CMp4Track :
	public CBaseTrack
{
public:
	CMp4Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CMp4Reader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMp4Track();

public:
	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam);

protected:
	virtual VO_BOOL		Init(VO_PTR pUserData);

protected:
	CMp4Reader*			m_pReader;

	VO_PBYTE			m_pHeadData;
	VO_U32				m_dwHeadData;

	VO_U32				m_dwMaxSampleSize;

	VO_U32				m_dwCurrIndex;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4Track_H__